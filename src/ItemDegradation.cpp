#include "ItemDegradation.h"
#include <unordered_map>
#include <RE/T/TESAmmo.h>
#include "debugLog.h"
#include "IGlobalConfig.h"
#include "GameForms.h"
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/T/TESDataHandler.h>
#include <format>
#include <RE/A/Actor.h>
#include <RE/B/BGSKeyword.h>
#include <RE/B/BGSListForm.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <RE/P/PlayerCharacter.h>
#include "Serialization.h"
#include <chrono>
#include <cstdint>
#include <RE/E/ExtraDataList.h>
#include <RE/S/Setting.h>
#include <RE/T/TBO_InstanceData.h>
#include <RE/T/TESEquipEvent.h>
#include <RE/E/EXTRA_DATA_TYPE.h>
#include "Hooks.h"
#include <variant>
#include <RE/B/BSFixedString.h>
#include <RE/I/IMenu.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Scaleform/G/GFx_Value.h>
#include <RE/U/UI.h>
#include "skills.h"
#include "Shared.h"
#include <cmath>
#include <string>
#include <RE/B/BGSEquipIndex.h>
#include <RE/B/BGSObjectInstance.h>
#include <RE/W/WEAPON_FLAGS.h>
#include <RE/W/WEAPON_TYPE.h>
#include <RE/P/PipboyDataManager.h>
#include <RE/A/ActorEquipManager.h>
#include <RE/B/BSExtraData.h>
#include <RE/E/ExtraInstanceData.h>
#include <RE/T/TES.h>
#include <REX/LOG.h>
#include <RE/A/ActorValueInfo.h>
#include <RE/B/BGSDamageType.h>
#include <Windows.h>
#include <RE/B/BGSAction.h>
#include <RE/E/ExtraCharge.h>
#include <RE/E/ExtraHealth.h>
#include <RE/B/BGSBodyPartDefs.h>

namespace F4CW {
	namespace ItemDegradation {

		std::unordered_map<RE::TESAmmo*, float> AmmoDegradationMap;

		//	weapon condition game settings
		RE::Setting* fDamageWeaponMult;
		RE::Setting* fDamageGunWeapCondMult;
		RE::Setting* fDamageGunWeapCondBase;
		RE::Setting* fDamageMeleeWeapCondMult;
		RE::Setting* fDamageMeleeWeapCondBase;
		float	 fDamageSkillBase;
		float	 fDamageSkillMult;
		float	 fAVDMeleeDamageStrengthMult;
		float	 fAVDMeleeDamageStrengthOffset;
		RE::Setting* fDamageArmConditionBase;
		RE::Setting* fDamageArmConditionMult;

		//	weapon condition jam game settings https://geck.bethsoft.com/index.php?title=FWeaponConditionReloadJamXX
		RE::Setting* fWeaponConditionReloadJam1;
		RE::Setting* fWeaponConditionReloadJam2;
		RE::Setting* fWeaponConditionReloadJam3;
		RE::Setting* fWeaponConditionReloadJam4;
		RE::Setting* fWeaponConditionReloadJam5;
		RE::Setting* fWeaponConditionReloadJam6;
		RE::Setting* fWeaponConditionReloadJam7;
		RE::Setting* fWeaponConditionReloadJam8;
		RE::Setting* fWeaponConditionReloadJam9;
		RE::Setting* fWeaponConditionReloadJam10;

		bool bShouldTryAndJamWeapon = false;
		std::chrono::system_clock::time_point lastTimeJammed;

		//	weapon condition rate of fire game settings https://geck.bethsoft.com/index.php?title=FWeaponConditionRateOfFireXX
		RE::Setting* fWeaponConditionRateOfFire1;
		RE::Setting* fWeaponConditionRateOfFire2;
		RE::Setting* fWeaponConditionRateOfFire3;
		RE::Setting* fWeaponConditionRateOfFire4;
		RE::Setting* fWeaponConditionRateOfFire5;
		RE::Setting* fWeaponConditionRateOfFire6;
		RE::Setting* fWeaponConditionRateOfFire7;
		RE::Setting* fWeaponConditionRateOfFire8;
		RE::Setting* fWeaponConditionRateOfFire9;
		RE::Setting* fWeaponConditionRateOfFire10;

		ItemDegredation_Struct ItemDegredationForms;

		RE::BGSKeyword* noItemDegredationKeyword;
		RE::BGSKeyword* crWeaponRanged;

		RE::BGSKeyword* furntitureTypePowerArmor;

		//	armor type keywords
		RE::BGSKeyword* armorTypePower;
		RE::BGSKeyword* armorBodyPartChest;
		RE::BGSKeyword* armorBodyPartHead;
		RE::BGSKeyword* armorBodyPartLeftArm;
		RE::BGSKeyword* armorBodyPartRightArm;
		RE::BGSKeyword* armorBodyPartLeftLeg;
		RE::BGSKeyword* armorBodyPartRightLeg;

		bool bIgnoreNoSkillWeaponPrompt = false;
		bool bIgnoreMaxSkillsNPC = false;
		bool bIgnoreMaxSkillsPlayer = false;

		float tempWantedWeaponCondition = 0.0;


		ArmorConditionData::ArmorConditionData() : actor(nullptr), Form(nullptr), extraData(nullptr), instance(nullptr) {}

		ArmorConditionData::ArmorConditionData(RE::TESForm* form, RE::ExtraDataList* extradata)
		{
			actor = nullptr;
			Form = form;
			extraData = extradata;
			instance = GetArmorInstanceData(extraData);
		}

		ArmorConditionData::ArmorConditionData(RE::Actor* myActor, RE::TESForm* form, RE::ExtraDataList* extradata)
		{
			actor = myActor;
			Form = form;
			extraData = extradata;
			instance = GetArmorInstanceData(extraData);
		}

		ArmorConditionData::ArmorConditionData(RE::TESObjectREFR* refr)
		{
			if (refr)
			{
				actor = RE::PlayerCharacter::GetSingleton();	//	Going to set this to Player so that when a ref gets initialised it already has the Player's stats
				Form = refr;
				extraData = refr->extraList.get();
				instance = GetArmorInstanceData(extraData);
			}
		}

		ArmorConditionData::ArmorConditionData(RE::Actor* myActor)
		{
			if (myActor)
			{
				actor = myActor;
				//Form = WPNUtilities::GetEquippedWeaponForm(myActor);
				//extraData = WPNUtilities::GetEquippedWeaponExtraData(myActor);
				//instance = WPNUtilities::GetEquippedWeaponInstanceData(myActor);
			}
		}
	}

	namespace ItemDegradation{
		WeaponConditionData::WeaponConditionData() : actor(nullptr), Form(nullptr), extraData(nullptr), instance(nullptr) {}

		WeaponConditionData::WeaponConditionData(RE::TESForm* form, RE::ExtraDataList* extradata)
		{
			actor = nullptr;
			Form = form;
			extraData = extradata;
			invHandle = 0;

			instance = GetWeaponInstanceData(extraData);
		}

