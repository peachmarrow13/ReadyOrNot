#include "pch.h"
#include <Windows.h>

#include "Cheats.h"
#include "Utils/Utils.h"
#include "ReadyOrNot_SDK/SDK/ReadyOrNot_classes.hpp"

using namespace SDK;

void Cheats::ToggleGodMode() {
	if (!GVars.PlayerController) return;
	if (!GVars.ReadyOrNotChar) return;
	auto* RONC = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	if (RONC->bGodMode != CVars.GodMode)
	{
		if (GVars.PlayerController->HasAuthority())
		{
			RONC->Server_ToggleGodMode();
		}
		else
			RONC->ToggleGodMode();
	}
}
