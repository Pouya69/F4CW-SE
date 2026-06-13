#include "main.h"

#include <F4SE/API.h>
#include <F4SE/Interfaces.h>
#include <RE/B/BSScript_IVirtualMachine.h>

#include "debugLog.h"
#include "skills.h"
#include <RE/T/TESDataHandler.h>
#include "IGlobalConfig.h"
#include "ObScript.h"
#include "Hooks.h"
#include "ItemDegradation.h"
#include "Shared.h"
#include "Serialization.h"
#include "Events/OnEquipEvent.h"
#include "Events/OnMenuOpenCloseEvent.h"
#include "Menus/HUD_Additions.h"

bool RegisterCustomFunctions(RE::BSScript::IVirtualMachine* vm) {
	vm->BindNativeMethod("TCW:F4CW", "PouyaFunction", F4CW::PouyaFunction, false, false);

	if (!RegisterSkillFunctions(*vm)) {
		// LOG_ERROR("Could not register Karma funcitons...");
		return false;
	}

	if (!F4CW::ItemDegradation::RegisterDegradationFunctions(vm)) {
		// LOG_ERROR("Could not register Degradation funcitons...");
		return false;
	}

	return true;
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{

	F4SE::Init(a_f4se);
	LOG_INFO("I am Pouya this is my first Fallout 4 CommonLib Mod.");
	
	LOG_WARNING(std::format("Pouya: F4SE Version: {}. Expected version: {}", F4SE::GetF4SEVersion(), F4SE_VERSION));
	
	if (F4SE::GetF4SEVersion() != F4SE_VERSION) {
		// REX::ERROR("F4SE Versions are not matching.");

		return false;
	}

	const F4SE::SerializationInterface* serializationInterface = F4SE::GetSerializationInterface();
	if (!serializationInterface) {
		REX::CRITICAL("Failed to fetch serialization interface."sv);
		return false;
	}
	serializationInterface->SetUniqueID('F4CW');
	serializationInterface->SetSaveCallback(F4CWSerialization::SaveCallback);
	serializationInterface->SetLoadCallback(F4CWSerialization::LoadCallback);
	serializationInterface->SetRevertCallback(F4CWSerialization::RevertCallback);
	LOG_INFO("Serialization callbacks registered."sv);

	const F4SE::MessagingInterface* messageInterface = F4SE::GetMessagingInterface();
	if (!messageInterface || !messageInterface->RegisterListener(F4SEMessageCallback)) {
		REX::CRITICAL("Failed to fetch Message Interface or message handler.."sv);
		return false;
	}
	
	
	const F4SE::ScaleformInterface* scaleformInterface = F4SE::GetScaleformInterface();
	if (!scaleformInterface) {
		REX::CRITICAL("Failed to fetch Scaleform Interface."sv);
		return false;
	}
	if (!RegisterScaleforms(scaleformInterface)) {
		REX::CRITICAL("Failed to register Scaleform custom Menus."sv);
		return false;
	}
	


	LOG_INFO("Installing Hooks...");
	F4CW::Hooks::Registers::Install();
	LOG_INFO("Hooks installed");

	// LOG_TO_CONSOLE("This is a console log by Pouya");

	// RE::TESDataHandler::GetSingleton()->LookupForm();

	if (!F4SE::GetPapyrusInterface()->Register(RegisterCustomFunctions)) {
		LOG_WARNING("Functions could not be registered."sv);
		return false;
	}

	LOG_INFO("Registering hooks...");
	F4CW::Hooks::Registers::RegisterAllHooks();
	LOG_INFO("All hooks are registered.");

	return true;
}


bool InitializeSharedGameVariables()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (!dataHandler) {
		// LOG_ERROR("Initializing game variables failed... Data Handler is null");
		return false;
	}

	InitializeGameVariables(dataHandler, MOD_ESM);

	Shared::InitializeSharedForms(dataHandler);

	LOG_INFO("All variables initialized.");




	return true;
}

void GameDataLoaded()
{

}

void InitializeCustomConsoleCommands() {
	F4CW::ObScript::Install();
}

bool RegisterScaleforms(const F4SE::ScaleformInterface* scaleformInterface)
{
	
	if (!scaleformInterface->Register("CWHUDAdditions", F4CW_Menus::HUD_Additions::RegisterScaleform)) {
		REX::CRITICAL("Failed to register 'CWHUDAdditions' Scaleform."sv);
		return false;
	}
	
	return true;
}

void InitializeGameEvents()
{
	F4CWEvents::RegisterOnMenuOpenCloseEvent();
	F4CWEvents::RegisterOnEquipEvent();
}

void GameDataReady()
{
	if (InitializeSharedGameVariables()) {
		Skills::RegisterForSkillLink();
	}
	else {
		// LOG_ERROR("Initializing Shared Game Variables failed.");
	}

	InitializeCustomConsoleCommands();
	LOG_INFO("All console commands initilaized.");
	LOG_INFO("Initializing game events...");
	InitializeGameEvents();
	LOG_INFO("Game events initialized");
	F4CW::ItemDegradation::DefineItemDegradationFormsFromGame();

	LOG_INFO("PACW Plugin is fully ready.");
}

void F4CW::PouyaFunction(std::monostate)
{
	LOG_TO_CONSOLE("FIRST FUNCTION OF POUYA THAT RUNS ON ACTORS ONLY!");
}

void F4SEMessageCallback(F4SE::MessagingInterface::Message* myMessage) {
	switch (myMessage->type)
	{
		case F4SE::MessagingInterface::kGameDataReady:
			GameDataReady();
			break;

		case F4SE::MessagingInterface::kGameLoaded:
			GameDataLoaded();
			break;

		default:
			break;
	}
}
