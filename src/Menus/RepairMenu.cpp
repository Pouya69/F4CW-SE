#include "RepairMenu.h"
#include "../GameForms.h"
#include "../skills.h"
#include <cmath>
#include <cstdint>
#include <RE/A/Actor.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectMISC.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/W/WEAPON_FLAGS.h>
#include <RE/W/WEAPON_TYPE.h>
#include <ItemDegradation.h>
#include <WinBase.h>
#include "../debugLog.h"
#include "../Scaleform_F4CW.h"
#include <Windows.h>
#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BGSKeyword.h>
#include <RE/B/BGSListForm.h>
#include <RE/B/BSFixedString.h>
#include <RE/B/BSGFxShaderFXTarget.h>
#include <RE/B/BSInputEventUser.h>
#include <RE/B/BSScaleformManager.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <RE/B/BSTArray.h>
#include <RE/B/BS_BUTTON_CODE.h>
#include <RE/B/ButtonEvent.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <RE/E/ExtraDataList.h>
#include <RE/E/EXTRA_DATA_TYPE.h>
#include <RE/G/GameScript.h>
#include <RE/H/HUDColorTypes.h>
#include <RE/I/IMenu.h>
#include <RE/M/MenuControls.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESForm.h>
#include <RE/U/UI.h>
#include <RE/U/UIMessageQueue.h>
#include <RE/U/UI_DEPTH_PRIORITY.h>
#include <RE/U/UI_MENU_FLAGS.h>
#include <RE/U/UI_MESSAGE_TYPE.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Scaleform/G/GFx_Value.h>
#include <REX/LOG.h>
#include <IGlobalConfig.h>
#include <Shared.h>

using namespace F4CW_Menus::RepairMenu;

bool bHideWorkshopTab = false;

RE::Actor* myCurrentVendor;
int myVendorSkill;
int myPlayerCaps;

RE::TESObjectMISC * caps;

float fRepairMax = 2.0;
float fRepairMin = 0.5;
float fRepairScavengeMult = 0.05;

int iCurrentRepairMenuType = kRepairType_Unknown;
InvObject currentPipboyRepairObject;


float CalculateNewDamageIfRepaired(F4CW::ItemDegradation::WeaponConditionData weaponData, float newMaxCondition)
{
	
	RE::TESObjectWEAP* baseWPNForm = static_cast<RE::TESObjectWEAP*>(weaponData.Form);

	float baseDamage = baseWPNForm->weaponData.attackDamage;

	float minimum = 0.66;

	std::uint32_t flags = weaponData.instance->flags.underlying();

	if (flags & (std::uint32_t) RE::WEAPON_FLAGS::kAutomatic)
		minimum = 0.54;
	else if (weaponData.instance->type.underlying() & (std::uint32_t)RE::WEAPON_TYPE::kTwoHandSword)
		minimum = 0.5;

	if (baseWPNForm->weaponData.damageTypes != nullptr)
	{
		for (int i = 0; i < baseWPNForm->weaponData.damageTypes->size(); i++)
		{
			if (baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthMaxDMGT && baseWPNForm->weaponData.damageTypes->at(i).first != ItemDegredationForms.weaponConditionHealthStartingDMGT)
			{
				float baseValue = baseWPNForm->weaponData.damageTypes->at(i).second.f;
				float newValue = F4CW::WPNUtilities::CalculateUpdatedDamageValue(baseValue, minimum, newMaxCondition, F4CW::WPNUtilities::CalculateSkillBonusFromActor(weaponData));
				return newValue;
			}
		}
	}

	float newDamage = F4CW::WPNUtilities::CalculateUpdatedDamageValue(baseDamage, minimum, newMaxCondition, F4CW::WPNUtilities::CalculateSkillBonusFromActor(weaponData));
	return newDamage;
	
}

float CalculateNewDRIfRepaired(F4CW::ItemDegradation::ArmorConditionData armorData, float newMaxCondition)
{
	RE::TESObjectARMO* baseARMOForm = static_cast<RE::TESObjectARMO*>(armorData.Form);

	float baseDR = baseARMOForm->armorData.rating;

	float maxDR = (float)baseARMOForm->armorData.rating;
	float newDR = ((0.2 * maxDR) + (newMaxCondition * 100) * ((0.8 * maxDR) / 100));
	if (newDR < 1.0)
	{
		// minimum DR should be 1 so we don't have 0 DR on an item
		newDR = 1.0;
	}

	return newDR;
}

int CalculateCostToRepairItem(F4CW::ItemDegradation::WeaponConditionData weaponData, float newMaxCondition)
{
	RE::TESObjectWEAP* myBaseWeapon = static_cast<RE::TESObjectWEAP*>(weaponData.Form);

	float baseValue = myBaseWeapon->weaponData.value;

	float maxValue = (baseValue * (pow(newMaxCondition, 1.5)));
	float currentValue = weaponData.instance->value;

	return (int)(2 * (maxValue - currentValue));
}

int CalculateCostToRepairItem(F4CW::ItemDegradation::ArmorConditionData armorData, float newMaxCondition)
{
	RE::TESObjectARMO* myBaseArmor = static_cast<RE::TESObjectARMO*>(armorData.Form);

	float baseValue = (float)myBaseArmor->armorData.value;

	float maxValue = (baseValue * (pow(newMaxCondition, 1.5)));
	float currentValue = armorData.instance->value;

	return (int)(2 * (maxValue - currentValue));
}

float CalculateMaxRepairCondition(int vendorSkill)
{
	float maxRepairCondition = (float)(0.4 + (0.006 * vendorSkill));

	return maxRepairCondition;
}

float CalculateMaxRepairConditionPlayer(int playerSkill)
{
	float maxRepairCondition = (float)(0.4 + (0.006 * playerSkill));

	return maxRepairCondition;
}

