#pragma once

#include <detourXS/detourxs.h>
#include <REL/ID.h>
#include <REL/Relocation.h>
#include <Windows.h>
#include "debugLog.h"
#include <cstdarg>
#include <cstdint>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BGSInventoryList.h>
#include <RE/T/TESBoundObject.h>

namespace F4CW {
	namespace Hooks {
		namespace Registers {

			template<typename T>
			static void RegisterDetourFunction(DetourXS detour, const REL::ID functionID, const LPVOID detourFunctionAddress, REL::Relocation<T> &OriginalFunction, std::string_view functionName) {
				REL::Relocation<T> functionLocation{ functionID };
				if (detour.Create(reinterpret_cast<void*>(functionLocation.address()), detourFunctionAddress)) {
					LOG_INFO(std::format("Installed '{}' hook", detourFunctionAddress));
					OriginalFunction = reinterpret_cast<uintptr_t>(detour.GetTrampoline());
				}
				else {
					//LOG_CRITICAL(std::format("Failed to hook '{}'. Exiting...", functionName));
				}
			}

			void RegisterAllHooks();



			void Install();
		}
		
		float Hook_CombatFormulasCalcWeaponDamage(const RE::TESForm* a_actorForm, const RE::TESObjectWEAP::InstanceData* a_weapon, const RE::TESAmmo* a_ammo, float a_condition, float a_damageMultiplier);
		void Hook_BGSInventoryListAddItem(RE::BGSInventoryList* a_this, RE::TESBoundObject* a_boundObject, const RE::BGSInventoryItem::Stack* a_stack, std::uint32_t* a_oldCount, std::uint32_t* a_newCount);
		

	}
}