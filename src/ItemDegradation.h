#pragma once
#include <unordered_map>
#include <RE/T/TESAmmo.h>
#include <RE/T/TESGlobal.h>
#include <RE/A/Actor.h>
#include <RE/B/BGSKeyword.h>
#include <RE/B/BGSListForm.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <stdint.h>
#include <cstdint>
#include <RE/E/ExtraDataList.h>

namespace F4CW {
	namespace ItemDegradation {
		void DefineItemDegradationFormsFromGame();

		// if the ammo type is not defined, we will use the default value.
		float GetDegradationMapping(RE::TESAmmo* AmmoToCheck);

		// Used when ammo type is not defined.
		float GetDefaultAmmoDegradation();

		extern std::unordered_map<RE::TESAmmo*, float> AmmoDegradationMap;

		bool RegisterDegradationFunctions(RE::BSScript::IVirtualMachine* vm);

		//	Weapon Condition Struct
		struct WeaponConditionData
		{
			WeaponConditionData();
			WeaponConditionData(RE::TESForm* form, RE::ExtraDataList* extradata);
			WeaponConditionData(RE::Actor* myActor, RE::TESForm* form, RE::ExtraDataList* extradata);
			WeaponConditionData(RE::TESObjectREFR* refr);
			WeaponConditionData(RE::Actor* myActor);

			RE::Actor* actor;
			RE::TESForm* Form;
			RE::ExtraDataList* extraData;
			RE::TESObjectWEAP::InstanceData* instance;
			std::uint64_t invHandle;
		};

		//	Armor Condition Struct
		struct ArmorConditionData
		{
			ArmorConditionData();
			ArmorConditionData(RE::TESForm* form, RE::ExtraDataList* extradata);
			ArmorConditionData(RE::Actor* myActor, RE::TESForm* form, RE::ExtraDataList* extradata);
			ArmorConditionData(RE::TESObjectREFR* refr);
			ArmorConditionData(RE::Actor* actor);

			RE::Actor* actor;
			RE::TESForm* Form;
			RE::ExtraDataList* extraData;
			RE::TESObjectARMO::InstanceData* instance;
		};

		//	This enum correlates with TESObjectWEAP's unk137
//	See GameObjects.h
		enum WeaponTypes
		{
			kWeaponType_HandToHandMelee = 0,
			kWeaponType_OneHandSword,
			kWeaponType_OneHandDagger,
			kWeaponType_OneHandAxe,
			kWeaponType_OneHandMace,
			kWeaponType_TwoHandSword,
			kWeaponType_TwoHandAxe,
			kWeaponType_Bow,
			kWeaponType_Staff,
			kWeaponType_Gun,
			kWeaponType_Grenade,
			kWeaponType_Mine
		};

		//	TESObjectWEAP::InstanceData flags
//	See GameObjects.h
		enum WeaponFlags
		{
			kFlag_IgnoresNormalResist = 0x0000002,
			kFlag_MinorCrime = 0x0000004,
			kFlag_ChargingReload = 0x0000008,
			kFlag_HideBackpack = 0x0000010,
			kFlag_NonHostile = 0x0000040,
			kFlag_NPCsUseAmmo = 0x0000200,
			kFlag_RepeatableSingleFire = 0x0000800,
			kFlag_HasScope = 0x0001000,
			kFlag_HoldInputToPower = 0x0002000,
			kFlag_Automatic = 0x0004000,
			kFlag_CantDrop = 0x0008000,
			kFlag_ChargingAttack = 0x0010000,
			kFlag_NotUsedInNormalCombat = 0x0020000,
			kFlag_BoundWeapon = 0x0040000,
			kFlag_SecondaryWeapon = 0x0200000,
			kFlag_BoltAction = 0x0400000,
			kFlag_NoJamAfterReload = 0x0800000,
			kFlag_DisableShells = 0x1000000,
		};

		struct ArmorDegrade
		{
			float	fDamageDealt;
			bool	isMelee;
		};

	}

