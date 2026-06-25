#pragma once
#include <RE/B/BGSKeyword.h>
#include <RE/T/TESDataHandler.h>
#include <Scaleform/Scaleform.h>
#include <cstdint>
#include <string>
#include <RE/A/Actor.h>
#include <RE/T/TESGlobal.h>
#include <RE/T/TESNPC.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_FunctionHandler.h>
#include <Scaleform/G/GFx_Value.h>
#include <Scaleform/P/Ptr.h>

namespace Shared {
	extern RE::BGSKeyword* noDegradation;

	extern bool noArmorDegradation;
	extern bool noWeaponDegradation;

	extern RE::TESGlobal* fAutomaticWeaponConditionReduction;
	extern RE::TESGlobal* fBoltWeaponConditionReduction;

	extern RE::TESGlobal* fArmourConditionReductionPerPercentage;

	extern RE::BGSKeyword* crWeaponRanged;

	void InitializeSharedForms(RE::TESDataHandler* dataHandler);
	
	template<typename T>
	bool RegisterFunction(Scaleform::GFx::Value* a_dest, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, const char* a_func_name) {
		Scaleform::GFx::FunctionHandler* func = new T;
		Scaleform::GFx::Value funcValue;

		a_movieRoot->CreateFunction(&funcValue, func);
		return a_dest->SetMember(a_func_name, funcValue);
	}

	namespace HUD {
		void UpdateMenus(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> movieRoot);
	}
	
	bool IsXPMetervisible();

	bool InMenuMode();

	float ConvertPercentageToFloat(std::uint8_t percentage);

	const char* GetItemDisplayName(RE::ExtraDataList* myExtraData, RE::TESBoundObject* baseForm);

	void RemovePipboyInventoryItem(const RE::BGSInventoryItem* item, bool bSilent);

}

bool WeaponHasKeyword(RE::TESObjectWEAP* weapon, RE::BGSKeyword* keyword);

bool ArmorHasKeyword(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

bool ReferenceHasKeyword(RE::TESObjectREFR* ref, RE::BGSKeyword* keyword);

bool ActorHasKeyword(RE::Actor* actor, RE::BGSKeyword* keyword);

bool NPCHasKeyword(RE::TESNPC* npc, RE::BGSKeyword* keyword);

bool IsFormIDStringBaseGame(std::string formIDString);

//	Returns FormID as hex string
std::string GetFormIDAsString(std::uint32_t formID);