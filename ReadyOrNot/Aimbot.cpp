#include "pch.h"
#include "Cheats.h"
#include "Utils.h"
#include <chrono>
#include <cmath>
#include <numbers>

bool Init = false;

void Cheats::Aimbot()
{
	if (!CVars.Aimbot) return;

	if (AimbotSettings.DrawFOV)
		Utils::DrawFOV(AimbotSettings.MaxFOV, AimbotSettings.FOVThickness);

	if (!GVars.POV || !GVars.PlayerController || !GVars.Level || !GVars.ReadyOrNotChar) return;

	if (AimbotSettings.RequireKeyHeld && !AimbotKeyDown)
		return;

	AActor* Target = Utils::GetBestTarget(
		GVars.PlayerController,
		AimbotSettings.TargetCivilians,
		AimbotSettings.TargetArrested,
		false, // TargetSurrendered (Temporary fix for struct member issue)
		AimbotSettings.TargetDead,
		AimbotSettings.MaxFOV,
		AimbotSettings.LOS,
		TextVars.AimbotBone,
		AimbotSettings.TargetAll
	);

	if (!Target) return;

	static std::string CachedBoneString = "";
	static FName CachedBoneName;
	if (CachedBoneString != TextVars.AimbotBone) {
		std::wstring WideString = UtfN::StringToWString(TextVars.AimbotBone);
		CachedBoneName = UKismetStringLibrary::Conv_StringToName(WideString.c_str());
		CachedBoneString = TextVars.AimbotBone;
	}

	FVector CameraPos = GVars.POV->Location;
	FVector TargetPos = ((AReadyOrNotCharacter*)Target)->Mesh->GetBoneTransform(CachedBoneName, ERelativeTransformSpace::RTS_World).Translation;

	double Dist = CameraPos.GetDistanceToInMeters(TargetPos);

	if (AimbotSettings.MinDistance > Dist || Dist > AimbotSettings.MaxDistance)
		return;

	if (AimbotSettings.DrawArrow)
		Utils::DrawSnapLine(TargetPos, AimbotSettings.ArrowThickness);

	FVector AngleDiff = {
	TargetPos.X - CameraPos.X,
	TargetPos.Y - CameraPos.Y,
	TargetPos.Z - CameraPos.Z
	};

	// compute desired yaw/pitch from AngleDiff
	float DesiredYaw = (float)(atan2(AngleDiff.Y, AngleDiff.X) * 180.0f / std::numbers::pi);
	float DesiredPitch = (float)(atan2(AngleDiff.Z, std::sqrt(AngleDiff.X * AngleDiff.X + AngleDiff.Y * AngleDiff.Y)) * 180.0f / std::numbers::pi);

	if (AimbotSettings.Smooth)
	{
		// Get current control rotation
		FRotator CurrentRot = GVars.PlayerController->ControlRotation;

		// Convert current rotator to a forward direction vector
		double yawRad = CurrentRot.Yaw * std::numbers::pi / 180.0;
		double pitchRad = CurrentRot.Pitch * std::numbers::pi / 180.0;

		FVector CurrentDir;
		CurrentDir.X = (float)(std::cos(pitchRad) * std::cos(yawRad));
		CurrentDir.Y = (float)(std::cos(pitchRad) * std::sin(yawRad));
		CurrentDir.Z = (float)(std::sin(pitchRad));

		// Target direction (normalized)
		float Dist = std::sqrt(AngleDiff.X * AngleDiff.X + AngleDiff.Y * AngleDiff.Y + AngleDiff.Z * AngleDiff.Z);
		FVector TargetDir = Dist > 0.0f ? FVector(AngleDiff.X / Dist, AngleDiff.Y / Dist, AngleDiff.Z / Dist) : CurrentDir;

		// Guard against zero or negative smoothing values
		float SmoothFactor = 5.0f;

		SmoothFactor = AimbotSettings.SmoothingVector;

		if (SmoothFactor <= 0.0f) SmoothFactor = 5.0f;

		FVector DeltaDir = TargetDir - CurrentDir;
		FVector SmoothedDir = CurrentDir + (DeltaDir / SmoothFactor);
		SmoothedDir.Normalize();

		// Convert smoothed direction back to rotator (yaw/pitch)
		float SmoothedYaw = (float)(atan2(SmoothedDir.Y, SmoothedDir.X) * 180.0f / std::numbers::pi);
		float SmoothedPitch = (float)(atan2(SmoothedDir.Z, std::sqrt(SmoothedDir.X * SmoothedDir.X + SmoothedDir.Y * SmoothedDir.Y)) * 180.0f / std::numbers::pi);

		GVars.PlayerController->ControlRotation.Yaw = SmoothedYaw;
		GVars.PlayerController->ControlRotation.Pitch = SmoothedPitch;
	}
	else
	{
		GVars.PlayerController->ControlRotation.Yaw = DesiredYaw;
		GVars.PlayerController->ControlRotation.Pitch = DesiredPitch;
	}
}