RepairMenuEntry PopulateWeaponEntryVendor(int inventoryIndex, RE::TESForm* myBaseForm, RE::ExtraDataList* myExtraData)
{
	RepairMenuEntry myEntry;

	auto player = RE::PlayerCharacter::GetSingleton();

	// RE::TESForm* myEquippedWeapon = F4CW::WPNUtilities::GetEquippedWeaponForm(player);

	F4CW::ItemDegradation::WeaponConditionData weaponData(player, myBaseForm, myExtraData);

	bool isRepairable = true;
	bool isEquipped = InventoryUtils::GetInventoryItemByIndex(inventoryIndex)->stackData->flags.underlying() & 7;

	float currentWeaponCondition = F4CW::GetWeaponConditionPercent(weaponData);
	float currentWeaponDamage = F4CW::WPNUtilities::GetWeaponDamage(weaponData);

	float maxRepairWeaponCondition = CalculateMaxRepairCondition(myVendorSkill);
	float maxRepairWeaponDamage = CalculateNewDamageIfRepaired(weaponData, maxRepairWeaponCondition);
	int costToRepair = CalculateCostToRepairItem(weaponData, maxRepairWeaponCondition);

	bool playerCanAfford = !(costToRepair > myPlayerCaps);

	if (maxRepairWeaponCondition <= currentWeaponCondition)
	{
		isRepairable = false;
	}

	std::string weaponName;

	if (!playerCanAfford || !isRepairable)
	{
		weaponName += "<font color=\'#838383\'>";
		weaponName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
		weaponName += "</font>";
	}
	else
	{
		weaponName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
	}

	const char* weaponSkillName;

	if (weaponData.instance->skill)
	{
		weaponSkillName = weaponData.instance->skill->GetFullName();
	}
	else
	{
		weaponSkillName = "null";
	}

	//myEntry.ItemCardInfoList = &myItemCard;
	myEntry.text = weaponName;
	myEntry.handleID = InventoryUtils::GetHandleIDByIndex(inventoryIndex);
	myEntry.stackID = InventoryUtils::GetStackIDByIndex(inventoryIndex);
	myEntry.condition = currentWeaponCondition;
	myEntry.newCondition = maxRepairWeaponCondition;
	myEntry.damage = currentWeaponDamage;
	myEntry.newDamage = maxRepairWeaponDamage;
	myEntry.equipped = isEquipped;
	myEntry.repairable = isRepairable;
	myEntry.canAfford = playerCanAfford;
	myEntry.repairCost = costToRepair;
	myEntry.skillName = weaponSkillName;
	myEntry.hasDR = false;

	return myEntry;
}

PipboyRepairMenuEntry PopulateWeaponEntryPipboy(int inventoryIndex, RepairObject baseObject, RepairObject otherObject)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	PipboyRepairMenuEntry myEntry;

	// RE::TESForm* myEquippedWeapon = F4CW::WPNUtilities::GetEquippedWeaponForm(player);

	F4CW::ItemDegradation::WeaponConditionData weaponDataBase(player, baseObject.form, baseObject.extraData);
	F4CW::ItemDegradation::WeaponConditionData weaponDataOther(player, otherObject.form, otherObject.extraData);

	baseObject.equipped = InventoryUtils::GetInventoryItemByIndex(baseObject.index)->stackData->flags.underlying() & 7;
	otherObject.equipped = InventoryUtils::GetInventoryItemByIndex(otherObject.index)->stackData->flags.underlying() & 7;

	bool isRepairable = true;
	bool isEquipped = InventoryUtils::GetInventoryItemByIndex(inventoryIndex)->stackData->flags.underlying() & 7;

	float currentWeaponCondition = F4CW::GetWeaponConditionPercent(weaponDataBase);
	float currentWeaponDamage = F4CW::WPNUtilities::GetWeaponDamage(weaponDataBase);

	float otherWeaponCondition = F4CW::GetWeaponConditionPercent(weaponDataOther);
	float otherWeaponDamage = F4CW::WPNUtilities::GetWeaponDamage(weaponDataOther);

	float maxRepairWeaponCondition = CalculateMaxRepairConditionPlayer((int)GetPlayerAVValue(GetSkillByName("Repair")));
	float maxRepairWeaponDamage = CalculateNewDamageIfRepaired(weaponDataBase, maxRepairWeaponCondition);
	//int costToRepair = CalculateCostToRepairItem(weaponDataBase, maxRepairWeaponCondition);

	//bool playerCanRepair = !(costToRepair > myPlayerCaps);

	if (maxRepairWeaponCondition <= currentWeaponCondition)
	{
		isRepairable = false;
	}

	std::string weaponName;

	if (!isRepairable)
	{
		weaponName += "<font color=\'#838383\'>";
		weaponName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
		weaponName += "</font>";
	}
	else
	{
		weaponName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
	}

	const char* weaponSkillName;

	if (weaponDataBase.instance->skill)
	{
		weaponSkillName = weaponDataBase.instance->skill->GetFullName();
	}
	else
	{
		weaponSkillName = "null";
	}

	myEntry.text = weaponName;
	myEntry.handleID = InventoryUtils::GetHandleIDByIndex(inventoryIndex);
	myEntry.stackID = InventoryUtils::GetStackIDByIndex(inventoryIndex);
	myEntry.condition = currentWeaponCondition;
	myEntry.otherCondition = otherWeaponCondition;
	myEntry.newCondition = maxRepairWeaponCondition;
	myEntry.damage = currentWeaponDamage;
	myEntry.otherDamage = otherWeaponDamage;
	myEntry.newDamage = maxRepairWeaponDamage;
	myEntry.equipped = isEquipped;
	myEntry.repairable = isRepairable;
	myEntry.canAfford = myEntry.repairable;
	myEntry.repairCost = 0;
	myEntry.skillName = weaponSkillName;
	myEntry.hasDR = false;

	return myEntry;
}

