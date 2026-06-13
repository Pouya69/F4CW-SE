#include <Scaleform/Scaleform.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Scaleform/G/GFx_Value.h>
#include <string.h>
#include "../Shared.h"
#include "Scaleform_F4CW.h"
#include "HUD_Additions.h"

namespace F4CW_Menus {
	namespace HUD_Additions {
		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value) {
			Scaleform::GFx::Value currentSWFPath;
			if (!a_view->asMovieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url"))
				return false;

			if (_stricmp(currentSWFPath.GetString(), "Interface/HUDMenu.swf") == 0) {
				Scaleform::GFx::Value loader, urlRequest, root;
				Scaleform::GFx::Value value = "CWHUDAdditions_02.swf";

				a_view->asMovieRoot->GetVariable(&root, "root");
				a_view->asMovieRoot->CreateObject(&loader, "flash.display.Loader");
				a_view->asMovieRoot->CreateObject(&urlRequest, "flash.net.URLRequest", &value, 1);

				loader.SetMember("name", "CWHUD_loader");
				root.SetMember("CWHUD_loader", loader);

				Scaleform::GFx::Value cwHUD;
				a_view->asMovieRoot->CreateObject(&cwHUD);
				root.SetMember("cwHUD", cwHUD);
				Shared::RegisterFunction<HUD_Ready>(&cwHUD, a_view->asMovieRoot, "Ready");
				Shared::RegisterFunction<HUD_GetCurrentCondition>(&cwHUD, a_view->asMovieRoot, "GetCurrentCondition");

				if (!loader.Invoke("load", nullptr, &urlRequest, 1)) {
					REX::CRITICAL("Cannot call load on CWHUD_loader.");
				}
				
				
				if (!a_view->asMovieRoot->Invoke("root.addChild", nullptr, &loader, 1)) {
					REX::CRITICAL("Cannot root.Menu_mc.addChild");
					// return false;
				}

				REX::DEBUG("Finished Scaleform for CWHUDAdditions_02.swf");
			}

			if (_stricmp(currentSWFPath.GetString(), "Interface/CWHUDAdditions_02.swf") == 0) {
				REX::DEBUG("stricmp on CWHUDAdditions_02.swf");
			}

			return true;
		}
	}
	
}
