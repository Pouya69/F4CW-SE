#pragma once
#include "../Menus/LevelUpMenu.h"

class OnLevelUpEvent : public RE::BSTEventSink<RE::LevelIncrease::Event> {
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::LevelIncrease::Event& a_event, RE::BSTEventSource<RE::LevelIncrease::Event>*) override {
		
		F4CW_Menus::LevelUpMenu::HandleLevelUp();

		return RE::BSEventNotifyControl::kContinue;
	}
};

namespace F4CWEvents {
	static void RegisterOnLevelUpEvent() {
		OnLevelUpEvent* onLevelUpEvent = new OnLevelUpEvent();
		RE::LevelIncrease::GetEventSource()->RegisterSink(onLevelUpEvent);
		REX::DEBUG("Registered 'OnLevelUpEvent' sink.");
	}
}
