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
	static AActor* GetBestTarget(APlayerController* ViewPoint, bool TargetCivs, bool TargetArrested, bool TargetSurrendered, bool TargetDead, float MaxFOV, bool RequiresLOS, std::string TargetBone, bool TargetAll);
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
	TArray<APlayerCharacter*> Players = TArray<APlayerCharacter*>();
	ULevel* Level = nullptr;
	ImVec2 ScreenSize;

	// Constructor to initialize variables safely
	Variables() {
		AutoSetVariables();
	}

	void AutoSetVariables() {

		// Get PlayerController first
		APlayerController* currentPC = Utils::GetPlayerController();
		if (!currentPC) {

			// Clear all dependent variables if PlayerController is null
			this->PlayerController = nullptr;
			this->POV = nullptr;
			this->Pawn = nullptr;
			this->Character = nullptr;
			this->ReadyOrNotChar = nullptr;
			this->World = Utils::GetWorldSafe();
			if (this->World && this->World->GameState && this->World->GameState->IsA(AReadyOrNotGameState::StaticClass()))
				this->GameState = this->World ? static_cast<AReadyOrNotGameState*>(this->World->GameState) : nullptr;
			else
				this->GameState = nullptr;
			this->Players = this->GameState ? this->GameState->AllPlayerCharacters : TArray<APlayerCharacter*>();
			this->Level = this->World ? this->World->PersistentLevel : nullptr;
			return;
		}

		// Update PlayerController if changed
		if (this->PlayerController != currentPC) {
			this->PlayerController = currentPC;
			// Reset dependent variables when PlayerController changes
			this->Pawn = nullptr;
			this->Character = nullptr;
			this->ReadyOrNotChar = nullptr;
		}

		// Update Pawn
		if (this->PlayerController && this->Pawn != this->PlayerController->Pawn) {
			this->Pawn = this->PlayerController->Pawn;
			// Reset Character-dependent variables when Pawn changes
			this->Character = nullptr;
			this->ReadyOrNotChar = nullptr;
		}

		// Update Character
		if (this->PlayerController && this->Character != this->PlayerController->Character) {
			this->Character = this->PlayerController->Character;
			// Reset ReadyOrNotChar when Character changes
			this->ReadyOrNotChar = nullptr;
		}

		// Update ReadyOrNotChar
		if (this->Character) {
			AReadyOrNotCharacter* newReadyOrNotChar = static_cast<AReadyOrNotCharacter*>(this->Character);
			if (this->ReadyOrNotChar != newReadyOrNotChar) {
				this->ReadyOrNotChar = newReadyOrNotChar;
			}
		}

		// Update World
		UWorld* currentWorld = Utils::GetWorldSafe();
		if (this->World != currentWorld) {
			this->World = currentWorld;
			this->Level = nullptr; // Reset Level when World changes
		}

		// Update Level
		if (this->World && this->Level != this->World->PersistentLevel) {
			this->Level = this->World->PersistentLevel;
		}

		// Update GameState
		if (this->World && this->World->GameState && this->GameState != this->World->GameState && this->World->GameState->IsA(AReadyOrNotGameState::StaticClass())) 
			this->GameState = static_cast<AReadyOrNotGameState*>(this->World->GameState);

		if (this->PlayerController && this->PlayerController->PlayerCameraManager) {
			this->POV = &this->PlayerController->PlayerCameraManager->CameraCachePrivate.POV;
		}
		else {
			this->POV = nullptr;
		}

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
