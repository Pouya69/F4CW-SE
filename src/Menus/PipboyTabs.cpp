#include "PipboyTabs.h"
#include "../skills.h"
#include <string>
#include <vector>
#include <RE/A/ActorValueInfo.h>
#include <Scaleform/G/GFx_ASMovieRootBase.h>
#include <Scaleform/G/GFx_Value.h>
#include "../Scaleform_F4CW.h"
#include "../GameForms.h"
#include <cstdint>
#include <RE/B/BSStringT.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/E/ENUM_FORM_ID.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Shared.h>
#include <string.h>
#include <format>
#include <RE/B/BSFixedString.h>
#include <RE/B/BSInputEventUser.h>
#include <RE/B/BSTArray.h>
#include <RE/B/BS_BUTTON_CODE.h>
#include <RE/B/ButtonEvent.h>
#include <RE/M/MenuControls.h>
#include <RE/P/PipboyDataManager.h>
#include <RE/U/UI.h>
#include <RE/U/UIMessageQueue.h>
#include <RE/U/UI_MESSAGE_TYPE.h>
#include <REX/LOG.h>

namespace F4CW_Menus {
	namespace PipboyTabs {
		EActionToDoAfterPiboyBroughtDown action_to_to_after_closePipboy = NONE;
	}
}


void F4CW_Menus::PipboyTabs::ProcessUserEvent(const char* controlName, bool isDown, int deviceType, std::uint32_t keyCode)
{
	auto ui = RE::UI::GetSingleton();
	RE::BSFixedString pipboyMenuStr("PipboyMenu");
	if (ui->GetMenuOpen("PipboyMenu"))
	{

		RE::IMenu* menu = ui->GetMenu(pipboyMenuStr).get();
		Scaleform::GFx::ASMovieRootBase* movieRoot = menu->uiMovie->asMovieRoot.get();
		Scaleform::GFx::Value args[4];
		args[0] = controlName;
		args[1] = isDown;
		args[2] = deviceType;
		args[3] = keyCode;

		movieRoot->Invoke("root.Menu_mc.CWPipboy_loader.content.ProcessUserEventExternal", nullptr, args, 4);
	}
}

void F4CW_Menus::PipboyTabs::CloseMenu(F4CW_Menus::PipboyTabs::EActionToDoAfterPiboyBroughtDown action)
{
	RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();

	RE::BSFixedString menuName = "PipboyMenu";
	if (RE::UI* ui = RE::UI::GetSingleton()) {
		if (ui->menuMap.contains(menuName))
		{
			uiMessageQueue->AddMessage(menuName, RE::UI_MESSAGE_TYPE::kHide);
			action_to_to_after_closePipboy = action;
		}
	}
}


class F4SEInputHandler : public RE::BSInputEventUser
{
public:
	F4SEInputHandler() : BSInputEventUser() {}

	virtual bool ShouldHandleEvent(const RE::InputEvent*) override {
		return true;
	}

	virtual void OnButtonEvent(const RE::ButtonEvent* inputEvent)
	{
		
		std::uint32_t	deviceType = (std::uint32_t)inputEvent->device.get();

		float timer = inputEvent->heldDownSecs;
		bool isDown = inputEvent->QHeldDown();
		bool isUp = timer != 0.0f;


		RE::BSFixedString control = inputEvent->strUserEvent;
		RE::BS_BUTTON_CODE buttonCode = inputEvent->GetBSButtonCode();

		// REX::DEBUG(std::format("Button event: deviceType: '{}', timer: '{}', isDown: '{}', isUp: '{}', control: '{}'", deviceType, timer, isDown ? 1 : 0, isUp ? 1 : 0, control.c_str(), (std::int32_t)buttonCode).c_str());

		
		if (isDown)
		{
			F4CW_Menus::PipboyTabs::ProcessUserEvent(control.c_str(), true, deviceType, (std::int32_t)buttonCode);
		}
		else if (isUp)
		{
			F4CW_Menus::PipboyTabs::ProcessUserEvent(control.c_str(), false, deviceType, (std::int32_t)buttonCode);
		}
		

	}
};

