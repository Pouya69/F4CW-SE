#pragma once

#include <cstdint>
#include <string>
#include <RE/B/BGSInventoryItem.h>
#include <RE/E/ExtraDataList.h>
#include <RE/T/TESForm.h>
#include "Shared.h"
#include "ItemDegradation.h"
#include <RE/P/PlayerCharacter.h>
#include <Scaleform/G/GFx_FunctionHandler.h>
#include <Scaleform/G/GFx_Value.h>





/*
namespace InventoryUtils
{
    std::uint32_t GetPipboyInventoryObjectCount();
    std::string GetInventoryDisplayName(std::uint32_t index);
    std::uint32_t GetHandleIDByIndex(std::uint32_t index);
    std::uint32_t GetStackIDByIndex(std::uint32_t index);
    RE::TESForm* GetInventoryFormByHandleID(std::uint32_t HandleID);
    RE::TESForm* GetInventoryFormByIndex(std::uint32_t index);
    std::uint32_t GetIndexByInventoryItem(RE::BGSInventoryItem* item);
    RE::BGSInventoryItem* GetInventoryItemByHandleID(std::uint32_t HandleID);
    RE::BGSInventoryItem* GetInventoryItemByIndex(std::uint32_t index);
    RE::BGSInventoryItem::Stack* GetStackByStackID(RE::BGSInventoryItem* Item, int StackID);
    RE::ExtraDataList* GetExtraDataListByStackID(RE::BGSInventoryItem* Item, int StackID);
    RE::ExtraDataList* GetExtraDataListByIndex(std::uint32_t index);
}
*/

class HUD_Ready : public Scaleform::GFx::FunctionHandler {
    virtual void Call(const Params& a_params) {
        auto movie = a_params.movie;
        if (movie)
            Shared::HUD::UpdateMenus(movie->asMovieRoot);
    }
};

class HUD_GetCurrentCondition : public Scaleform::GFx::FunctionHandler {
    virtual void Call(const Params& a_params) {
        const F4CW::ItemDegradation::WeaponConditionData condition = F4CW::ItemDegradation::WeaponConditionData(RE::PlayerCharacter::GetSingleton());
        //const float result = condition.extraData ? condition.extraData->GetHealthPerc() : -1.0f;
        const float result = condition.extraData->GetHealthPerc();
        Scaleform::GFx::Value returnValue = result;
        a_params.retVal->SetElement(0, returnValue);
    }
};
