#pragma once

#include "../Shared.h"
#include "../GameForms.h"
#include <Windows.h>
#include <format>
#include <RE/A/Actor.h>
#include <RE/A/ActorEquipManager.h>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BSCoreTypes.h>
#include <RE/B/BSFixedString.h>
#include <RE/B/BSRandom.h>
#include <RE/B/BSTEvent.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <RE/E/EquippedItem.h>
#include <RE/E/ExtraDataList.h>
#include <RE/P/PipboyDataManager.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/S/SendHUDMessage.h>
#include <RE/S/Setting.h>
#include <RE/T/TESHitEvent.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/U/UI.h>
#include <RE/W/WEAPON_TYPE.h>
#include <Scaleform/G/GFx_Value.h>
#include <REX/LOG.h>

class OnHitEvent : public RE::BSTEventSink<RE::TESHitEvent> {
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent& a_event, RE::BSTEventSource<RE::TESHitEvent>*) override {
		auto player = RE::PlayerCharacter::GetSingleton();
		
		RE::Actor* TargetActor = (RE::Actor*) a_event.target.get();

		if (TargetActor == player)
			return RE::BSEventNotifyControl::kContinue;

		/*
		if (!Shared::noArmorDegradation) {
			ModifyArmourCondition(a_event, player, TargetActor);
		}
		*/
		RE::Actor* CauseActor = (RE::Actor*)a_event.cause.get();
		if (!a_event.cause.get() || a_event.cause->GetFormType() != RE::ENUM_FORM_ID::kACHR || !CauseActor || CauseActor != player) {
			// Only player can affect it.
			return RE::BSEventNotifyControl::kContinue;
		}

		if (!Shared::noWeaponDegradation) {
			RE::TESObjectREFR* sourceREFR = a_event.hitData.sourceRef.get().get();
			if (!sourceREFR) {
				// If sourceREFR is 'nullptr' it's a melee attack.
				ModifyMeleeWeaponOnHit(a_event, player);
			}
			
		}

		return RE::BSEventNotifyControl::kContinue;
	}
	/*
	void ModifyArmourCondition(const RE::TESHitEvent& a_event, RE::PlayerCharacter* player, RE::Actor* NPCVictim) {
		
		// a_event.hitData.damageLimb
	}
	*/
	void ModifyMeleeWeaponOnHit(const RE::TESHitEvent& a_event, RE::PlayerCharacter* player) {
		if (!a_event.hitData.weapon.object)
			return;

		RE::TESFormID weaponFormID = a_event.hitData.weapon.object->GetFormID();
		RE::BGSInventoryItem* inventoryItem = nullptr;

		for (RE::BGSInventoryItem& item : player->inventoryList->data)
		{
			if (item.object->GetFormID() == weaponFormID)
			{
				inventoryItem = &item;
				break;
			}
		}

		if (!inventoryItem)
			return;

		RE::EquippedItem& equippedWeapon = player->currentProcess->middleHigh->equippedItems[0];
		RE::ExtraDataList* extraDataList = inventoryItem->stackData->extra.get();

		float currentHealth = extraDataList->GetHealthPerc();

		RE::TESObjectWEAP* weapon = (RE::TESObjectWEAP*)a_event.hitData.weapon.object;

		float newHealth;
		float conditionReduction = 0.015f; // 1.5% degradation when weapon bashing, melee weapons use random value.

		// Melee weapon
		if (weapon->weaponData.type != RE::WEAPON_TYPE::kGun)
		{
			conditionReduction = RE::BSRandom::Float(0.005, 0.015);
		}

		// Reduces damage to weapon depending on players 'Melee weapons' level.
							// Linear reduction from 0 - 100, 100 resulting in 20% less damage to weapon.
		float meleeWeaponsSkillValue = player->GetActorValue(*CW_Skills.MeleeWeapons);
		float reductionPercentFromSkill = (meleeWeaponsSkillValue / 100.0f) * 0.2;

		conditionReduction *= (1.0f - reductionPercentFromSkill);

		newHealth = max(currentHealth - conditionReduction, 0.0f);

		if (newHealth == 0.0f)
		{
			RE::ActorEquipManager::GetSingleton()->UnequipItem(player, &equippedWeapon, false);
			RE::GameSettingCollection* gameSettingCollection = RE::GameSettingCollection::GetSingleton();
			RE::SendHUDMessage::ShowHUDMessage(gameSettingCollection->GetSetting("sWeaponBreak")->GetString().data(), "00DCUIWeaponBreak", true, true);
		}
		extraDataList->SetHealthPerc(newHealth);

		
		REX::DEBUG(std::format("Hit! {}", newHealth).c_str());
		RE::BSFixedString menuString("HUDMenu");
		if (RE::UI::GetSingleton()->GetMenuOpen(menuString)) {

			// REX::DEBUG("Hit! Adjusting hud conditions.");

			auto myHudMenu = RE::UI::GetSingleton()->GetMenu(menuString).get();
			Scaleform::GFx::Value myConditionValue[1];

			myConditionValue[0] = Scaleform::GFx::Value(newHealth);

			const bool result = myHudMenu->uiMovie->asMovieRoot->Invoke("root.CWHUD_loader.content.SetCondition", nullptr, myConditionValue, 1);
			LOG_TO_CONSOLE(result ? "Hud conditions set for melee\n" : "Hud didn't work for melee.\n");

		}

		RE::PipboyDataManager::GetSingleton()->inventoryData.RepopulateItemCardOnSection(RE::ENUM_FORM_ID::kWEAP);
	}

};


namespace F4CWEvents {
	static void RegisterOnHitEvent() {
		auto onHitEvent = new OnHitEvent();
		RE::TESHitEvent::GetEventSource()->RegisterSink(onHitEvent);
		// EquipEventSource::GetSingleton()->RegisterSink(onEquipEvent);
		REX::DEBUG("Registered 'OnHitEvent' sink.");
	}
}
