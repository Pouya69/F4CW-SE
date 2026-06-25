#pragma once

#include <Scaleform/Scaleform.h>
#include "../ItemDegradation.h"
#include <cstdint>
#include <string>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/E/ExtraDataList.h>
#include <RE/T/TESForm.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_Value.h>
#include <RE/E/ENUM_FORM_ID.h>
 

// extern F4SEInputHandler g_cwPipboyMenuInputHandler;

namespace F4CW_Menus {
	namespace PipboyTabs {
		void ProcessUserEvent(const char* controlName, bool isDown, int deviceType, std::uint32_t keyCode);

		enum EActionToDoAfterPiboyBroughtDown {
			NONE = 0,
			OPEN_REPAIR_MENU
		};

		extern EActionToDoAfterPiboyBroughtDown action_to_to_after_closePipboy;

		void CloseMenu(EActionToDoAfterPiboyBroughtDown action = EActionToDoAfterPiboyBroughtDown::NONE);

		void RegisterForInput(bool bRegister);

		void UpdateMenus(Scaleform::GFx::ASMovieRootBase* myMovieRoot);

		void UpdateKarmaMenu(Scaleform::GFx::ASMovieRootBase* myMovieRoot);
		void UpdateSkillMenu(Scaleform::GFx::ASMovieRootBase* myMovieRoot);

		void PopulateSkillEntry(Scaleform::GFx::Value* myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot, int index, int filter, std::vector<std::string> stringValue);
		void PopulateKarmaEntry(Scaleform::GFx::Value* myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot);

		void UpdateItemCardsOnSection(std::uint32_t section);
		void RepopulateItemCardsOnSection(RE::ENUM_FORM_ID formType);

		enum KarmaTypes
		{
			Karma_VeryEvil = 0,
			Karma_Evil,
			Karma_Neutral,
			Karma_Good,
			Karma_VeryGood
		};

		enum PageTypes
		{
			PipboyPage_Skills = 0,
			PipboyPage_Karma
		};


		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value);

	}
}