		WeaponConditionData::WeaponConditionData(RE::Actor* myActor, RE::TESForm* form, RE::ExtraDataList* extradata)
		{
			actor = myActor;
			Form = form;
			extraData = extradata;
			invHandle = 0;
			// invHandle = WPNUtilities::GetEquippedWeaponHandleID(myActor);

			instance = GetWeaponInstanceData(extraData);
		}

		WeaponConditionData::WeaponConditionData(RE::TESObjectREFR* refr)
		{
			if (refr)
			{
				actor = RE::PlayerCharacter::GetSingleton();	//	Going to set this to Player so that when a ref gets initialised it already has the Player's stats
				Form = refr;
				extraData = refr->extraList.get();
				invHandle = 0;

				instance = GetWeaponInstanceData(extraData);
			}
		}
		
		WeaponConditionData::WeaponConditionData(RE::Actor* myActor)
		{
			if (myActor)
			{
				auto weapon = static_cast<RE::TESObjectREFR*>(myActor->currentProcess->middleHigh->equippedItems[0].item.object);
				if (weapon) {
					actor = myActor;
					Form = weapon;
					extraData = weapon->extraList.get();
					instance = GetWeaponInstanceData(extraData);
					invHandle = 0;
				}
				else {
					REX::WARN(std::format("Tried to get equipoped weapon from actor '{}' but didn't cast", myActor->GetDisplayFullName()));
				}
			}
		}
		
	}

	RE::TESObjectWEAP::InstanceData* GetWeaponInstanceData(RE::ExtraDataList* myExtraDataList)
	{
		RE::TESObjectWEAP::InstanceData* result = nullptr;
		RE::TBO_InstanceData* myInstanceData = nullptr;

		if (myExtraDataList)
		{
			RE::BSExtraData* myExtraData = myExtraDataList->GetByType(RE::EXTRA_DATA_TYPE::kInstanceData);

			if (myExtraData)
			{
				RE::ExtraInstanceData* myExtraInstanceData = static_cast<RE::ExtraInstanceData*>(myExtraData);
				if (myExtraInstanceData)
				{
					myInstanceData = myExtraInstanceData->data.get();
				}
			}
		}
		if (!myInstanceData)
			return nullptr;

		result = static_cast<RE::TESObjectWEAP::InstanceData*>(myInstanceData);
		return result;
	}

	RE::TESObjectARMO::InstanceData* GetArmorInstanceData(RE::ExtraDataList* myExtraDataList)
	{
		RE::TESObjectARMO::InstanceData* result = nullptr;
		RE::TBO_InstanceData* myInstanceData = nullptr;

		if (myExtraDataList)
		{
			RE::BSExtraData* myExtraData = myExtraDataList->GetByType(RE::EXTRA_DATA_TYPE::kInstanceData);

			if (myExtraData)
			{
				RE::ExtraInstanceData* myExtraInstanceData = static_cast<RE::ExtraInstanceData*>(myExtraData);
				if (myExtraInstanceData)
				{
					myInstanceData = myExtraInstanceData->data.get();
				}
			}
		}
		if (!myInstanceData)
			return nullptr;

		result = static_cast<RE::TESObjectARMO::InstanceData*>(myInstanceData);
		return result;
	}

	float GetWeaponConditionMaximum(ItemDegradation::WeaponConditionData Data)
	{
		if (!Data.extraData)
			return -1.0;
		if (!Data.extraData->HasType(RE::EXTRA_DATA_TYPE::kCharge))
			return -1.0;

		return ((RE::ExtraCharge*) Data.extraData->GetByType(RE::EXTRA_DATA_TYPE::kCharge))->charge;
	}

	float GetWeaponConditionMaximum(RE::TESObjectREFR* refr)
	{
		return refr ? GetWeaponConditionMaximum(ItemDegradation::WeaponConditionData(refr)) : -1.0f;
	}

	float GetWeaponConditionPercent(ItemDegradation::WeaponConditionData Data)
	{
		if (!Data.extraData)
			return -1.0;

		return Data.extraData->GetHealthPerc();
	}

	float GetWeaponConditionPercent(RE::TESObjectREFR* refr)
	{
		return refr ? GetWeaponConditionPercent(ItemDegradation::WeaponConditionData(refr)) : -1.0f;
	}

	float GetWeaponConditionCurrent(ItemDegradation::WeaponConditionData Data)
	{
		float Percent = GetWeaponConditionPercent(Data);
		float Maximum = GetWeaponConditionMaximum(Data);
		if ((0 > Percent) || (0 > Maximum))
			return -1.0;
		return Percent * Maximum;
	}

	float GetWeaponConditionCurrent(RE::TESObjectREFR* refr)
	{
		return refr ? GetWeaponConditionCurrent(ItemDegradation::WeaponConditionData(refr)) : -1.0f;
	}

	void SetWeaponConditionMaximum(ItemDegradation::WeaponConditionData Data, float Value)
	{
		if (!Data.extraData)
			return;
		if (!Data.extraData->HasType(RE::EXTRA_DATA_TYPE::kCharge))
			return;
		Value = max(1, Value);
		((RE::ExtraCharge*)Data.extraData->GetByType(RE::EXTRA_DATA_TYPE::kCharge))->charge = Value;
		// SetExtraData(Data.extraData, Charge, charge, Value);
	}

	void SetWeaponConditionMaximum(RE::TESObjectREFR* refr, float Value)
	{
		if (!refr)
			return;

		SetWeaponConditionMaximum(ItemDegradation::WeaponConditionData(refr), Value);
	}

	void SetWeaponConditionPercent(ItemDegradation::WeaponConditionData Data, float Value)
	{}

	void SetWeaponConditionPercent(RE::TESObjectREFR * refr, float Value)
	{
		SetWeaponConditionPercent(ItemDegradation::WeaponConditionData(refr), Value);
	}

	void SetWeaponConditionCurrent(ItemDegradation::WeaponConditionData Data, float Value)
	{}

	void SetWeaponConditionCurrent(RE::TESObjectREFR * refr, float Value)
	{
		if (!refr)
			return;

		SetWeaponConditionCurrent(ItemDegradation::WeaponConditionData(refr), Value);
	}

	float GetArmorConditionMaximum(ItemDegradation::ArmorConditionData Data)
	{
		return 0.0f;
	}
	float GetArmorConditionMaximum(RE::TESObjectREFR* refr)
	{
		if (!refr)
			return -1.0f;

		return GetArmorConditionMaximum(ItemDegradation::ArmorConditionData(refr));

	}
	float GetArmorConditionPercent(ItemDegradation::ArmorConditionData Data)
	{
		return 0.0f;
	}
	float GetArmorConditionPercent(RE::TESObjectREFR* refr)
	{
		if (!refr)
			return -1.0f;

		return GetArmorConditionPercent(ItemDegradation::ArmorConditionData(refr));
	}
	float GetArmorConditionCurrent(ItemDegradation::ArmorConditionData Data)
	{
		return 0.0f;
	}
	float GetArmorConditionCurrent(RE::TESObjectREFR* refr)
	{
		if (!refr)
			return -1.0f;

		return GetArmorConditionCurrent(ItemDegradation::ArmorConditionData(refr));
	}
	void SetArmorConditionMaximum(ItemDegradation::ArmorConditionData Data, float Value)
	{}
	void SetArmorConditionMaximum(RE::TESObjectREFR * refr, float Value)
	{
		if (!refr)
			return;
		SetArmorConditionMaximum(ItemDegradation::ArmorConditionData(refr), Value);
	}
	void SetArmorConditionPercent(ItemDegradation::ArmorConditionData Data, float Value)
	{}
	void SetArmorConditionPercent(RE::TESObjectREFR * refr, float Value)
	{
		if (!refr)
			return;
		SetArmorConditionPercent(ItemDegradation::ArmorConditionData(refr), Value);
	}
	void SetArmorConditionCurrent(ItemDegradation::ArmorConditionData Data, float Value)
	{}
	void SetArmorConditionCurrent(RE::TESObjectREFR * refr, float Value)
	{
		if (!refr)
			return;
		SetArmorConditionCurrent(ItemDegradation::ArmorConditionData(refr), Value);
	}
	void ModWeaponCondition(ItemDegradation::WeaponConditionData Data, float Value)
	{}
	void ModWeaponCondition(RE::TESObjectREFR * refr, float Value)
	{
		if (!refr)
			return;
		ModWeaponCondition(ItemDegradation::WeaponConditionData(refr), Value);
	}
	void ModWeaponCondition(RE::Actor * actor)
	{}

