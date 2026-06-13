#pragma once
#include <Scaleform/Scaleform.h>
#include <Scaleform/G/GFx_Movie.h>
#include <Scaleform/G/GFx_Value.h>
#include <string.h>
#include "../Shared.h"
#include "Scaleform_F4CW.h"

namespace F4CW_Menus {
	
	namespace HUD_Additions {
		bool RegisterScaleform(Scaleform::GFx::Movie* a_view, Scaleform::GFx::Value* a_value);
	}
	
}
