#include "pch.h"
#include "Engine.h"

using namespace SDK;

// can return nullptr
UWorld* Utils::GetWorldSafe() 
{
    UWorld* World = nullptr;
    int i = 0;
    while (i < 50) {
        i++;
        UEngine* Engine = UEngine::GetEngine();
        if (!Engine) {
            //printf("[Error] Engine not found!\n");
            Sleep(50);
            continue;
        }

        UGameViewportClient* Viewport = Engine->GameViewport;
        if (!Viewport) {
            //printf("[Error] GameViewport not found!\n");
            Sleep(50);
            continue;
        }

        World = Viewport->World;
        if (!World) {
            //printf("[Error] World not found!\n");
            Sleep(50);
            continue;
        }
		break; // Successfully obtained World
    }
    return World;
}

// can return nullptr
APlayerController* Utils::GetPlayerController()
{
	APlayerController* PlayerController = nullptr;
    
    UWorld* World = GetWorldSafe();
    if (!World)
        return nullptr;

    UGameInstance* GameInstance = World->OwningGameInstance;
    if (!GameInstance)
        return nullptr;

    if (GameInstance->LocalPlayers.Num() <= 0)
        return nullptr;

    ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
    if (!LocalPlayer)
        return nullptr;

    PlayerController = LocalPlayer->PlayerController;
    if (!PlayerController)
        return nullptr;

    if (!Utils::IsValidActor(PlayerController))
        return nullptr;
    
    return PlayerController;
}

unsigned Utils::ConvertImVec4toU32(ImVec4 Color)
{
    return IM_COL32((int)(Color.x * 255.0f), (int)(Color.y * 255.0f), (int)(Color.z * 255.0f), (int)(Color.w * 255.0f));
}

void Utils::PrintActors(const char* Exclude)
{
	ULevel* Level = GVars.Level;
	if (Level)
	{
        TArray<AActor*> Actors = Level->Actors;
        for (int i = 0; i < Actors.Num(); i++)
        {
            AActor* Actor = Actors[i];
            if (Actor)
            {
                if (!Utils::IsValidActor(Actor)) continue;
                if (Exclude && Actor->GetName().find(Exclude) != std::string::npos)
					continue;

                printf("Actor %d: %s - Class: %s\n", i, Actor->GetName().c_str(), Actor->Class->Name.ToString().c_str());
            }
		}
	}
}

FRotator Utils::VectorToRotation(const FVector& Vec)
{
    FRotator Rot;
    Rot.Yaw = std::atan2(Vec.Y, Vec.X) * (180.0 / std::numbers::pi);
    Rot.Pitch = std::atan2(Vec.Z, std::sqrt(Vec.X * Vec.X + Vec.Y * Vec.Y)) * (180.0 / std::numbers::pi);
    Rot.Roll = 0.0;
    return Rot;
}

FVector Utils::FRotatorToVector(const FRotator& Rot)
{
    double PitchRad = Rot.Pitch * (std::numbers::pi / 180.0);
    double YawRad = Rot.Yaw * (std::numbers::pi / 180.0);

    double CP = cos(PitchRad);
    double SP = sin(PitchRad);
    double CY = cos(YawRad);
    double SY = sin(YawRad);

    return FVector(
        CP * CY,   // X
        CP * SY,   // Y
        SP         // Z
    ).GetNormalized(); // normalize just in case
}

// Helper function to get or create player cheat data
PlayerCheatData& Utils::GetPlayerCheats(APlayerCharacter* Player)
{
    return PlayerCheatMap[Player];
}

bool Utils::IsValidActor(AActor* Actor)
{
    if (!Actor) return false;

    if (!UKismetSystemLibrary::IsValid(Actor)) return false;

    if (!Actor->Class) return false;

    if (!Actor->GetLevel()) return false;

    if (Actor->IsActorBeingDestroyed()) return false;

	if (!Actor->VTable) return false;

	if (Actor->bActorIsBeingDestroyed) return false;

    return true;
}

float Utils::GetFOVFromScreenCoords(const ImVec2& ScreenLocation)
{
    ImVec2 ScreenCenter(GVars.ScreenSize.x / 2.0f, GVars.ScreenSize.y / 2.0f);

    float DeltaX = ScreenLocation.x - ScreenCenter.x;
    float DeltaY = ScreenLocation.y - ScreenCenter.y;

    return std::sqrt(DeltaX * DeltaX + DeltaY * DeltaY);
}