RepairMenuEntry PopulateArmorEntryVendor(int inventoryIndex, RE::TESForm* myBaseForm, RE::ExtraDataList* myExtraData)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	RepairMenuEntry myEntry{};

	F4CW::ItemDegradation::ArmorConditionData armorData(player, myBaseForm, myExtraData);

	bool isRepairable = true;
	bool isEquipped = InventoryUtils::GetInventoryItemByIndex(inventoryIndex)->stackData->flags.underlying() & 7;

	if (!armorData.instance)
	{
		return myEntry;
	}

	F4CW::ARMOUtilities::UpdateArmorStats(armorData);

	float currentCondition = F4CW::GetArmorConditionPercent(armorData);
	float currentDR = (float)armorData.instance->rating;

	float maxRepairCondition = CalculateMaxRepairCondition(myVendorSkill);
	float maxRepairDR = CalculateNewDRIfRepaired(armorData, maxRepairCondition);
	int costToRepair = CalculateCostToRepairItem(armorData, maxRepairCondition);

	bool playerCanAfford = !(costToRepair > myPlayerCaps);

	if (maxRepairCondition <= currentCondition)
	{
		isRepairable = false;
	}

	std::string armorName;

	if (!playerCanAfford || !isRepairable)
	{
		armorName += "<font color=\'#838383\'>";
		armorName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
		armorName += "</font>";
	}
	else
	{
		armorName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
	}

	const char* armorSkillName = "null";

	//myEntry.ItemCardInfoList = &myItemCard;
	myEntry.text = armorName;
	myEntry.handleID = InventoryUtils::GetHandleIDByIndex(inventoryIndex);
	myEntry.stackID = InventoryUtils::GetStackIDByIndex(inventoryIndex);
	myEntry.condition = currentCondition;
	myEntry.newCondition = maxRepairCondition;
	myEntry.damage = currentDR;
	myEntry.newDamage = maxRepairDR;
	myEntry.equipped = isEquipped;
	myEntry.repairable = isRepairable;
	myEntry.canAfford = playerCanAfford;
	myEntry.repairCost = costToRepair;
	myEntry.skillName = armorSkillName;
	myEntry.hasDR = true;

	return myEntry;
}

PipboyRepairMenuEntry PopulateArmorEntryPipboy(int inventoryIndex, RepairObject baseObject, RepairObject otherObject)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	PipboyRepairMenuEntry myEntry;

	F4CW::ItemDegradation::ArmorConditionData armorDataBase(player, baseObject.form, baseObject.extraData);
	F4CW::ItemDegradation::ArmorConditionData armorDataOther(player, otherObject.form, otherObject.extraData);

	baseObject.equipped = InventoryUtils::GetInventoryItemByIndex(baseObject.index)->stackData->flags.underlying() & 7;
	otherObject.equipped = InventoryUtils::GetInventoryItemByIndex(otherObject.index)->stackData->flags.underlying() & 7;

	F4CW::ARMOUtilities::UpdateArmorStats(armorDataBase);
	F4CW::ARMOUtilities::UpdateArmorStats(armorDataOther);

	bool isRepairable = true;
	bool isEquipped = InventoryUtils::GetInventoryItemByIndex(inventoryIndex)->stackData->flags.underlying() & 7;

	if (!armorDataBase.instance || !armorDataOther.instance)
	{
		return myEntry;
	}

	float currentCondition = F4CW::GetArmorConditionPercent(armorDataBase);
	float currentDR = (float)armorDataBase.instance->rating;

	float otherCondition = F4CW::GetArmorConditionPercent(armorDataOther);
	float otherDR = (float)armorDataOther.instance->rating;

	float maxRepairCondition = CalculateMaxRepairConditionPlayer((int)GetPlayerAVValue(GetSkillByName("Repair")));
	float maxRepairDamage = CalculateNewDRIfRepaired(armorDataBase, maxRepairCondition);
	//int costToRepair = CalculateCostToRepairItem(armorDataBase, maxRepairCondition);

	//bool playerCanRepair = !(costToRepair > myPlayerCaps);

	if (maxRepairCondition <= currentCondition)
	{
		isRepairable = false;
	}

	std::string armorName;

	if (!isRepairable)
	{
		armorName += "<font color=\'#838383\'>";
		armorName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
		armorName += "</font>";
	}
	else
	{
		armorName += InventoryUtils::GetInventoryDisplayName(inventoryIndex);
	}

	const char* armorSkillName = "null";

	myEntry.text = armorName;
	myEntry.handleID = InventoryUtils::GetHandleIDByIndex(inventoryIndex);
	myEntry.stackID = InventoryUtils::GetStackIDByIndex(inventoryIndex);
	myEntry.condition = currentCondition;
	myEntry.otherCondition = otherCondition;
	myEntry.newCondition = maxRepairCondition;
	myEntry.damage = currentDR;
	myEntry.otherDamage = otherDR;
	myEntry.newDamage = maxRepairDamage;
	myEntry.equipped = isEquipped;
	myEntry.repairable = isRepairable;
	myEntry.canAfford = myEntry.repairable;
	myEntry.repairCost = 0;
	myEntry.skillName = armorSkillName;
	myEntry.hasDR = true;

	return myEntry;
}

//	Processes Player's entire WEAP and ARMO inventory

bool ProcessPlayerInventory(Scaleform::GFx::Value* myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	std::uint32_t inventoryItemCount = InventoryUtils::GetPipboyInventoryObjectCount();

	std::vector<RepairMenuEntry> RepairList, RepairIneligible;
	for (int i = 0; i < inventoryItemCount; i++)
	{
		RE::TESForm* form = InventoryUtils::GetInventoryFormByIndex(i);
		if (!form)
		{
			continue;
		}

		RE::ExtraDataList* extraData = InventoryUtils::GetExtraDataListByIndex(i);
		if (!extraData->HasType(RE::EXTRA_DATA_TYPE::kHealth))
			continue;
		
		switch (form->formType.get())
		{
		case RE::ENUM_FORM_ID::kWEAP:
		{
			RepairMenuEntry currentEntry = PopulateWeaponEntryVendor(i, form, extraData);

			if (currentEntry.text == "")
			{
				break;
			}

			if (!currentEntry.repairable || !currentEntry.canAfford)
			{
				RepairIneligible.emplace_back(currentEntry);
			}
			else
			{
				RepairList.emplace_back(currentEntry);
			}

			break;
		}
		case RE::ENUM_FORM_ID::kARMO:
		{
			RepairMenuEntry currentEntry = PopulateArmorEntryVendor(i, form, extraData);

			if (currentEntry.text == "")
			{
				break;
			}

			if (!currentEntry.repairable || !currentEntry.canAfford)
			{
				RepairIneligible.emplace_back(currentEntry);
			}
			else
			{
				RepairList.emplace_back(currentEntry);
			}

			break;
		}
		default:
			break;
		}
	}

	RepairList.insert(RepairList.end(), RepairIneligible.begin(), RepairIneligible.end());

	for (auto iter : RepairList)
	{
		Scaleform::GFx::Value entry;
		myMovieRoot->CreateObject(&entry);

		entry.SetMember("text", iter.text.c_str());
		entry.SetMember("handleID", iter.handleID);
		entry.SetMember("stackID", iter.stackID);
		entry.SetMember("condition", iter.condition);
		entry.SetMember("newCondition", iter.newCondition);
		entry.SetMember("damage", iter.damage);
		entry.SetMember("newDamage", iter.newDamage);
		entry.SetMember("equipped", iter.equipped);
		entry.SetMember("repairable", iter.repairable);
		entry.SetMember("canAfford", iter.canAfford);
		entry.SetMember("repairCost", iter.repairCost);
		//GFxUtilities::RegisterString(&entry, myMovieRoot, "text", iter.text.c_str());
		// entry.SetMember( "handleID", iter.handleID);
		// entry.SetMember( "stackID", iter.stackID);
		// entry.SetMember( "condition", iter.condition);
		// entry.SetMember( "newCondition", iter.newCondition);
		// entry.SetMember( "damage", iter.damage);
		// entry.SetMember( "newDamage", iter.newDamage);
		// entry.SetMember( "equipped", iter.equipped);
		// entry.SetMember( "repairable", iter.repairable);
		// entry.SetMember( "canAfford", iter.canAfford);
		// entry.SetMember( "repairCost", iter.repairCost);
		if (!iter.hasDR)
		{
			entry.SetMember("skillName", iter.skillName);
			// GFxUtilities::RegisterString(&entry, myMovieRoot, "skillName", iter.skillName);
		}
		entry.SetMember("hasDR", iter.hasDR);
		// entry.SetMember( "hasDR", iter.hasDR);

		myDestination->PushBack(entry);
	}

	return true;
}


