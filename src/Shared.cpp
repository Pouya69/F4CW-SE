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

namespace Shared {
	RE::BGSKeyword* noDegradation;
	bool noArmorDegradation;
	bool noWeaponDegradation;

	RE::BGSKeyword* crWeaponRanged;


	void InitializeSharedForms(RE::TESDataHandler* dataHandler)
	{
		crWeaponRanged = dataHandler->LookupForm<RE::BGSKeyword>(0x0DEDC5, "Fallout4.esm");
		noDegradation = dataHandler->LookupForm<RE::BGSKeyword>(0x189348, MOD_ESM);
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