	void InitializeWeaponCondition(ItemDegradation::WeaponConditionData myConditionData)
	{
		
	}
	void InitializeWeaponCondition(RE::TESObjectREFR * myRef)
	{
		if (!myRef)
			return;
		InitializeWeaponCondition(ItemDegradation::WeaponConditionData(myRef));
	}
	void InitializeArmorCondition(ItemDegradation::ArmorConditionData myConditionData)
	{}
	void InitializeArmorCondition(RE::TESObjectREFR * myRef)
	{
		if (!myRef)
			return;
		InitializeArmorCondition(ItemDegradation::ArmorConditionData(myRef));
	}


	void InitializeInventoryItemCondition(RE::TESObjectREFR * myRef, RE::TESForm * myForm)
	{}
	void InitializeInventoryCondition(RE::TESObjectREFR * myRef)
	{}
	void InitializePowerArmorFrameInventoryCondition(RE::TESObjectREFR * ref)
	{}
	void ItemDegredation_ItemEquipped(RE::TESEquipEvent * myEvent)
	{}
	void HandleWeaponJamming()
	{}
	void CheckForNPCWeaponConditionPerk(RE::TESObjectREFR * myRef)
	{}
}

void F4CW::ItemDegradation::DefineItemDegradationFormsFromGame()
{
	LOG_INFO("Fetching Item Degradation Forms From Game...");
	RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
	
	RE::TESAmmo* ammo10mm = dataHandler->LookupForm<RE::TESAmmo>(0x01F276, "Fallout4.esm");
	RE::TESAmmo* ammo2mmEC = dataHandler->LookupForm<RE::TESAmmo>(0x18ABDF, "Fallout4.esm");
	RE::TESAmmo* ammo308 = dataHandler->LookupForm<RE::TESAmmo>(0x01F66B, "Fallout4.esm");
	RE::TESAmmo* ammo38 = dataHandler->LookupForm<RE::TESAmmo>(0x04CE87, "Fallout4.esm");
	RE::TESAmmo* ammo44 = dataHandler->LookupForm<RE::TESAmmo>(0x09221C, "Fallout4.esm");
	RE::TESAmmo* ammo45 = dataHandler->LookupForm<RE::TESAmmo>(0x01F66A, "Fallout4.esm");


	ItemDegredationForms.weaponConditionHealthMaxDMGT = dataHandler->LookupForm<RE::BGSDamageType>(0x0D26DB, MOD_ESM);

	//	Actor Values that hold information on Items
	ItemDegredationForms.itemConditionMaxHealth = dataHandler->LookupForm<RE::ActorValueInfo>(0x0D26E2, MOD_ESM);
	ItemDegredationForms.itemConditionMinHealth = dataHandler->LookupForm<RE::ActorValueInfo>(0x0D26E3, MOD_ESM);
	ItemDegredationForms.itemConditionStartCond = dataHandler->LookupForm<RE::ActorValueInfo>(0x0D26E4, MOD_ESM);

	//	this holds our starting condition percent on OMODs in the Creation Kit
	ItemDegredationForms.weaponConditionHealthStartingDMGT = dataHandler->LookupForm<RE::BGSDamageType>(0x0D3B74, MOD_ESM);

	ItemDegradation::AmmoDegradationMap[ammo10mm] = 0.005f;
	ItemDegradation::AmmoDegradationMap[ammo2mmEC] = 0.04f;
	ItemDegradation::AmmoDegradationMap[ammo308] = 0.0133f;
	ItemDegradation::AmmoDegradationMap[ammo38] = 0.003f;
	ItemDegradation::AmmoDegradationMap[ammo44] = 0.011f;
	ItemDegradation::AmmoDegradationMap[ammo45] = 0.0035f;

	//	Get Condition Game Settings
	fDamageWeaponMult = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageWeaponMult");
	fDamageGunWeapCondMult = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageGunWeapCondMult");
	fDamageGunWeapCondBase = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageGunWeapCondBase");
	fDamageMeleeWeapCondMult = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageMeleeWeapCondMult");
	fDamageMeleeWeapCondBase = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageMeleeWeapCondBase");
	//	getting custom GSTT doesn't seem to work? using floats instead
	//fDamageSkillBase = dataHandler->LookupForm<RE::Setting*>(GetFormFromIdentifier("CapitalWasteland.esm|D3BD1"));
	fDamageSkillBase = 0.5;
	//fDamageSkillMult = dataHandler->LookupForm<RE::Setting*>(GetFormFromIdentifier("CapitalWasteland.esm|D3BD2"));
	fDamageSkillMult = 0.5;
	//fAVDMeleeDamageStrengthMult = dataHandler->LookupForm<RE::Setting*>(GetFormFromIdentifier("CapitalWasteland.esm|D3BD3"));
	fAVDMeleeDamageStrengthMult = 0.5;
	//fAVDMeleeDamageStrengthOffset = dataHandler->LookupForm<RE::Setting*>(GetFormFromIdentifier("CapitalWasteland.esm|D3BD4"));
	fAVDMeleeDamageStrengthOffset = 0.0;
	fDamageArmConditionBase = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageArmConditionBase");
	fDamageArmConditionMult = RE::GameSettingCollection::GetSingleton()->GetSetting("fDamageArmConditionMult");

	fWeaponConditionReloadJam1 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam1");
	fWeaponConditionReloadJam2 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam2");
	fWeaponConditionReloadJam3 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam3");
	fWeaponConditionReloadJam4 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam4");
	fWeaponConditionReloadJam5 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam5");
	fWeaponConditionReloadJam6 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam6");
	fWeaponConditionReloadJam7 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam7");
	fWeaponConditionReloadJam8 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam8");
	fWeaponConditionReloadJam9 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam9");
	fWeaponConditionReloadJam10 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionReloadJam10");

	fWeaponConditionRateOfFire1 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire1");
	fWeaponConditionRateOfFire2 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire2");
	fWeaponConditionRateOfFire3 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire3");
	fWeaponConditionRateOfFire4 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire4");
	fWeaponConditionRateOfFire5 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire5");
	fWeaponConditionRateOfFire6 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire6");
	fWeaponConditionRateOfFire7 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire7");
	fWeaponConditionRateOfFire8 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire8");
	fWeaponConditionRateOfFire9 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire9");
	fWeaponConditionRateOfFire10 = RE::GameSettingCollection::GetSingleton()->GetSetting("fWeaponConditionRateOfFire10");

	//	Reload Jam Stuff
	ItemDegredationForms.actionReloadJam = dataHandler->LookupForm<RE::BGSAction>(0x0DDAE3, MOD_ESM);

	//	Misc Degredation Stuff
	ItemDegredationForms.tempConditionVariable = dataHandler->LookupForm<RE::ActorValueInfo>(0x0DCE0F, "Fallout4.esm");

	//	Store current time
	lastTimeJammed = std::chrono::system_clock::now();

	//	Get Item Degredation misc keywords
	noItemDegredationKeyword = dataHandler->LookupForm<RE::BGSKeyword>(0x0DEDC5, MOD_ESM);
	crWeaponRanged = dataHandler->LookupForm<RE::BGSKeyword>(0x189348, "Fallout4.esm");

	//	Get Armor Type Keywords
	armorTypePower = dataHandler->LookupForm<RE::BGSKeyword>(0x04D8A1, "Fallout4.esm");
	armorBodyPartChest = dataHandler->LookupForm<RE::BGSKeyword>(0x06C0EC, "Fallout4.esm");
	armorBodyPartHead = dataHandler->LookupForm<RE::BGSKeyword>(0x10C418, "Fallout4.esm");

	armorBodyPartLeftArm = dataHandler->LookupForm<RE::BGSKeyword>(0x09FFC5, MOD_ESM);
	armorBodyPartRightArm = dataHandler->LookupForm<RE::BGSKeyword>(0x09FFC6, MOD_ESM);
	armorBodyPartLeftLeg = dataHandler->LookupForm<RE::BGSKeyword>(0x09FFC7, MOD_ESM);
	armorBodyPartRightLeg = dataHandler->LookupForm<RE::BGSKeyword>(0x09FFC8, MOD_ESM);

	//	Get INI Settings
	std::string myINI = "./Data/CapitalWasteland.ini";
	bIgnoreMaxSkillsNPC = GetPrivateProfileInt("Skills", "bIgnoreMaxSkillsNPC", 0, myINI.c_str());
	bIgnoreMaxSkillsPlayer = GetPrivateProfileInt("Skills", "bIgnoreMaxSkillsPlayer", 0, myINI.c_str());


	LOG_INFO("Item degradation forms fetched.");
}

