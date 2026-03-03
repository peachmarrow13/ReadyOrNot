#include "pch.h"
#include "Engine.h"

void Cheats::SilentAim(Params::BaseMagazineWeapon_OnFire* FireParams)
{
	if (!CVars.SilentAim) return;

	if (!GVars.ReadyOrNotChar) return;

	AActor* TargetActor =
		Utils::GetBestTarget(
			GVars.PlayerController,
			SilentAimSettings.TargetCivilians,
			SilentAimSettings.TargetArrested,
			SilentAimSettings.TargetSurrendered,
			SilentAimSettings.TargetDead,
			SilentAimSettings.MaxFOV,
			SilentAimSettings.RequiresLOS,
			TextVars.SilentAimBone,
			SilentAimSettings.TargetAll);

		if (!TargetActor) return;

		float RandomValue = UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f);
		bool bShouldShoot = (RandomValue <= SilentAimSettings.HitChance);
		if (!bShouldShoot) return;

		std::wstring WideString = UtfN::StringToWString(TextVars.SilentAimBone);
		FName BoneName = UKismetStringLibrary::Conv_StringToName(WideString.c_str());

		if (!((AReadyOrNotCharacter*)TargetActor)->Mesh) return;
		FVector TargetLocation = ((AReadyOrNotCharacter*)TargetActor)->Mesh->GetBoneTransform(BoneName, ERelativeTransformSpace::RTS_World).Translation;

	if (SilentAimSettings.MagicBullet)
	{
		FireParams->SpawnLoc = TargetLocation;
	}
	else
	{
		FVector MuzzleLocation = FireParams->SpawnLoc;
		FVector DirectionVec = TargetLocation - MuzzleLocation;

		FireParams->Direction = UKismetMathLibrary::Conv_VectorToRotator(DirectionVec);
	}
}
