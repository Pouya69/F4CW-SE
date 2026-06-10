#include "skills.h"
#include "GameForms.h"
#include "IGlobalConfig.h"
#include "debugLog.h"
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <RE/A/Actor.h>
#include <RE/A/ActorValueInfo.h>
#include <RE/A/ActorValueOwner.h>
#include <RE/B/BGSPerk.h>
#include <RE/B/BSFixedString.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESGlobal.h>
#include <format>

namespace Skills {
	std::vector<RE::ActorValueInfo*> SkillsList;
	std::unordered_map<std::string, RE::ActorValueInfo*> strSkillMap = std::unordered_map<std::string, RE::ActorValueInfo*>();

	std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>> skillsToSpecialMap = std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>>();
	std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>> specialToSkillsMap = std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>>();

	std::unordered_map<std::string, RE::ActorValueInfo*> PerksMap = std::unordered_map<std::string, RE::ActorValueInfo*>();

	std::vector<RE::BGSPerk*> CWNPCPerksList = std::vector<RE::BGSPerk*>();
}

// VanillaAV_Struct VanillaActorValues;

using namespace Skills;




bool RegisterSkillFunctions(RE::BSScript::IVirtualMachine& vm)
{
	
	vm.BindNativeMethod("TCW:F4CW", "ModPermanentSkillValue", CW_SkillsPapyrus::ModPermanentSkillValue_Papyrus, false, false);
	vm.BindNativeMethod("TCW:F4CW", "GetSkillAV", CW_SkillsPapyrus::GetSkillByName_Papyrus, false, false);
	vm.BindNativeMethod("TCW:F4CW", "GetSkill", CW_SkillsPapyrus::GetSkillValueByName_Papyrus, false, false);
	vm.BindNativeMethod("TCW:F4CW", "GetBaseSkill", CW_SkillsPapyrus::GetBaseSkillValueByName_Papyrus, false, false);

	// Debug
	vm.BindNativeMethod("TCW:F4CW", "DEBUG_LogSkillsToConsole", CW_SkillsPapyrus::DEBUG_LogSkillsToConsole_Papyrus, false, false);
	vm.BindNativeMethod("TCW:F4CW", "DEBUG_LogSkillToConsole", CW_SkillsPapyrus::DEBUG_LogSkillToConsole_Papyrus, false, false);
	
	return true;
}

void InitializeGameVariables(RE::TESDataHandler* dataHandler, std::string mod_esm)
{
	Initialize_TOBEMERGED_Vars(dataHandler);
	InitializeGlobalVariables(dataHandler);

	// S.P.E.C.I.A.L
	VanillaActorValues.Strength = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::StrengthID, mod_esm);
	VanillaActorValues.Perception = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::PerceptionID, mod_esm);
	VanillaActorValues.Endurance = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::EnduranceID, mod_esm);
	VanillaActorValues.Charisma = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::CharismaID, mod_esm);
	VanillaActorValues.Intelligence = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::IntelligenceID, mod_esm);
	VanillaActorValues.Agility = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::AgilityID, mod_esm);
	VanillaActorValues.Luck = dataHandler->LookupForm<RE::ActorValueInfo>(SPECIALFormIDs::LuckID, mod_esm);

	// Skills
	CW_Skills.Barter = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE6, mod_esm);
	CW_Skills.BigGuns = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE5, mod_esm);
	CW_Skills.EnergyWeapons = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE4, mod_esm);
	CW_Skills.Explosives = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE3, mod_esm);
	CW_Skills.Lockpick = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE2, mod_esm);
	CW_Skills.Medicine = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE1, mod_esm);
	CW_Skills.MeleeWeapons = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FE0, mod_esm);
	CW_Skills.Repair = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDF, mod_esm);
	CW_Skills.Science = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDE, mod_esm);
	CW_Skills.SmallGuns = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDD, mod_esm);
	CW_Skills.Sneak = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDC, mod_esm);
	CW_Skills.Speech = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDB, mod_esm);
	CW_Skills.Unarmed = dataHandler->LookupForm<RE::ActorValueInfo>(0x048FDA, mod_esm);

	SkillsList.emplace_back(CW_Skills.Barter);
	SkillsList.emplace_back(CW_Skills.BigGuns);
	SkillsList.emplace_back(CW_Skills.EnergyWeapons);
	SkillsList.emplace_back(CW_Skills.Explosives);
	SkillsList.emplace_back(CW_Skills.Lockpick);
	SkillsList.emplace_back(CW_Skills.Medicine);
	SkillsList.emplace_back(CW_Skills.MeleeWeapons);
	SkillsList.emplace_back(CW_Skills.Repair);
	SkillsList.emplace_back(CW_Skills.Science);
	SkillsList.emplace_back(CW_Skills.SmallGuns);
	SkillsList.emplace_back(CW_Skills.Sneak);
	SkillsList.emplace_back(CW_Skills.Speech);
	SkillsList.emplace_back(CW_Skills.Unarmed);


	InitializePerks(dataHandler, mod_esm);

	LOG_INFO("Finished initializing skill variables.");

}