bool ProcessPlayerInventorySpecific(Scaleform::GFx::Value * myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	std::uint32_t inventoryItemCount = InventoryUtils::GetPipboyInventoryObjectCount();

	const RE::BGSInventoryItem* myItem = InventoryUtils::GetInventoryItemByIndex(currentPipboyRepairObject.selectedIndex);

	RepairObject baseObject{};
	baseObject.form = myItem->object;
	baseObject.extraData = myItem->stackData->extra.get();
	baseObject.index = currentPipboyRepairObject.selectedIndex;

	if (!baseObject.form->formID == currentPipboyRepairObject.formID)
	{
		REX::WARN("ProcessPlayerInventorySpecific: Current Inventory Item Form does not match its Inventory Index");
		return false;
	}

	RE::TESObjectWEAP* myWeap;
	RE::TESObjectARMO* myArmor;
	RE::BGSKeyword* myRepairKeyword;
	RE::BGSListForm* myRepairFormList{};

	std::int32_t currentType;

	switch (baseObject.form->GetFormType())
	{
	case RE::ENUM_FORM_ID::kWEAP:
	{
		currentType = (std::int32_t) RE::ENUM_FORM_ID::kWEAP;
		myWeap = static_cast<RE::TESObjectWEAP*>(baseObject.form);
		myRepairKeyword = F4CW::WPNUtilities::GetWeaponRepairKeyword(myWeap);

		if (!myRepairKeyword)
		{
			REX::DEBUG("ProcessPlayerInventorySpecific: This weapon does NOT have a repair list keyword!");
			ShowNotification("This item cannot be repaired in your Pip-Boy.");
			return false;
		}

		myRepairFormList = F4CW::WPNUtilities::GetLinkedWeaponRepairListFromKeyword(myRepairKeyword);
		break;
	}
	case RE::ENUM_FORM_ID::kARMO:
	{
		currentType = (std::int32_t) RE::ENUM_FORM_ID::kARMO;
		myArmor = static_cast<RE::TESObjectARMO*>(baseObject.form);
		myRepairKeyword = F4CW::ARMOUtilities::GetArmorRepairKeyword(myArmor);

		if (!myRepairKeyword)
		{
			REX::DEBUG("ProcessPlayerInventorySpecific: This armor does NOT have a repair list keyword!");
			ShowNotification("This item cannot be repaired in your Pip-Boy.");
			return false;
		}

		myRepairFormList = F4CW::ARMOUtilities::GetLinkedArmorRepairListFromKeyword(myRepairKeyword);
		break;
	}
	default:
	{
		break;
	}
	}

	std::vector<PipboyRepairMenuEntry> RepairList, RepairIneligible;
	for (int i = 0; i < inventoryItemCount; i++)
	{
		//	same object, skip it
		if (baseObject.index == i)
		{
			continue;
		}

		RE::TESForm* form = InventoryUtils::GetInventoryFormByIndex(i);
		if (!form)
		{
			continue;
		}

		RE::ExtraDataList* extraData = InventoryUtils::GetExtraDataListByIndex(i);
		if (extraData->HasType(RE::EXTRA_DATA_TYPE::kHealth))
		{
			switch (form->GetFormType())
			{
			case RE::ENUM_FORM_ID::kWEAP:
			{
				if (!(currentType == (std::int32_t) RE::ENUM_FORM_ID::kWEAP))
				{
					continue;
				}

				if (!IsFormInList(form, myRepairFormList))
				{
					//	this weapon is not in repair list, skip
					continue;
				}

				RepairObject otherObject;
				otherObject.form = form;
				otherObject.extraData = extraData;
				otherObject.index = i;

				PipboyRepairMenuEntry currentEntry = PopulateWeaponEntryPipboy(i, baseObject, otherObject);

				if (!currentEntry.repairable || !currentEntry.canAfford)
				{
					RepairIneligible.emplace_back(currentEntry);
				}
				else
				{
					RepairList.emplace_back(currentEntry);
				}

				break;
			}
			case RE::ENUM_FORM_ID::kARMO:
			{
				if (!(currentType == (std::int32_t) RE::ENUM_FORM_ID::kARMO))
				{
					continue;
				}

				if (!IsFormInList(form, myRepairFormList))
				{
					//	this armor is not in repair list, skip
					continue;
				}

				RepairObject otherObject;
				otherObject.form = form;
				otherObject.extraData = extraData;
				otherObject.index = i;

				PipboyRepairMenuEntry currentEntry = PopulateArmorEntryPipboy(i, baseObject, otherObject);

				if (!currentEntry.repairable || !currentEntry.canAfford)
				{
					RepairIneligible.emplace_back(currentEntry);
				}
				else
				{
					RepairList.emplace_back(currentEntry);
				}

				break;
			}
			default:
				continue;
			}
		}
	}

	RepairList.insert(RepairList.end(), RepairIneligible.begin(), RepairIneligible.end());

	for (auto& iter : RepairList)
	{
		Scaleform::GFx::Value entry;
		myMovieRoot->CreateObject(&entry);
		entry.SetMember( "text", iter.text.c_str());
		entry.SetMember( "handleID", iter.handleID);
		entry.SetMember( "stackID", iter.stackID);
		entry.SetMember( "condition", iter.condition);
		entry.SetMember( "otherCondition", iter.otherCondition);
		entry.SetMember( "newCondition", iter.newCondition);
		entry.SetMember( "damage", iter.damage);
		entry.SetMember( "otherDamage", iter.otherDamage);
		entry.SetMember( "newDamage", iter.newDamage);
		entry.SetMember( "equipped", iter.equipped);
		entry.SetMember( "repairable", iter.repairable);
		entry.SetMember( "canAfford", iter.canAfford);
		entry.SetMember( "repairCost", iter.repairCost);
		if (!iter.hasDR)
		{
			entry.SetMember( "skillName", iter.skillName);
		}
		entry.SetMember( "hasDR", iter.hasDR);

		myDestination->PushBack(entry);
	}

	return true;
}


