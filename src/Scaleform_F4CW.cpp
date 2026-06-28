#include "Scaleform_F4CW.h"
#include <cstdint>
#include <string>
#include <RE/B/BGSInventoryItem.h>
#include <RE/B/BSFixedString.h>
#include <RE/E/ExtraDataList.h>
#include <RE/P/PipboyArray.h>
#include <RE/P/PipboyPrimitiveValue.h>
#include <RE/T/TESForm.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_Value.h>
#include <chrono>
#include <format>
#include <thread>
#include <RE/B/BGSInventoryInterface.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/U/UIUtils.h>
#include <Scaleform/G/GFx_FunctionHandler.h>
#include <REX/LOG.h>
#include "debugLog.h"
#include "ItemDegradation.h"
#include "Menus/LevelUpMenu.h"
#include "Menus/PipboyTabs.h"
#include "Menus/RepairMenu.h"
#include "Shared.h"
#include <RE/A/Actor.h>
#include <RE/M/MenuControls.h>
#include <RE/P/PipboyManager.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/U/UI.h>

void GFxUtilities::SetScaleformValue(Scaleform::GFx::Value* dst, const char* name, Scaleform::GFx::Value value)
{
	dst->SetMember(name, &value);
}

void GFxUtilities::SetScaleformValue(Scaleform::GFx::Value* dst, const char* name, Scaleform::GFx::Value* value)
{
	dst->SetMember(name, value);
}

void GFxUtilities::RegisterString(Scaleform::GFx::Value* destination, Scaleform::GFx::ASMovieRootBase* root, const char* name, const char* str)
{
	Scaleform::GFx::Value* fxValue;
	root->CreateString(fxValue, str);
	destination->SetMember(name, *fxValue);
}

namespace InventoryUtils
{
	using namespace RE;

	std::uint32_t GetPipboyInventoryObjectCount()
	{
		return PipboyInventoryObjects.size();

		// RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		// return player->inventoryList->data.size();
	}

	std::string GetInventoryDisplayName(std::uint32_t index)
	{
		PipboyPrimitiveValue<BSFixedString>* pipboyObject = static_cast<PipboyPrimitiveValue<BSFixedString>*>(PipboyInventoryObjects.at(index)->memberMap.find(BSFixedString("text"))->second);
		
		// RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		// auto a = player->inventoryList->data.at(index).GetDisplayFullName()
		// return player->inventoryList->data.at(index).GetDisplayFullName;

		return pipboyObject->value.c_str();
	}

	std::uint32_t GetHandleIDByIndex(std::uint32_t index)
	{
		PipboyPrimitiveValue<std::uint32_t>* pipboyObject = static_cast<PipboyPrimitiveValue<std::uint32_t>*>(PipboyInventoryObjects.at(index)->memberMap.find(BSFixedString("HandleID"))->second);

		// RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		// auto a = player->inventoryList->data.at(index).GetDisplayFullName()

		return pipboyObject->value;
	}

	std::uint32_t GetStackIDByIndex(std::uint32_t index)
	{

		PipboyArray* StackIDs = static_cast<PipboyArray*>(PipboyInventoryObjects.at(index)->memberMap.find(BSFixedString("StackID"))->second);
		std::uint32_t Result = 0;

		for (int i = 0; i < StackIDs->elements.size(); i++)
		{
			PipboyPrimitiveValue<std::uint32_t>* pipboyObject = static_cast<PipboyPrimitiveValue<std::uint32_t>*>(StackIDs->elements.at(i));
			Result = pipboyObject->value;
		}

		return Result;
	}

	
	TESForm* GetInventoryFormByHandleID(std::uint32_t HandleID)
	{
		return BGSInventoryInterface::GetSingleton()->RequestInventoryItem(HandleID)->object;
	}
	

	TESForm* GetInventoryFormByIndex(std::uint32_t index)
	{
		PipboyPrimitiveValue<std::uint32_t>* pipboyObject = static_cast<PipboyPrimitiveValue<std::uint32_t>*>(PipboyInventoryObjects.at(index)->memberMap.find(BSFixedString("HandleID"))->second);
		std::uint32_t HandleID = pipboyObject->value;

		// RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		// return player->inventoryList->data.at(index).object;

		return GetInventoryFormByHandleID(HandleID);
	}

	
	const BGSInventoryItem* GetInventoryItemByHandleID(std::uint32_t HandleID)
	{
		return RE::BGSInventoryInterface::GetSingleton()->RequestInventoryItem(HandleID);
	}
	

	const BGSInventoryItem* GetInventoryItemByIndex(std::uint32_t index)
	{
		// auto player = RE::PlayerCharacter::GetSingleton();
		PipboyPrimitiveValue<std::uint32_t>* pipboyObject = static_cast<PipboyPrimitiveValue<std::uint32_t>*>(PipboyInventoryObjects.at(index)->memberMap.find(BSFixedString("HandleID"))->second);
		std::uint32_t HandleID = pipboyObject->value;
		return GetInventoryItemByHandleID(HandleID);
		// return &player->inventoryList->data.at(index);
	}

