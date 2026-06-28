#include "Shared.h"
#include <RE/B/BGSKeyword.h>
#include "debugLog.h"
#include "IGlobalConfig.h"
#include <RE/T/TESDataHandler.h>
#include <RE/A/Actor.h>
#include <RE/B/BSExtraData.h>
#include <RE/E/ExtraInstanceData.h>
#include <RE/E/EXTRA_DATA_TYPE.h>
#include <RE/T/TBO_InstanceData.h>
#include <RE/T/TESNPC.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/T/TESRace.h>
#include <cstdint>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BSFixedString.h>
#include <RE/E/ExtraDataList.h>
#include <RE/E/ExtraTextDisplayData.h>
#include <RE/I/IMenu.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/S/SendHUDMessage.h>
#include <RE/S/Setting.h>
#include <RE/T/TESBoundObject.h>
#include <RE/T/TESGlobal.h>
#include <RE/U/UI.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_Value.h>
#include <Scaleform/P/Ptr.h>
#include <REX/LOG.h>

namespace Shared {
	RE::BGSKeyword* noDegradation;
	bool noArmorDegradation;
	bool noWeaponDegradation;

	RE::TESGlobal* fAutomaticWeaponConditionReduction;
	RE::TESGlobal* fBoltWeaponConditionReduction;

	RE::TESGlobal* fArmourConditionReductionPerPercentage;


	RE::BGSKeyword* crWeaponRanged;


	void InitializeSharedForms(RE::TESDataHandler* dataHandler)
	{
		crWeaponRanged = dataHandler->LookupForm<RE::BGSKeyword>(0x189348, "Fallout4.esm");
		noDegradation = dataHandler->LookupForm<RE::BGSKeyword>(0x0DEDC5, MOD_ESM);

		fAutomaticWeaponConditionReduction = dataHandler->LookupForm<RE::TESGlobal>(0x0E0A1F, MOD_ESM);
		fBoltWeaponConditionReduction = dataHandler->LookupForm<RE::TESGlobal>(0x0E0A20, MOD_ESM);

		fArmourConditionReductionPerPercentage = dataHandler->LookupForm<RE::TESGlobal>(0x0E0A21, MOD_ESM);
	}

	bool IsXPMetervisible()
	{
		RE::BSFixedString menuString("HUDMenu");
		RE::IMenu* menu = RE::UI::GetSingleton()->GetMenu(menuString).get();
		Scaleform::GFx::Value openValue;

		// menu->uiMovie->asMovieRoot->GetVariable(&openValue, "root.XPMeter_mc.visible");
		if (!menu->uiMovie->asMovieRoot->GetVariable(&openValue, "root.HUDNotificationsGroup_mc.XPMeter_mc.visible")) {
			REX::DEBUG("root.HUDNotificationsGroup_mc.XPMeter_mc.visible did not return a value."sv);
		}
		
		return openValue.GetBoolean();
	}

	bool InMenuMode()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		return (
			(ui->menuMode >= 1)
			|| ui->GetMenuOpen("CookingMenu")
			|| ui->GetMenuOpen("FaderMenu")
			|| ui->GetMenuOpen("FavoritesMenu")
			|| ui->GetMenuOpen("PowerArmorModMenu")
			|| ui->GetMenuOpen("RobotModMenu")
			|| ui->GetMenuOpen("VATSMenu")
			|| ui->GetMenuOpen("WorkshopMenu")
			|| ui->GetMenuOpen("DialogueMenu")
			);
	}

	float ConvertPercentageToFloat(std::uint8_t percentage)
	{
		return (percentage / static_cast<float>(100));
	}

	const char* GetItemDisplayName(RE::ExtraDataList* myExtraData, RE::TESBoundObject* baseForm)
	{
		RE::BSExtraData* extraData = myExtraData->GetByType(RE::EXTRA_DATA_TYPE::kTextDisplayData);
		RE::ExtraTextDisplayData* displayText = static_cast<RE::ExtraTextDisplayData*>(extraData);
		if (displayText)
		{
			return displayText->GetDisplayName(baseForm).c_str();
		}
		else
		{
			return "";
		}
	}

	void RemovePipboyInventoryItem(const RE::BGSInventoryItem* item, bool bSilent)
	{
		if (!bSilent)
		{
			std::string itemName = GetItemDisplayName(item->stackData->extra.get(), item->object);
			itemName.append(" ");
			itemName.append(RE::GameSettingCollection::GetSingleton()->GetSetting("sRemoveItemfromInventory")->GetString());
			RE::UIUtils::PlayPipboySound("OBJLunchboxKidsRobotBuild");
			RE::SendHUDMessage::ShowHUDMessage(itemName.c_str(), "", false, true);
		}
		RE::PlayerCharacter::GetSingleton()->inventoryList->RemoveItem1(item->object, item->GetCount(), false);
		//RE::PipboyDataManager::GetSingleton()->
		//RemovePipboyItem(&(*g_PipboyDataManager)->inventoryData, item);
	}

}