bool ProcessVendorRepairList(Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	Scaleform::GFx::Value arrArgs[5];
	myMovieRoot->CreateArray(&arrArgs[0]);

	ProcessPlayerInventory(&arrArgs[0], myMovieRoot);

	std::string tracea = "Sending ";
	tracea.append(std::to_string(arrArgs[0].GetArraySize()));
	tracea.append(" items to Repair Menu.");
	REX::DEBUG(tracea.c_str());

	arrArgs[1] = ((int)myVendorSkill);
	arrArgs[2] = ((int)myPlayerCaps);
	REX::DEBUG("RepairMenu: Items Ready, Sending to AS3");

	myMovieRoot->Invoke("root.Menu_mc.onRepairMenuStartVendor", nullptr, arrArgs, 3);

	return true;
}

bool ProcessPipboyRepairList(Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	Scaleform::GFx::Value arrArgs[2];
	myMovieRoot->CreateArray(&arrArgs[0]);

	if (!ProcessPlayerInventorySpecific(&arrArgs[0], myMovieRoot))
	{
		REX::DEBUG("RepairMenu Pipboy: Unable to Process current item. Closing Repair Menu.");
		CloseRepairMenu();
		// Scaleform::CloseMenu_Internal("RepairMenu");
		return false;
	}

	arrArgs[1] = ((int)GetPlayerAVValue(GetSkillByName("Repair")));
	REX::DEBUG(std::format("RepairMenu Pipboy: Player sent {} for Repair", currentPipboyRepairObject.text.c_str()).c_str());
	
	myMovieRoot->Invoke("root.Menu_mc.onRepairMenuStartPipboy", nullptr, arrArgs, 2);

	return true;
}

void F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuOpen(Scaleform::GFx::ASMovieRootBase* myMovieRoot, std::uint32_t playerCapsOverride)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	// if this is true then the player is using the pipboy to repair, if not then we assume the player is repairing via vendor
	if (iCurrentRepairMenuType == kRepairType_Weapons || iCurrentRepairMenuType == kRepairType_Apparel)
	{
		ProcessPipboyRepairList(myMovieRoot);
	}
	else
	{
		iCurrentRepairMenuType = kRepairType_Vendor;
		myVendorSkill = GetAVValue(myCurrentVendor, CW_Skills.Repair);
		myPlayerCaps = player->inventoryList->GetItemCount(caps);
		if (playerCapsOverride != -1)
		{
			myPlayerCaps -= playerCapsOverride;
		}
		ProcessVendorRepairList(myMovieRoot);
	}

	//F4CW_Menus::RepairMenu::RepairMenuFunctions::RegisterForInput(true);
}

void F4CW_Menus::RepairMenu::RepairMenuFunctions::OpenRepairFromPipboy(std::uint32_t menuType, InvObject invObject)
{
	iCurrentRepairMenuType = menuType;
	currentPipboyRepairObject = invObject;
	LOG_TO_CONSOLE(std::format("Menu Type = {}", menuType).c_str());
	LOG_TO_CONSOLE(std::format("Pipboy Selected Index: {}", invObject.selectedIndex).c_str());
	OpenRepairMenu();
	F4CW_Menus::RepairMenu::RepairMenuFunctions::RegisterForInput(true);
}

class F4SEInputHandler : public RE::BSInputEventUser
{
public:
	F4SEInputHandler() : BSInputEventUser() {}

	virtual bool ShouldHandleEvent(const RE::InputEvent*) override {
		return true;
	}

	virtual void OnButtonEvent(const RE::ButtonEvent* inputEvent)
	{
		
		std::uint32_t	deviceType = (std::uint32_t) inputEvent->device.get();

		float timer = inputEvent->heldDownSecs;
		bool isDown = inputEvent->QHeldDown();
		bool isUp = timer != 0.0f;


		RE::BSFixedString control = inputEvent->strUserEvent;
		RE::BS_BUTTON_CODE buttonCode = inputEvent->GetBSButtonCode();

		REX::DEBUG(std::format("Button event: deviceType: '{}', timer: '{}', isDown: '{}', isUp: '{}', control: '{}', buttonCode: '{}'", deviceType, timer, isDown ? 1 : 0, isUp ? 1 : 0, control.c_str(), (std::int32_t) buttonCode).c_str());

		if (isDown)
		{
			ProcessUserEvent(control.c_str(), true, deviceType, (std::int32_t) buttonCode);
		}
		else if (isUp)
		{
			ProcessUserEvent(control.c_str(), false, deviceType, (std::int32_t) buttonCode);
		}
		

	}
};

F4SEInputHandler g_repairMenuInputHandler;

