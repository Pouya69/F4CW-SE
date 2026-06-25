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
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BSFixedString.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <RE/E/ExtraDataList.h>
#include <RE/T/TESObjectWEAP.h>
#include <Scaleform/G/GFx_Value.h>


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

		

		auto weapon = RE::TESForm::GetFormByID(a_event.baseObject);

		RE::TESObjectWEAP* weaponObject = nullptr;
		RE::ExtraDataList* extraData = nullptr;

		for (RE::BGSInventoryItem& inventoryItem : player->inventoryList->data) {
			if (!inventoryItem.IsEquipped(0))
				continue;
			if (!inventoryItem.object || inventoryItem.object->GetFormType() != RE::ENUM_FORM_ID::kWEAP)
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
			REX::DEBUG(std::format("Equip Event! Adjusting hud conditions: {}", conditionValue).c_str());

			myConditionValue[0] = Scaleform::GFx::Value(conditionValue);

			const bool result = myHudMenu->uiMovie->asMovieRoot->Invoke("root.CWHUD_loader.content.SetCondition", nullptr, myConditionValue, 1);

		}

		
		return RE::BSEventNotifyControl::kContinue;
	}
};

namespace F4CWEvents {
	static void RegisterOnEquipEvent() {
		auto onEquipEvent = new OnEquipEvent();
		// RE::TESEquipEvent::GetEventSource()->RegisterSink(onEquipEvent);
		EquipEventSource::GetSingleton()->RegisterSink(onEquipEvent);
		REX::DEBUG("Registered 'OnEquipEvent' sink.");
	}
}
