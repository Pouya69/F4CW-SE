#pragma once
#include "debugLog.h"
#include <RE/S/SCRIPT_PARAMETER.h>
#include <RE/T/TESObjectREFR.h>
#include <string.h>
#include <algorithm>
#include <RE/S/Script.h>
#include <RE/S/ScriptLocals.h>
#include <RE/S/SCRIPT_FUNCTION.h>
#include <cstdint>
#include <array>
#include <RE/S/SCRIPT_PARAM_TYPE.h>
#include "skills.h"
#include <string>
#include <string_view>
#include <variant>
#include <RE/P/PlayerCharacter.h>
#include <format>
#include "ItemDegradation.h"

namespace F4CW {
	namespace ObScript {

		// ShowPlayerSkills SkillName ====> Will show the skillName requested.
		class ShowPlayerSkills {
		public:
			static void Install() {
				const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
				const auto it = std::find_if(
					functions.begin(),
					functions.end(),
					[&](auto&& a_elem) {
						return _stricmp(a_elem.functionName, "ShowNodes") == 0;
					});

				if (it == functions.end()) {
					LOG_WARNING("Failed to reigster console command: 'ShowNodes'");
					return;
				}

				static std::array params{
					RE::SCRIPT_PARAMETER{"SkillName", RE::SCRIPT_PARAM_TYPE::kChar, true},
				};

				*it = RE::SCRIPT_FUNCTION{ LONG_NAME.data(), SHORT_NAME.data(), it->output };
				it->helpString = HelpString().data();
				it->referenceFunction = false;
				it->paramCount = static_cast<std::uint16_t>(params.size());
				it->parameters = params.data();
				it->executeFunction = Execute;

				LOG_INFO("Registered 'ShowNodes' console command");
			}

		private:
			static bool Execute(
				const RE::SCRIPT_PARAMETER* a_parameters,
				const char* a_compiledParams,
				RE::TESObjectREFR* a_refObject,
				RE::TESObjectREFR* a_container,
				RE::Script* a_script,
				RE::ScriptLocals* a_scriptLocals,
				float&,
				std::uint32_t& a_offset)
			{
				std::array<char, 0x200> skillName = {'\0'};

				auto paramsParsed = RE::Script::ParseParameters(
					a_parameters,
					a_compiledParams,
					a_offset,
					a_refObject,
					a_container,
					a_script,
					a_scriptLocals,
					skillName.data()
				);

				if (!paramsParsed || skillName[0] == '\0') {
					CW_SkillsPapyrus::DEBUG_LogSkillsToConsole_Papyrus(std::monostate(), RE::PlayerCharacter::GetSingleton());
					return true;
				}

				auto skill = GetSkillByName(skillName.data());

				if (!skill) {
					LOG_TO_CONSOLE("Skill not found: " + *skillName.data());
					return true;
				}

				CW_SkillsPapyrus::DEBUG_LogSkillToConsole_Papyrus(std::monostate(), RE::PlayerCharacter::GetSingleton(), skill);

				

				return true;
			}

			[[nodiscard]] static const std::string& HelpString()
			{
				static auto help = []()
					{
						std::string buf;
						buf += "Show the current player skills.\nadd a param to show a specific skill."sv;
						return buf;
					}();
				return help;
			}

			static constexpr auto LONG_NAME = "ShowPlayerSkills"sv;
			static constexpr auto SHORT_NAME = "shpk"sv;
		};


		class ShowWeaponCondition {
		public:
			static void Install() {
				const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
				const auto it = std::find_if(
					functions.begin(),
					functions.end(),
					[&](auto&& a_elem) {
						return _stricmp(a_elem.functionName, "ShowAnim") == 0;
					});

				if (it == functions.end()) {
					LOG_WARNING("Failed to reigster console command: 'ShowAnim'");
					return;
				}

				// static std::array params{
				// 	RE::SCRIPT_PARAMETER{"SkillName", RE::SCRIPT_PARAM_TYPE::kChar, true},
				// };

				*it = RE::SCRIPT_FUNCTION{ LONG_NAME.data(), SHORT_NAME.data(), it->output };
				it->helpString = HelpString().data();
				it->referenceFunction = false;
				// it->paramCount = static_cast<std::uint16_t>(params.size());
				// it->parameters = params.data();
				it->executeFunction = Execute;

				LOG_INFO("Registered 'ShowAnim' console command");
			}

