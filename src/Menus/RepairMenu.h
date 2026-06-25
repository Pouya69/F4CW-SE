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
#include <RE/A/Actor.h>
#include <RE/B/BGSListForm.h>
#include <RE/T/TESObjectMISC.h>
#include <Scaleform/G/GFx_Movie.h>
#include "../Shared.h"
#include <stdint.h>
#include "../Scaleform_F4CW.h"

// extern F4SEInputHandler g_repairMenuInputHandler;

namespace F4CW_Menus {
	namespace RepairMenu {
		struct RepairMenuEntry
		{
			std::string		text;
			std::uint32_t	handleID;
			std::uint32_t	stackID;
			float			condition;
			float			newCondition;
			float			damage;
			float			newDamage;
			bool			equipped;
			bool			repairable;
			bool			canAfford;
			int				repairCost;
			const char* skillName;
			bool			hasDR;
		};

		struct PipboyRepairMenuEntry
		{
			std::string		text;
			std::uint32_t	handleID;
			std::uint32_t	stackID;
			float			condition;
			float			otherCondition;
			float			newCondition;
			float			damage;
			float			otherDamage;
			float			newDamage;
			bool			equipped;
			bool			repairable;
			bool			canAfford;
			int				repairCost;
			const char* skillName;
			bool			hasDR;
		};

		struct InvObject
		{
			bool			favorite;
			bool			taggedForSearch;
			bool			isLegendary;
			std::uint32_t	count;
			std::uint32_t	nodeID;
			std::uint32_t	filterFlag;
			std::uint32_t	equipState;
			std::string		text;
			std::uint32_t	clipIndex;
			int				formID;
			bool			canFavorite;
			int				selectedIndex;
			bool			hasDR;
		};

		struct RepairObject
		{
			RE::TESForm * form;
			RE::ExtraDataList* extraData;
			int				index;
			bool			equipped;
		};

		enum RepairMenuTypes
		{
			kRepairType_Unknown = -1,
			kRepairType_Weapons,
			kRepairType_Apparel,
			kRepairType_Vendor,
		};

		namespace RepairMenuFunctions
		{
			void HandleMenuOpen(Scaleform::GFx::ASMovieRootBase* myMovieRoot, std::uint32_t playerCapsOverride = -1);
			void OpenRepairFromPipboy(std::uint32_t menuType, InvObject invObject);
			void RegisterForInput(bool bRegister);
			void HandleMenuClose();

			void GiveCapsToVendor(int caps);
			bool RepairItem(F4CW::ItemDegradation::WeaponConditionData myWeaponData, float newCondition);
			bool RepairItem(F4CW::ItemDegradation::ArmorConditionData myArmorData, float newCondition);
			bool RepairItemPipboy(const RE::BGSInventoryItem* itemToRepairWith, float newCondition);
			InvObject CreateInvObjectFromArray(Scaleform::GFx::Value array);
			bool CheckInventoryForEligibleRepair(int inventoryIndex);

			void GetINIOptions();

			bool CheckWorkshopTab();

		}

		void ProcessUserEvent(const char* controlName, bool isDown, int deviceType, std::uint32_t keyCode);

		void ShowNotification(std::string message);

		static bool IsFormInList(RE::TESForm* form, RE::BGSListForm* list);

		void OpenRepairMenu();
		void CloseRepairMenu();

		bool DefineRepairMenuFormsFromGame();

		bool RegisterRepairMenuFunctions(RE::BSScript::IVirtualMachine* vm);

		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value);

		class CWRepairMenu :
			public RE::GameMenuBase
		{
		public:

			const char* sMenuName = "CWRepairMenu";

			void ProcessUserEventExternal(const char* controlName, bool isDown, std::uint32_t deviceType, RE::BS_BUTTON_CODE buttonCode)
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->GetMenuOpen(sMenuName))
				{
					RE::IMenu* menu = ui->GetMenu(sMenuName).get();
					Scaleform::GFx::ASMovieRootBase* movieRoot = menu->uiMovie->asMovieRoot.get();
					Scaleform::GFx::Value arguments[4];
					arguments[0] = controlName;
					arguments[1] = true;
					arguments[2] = deviceType;
					arguments[3] = (std::int32_t) buttonCode;

					// REX::DEBUG("RepairMenu: USER INPUT TRIGGERED!");

					// ProcessUserEvent(controlName, isDown, deviceType, (std::int32_t)buttonCode);
					REX::DEBUG(std::format("repair menu - controlname: '{}', isDown: '{}', deviceType: '{}', buttonCode: '{}'", controlName, isDown ? 1 : 0, deviceType, (std::int32_t)buttonCode).c_str());
					
					if (!movieRoot->Invoke("root.Menu_mc.ProcessUserEventExternal", nullptr, arguments, 4)) {
						REX::DEBUG("RepairMenu - Could not call 'root.Menu_mc.ProcessUserEventExternal'");
					}
				}
			}

			void OnButtonEvent(const RE::ButtonEvent* a_event) override
			{
				ProcessUserEventExternal(a_event->QUserEvent().c_str(), a_event->QHeldDown(), a_event->device.underlying(), a_event->GetBSButtonCode());
			}

			CWRepairMenu();

			static IMenu* Create(const RE::UIMessage&)
			{
				return new CWRepairMenu();
			}
		};


		void CreateRepairMenu();
	}
}

extern bool bHideWorkshopTab;

extern RE::Actor* myCurrentVendor;
extern int myVendorSkill;
extern int myPlayerCaps;

extern RE::TESObjectMISC* caps;

extern float fRepairMax;
extern float fRepairMin;
extern float fRepairScavengeMult;

extern int iCurrentRepairMenuType;
extern  F4CW_Menus::RepairMenu::InvObject currentPipboyRepairObject;