ImVec2 Utils::FVector2DToImVec2(FVector2D Vector)
{
	return ImVec2(Vector.X, Vector.Y);
}

FRotator Utils::GetRotationToTarget(const FVector& Start, const FVector& Target)
{
    FVector Delta = Target - Start;
    Delta.Normalize(); // Important for safe calculations

    FRotator Rotation;
    Rotation.Pitch = std::atan2(Delta.Z, std::sqrt(Delta.X * Delta.X + Delta.Y * Delta.Y)) * (180.0f / std::numbers::pi);
    Rotation.Yaw = std::atan2(Delta.Y, Delta.X) * (180.0f / std::numbers::pi);
    Rotation.Roll = 0.0f;

    return Rotation;
}

FVector2D Utils::ImVec2ToFVector2D(ImVec2 Vector)
{
	return FVector2D(Vector.x, Vector.y);
}

AActor* Utils::GetBestTarget(APlayerController* ViewPoint, bool TargetCivs, bool TargetArrested, bool TargetSurrendered, bool TargetDead, float MaxFOV, bool RequiresLOS, std::string TargetBone, bool TargetAll)
{
    if (!GVars.World || !GVars.Level || !ViewPoint || !ViewPoint->PlayerCameraManager) return nullptr;

    std::wstring WideString = UtfN::StringToWString(TargetBone);
    FName BoneName = UKismetStringLibrary::Conv_StringToName(WideString.c_str());

    AActor* BestTarget = nullptr;
    float BestFOV = MaxFOV;

	TArray<AActor*> Actors = GVars.Level->Actors;
	if (!Actors || !Actors.IsValid() || Actors.Num() <= 0)
		return nullptr;

    for (AActor* Actor : Actors)
    {
        if (!Actors)
            break;
        if (!Actor || !Utils::IsValidActor(Actor))
            continue;
		if (Actor == GVars.ReadyOrNotChar)
			continue;

        AReadyOrNotCharacter* ReadyOrNotChar;

        if (Actor->IsA(AReadyOrNotCharacter::StaticClass()))
        {
            ReadyOrNotChar = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
            if (!ReadyOrNotChar)
                continue;
            if (!TargetAll)
            {
	            bool bIsCivilian = ReadyOrNotChar->IsCivilian();
            	bool bIsSuspect = ReadyOrNotChar->IsSuspect();

            	// Keep target if suspect OR (civilian AND TargetCivs)
            	if (!(bIsSuspect || (bIsCivilian && TargetCivs)))
            		continue;
            }
        }
        else
            continue;

        if (!ReadyOrNotChar)
            continue;
        if (!TargetDead && (ReadyOrNotChar->IsDeadOrUnconscious() || ReadyOrNotChar->IsIncapacitated()))
            continue;
        if (!TargetArrested && ReadyOrNotChar->IsArrested())
            continue;
        if (!TargetSurrendered && ReadyOrNotChar->IsSurrendered())
            continue;
		if (!ReadyOrNotChar->Mesh)
			continue;

        // Get the target bone location
        FVector BoneLocation = ReadyOrNotChar->Mesh->GetBoneTransform(BoneName, ERelativeTransformSpace::RTS_World).Translation;

        if (RequiresLOS)
        {
            FVector Start = ViewPoint->PlayerCameraManager->CameraCachePrivate.POV.Location;
            FVector End = BoneLocation;

            TArray<AActor*> IgnoreActors;
            //IgnoreActors.Add(GVars.ReadyOrNotChar);

            FHitResult HitResult;
            bool bHit = UKismetSystemLibrary::LineTraceSingle(
                GVars.World,
                Start,
                End,
                ETraceTypeQuery::TraceTypeQuery1,
                true,
                IgnoreActors,
                EDrawDebugTrace::None,
                &HitResult,
                true,
                FLinearColor(),
                FLinearColor(),
                1.0f
            );

			AActor* HitActor = HitResult.Component ? HitResult.Component->GetOwner() : nullptr;

            bool bHasLOS = !HitResult.bBlockingHit || HitActor == ReadyOrNotChar;
            if (!bHasLOS)
                continue;
        }

        FVector2D ScreenLocation;
        if (!ViewPoint->ProjectWorldLocationToScreen(BoneLocation, &ScreenLocation, true))
            continue;

        FVector2D ViewportSize = Utils::ImVec2ToFVector2D(GVars.ScreenSize);
        FVector2D ViewportCenter = ViewportSize / 2.0;
        FVector2D Delta = ScreenLocation - ViewportCenter;

        // Compute length manually
        float DeltaLength = std::sqrt(Delta.X * Delta.X + Delta.Y * Delta.Y);

        // Normalize by half the screen height
        float NormalizedOffset = DeltaLength / (ViewportSize.Y * 0.5f);
        float MaxFOVNormalized = MaxFOV / 90.0f;
        float FOV = NormalizedOffset * 90.0f;
        if (NormalizedOffset < MaxFOVNormalized && FOV < BestFOV)
        {
            BestFOV = FOV;
            BestTarget = Actor;
        }
    }
    if (BestTarget)
        return BestTarget;
    return nullptr;
}

