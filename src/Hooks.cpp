#include "Hooks.h"
#include <REL/Trampoline.h>
#include "detourXS/detourxs.h"
#include <REL/Relocation.h>
#include <RE/IDs.h>
#include <cstdint>
#include <cstdarg>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BGSInventoryList.h>
#include <RE/T/TESBoundObject.h>
#include <RE/B/BSRandom.h>
#include <RE/E/ENUM_FORM_ID.h>
#include "Shared.h"
#include <Windows.h>
#include <RE/T/TESAmmo.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/W/WEAPON_TYPE.h>
#include <REX/LOG.h>
#include <RE/A/Actor.h>
#include <RE/A/ActorValueInfo.h>
#include <RE/E/ExtraDataList.h>
#include <RE/E/EXTRA_DATA_TYPE.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/A/AlchemyItem.h>
#include <RE/B/BGSEquipIndex.h>
#include <RE/B/BGSObjectInstance.h>
#include <RE/B/BSCoreTypes.h>
#include <RE/T/TESObjectREFR.h>
#include "ItemDegradation.h"
#include <RE/E/EquippedItem.h>
#include <RE/P/PipboyDataManager.h>
#include <RE/W/WEAPON_FLAGS.h>
#include <RE/A/ActorEquipManager.h>
#include <RE/S/SendHUDMessage.h>
#include <RE/S/Setting.h>
#include <RE/B/BGSBodyPart.h>
#include <RE/B/BGSTypedFormValuePair.h>
#include <RE/B/BSTArray.h>
#include <RE/B/BSTTuple.h>
#include <RE/A/ActorUtils.h>
#include <RE/E/ExtraHealth.h>

namespace F4CW {
	namespace Hooks {


		DetourXS hook_AddItem;
		typedef void(AddItemSig)(RE::BGSInventoryList* a_this, RE::TESBoundObject* a_boundObject, const RE::BGSInventoryItem::Stack* a_stack, std::uint32_t* a_oldCount, std::uint32_t* a_newCount);
		REL::Relocation<AddItemSig> AddItem_Original;
		void Hook_BGSInventoryListAddItem(RE::BGSInventoryList* a_this, RE::TESBoundObject* a_boundObject, const RE::BGSInventoryItem::Stack* a_stack, std::uint32_t* a_oldCount, std::uint32_t* a_newCount) {
			if (!a_boundObject || (a_boundObject->GetFormType() != RE::ENUM_FORM_ID::kWEAP && a_boundObject->GetFormType() != RE::ENUM_FORM_ID::kARMO)) {
				AddItem_Original(a_this, a_boundObject, a_stack, a_oldCount, a_newCount);
				return;
			}

			std::uint32_t iterCount = 0;
			for (const RE::BGSInventoryItem::Stack* traverse = a_stack; traverse; traverse->nextStack)
			{
				if (!traverse || !traverse->extra)
					break;
				bool willHaveHealth = false;
				if (a_boundObject->GetFormType() == RE::ENUM_FORM_ID::kWEAP)
				{
					RE::TESObjectWEAP* tempREFR = static_cast<RE::TESObjectWEAP*>(a_boundObject);
					if (tempREFR->weaponData.type == RE::WEAPON_TYPE::kGrenade || tempREFR->weaponData.type == RE::WEAPON_TYPE::kMine)
					{
						REX::DEBUG("BGSInventoryList::AddItem: REFR grenade/mine weapon type.");
						break;
					}
					willHaveHealth = true;
					
					if (traverse->extra->GetHealthPerc() < 0.0f) {
						LOG_TO_CONSOLE("Added weapon first time.\n");
						const float cond = RE::BSRandom::Float(0.45f, 0.85f);
						traverse->extra->SetHealthPerc(cond);

						WPNUtilities::CalculateUpdatedRateOfFireValue(tempREFR, &tempREFR->weaponData, cond);
					}
					

					// Set to '1.0' when initializing if the 'noDegradation' keyword is on the object.
					if (tempREFR->HasKeyword(Shared::noDegradation))
					{
						REX::DEBUG("'DC_NoDegradation' keyword found on weapon: {}.", tempREFR->GetFormEditorID());
						break;
					}
				}

				if (a_boundObject->GetFormType() == RE::ENUM_FORM_ID::kARMO)
				{
					RE::TESObjectARMO* tempREFR = static_cast<RE::TESObjectARMO*>(a_boundObject);
					// Set to '1.0' when initializing if the 'noDegradation' keyword is on the object.
					
					if (tempREFR->armorData.rating == 0 && !tempREFR->armorData.damageTypes)
					{
						break;
					}
					willHaveHealth = true;

					if (tempREFR->HasKeyword(Shared::noDegradation))
					{
						REX::DEBUG("'CAS_NoDegradation' keyword found on armor: {}.", tempREFR->GetFormEditorID());
						break;
					}
				}

				// GetHealthPerc returns -1.0 if it can't find the 'kHealth' type.
				if (willHaveHealth && traverse->extra->GetHealthPerc() < 0.0f) {
					// Set health randomly
					LOG_TO_CONSOLE(std::format("Added health to item '{}' added.", a_boundObject->GetFormEditorID()).c_str());
					traverse->extra->SetHealthPerc(RE::BSRandom::Float(0.45f, 0.85f));
					break;
				}
				
				
				iterCount++;
				if (iterCount > traverse->count - 1)
					break;

			}

			AddItem_Original(a_this, a_boundObject, a_stack, a_oldCount, a_newCount);
		}