F4SEInputHandler g_cwPipboyMenuInputHandler;

void F4CW_Menus::PipboyTabs::RegisterForInput(bool bRegister)
{
	if (bRegister)
	{
		g_cwPipboyMenuInputHandler.inputEventHandlingEnabled = true;
		RE::BSTArray<RE::BSInputEventUser*>* inputEvents = &RE::MenuControls::GetSingleton()->handlers;
		RE::BSInputEventUser* inputHandler = &g_cwPipboyMenuInputHandler;

		int idx = -1;
		for (std::uint32_t i = 0; i < inputEvents->size(); i++)
		{
			if (inputEvents->at(i) == inputHandler) {
				idx = i;
				break;
			}
		}
		
		if (idx == -1)
		{
			REX::DEBUG("Registering handler for pipboy inputs.");
			RE::MenuControls::GetSingleton()->RegisterHandler(&g_cwPipboyMenuInputHandler);
			// inputEvents->push_back(&g_cwPipboyMenuInputHandler);
		}
		else {
			REX::DEBUG("Register Handler already exists.");
		}
	}
	else
	{
		g_cwPipboyMenuInputHandler.inputEventHandlingEnabled = false;
	}
}

void F4CW_Menus::PipboyTabs::UpdateMenus(Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	REX::DEBUG("Update Menu Called.");
	UpdateSkillMenu(myMovieRoot);

	UpdateKarmaMenu(myMovieRoot);
}


void F4CW_Menus::PipboyTabs::PopulateSkillEntry(Scaleform::GFx::Value* myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot, int index, int filter, std::vector<std::string> stringValue)
{
	RE::ActorValueInfo* mySkill = Skills::SkillsList.at(index);

	Scaleform::GFx::Value arrArg;
	myMovieRoot->CreateObject(&arrArg);

	// REX::DEBUG(std::format("populating skill '{}' in Pipboy.", mySkill->GetFullName()));

	auto player = RE::PlayerCharacter::GetSingleton();


	const float baseValue = player->GetBaseActorValue(*mySkill);
	const float buffedValue = player->GetActorValue(*mySkill);

	// REX::DEBUG(std::format("Got values. populating skill '{}' in Pipboy with base value: {}, buffed: {}", mySkill->GetFullName(), baseValue, buffedValue));

	RE::BSStringT<char> skillDescription;
	mySkill->GetDescription(skillDescription);
	/*
	GFxUtilities::RegisterString(&arrArg, myMovieRoot, "text", mySkill->fullName.c_str());
	GFxUtilities::RegisterString(&arrArg, myMovieRoot, "qname", mySkill->GetFormEditorID());
	GFxUtilities::RegisterString(&arrArg, myMovieRoot, "editorid", mySkill->GetFormEditorID());
	GFxUtilities::RegisterString(&arrArg, myMovieRoot, "description", skillDescription.c_str());


	GFxUtilities::RegisterString(&arrArg, myMovieRoot, "stringValue", std::to_string((int)baseValue).c_str());
	*/

	arrArg.SetMember("text", mySkill->fullName.c_str());
	arrArg.SetMember("qname", mySkill->GetFormEditorID());
	arrArg.SetMember("editorid", mySkill->GetFormEditorID());
	arrArg.SetMember("description", skillDescription.c_str());
	arrArg.SetMember("stringValue", std::to_string((int)baseValue).c_str());
		
		/*
	if (stringValue.size() > (int)baseValue)
	{
		GFxUtilities::RegisterString(&arrArg, myMovieRoot, "stringValue", std::to_string((int)baseValue).c_str());
		
	}
	else
	{
		GFxUtilities::RegisterString(&arrArg, myMovieRoot, "stringValue", "");
	}
	*/
	arrArg.SetMember("readed", false);
	arrArg.SetMember("formid", mySkill->formID);
	arrArg.SetMember("value", buffedValue);
	arrArg.SetMember("maxVal", 0.0);
	arrArg.SetMember("basevalue", baseValue);
	arrArg.SetMember("modifier", buffedValue - baseValue);
	arrArg.SetMember("buffervalue", buffedValue);

	arrArg.SetMember("filterFlag", filter);

	//REX::DEBUG(std::format("populating skill '{}' in Pipboy with base value: {}, buffed: {}, editorid: '{}', formid: '{}'", mySkill->GetFullName(), baseValue, buffedValue
	//, mySkill->GetFormEditorID(), (std::int32_t) mySkill->GetFormID()).c_str());
	myDestination->PushBack(arrArg);
	// myDestination[index] = arrArg;
}


