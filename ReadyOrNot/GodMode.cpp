#include <Windows.h>

#include "Cheats.h"
#include "Utils.h"
#include "SDK/ReadyOrNot_classes.hpp"

using namespace SDK;

void Cheats::ToggleGodMode() {
	APlayerController* PlayerController = Utils::GetPlayerController();
	if (!PlayerController) {
		printf("[Error] Cannot enable God Mode: PlayerController not found!\n");
		return;
	}
	ACharacter* Character = PlayerController->Character;
	if (!Character) {
		printf("[Error] Cannot enable God Mode: Pawn is not a Character!\n");
		return;
	}

	AReadyOrNotCharacter* ReadyOrNotChar = (AReadyOrNotCharacter*)Character;

	ReadyOrNotChar->bGodMode = !ReadyOrNotChar->bGodMode;
}