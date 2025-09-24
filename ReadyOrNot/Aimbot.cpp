#include <Windows.h>

#include "Cheats.h"
#include "Utils.h"
#include <chrono>
#include <numbers>

bool AimbotEnabled = false;
TArray<AActor*> Actors; // TArray<AActor*>
static float LastActorUpdateTime = 0.f;
const float ActorUpdateInterval = 1.f; // update every 1 second
static float CurrentTime = 0.0f;
static std::chrono::high_resolution_clock::time_point LastFrameTime = std::chrono::high_resolution_clock::now();

TArray<AActor*> GetActors(Variables* Vars);

// Parameters you can tweak
const float MaxFOVDegrees = 15.0f;        // Only pick targets inside this cone
const bool  UseSmoothing = false;
const float SmoothAlpha = 0.18f;        // 0..1 each tick (lower = slower)
const float MinDistance = 50.0f;        // Ignore absurdly close (optional)

void Cheats::ToggleAimbot() {
	AimbotEnabled = !AimbotEnabled;
}

void Cheats::Aimbot(Variables* Vars)
{
    if (!AimbotEnabled) return;
	auto Now = std::chrono::high_resolution_clock::now();
    float DeltaTime = std::chrono::duration<float>(Now - LastFrameTime).count();
    LastFrameTime = Now;
    CurrentTime += DeltaTime;

    if (!Vars || !Vars->PlayerController || !Vars->Character) return;

    ULevel* Level = Vars->Level;
    if (!Level) return;

    FRotator CurrentRot;
    FVector PlayerPos;
    Vars->Character->GetActorEyesViewPoint(&PlayerPos, &CurrentRot);
    FVector  Forward = ForwardFromRot(CurrentRot);

    AActor* BestTarget = nullptr;
    float BestAngle = 99999.0f;
    float BestDist = 999999.f;

    if (CurrentTime - LastActorUpdateTime > ActorUpdateInterval)
    {
        GetActors(Vars);
        LastActorUpdateTime = CurrentTime;
    }
    for (int i = 0; i < Actors.Num(); ++i)
    {
        AActor* Actor = Actors[i];
        ASuspectCharacter* TargetActor;
		if (!Actor) continue;
        if (Actor->Class == ASuspectController::StaticClass())
        {
            ASuspectController* ActorController = (ASuspectController*)Actor;
            APawn* Pawn = ActorController->Pawn;
            TargetActor = (ASuspectCharacter*)Pawn;
        }
        else continue;

		AReadyOrNotPlayerController* PC = (AReadyOrNotPlayerController*)Vars->PlayerController;

        if (!PC->LineOfSightTo(TargetActor, PlayerPos, false)) continue;

        if (!TargetActor || TargetActor == Vars->Character) continue;
        
        FVector TargetPos = TargetActor->K2_GetActorLocation();

		if (TargetPos.X == 0.f && TargetPos.Y == 0.f && TargetPos.Z == 0.f) continue;

        FVector Delta = FVector{ TargetPos.X - PlayerPos.X,
                                 TargetPos.Y - PlayerPos.Y,
                                 TargetPos.Z - PlayerPos.Z };

        float Dist = Length3(Delta);
        if (Dist < MinDistance) continue;
        if (Dist <= 0.0001f) continue;

        FVector Dir = Normalize(Delta);
        float Dot = Dot3(Forward, Dir);
        float Angle = AngleDegFromDot(Dot);

        if (Angle > MaxFOVDegrees) continue;

        if (Angle < BestAngle ||
            (fabsf(Angle - BestAngle) < 5.0f && Dist < BestDist))
        {
            BestAngle = Angle;
            BestDist = Dist;
            BestTarget = TargetActor;
        }
    }

    if (!BestTarget) return;

    // Compute desired aim rotation
    FVector AimPos;
	BestTarget->GetActorEyesViewPoint(&AimPos, nullptr);

    FVector D = FVector{ AimPos.X - PlayerPos.X,
                         AimPos.Y - PlayerPos.Y,
                         AimPos.Z - PlayerPos.Z };

	float DistXY = sqrtf(D.X * D.X + D.Y * D.Y);
    if (DistXY < 0.0001f) return;

    FRotator TargetRot;
    TargetRot.Yaw = atan2f(D.Y, D.X) * (180.0f / std::numbers::pi);
    TargetRot.Pitch = atan2f(D.Z, DistXY) * (180.0f / std::numbers::pi);
    TargetRot.Roll = 0.f;
    ClampRotator(TargetRot);

    FRotator FinalRot = TargetRot;

    if (UseSmoothing)
    {
        // Simple linear smoothing per frame
        // Normalize deltas so we rotate shortest way
        float YawDelta = TargetRot.Yaw - CurrentRot.Yaw;
        while (YawDelta > 180.f)  YawDelta -= 360.f;
        while (YawDelta < -180.f) YawDelta += 360.f;

        float PitchDelta = TargetRot.Pitch - CurrentRot.Pitch;

        FinalRot.Yaw = CurrentRot.Yaw + YawDelta * SmoothAlpha;
        FinalRot.Pitch = CurrentRot.Pitch + PitchDelta * SmoothAlpha;
        FinalRot.Roll = 0.f;
        ClampRotator(FinalRot);
    }

    Vars->PlayerController->ControlRotation = FinalRot;
}

TArray<AActor*> GetActors(Variables* Vars)
{
    ULevel* Level = Vars->Level;
    while (!Level)
    {
	    return TArray<AActor*>();
    }
    

    Actors = Level->Actors; // TArray<AActor*>
}