void F4CW_Menus::PipboyTabs::UpdateSkillMenu(Scaleform::GFx::ASMovieRootBase * myMovieRoot)
{
	Scaleform::GFx::Value arrSkills[7];

	myMovieRoot->CreateString(&arrSkills[0], "skills");
	myMovieRoot->CreateString(&arrSkills[1], "$F4CW_SKILLS");

	arrSkills[2] = 0;
	arrSkills[3] = PipboyPage_Skills;

	myMovieRoot->CreateObject(&arrSkills[4]);
	myMovieRoot->CreateArray(&arrSkills[4]);
	std::vector<std::string> skillNamesList;

	for (int i = 0; i < 13; i++)
	{
		PopulateSkillEntry(&arrSkills[4], myMovieRoot, i, 1, skillNamesList);
	}

	// arrSkills[4].SetMember("arrayList", skillsArray);
	// arrSkills[4] = arrSkills[4];
	arrSkills[5] = 0;
	arrSkills[6] = 0;

	if (!myMovieRoot->Invoke("root.Menu_mc.CWPipboy_loader.content.registerTab", nullptr, arrSkills, 7)) {
		REX::DEBUG("Could not call root.Menu_mc.CWPipboy_loader.content.registerTab from Skill");
	}
}

// Karma stuff ==========================================================================================================

void F4CW_Menus::PipboyTabs::PopulateKarmaEntry(Scaleform::GFx::Value* myDestination, Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	auto player = RE::PlayerCharacter::GetSingleton();

	Scaleform::GFx::Value karmaParams;
	myMovieRoot->CreateObject(&karmaParams);

	std::int16_t playerLevel = player->GetLevel();
	const float karmaValue = CWGlobals.Karma->GetValue();

	int karmaType = -1;
	std::string myKarmaTitle = "$F4CW_KARMA_";
	std::string myKarmaDescription = "$F4CW_KARMA_";
	std::string myKarmaSWFPath = "Components/CapitalWasteland/Karma/Karma_";

	if (karmaValue < -750)
	{
		karmaType = Karma_VeryEvil;
		myKarmaTitle += "VeryEvil";
		myKarmaDescription += "BAD_";
		myKarmaSWFPath += "VeryEvil.swf";
	}
	else if (karmaValue < -250 && karmaValue >= -750)
	{
		karmaType = Karma_Evil;
		myKarmaTitle += "Evil";
		myKarmaDescription += "BAD_";
		myKarmaSWFPath += "Evil.swf";
	}
	else if (karmaValue < 250 && karmaValue >= -250)
	{
		karmaType = Karma_Neutral;
		myKarmaTitle += "Neutral";
		myKarmaDescription += "NEUTRAL_";
		myKarmaSWFPath += "Neutral.swf";
	}
	else if (karmaValue < 750 && karmaValue >= 250)
	{
		karmaType = Karma_Good;
		myKarmaTitle += "Good";
		myKarmaDescription += "GOOD_";
		myKarmaSWFPath += "Good.swf";
	}
	else if (karmaValue >= 750)
	{
		karmaType = Karma_VeryGood;
		myKarmaTitle += "VeryGood";
		myKarmaDescription += "GOOD_";
		myKarmaSWFPath += "VeryGood.swf";
	}

	if (playerLevel > 30)
	{
		playerLevel = 30;
	}

	myKarmaDescription += std::to_string(playerLevel);

	float karmaPercent = (karmaValue / 1000);

	karmaParams.SetMember("title", myKarmaTitle.c_str());
	karmaParams.SetMember("description", myKarmaDescription.c_str());
	karmaParams.SetMember("SWFPath", myKarmaSWFPath.c_str());
	karmaParams.SetMember("percentage", karmaPercent);

	// GFxUtilities::RegisterString(&karmaParams, myMovieRoot, "title", myKarmaTitle.c_str());
	// GFxUtilities::RegisterString(&karmaParams, myMovieRoot, "description", myKarmaDescription.c_str());
	// GFxUtilities::RegisterString(&karmaParams, myMovieRoot, "SWFPath", myKarmaSWFPath.c_str());
	// GFxUtilities::SetScaleformValue(&karmaParams, "percentage", karmaPercent);

	myDestination->PushBack(karmaParams);

	// myMovieRoot->Invoke("root.CWPipboy_loader.content.registerTab", nullptr, &karmaParams, 7);
}