float F4CW::ItemDegradation::GetDegradationMapping(RE::TESAmmo* AmmoToCheck)
{
	auto result = AmmoDegradationMap.find(AmmoToCheck);
	if (AmmoDegradationMap.empty() || result == AmmoDegradationMap.end()) {
		// Did not find ammo in the map. Returning default value from Globals.
		return GetDefaultAmmoDegradation();
	}

	return result->second;
}

float F4CW::ItemDegradation::GetDefaultAmmoDegradation()
{
	return CWGlobals.AmmoDefaultDegradation ? CWGlobals.AmmoDefaultDegradation->GetValue() : 0.01f;
}

bool F4CW::ItemDegradation::RegisterDegradationFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->BindNativeMethod("TCW:F4CW", "AddWeaponRepairList", F4CW::DegradationPapyrus::AddWeaponRepairList_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "AddArmorRepairList", F4CW::DegradationPapyrus::AddArmorRepairList_Papyrus);

	vm->BindNativeMethod("TCW:F4CW", "UpdateInventoryCondition", F4CW::DegradationPapyrus::UpdateInventoryCondition_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "GetEquippedWeaponConditionPercent", F4CW::DegradationPapyrus::GetEquippedWeaponConditionPercentage_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "ModEquippedWeaponConditionPercent", F4CW::DegradationPapyrus::ModEquippedWeaponConditionPercentage_Papyrus);

	vm->BindNativeMethod("TCW:F4CW", "GetWeaponRepairList", F4CW::DegradationPapyrus::GetWeaponRepairList_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "GetWeaponRepairKeyword", F4CW::DegradationPapyrus::GetWeaponRepairKeyword_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "CanWeaponBeRepairedWithOther", F4CW::DegradationPapyrus::CanWeaponBeRepairedWithOther_Papyrus);
	
	vm->BindNativeMethod("TCW:F4CW", "UpdateWeaponRefStats", F4CW::DegradationPapyrus::UpdateWeaponRefStats_Papyrus);

	vm->BindNativeMethod("TCW:F4CW", "GetArmorRepairList", F4CW::DegradationPapyrus::GetArmorRepairList_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "GetArmorRepairKeyword", F4CW::DegradationPapyrus::GetArmorRepairKeyword_Papyrus);
	vm->BindNativeMethod("TCW:F4CW", "CanArmorBeRepairedWithOther", F4CW::DegradationPapyrus::CanArmorBeRepairedWithOther_Papyrus);

	vm->BindNativeMethod("TCW:F4CW", "UpdateArmorRefStats", F4CW::DegradationPapyrus::UpdateArmorRefStats_Papyrus);

	// vm->BindNativeMethod("TCW:F4CW", "AddItemHealthPercent", F4CW::DegradationPapyrus::AddItemHealthPercent_Papyrus);

	return true;
}

// Papyrus Functions ==============================================================================================

bool F4CW::DegradationPapyrus::AddWeaponRepairList_Papyrus(std::monostate, RE::BGSListForm* repairList, RE::BGSKeyword* keyword)
{
	return F4CWSerialization::AddToWeaponRepairList(repairList, keyword);
}

bool F4CW::DegradationPapyrus::AddArmorRepairList_Papyrus(std::monostate, RE::BGSListForm* repairList, RE::BGSKeyword* keyword)
{
	return F4CWSerialization::AddToArmorRepairList(repairList, keyword);
}

void F4CW::DegradationPapyrus::UpdateInventoryCondition_Papyrus(std::monostate, RE::TESObjectREFR* myRef)
{
	if (myRef == nullptr) return;

	LOG_INFO(std::format("Update Inventory Condition Called on %s from Papyrus", myRef->GetDisplayFullName()));
	InitializeInventoryCondition(myRef);
}

/*
void F4CW::DegradationPapyrus::AddItemHealthPercent_Papyrus(std::monostate, RE::TESForm * form, RE::TESObjectREFR * containerRef, float health)
{
	switch (form->GetFormType())
	{
		case RE::ENUM_FORM_ID::kWEAP:
			WPNUtilities::AddItemHealthPercent(form, containerRef, health);
			break;

		//default:
			//Hooks::Hook_BGSInventoryListAddItem(containerRef->inventoryList, form->As<RE::TESBoundObject>(), 1, false);
			//break;
	}
}
*/

float F4CW::DegradationPapyrus::GetEquippedWeaponConditionPercentage_Papyrus(std::monostate, RE::Actor * myActor)
{
	LOG_INFO("GetEquippedWeaponConditionPercentage called from Papyrus");

	ItemDegradation::WeaponConditionData actorData(myActor);

	if (actorData.Form == nullptr || actorData.instance == nullptr || actorData.extraData == nullptr)
		return -1.0;

	return GetWeaponConditionPercent(actorData);
}

