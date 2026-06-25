#include "LevelUpMenu.h"

namespace F4CW_Menus {
	namespace LevelUpMenu {

		bool bForceLevelUp;

		RE::BSSoundHandle previousSoundHandle;

		float perkPoints;

		std::uint32_t menuModeType = 0;
		std::uint32_t tagPointsValue = 0;

		bool allowRetag = false;

		RE::BSTArray<RE::ActorValueInfo*> scaleformSkills;

		namespace InitialValues {
			std::uint32_t barter;
			std::uint32_t bigGuns;
			std::uint32_t energyWeapons;
			std::uint32_t explosives;
			std::uint32_t lockpick;
			std::uint32_t medicine;
			std::uint32_t meleeWeapons;
			std::uint32_t repair;
			std::uint32_t science;
			std::uint32_t smallGuns;
			std::uint32_t sneak;
			std::uint32_t speech;
			std::uint32_t unarmed;
		}

		namespace TempModValues {
			std::uint32_t barter;
			std::uint32_t bigGuns;
			std::uint32_t energyWeapons;
			std::uint32_t explosives;
			std::uint32_t lockpick;
			std::uint32_t medicine;
			std::uint32_t meleeWeapons;
			std::uint32_t repair;
			std::uint32_t science;
			std::uint32_t smallGuns;
			std::uint32_t sneak;
			std::uint32_t speech;
			std::uint32_t unarmed;
		}
		void GetINIOptions()
		{
			std::string myINI = "./Data/CapitalWasteland.ini";
			bool forceLevelUp;
			forceLevelUp = GetPrivateProfileInt("Gameplay", "bForceLevelUp", 1, myINI.c_str());

			bForceLevelUp = forceLevelUp;
		}
		bool GetForcedLevelUp()
		{
			return bForceLevelUp;
		}

		void InitializeValues()
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

			InitialValues::barter = playerCharacter->GetPermanentActorValue(*CW_Skills.Barter);
			InitialValues::bigGuns = playerCharacter->GetPermanentActorValue(*CW_Skills.BigGuns);
			InitialValues::energyWeapons = playerCharacter->GetPermanentActorValue(*CW_Skills.EnergyWeapons);
			InitialValues::explosives = playerCharacter->GetPermanentActorValue(*CW_Skills.Explosives);
			InitialValues::lockpick = playerCharacter->GetPermanentActorValue(*CW_Skills.Lockpick);
			InitialValues::medicine = playerCharacter->GetPermanentActorValue(*CW_Skills.Medicine);
			InitialValues::meleeWeapons = playerCharacter->GetPermanentActorValue(*CW_Skills.MeleeWeapons);
			InitialValues::repair = playerCharacter->GetPermanentActorValue(*CW_Skills.Repair);
			InitialValues::science = playerCharacter->GetPermanentActorValue(*CW_Skills.Science);
			InitialValues::smallGuns = playerCharacter->GetPermanentActorValue(*CW_Skills.SmallGuns);
			InitialValues::sneak = playerCharacter->GetPermanentActorValue(*CW_Skills.Sneak);
			InitialValues::speech = playerCharacter->GetPermanentActorValue(*CW_Skills.Speech);
			InitialValues::unarmed = playerCharacter->GetPermanentActorValue(*CW_Skills.Unarmed);
		}