	const RE::BGSInventoryItem* GetCurrentEquippedWeapon(RE::Actor* a_actor)
	{
		a_actor->inventoryList->rwLock.lock_read();
		for (RE::BGSInventoryItem& inventoryItem : a_actor->inventoryList->data) {
			if (!inventoryItem.IsEquipped(0))
				continue;

			RE::TESObjectWEAP* weapon = static_cast<RE::TESObjectWEAP*>(inventoryItem.object);
			if (inventoryItem.object && weapon->formType == RE::ENUM_FORM_ID::kWEAP) {
				a_actor->inventoryList->rwLock.unlock_read();
				return &inventoryItem;
			}
		}
		a_actor->inventoryList->rwLock.unlock_read();


		return nullptr;
	}

	std::uint32_t GetIndexByInventoryItem(const BGSInventoryItem* item)
	{
		std::uint32_t inventoryCount = GetPipboyInventoryObjectCount();

		for (std::uint32_t i = 0; i < inventoryCount; i++)
		{
			const BGSInventoryItem* iter = GetInventoryItemByIndex(i);

			if (item == iter)
			{
				return i;
			}
		}

		return -1;
	}

	const BGSInventoryItem::Stack* GetStackByStackID(const BGSInventoryItem* Item, int StackID)
	{
		BGSInventoryItem::Stack* traverse = Item->stackData.get();
		if (!traverse)
			return nullptr;

		while (StackID != 0) {
			traverse = traverse->nextStack.get();
			if (!traverse)
				return nullptr;
			StackID--;
		}

		return traverse;
	}

	ExtraDataList* GetExtraDataListByStackID(const BGSInventoryItem* Item, int StackID)
	{
		if (!Item)
			return nullptr;

		const BGSInventoryItem::Stack* stack = GetStackByStackID(Item, StackID);
		return stack->extra.get() ? stack->extra.get() : nullptr;
	}

	ExtraDataList* GetExtraDataListByIndex(std::uint32_t index)
	{
		const BGSInventoryItem* Item = GetInventoryItemByIndex(index);
		std::uint32_t              StackID = GetStackIDByIndex(index);

		if (!Item)
			return nullptr;

		const BGSInventoryItem::Stack* stack = GetStackByStackID(Item, StackID);
		return stack->extra.get() ? stack->extra.get() : nullptr;
	}
}

void Pipboy_Repair::Call(const Params& a_params)
{
	std::uint32_t arraySize = a_params.args[0].GetArraySize();
	Scaleform::GFx::Value arrayElement;
	Scaleform::GFx::Value menuType;
	Scaleform::GFx::Value invObjectArray;

	std::uint32_t iMenuType;
	F4CW_Menus::RepairMenu::InvObject currentInvObject;

	for (int i = 0; i < arraySize; i++)
	{
		a_params.args[0].GetElement(i, &arrayElement);
		arrayElement.GetMember("iCurrentTab", &menuType);
		arrayElement.GetMember("invItem", &invObjectArray);
		invObjectArray.GetElement(0, &invObjectArray);

		iMenuType = menuType.GetInt();
		currentInvObject = F4CW_Menus::RepairMenu::RepairMenuFunctions::CreateInvObjectFromArray(invObjectArray);
	}

	REX::DEBUG("Pipboy Menu: Repair Button Pressed");
	F4CW_Menus::RepairMenu::RepairMenuFunctions::OpenRepairFromPipboy(iMenuType, currentInvObject);
	// F4CW_Menus::PipboyTabs::CloseMenu();
	// F4CW_Menus::PipboyTabs::RegisterForInput(false);
}

void Pipboy_CheckInventoryItem::Call(const Params& a_params)
{
	const int index = a_params.args[0].GetInt();

	const bool result = F4CW_Menus::RepairMenu::RepairMenuFunctions::CheckInventoryForEligibleRepair(index);

	*a_params.retVal = result;
}

void RepairMenu_CloseMenu::Call(const Params& a_params)
{
	F4CW_Menus::RepairMenu::RepairMenuFunctions::RegisterForInput(false);
	a_params.movie->asMovieRoot->Invoke("root.Menu_mc.onCodeObjDestruction", nullptr, nullptr, 0);
	//Scaleform::OpenMenu_Internal("HUDMenu");
	F4CW_Menus::RepairMenu::CloseRepairMenu();
	// Scaleform::CloseMenu_Internal("RepairMenu");
	F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuClose();

	if (RE::UI::GetSingleton()->GetMenuOpen("PipboyMenu")) {
		F4CW_Menus::PipboyTabs::RegisterForInput(true);
		RE::PipboyManager::GetSingleton()->inputEventHandlingEnabled = true;
		auto menuControls = RE::MenuControls::GetSingleton();
		//auto handler = reinterpret_cast<RE::BSInputEventUser*>(menuControls->pipboyHandler);
		//menuControls->RegisterHandler(handler);
	}
}

