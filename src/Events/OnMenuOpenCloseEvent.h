#pragma once
#include <RE/B/BSTEvent.h>
#include <RE/M/MenuOpenCloseEvent.h>
#include <RE/P/PlayerCharacter.h>
#include "../ItemDegradation.h"
#include <RE/B/BSFixedString.h>

class OnMenuOpenCloseEvent : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
		if (!a_event.opening)
			return RE::BSEventNotifyControl::kContinue;


		if (a_event.menuName != RE::BSFixedString("HUDMenu"))
			return RE::BSEventNotifyControl::kContinue;



		// REX::DEBUG("HUD Opening...");

		auto player = RE::PlayerCharacter::GetSingleton();
		if (!player)
			return RE::BSEventNotifyControl::kContinue;

		// F4CW::WPNUtilities::UpdateHUDCondition());

		return RE::BSEventNotifyControl::kContinue;
	}
};

namespace F4CWEvents {
	static void RegisterOnMenuOpenCloseEvent() {
		
		OnMenuOpenCloseEvent* onMenuOpenCloseEvent = new OnMenuOpenCloseEvent();
		RE::UI::GetSingleton()->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(onMenuOpenCloseEvent);
		REX::DEBUG("Registered 'OnMenuOpenCloseEvent' sink.");
		
	}
}