		float GetInitialActorValue(RE::ActorValueInfo* a_skill)
		{
			if (a_skill == CW_Skills.Barter)
			{
				return InitialValues::barter;
			}
			else if (a_skill == CW_Skills.EnergyWeapons)
			{
				return InitialValues::energyWeapons;
			}
			else if (a_skill == CW_Skills.Explosives)
			{
				return InitialValues::explosives;
			}
			else if (a_skill == CW_Skills.BigGuns)
			{
				return InitialValues::bigGuns;
			}
			else if (a_skill == CW_Skills.Lockpick)
			{
				return InitialValues::lockpick;
			}
			else if (a_skill == CW_Skills.Medicine)
			{
				return InitialValues::medicine;
			}
			else if (a_skill == CW_Skills.MeleeWeapons)
			{
				return InitialValues::meleeWeapons;
			}
			else if (a_skill == CW_Skills.Repair)
			{
				return InitialValues::repair;
			}
			else if (a_skill == CW_Skills.Science)
			{
				return InitialValues::science;
			}
			else if (a_skill == CW_Skills.Sneak)
			{
				return InitialValues::sneak;
			}
			else if (a_skill == CW_Skills.Speech)
			{
				return InitialValues::speech;
			}
			else if (a_skill == CW_Skills.SmallGuns)
			{
				return InitialValues::smallGuns;
			}
			else if (a_skill == CW_Skills.Unarmed)
			{
				return InitialValues::unarmed;
			}
			else
			{
				return -1;
			}
		}
		void PopulateSkillEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::ActorValueInfo* a_skill)
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			float buffedValue = playerCharacter->GetActorValue(*a_skill);

			Scaleform::GFx::Value skillEntry;
			a_movieRoot->CreateObject(&skillEntry);

			RE::BSStringT<char> description;
			a_skill->GetDescription(description);

			skillEntry.SetMember("text", a_skill->fullName.c_str());
			skillEntry.SetMember("editorID", a_skill->GetFormEditorID());
			skillEntry.SetMember("description", description.c_str());
			skillEntry.SetMember("formid", a_skill->formID);

			float baseValue = GetInitialActorValue(a_skill);
			float value = playerCharacter->GetPermanentActorValue(*a_skill);
			skillEntry.SetMember("value", (std::uint32_t)baseValue);
			skillEntry.SetMember("baseValue", (std::uint32_t)baseValue);
			skillEntry.SetMember("buffedValue", (std::uint32_t)value);

			skillEntry.SetMember("alreadyTagged", F4CWSerialization::IsSkillTagged(a_skill->formID));
			skillEntry.SetMember("tagged", F4CWSerialization::IsSkillTagged(a_skill->formID));
			a_destination->PushBack(skillEntry);
		}
		bool ProcessSkillsList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			Scaleform::GFx::Value argumentsArray[3];
			a_movieRoot->CreateArray(&argumentsArray[0]);

			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Barter);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.BigGuns);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.EnergyWeapons);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Explosives);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Lockpick);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Medicine);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.MeleeWeapons);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Repair);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Science);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.SmallGuns);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Sneak);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Speech);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Unarmed);

			argumentsArray[1] = F4CWSerialization::GetSkillPoints();
			argumentsArray[2] = RE::PlayerCharacter::GetSingleton()->GetLevel();
			a_movieRoot->Invoke("root.Menu_mc.onLevelUpStart", nullptr, argumentsArray, 3);

			return true;
		}
		bool ProcessTagSkillsList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, std::uint32_t a_tagSkills, bool a_retag)
		{
			Scaleform::GFx::Value argumentsArray[3];
			a_movieRoot->CreateArray(&argumentsArray[0]);

			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Barter);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.BigGuns);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.EnergyWeapons);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Explosives);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Lockpick);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Medicine);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.MeleeWeapons);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Repair);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Science);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.SmallGuns);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Sneak);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Speech);
			PopulateSkillEntry(&argumentsArray[0], a_movieRoot, CW_Skills.Unarmed);

			argumentsArray[1] = a_tagSkills;
			argumentsArray[2] = a_retag;

			a_movieRoot->Invoke("root.Menu_mc.onTagSkillsStart", nullptr, argumentsArray, 3);

			return true;
		}
		void PopulateSpecialEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::ActorValueInfo* a_skill)
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

			Scaleform::GFx::Value skillEntry;
			a_movieRoot->CreateObject(&skillEntry);

			RE::BSStringT<char> description;
			a_skill->GetDescription(description);

			skillEntry.SetMember("text", a_skill->fullName.c_str());
			skillEntry.SetMember("editorID", a_skill->GetFormEditorID());
			skillEntry.SetMember("description", description.c_str());
			skillEntry.SetMember("formid", a_skill->formID);

			float baseAVValue = playerCharacter->GetBaseActorValue(*a_skill);
			float avValue = playerCharacter->GetActorValue(*a_skill);

			skillEntry.SetMember("value", (std::uint32_t)baseAVValue);
			skillEntry.SetMember("baseValue", (std::uint32_t)baseAVValue);
			skillEntry.SetMember("buffedValue", (std::uint32_t)avValue);

			a_destination->PushBack(skillEntry);
		}
		bool ProcessSpecialList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			Scaleform::GFx::Value argumentsArray[1];
			a_movieRoot->CreateArray(&argumentsArray[0]);

			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Strength);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Perception);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Endurance);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Charisma);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Intelligence);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Agility);
			PopulateSpecialEntry(&argumentsArray[0], a_movieRoot, VanillaActorValues.Luck);

			switch (menuModeType) {
			case kSpecialRespec:
				a_movieRoot->Invoke("root.Menu_mc.onSpecialRespecStart", nullptr, argumentsArray, 1);
				break;
			case kIntenseTraining:
				a_movieRoot->Invoke("root.Menu_mc.onIntenseTrainingStart", nullptr, argumentsArray, 1);
				break;
			default:
				break;
			}

			return true;
		}
		void PopulatePerkEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::BGSPerk* a_perk, bool a_eligibleOnly)
		{
			F4CW::PerkHelpers::AvailablePerk currentPerk = F4CW::PerkHelpers::GetAvailablePerk(a_perk);

			if (currentPerk.perk != nullptr)
			{
				REX::DEBUG("Processing perk: {}", currentPerk.perk->GetFormEditorID());

				F4CW::PerkHelpers::PerkData temporaryPerkData = F4CW::PerkHelpers::GetPerkRequirements(currentPerk.perk);

				Scaleform::GFx::Value perkEntry;
				a_movieRoot->CreateObject(&perkEntry);
				perkEntry.SetMember("text", currentPerk.perk->fullName.c_str());
				perkEntry.SetMember("reqs", temporaryPerkData.requirementsString.c_str());
				perkEntry.SetMember("filterFlag", temporaryPerkData.filterFlag);
				perkEntry.SetMember("iselig", temporaryPerkData.isEligible);
				perkEntry.SetMember("isHighLevel", temporaryPerkData.isHighLevel);
				perkEntry.SetMember("reqLevel", temporaryPerkData.requiredLevel);
				perkEntry.SetMember("qname", a_perk->formID);
				perkEntry.SetMember("formid", currentPerk.perk->formID);
				perkEntry.SetMember("level", currentPerk.level);
				perkEntry.SetMember("SWFPath", temporaryPerkData.SWFPath.c_str());
				perkEntry.SetMember("numranks", currentPerk.numRanks);
				perkEntry.SetMember("type", PerkTypes::kDefault);
				perkEntry.SetMember("ranksinfo", "");

				RE::BSStringT<char> description;
				currentPerk.perk->GetDescription(description);

				perkEntry.SetMember("description", description.c_str());
				perkEntry.SetMember("isTagged", temporaryPerkData.isTagged);

				if (a_eligibleOnly)
				{
					if (temporaryPerkData.isEligible && temporaryPerkData.isAllowable)
					{
						a_destination->PushBack(perkEntry);
					}
				}
				else
				{
					if (!temporaryPerkData.isEligible && temporaryPerkData.isAllowable)
					{
						a_destination->PushBack(perkEntry);
					}
				}
			}
		}
		bool ProcessPerkList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			Scaleform::GFx::Value argumentsArray[2];
			a_movieRoot->CreateArray(&argumentsArray[0]);

			// Eligible perks iterated through first to sort at top.
			for (std::uint32_t p1 = 0; p1 < Skills::CWPerksLevelUp.size(); p1++)
			{
				RE::BGSPerk* perk = Skills::CWPerksLevelUp.at(p1);
				PopulatePerkEntry(&argumentsArray[0], a_movieRoot, perk, true);
			}

			// Ineligible perks iterated through last to sort at bottom.
			for (std::uint32_t p2 = 0; p2 < Skills::CWPerksLevelUp.size(); p2++)
			{
				RE::BGSPerk* perk = Skills::CWPerksLevelUp.at(p2);
				PopulatePerkEntry(&argumentsArray[0], a_movieRoot, perk, false);
			}

			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

			argumentsArray[1] = playerCharacter->perkCount;

			a_movieRoot->Invoke("root.Menu_mc.onSwitchToPerks", nullptr, argumentsArray, 2);
			return true;
		}
		std::uint32_t GetSkillArrayIndexByEditorID(const char* myEditorID)
		{
			if (myEditorID == "00DCBarter")
			{
				return SkillArray::Barter;
			}
			else if (myEditorID == "00DCBigGuns")
			{
				return SkillArray::BigGuns;
			}
			else if (myEditorID == "00DCEnergyWeapons")
			{
				return SkillArray::EnergyWeapons;
			}
			else if (myEditorID == "00DCExplosives")
			{
				return SkillArray::Explosives;
			}
			else if (myEditorID == "00DCLockpick")
			{
				return SkillArray::Lockpick;
			}
			else if (myEditorID == "00DCMedicine")
			{
				return SkillArray::Medicine;
			}
			else if (myEditorID == "00DCMeleeWeapons")
			{
				return SkillArray::MeleeWeapons;
			}
			else if (myEditorID == "00DCRepair")
			{
				return SkillArray::Repair;
			}
			else if (myEditorID == "00DCScience")
			{
				return SkillArray::Science;
			}
			else if (myEditorID == "00DCSmallGuns")
			{
				return SkillArray::SmallGuns;
			}
			else if (myEditorID == "00DCSneak")
			{
				return SkillArray::Sneak;
			}
			else if (myEditorID == "00DCSpeech")
			{
				return SkillArray::Speech;
			}
			else if (myEditorID == "00DCUnarmed")
			{
				return SkillArray::Unarmed;
			}
			else
			{
				return -1;
			}
		}
		float GetSkillPointsToAdd()
		{
			// Fallout 3 formula is 10 + base intelligence
			// Extra 3 points if the player has "Educated" perk
			// Using a global in 'FalloutCascaida.esm' we can adjust the base value.

			RE::TESGlobal* baseSkillPointsGlobal = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x065BAE, MOD_ESM);
			float skillPointsValue = 0;

			if (baseSkillPointsGlobal)
			{
				skillPointsValue = baseSkillPointsGlobal->value;
			}
			else
			{
				skillPointsValue = 10;
			}

			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

			float playerIntelligence = playerCharacter->GetBaseActorValue(*VanillaActorValues.Intelligence);

			skillPointsValue = (skillPointsValue + playerIntelligence);

			RE::BGSPerk* educatedPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x04901E, MOD_ESM);

			if (educatedPerk)
			{
				if (playerCharacter->GetPerkRank(educatedPerk) != 0)
				{
					skillPointsValue = (skillPointsValue + 3);
				}
			}

			return skillPointsValue;
		}
		bool CanLevelUpMenuBeShown()
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			if (playerCharacter->IsInCombat())
			{
				return false;
			}

			if (Shared::InMenuMode())
			{
				return false;
			}

			if (RE::IsPlayerInDialogue())
			{
				return false;
			}

			if (Shared::IsXPMetervisible())
			{
				return false;
			}

			return true;
		}
		void ShowLvlUpMenu()
		{
			RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
			if (RE::UI* ui = RE::UI::GetSingleton())
			{
				if (ui->menuMap.contains("CWLevelUpMenu"))
				{
					uiMessageQueue->AddMessage("CWLevelUpMenu", RE::UI_MESSAGE_TYPE::kShow);
				}
			}
		}

		void WaitForLevelUpReady()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			if (!CanLevelUpMenuBeShown())
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));

				std::async(std::launch::async, WaitForLevelUpReady);
				return;
			}
			else
			{
				ShowLvlUpMenu();
			}
		}

		void ModPerkCount(std::int8_t a_count)
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			std::int8_t result = playerCharacter->perkCount;

			if (a_count < -result)
			{
				result = 0;
			}
			else
			{
				result = result + a_count;
				if (result > 255)
				{
					result = 255;
				}
			}
			playerCharacter->SetPerkCount(result);
		}

		void SetSkillTagged(RE::ActorValueInfo* a_skill, bool a_tag)
		{
			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			auto formID = a_skill->formID;

			if (a_tag)
			{
				if (!F4CWSerialization::IsSkillTagged(formID))
				{
					F4CWSerialization::SetSkillTagged(formID);
					playerCharacter->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, *a_skill, 15);
				}
			}
			else
			{
				if (F4CWSerialization::IsSkillTagged(formID))
				{
					F4CWSerialization::RemoveSkillTagged(formID);
					playerCharacter->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, *a_skill, -15);
				}
			}
		}

		void HandleLevelUp()
		{
			float pointsToAdd = GetSkillPointsToAdd();

			F4CWSerialization::ModSkillPoints(pointsToAdd);

			Skills::CWPerksLevelUp.clear();
			Skills::CWSkillsLevelUp.clear();
			Skills::ScaleformSkillSounds.clear();

			GetLevelUpFormsFromGame();

			F4CWSerialization::SetReadyToLevelUp(true);

			std::thread LevelUpWait(WaitForLevelUpReady);
			LevelUpWait.detach();
		}

		void ModSkillByName(std::string a_skillName, std::uint32_t a_value, std::uint32_t a_baseValue)
		{
			std::string skillNameString = a_skillName;
			skillNameString.erase(std::remove_if(skillNameString.begin(), skillNameString.end(), isspace), skillNameString.end());

			RE::ActorValueInfo* skill = GetSkillByName(skillNameString);
			const char* skillEditorID = skill->GetFormEditorID();

			std::uint32_t modValue = (a_value - a_baseValue);

			if (skillEditorID == "00DCBarter")
			{
				TempModValues::barter = modValue;
			}
			else if (skillEditorID == "00DCEnergyWeapons")
			{
				TempModValues::energyWeapons = modValue;
			}
			else if (skillEditorID == "00DCExplosives")
			{
				TempModValues::explosives = modValue;
			}
			else if (skillEditorID == "00DCBigGuns")
			{
				TempModValues::bigGuns = modValue;
			}
			else if (skillEditorID == "00DCLockpick")
			{
				TempModValues::lockpick = modValue;
			}
			else if (skillEditorID == "00DCMedicine")
			{
				TempModValues::medicine = modValue;
			}
			else if (skillEditorID == "00DCMeleeWeapons")
			{
				TempModValues::meleeWeapons = modValue;
			}
			else if (skillEditorID == "00DCRepair")
			{
				TempModValues::repair = modValue;
			}
			else if (skillEditorID == "00DCScience")
			{
				TempModValues::science = modValue;
			}
			else if (skillEditorID == "00DCSneak")
			{
				TempModValues::sneak = modValue;
			}
			else if (skillEditorID == "00DCSpeech")
			{
				TempModValues::speech = modValue;
			}
			else if (skillEditorID == "00DCSmallGuns")
			{
				TempModValues::smallGuns = modValue;
			}
			else if (skillEditorID == "00DCUnarmed")
			{
				TempModValues::unarmed = modValue;
			}

			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			playerCharacter->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, *skill, modValue);
		}

		void RevertSkill(RE::ActorValueInfo* skill)
		{
			const char* editorID = skill->GetFormEditorID();

			std::uint32_t modValue;

			if (editorID == "00DCBarter")
			{
				modValue = TempModValues::barter;
				TempModValues::barter = 0;
			}
			else if (editorID == "00DCEnergyWeapons")
			{
				modValue = TempModValues::energyWeapons;
				TempModValues::energyWeapons = 0;
			}
			else if (editorID == "00DCExplosives")
			{
				modValue = TempModValues::explosives;
				TempModValues::explosives = 0;
			}
			else if (editorID == "00DCBigGuns")
			{
				modValue = TempModValues::bigGuns;
				TempModValues::bigGuns = 0;
			}
			else if (editorID == "00DCLockpick")
			{
				modValue = TempModValues::lockpick;
				TempModValues::lockpick = 0;
			}
			else if (editorID == "00DCMedicine")
			{
				modValue = TempModValues::medicine;
				TempModValues::medicine = 0;
			}
			else if (editorID == "00DCMeleeWeapons")
			{
				modValue = TempModValues::meleeWeapons;
				TempModValues::meleeWeapons = 0;
			}
			else if (editorID == "00DCRepair")
			{
				modValue = TempModValues::repair;
				TempModValues::repair = 0;
			}
			else if (editorID == "00DCScience")
			{
				modValue = TempModValues::science;
				TempModValues::science = 0;
			}
			else if (editorID == "00DCSneak")
			{
				modValue = TempModValues::sneak;
				TempModValues::sneak = 0;
			}
			else if (editorID == "00DCSpeech")
			{
				modValue = TempModValues::speech;
				TempModValues::speech = 0;
			}
			else if (editorID == "00DCSmallGuns")
			{
				modValue = TempModValues::smallGuns;
				TempModValues::smallGuns = 0;
			}
			else if (editorID == "00DCUnarmed")
			{
				modValue = TempModValues::unarmed;
				TempModValues::unarmed = 0;
			}

			RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();
			playerCharacter->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, *skill, modValue);
		}

		void CheckForLevelUp()
		{
			if (F4CWSerialization::IsReadyToLevelUp())
			{
				std::thread LevelUpWait(WaitForLevelUpReady);
				LevelUpWait.detach();
			}
		}

		void OnLevelUpStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			ProcessSkillsList(a_movieRoot);
		}

		void OnTagSkillsStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			ProcessTagSkillsList(a_movieRoot, tagPointsValue, allowRetag);
		}

		void OnSpecialRespecStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			ProcessSpecialList(a_movieRoot);
		}

		void OnIntenseTrainingStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			ProcessSpecialList(a_movieRoot);
		}

		void HandleLevelUpMenuOpen(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot)
		{
			previousSoundHandle.Stop();
			switch (menuModeType)
			{
			case kLevelUp:
				OnLevelUpStart(a_movieRoot);
				break;

			case kTagSkills:
				OnTagSkillsStart(a_movieRoot);
				break;

			case kSpecialRespec:
				OnSpecialRespecStart(a_movieRoot);
				break;

			case kIntenseTraining:
				OnIntenseTrainingStart(a_movieRoot);
				break;

			default:
				break;
			}
		}

		void CompleteLevelUp()
		{
			switch (menuModeType)
			{
			case kLevelUp:
				F4CWSerialization::SetReadyToLevelUp(false);
				F4CWSerialization::SetSkillPoints(0);
				REX::DEBUG("'CompleteLevelUp' - 'kLevelUp'");
				break;

			case kTagSkills:
				tagPointsValue = 0;
				allowRetag = false;
				REX::DEBUG("'CompleteLevelUp' - 'kTagSkills'");
				break;

			case kSpecialRespec:
				REX::DEBUG("'CompleteLevelUp' - 'kSpecialRespec'");
				break;

			case kIntenseTraining:
				REX::DEBUG("'CompleteLevelUp' - 'kIntenseTraining'");
				break;
			default:
				break;
			}

			menuModeType = 0;
		}

		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value)
		{
			Scaleform::GFx::Value currentSWFPath;
			// Register native code handlers.
			if (a_view->asMovieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url"))
			{
				if (_stricmp(currentSWFPath.GetString(), "Interface/CWLevelUpMenu.swf") == 0)
				{
					Scaleform::GFx::Value bgsCodeObj;
					a_view->asMovieRoot->GetVariable(&bgsCodeObj, "root.Menu_mc.BGSCodeObj");

					Shared::RegisterFunction<OpenMenu>(&bgsCodeObj, a_view->asMovieRoot, "OpenMenu");
					Shared::RegisterFunction<SetSkills>(&bgsCodeObj, a_view->asMovieRoot, "SetSkills");
					Shared::RegisterFunction<ResetSkills>(&bgsCodeObj, a_view->asMovieRoot, "ResetSkills");
					Shared::RegisterFunction<ResetTagSkills>(&bgsCodeObj, a_view->asMovieRoot, "ResetTagSkills");
					Shared::RegisterFunction<BackToSkills>(&bgsCodeObj, a_view->asMovieRoot, "BackToSkills");
					Shared::RegisterFunction<UpdatePerkMenu>(&bgsCodeObj, a_view->asMovieRoot, "UpdatePerkMenu");
					Shared::RegisterFunction<AddPerks>(&bgsCodeObj, a_view->asMovieRoot, "AddPerks");
					Shared::RegisterFunction<TagSkills>(&bgsCodeObj, a_view->asMovieRoot, "TagSkills");
					Shared::RegisterFunction<LearnSpecial>(&bgsCodeObj, a_view->asMovieRoot, "LearnSpecial");
					Shared::RegisterFunction<ResetSpecial>(&bgsCodeObj, a_view->asMovieRoot, "ResetSpecial");
					Shared::RegisterFunction<PlaySkillSound>(&bgsCodeObj, a_view->asMovieRoot, "PlaySkillSound");
					Shared::RegisterFunction<PlayPerkSound>(&bgsCodeObj, a_view->asMovieRoot, "PlayPerkSound");
					Shared::RegisterFunction<CloseMenu>(&bgsCodeObj, a_view->asMovieRoot, "CloseMenu");
					Shared::RegisterFunction<PlayUISound>(&bgsCodeObj, a_view->asMovieRoot, "PlayUISound");

					a_view->asMovieRoot->Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);
				}
				return true;
			}
			return false;
		}

		void RegisterMenu()
		{
			if (RE::UI* ui = RE::UI::GetSingleton())
			{
				if (!ui->menuMap.contains("CWLevelUpMenu"))
				{
					ui->RegisterMenu("CWLevelUpMenu", CWLevelUpMenu::Create);
					REX::DEBUG("Registered 'CWLevelUpMenu'.");
				}
			}
		}

		namespace LevelUpMenu_Papyrus
		{
			void OpenLevelUpMenu_Papyrus(std::monostate)
			{
				menuModeType = LevelTypes::kLevelUp;
				ShowLvlUpMenu();
			}

			void OpenTagSkillsMenu_Papyrus(std::monostate, std::uint32_t tagPoints, bool reTag)
			{
				menuModeType = LevelTypes::kTagSkills;
				tagPointsValue = tagPoints;
				allowRetag = reTag;
				ShowLvlUpMenu();
			}

			void OpenSpecialRespecMenu_Papyrus(std::monostate)
			{
				menuModeType = LevelTypes::kSpecialRespec;
				ShowLvlUpMenu();
			}

			void OpenIntenseTrainingMenu_Papyrus(std::monostate)
			{
				menuModeType = LevelTypes::kIntenseTraining;
				ShowLvlUpMenu();
			}

			bool GetSkillTagged_Papyrus(std::monostate, RE::ActorValueInfo* mySkill)
			{
				return F4CWSerialization::IsSkillTagged(mySkill->GetFormID());
			}

			void SetSkillTagged_Papyrus(std::monostate, RE::ActorValueInfo* mySkill, bool bTag)
			{
				SetSkillTagged(mySkill, bTag);
			}

			bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
			{
				vm->BindNativeMethod("TCW:F4CW", "OpenLevelUpMenu", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::OpenLevelUpMenu_Papyrus, false, false);
				vm->BindNativeMethod("TCW:F4CW", "OpenTagSkillsMenu", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::OpenTagSkillsMenu_Papyrus, false, false);
				vm->BindNativeMethod("TCW:F4CW", "OpenSpecialRespecMenu", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::OpenSpecialRespecMenu_Papyrus, false, false);
				vm->BindNativeMethod("TCW:F4CW", "OpenIntenseTrainingMenu", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::OpenIntenseTrainingMenu_Papyrus, false, false);
				vm->BindNativeMethod("TCW:F4CW", "GetSkillTagged", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::GetSkillTagged_Papyrus, false, false);
				vm->BindNativeMethod("TCW:F4CW", "SetSkillTagged", F4CW_Menus::LevelUpMenu::LevelUpMenu_Papyrus::SetSkillTagged_Papyrus, false, false);
				return true;
			}
		}
	}
}