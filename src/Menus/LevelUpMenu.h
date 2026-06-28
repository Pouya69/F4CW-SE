#pragma once

#include <Scaleform/Scaleform.h>
#include "../GameForms.h"
#include "../skills.h"
#include "../Serialization.h"
#include <future>
#include <chrono>
#include "../Shared.h"
#include "../IGlobalConfig.h"
#include "../PerkHelpers.h"
#include <string.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <RE/A/ActorValueInfo.h>
#include <RE/A/ACTOR_VALUE_MODIFIER.h>
#include <RE/B/BGSPerk.h>
#include <RE/B/BSGFxShaderFXTarget.h>
#include <RE/B/BSScaleformManager.h>
#include <RE/B/BSStringT.h>
#include <RE/B/BSTArray.h>
#include <RE/B/BS_BUTTON_CODE.h>
#include <RE/B/ButtonEvent.h>
#include <RE/G/GameMenuBase.h>
#include <RE/H/HUDColorTypes.h>
#include <RE/I/IMenu.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESGlobal.h>
#include <RE/U/UI.h>
#include <RE/U/UIMessage.h>
#include <RE/U/UIMessageQueue.h>
#include <RE/U/UIUtils.h>
#include <RE/U/UI_DEPTH_PRIORITY.h>
#include <RE/U/UI_MENU_FLAGS.h>
#include <RE/U/UI_MESSAGE_TYPE.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_FunctionHandler.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Scaleform/G/GFx_Value.h>
#include <Scaleform/P/Ptr.h>
#include <REX/LOG.h>
#include <format>
#include <RE/B/BGSSoundDescriptorForm.h>
#include <RE/B/BSScript_IVirtualMachine.h>
#include <Windows.h>
#include <variant>
#include <RE/B/BSAudioManager.h>
#include <RE/B/BSISoundDescriptor.h>
#include <RE/B/BSSoundHandle.h>


namespace F4CW_Menus {
	namespace LevelUpMenu {

		extern bool bForceLevelUp;

		extern RE::BSSoundHandle previousSoundHandle;

		extern float perkPoints;

		extern std::uint32_t menuModeType;
		extern std::uint32_t tagPointsValue;

		extern bool allowRetag;

		extern RE::BSTArray<RE::ActorValueInfo*> scaleformSkills;

		namespace InitialValues {
			extern std::uint32_t barter;
			extern std::uint32_t bigGuns;
			extern std::uint32_t energyWeapons;
			extern std::uint32_t explosives;
			extern std::uint32_t lockpick;
			extern std::uint32_t medicine;
			extern std::uint32_t meleeWeapons;
			extern std::uint32_t repair;
			extern std::uint32_t science;
			extern std::uint32_t smallGuns;
			extern std::uint32_t sneak;
			extern ::uint32_t speech;
			extern std::uint32_t unarmed;
		}

		namespace TempModValues {
			extern std::uint32_t barter;
			extern std::uint32_t bigGuns;
			extern std::uint32_t energyWeapons;
			extern std::uint32_t explosives;
			extern std::uint32_t lockpick;
			extern std::uint32_t medicine;
			extern std::uint32_t meleeWeapons;
			extern std::uint32_t repair;
			extern std::uint32_t science;
			extern std::uint32_t smallGuns;
			extern std::uint32_t sneak;
			extern std::uint32_t speech;
			extern std::uint32_t unarmed;
		}

		void GetINIOptions();

		//	Returns true if forced level up is enabled
		bool GetForcedLevelUp();

		void InitializeValues();

		enum SkillArray
		{
			Barter = 0,
			BigGuns = 1,
			EnergyWeapons = 2,
			Explosives = 3,
			Lockpick = 4,
			Medicine = 5,
			MeleeWeapons = 6,
			Repair = 7,
			Science = 8,
			SmallGuns = 9,
			Sneak = 10,
			Speech = 11,
			Unarmed = 12
		};

		enum LevelTypes
		{
			kLevelUp = 0,			// Normal level up
			kTagSkills = 1,			// Tag skills mode
			kSpecialRespec = 2,		// Special respec mode
			kIntenseTraining = 3	// 'Intense Training' perk
		};

