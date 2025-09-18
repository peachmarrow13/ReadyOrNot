#pragma once
#include "SDK/Engine_classes.hpp"
#include "SDK/ReadyOrNot_classes.hpp"

class Variables;
using namespace SDK;

class Utils
{
public:
	static UWorld* GetWorldSafe();
	static APlayerController* GetPlayerController();
	static Variables* GetVariables();
};

class Variables
{
public:
	APlayerController* PlayerController = nullptr;
	APawn* Pawn = nullptr;
	ACharacter* Character = nullptr;
	AReadyOrNotCharacter* ReadyOrNotChar = nullptr;
	UWorld* World = nullptr;
	ULevel* Level = nullptr;

	// Constructor to initialize variables safely
	Variables() {
		AutoSetVariables(*this);
	}

	static void AutoSetVariables(Variables& vars) {
		// Get PlayerController first
		APlayerController* currentPC = Utils::GetPlayerController();
		if (!currentPC) {
			// Clear all dependent variables if PlayerController is null
			vars.PlayerController = nullptr;
			vars.Pawn = nullptr;
			vars.Character = nullptr;
			vars.ReadyOrNotChar = nullptr;
			vars.World = Utils::GetWorldSafe(); // World can exist without PlayerController
			vars.Level = vars.World ? vars.World->PersistentLevel : nullptr;
			return;
		}

		// Update PlayerController if changed
		if (vars.PlayerController != currentPC) {
			vars.PlayerController = currentPC;
			// Reset dependent variables when PlayerController changes
			vars.Pawn = nullptr;
			vars.Character = nullptr;
			vars.ReadyOrNotChar = nullptr;
		}

		// Update Pawn
		if (vars.PlayerController && vars.Pawn != vars.PlayerController->Pawn) {
			vars.Pawn = vars.PlayerController->Pawn;
			// Reset Character-dependent variables when Pawn changes
			vars.Character = nullptr;
			vars.ReadyOrNotChar = nullptr;
		}

		// Update Character
		if (vars.PlayerController && vars.Character != vars.PlayerController->Character) {
			vars.Character = vars.PlayerController->Character;
			// Reset ReadyOrNotChar when Character changes
			vars.ReadyOrNotChar = nullptr;
		}

		// Update ReadyOrNotChar
		if (vars.Character) {
			AReadyOrNotCharacter* newReadyOrNotChar = static_cast<AReadyOrNotCharacter*>(vars.Character);
			if (vars.ReadyOrNotChar != newReadyOrNotChar) {
				vars.ReadyOrNotChar = newReadyOrNotChar;
			}
		}

		// Update World
		UWorld* currentWorld = Utils::GetWorldSafe();
		if (vars.World != currentWorld) {
			vars.World = currentWorld;
			vars.Level = nullptr; // Reset Level when World changes
		}

		// Update Level
		if (vars.World && vars.Level != vars.World->PersistentLevel) {
			vars.Level = vars.World->PersistentLevel;
		}
	}
};