		DetourXS hook_GetEquippedDamageResistance;
		typedef float(GetEquippedDamageResistanceSig)(RE::Actor*, const RE::ActorValueInfo*);
		REL::Relocation<GetEquippedDamageResistanceSig> GetEquippedDamageResistanceOriginal;
		float Hook_GetEquippedDamageResistance(RE::Actor* a_actor, const RE::ActorValueInfo* a_info)
		{
			float retailResistance = GetEquippedDamageResistanceOriginal(a_actor, a_info);
			// Only calculated for the player.
			if (a_actor != RE::PlayerCharacter::GetSingleton())
				return retailResistance;

			RE::BGSInventoryList* inventoryList = a_actor->inventoryList;
			inventoryList->rwLock.lock_read();

			for (RE::BGSInventoryItem& inventoryItem : inventoryList->data) {
				RE::TESObjectARMO* armor = static_cast<RE::TESObjectARMO*>(inventoryItem.object);
				if (inventoryItem.object && armor->formType == RE::ENUM_FORM_ID::kARMO && armor->Protects(a_info, false)) {
					if (!inventoryItem.IsEquipped(0))
						continue;

					RE::ExtraDataList* dataList = inventoryItem.stackData->extra.get();
					if (!dataList->HasType(RE::EXTRA_DATA_TYPE::kHealth))
						continue;

					float currentHealth = dataList->GetHealthPerc();
					if (currentHealth <= 0.0f) {
						// Fully broken. No damage resistance.
						return 0.0f;
					}

					REX::DEBUG("Damage resistance prior to condition modifier: {}", retailResistance);
					const float conditionModifier = 0.66f + min(0.34 * currentHealth / 0.5f, 0.34f);
					retailResistance *= conditionModifier;
					REX::DEBUG("Damage resistance after condition modifier: {}", retailResistance);
				}
			}

			inventoryList->rwLock.unlock_read();
			return retailResistance;
		}

		DetourXS hook_CombatFormulasCalcWeaponDamage;
		typedef float(CombatFormulasCalcWeaponDamageSig)(const RE::TESForm*, const RE::TESObjectWEAP::InstanceData*, const RE::TESAmmo*, float, float);
		REL::Relocation<CombatFormulasCalcWeaponDamageSig> CombatFormulasCalcWeaponDamageOriginal;
		float Hook_CombatFormulasCalcWeaponDamage(const RE::TESForm* a_actorForm, const RE::TESObjectWEAP::InstanceData* a_weapon, const RE::TESAmmo* a_ammo, float a_condition, float a_damageMultiplier)
		{
			float retailDamage = CombatFormulasCalcWeaponDamageOriginal(a_actorForm, a_weapon, a_ammo, a_condition, a_damageMultiplier);

			if (a_condition != -1.0f || a_condition < 0.75f)
				retailDamage = retailDamage * (0.5f + min((0.5f * a_condition) / 0.75f, 0.5f));

			return retailDamage;
		}