		enum PerkTypes
		{
			kDefault = 0,
			kStrength = 1,
			kPerception = 2,
			kEndurance = 3,
			kCharisma = 4,
			kIntelligence = 5,
			kAgility = 6,
			kLuck = 7,
			kIntenseTrainingPerk = 8,
			kCancel = 9
		};

		float GetInitialActorValue(RE::ActorValueInfo* a_skill);

		void PopulateSkillEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::ActorValueInfo* a_skill);

		bool ProcessSkillsList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		bool ProcessTagSkillsList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, std::uint32_t a_tagSkills, bool a_retag);

		void PopulateSpecialEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::ActorValueInfo* a_skill);

		bool ProcessSpecialList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		void PopulatePerkEntry(Scaleform::GFx::Value* a_destination, Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot, RE::BGSPerk* a_perk, bool a_eligibleOnly);

		bool ProcessPerkList(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		std::uint32_t GetSkillArrayIndexByEditorID(const char* myEditorID);

		float GetSkillPointsToAdd(std::uint16_t levelsToProgress = 1);

		bool CanLevelUpMenuBeShown();

		void ShowLvlUpMenu();

		// Waits for level up to be ready and then shows menu
		void WaitForLevelUpReady();

		void ModPerkCount(std::int8_t a_count);

		void SetSkillTagged(RE::ActorValueInfo* a_skill, bool a_tag);

		// Called from 'LevelIncrease::Event'
		void HandleLevelUp(std::uint16_t levelsToProgress = 1);

		void ModSkillByName(std::string a_skillName, std::uint32_t a_value, std::uint32_t a_baseValue);

		void RevertSkill(RE::ActorValueInfo* skill);

		// Check if player saved in the middle of a level up.
		void CheckForLevelUp();

		// Send initial data to AS3 for Level Up.
		void OnLevelUpStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		// Send initial data to AS3 for 'Tag Skills'.
		void OnTagSkillsStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		// Send initial data to AS3 for S.P.E.C.I.A.L respec - might be cut.
		void OnSpecialRespecStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		// Send initial data to AS3 for  Intense Training.
		void OnIntenseTrainingStart(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		void HandleLevelUpMenuOpen(Scaleform::Ptr<Scaleform::GFx::ASMovieRootBase> a_movieRoot);

		void CompleteLevelUp();

		class OpenMenu : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'OpenMenu' called from AS3.");
				LevelUpMenu::InitializeValues();
				// TODO - RegisterForInput
				LevelUpMenu::HandleLevelUpMenuOpen(a_params.movie->asMovieRoot);
			}
		};

		class CloseMenu : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'CloseMenu' called from AS3.");
				previousSoundHandle.Stop();
				RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
				if (RE::UI* ui = RE::UI::GetSingleton())
				{
					if (ui->menuMap.contains("CWLevelUpMenu"))
					{
						uiMessageQueue->AddMessage("CWLevelUpMenu", RE::UI_MESSAGE_TYPE::kHide);
					}
				}
				CompleteLevelUp();
			}
		};

		class SetSkills : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'SetSkills' called from AS3.");

				std::uint32_t skillsCount = a_params.args[0].GetArraySize();
				Scaleform::GFx::Value arrayElement, skillName, skillValue, skillBaseValue;
				previousSoundHandle.Stop();
				for (std::uint32_t i = 0; i < skillsCount; i++)
				{
					a_params.args[0].GetElement(i, &arrayElement);
					arrayElement.GetMember("sName", &skillName);
					arrayElement.GetMember("iValue", &skillValue);
					arrayElement.GetMember("iBaseValue", &skillBaseValue);

					auto skillValueNormal = 0;
					if (skillValue.IsNumber())
					{
						skillValueNormal = skillValue.GetNumber();
					}
					else if (skillValue.IsUInt())
					{
						skillValueNormal = skillValue.GetUInt();
					}

					auto skillBaseValueNormal = 0;
					if (skillBaseValue.IsNumber())
					{
						skillBaseValueNormal = skillBaseValue.GetNumber();
					}
					else if (skillBaseValue.IsUInt())
					{
						skillBaseValueNormal = skillBaseValue.GetUInt();
					}

					ModSkillByName(skillName.GetString(), skillValueNormal, skillBaseValueNormal);
				}

				ProcessPerkList(a_params.movie->asMovieRoot);
			}
		};

		class ResetSkills : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'ResetSkills' called from AS3.");
				ProcessSkillsList(a_params.movie->asMovieRoot);
			}
		};

		class ResetTagSkills : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'ResetTagSkills' called from AS3.");
				OnTagSkillsStart(a_params.movie->asMovieRoot);
			}
		};

		class BackToSkills : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'BackToSkills' called from AS3.");

				for (std::uint32_t skillEntry = 0; skillEntry < Skills::CWSkillsLevelUp.size(); skillEntry++)
				{
					RE::ActorValueInfo* skillAV = Skills::CWSkillsLevelUp.at(skillEntry);
					RevertSkill(skillAV);
				}

				a_params.movie->asMovieRoot->Invoke("root.Menu_mc.onSwitchBackToSkills", nullptr, nullptr, 0);
			}
		};

		class UpdatePerkMenu : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'UpdatePerksMenu' called from AS3.");
				ProcessPerkList(a_params.movie->asMovieRoot);
			}
		};

		class AddPerks : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'AddPerks' called from AS3.");
				std::uint32_t perkCount = a_params.args[0].GetArraySize();
				Scaleform::GFx::Value arrayElement, perkFormID;

				RE::PlayerCharacter* playerCharacter = RE::PlayerCharacter::GetSingleton();

				for (std::uint32_t i = 0; i < perkCount; i++) {
					a_params.args[0].GetElement(i, &arrayElement);
					arrayElement.GetMember("iFormID", &perkFormID);
					RE::TESForm* a_perkForm = RE::TESForm::GetFormByNumericID(perkFormID.GetUInt());
					RE::BGSPerk* a_perk = static_cast<RE::BGSPerk*>(a_perkForm);
					playerCharacter->AddPerk(a_perk);
					ModPerkCount(-1);
				}
			}
		};

		class TagSkills : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'TagSkills' called from AS3.");
				std::uint32_t skillsCount = a_params.args[0].GetArraySize();
				Scaleform::GFx::Value arrayElement, skillName, skillTagged;

				for (std::uint32_t i = 0; i < skillsCount; i++) {
					a_params.args[0].GetElement(i, &arrayElement);
					arrayElement.GetMember("sName", &skillName);
					arrayElement.GetMember("isTagged", &skillTagged);
					SetSkillTagged(GetSkillByName(skillName.GetString()), skillTagged.GetBoolean());
				}
			}
		};

		class PlayUISound : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'PlayUISound' called from AS3.");
				if (a_params.argCount < 1) return;
				if (!a_params.args[0].IsString()) return;

				auto uiSound = a_params.args[0].GetString();

				RE::UIUtils::PlayMenuSound(uiSound);
			}
		};

		class LearnSpecial : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'LearnSpecial' called from AS3.");
				std::uint32_t specialCount = a_params.args[0].GetArraySize();
				Scaleform::GFx::Value arrayElement, specialFormID, specialValue;

				auto player = RE::PlayerCharacter::GetSingleton();

				for (std::uint32_t i = 0; i < specialCount; i++) {

					a_params.args[0].GetElement(i, &arrayElement);
					arrayElement.GetMember("formID", &specialFormID);
					arrayElement.GetMember("iValue", &specialValue);

					RE::ActorValueInfo* special = static_cast<RE::ActorValueInfo*>(RE::TESForm::GetFormByNumericID(specialFormID.GetUInt()));
					
					auto specialValueNormal = 0;
					if (specialValue.IsNumber())
					{
						specialValueNormal = specialValue.GetNumber();
					}
					else if (specialValue.IsUInt())
					{
						specialValueNormal = specialValue.GetUInt();
					}

					player->SetBaseActorValue(*special, specialValueNormal);
					
					REX::DEBUG(std::format("Learned special: {} with new value: {}", special->GetFullName(), specialValueNormal).c_str());
					
				}
			}
		};

		class ResetSpecial : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'ResetSpecial' called from AS3.");
				LevelUpMenu::HandleLevelUpMenuOpen(a_params.movie->asMovieRoot);
			}
		};

		class PlaySkillSound : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{
				REX::DEBUG("'PlaySkillSound' called from AS3.");

				for (std::uint32_t i = 0; i < Skills::CWSkillsLevelUp.size(); i++)
				{
					RE::ActorValueInfo* myAV = Skills::CWSkillsLevelUp[i];
					if (!a_params.args[0].IsString()) {
						REX::WARN(std::format("PlaySkillSound: First param is not a string. Param type: {}", (std::int32_t) a_params.args[0].GetType()).c_str());
						break;
					}
					auto paramString = a_params.args[0].GetString();
					RE::ActorValueInfo* avFromParams = GetSkillByName(paramString);
					if (!avFromParams) {
						REX::WARN(std::format("PlaySkillSound: Param '{}' is not a registered skill. No sound will be played.", paramString).c_str());
						continue;
					}

					if ((std::uint32_t) myAV->GetFormID() == (std::uint32_t) avFromParams->GetFormID())
					{
						RE::BGSSoundDescriptorForm* mySkillSound = Skills::ScaleformSkillSounds[i];
						if (!mySkillSound) {
							REX::WARN(std::format("PlaySkillSound: Param '{}' matched a sound from ScaleformSkillSounds but the SoundDescriptor is null.", paramString).c_str());
							break;
						}
						
						previousSoundHandle.Stop();

						//auto uiSound = a_params.args[0].GetString();
						RE::BSSoundHandle handle;
						RE::BSISoundDescriptor::ExtraResolutionData* sounddata;
						const bool grabbed = RE::BSAudioManager::GetSingleton()->GetSoundHandle(handle, mySkillSound, 0.0f, 0);
						if (!grabbed) {
							REX::WARN(std::format("PlaySkillSound: Param '{}' could not be grabbed as a SoundHandle.", paramString).c_str());
							break;
						}
						handle.Play();

						previousSoundHandle.assumeSuccess = false;
						previousSoundHandle.soundID = handle.soundID;
						
						break;
					}
				}
			}
		};

		class PlayPerkSound : public Scaleform::GFx::FunctionHandler
		{
		public:
			virtual void Call(const Params& a_params)
			{

				REX::DEBUG(std::format("'PlayPerkSound' called from AS3. Param1: {}, type: {}", a_params.args[0].IsUInt() ? a_params.args[0].GetUInt() : -999999, (std::int32_t)a_params.args[0].GetType()).c_str());
				// TODO - might not be needed.
				if (!a_params.args[0].IsUInt()) {
					REX::WARN(std::format("PlayPerkSound: First param is not a UINT. Param type: {}", (std::int32_t)a_params.args[0].GetType()).c_str());
					return;
				}
				
				// @TODO: This makes perkIndex a very big number 1231231236 smth like that.
				std::uint32_t perkIndex = a_params.args[0].GetUInt();

				RE::BGSPerk* myPerk = Skills::CWPerksLevelUp[perkIndex];
				const int arraySize = Skills::ScaleformPerkSounds.size();
				if (arraySize <= perkIndex) {
					REX::WARN(std::format("PlayPerkSound: Perk index '{}' is more than array size '{}'", perkIndex, arraySize));
					return;
				}

				RE::BGSSoundDescriptorForm* myPerkSound = Skills::ScaleformPerkSounds[perkIndex];

				if (!myPerkSound) {
					REX::WARN(std::format("PlayPerkSound: Perk index '{}' matched a sound from ScaleformPerkSounds but the SoundDescriptor is null.", perkIndex).c_str());
					return;
				}

				previousSoundHandle.Stop();

				//auto uiSound = a_params.args[0].GetString();
				RE::BSSoundHandle handle;
				RE::BSISoundDescriptor::ExtraResolutionData* sounddata;
				const bool grabbed = RE::BSAudioManager::GetSingleton()->GetSoundHandle(handle, myPerkSound, 0.0f, 0);
				if (!grabbed) {
					REX::WARN(std::format("PlayPerkSound: Param '{}' could not be grabbed as a SoundHandle.", perkIndex).c_str());
					return;
				}
				handle.Play();

				previousSoundHandle.assumeSuccess = false;
				previousSoundHandle.soundID = handle.soundID;
			}
		};

		class CWLevelUpMenu :
			public RE::GameMenuBase
		{
		public:
			static IMenu* CreateCWLevelUpMenu(const RE::UIMessage&)
			{
				return new CWLevelUpMenu();
			}

			const char* sMenuName = "CWLevelUpMenu";

			void ProcessUserEventExternal(const char* controlName, bool isDown, std::uint32_t deviceType, RE::BS_BUTTON_CODE buttonCode)
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->GetMenuOpen(sMenuName))
				{
					RE::IMenu* menu = ui->GetMenu(sMenuName).get();
					Scaleform::GFx::ASMovieRootBase* movieRoot = menu->uiMovie->asMovieRoot.get();
					Scaleform::GFx::Value arguments[4];
					arguments[0] = controlName;
					arguments[1] = isDown;
					arguments[2] = deviceType;
					arguments[3] = static_cast<std::int32_t>(buttonCode);

					// REX::DEBUG("CWLevelUpMenu: USER INPUT TRIGGERED!");

					movieRoot->Invoke("root.Menu_mc.ProcessUserEventExternal", nullptr, arguments, 4);
				}
			}

			void OnButtonEvent(const RE::ButtonEvent* a_event) override
			{
				ProcessUserEventExternal(a_event->QUserEvent().c_str(), a_event->QHeldDown(), a_event->device.underlying(), a_event->GetBSButtonCode());
			}

			CWLevelUpMenu()
			{
				menuFlags.set(
					RE::UI_MENU_FLAGS::kPausesGame,
					RE::UI_MENU_FLAGS::kUsesCursor,
					RE::UI_MENU_FLAGS::kTopmostRenderedMenu,
					RE::UI_MENU_FLAGS::kUpdateUsesCursor,
					RE::UI_MENU_FLAGS::kUsesBlurredBackground
				);
				menuHUDMode = "SpecialMode";
				depthPriority = RE::UI_DEPTH_PRIORITY::kTerminal;
				const auto ScaleformManager = RE::BSScaleformManager::GetSingleton();

				[[maybe_unused]] const auto LoadMovieSuccess =
					ScaleformManager->LoadMovieEx(*this, "Interface/CWLevelUpMenu.swf"sv, "root", Scaleform::GFx::Movie::ScaleModeType::kExactFit);
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

				Shared::RegisterFunction<OpenMenu>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "OpenMenu");
				Shared::RegisterFunction<SetSkills>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "SetSkills");
				Shared::RegisterFunction<ResetSkills>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "ResetSkills");
				Shared::RegisterFunction<ResetTagSkills>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "ResetTagSkills");
				Shared::RegisterFunction<BackToSkills>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "BackToSkills");
				Shared::RegisterFunction<UpdatePerkMenu>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "UpdatePerkMenu");
				Shared::RegisterFunction<AddPerks>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "AddPerks");
				Shared::RegisterFunction<TagSkills>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "TagSkills");
				Shared::RegisterFunction<LearnSpecial>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "LearnSpecial");
				Shared::RegisterFunction<ResetSpecial>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "ResetSpecial");
				Shared::RegisterFunction<PlaySkillSound>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "PlaySkillSound");
				Shared::RegisterFunction<PlayPerkSound>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "PlayPerkSound");
				Shared::RegisterFunction<CloseMenu>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "CloseMenu");
				Shared::RegisterFunction<PlayUISound>(&bgsCodeObj, uiMovie.get()->asMovieRoot, "PlayUISound");

				movieRoot->Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);
			};

			static IMenu* Create(const RE::UIMessage&)
			{
				return new CWLevelUpMenu();
			}
		};

		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value);

		void RegisterMenu();

		namespace LevelUpMenu_Papyrus
		{
			void OpenLevelUpMenu_Papyrus(std::monostate);

			void OpenTagSkillsMenu_Papyrus(std::monostate, std::uint32_t tagPoints, bool reTag);

			void OpenSpecialRespecMenu_Papyrus(std::monostate);

			void OpenIntenseTrainingMenu_Papyrus(std::monostate);

			bool GetSkillTagged_Papyrus(std::monostate, RE::ActorValueInfo* mySkill);

			void SetSkillTagged_Papyrus(std::monostate, RE::ActorValueInfo* mySkill, bool bTag);

			bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
		}
		
	}
}