	namespace WPNUtilities
	{
		void UpdateHUDCondition(ItemDegradation::WeaponConditionData myConditionData);
		// void AddItemHealthPercent(RE::TESForm* form, RE::TESObjectREFR* containerRef, float health);
		float CalculateSkillBonusFromActor(ItemDegradation::WeaponConditionData myConditionData);
		float CalculateUpdatedDamageValue(float baseDamage, float minimum, float conditionPercent, float skillBonus);
		// void UpdateWeaponStats(ItemDegradation::WeaponConditionData myConditionData);
		float GetWeaponDamage(ItemDegradation::WeaponConditionData myConditionData);
		float CalculateUpdatedRateOfFireValue(ItemDegradation::WeaponConditionData myConditionData, float currentCondition);
		/*
		RE::TESForm* GetEquippedWeaponForm(RE::Actor* actor);
		std::uint64_t GetEquippedWeaponHandleID(RE::Actor* actor);
		RE::ExtraDataList* GetEquippedWeaponExtraData(RE::Actor* actor);
		*/
		// RE::TESObjectWEAP::InstanceData* CastInstanceData(RE::TBO_InstanceData* myInstanceData);
		bool IsMeleeWeapon(ItemDegradation::WeaponConditionData myConditionData);

		RE::BGSKeyword* GetWeaponRepairKeyword(RE::TESObjectWEAP* myWeapon);
		RE::BGSListForm* GetLinkedWeaponRepairListFromKeyword(RE::BGSKeyword* myKeyword);
		RE::BGSKeyword* GetLinkedWeaponRepairKeywordFromFormList(RE::BGSListForm* myList);
		bool CanWeaponBeRepairedWithOther(RE::TESObjectWEAP* weapon1, RE::TESObjectWEAP* weapon2);

	}

	namespace ARMOUtilities
	{
		RE::BGSKeyword* GetPowerArmorTypeKeyword();
		//void UpdateArmorStats(ItemDegradation::ArmorConditionData myConditionData);
		//void UpdateArmorStatsOnHit(RE::Actor* actor, std::uint32_t eDamageLimb, std::uint32_t eIncomingDamageType, float fDamage);

		RE::BGSKeyword* GetArmorRepairKeyword(RE::TESObjectARMO* myArmor);
		RE::BGSListForm* GetLinkedArmorRepairListFromKeyword(RE::BGSKeyword* myKeyword);
		RE::BGSKeyword* GetLinkedArmorRepairKeywordFromFormList(RE::BGSListForm* myList);
		bool CanArmorBeRepairedWithOther(RE::TESObjectARMO* armor1, RE::TESObjectARMO* armor2);
	}

	namespace DegradationPapyrus {

		bool AddWeaponRepairList_Papyrus(std::monostate, RE::BGSListForm* repairList, RE::BGSKeyword* keyword);

		bool AddArmorRepairList_Papyrus(std::monostate, RE::BGSListForm* repairList, RE::BGSKeyword* keyword);

		void UpdateInventoryCondition_Papyrus(std::monostate, RE::TESObjectREFR* myRef);

		// void AddItemHealthPercent_Papyrus(std::monostate, RE::TESForm* form, RE::TESObjectREFR* containerRef, float health);

		float GetEquippedWeaponConditionPercentage_Papyrus(std::monostate, RE::Actor* myActor);

		void ModEquippedWeaponConditionPercentage_Papyrus(std::monostate, RE::Actor* myActor, float value);

		RE::BGSListForm* GetWeaponRepairList_Papyrus(std::monostate, RE::TESObjectWEAP* myWeapon);

		RE::BGSKeyword* GetWeaponRepairKeyword_Papyrus(std::monostate, RE::TESObjectWEAP* myWeapon);

		bool CanWeaponBeRepairedWithOther_Papyrus(std::monostate, RE::TESObjectWEAP* weapon1, RE::TESObjectWEAP* weapon2);

		void UpdateWeaponRefStats_Papyrus(std::monostate, RE::TESObjectREFR* weaponRef);