		DetourXS hook_TESObjectWeaponFire;
		typedef void(TESObjectWeaponFireSig)(const RE::BGSObjectInstanceT<RE::TESObjectWEAP>*, RE::TESObjectREFR*, RE::BGSEquipIndex, RE::TESAmmo*, RE::AlchemyItem*);
		REL::Relocation<TESObjectWeaponFireSig> TESObjectWeaponFireOriginal;
		void Hook_TESObjectWeaponFire(const RE::BGSObjectInstanceT<RE::TESObjectWEAP>* a_weapon, RE::TESObjectREFR* a_source, RE::BGSEquipIndex a_equipIndex, RE::TESAmmo* a_ammo, RE::AlchemyItem* a_poiso) {

			if (!a_source) {
				TESObjectWeaponFireOriginal(a_weapon, a_source, a_equipIndex, a_ammo, a_poiso);
				return;
			}
				

			// Only apply Weapon Condition Change on Player.
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			if (a_source != playerCharacter) {
				TESObjectWeaponFireOriginal(a_weapon, a_source, a_equipIndex, a_ammo, a_poiso);
				return;
			}

			if (playerCharacter->IsGodMode() || Shared::noWeaponDegradation) {
				TESObjectWeaponFireOriginal(a_weapon, a_source, a_equipIndex, a_ammo, a_poiso);
				return;
			}

			

			// LOG_TO_CONSOLE("Shot weapon.\n");

			//if (weaponObject)
				//weaponObject->weaponData.attackDelaySec = WPNUtilities::CalculateUpdatedRateOfFireValue(weaponObject, a_weapon->instanceData);

			RE::TESObjectWEAP* weaponObject = static_cast<RE::TESObjectWEAP*>(a_weapon->object);
			

			if (weaponObject->weaponData.type == RE::WEAPON_TYPE::kGrenade || weaponObject->weaponData.type == RE::WEAPON_TYPE::kMine
				|| weaponObject->HasKeyword(Shared::noDegradation))
				return;

			// Getting the inventory form of the a_weapon
			RE::BGSInventoryItem* inventoryItem = nullptr;
			int index = 0;
			RE::TESFormID weaponFormID = a_weapon->object->GetFormID();
			for (RE::BGSInventoryItem& item : playerCharacter->inventoryList->data) {
				if (item.object->GetFormID() == weaponFormID) {
					inventoryItem = &item;
					break;
				}
				index++;
			}
			if (!inventoryItem) {
				TESObjectWeaponFireOriginal(a_weapon, a_source, a_equipIndex, a_ammo, a_poiso);
				return;
			}
				
			RE::TESObjectWEAP::InstanceData* weaponInstanceData = static_cast<RE::TESObjectWEAP::InstanceData*>(a_weapon->instanceData.get());
			
			RE::EquippedItem& equippedItem = playerCharacter->currentProcess->middleHigh->equippedItems[0];

			RE::TESAmmo* weaponAmmo = weaponInstanceData->ammo;

			double degradationAmount = ItemDegradation::GetDegradationMapping(weaponAmmo);
			if (degradationAmount == 0.01f)
				REX::WARN("TESObjectWEAP::Fire - degradation ammo mapping for ammo type '{}' is not declared in 'AmmoDegradationMap' defaulting to 0.01f", weaponAmmo->GetFormEditorID());

			const std::uint32_t flags = weaponInstanceData->flags.underlying();

			if (flags & (std::uint32_t)RE::WEAPON_FLAGS::kAutomatic)
				degradationAmount *= Shared::fAutomaticWeaponConditionReduction->GetValue();
			else if (flags & (std::uint32_t)RE::WEAPON_FLAGS::kBoltAction)
				degradationAmount *= Shared::fBoltWeaponConditionReduction->GetValue();

			// Linear reduction from 0 - 100, 100 resulting in 20% less damage to weapon.
			// Mapped out. from 0 to 20%
			RE::ActorValueInfo* gunSkill = weaponInstanceData->skill;
			float playerGunSkill = 0.0f;
			if (gunSkill) {
				REX::DEBUG("Weapon '{}' set to skill '{}'", a_weapon->object->GetFormEditorID(), gunSkill->GetFormEditorID());
				playerGunSkill = playerCharacter->GetActorValue(*gunSkill);
			}
			else {
				REX::DEBUG("Weapon '{}' is missing skill value. Change this value in xEdit.", a_weapon->object->GetFormEditorID());
			}
			degradationAmount *= 1.0f - (playerGunSkill / 100.0f * 0.2f);

			RE::ExtraDataList* extraDataList = inventoryItem->stackData->extra.get();
			
			double currentWeaponHealth = max(extraDataList->GetHealthPerc() - degradationAmount, 0.0f);
			
			
			extraDataList->SetHealthPerc(currentWeaponHealth);

			WPNUtilities::CalculateUpdatedRateOfFireValue(weaponObject, weaponInstanceData, currentWeaponHealth);
			TESObjectWeaponFireOriginal(a_weapon, a_source, a_equipIndex, a_ammo, a_poiso);

			if (currentWeaponHealth == 0.0f) {
				// Weapon breaks.
				RE::ActorEquipManager::GetSingleton()->UnequipItem(playerCharacter, &equippedItem, false);
				RE::GameSettingCollection* gameSettingCollection = RE::GameSettingCollection::GetSingleton();
				RE::SendHUDMessage::ShowHUDMessage(gameSettingCollection->GetSetting("sWeaponBreak")->GetString().data(), "00DCUIWeaponBreak", true, true);
			}
			RE::PipboyDataManager::GetSingleton()->inventoryData.RepopulateItemCardOnSection(RE::ENUM_FORM_ID::kWEAP);
			if (weaponObject->weaponData.flags.underlying() & (std::uint32_t)RE::WEAPON_FLAGS::kAutomatic) {
				LOG_TO_CONSOLE(std::format("Shot weapon {}. Cond: {}, New FireRate: {}, {}\n", weaponObject->GetFormEditorID(), currentWeaponHealth, weaponInstanceData->attackSeconds, weaponInstanceData->attackDelaySec).c_str());
			}
			else {
				LOG_TO_CONSOLE(std::format("Shot weapon {}. Cond: {}, New FireRate: {}, {}\n", weaponObject->GetFormEditorID(), currentWeaponHealth, weaponInstanceData->attackDelaySec, weaponInstanceData->attackSeconds).c_str());
			}
				
		}

