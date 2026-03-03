#pragma once
#include "Engine.h"

void cerrf(const char* Format, ...);

using namespace SDK;

struct Colors
{
	static const ImU32 White = IM_COL32(255, 255, 255, 255);
	static const ImU32 Black = IM_COL32(0, 0, 0, 255);
	static const ImU32 Red = IM_COL32(255, 0, 0, 255);
	static const ImU32 Green = IM_COL32(0, 255, 0, 255);
	static const ImU32 Blue = IM_COL32(0, 0, 255, 255);
	static const ImU32 Yellow = IM_COL32(255, 255, 0, 255);
	static const ImU32 Cyan = IM_COL32(0, 255, 255, 255);
	static const ImU32 Magenta = IM_COL32(255, 0, 255, 255);
	static const ImU32 Gray = IM_COL32(128, 128, 128, 255);
	static const ImU32 Orange = IM_COL32(255, 165, 0, 255);
	static const ImU32 Purple = IM_COL32(128, 0, 128, 255);
	static const ImU32 Pink = IM_COL32(255, 192, 203, 255);
};

enum class ETeam
{
	TEAM_CIVILIAN = 0,
	TEAM_SUSPECT,
	TEAM_SWAT,
	TEAM_MAX
};

// Per-player cheat settings
struct PlayerCheatData
{
	bool GodMode = false;
	bool InfAmmo = false;

	// Constructor
	PlayerCheatData() = default;
};

// Global map to store per-player cheat data
inline std::unordered_map<APlayerCharacter*, PlayerCheatData> PlayerCheatMap;



inline float GetDistance(AActor* Actor, FVector AActorLocation)
{
	if (!Actor || !Actor->RootComponent)
		return -1.0f;
	const auto RootComponent = Actor->RootComponent;
	auto deltaX = (float)(RootComponent->RelativeLocation.X - AActorLocation.X);
	auto deltaY = (float)(RootComponent->RelativeLocation.Y - AActorLocation.Y);
	auto deltaZ = (float)(RootComponent->RelativeLocation.Z - AActorLocation.Z);

	return (float)std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}

struct Utils
{
	static UWorld* GetWorldSafe(); // can return nullptr
	static APlayerController* GetPlayerController(); // can return nullptr
	static unsigned ConvertImVec4toU32(ImVec4 Color);
	static void PrintActors(const char* Exclude);
	static FRotator VectorToRotation(const FVector& Vec);
	static AActor* GetBestTarget(APlayerController* ViewPoint, bool TargetCivs, bool TargetArrested, bool TargetSurrendered, bool TargetDead, float MaxFOV, bool RequiresLOS, std::string TargetBone, bool TargetAll, bool ExcludeTargetSuspects = false);
	static void DrawFOV(float MaxFOV, float Thickness);
	static void DrawSnapLine(FVector TargetPos, float Thickness);
	static FVector FRotatorToVector(const FRotator& Rot);
	static PlayerCheatData& GetPlayerCheats(APlayerCharacter* Player);
	static bool IsValidActor(AActor* Actor);
	static float GetFOVFromScreenCoords(const ImVec2& ScreenLocation);
	static ImVec2 FVector2DToImVec2(FVector2D Vector);
	static FRotator GetRotationToTarget(const FVector& Start, const FVector& Target);
	static FVector2D ImVec2ToFVector2D(ImVec2 Vector);
	static ACharacter* GetNearestCharacter(ETeam Team);
	static void Error(std::string msg);
	static bool IsTargetSuspect(AActor* Actor);

};

struct Variables
{
	APlayerController* PlayerController = nullptr;
	FMinimalViewInfo* POV = nullptr;
	APawn* Pawn = nullptr;
	ACharacter* Character = nullptr;
	AReadyOrNotCharacter* ReadyOrNotChar = nullptr;
	UWorld* World = nullptr;
	AReadyOrNotGameState* GameState = nullptr;
	ULevel* Level = nullptr;
	ImVec2 ScreenSize;

	// Constructor to initialize variables safely
	Variables() {
		AutoSetVariables();
	}

	void AutoSetVariables() {
		// 1. Update World first
		UWorld* currentWorld = Utils::GetWorldSafe();
		if (this->World != currentWorld) {
			this->World = currentWorld;
			// Reset EVERYTHING when World changes
			this->PlayerController = nullptr;
			this->GameState = nullptr;
			this->Pawn = nullptr;
			this->Character = nullptr;
			this->ReadyOrNotChar = nullptr;
			this->Level = nullptr;
			this->POV = nullptr;
		}

		if (!this->World || !this->World->VTable) return;

		// 2. Update Level and GameState
		this->Level = this->World->PersistentLevel;

		if (this->World->GameState && this->World->GameState->VTable && this->World->GameState->IsA(AReadyOrNotGameState::StaticClass()))
			this->GameState = static_cast<AReadyOrNotGameState*>(this->World->GameState);
		else
			this->GameState = nullptr;

		// 3. Update PlayerController
		APlayerController* currentPC = Utils::GetPlayerController();
		if (this->PlayerController != currentPC) {
			this->PlayerController = currentPC;
			// Reset PC dependents
			this->Pawn = nullptr;
			this->Character = nullptr;
			this->ReadyOrNotChar = nullptr;
			this->POV = nullptr;
		}

		if (!this->PlayerController || !this->PlayerController->VTable) return;

		// 4. Update PC dependents
		this->Pawn = this->PlayerController->Pawn;
		if (this->Pawn && !this->Pawn->VTable) this->Pawn = nullptr;

		this->Character = this->PlayerController->Character;
		if (this->Character && !this->Character->VTable) this->Character = nullptr;

		if (this->Character && this->Character->IsA(AReadyOrNotCharacter::StaticClass()))
			this->ReadyOrNotChar = static_cast<AReadyOrNotCharacter*>(this->Character);
		else
			this->ReadyOrNotChar = nullptr;

		if (this->PlayerController->PlayerCameraManager && this->PlayerController->PlayerCameraManager->VTable)
			this->POV = &this->PlayerController->PlayerCameraManager->CameraCachePrivate.POV;
		else
			this->POV = nullptr;

		// Update Screen size
		if (ImGui::GetCurrentContext())
			ScreenSize = ImGui::GetIO().DisplaySize;
	}
} inline GVars;

static inline float Dot3(const FVector& A, const FVector& B)
{
	return (float)A.X * (float)B.X + (float)A.Y * (float)B.Y + (float)A.Z * (float)B.Z;
}

static inline float Length3(const FVector& V)
{
	return sqrtf((float)V.X * (float)V.X + (float)V.Y * (float)V.Y + (float)V.Z * (float)V.Z);
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
	R.Normalize();

	if (R.Pitch > 89.f)  R.Pitch = 89.f;
	if (R.Pitch < -89.f) R.Pitch = -89.f;
	R.Roll = 0.f;
}

static inline FVector ForwardFromRot(const FRotator& Rot)
{
	float PitchRad = (float)Rot.Pitch * (3.1415926535f / 180.0f);
	float YawRad = (float)Rot.Yaw * (3.1415926535f / 180.0f);

	float CP = cosf(PitchRad);
	return FVector{
		cosf(YawRad) * CP,
		sinf(YawRad) * CP,
		sinf(PitchRad)
	};
}