void F4CW::DegradationPapyrus::ModEquippedWeaponConditionPercentage_Papyrus(std::monostate, RE::Actor* myActor, float value)
{
	LOG_INFO("ModEquippedWeaponConditionPercentage called from Papyrus");

	ItemDegradation::WeaponConditionData actorData(myActor);

	auto &weapon = myActor->currentProcess->middleHigh->equippedItems[0];

	if (actorData.Form == nullptr || actorData.instance == nullptr || actorData.extraData == nullptr)
		return;

	float percentageInitial = GetWeaponConditionPercent(actorData);
	float percentageIncrease = value;

	percentageIncrease += percentageInitial;

	if (percentageIncrease > 1.0)
	{
		percentageIncrease = 1.0;
	}
	else if (percentageIncrease <= 0.0)
	{
		percentageIncrease = 0.0;

		myActor->inventoryList->rwLock.unlock_read();
		RE::ActorEquipManager::GetSingleton()->UnequipItem(myActor, &weapon, false);
		myActor->inventoryList->rwLock.lock_read();
	}

	LOG_INFO(std::format("Initial Condition = {}, New Condition = {}", percentageInitial, percentageIncrease));
	SetWeaponConditionPercent(actorData, percentageIncrease);

	// @TODO
	// WPNUtilities::UpdateWeaponStats(actorData);

	if (myActor == RE::PlayerCharacter::GetSingleton())
	{
		WPNUtilities::UpdateHUDCondition(actorData);
	}
}

RE::BGSListForm* F4CW::DegradationPapyrus::GetWeaponRepairList_Papyrus(std::monostate, RE::TESObjectWEAP * myWeapon)
{
	RE::BGSKeyword* myRepairKeyword = WPNUtilities::GetWeaponRepairKeyword(myWeapon);

	if (myRepairKeyword)
	{
		return WPNUtilities::GetLinkedWeaponRepairListFromKeyword(myRepairKeyword);
	}

	return NULL;
}

RE::BGSKeyword* F4CW::DegradationPapyrus::GetWeaponRepairKeyword_Papyrus(std::monostate, RE::TESObjectWEAP* myWeapon)
{
	return WPNUtilities::GetWeaponRepairKeyword(myWeapon);
}

bool F4CW::DegradationPapyrus::CanWeaponBeRepairedWithOther_Papyrus(std::monostate, RE::TESObjectWEAP* weapon1, RE::TESObjectWEAP* weapon2)
{
	return WPNUtilities::CanWeaponBeRepairedWithOther(weapon1, weapon2);
}

void F4CW::DegradationPapyrus::UpdateWeaponRefStats_Papyrus(std::monostate, RE::TESObjectREFR* weaponRef)
{
	InitializeWeaponCondition(weaponRef);
}

RE::BGSListForm* F4CW::DegradationPapyrus::GetArmorRepairList_Papyrus(std::monostate, RE::TESObjectARMO * myArmor)
{
	RE::BGSKeyword* myRepairKeyword = ARMOUtilities::GetArmorRepairKeyword(myArmor);

	if (myRepairKeyword)
	{
		return ARMOUtilities::GetLinkedArmorRepairListFromKeyword(myRepairKeyword);
	}

	return nullptr;
}

RE::BGSKeyword* F4CW::DegradationPapyrus::GetArmorRepairKeyword_Papyrus(std::monostate, RE::TESObjectARMO* myArmor)
{
	return ARMOUtilities::GetArmorRepairKeyword(myArmor);
}

bool F4CW::DegradationPapyrus::CanArmorBeRepairedWithOther_Papyrus(std::monostate, RE::TESObjectARMO* armor1, RE::TESObjectARMO* armor2)
{
	return ARMOUtilities::CanArmorBeRepairedWithOther(armor1, armor2);
}

void F4CW::DegradationPapyrus::UpdateArmorRefStats_Papyrus(std::monostate, RE::TESObjectREFR* armorRef)
{
	InitializeArmorCondition(armorRef);
}

// WEAPON UTILS =======================================================================================================================

void F4CW::WPNUtilities::UpdateHUDCondition(ItemDegradation::WeaponConditionData myConditionData)
{
	RE::BSFixedString menuString("HUDMenu");
	if (RE::UI::GetSingleton()->GetMenuOpen(menuString)) {
		auto myHudMenu = RE::UI::GetSingleton()->GetMenu(menuString).get();
		Scaleform::GFx::Value myConditionValue[1];

		float conditionValue = GetWeaponConditionPercent(myConditionData);

		myConditionValue[0].SetElement(0, conditionValue);

		myHudMenu->uiMovie->Invoke("root.CWHUD_loader.content.SetCondition", nullptr, myConditionValue, 1);
	}
	// auto myMovieRoot = myHUDMenu->uiMovie->asMovieRoot;

	
}

float F4CW::WPNUtilities::CalculateSkillBonusFromActor(ItemDegradation::WeaponConditionData myConditionData)
{
	using namespace ItemDegradation;

	float actorSkillValue = 1.0;

	RE::Actor * actor = myConditionData.actor;

	RE::TESObjectWEAP* myWeapon = static_cast<RE::TESObjectWEAP*>(myConditionData.Form);

	if (myWeapon->weaponData.skill != nullptr)
	{
		if (actor == nullptr)
		{
			//	use player instead
			actor = RE::PlayerCharacter::GetSingleton();
		}

		actorSkillValue = (GetAVValue(actor, myWeapon->weaponData.skill) / 100);
	}

	if (actorSkillValue > 1.0)
	{
		if (actor == RE::PlayerCharacter::GetSingleton())
		{
			if (!bIgnoreMaxSkillsPlayer)
			{
				actorSkillValue = 1.0;
			}
		}
		else
		{
			if (!bIgnoreMaxSkillsNPC)
			{
				actorSkillValue = 1.0;
			}
		}
	}

	float result = (fDamageSkillBase + fDamageSkillMult * actorSkillValue);

	//REX::WARN("Calculate Skill Bonus From Actor = %f", result);

	return result;
}