		DetourXS hook_CombatFormulasCalcTargetedLimbDamage;
		typedef float(CombatFormulasCalcTargetedLimbDamageSig)(RE::Actor*, const RE::BGSBodyPart*, float, RE::BSTArray<RE::BSTTuple<RE::TESForm*, RE::BGSTypedFormValuePair::SharedVal>, RE::BSTArrayHeapAllocator>*);
		REL::Relocation<CombatFormulasCalcTargetedLimbDamageSig> CombatFormulasCalcTargetedLimbDamageOriginal;
		float Hook_CombatFormulasCalcTargetedLimbDamage(RE::Actor* a_actor, const RE::BGSBodyPart* a_bodyPart, float a_physicalDamage, RE::BSTArray<RE::BSTTuple<RE::TESForm*, RE::BGSTypedFormValuePair::SharedVal>, RE::BSTArrayHeapAllocator>* a_damageTypes) {
			float retailDamage = CombatFormulasCalcTargetedLimbDamageOriginal(a_actor, a_bodyPart, a_physicalDamage, a_damageTypes);
			
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			if (a_actor == playerCharacter && (playerCharacter->IsGodMode() || Shared::noArmorDegradation))
				return retailDamage;

			RE::ActorValueInfo* targetedLimbAV = a_bodyPart->data.actorValue;
			const float DR_ForHitLimb = RE::ActorUtils::GetEquippedArmorDamageResistance(a_actor, targetedLimbAV);

			if (DR_ForHitLimb >= a_physicalDamage) {
				LOG_TO_CONSOLE(std::format("Armor not dmged. DR_ForHitLimb is more than damage. DR_ForHitLimb: {}, dmg: {}\n", DR_ForHitLimb, a_physicalDamage).c_str());
				return retailDamage;
			}

			RE::BGSInventoryList* inventoryList = a_actor->inventoryList;
			inventoryList->rwLock.lock_read();
			for (RE::BGSInventoryItem& inventoryItem : inventoryList->data) {
				if (!inventoryItem.IsEquipped(0))
					continue;

				RE::TESObjectARMO* armor = static_cast<RE::TESObjectARMO*>(inventoryItem.object);
				if (inventoryItem.object && armor->formType == RE::ENUM_FORM_ID::kARMO && armor->Protects(targetedLimbAV, false)) {
					RE::ExtraDataList* armorExtraData = inventoryItem.stackData->extra.get();
					// If armor does not have kHealth, it would return -1.
					double armourHealth = armorExtraData->GetHealthPerc();

					if (armourHealth > 0.0f) {
						REX::DEBUG("Armor condition of '{}' before degradation: {}", armor->GetFormEditorID(), armourHealth);
						const double degradationAmount = (a_physicalDamage - DR_ForHitLimb) / DR_ForHitLimb * Shared::fArmourConditionReductionPerPercentage->GetValue();
						armourHealth = max(armourHealth - degradationAmount, 0.0f);

						if (armourHealth == 0.0f) {
							// Armor breaks.
							RE::BGSObjectInstance* armorInstance = new RE::BGSObjectInstance(armor, &armor->armorData);
							inventoryList->rwLock.unlock_read();
							RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, armorInstance, 1, armor->equipSlot, 0, false, false, true, true, nullptr);
							inventoryList->rwLock.lock_read();
							RE::SendHUDMessage::ShowHUDMessage(a_actor == playerCharacter ? "$F4CW_ArmorBroken" : std::format("You broke {}'s armor!", a_actor->GetDisplayFullName()).c_str(), "00DCUIWeaponBreak", true, true);
						}

						armorExtraData->SetHealthPerc(armourHealth);

						LOG_TO_CONSOLE(std::format("Armor condition of '{}' after degradation: {}. degradationAmount: {}\n", armor->GetFormEditorID(), armourHealth, degradationAmount).c_str());
						REX::DEBUG("Armor condition of '{}' after degradation: {}. degradationAmount: {}", armor->GetFormEditorID(), armorExtraData->GetHealthPerc(), degradationAmount);
					}
					break;
				}
			}
			inventoryList->rwLock.unlock_read();
			return retailDamage;
		}


