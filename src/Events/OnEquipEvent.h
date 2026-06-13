#pragma once
#include <RE/B/BSTEvent.h>
#include <RE/T/TESEquipEvent.h>
#include <RE/P/PlayerCharacter.h>
#include "../ItemDegradation.h"
#include <RE/U/UI.h>
#include <REX/LOG.h>
#include <F4SE/Interfaces.h>
#include <F4SE/API.h>
#include <RE/A/ActorEquipManager.h>
#include <RE/A/ActorEquipManagerEvent.h>
#include <REL/ID.h>
#include <REL/Relocation.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESObjectREFR.h>


// Using our custom addresses. UPDATE-RELATED
class EquipEventSource : public RE::BSTEventSource<RE::TESEquipEvent>
{
public:
	[[nodiscard]] static EquipEventSource* GetSingleton()
	{
		REL::Relocation<EquipEventSource*> singleton{ REL::ID(4798533) };
		return singleton.get();
	}
};

class OnEquipEvent : public RE::BSTEventSink<RE::TESEquipEvent> {
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent& a_event, RE::BSTEventSource<RE::TESEquipEvent>*) override {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (a_event.actor.get() != player)
			return RE::BSEventNotifyControl::kContinue;

		if (!a_event.equipped)
			return RE::BSEventNotifyControl::kContinue;

		REX::DEBUG("Equip Event!");

		auto weapon = RE::TESForm::GetFormByID(a_event.baseObject);

		RE::TESObjectWEAP* weaponObject;
		RE::ExtraDataList* extraData;

		for (RE::BGSInventoryItem& inventoryItem : player->inventoryList->data) {
			if (!inventoryItem.IsEquipped(0))
				continue;
			
			auto weaponObj = static_cast<RE::TESObjectWEAP*>(inventoryItem.object);

			if (weaponObj) {
				weaponObject = weaponObj;
				extraData = inventoryItem.stackData->extra.get();
				break;
			}

		}

		if (!weaponObject || !extraData)
			return RE::BSEventNotifyControl::kContinue;


		

		RE::BSFixedString menuString("HUDMenu");
		if (RE::UI::GetSingleton()->GetMenuOpen(menuString)) {

			auto myHudMenu = RE::UI::GetSingleton()->GetMenu(menuString).get();
			Scaleform::GFx::Value myConditionValue[1];

			float conditionValue = extraData->GetHealthPerc();

			myConditionValue[0] = Scaleform::GFx::Value(conditionValue);

			const bool result = myHudMenu->uiMovie->asMovieRoot->Invoke("root.CWHUD_loader.content.SetCondition", nullptr, myConditionValue, 1);

		}

		
		return RE::BSEventNotifyControl::kContinue;
	}
};

namespace F4CWEvents {
	static void RegisterOnEquipEvent() {
		auto onEquipEvent = new OnEquipEvent();
		EquipEventSource::GetSingleton()->RegisterSink(onEquipEvent);
		REX::DEBUG("Registered 'OnEquipEvent' sink.");
	}
}
