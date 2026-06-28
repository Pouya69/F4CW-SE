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
#include "Menus/PipboyTabs.h"
#include "Menus/RepairMenu.h"
#include "Menus/LevelUpMenu.h"
#include <RE/P/PipboyDataManager.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <REX/LOG.h>
#include "debugLog.h"




#define PipboyInventoryObjects RE::PipboyDataManager::GetSingleton()->inventoryData.sortedItems

#define GetTableItem(Index, Name)\
    (PipboyInventoryObjects[Index]->table.Find(&BSFixedString(Name)))

namespace InventoryUtils
{
    std::uint32_t GetPipboyInventoryObjectCount();
    std::string GetInventoryDisplayName(std::uint32_t index);
    std::uint32_t GetHandleIDByIndex(std::uint32_t index);
    std::uint32_t GetStackIDByIndex(std::uint32_t index);
    RE::TESForm* GetInventoryFormByHandleID(std::uint32_t HandleID);
    RE::TESForm* GetInventoryFormByIndex(std::uint32_t index);
    std::uint32_t GetIndexByInventoryItem(const RE::BGSInventoryItem* item);
    const RE::BGSInventoryItem* GetInventoryItemByHandleID(std::uint32_t HandleID);
    const RE::BGSInventoryItem* GetInventoryItemByIndex(std::uint32_t index);
    const RE::BGSInventoryItem* GetCurrentEquippedWeapon(RE::Actor* a_actor);
    const RE::BGSInventoryItem::Stack* GetStackByStackID(const RE::BGSInventoryItem* Item, int StackID);
    RE::ExtraDataList* GetExtraDataListByStackID(const RE::BGSInventoryItem* Item, int StackID);
    RE::ExtraDataList* GetExtraDataListByIndex(std::uint32_t index);

}


enum PipboyInvPages
{
    kPage_Weapons = 0,
    kPage_Apparel,
    kPage_Aid,
    kPage_Misc,
    kPage_Junk,
    kPage_Mods,
    kPage_Ammo
};

namespace GFxUtilities
{
    void RegisterString(Scaleform::GFx::Value* destination, Scaleform::GFx::ASMovieRootBase* root, const char* name, const char* str);

    void SetScaleformValue(Scaleform::GFx::Value* dst, const char* name, Scaleform::GFx::Value value);

    void SetScaleformValue(Scaleform::GFx::Value* dst, const char* name, Scaleform::GFx::Value* value);
}


class HUD_Ready : public Scaleform::GFx::FunctionHandler {
    virtual void Call(const Params& a_params);
};

class Pipboy_Ready : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class PipboyInventory_Ready : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Debug_ActionScript : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_Repair : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_AddCND_ForItemCard : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_IsRepairMenuOpen : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_UpdateItemCardsOnSection : public Scaleform::GFx::FunctionHandler {
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_CheckInventoryItem : public Scaleform::GFx::FunctionHandler {
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_CheckForcedLevelUp : public Scaleform::GFx::FunctionHandler {
public:

    virtual void Call(const Params& a_params);
};

class Pipboy_CheckWorkshopTab : public Scaleform::GFx::FunctionHandler {
public:

    virtual void Call(const Params& a_params);
};

void WaitForRepairMenu();

class RepairMenu_OpenMenu : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class RepairMenu_RepairItems : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class RepairMenu_RepairItemPipboy : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class RepairMenu_CloseMenu : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};

class Scaleform_PlayUISound : public Scaleform::GFx::FunctionHandler
{
public:

    virtual void Call(const Params& a_params);
};





class HUD_GetCurrentCondition : public Scaleform::GFx::FunctionHandler {
    virtual void Call(const Params& a_params);
};