float F4CW::WPNUtilities::CalculateUpdatedDamageValue(float baseDamage, float minimum, float conditionPercent, float skillBonus)
{
	return (baseDamage * (minimum + conditionPercent * (1 - minimum)) * skillBonus);
}
/*
void F4CW::WPNUtilities::UpdateWeaponStats(ItemDegradation::WeaponConditionData myConditionData)
{

	if (myConditionData.instance == nullptr)
	{
		return;
	}

	RE::TESObjectWEAP * baseWPNForm = static_cast<RE::TESObjectWEAP*>(myConditionData.Form);
	RE::TESObjectWEAP::InstanceData* myInstance = myConditionData.instance;

	//	Ignore No Item Degredation keyworded items && crWeaponRanged items
	//if (baseWPNForm->keyword.HasKeyword(noItemDegredationKeyword) || baseWPNForm->keyword.HasKeyword(crWeaponRanged))
	if (WeaponHasKeyword(baseWPNForm, Shared::noDegradation) || WeaponHasKeyword(baseWPNForm, Shared::crWeaponRanged))
	{
		return;
	}

	float currentCondition = GetWeaponConditionPercent(myConditionData);

	if (currentCondition == -1.0)
	{
		//	This will likely happen with weapons not set up for Weapon Condition (ignoring Base game and DLCs for now)
		//	try and initialise condition anyway as a failsafe
		InitializeWeaponCondition(myConditionData);

		currentCondition = GetWeaponConditionPercent(myConditionData);

		if (currentCondition == -1.0)
		{
			std::string weaponFormIDString = GetFormIDAsString(baseWPNForm->formID);
			if (!IsFormIDStringBaseGame(weaponFormIDString))
				LOG_WARNING(std::format("CONDITION: Trying to update a weapon (%s) that has no Weapon Condition set up. Please set up in the Creation Kit!", weaponFormIDString.c_str()));
			return;
		}
	}

	float baseValue = baseWPNForm->weaponData.value;

	//Value
	float newValue = (baseValue * (pow(currentCondition, 1.5)));
	myInstance->value = newValue;

	//Damage
	float baseDamage = baseWPNForm->weaponData.attackDamage;
	float minimum = 0.66;

	//Rate of Fire
	if (!IsMeleeWeapon(myConditionData))
	{
		myInstance->attackDelaySec = CalculateUpdatedRateOfFireValue(myConditionData, currentCondition);
	}

	switch (myInstance->flags.get())
	{
	case RE::WEAPON_FLAGS::kAutomatic:
		minimum = 0.54;
		break;
	default:
		switch (myInstance->type.get())
		{
		case RE::WEAPON_TYPE::kHandToHand:
		case RE::WEAPON_TYPE::kOneHandAxe:
		case RE::WEAPON_TYPE::kOneHandDagger:
		case RE::WEAPON_TYPE::kOneHandMace:
		case RE::WEAPON_TYPE::kOneHandSword:
		case RE::WEAPON_TYPE::kStaff:
		case RE::WEAPON_TYPE::kTwoHandAxe:
		case RE::WEAPON_TYPE::kTwoHandSword:
			minimum = 0.5;
			break;
		default:
			minimum = 0.66;
			break;
		}
		break;
	}

	const char* actorName;

	if (myConditionData.actor)
	{
		if (myConditionData.actor == RE::PlayerCharacter::GetSingleton())
		{
			actorName = "Player";
		}
		else
		{
			actorName = myConditionData.actor->GetDisplayFullName();
		}
	}
	else
	{
		actorName = "NULL";
	}

	if (baseWPNForm->weaponData.damageTypes != nullptr)
	{
		for (int i = 0; i < baseWPNForm->weaponData.damageTypes->size(); i++)
		{
			if (baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthMaxDMGT && baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthStartingDMGT)
			{
				float baseValue = baseWPNForm->weaponData.damageTypes->at(i).second.f;
				float newValue = CalculateUpdatedDamageValue(baseValue, minimum, currentCondition, CalculateSkillBonusFromActor(myConditionData));
				myInstance->damageTypes->at(i).second.f = newValue;
				//REX::WARN("Actor: %s, Extra Damage Type: Base Damage = %f, New Damage = %f", actorName, baseValue, newValue);
			}
		}
	}

	float newDamage = CalculateUpdatedDamageValue(baseDamage, minimum, currentCondition, CalculateSkillBonusFromActor(myConditionData));
	myInstance->attackDamage = newDamage;
	//REX::WARN("Actor: %s, Condtion: %f, Base Damage Type: Base Damage = %f, New Damage = %f", actorName, currentCondition, baseDamage, newDamage);
}
*/
float F4CW::WPNUtilities::GetWeaponDamage(ItemDegradation::WeaponConditionData myConditionData)
{
	if (myConditionData.instance == nullptr)
	{
		return 0.0;
	}

	RE::TESObjectWEAP* baseWPNForm = static_cast<RE::TESObjectWEAP*>(myConditionData.Form);
	RE::TESObjectWEAP::InstanceData* myInstance = myConditionData.instance;

	float currentCondition = GetWeaponConditionPercent(myConditionData);

	float baseValue = baseWPNForm->weaponData.value;

	//Value
	float newValue = (baseValue * (pow(currentCondition, 1.5)));
	myInstance->value = newValue;

	//Damage
	float baseDamage = baseWPNForm->weaponData.attackDamage;
	float minimum = 0.66;

	//Rate of Fire
	if (!IsMeleeWeapon(myConditionData))
	{
		myInstance->attackDelaySec = CalculateUpdatedRateOfFireValue(myConditionData, currentCondition);
	}

	switch (myInstance->flags.get())
	{
	case RE::WEAPON_FLAGS::kAutomatic:
		minimum = 0.54;
		break;
	default:
		switch (myInstance->type.get())
		{
		case RE::WEAPON_TYPE::kHandToHand:
		case RE::WEAPON_TYPE::kOneHandAxe:
		case RE::WEAPON_TYPE::kOneHandDagger:
		case RE::WEAPON_TYPE::kOneHandMace:
		case RE::WEAPON_TYPE::kOneHandSword:
		case RE::WEAPON_TYPE::kStaff:
		case RE::WEAPON_TYPE::kTwoHandAxe:
		case RE::WEAPON_TYPE::kTwoHandSword:
			minimum = 0.5;
			break;
		default:
			minimum = 0.66;
			break;
		}
		break;
	}

	if (baseWPNForm->weaponData.damageTypes != nullptr)
	{
		for (int i = 0; i < baseWPNForm->weaponData.damageTypes->size(); i++)
		{
			if (baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthMaxDMGT && baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthStartingDMGT)
			{
				float baseValue = baseWPNForm->weaponData.damageTypes->at(i).second.f;
				float newValue = CalculateUpdatedDamageValue(baseValue, minimum, currentCondition, CalculateSkillBonusFromActor(myConditionData));
				return newValue;
				//myInstance->damageTypes->entries[i].value = newValue;
				//REX::WARN("Extra Damage Type: Base Damage = %f, New Damage = %f", baseValue, newValue);
			}
		}
	}

	float newDamage = CalculateUpdatedDamageValue(baseDamage, minimum, currentCondition, CalculateSkillBonusFromActor(myConditionData));
	return newDamage;
	//myInstance->baseDamage = newDamage;
	//REX::WARN("Base Damage Type: Base Damage = %f, New Damage = %f", baseDamage, newDamage);
}

float F4CW::WPNUtilities::CalculateUpdatedRateOfFireValue(ItemDegradation::WeaponConditionData myConditionData, float currentCondition)
{
	return CalculateUpdatedRateOfFireValue(myConditionData.Form, currentCondition);
}

