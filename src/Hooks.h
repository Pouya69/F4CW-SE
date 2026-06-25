#pragma once

#include <detourXS/detourxs.h>
#include <REL/ID.h>
#include <REL/Relocation.h>
#include "debugLog.h"
#include <cstdarg>
#include <cstdint>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BGSInventoryList.h>
#include <RE/T/TESBoundObject.h>
#include <string_view>
#include <RE/A/Actor.h>
#include <RE/A/ActorValueInfo.h>
#include <RE/A/AlchemyItem.h>
#include <RE/B/BGSEquipIndex.h>
#include <RE/B/BGSObjectInstance.h>
#include <RE/T/TESAmmo.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESObjectREFR.h>
#include <RE/T/TESObjectWEAP.h>
#include <Windows.h>
#include <RE/A/ActorEquipManager.h>
#include <RE/B/BGSBodyPartData.h>
#include <RE/B/BGSEquipSlot.h>
#include <RE/B/BGSTypedFormValuePair.h>
#include <RE/B/BSTArray.h>
#include <RE/B/BSTTuple.h>
#include <RE/E/ExtraDataList.h>

namespace F4CW {
	namespace Hooks {
		namespace Registers {

			template<typename T>
			static void RegisterDetourFunction(DetourXS& detour, const REL::ID functionID, const LPVOID& detourFunctionAddress, REL::Relocation<T> &OriginalFunction, std::string_view functionName) {
				REL::Relocation<T> functionLocation{ functionID };
				if (detour.Create(reinterpret_cast<void*>(functionLocation.address()), detourFunctionAddress)) {
					LOG_INFO(std::format("Installed '{}' hook", functionName));
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
		float Hook_GetEquippedDamageResistance(RE::Actor* a_actor, const RE::ActorValueInfo* a_info);
		void Hook_TESObjectWeaponFire(const RE::BGSObjectInstanceT<RE::TESObjectWEAP>* a_weapon, RE::TESObjectREFR* a_source, RE::BGSEquipIndex a_equipIndex, RE::TESAmmo* a_ammo, RE::AlchemyItem* a_poiso);
		float Hook_CombatFormulasCalcTargetedLimbDamage(RE::Actor* a_actor, const RE::BGSBodyPart* a_bodyPart, float a_physicalDamage, RE::BSTArray<RE::BSTTuple<RE::TESForm*, RE::BGSTypedFormValuePair::SharedVal>, RE::BSTArrayHeapAllocator>* a_damageTypes);
		void Hook_ExtraDataListSetHealthPerc(RE::ExtraDataList* a_this, float a_health);
		float Hook_GetWeaponDisplayRateOfFire(const RE::TESObjectWEAP& a_weapon, const RE::TESObjectWEAP::InstanceData* a_data);

		void Hook_ClosedownPipboy(RE::PipboyManager* a_manager);
		void Hook_LowerPipboy(RE::PipboyManager* a_manager, RE::PipboyManager::LOWER_REASON a_reason);

		bool Hook_EquipObject(
			RE::ActorEquipManager* a_actorEquipManager,
			RE::Actor* a_actor,
			const RE::BGSObjectInstance& a_object,
			std::uint32_t            a_stackID,
			std::uint32_t            a_number,
			const RE::BGSEquipSlot* a_slot,
			bool                     a_queueEquip,
			bool                     a_forceEquip,
			bool                     a_playSounds,
			bool                     a_applyNow,
			bool                     a_locked);

		void Hook_HandleItemEquip(RE::Actor* a_this, bool bCullBone);

		void Hook_OnPipboyClosed(RE::PipboyManager* a_pipboyManager);

		void Hook_SetCurrentAmmoCount(RE::Actor* a_this, RE::BGSEquipIndex a_equipIndex, std::uint32_t a_count);

	}
}