void Utils::DrawFOV(float MaxFOV, float Thickness = 1.0f)
{
    FVector2D ViewportSize = Utils::ImVec2ToFVector2D(GVars.ScreenSize);
    FVector2D Center = ViewportSize * 0.5f;

    float MaxFOVNormalized = MaxFOV / 90.0f;
    float RadiusPixels = MaxFOVNormalized * (ViewportSize.Y * 0.5f);

    // Draw using ImGui (pixel radius)
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(Center.X, Center.Y), RadiusPixels, IM_COL32(255, 0, 0, 255), 64, Thickness);
}

void Utils::DrawSnapLine(FVector TargetPos, float Thickness = 2.0f)
{
    FVector2D ScreenPos;

    if (!GVars.PlayerController->ProjectWorldLocationToScreen(TargetPos, &ScreenPos, true))
        return;

    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(GVars.ScreenSize.x / 2, GVars.ScreenSize.y / 2), ImVec2(ScreenPos.X, ScreenPos.Y), IM_COL32(255, 255, 255, 255), Thickness);
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ScreenPos.X, ScreenPos.Y), 2.0f, IM_COL32(0, 255, 0, 255));
}

void Utils::Error(std::string msg)
{
    printf("[Error] %s\n", msg.c_str());
}

void cerrf(const char* Format, ...)
{
    va_list Args;
    va_start(Args, Format);

    // Print to stderr
    vfprintf(stderr, Format, Args);

    va_end(Args);
}

ACharacter* Utils::GetNearestCharacter(ETeam Team)
{
    if (!GVars.Level || !GVars.ReadyOrNotChar) return nullptr;
    ACharacter* NearestCharacter = nullptr;
    float NearestDistance = FLT_MAX;
    for (AActor* Actor : GVars.Level->Actors)
    {
        if (!Actor || !Utils::IsValidActor(Actor))
            continue;
        AReadyOrNotCharacter* ReadyOrNotChar;
        if (Actor->IsA(AReadyOrNotCharacter::StaticClass()))
        {
            ReadyOrNotChar = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
            if (!ReadyOrNotChar)
                continue;
            bool bIsCivilian = ReadyOrNotChar->IsCivilian();
            bool bIsSuspect = ReadyOrNotChar->IsSuspect();
            if (Team == ETeam::TEAM_CIVILIAN && !bIsCivilian)
                continue;
            if (Team == ETeam::TEAM_SUSPECT && !bIsSuspect)
                continue;
            if (Team == ETeam::TEAM_SWAT && (bIsCivilian || bIsSuspect))
                continue;
        }
        else
            continue;
        FVector PlayerLocation = GVars.ReadyOrNotChar->K2_GetActorLocation();
        FVector TargetLocation = ReadyOrNotChar->K2_GetActorLocation();
        float Distance = PlayerLocation.GetDistanceTo(TargetLocation);
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestCharacter = ReadyOrNotChar;
        }
    }
	return NearestCharacter;
}

bool Utils::SafeProjectWorldLocationToScreen(APlayerController* Controller, const FVector& WorldPosition, FVector2D* OutScreenPosition, const bool PlayerViewportRelative)
{
    if (Controller)
    {
        return Controller->ProjectWorldLocationToScreen(WorldPosition, OutScreenPosition, PlayerViewportRelative);
    }

    return false;
}