void Scaleform_PlayUISound::Call(const Params& a_params)
{
	if (!a_params.args[0].IsString())
	{
		return;
	}
	else
	{
		const std::string soundS = a_params.args[0].GetString();
		REX::DEBUG(std::format("Scaleform_PlayUISound: Playing UI sound: {}", soundS).c_str());

		RE::UIUtils::PlayMenuSound(soundS.c_str());
	}
}

void RepairMenu_RepairItemPipboy::Call(const Params& a_params)
{
	//	this will pass through the object we are using to repair the current object
	std::uint32_t repairCount = a_params.args[0].GetArraySize(); //	this should always be 1 in this case, but just to be sure
	Scaleform::GFx::Value arrayElement;
	Scaleform::GFx::Value handleID;
	Scaleform::GFx::Value stackID;
	Scaleform::GFx::Value newCondition;

	REX::DEBUG("RepairMenu Pipboy: Repairing Items");

	//F4CW_Menus::PipboyTabs::CloseMenu();

	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	for (int i = 0; i < repairCount; i++)
	{
		a_params.args[0].GetElement(i, &arrayElement);
		arrayElement.GetMember("iHandleID", &handleID);
		arrayElement.GetMember("iStackID", &stackID);
		arrayElement.GetMember("fCondition", &newCondition);

		const RE::BGSInventoryItem* myItem = InventoryUtils::GetInventoryItemByHandleID(handleID.GetUInt());

		F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItemPipboy(myItem, newCondition.GetNumber());
		F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuOpen(a_params.movie->asMovieRoot.get(), -1);
	}
}

void RepairMenu_RepairItems::Call(const Params& a_params)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	std::uint32_t repairCount = a_params.args[0].GetArraySize();
	Scaleform::GFx::Value arrayElement;
	Scaleform::GFx::Value handleID;
	Scaleform::GFx::Value stackID;
	Scaleform::GFx::Value newCondition;
	Scaleform::GFx::Value repairCost;

	int totalRepairCost = 0;

	REX::DEBUG("RepairMenu: Repairing Items");

	for (int i = 0; i < repairCount; i++)
	{
		a_params.args[0].GetElement(i, &arrayElement);
		arrayElement.GetMember("iHandleID", &handleID);
		arrayElement.GetMember("iStackID", &stackID);
		arrayElement.GetMember("fCondition", &newCondition);
		arrayElement.GetMember("fCost", &repairCost);

		const RE::BGSInventoryItem* myItem = InventoryUtils::GetInventoryItemByHandleID(handleID.GetUInt());

		if (myItem->object->GetFormType() == RE::ENUM_FORM_ID::kWEAP)
		{
			F4CW::ItemDegradation::WeaponConditionData myWeapon(player, myItem->object, myItem->stackData->extra.get());
			F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItem(myWeapon, newCondition.GetNumber());
		}
		else if (myItem->object->GetFormType() == RE::ENUM_FORM_ID::kARMO)
		{
			F4CW::ItemDegradation::ArmorConditionData myArmor(player, myItem->object, myItem->stackData->extra.get());
			F4CW_Menus::RepairMenu::RepairMenuFunctions::RepairItem(myArmor, newCondition.GetNumber());
		}

		totalRepairCost += repairCost.GetInt();
	}

	F4CW_Menus::RepairMenu::RepairMenuFunctions::GiveCapsToVendor(totalRepairCost);
	
	RE::UIUtils::PlayMenuSound("OBJLunchboxKidsRobotBuild");
	// RE::SendHUDMessage::ShowHUDMessage(std::format("Removed {}.", repairCount).c_str(), "", false, true);
	F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuOpen(a_params.movie->asMovieRoot.get(), totalRepairCost);
}

void RepairMenu_OpenMenu::Call(const Params& a_params)
{
	REX::DEBUG("RepairMenu: OpenMenu called from AS3");
	
	if (RE::UI::GetSingleton()->GetMenuOpen("PipboyMenu")) {
		F4CW_Menus::PipboyTabs::RegisterForInput(false);
		RE::PipboyManager::GetSingleton()->inputEventHandlingEnabled = false;
		auto menuControls = RE::MenuControls::GetSingleton();
		// auto handler = reinterpret_cast<RE::BSInputEventUser*>(menuControls->pipboyHandler);
		// menuControls->UnregisterHandler(handler);
		// static_cast<RE::BSInputEventUser*>();
	}


	

	F4CW_Menus::RepairMenu::RepairMenuFunctions::HandleMenuOpen(a_params.movie->asMovieRoot.get());
	// Scaleform::CloseMenu_Internal("PipboyMenu");
}