		private:
			static bool Execute(
				const RE::SCRIPT_PARAMETER* a_parameters,
				const char* a_compiledParams,
				RE::TESObjectREFR* a_refObject,
				RE::TESObjectREFR* a_container,
				RE::Script* a_script,
				RE::ScriptLocals* a_scriptLocals,
				float&,
				std::uint32_t& a_offset)
			{
				/*
				std::array<char, 0x200> skillName = { '\0' };
				
				auto paramsParsed = RE::Script::ParseParameters(
					a_parameters,
					a_compiledParams,
					a_offset,
					a_refObject,
					a_container,
					a_script,
					a_scriptLocals,
					skillName.data()
				);

				if (!paramsParsed || skillName[0] == '\0') {
					CW_SkillsPapyrus::DEBUG_LogSkillsToConsole_Papyrus(std::monostate(), RE::PlayerCharacter::GetSingleton());
					return true;
				}

				auto skill = GetSkillByName(skillName.data());

				if (!skill) {
					LOG_TO_CONSOLE("Skill not found: " + *skillName.data());
					return true;
				}
				*/
				RE::EquippedItem equippedItem = RE::PlayerCharacter::GetSingleton()->currentProcess->middleHigh->equippedItems[0];

				auto cond = ItemDegradation::WeaponConditionData(static_cast<RE::TESObjectREFR*>(equippedItem.item.object));

				RE::TESObjectWEAP* weaponObject = static_cast<RE::TESObjectWEAP*>(cond.Form);
				if (!weaponObject) {
					LOG_TO_CONSOLE("Equipped item is not a weapon!");
					return true;
				}
				if (weaponObject->weaponData.type == RE::WEAPON_TYPE::kGrenade || weaponObject->weaponData.type == RE::WEAPON_TYPE::kMine) {
					LOG_TO_CONSOLE("Equipped item is not a weapon! It is a mine or grenade!");
					return true;
				}

				LOG_TO_CONSOLE(std::format("Weapon '{}' condition: {}%", weaponObject->GetFullName(), cond.extraData->GetHealthPerc()).c_str());

				// CW_SkillsPapyrus::DEBUG_LogSkillToConsole_Papyrus(std::monostate(), RE::PlayerCharacter::GetSingleton(), skill);



				return true;
			}

			[[nodiscard]] static const std::string& HelpString()
			{
				static auto help = []()
					{
						std::string buf;
						buf += "Show the current equipped weapon's condition in %."sv;
						return buf;
					}();
				return help;
			}

			static constexpr auto LONG_NAME = "ShowWeaponCondition"sv;
			static constexpr auto SHORT_NAME = "shwc"sv;
		};

		class ShowArmorCondition {
		public:
			static void Install() {
				const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
				const auto it = std::find_if(
					functions.begin(),
					functions.end(),
					[&](auto&& a_elem) {
						return _stricmp(a_elem.functionName, "ShowScenegraph") == 0;
					});

				if (it == functions.end()) {
					LOG_WARNING("Failed to reigster console command: 'ShowScenegraph'");
					return;
				}

				// static std::array params{
				// 	RE::SCRIPT_PARAMETER{"SkillName", RE::SCRIPT_PARAM_TYPE::kChar, true},
				// };

				*it = RE::SCRIPT_FUNCTION{ LONG_NAME.data(), SHORT_NAME.data(), it->output };
				it->helpString = HelpString().data();
				it->referenceFunction = false;
				// it->paramCount = static_cast<std::uint16_t>(params.size());
				// it->parameters = params.data();
				it->executeFunction = Execute;

				LOG_INFO("Registered 'ShowScenegraph' console command");
			}

		private:
			static bool Execute(
				const RE::SCRIPT_PARAMETER* a_parameters,
				const char* a_compiledParams,
				RE::TESObjectREFR* a_refObject,
				RE::TESObjectREFR* a_container,
				RE::Script* a_script,
				RE::ScriptLocals* a_scriptLocals,
				float&,
				std::uint32_t& a_offset)
			{
			
					
				RE::Actor* playerCharacter = static_cast<RE::Actor*>(a_refObject);
				if (!playerCharacter)
					playerCharacter = RE::PlayerCharacter::GetSingleton();

				RE::EquippedItem equippedItem = playerCharacter->currentProcess->middleHigh->equippedItems[0];

				std::string buf = "Current Armors: ";

				for (RE::BGSInventoryItem& inventoryItem : playerCharacter->inventoryList->data) {
					if (!inventoryItem.IsEquipped(0))
						continue;

					RE::TESObjectARMO* armor = static_cast<RE::TESObjectARMO*>(inventoryItem.object);
					// auto cond = ItemDegradation::ArmorConditionData(armor->);
					if (inventoryItem.object && armor->formType == RE::ENUM_FORM_ID::kARMO) {
						RE::ExtraDataList* armorExtraData = inventoryItem.stackData->extra.get();
						// If armor does not have kHealth, it would return -1.
						buf += std::format("{}: {} |\t", armor->GetFullName(), armorExtraData->GetHealthPerc());
						
					}
				}


				LOG_TO_CONSOLE(buf.c_str());

				// CW_SkillsPapyrus::DEBUG_LogSkillToConsole_Papyrus(std::monostate(), RE::PlayerCharacter::GetSingleton(), skill);



				return true;
			}

			[[nodiscard]] static const std::string& HelpString()
			{
				static auto help = []()
					{
						std::string buf;
						buf += "Show the current equipped armors' condition in %."sv;
						return buf;
					}();
				return help;
			}

			static constexpr auto LONG_NAME = "ShowArmorCondition"sv;
			static constexpr auto SHORT_NAME = "shac"sv;
		};


		static void Install() {

			ShowPlayerSkills::Install();
			ShowWeaponCondition::Install();
			ShowArmorCondition::Install();
		}
	}
}