void InitializePerks(RE::TESDataHandler* dataHandler, std::string mod_esm) {
	// Perks

	CWPerks.WeaponTypeBigGunsPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065C20, mod_esm);
	CWPerks.WeaponTypeEnergyWeaponsPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065C21, mod_esm);
	CWPerks.WeaponTypeExplosivesPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065B80, mod_esm);
	CWPerks.WeaponTypeMeleeWeaponsPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065C22, mod_esm);
	CWPerks.WeaponTypeSmallGunsPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065B7B, mod_esm);
	CWPerks.WeaponTypeUnarmedPerk = dataHandler->LookupForm<RE::BGSPerk>(0x065C23, mod_esm);

	// CW Handler Perks
	CWPerks.WeaponConditionHandlerPerk = dataHandler->LookupForm<RE::BGSPerk>(0x0D26DD, mod_esm);
	
	CWNPCPerksList.emplace_back(CWPerks.WeaponConditionHandlerPerk);
}

// Files in ESPs.
void Initialize_TOBEMERGED_Vars(RE::TESDataHandler* dataHandler) {
	// CWGlobals.AmmoDefaultDegradation = dataHandler->LookupForm<RE::TESGlobal>(0x001B06, CURRENT_ESP);
}

void InitializeGlobalVariables(RE::TESDataHandler* dataHandler)
{
	// Globals
	CWGlobals.Karma = dataHandler->LookupForm<RE::TESGlobal>(0x048C3C, MOD_ESM);
	CWGlobals.TutorialWPNCND = dataHandler->LookupForm<RE::TESGlobal>(0x0D3BDE, MOD_ESM);
}

// ================================================================================
// Papyrus Functions

RE::ActorValueInfo* CW_SkillsPapyrus::GetSkillByName_Papyrus(std::monostate, RE::BSFixedString skillToGet)
{
	return GetSkillByName(skillToGet.c_str());
}

uint32_t CW_SkillsPapyrus::GetSkillValueByName_Papyrus(std::monostate, RE::Actor* Actor, RE::BSFixedString skillToGet)
{
	return GetAVValue(Actor, GetSkillByName(skillToGet.c_str()));
}

uint32_t CW_SkillsPapyrus::GetBaseSkillValueByName_Papyrus(std::monostate, RE::Actor* Actor, RE::BSFixedString skillToGet)
{
	return GetBaseAVValue(Actor, GetSkillByName(skillToGet.c_str()));
}

void CW_SkillsPapyrus::ModPermanentSkillValue_Papyrus(std::monostate, RE::TESForm* Form, RE::ActorValueInfo* AVToModify, float modAmount)
{
	ModPermanentSkillValue(Form, AVToModify, modAmount);
}

void CW_SkillsPapyrus::DEBUG_LogSkillsToConsole_Papyrus(std::monostate, RE::Actor* Actor)
{
	std::string result = "Skills: ";
	for (int i = 0; i < SkillsList.size(); i++)
	{
		auto skill = SkillsList[i];
		result += std::format("{}, {}\t|\t", skill->GetFullName(), Actor->GetActorValue(*skill));
	}

	LOG_TO_CONSOLE(result.c_str());
}

void CW_SkillsPapyrus::DEBUG_LogSkillToConsole_Papyrus(std::monostate, RE::Actor* Actor, RE::ActorValueInfo* AV)
{
	LOG_TO_CONSOLE(std::format("Skill {}, {}\n", AV->GetFullName(), Actor->GetActorValue(*AV)).c_str());
}

// ================================================================================
// Internal Functions

float GetAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo)
{
	if (!Actor)
		return NULL;

	return Actor->GetActorValue(*valueInfo);
}

float GetBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo)
{
	if (!Actor)
		return NULL;

	return Actor->GetBaseActorValue(*valueInfo);
}

void ModBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int modAmount)
{
	if (!Actor)
		return;

	return Actor->ModBaseActorValue(*valueInfo, modAmount);
}

void SetBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int setAmount)
{
	if (!Actor)
		return;

	return Actor->SetBaseActorValue(*valueInfo, setAmount);
}

void ModPermanentSkillValue(RE::TESForm* Form, RE::ActorValueInfo* AVToModify, float modAmount)
{
	if (auto valueOwner = dynamic_cast<RE::ActorValueOwner*>(Form)) {
		valueOwner->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kTemporary, *AVToModify, modAmount);
	}
}

RE::ActorValueInfo* GetSkillByName(std::string skillName)
{
	auto result = strSkillMap.find(skillName);
	if (result == strSkillMap.end())
		return nullptr;

	return result->second;
}

float GetPlayerAVValue(RE::ActorValueInfo* valueInfo)
{
	return GetAVValue(RE::PlayerCharacter::GetSingleton(), valueInfo);
}