float F4CW::WPNUtilities::CalculateUpdatedRateOfFireValue(RE::TESForm* weaponForm, float currentCondition)
{
	using namespace ItemDegradation;
	// the calculation here is a bit weird in FO4, Fire Rate is linked to attack delay, with attack delay, the lower it is the higher the Fire Rate
		// instead of multiplying the base attack delay by a set amount instead we divide to make attack delay longer and in turn Fire Rate lower

	RE::TESObjectWEAP* baseWPNForm = static_cast<RE::TESObjectWEAP*>(weaponForm);

	float baseROF = baseWPNForm->weaponData.attackDelaySec;	//	unkC0 == fAttackSeconds;

	float result = baseROF;

	//REX::WARN("%s: Base Attack Delay = %f", baseWPNForm->GetFullName(), baseROF);

	if (currentCondition <= 1.0 && currentCondition > 0.9)
	{
		result = (baseROF / fWeaponConditionRateOfFire10->GetFloat());
	}
	else if (currentCondition <= 0.9 && currentCondition > 0.8)
	{
		result = (baseROF / fWeaponConditionRateOfFire9->GetFloat());
	}
	else if (currentCondition <= 0.8 && currentCondition > 0.7)
	{
		result = (baseROF / fWeaponConditionRateOfFire8->GetFloat());
	}
	else if (currentCondition <= 0.7 && currentCondition > 0.6)
	{
		result = (baseROF / fWeaponConditionRateOfFire7->GetFloat());
	}
	else if (currentCondition <= 0.6 && currentCondition > 0.5)
	{
		result = (baseROF / fWeaponConditionRateOfFire6->GetFloat());
	}
	else if (currentCondition <= 0.5 && currentCondition > 0.4)
	{
		result = (baseROF / fWeaponConditionRateOfFire5->GetFloat());
	}
	else if (currentCondition <= 0.4 && currentCondition > 0.3)
	{
		result = (baseROF / fWeaponConditionRateOfFire4->GetFloat());
	}
	else if (currentCondition <= 0.3 && currentCondition > 0.2)
	{
		result = (baseROF / fWeaponConditionRateOfFire3->GetFloat());
	}
	else if (currentCondition <= 0.2 && currentCondition > 0.1)
	{
		result = (baseROF / fWeaponConditionRateOfFire2->GetFloat());
	}
	else if (currentCondition <= 0.1 && currentCondition > 0.0)
	{
		result = (baseROF / fWeaponConditionRateOfFire1->GetFloat());
	}

	//REX::WARN("%s: Updated Attack Delay = %f", baseWPNForm->GetFullName(), result);

	return result;
}

bool F4CW::WPNUtilities::IsMeleeWeapon(ItemDegradation::WeaponConditionData myConditionData)
{
	return IsMeleeWeapon(myConditionData.instance->type.get());
}

bool F4CW::WPNUtilities::IsMeleeWeapon(RE::WEAPON_TYPE weaponType)
{
	bool result = false;

	switch (weaponType)
	{
	case RE::WEAPON_TYPE::kHandToHand:
	case RE::WEAPON_TYPE::kOneHandAxe:
	case RE::WEAPON_TYPE::kOneHandDagger:
	case RE::WEAPON_TYPE::kOneHandMace:
	case RE::WEAPON_TYPE::kOneHandSword:
	case RE::WEAPON_TYPE::kStaff:
	case RE::WEAPON_TYPE::kTwoHandAxe:
	case RE::WEAPON_TYPE::kTwoHandSword:
		result = true;
		break;
	default:
		result = false;
		break;
	}

	return result;
}


/*
RE::TESObjectWEAP::InstanceData* F4CW::WPNUtilities::CastInstanceData(RE::TBO_InstanceData * myInstanceData)
{
	return nullptr;
}
*/

RE::BGSKeyword* F4CW::WPNUtilities::GetWeaponRepairKeyword(RE::TESObjectWEAP* myWeapon)
{
	if (!myWeapon)
		return nullptr;

	RepairListMap weaponRepairListMap = F4CWSerialization::GetWeaponRepairList();
	
	for (auto it = weaponRepairListMap.begin(); it != weaponRepairListMap.end(); ++it)
	{
		if (myWeapon->HasKeyword(it->second))
			return it->second;
	}

	return nullptr;
}

RE::BGSListForm* F4CW::WPNUtilities::GetLinkedWeaponRepairListFromKeyword(RE::BGSKeyword* myKeyword)
{
	if (!myKeyword)
		return nullptr;

	RepairListMap weaponRepairListMap = F4CWSerialization::GetWeaponRepairList();

	for (auto it = weaponRepairListMap.begin(); it != weaponRepairListMap.end(); ++it)
	{
		if (it->second == myKeyword)
			return it->first;
	}

	return nullptr;
}

RE::BGSKeyword* F4CW::WPNUtilities::GetLinkedWeaponRepairKeywordFromFormList(RE::BGSListForm* myList)
{
	if (!myList)
		return nullptr;

	RepairListMap weaponRepairListMap = F4CWSerialization::GetWeaponRepairList();

	for (auto it = weaponRepairListMap.begin(); it != weaponRepairListMap.end(); ++it)
	{
		if (it->first == myList)
			return it->second;
	}

	return nullptr;
}

bool F4CW::WPNUtilities::CanWeaponBeRepairedWithOther(RE::TESObjectWEAP* weapon1, RE::TESObjectWEAP* weapon2)
{
	if (weapon1 && weapon2)
	{
		RE::BGSKeyword* myArmorRepairKeyword = GetWeaponRepairKeyword(weapon1);

		if (myArmorRepairKeyword)
		{
			RE::BGSListForm* myArmorRepairList = GetLinkedWeaponRepairListFromKeyword(myArmorRepairKeyword);

			for (const RE::TESForm* weapon : myArmorRepairList->arrayOfForms) {
				if (weapon == weapon2)
					return true;
			}
		}

		else if (weapon1 == weapon2)
			return true;
	}

	return false;
}

// ARMOR UTILS =======================================================================================================================

RE::BGSKeyword* F4CW::ARMOUtilities::GetPowerArmorTypeKeyword()
{
	return ItemDegradation::armorTypePower;
}

RE::BGSKeyword* F4CW::ARMOUtilities::GetArmorRepairKeyword(RE::TESObjectARMO * myArmor)
{
	if (!myArmor)
		return nullptr;

	RepairListMap armorRepairListMap = F4CWSerialization::GetArmorRepairList();

	for (auto it = armorRepairListMap.begin(); it != armorRepairListMap.end(); ++it)
	{
		if (myArmor->HasKeyword(it->second))
			return it->second;
	}

	return nullptr;
}

RE::BGSListForm* F4CW::ARMOUtilities::GetLinkedArmorRepairListFromKeyword(RE::BGSKeyword* myKeyword)
{
	if (!myKeyword)
		return nullptr;

	RepairListMap armorRepairListMap = F4CWSerialization::GetArmorRepairList();

	for (auto it = armorRepairListMap.begin(); it != armorRepairListMap.end(); ++it)
	{
		if (it->second == myKeyword)
			return it->first;
	}

	return nullptr;
}

RE::BGSKeyword* F4CW::ARMOUtilities::GetLinkedArmorRepairKeywordFromFormList(RE::BGSListForm* myList)
{
	if (!myList)
		return nullptr;

	RepairListMap armorRepairListMap = F4CWSerialization::GetArmorRepairList();

	for (auto it = armorRepairListMap.begin(); it != armorRepairListMap.end(); ++it)
	{
		if (it->first == myList)
			return it->second;
	}

	return nullptr;
}