void Pipboy_CheckWorkshopTab::Call(const Params& a_params)
{
	const bool result = F4CW_Menus::RepairMenu::RepairMenuFunctions::CheckWorkshopTab();

	*a_params.retVal = result;
}

void Pipboy_Ready::Call(const Params& a_params)
{
	REX::DEBUG("Pipboy_Ready called");
	F4CW_Menus::PipboyTabs::RegisterForInput(true);
	F4CW_Menus::PipboyTabs::UpdateMenus(a_params.movie->asMovieRoot.get());
}

void Debug_ActionScript::Call(const Params& a_params)
{
	std::string s = std::format("Type is not registered for debug: {}", (std::int32_t) a_params.args[0].GetType());
	switch (a_params.args[0].GetType()) {
		case Scaleform::GFx::Value::ValueType::kUInt:
			s = std::to_string(a_params.args[0].GetUInt());
			break;
		case Scaleform::GFx::Value::ValueType::kInt:
			s = std::to_string(a_params.args[0].GetInt());
			break;
		case Scaleform::GFx::Value::ValueType::kBoolean:
			s = std::to_string(a_params.args[0].GetBoolean());
			break;
		case Scaleform::GFx::Value::ValueType::kString:
			s = a_params.args[0].GetString();
			break;
		default:
			break;
	}

	REX::DEBUG(std::format("Debug_ActionScript: {}", s).c_str());
}

void Pipboy_UpdateItemCardsOnSection::Call(const Params& a_params)
{
	const std::uint32_t section = a_params.args[0].GetUInt();
	// Or GetUInt() ?
	LOG_TO_CONSOLE(std::format("Pipboy_UpdateItemCardsOnSection type: {}", section).c_str());
	F4CW_Menus::PipboyTabs::UpdateItemCardsOnSection(section);
}

void HUD_Ready::Call(const Params& a_params)
{
	auto movie = a_params.movie;
	if (movie)
		Shared::HUD::UpdateMenus(movie->asMovieRoot);
}

void HUD_GetCurrentCondition::Call(const Params& a_params)
{
	const F4CW::ItemDegradation::WeaponConditionData condition = F4CW::ItemDegradation::WeaponConditionData(RE::PlayerCharacter::GetSingleton());
	//const float result = condition.extraData ? condition.extraData->GetHealthPerc() : -1.0f;
	const float result = condition.extraData->GetHealthPerc();
	Scaleform::GFx::Value returnValue = result;
	a_params.retVal->SetElement(0, returnValue);
}

void Pipboy_CheckForcedLevelUp::Call(const Params& a_params)
{
	const bool result = F4CW_Menus::LevelUpMenu::GetForcedLevelUp();

	*a_params.retVal = result;
}

void WaitForRepairMenu()
{}

void Pipboy_AddCND_ForItemCard::Call(const Params& a_params)
{
	Scaleform::GFx::Value handleIDValue;
	float returnValue = -1.0f;
	std::int32_t index = a_params.args[0].GetInt();
	const RE::BGSInventoryItem* currentHoveredItem = InventoryUtils::GetInventoryItemByIndex(index);

	switch (currentHoveredItem->object->GetFormType())
	{
	case RE::ENUM_FORM_ID::kWEAP:
		returnValue = F4CW::ItemDegradation::WeaponConditionData(RE::PlayerCharacter::GetSingleton(), currentHoveredItem->object, currentHoveredItem->stackData->extra.get()).extraData->GetHealthPerc();
	case RE::ENUM_FORM_ID::kARMO:
		returnValue = F4CW::ItemDegradation::ArmorConditionData(RE::PlayerCharacter::GetSingleton(), currentHoveredItem->object, currentHoveredItem->stackData->extra.get()).extraData->GetHealthPerc();
	default:
		break;
	}

	REX::DEBUG(std::format("AddCND_ForItemCard - Called. Return Value: {}", returnValue).c_str());
	*a_params.retVal = returnValue;

	// a_params.args.getu
	// const F4CW::ItemDegradation::WeaponConditionData condition = F4CW::ItemDegradation::WeaponConditionData(RE::PlayerCharacter::GetSingleton());
}

void PipboyInventory_Ready::Call(const Params& a_params)
{
	REX::DEBUG("AS3, PipboyInventoryRead - Called.");
}

void Pipboy_IsRepairMenuOpen::Call(const Params& a_params)
{
	auto ui = RE::UI::GetSingleton();
	RE::BSFixedString repairMenuStr("RepairMenu");

	*a_params.retVal = ui ? RE::UI::GetSingleton()->GetMenuOpen(repairMenuStr) : false;
}