		RE::BGSListForm* GetArmorRepairList_Papyrus(std::monostate, RE::TESObjectARMO* myArmor);

		RE::BGSKeyword* GetArmorRepairKeyword_Papyrus(std::monostate, RE::TESObjectARMO* myArmor);

		bool CanArmorBeRepairedWithOther_Papyrus(std::monostate, RE::TESObjectARMO* armor1, RE::TESObjectARMO* armor2);

		void UpdateArmorRefStats_Papyrus(std::monostate, RE::TESObjectREFR* armorRef);
	}

	RE::TESObjectWEAP::InstanceData* GetWeaponInstanceData(RE::ExtraDataList* myExtraDataList);
	RE::TESObjectARMO::InstanceData* GetArmorInstanceData(RE::ExtraDataList* myExtraDataList);

	float GetWeaponConditionMaximum(ItemDegradation::WeaponConditionData Data);
	float GetWeaponConditionMaximum(RE::TESObjectREFR* refr);
	float GetWeaponConditionPercent(ItemDegradation::WeaponConditionData Data);
	float GetWeaponConditionPercent(RE::TESObjectREFR* refr);
	float GetWeaponConditionCurrent(ItemDegradation::WeaponConditionData Data);
	float GetWeaponConditionCurrent(RE::TESObjectREFR* refr);

	void SetWeaponConditionMaximum(ItemDegradation::WeaponConditionData Data, float Value);
	void SetWeaponConditionMaximum(RE::TESObjectREFR* refr, float Value);
	void SetWeaponConditionPercent(ItemDegradation::WeaponConditionData Data, float Value);
	void SetWeaponConditionPercent(RE::TESObjectREFR* refr, float Value);
	void SetWeaponConditionCurrent(ItemDegradation::WeaponConditionData Data, float Value);
	void SetWeaponConditionCurrent(RE::TESObjectREFR* refr, float Value);

	float GetArmorConditionMaximum(ItemDegradation::ArmorConditionData Data);
	float GetArmorConditionMaximum(RE::TESObjectREFR* refr);
	float GetArmorConditionPercent(ItemDegradation::ArmorConditionData Data);
	float GetArmorConditionPercent(RE::TESObjectREFR* refr);
	float GetArmorConditionCurrent(ItemDegradation::ArmorConditionData Data);
	float GetArmorConditionCurrent(RE::TESObjectREFR* refr);

	void SetArmorConditionMaximum(ItemDegradation::ArmorConditionData Data, float Value);
	void SetArmorConditionMaximum(RE::TESObjectREFR* refr, float Value);
	void SetArmorConditionPercent(ItemDegradation::ArmorConditionData Data, float Value);
	void SetArmorConditionPercent(RE::TESObjectREFR* refr, float Value);
	void SetArmorConditionCurrent(ItemDegradation::ArmorConditionData Data, float Value);
	void SetArmorConditionCurrent(RE::TESObjectREFR* refr, float Value);

	void ModWeaponCondition(ItemDegradation::WeaponConditionData Data, float Value);
	void ModWeaponCondition(RE::TESObjectREFR* refr, float Value);
	void ModWeaponCondition(RE::Actor* actor);

	void InitializeWeaponCondition(ItemDegradation::WeaponConditionData myConditionData);
	void InitializeWeaponCondition(RE::TESObjectREFR* myRef);

	void InitializeArmorCondition(ItemDegradation::ArmorConditionData myConditionData);
	void InitializeArmorCondition(RE::TESObjectREFR* myRef);

	void InitializeInventoryItemCondition(RE::TESObjectREFR* myRef, RE::TESForm* myForm);
	void InitializeInventoryCondition(RE::TESObjectREFR* myRef);
	void InitializePowerArmorFrameInventoryCondition(RE::TESObjectREFR* ref);

	void ItemDegredation_ItemEquipped(RE::TESEquipEvent* myEvent);

	void HandleWeaponJamming();

	void CheckForNPCWeaponConditionPerk(RE::TESObjectREFR* myRef);
}