bool WeaponHasKeyword(RE::TESObjectWEAP* weapon, RE::BGSKeyword* keyword)
{
	if (weapon)
	{
		return weapon->HasKeyword(keyword);
	}

	return false;
}

bool ArmorHasKeyword(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword)
{
	if (armor)
	{
		return armor->HasKeyword(keyword);
	}

	return false;
}

bool ReferenceHasKeyword(RE::TESObjectREFR* ref, RE::BGSKeyword* keyword)
{
	if (ref)
	{
		if (ref->HasKeyword(keyword))
			return true;

		RE::TBO_InstanceData* myInstanceData = nullptr;
		RE::BSExtraData* myExtraData = ref->extraList->GetByType(RE::EXTRA_DATA_TYPE::kInstanceData);

		if (myExtraData)
		{
			RE::ExtraInstanceData* myExtraInstanceData = dynamic_cast<RE::ExtraInstanceData*>(myExtraData);
			if (myExtraInstanceData)
			{
				myInstanceData = myExtraInstanceData->data.get();
			}
		}

		if (myInstanceData)
		{
			return myInstanceData->GetKeywordData()->HasKeyword(keyword);
		}
	}

	return false;
}

bool ActorHasKeyword(RE::Actor* actor, RE::BGSKeyword* keyword)
{
	if (actor)
	{
		if (actor->HasKeyword(keyword))
			return true;

		RE::TBO_InstanceData* myInstanceData = nullptr;
		RE::BSExtraData* myExtraData = actor->extraList->GetByType(RE::EXTRA_DATA_TYPE::kInstanceData);

		if (myExtraData)
		{
			RE::ExtraInstanceData* myExtraInstanceData = dynamic_cast<RE::ExtraInstanceData*>(myExtraData);
			if (myExtraInstanceData)
			{
				myInstanceData = myExtraInstanceData->data.get();
			}
		}

		if (myInstanceData)
		{
			return myInstanceData->GetKeywordData()->HasKeyword(keyword);
		}
	}

	return false;
}

bool NPCHasKeyword(RE::TESNPC* npc, RE::BGSKeyword* keyword)
{
	if (npc)
	{
		if (npc->HasKeyword(keyword))
			return true;
	}

	//	if still false, check if race has keyword (doesnt seem to be found on the NPC check if exists on race?)
	RE::TESRace* npcRace = npc->formRace;

	if (npcRace)
	{
		return npcRace->HasKeyword(keyword);
	}

	return false;
}


//	Returns True if FormID string is base game OR dynamically placed (FF index)
//	Assumes all DLC is installed
bool IsFormIDStringBaseGame(std::string formIDString)
{
	std::string formIndex = formIDString.substr(0, 2);

	if (formIndex == "00" || formIndex == "01" || formIndex == "02" || formIndex == "03" || formIndex == "04" || formIndex == "05" || formIndex == "06" || formIndex == "ff")
	{
		return true;
	}

	return false;
}

//	Returns FormID as hex string
std::string GetFormIDAsString(std::uint32_t formID)
{
	std::string result{};
	std::stringstream formIDStream;
	formIDStream << std::hex << formID;
	result = formIDStream.str();
	if (result.length() < 8)
	{
		std::string temp;
		for (int i = 0; i < (8 - result.length()); i++)
		{
			temp.append("0");
		}
		temp.append(result);
		result = temp;
	}
	return result;
}



void Shared::HUD::UpdateMenus(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> movieRoot)
{
	REX::WARN("Menu Update requested event!");
	// movieRoot->Invoke("");
}
