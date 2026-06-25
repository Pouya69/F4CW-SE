#pragma once

#include "skills.h"
#include "GameForms.h"
#include "PerkHelpers.h"

F4CW::PerkHelpers::AvailablePerk F4CW::PerkHelpers::GetAvailablePerk(RE::BGSPerk* a_perk)
{
	AvailablePerk result;

	result.level = 0;
	result.perk = nullptr;
	result.numRanks = a_perk->data.numRanks;

	RE::BGSPerk* temporaryPerk;

	bool gotResult = false;
	bool learned = false;

	RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

	temporaryPerk = a_perk;

	if (a_perk->data.numRanks > 1 && (a_perk->nextPerk == a_perk || a_perk->nextPerk == nullptr))
	{
		std::uint32_t currentPerkRank = playerCharacter->GetPerkRank(temporaryPerk);
		if (currentPerkRank < a_perk->data.numRanks)
		{
			result.level = currentPerkRank + 1;
			result.perk = a_perk;
			return result;
		}
		else
		{
			return result;
		}
	}
	else
	{
		std::uint32_t counter = 0;
		while (true)
		{
			temporaryPerk = temporaryPerk->nextPerk;
			counter++;
			if (temporaryPerk == a_perk || temporaryPerk == nullptr)
			{
				break;
			}
		}
		if (counter != a_perk->data.numRanks)
		{
			result.numRanks = counter;
		}
	}
	temporaryPerk = a_perk;

	for (std::uint32_t i = 0; i < result.numRanks; i++)
	{
		if (playerCharacter->GetPerkRank(temporaryPerk) == 0)
		{
			if (!gotResult)
			{
				gotResult = true;
				result.perk = temporaryPerk;
				result.level = i + 1;
			}
		}
		else
		{
			if (!learned)
			{
				learned = true;
			}
		}
		temporaryPerk = temporaryPerk->nextPerk;
	}
	return result;
}

F4CW::PerkHelpers::PerkData F4CW::PerkHelpers::GetPerkRequirements(RE::BGSPerk* a_perk)
{
	PerkData result;
	RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

	bool isAllowable = true;
	bool isHighLevel = a_perk->data.level > playerCharacter->GetLevel();
	bool isEligible = !isHighLevel;
	std::uint32_t filterFlag = 0;
	result.requiredLevel = a_perk->data.level;
	std::string requirementString = "";

	RE::TESConditionItem* perkConditionItem = a_perk->perkConditions.head;

	bool lastFlag = false;

	RE::ActorValueInfo* temporaryAVI;

	while (perkConditionItem != nullptr)
	{
		RE::SCRIPT_OUTPUT scriptOutput = perkConditionItem->data.functionData.function.get();
		float compareValue = perkConditionItem->GetComparisonValue();
		RE::ENUM_COMPARISON_CONDITION compareCondition = static_cast<RE::ENUM_COMPARISON_CONDITION>(static_cast<std::int32_t>(perkConditionItem->data.condition));

		bool eligible = true;
		bool lessThan = false;
		bool lessThanEqual = false;

		switch (scriptOutput)
		{
		case RE::SCRIPT_OUTPUT::kScript_GetPermanentValue:
		case RE::SCRIPT_OUTPUT::kScript_GetBaseValue:
		case RE::SCRIPT_OUTPUT::kScript_GetValue:
			temporaryAVI = static_cast<RE::ActorValueInfo*>(perkConditionItem->data.functionData.param[0]);
			eligible = perkConditionItem->IsTrue(playerCharacter, nullptr);

			if (compareCondition == RE::ENUM_COMPARISON_CONDITION::kLessThan)
			{
				lessThan = true;
			}
			else if (compareCondition == RE::ENUM_COMPARISON_CONDITION::kLessThanEqual)
			{
				lessThanEqual = true;
			}

			filterFlag |= FilterFlags::kNonSpecial;

			if (!eligible)
			{
				requirementString += "<font color=\'#838383\'>";
			}

			switch (temporaryAVI->formID)
			{
			case SPECIALFormIDs::StrengthID:
				requirementString += "$F4CW_STR";
				break;
			case SPECIALFormIDs::PerceptionID:
				requirementString += "$F4CW_PER";
				break;
			case SPECIALFormIDs::EnduranceID:
				requirementString += "$F4CW_END";
				break;
			case SPECIALFormIDs::CharismaID:
				requirementString += "$F4CW_CHA";
				break;
			case SPECIALFormIDs::IntelligenceID:
				requirementString += "$F4CW_INT";
				break;
			case SPECIALFormIDs::AgilityID:
				requirementString += "$F4CW_AGI";
				break;
			case SPECIALFormIDs::LuckID:
				requirementString += "$F4CW_LCK";
				break;
			default:
				if (temporaryAVI->GetFullName() == "")
				{
					requirementString += temporaryAVI->GetFormEditorID();
				}
				else
				{
					requirementString += temporaryAVI->GetFullName();
				}
				break;
			}

			if (lessThan) {
				requirementString += " > " + std::to_string((std::uint32_t)compareValue);
			}
			else if (lessThanEqual) {
				requirementString += " > " + std::to_string((std::uint32_t)compareValue - 1);
			}
			else {
				requirementString += " " + std::to_string((std::uint32_t)compareValue);
			}
			break;
		case RE::SCRIPT_OUTPUT::kScript_GetIsSex:
			isAllowable = perkConditionItem->IsTrue(playerCharacter, nullptr);
			break;
		default:
			break;
		}

		lastFlag = (perkConditionItem->data.compareOr == 0);

		perkConditionItem = perkConditionItem->next;

		if (perkConditionItem)
		{
			RE::SCRIPT_OUTPUT scriptOutputNext = perkConditionItem->data.functionData.function.get();

			if (scriptOutputNext == RE::SCRIPT_OUTPUT::kScript_GetPermanentValue || scriptOutputNext == RE::SCRIPT_OUTPUT::kScript_GetBaseValue || scriptOutputNext == RE::SCRIPT_OUTPUT::kScript_GetValue) {
				if (scriptOutput == RE::SCRIPT_OUTPUT::kScript_GetIsSex)
				{
					requirementString += "";
					if (!eligible)
					{
						requirementString += "</font>";
					}
				}
				else if (lastFlag)
				{
					requirementString += ", ";
					if (!eligible)
					{
						requirementString += "</font>";
					}
				}
				else
				{
					requirementString += " $F4CW_or ";
				}
			}
		}
	}

	result.isEligible = isEligible && a_perk->perkConditions.IsTrue(playerCharacter, playerCharacter);
	result.isAllowable = isAllowable;
	result.isHighLevel = isHighLevel;
	if (filterFlag == 0) filterFlag |= FilterFlags::kOther;
	result.filterFlag = filterFlag | (result.isEligible ? 1 : 2);
	result.requirementsString = requirementString;
	result.SWFPath = a_perk->swfFile.c_str();
	result.isTagged = false;

	return result;
}
