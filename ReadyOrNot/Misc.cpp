#include "Cheats.h"
#include "Utils.h"
#include "SDK/Engine_classes.hpp"
#include "SDK/ReadyOrNot_classes.hpp"

void Cheats::ToggleGodMode() {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	RONC->bGodMode = !RONC->bGodMode;
}

void Cheats::RefillAmmo() {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	auto Character = (APlayerCharacter*)RONC;
	Character->ReplenishAllMagazineAmmo();
}