void F4CW_Menus::RepairMenu::RepairMenuFunctions::RegisterForInput(bool bRegister)
{
	if (bRegister)
	{
		//RE::PipboyManager::GetSingleton()->inputEventHandlingEnabled = false;

		g_repairMenuInputHandler.inputEventHandlingEnabled = true;
		RE::BSTArray<RE::BSInputEventUser*>* inputEvents = &RE::MenuControls::GetSingleton()->handlers;
		RE::BSInputEventUser* inputHandler = &g_repairMenuInputHandler;
		int idx = -1;

		for (std::uint64_t n = 0; n < inputEvents->size(); n++) {
			if (inputEvents->at(n) == inputHandler) {
				idx = n;
				break;
			}
		}

		if (idx == -1)
		{
			REX::DEBUG("RepairMenu - Registering input.");
			RE::MenuControls::GetSingleton()->RegisterHandler(&g_repairMenuInputHandler);
			// inputEvents->push_back(&g_repairMenuInputHandler);
		}
		else {
			REX::DEBUG("RepairMenu - Registering input already exists.");
		}
	}
	else
	{
		//RE::PipboyManager::GetSingleton()->inputEventHandlingEnabled = true;
		g_repairMenuInputHandler.inputEventHandlingEnabled = false;
	}
}

void F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuClose()
{
	myCurrentVendor = nullptr;
	myVendorSkill = 0;
	myPlayerCaps = 0;
	iCurrentRepairMenuType = -1;
}

void F4CW_Menus::RepairMenu::RepairMenuFunctions::GiveCapsToVendor(int caps)
{
	if (!myCurrentVendor)
		return;

	auto vm = RE::GameVM::GetSingleton()->GetVM();
	vm->DispatchStaticCall<RE::Actor*, std::uint32_t>("TCW:F4CW", "GiveCapsToVendor", NULL, myCurrentVendor, caps);
}

bool F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItem(F4CW::ItemDegradation::WeaponConditionData myWeaponData, float newCondition)
{
	LOG_TO_CONSOLE(std::format("Repairing weapon to new condition {}", newCondition).c_str());

	F4CW::SetWeaponConditionPercent(myWeaponData, newCondition);
	// F4CW::WPNUtilities::UpdateWeaponStats(myWeaponData);

	return true;
}

bool F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItem(F4CW::ItemDegradation::ArmorConditionData myArmorData, float newCondition)
{
	LOG_TO_CONSOLE(std::format("Repairing Armor to new condition {}", newCondition).c_str());

	F4CW::SetArmorConditionPercent(myArmorData, newCondition);
	F4CW::ARMOUtilities::UpdateArmorStats(myArmorData);

	return true;
}

bool F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItemPipboy(const RE::BGSInventoryItem* itemToRepairWith, float newCondition)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	const RE::BGSInventoryItem* repairedItem = InventoryUtils::GetInventoryItemByIndex(currentPipboyRepairObject.selectedIndex);

	if (repairedItem->object->GetFormType() == RE::ENUM_FORM_ID::kWEAP)
	{
		F4CW::ItemDegradation::WeaponConditionData weapData(player, repairedItem->object, repairedItem->stackData->extra.get());
		RepairItem(weapData, newCondition);
	}
	else if (repairedItem->object->GetFormType() == RE::ENUM_FORM_ID::kARMO)
	{
		F4CW::ItemDegradation::ArmorConditionData armorData(player, repairedItem->object, repairedItem->stackData->extra.get());
		RepairItem(armorData, newCondition);
	}

	std::uint32_t removedItemIndex = InventoryUtils::GetIndexByInventoryItem(itemToRepairWith);

	Shared::RemovePipboyInventoryItem(itemToRepairWith, false);

	if (currentPipboyRepairObject.selectedIndex > removedItemIndex)
	{
		currentPipboyRepairObject.selectedIndex -= 1;
	}

	return true;
}


void F4CW_Menus::RepairMenu::OpenRepairMenu()
{
	RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
	if (RE::UI* ui = RE::UI::GetSingleton())
	{
		if (ui->menuMap.contains("CWRepairMenu"))
		{
			uiMessageQueue->AddMessage("CWRepairMenu", RE::UI_MESSAGE_TYPE::kShow);
		}
	}
}

void F4CW_Menus::RepairMenu::CloseRepairMenu()
{
	RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();

	RE::BSFixedString menuName = "CWRepairMenu";
	if (RE::UI* ui = RE::UI::GetSingleton()) {
		if (ui->menuMap.contains("CWRepairMenu"))
		{
			uiMessageQueue->AddMessage("CWRepairMenu", RE::UI_MESSAGE_TYPE::kHide);
		}
	}
}

bool F4CW_Menus::RepairMenu::DefineRepairMenuFormsFromGame()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	caps = dataHandler->LookupForm<RE::TESObjectMISC>(0xF, "Fallout4.esm");

	// @TODO: TO BE REPLACED WHEN MERGED.
	RE::BGSListForm* keywordsInOrder = dataHandler->LookupForm<RE::BGSListForm>(0x0E0A1D, MOD_ESM);
	RE::BGSListForm* listsInOrder = dataHandler->LookupForm<RE::BGSListForm>(0x0E0A1E, MOD_ESM);
	const std::uint32_t keywordsSize = keywordsInOrder->arrayOfForms.size();
	const std::uint32_t listsSize = listsInOrder->arrayOfForms.size();

	if (keywordsSize != listsSize) {
		REX::CRITICAL(std::format("Keywords size does not match lists size for RepairKeywords mapping. Keywords size: '{}', Lists size: '{}'", keywordsSize, listsSize).c_str());
		return false;
	}

	for (std::uint32_t i = 0; i < keywordsSize; i++)
	{
		RE::BGSListForm* repairList = static_cast<RE::BGSListForm*>(listsInOrder->arrayOfForms.at(i));
		if (!repairList) {
			REX::CRITICAL(std::format("RepairList index '{}' is not a BGSListForm type. Check the index in CK.", i).c_str());
			return false;
		}
		RE::BGSKeyword* keyword = static_cast<RE::BGSKeyword*>(keywordsInOrder->arrayOfForms.at(i));
		if (!keyword) {
			REX::CRITICAL(std::format("Keyword index '{}' is not a BGSKeyword type. Check the index in CK.", i).c_str());
			return false;
		}

		switch (repairList->arrayOfForms.at(0)->GetFormType()) {
		case RE::ENUM_FORM_ID::kWEAP:
			F4CWSerialization::AddToWeaponRepairList(repairList, keyword);
			break;
		case RE::ENUM_FORM_ID::kARMO:
			F4CWSerialization::AddToArmorRepairList(repairList, keyword);
			break;
		default:
			REX::WARN(std::format("index '{}' is not either a weapon or armor type when defining list/keyword mappings. Check the lists items.", i).c_str());
			break;
		}
	}


	return true;
}