void F4CW_Menus::PipboyTabs::UpdateKarmaMenu(Scaleform::GFx::ASMovieRootBase* myMovieRoot)
{
	Scaleform::GFx::Value arrKarma[7];

	myMovieRoot->CreateString(&arrKarma[0], "karma");
	myMovieRoot->CreateString(&arrKarma[1], "$F4CW_KARMA");

	arrKarma[2] = 0;
	arrKarma[3] = PipboyPage_Karma;

	myMovieRoot->CreateObject(&arrKarma[4]);
	// Scaleform::GFx::Value karmaArray;
	myMovieRoot->CreateArray(&arrKarma[4]);

	PopulateKarmaEntry(&arrKarma[4], myMovieRoot);

	// arrKarma[4].SetMember("arrayList", &karmaArray);
	arrKarma[5] = 0;
	arrKarma[6] = 0;
	if (!myMovieRoot->Invoke("root.Menu_mc.CWPipboy_loader.content.registerTab", nullptr, arrKarma, 7)) {
		REX::DEBUG("Could not call root.Menu_mc.CWPipboy_loader.content.registerTab from Karma");
	}
}

void F4CW_Menus::PipboyTabs::UpdateItemCardsOnSection(std::uint32_t section)
{
	switch (section)
	{
	case kPage_Weapons:
		RepopulateItemCardsOnSection(RE::ENUM_FORM_ID::kWEAP);
		break;
	case kPage_Apparel:
		RepopulateItemCardsOnSection(RE::ENUM_FORM_ID::kARMO);
		break;
	case kPage_Aid:
		RepopulateItemCardsOnSection(RE::ENUM_FORM_ID::kALCH);
		RepopulateItemCardsOnSection(RE::ENUM_FORM_ID::kINGR);
		break;
	case kPage_Misc:
	case kPage_Junk:
	case kPage_Mods:
		break;
	case kPage_Ammo:
		RepopulateItemCardsOnSection(RE::ENUM_FORM_ID::kAMMO);
		break;
	default:
		break;
	}
}

void F4CW_Menus::PipboyTabs::RepopulateItemCardsOnSection(RE::ENUM_FORM_ID formType)
{
	RE::PipboyDataManager::GetSingleton()->inventoryData.RepopulateItemCardOnSection(formType);
}

