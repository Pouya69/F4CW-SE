#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <RE/A/Actor.h>
#include <RE/A/ActorValueInfo.h>
#include <RE/A/ActorValueOwner.h>
#include <RE/B/BGSPerk.h>
#include <RE/B/BSFixedString.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESForm.h>




namespace Skills {
	extern RE::BSTArray<RE::BGSPerk*> CWPerksLevelUp;
	extern RE::BSTArray<RE::ActorValueInfo*> CWSkillsLevelUp;
	extern RE::BSTArray<RE::BGSSoundDescriptorForm*> ScaleformSkillSounds;
	extern RE::BSTArray<RE::BGSSoundDescriptorForm*> ScaleformPerkSounds;

	extern std::vector<RE::BGSPerk*> CWNPCPerksList;
	extern std::vector<RE::ActorValueInfo*> SkillsList;
	extern std::unordered_map<std::string, RE::ActorValueInfo*> strSkillMap;

	extern std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>> skillsToSpecialMap;
	extern std::unordered_map<const RE::ActorValueInfo*, std::vector<RE::ActorValueInfo*>> specialToSkillsMap;

	extern std::unordered_map<std::string, RE::ActorValueInfo*> PerksMap;

	void RegisterForSkillLink();

	std::vector<RE::ActorValueInfo*>* GetDependantAVs(const RE::ActorValueInfo* a_info);

	// Used for calculating the skill based on Fallout 3's formula. (Dependant * 2) + 2 + (Luck / 2)
	float CalculateSkillOffset(const RE::ActorValueOwner* a_actor, const RE::ActorValueInfo& a_info);

	void RegisterLinkedAV(RE::ActorValueInfo* AV, RE::ActorValueInfo::DerivationFunction_t* CalcFunction, RE::ActorValueInfo* linkedToSPECIAL, RE::ActorValueInfo* linkedToSPECIAL_02 = nullptr);
}

namespace CW_SkillsPapyrus {

	RE::ActorValueInfo* GetSkillByName_Papyrus(std::monostate, RE::BSFixedString skillToGet);
	uint32_t GetSkillValueByName_Papyrus(std::monostate, RE::Actor* Actor, RE::BSFixedString skillToGet);
	uint32_t GetBaseSkillValueByName_Papyrus(std::monostate, RE::Actor* Actor, RE::BSFixedString skillToGet);
	void ModPermanentSkillValue_Papyrus(std::monostate, RE::TESForm* Actor, RE::ActorValueInfo* AVToModify, float modAmount);

	// Debug
	void DEBUG_LogSkillsToConsole_Papyrus(std::monostate, RE::Actor* Actor);
	inline void DEBUG_LogSkillToConsole_Papyrus(std::monostate, RE::Actor* Actor, RE::ActorValueInfo* AV);
}

void InitializeGameVariables(RE::TESDataHandler* dataHandler, std::string mod_esm);
bool RegisterSkillFunctions(RE::BSScript::IVirtualMachine& vm);
void Initialize_TOBEMERGED_Vars(RE::TESDataHandler* dataHandler);

void InitializeGlobalVariables(RE::TESDataHandler* dataHandler);

void InitializePerks(RE::TESDataHandler* dataHandler, std::string mod_esm);

void GetLevelUpFormsFromGame();

// Internal Functions
// ===========================================================================

float GetAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo);
float GetBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo);
void ModBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int modAmount);
void SetBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int setAmount);
void ModPermanentSkillValue(RE::TESForm* Actor, RE::ActorValueInfo* AVToModify, float modAmount);

RE::ActorValueInfo* GetSkillByName(std::string skillName);


float GetPlayerAVValue(RE::ActorValueInfo* valueInfo);
float GetPlayerBaseAVValue(RE::ActorValueInfo* valueInfo);
void ModPlayerBaseAVValue(RE::ActorValueInfo* valueInfo, int modAmount);
void SetPlayerBaseAVValue(RE::Actor* Actor, RE::ActorValueInfo* valueInfo, int setAmount);