float GetPlayerBaseAVValue(RE::ActorValueInfo* valueInfo)
{
	return GetBaseAVValue(RE::PlayerCharacter::GetSingleton(), valueInfo);
}

void ModPlayerBaseAVValue(RE::ActorValueInfo* valueInfo, int modAmount)
{
	ModBaseAVValue(RE::PlayerCharacter::GetSingleton(), valueInfo, modAmount);
}

void SetPlayerBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int setAmount)
{
	return SetBaseAVValue(RE::PlayerCharacter::GetSingleton(), valueInfo, setAmount);
}

void Skills::RegisterForSkillLink()
{
	LOG_INFO(std::format("Skills: Linking Skills from {}", MOD_ESM));

	strSkillMap.clear();
	skillsToSpecialMap.clear();
	specialToSkillsMap.clear();

	RegisterLinkedAV(CW_Skills.Barter, CalculateSkillOffset, VanillaActorValues.Charisma, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.BigGuns, CalculateSkillOffset, VanillaActorValues.Endurance, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.EnergyWeapons, CalculateSkillOffset, VanillaActorValues.Perception, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Explosives, CalculateSkillOffset, VanillaActorValues.Perception, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Lockpick, CalculateSkillOffset, VanillaActorValues.Perception, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Medicine, CalculateSkillOffset, VanillaActorValues.Intelligence, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.MeleeWeapons, CalculateSkillOffset, VanillaActorValues.Strength, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Repair, CalculateSkillOffset, VanillaActorValues.Intelligence, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Science, CalculateSkillOffset, VanillaActorValues.Intelligence, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.SmallGuns, CalculateSkillOffset, VanillaActorValues.Agility, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Sneak, CalculateSkillOffset, VanillaActorValues.Agility, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Speech, CalculateSkillOffset, VanillaActorValues.Charisma, VanillaActorValues.Luck);
	RegisterLinkedAV(CW_Skills.Unarmed, CalculateSkillOffset, VanillaActorValues.Endurance, VanillaActorValues.Luck);

	// For being able to find these later by name.
	strSkillMap.emplace("Barter", CW_Skills.Barter);
	strSkillMap.emplace("BigGuns", CW_Skills.BigGuns);
	strSkillMap.emplace("EnergyWeapons", CW_Skills.EnergyWeapons);
	strSkillMap.emplace("Explosives", CW_Skills.Explosives);
	strSkillMap.emplace("Lockpick", CW_Skills.Lockpick);
	strSkillMap.emplace("Medicine", CW_Skills.Medicine);
	strSkillMap.emplace("MeleeWeapons", CW_Skills.MeleeWeapons);
	strSkillMap.emplace("Repair", CW_Skills.Repair);
	strSkillMap.emplace("Science", CW_Skills.Science);
	strSkillMap.emplace("SmallGuns", CW_Skills.SmallGuns);
	strSkillMap.emplace("Sneak", CW_Skills.Sneak);
	strSkillMap.emplace("Speech", CW_Skills.Speech);
	strSkillMap.emplace("Unarmed", CW_Skills.Unarmed);

	

}


float Skills::CalculateSkillOffset(const RE::ActorValueOwner* a_actor, const RE::ActorValueInfo& a_info)
{
	if (!a_actor)
		return 0.0f;


	//	(Dependant x 2) + 2 + (Dependant_02 / 2)
	auto& dependants = *GetDependantAVs(&a_info);

	float dep1 = a_actor->GetActorValue(*dependants[0]);

	float dep2 = a_actor->GetActorValue(*VanillaActorValues.Luck);


	return (dep1 * 2) + 2 + ceilf(dep2 / 2);
	/*
	if (dependants.size() == 1)
		return (a_actor->GetActorValue(*dependants[0]) * 2) + 2 + ceilf(a_actor->GetActorValue(*VanillaActorValues.Luck) / 2);
	else
		return (a_actor->GetActorValue(*dependants[0]) * 2) + 2 + ceilf(a_actor->GetActorValue(*dependants[1]) / 2);
	*/
}

void Skills::RegisterLinkedAV(RE::ActorValueInfo* AV, RE::ActorValueInfo::DerivationFunction_t* CalcFunction, RE::ActorValueInfo* linkedToSPECIAL, RE::ActorValueInfo* linkedToSPECIAL_02)
{
	specialToSkillsMap[linkedToSPECIAL].push_back(AV);
	specialToSkillsMap[linkedToSPECIAL_02].push_back(AV);

	skillsToSpecialMap[AV].push_back(linkedToSPECIAL);
	skillsToSpecialMap[AV].push_back(linkedToSPECIAL_02);

	// @TODO
	// AV->derivationFunction = CalcFunction;
	//AV->VTABLE
}

std::vector<RE::ActorValueInfo*>* Skills::GetDependantAVs(const RE::ActorValueInfo* a_info)
{
	auto result = skillsToSpecialMap.find(a_info);

	if (result == skillsToSpecialMap.end())
		return nullptr;

	return &result->second;
}