bool F4CW::ARMOUtilities::CanArmorBeRepairedWithOther(RE::TESObjectARMO* armor1, RE::TESObjectARMO* armor2)
{
	if (armor1 && armor2)
	{
		RE::BGSKeyword* myArmorRepairKeyword = GetArmorRepairKeyword(armor1);

		if (myArmorRepairKeyword)
		{
			RE::BGSListForm* myArmorRepairList = GetLinkedArmorRepairListFromKeyword(myArmorRepairKeyword);

			for (const RE::TESForm* armour : myArmorRepairList->arrayOfForms) {
				if (armour == armor2)
					return true;
			}
		}

		else if (armor1 == armor2)
			return true;
	}

	return false;
}
/*


void F4CW::ARMOUtilities::UpdateArmorStats(ItemDegradation::ArmorConditionData myConditionData)
{
	if (myConditionData.instance == nullptr)
	{
		return;
	}

	RE::TESObjectARMO* baseARMOForm = static_cast<RE::TESObjectARMO*>(myConditionData.Form);
	RE::TESObjectARMO::InstanceData* myInstance = myConditionData.instance;

	//	Ignore No Item Degredation keyworded items
	if (ArmorHasKeyword(baseARMOForm, Shared::noDegradation))
	{
		return;
	}

	//	Check if has Armor Body Part Keywords
	bool bHasBodyPartKeyword = false;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartChest))
		bHasBodyPartKeyword = true;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartHead))
		bHasBodyPartKeyword = true;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartLeftArm))
		bHasBodyPartKeyword = true;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartLeftLeg))
		bHasBodyPartKeyword = true;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartRightArm))
		bHasBodyPartKeyword = true;

	if (ArmorHasKeyword(baseARMOForm, ItemDegradation::armorBodyPartRightLeg))
		bHasBodyPartKeyword = true;

	if (!bHasBodyPartKeyword)
	{
		std::string armorFormIDString = GetFormIDAsString(baseARMOForm->formID);
		if (!IsFormIDStringBaseGame(armorFormIDString))
			REX::WARN("CONDITION: Trying to update a armor (%s) that has no Armor Condition Keywords. Please set up in the Creation Kit!", armorFormIDString.c_str());
		return;
	}

	float currentCondition = GetArmorConditionPercent(myConditionData);

	if (currentCondition == -1.0)
	{
		//	This will likely happen with armors not set up for Armor Condition (ignoring Base game and DLCs for now)
		//	try and initialise condition anyway as a failsafe
		InitializeArmorCondition(myConditionData);

		if (currentCondition == -1.0)
		{
			std::string armorFormIDString = GetFormIDAsString(baseARMOForm->formID);
			if (!IsFormIDStringBaseGame(armorFormIDString))
				REX::WARN("CONDITION: Trying to update a armor (%s) that has no Armor Condition Health. Please set up in the Creation Kit!", armorFormIDString.c_str());
			return;
		}
	}

	float baseValue = (float) baseARMOForm->armorData.value;

	//	Value
	float newValue = (baseValue * (pow(currentCondition, 1.5)));
	myInstance->value = (std::uint32_t)newValue;

	//	DR
	float maxDR = (float)baseARMOForm->armorData.rating;
	float newDR = ((0.2 * maxDR) + (currentCondition * 100) * ((0.8 * maxDR) / 100));
	//REX::WARN("Armor (%s): Max DR = %.4f, Health = %.4f%%, Current DR = %.4f", baseARMOForm->GetFullName(), maxDR, (currentCondition * 100), newDR);
	if (newDR < 1.0)
	{
		// minimum DR should be 1 so we don't have 0 DR on an item
		newDR = 1.0;
	}
	myInstance->rating = (std::uint16_t)newDR;
}


void F4CW::ARMOUtilities::UpdateArmorStatsOnHit(RE::Actor* actor, std::uint32_t eDamageLimb, std::uint32_t eIncomingDamageType, float fDamage)
{
	RE::BGSKeyword* checkKeyword = nullptr;

	if (actor != GetPlayer())
	{
		//	Ignoring any NPCs
		return;
	}

	bool isPowerArmor = WornHasKeyword(actor, ItemDegradation::armorTypePower);
	bool isMelee = (eIncomingDamageType == RE::damage);
	bool isUnarmed = (eIncomingDamageType == IncomingDamageType::kDamage_Unarmed || eIncomingDamageType == IncomingDamageType::kDamage_Unknown);

	switch (static_cast<RE::BGSBodyPartDefs::LIMB_ENUM>(eDamageLimb))
	{
	case RE::BGSBodyPartDefs::LIMB_ENUM::kHead1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kHead2:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kEye1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kBrain:
	{
		//_MESSAGE("Hit in Head");
		checkKeyword = ItemDegradation::armorBodyPartHead;
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kTorso:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kPelvis:
	{
		//_MESSAGE("Hit in Body");
		checkKeyword = ItemDegradation::armorBodyPartChest;
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftArm1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftArm2:
	{
		//_MESSAGE("Hit in Left Arm");
		if (isPowerArmor)
		{
			checkKeyword = ItemDegradation::armorBodyPartLeftArm;
		}
		else
		{
			checkKeyword = ItemDegradation::armorBodyPartChest;
		}
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightArm1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightArm2:
	{
		//_MESSAGE("Hit in Right Arm");
		if (isPowerArmor)
		{
			checkKeyword = ItemDegradation::armorBodyPartRightArm;
		}
		else
		{
			checkKeyword = ItemDegradation::armorBodyPartChest;
		}
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftLeg1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftLeg2:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftLeg3:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kLeftFoot:
	{
		//_MESSAGE("Hit in Left Leg");
		if (isPowerArmor)
		{
			checkKeyword = ItemDegradation::armorBodyPartLeftLeg;
		}
		else
		{
			checkKeyword = ItemDegradation::armorBodyPartChest;
		}
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightLeg1:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightLeg2:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightLeg3:
	case RE::BGSBodyPartDefs::LIMB_ENUM::kRightFoot:
	{
		//_MESSAGE("Hit in Right Leg");
		if (isPowerArmor)
		{
			checkKeyword = ItemDegradation::armorBodyPartRightLeg;
		}
		else
		{
			checkKeyword = ItemDegradation::armorBodyPartChest;
		}
		break;
	}
	case RE::BGSBodyPartDefs::LIMB_ENUM::kWeapon:

	default:
	{
		//_MESSAGE("Unknown Hit Type, Assuming Body");
		checkKeyword = ItemDegradation::armorBodyPartChest;
		break;
	}
	}

	if (checkKeyword)
	{
		RE::BGSInventoryItem invItem = GetEquippedArmorInventoryItemBasedOnKeyword(actor, checkKeyword);

		if (invItem.for)
		{
			//_MESSAGE("Got Form 0x%08X for Armor Degredation", invItem.form->formID);
			ItemDegradation::ArmorConditionData conditionData = ItemDegradation::ArmorConditionData(actor, invItem.form, invItem.stack->extraData);
			if (!isUnarmed)
			{
				ArmorDegrade degrade{};
				degrade.fDamageDealt = fDamage;
				degrade.isMelee = isMelee;
				ModArmorCondition(conditionData, degrade);
			}
		}
	}
}
*/