		DetourXS hook_SetHealthPerc;
		typedef void(SetHealthPercSig)(RE::ExtraDataList*, float);
		REL::Relocation<SetHealthPercSig> SetHealthPercOriginal;
		// Required to allow the game to store a value of 1.0, by default it would set it to '-1.0'
		// This causes issues in the way we handle initialization of the health data.
		// Here we simply, run the original function, then if 'a_health' was '1.0', we manually set it 
		// again to '1.0', reverting the original functions result of '-1.0'.
		void Hook_ExtraDataListSetHealthPerc(RE::ExtraDataList* a_this, float a_health)
		{
			SetHealthPercOriginal(a_this, a_health);
			if (a_health == 1.0f && a_this->GetHealthPerc() != 1.0f)
			{
				a_this->AddExtra(new RE::ExtraHealth(1.0f));
			}
		}

		DetourXS hook_GetWeaponDisplayRateOfFire;
		typedef float(GetWeaponDisplayRateOfFireSig)(const RE::TESObjectWEAP&, const RE::TESObjectWEAP::InstanceData*);
		REL::Relocation<GetWeaponDisplayRateOfFireSig> GetWeaponDisplayRateOfFireOriginal;
		float Hook_GetWeaponDisplayRateOfFire(const RE::TESObjectWEAP& a_weapon, const RE::TESObjectWEAP::InstanceData* a_data)
		{
			// float original = GetWeaponDisplayRateOfFireOriginal(a_weapon, a_data);

			// LOG_TO_CONSOLE(std::format("Original FireRate: {}, Attack Delay: {}", original, a_data->attackDelaySec).c_str());

			return a_data->attackDelaySec;
		}

		
		
		namespace Registers {
			void RegisterAllHooks() {
				RegisterDetourFunction(hook_CombatFormulasCalcWeaponDamage, RE::ID::CombatFormulas::CalcWeaponDamage, &Hook_CombatFormulasCalcWeaponDamage, CombatFormulasCalcWeaponDamageOriginal, "CalcWeaponDamage");
				RegisterDetourFunction(hook_AddItem, RE::ID::BGSInventoryList::AddItem1, &Hook_BGSInventoryListAddItem, AddItem_Original, "AddItem");
				RegisterDetourFunction(hook_GetEquippedDamageResistance, RE::ID::ActorUtils::GetEquippedArmorDamageResistance, &Hook_GetEquippedDamageResistance, GetEquippedDamageResistanceOriginal, "GetEquippedArmorDamageResistance");
				RegisterDetourFunction(hook_TESObjectWeaponFire, RE::ID::TESObjectWEAP::Fire, &Hook_TESObjectWeaponFire, TESObjectWeaponFireOriginal, "TESObjectWeaponFire");
				RegisterDetourFunction(hook_CombatFormulasCalcTargetedLimbDamage, RE::ID::CombatFormulas::CalcTargetedLimbDamage, &Hook_CombatFormulasCalcTargetedLimbDamage, CombatFormulasCalcTargetedLimbDamageOriginal, "CalcTargetedLimbDamage");
				RegisterDetourFunction(hook_SetHealthPerc, RE::ID::ExtraDataList::SetHealthPerc, &Hook_ExtraDataListSetHealthPerc, SetHealthPercOriginal, "SetHealthPerc");
				
				// RegisterDetourFunction(hook_GetWeaponDisplayRateOfFire, RE::ID::CombatFormulas::GetWeaponDisplayRateOfFire, &Hook_GetWeaponDisplayRateOfFire, GetWeaponDisplayRateOfFireOriginal, "GetWeaponDisplayRateOfFire");
			}

			void Install()
			{
				auto& trampoline = REL::GetTrampoline();


			}
		}
	}
}
