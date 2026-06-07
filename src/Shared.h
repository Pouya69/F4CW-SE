#pragma once
#include <RE/B/BGSKeyword.h>
#include <RE/T/TESDataHandler.h>

namespace Shared {
	extern RE::BGSKeyword* noDegradation;

	extern bool noArmorDegradation;
	extern bool noWeaponDegradation;

	extern const float fAutomaticWeaponConditionReduction;
	extern const float fBoltWeaponConditionReduction;

	extern RE::BGSKeyword* crWeaponRanged;

	void InitializeSharedForms(RE::TESDataHandler* dataHandler);
}

bool WeaponHasKeyword(RE::TESObjectWEAP* weapon, RE::BGSKeyword* keyword);

bool ArmorHasKeyword(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

bool ReferenceHasKeyword(RE::TESObjectREFR* ref, RE::BGSKeyword* keyword);

bool ActorHasKeyword(RE::Actor* actor, RE::BGSKeyword* keyword);

bool NPCHasKeyword(RE::TESNPC* npc, RE::BGSKeyword* keyword);

bool IsFormIDStringBaseGame(std::string formIDString);

//	Returns FormID as hex string
std::string GetFormIDAsString(std::uint32_t formID);