namespace RepairMenu_Papyrus
{
	void OpenRepairVendorMenu_Papyrus(std::monostate, RE::Actor* myVendor)
	{
		myCurrentVendor = myVendor;
		OpenRepairMenu();
	}
}

bool F4CW_Menus::RepairMenu::RegisterRepairMenuFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->BindNativeMethod("TCW:F4CW", "OpenRepairVendorMenu", RepairMenu_Papyrus::OpenRepairVendorMenu_Papyrus);
	return true;
}

InvObject F4CW_Menus::RepairMenu::RepairMenuFunctions::CreateInvObjectFromArray(Scaleform::GFx::Value array)
{
	InvObject myInvObject;

	Scaleform::GFx::Value bFavorite;
	Scaleform::GFx::Value bTaggedForSearch;
	Scaleform::GFx::Value bIsLegendary;
	Scaleform::GFx::Value iCount;
	Scaleform::GFx::Value iNodeID;
	Scaleform::GFx::Value iFilterFlag;
	Scaleform::GFx::Value iEquipState;
	Scaleform::GFx::Value sText;
	Scaleform::GFx::Value iClipIndex;
	Scaleform::GFx::Value iFormID;
	Scaleform::GFx::Value bCanFavorite;
	Scaleform::GFx::Value iSelectedIndex;

	array.GetMember("favorite", &bFavorite);
	array.GetMember("taggedForSearch", &bTaggedForSearch);
	array.GetMember("isLegendary", &bIsLegendary);
	array.GetMember("count", &iCount);
	array.GetMember("nodeID", &iNodeID);
	array.GetMember("filterFlag", &iFilterFlag);
	array.GetMember("equipState", &iEquipState);
	array.GetMember("text", &sText);
	array.GetMember("clipIndex", &iClipIndex);
	array.GetMember("formID", &iFormID);
	array.GetMember("canFavorite", &bCanFavorite);
	array.GetMember("selectedIndex", &iSelectedIndex);

	/*
	_MESSAGE("bFavorite Type = %i", bFavorite.GetType());
	_MESSAGE("bTaggedForSearch Type = %i", bTaggedForSearch.GetType());
	_MESSAGE("bIsLegendary Type = %i", bIsLegendary.GetType());
	_MESSAGE("iCount Type = %i", iCount.GetType());
	_MESSAGE("iNodeID Type = %i", iNodeID.GetType());
	_MESSAGE("iFilterFlag Type = %i", iFilterFlag.GetType());
	_MESSAGE("iEquipState Type = %i", iEquipState.GetType());
	_MESSAGE("sText Type = %i", sText.GetType());
	_MESSAGE("iClipIndex Type = %i", iClipIndex.GetType());
	_MESSAGE("iFormID Type = %i", iFormID.GetType());
	_MESSAGE("bCanFavorite Type = %i", bCanFavorite.GetType());
	*/

	myInvObject.favorite = bFavorite.GetBoolean();
	myInvObject.taggedForSearch = bTaggedForSearch.GetBoolean();
	myInvObject.isLegendary = bIsLegendary.GetBoolean();
	myInvObject.count = iCount.GetUInt();
	myInvObject.nodeID = iNodeID.GetUInt();
	myInvObject.filterFlag = iFilterFlag.GetUInt();
	myInvObject.equipState = iEquipState.GetUInt();
	myInvObject.text = sText.GetString();
	myInvObject.clipIndex = iClipIndex.GetUInt();
	myInvObject.formID = iFormID.GetInt();
	myInvObject.canFavorite = bCanFavorite.GetBoolean();
	myInvObject.selectedIndex = iSelectedIndex.GetInt();

	return myInvObject;
}

bool F4CW_Menus::RepairMenu::RepairMenuFunctions::CheckInventoryForEligibleRepair(int inventoryIndex)
{

	std::uint32_t inventoryItemCount = InventoryUtils::GetPipboyInventoryObjectCount();
	REX::DEBUG(std::format("Checking for repair for index '{}' out of '{}' objects...", inventoryIndex, inventoryItemCount));
	const RE::BGSInventoryItem* myItem = InventoryUtils::GetInventoryItemByIndex(inventoryIndex);

	RepairObject baseObject{};
	baseObject.form = myItem->object;
	baseObject.extraData = myItem->stackData->extra.get();
	baseObject.index = inventoryIndex;

	RE::TESObjectWEAP* myWeap;
	RE::TESObjectARMO* myArmor;
	RE::BGSKeyword* myRepairKeyword;
	RE::BGSListForm* myRepairFormList;

	switch (baseObject.form->formType.get())
	{
	case RE::ENUM_FORM_ID::kWEAP:
	{
		F4CW::ItemDegradation::WeaponConditionData Data(baseObject.form, baseObject.extraData);

		if (!Data.instance)
		{
			REX::DEBUG(std::format("Instance is null for weapon index '{}'. Not repairable...", inventoryIndex).c_str());
			return false;
		}

		myWeap = static_cast<RE::TESObjectWEAP*>(baseObject.form);
		myRepairKeyword = F4CW::WPNUtilities::GetWeaponRepairKeyword(myWeap);

		if (!myRepairKeyword)
		{
			REX::DEBUG(std::format("Repair Keyword is null for weapon index '{}'. Not repairable...", inventoryIndex).c_str());
			return false;
		}

		myRepairFormList = F4CW::WPNUtilities::GetLinkedWeaponRepairListFromKeyword(myRepairKeyword);
		break;
	}
	case RE::ENUM_FORM_ID::kARMO:
	{
		F4CW::ItemDegradation::ArmorConditionData Data(baseObject.form, baseObject.extraData);

		if (!Data.instance)
		{
			REX::DEBUG(std::format("Instance is null for armor index '{}'. Not repairable...", inventoryIndex).c_str());
			return false;
		}

		myArmor = static_cast<RE::TESObjectARMO*>(baseObject.form);
		myRepairKeyword = F4CW::ARMOUtilities::GetArmorRepairKeyword(myArmor);

		if (!myRepairKeyword)
		{
			REX::DEBUG(std::format("Repair Keyword is null for armor index '{}'. Not repairable...", inventoryIndex).c_str());
			return false;
		}

		myRepairFormList = F4CW::ARMOUtilities::GetLinkedArmorRepairListFromKeyword(myRepairKeyword);
		break;
	}
	default:
	{
		break;
	}
	}

	/*
	if (myRepairKeyword) {
		REX::DEBUG(std::format("Found Repair Keyword on {}", myItem->GetDisplayFullName(baseObject.extraData)).c_str());
	}
	if (myRepairFormList) {
		REX::DEBUG(std::format("Found Repair Form List for {}", myItem->GetDisplayFullName(baseObject.extraData)).c_str());
	}
	*/
	int eligibleCount = 0;

	for (int i = 0; i < inventoryItemCount; i++)
	{
		//	same object, skip it
		if (baseObject.index == i)
		{
			continue;
		}

		RE::TESForm* form = InventoryUtils::GetInventoryFormByIndex(i);
		if (!form)
		{
			continue;
		}

		//	not the same form type, skip it
		if ((std::int32_t) form->formType.get() != (std::int32_t) baseObject.form->formType.get())
		{
			continue;
		}

		RE::ExtraDataList* extraData = InventoryUtils::GetExtraDataListByIndex(i);
		if (extraData->HasType(RE::EXTRA_DATA_TYPE::kHealth))
		{
			if (!IsFormInList(form, myRepairFormList))
			{
				//	this item is not in repair list, skip
				continue;
			}
			eligibleCount++;
		}
	}

	if (eligibleCount > 0)
	{
		return true;
	}

	return false;
}