bool F4CW_Menus::PipboyTabs::RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value)
{
	std::string currentSWFPathString = "";
	Scaleform::GFx::Value swfPath;

	if (!a_view->GetVariable(&swfPath, "root.loaderInfo.url"))
		return false;

	currentSWFPathString = swfPath.GetString();

	REX::DEBUG(currentSWFPathString.c_str());
	if (_stricmp("Interface/PipboyMenu.swf", currentSWFPathString.c_str()) == 0) {
		Scaleform::GFx::Value loader, urlRequest, root;

		if (!a_view->asMovieRoot->GetVariable(&root, "root")) {
			REX::DEBUG("Could not get 'root''");
		}

		a_view->asMovieRoot->CreateObject(&loader, "flash.display.Loader");
		Scaleform::GFx::Value cwf = "CWPipboy.swf";
		a_view->asMovieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &cwf, 1);

		loader.SetMember("name", "CWPipboy_loader");
		root.SetMember("CWPipboy_loader", loader);

		 Scaleform::GFx::Value cwPipboy;
		a_view->asMovieRoot->CreateObject(&cwPipboy);
		root.SetMember("cwPipboy", cwPipboy);

		if (!Shared::RegisterFunction<Pipboy_Ready>(&cwPipboy, a_view->asMovieRoot, "ready")) {
			REX::DEBUG("Could not register 'Pipboy_Ready' for 'Ready'");
		}

		Shared::RegisterFunction<Debug_ActionScript>(&cwPipboy, a_view->asMovieRoot, "DebugPrint");
		Shared::RegisterFunction<Pipboy_Repair>(&cwPipboy, a_view->asMovieRoot, "Repair");
		Shared::RegisterFunction<Pipboy_CheckInventoryItem>(&cwPipboy, a_view->asMovieRoot, "CheckSelectedItem");
		Shared::RegisterFunction<Pipboy_CheckForcedLevelUp>(&cwPipboy, a_view->asMovieRoot, "IsForcedLevelUp");
		Shared::RegisterFunction<Pipboy_CheckWorkshopTab>(&cwPipboy, a_view->asMovieRoot, "IsWorkshopTabHidden");
		Shared::RegisterFunction<Pipboy_UpdateItemCardsOnSection>(&cwPipboy, a_view->asMovieRoot, "UpdateItemCardsOnSection");
		Shared::RegisterFunction<Pipboy_IsRepairMenuOpen>(&cwPipboy, a_view->asMovieRoot, "IsRepairMenuOpen");
		Shared::RegisterFunction<Pipboy_AddCND_ForItemCard>(&cwPipboy, a_view->asMovieRoot, "AddCND_ItemCard");

		if (!loader.Invoke("load", nullptr, &urlRequest, 1)) {
			REX::DEBUG("Could not load in CWPipboy");
		}
		
		if (!a_view->asMovieRoot->Invoke("root.Menu_mc.addChild", nullptr, &loader, 1)) {
			REX::DEBUG("Could not addChild in CWPipboy");
		}
		
		/*
		if (!a_view->asMovieRoot->Invoke("root.addChild", nullptr, &loader, 1)) {
			REX::DEBUG("Could not addChild in CWPipboy");
		}
		*/

		// root.Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);

		REX::DEBUG("CWPipboy.swf");


		// a_view->asMovieRoot->GetVariable(&bgsCodeObj, "root.codeObj");
		/*
		if (!Shared::RegisterFunction<PipboyInventory_Ready>(&bgsCodeObj, a_view->asMovieRoot, "ready")) {
			REX::DEBUG("Could not register 'PipboyInventory_Ready' for 'ready'");
		}

		
		Shared::RegisterFunction<Debug_ActionScript>(&bgsCodeObj, a_view->asMovieRoot, "DebugPrint");
		*/


		REX::DEBUG("Inventory.swf");





	}
	if (_stricmp("Interface/Pipboy_InvPage.swf", currentSWFPathString.c_str()) == 0) {

		

		//Scaleform::GFx::Value cwf = "CWPipboy.swf";
		//a_view->asMovieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &cwf, 1);
	}

	return true;
}
