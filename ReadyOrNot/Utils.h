#pragma once
#include "SDK/Engine_classes.hpp"
#include "SDK/ReadyOrNot_classes.hpp"
#include "ImGui/imgui.h"

class Variables;
using namespace SDK;

class Utils
{
public:
	static UWorld* GetWorldSafe();
	static APlayerController* GetPlayerController();
	static Variables* GetVariables();
	static unsigned ConvertImVec4toU32(ImVec4 Color);
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

static inline float Dot3(const FVector& A, const FVector& B)
{
	return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}

static inline float Length3(const FVector& V)
{
	return sqrtf(V.X * V.X + V.Y * V.Y + V.Z * V.Z);
}

static inline FVector Normalize(const FVector& V)
{
	float L = Length3(V);
	if (L <= 0.0001f) return FVector{ 0,0,0 };
	return FVector{ V.X / L, V.Y / L, V.Z / L };
}

static inline float ClampFloat(float v, float a, float b)
{
	return v < a ? a : (v > b ? b : v);
}

static inline float AngleDegFromDot(float Dot)
{
	Dot = ClampFloat(Dot, -1.0f, 1.0f);
	return acosf(Dot) * (180.0f / 3.1415926535f);
}

static inline void ClampRotator(FRotator& R)
{
	// Unreal style clamping (optional)
	while (R.Yaw > 180.f)  R.Yaw -= 360.f;
	while (R.Yaw < -180.f) R.Yaw += 360.f;
	if (R.Pitch > 89.f)  R.Pitch = 89.f;
	if (R.Pitch < -89.f) R.Pitch = -89.f;
	R.Roll = 0.f;
}

static inline FVector ForwardFromRot(const FRotator& Rot)
{
	float PitchRad = Rot.Pitch * (3.1415926535f / 180.0f);
	float YawRad = Rot.Yaw * (3.1415926535f / 180.0f);
	float CP = cosf(PitchRad);
	return FVector{
		cosf(YawRad) * CP,
		sinf(YawRad) * CP,
		sinf(PitchRad)
	};
}