void F4CW_Menus::RepairMenu::RepairMenuFunctions::GetINIOptions()
{
	std::string myINI = "./Data/CapitalWasteland.ini";
	bool workshopTab;
	workshopTab = GetPrivateProfileIntA("Pipboy", "bHideWorkshopTab", 0, myINI.c_str());

	bHideWorkshopTab = workshopTab;
}

bool F4CW_Menus::RepairMenu::RepairMenuFunctions::CheckWorkshopTab()
{
	return bHideWorkshopTab;
}

void F4CW_Menus::RepairMenu::ProcessUserEvent(const char* controlName, bool isDown, int deviceType, std::uint32_t keyCode)
{
	auto ui = RE::UI::GetSingleton();
	RE::BSFixedString repairMenuStr("RepairMenu");
	if (ui->GetMenuOpen(repairMenuStr))
	{
		RE::IMenu* menu = ui->GetMenu(repairMenuStr).get();
		Scaleform::GFx::ASMovieRootBase* movieRoot = menu->uiMovie->asMovieRoot.get();
		Scaleform::GFx::Value args[4];
		args[0] = controlName;
		args[1] = isDown;
		args[2] = deviceType;
		args[3] = keyCode;

		movieRoot->Invoke("root.Menu_mc.ProcessUserEventExternal", nullptr, args, 4);
	}
}

void F4CW_Menus::RepairMenu::ShowNotification(std::string message)
{
	auto vm = RE::GameVM::GetSingleton()->GetVM();
	vm->DispatchStaticCall<RE::BSFixedString>("Debug", "Notification", NULL, RE::BSFixedString(message.c_str()));
}

bool F4CW_Menus::RepairMenu::IsFormInList(RE::TESForm* form, RE::BGSListForm* list)
{
	for (int i = 0; i < list->arrayOfForms.size(); i++)
	{
		if (list->arrayOfForms[i] == form)
			return true;
	}

	return false;
}

bool F4CW_Menus::RepairMenu::RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value)
{
	
	

	return true;
}

void F4CW_Menus::RepairMenu::CreateRepairMenu()
{
	if (RE::UI* ui = RE::UI::GetSingleton())
	{
		if (!ui->menuMap.contains("CWRepairMenu"))
		{
			ui->RegisterMenu("CWRepairMenu", F4CW_Menus::RepairMenu::CWRepairMenu::Create);
			REX::DEBUG("Registered 'CWLevelUpMenu'.");
		}
	}
}

F4CW_Menus::RepairMenu::CWRepairMenu::CWRepairMenu()
{
	//flags = kFlag_PauseGame | kFlag_ShowCursor | kFlag_HideOther | kFlag_UpdateCursorOnPlatformChange | kFlag_BlurBackground;
				//flags = kFlag_PauseGame | kFlag_ShowCursor | kFlag_UpdateCursorOnPlatformChange | kFlag_BlurBackground;

				//depth = 0x09;
				//depth = 0x0E;






	menuFlags.set(
		RE::UI_MENU_FLAGS::kPausesGame,
		RE::UI_MENU_FLAGS::kUsesCursor,
		RE::UI_MENU_FLAGS::kTopmostRenderedMenu,
		RE::UI_MENU_FLAGS::kUpdateUsesCursor,
		RE::UI_MENU_FLAGS::kUsesBlurredBackground
	);
	menuHUDMode = "SpecialMode";
	depthPriority = RE::UI_DEPTH_PRIORITY::kLoadingMenu;
	const auto ScaleformManager = RE::BSScaleformManager::GetSingleton();

	[[maybe_unused]] const auto LoadMovieSuccess =
		ScaleformManager->LoadMovieEx(*this, "Interface/RepairMenu.swf"sv, "root", Scaleform::GFx::Movie::ScaleModeType::kExactFit);
	assert(LoadMovieSuccess);

	Scaleform::GFx::ASMovieRootBase* movieRoot = uiMovie.get()->asMovieRoot.get();
	Scaleform::GFx::Value bgsCodeObj;
	movieRoot->GetVariable(&bgsCodeObj, "root.Menu_mc.BGSCodeObj");

	filterHolder = std::make_unique<RE::BSGFxShaderFXTarget>(*uiMovie, "root.Menu_mc");
	if (filterHolder)
	{
		filterHolder->CreateAndSetFiltersToHUD(RE::HUDColorTypes::kGameplayHUDColor);
		shaderFXObjects.push_back(filterHolder.get());
	}

	Shared::RegisterFunction<Debug_ActionScript>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "DebugPrint");
	Shared::RegisterFunction<RepairMenu_OpenMenu>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "OpenMenu");
	Shared::RegisterFunction<RepairMenu_RepairItems>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "RepairItems");
	Shared::RegisterFunction<RepairMenu_RepairItemPipboy>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "RepairItemPipboy");
	Shared::RegisterFunction<Scaleform_PlayUISound>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "PlayUISound");
	Shared::RegisterFunction<RepairMenu_CloseMenu>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "CloseMenu");

	movieRoot->Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);
}
