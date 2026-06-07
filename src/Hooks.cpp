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

				if (a_boundObject->GetFormType() == RE::ENUM_FORM_ID::kWEAP)
				{
					RE::TESObjectWEAP* tempREFR = static_cast<RE::TESObjectWEAP*>(a_boundObject);
					if (tempREFR->weaponData.type == RE::WEAPON_TYPE::kGrenade || tempREFR->weaponData.type == RE::WEAPON_TYPE::kMine)
					{
						REX::DEBUG("BGSInventoryList::AddItem: REFR grenade/mine weapon type.");
						break;
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

					if (tempREFR->HasKeyword(Shared::noDegradation))
					{
						REX::DEBUG("'CAS_NoDegradation' keyword found on armor: {}.", tempREFR->GetFormEditorID());
						break;
					}
				}

				// GetHealthPerc returns -1.0 if it can't find the 'kHealth' type.
				if (traverse->extra->GetHealthPerc() < 0.0f) {
					// Set health randomly
					traverse->extra->SetHealthPerc(RE::BSRandom::Float(0.45f, 0.85f));
					break;
				}

				iterCount++;
				if (iterCount > traverse->count - 1)
					break;

			}

			AddItem_Original(a_this, a_boundObject, a_stack, a_oldCount, a_newCount);
		}

		DetourXS hook_CombatFormulasCalcWeaponDamage;
		typedef float(CombatFormulasCalcWeaponDamageSig)(const RE::TESForm*, const RE::TESObjectWEAP::InstanceData*, const RE::TESAmmo*, float, float);
		REL::Relocation<CombatFormulasCalcWeaponDamageSig> CombatFormulasCalcWeaponDamageOriginal;

		float Hook_CombatFormulasCalcWeaponDamage(const RE::TESForm* a_actorForm, const RE::TESObjectWEAP::InstanceData* a_weapon, const RE::TESAmmo* a_ammo, float a_condition, float a_damageMultiplier)
		{
			float retailDamage = CombatFormulasCalcWeaponDamageOriginal(a_actorForm, a_weapon, a_ammo, a_condition, a_damageMultiplier);
			if (a_condition != -1.0f || a_condition < 0.75f)
			{
				retailDamage = retailDamage * (0.5f + min((0.5f * a_condition) / 0.75f, 0.5f));
			}
			return retailDamage;
		}
		
		
		namespace Registers {
			void RegisterAllHooks() {
				RegisterDetourFunction(hook_CombatFormulasCalcWeaponDamage, RE::ID::CombatFormulas::CalcWeaponDamage, &Hook_CombatFormulasCalcWeaponDamage, CombatFormulasCalcWeaponDamageOriginal, "CalcWeaponDamage");
				RegisterDetourFunction(hook_AddItem, RE::ID::BGSInventoryList::AddItem1, &Hook_BGSInventoryListAddItem, AddItem_Original, "AddItem");
			}

			void Install()
			{
				auto& trampoline = REL::GetTrampoline();


			}
		}
	}
}

