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


		static void Install() {

			ShowPlayerSkills::Install();
		}
	}
}