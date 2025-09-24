#include "Cheats.h"
#include "Utils.h"

#include "SDK/Engine_classes.hpp"
#include "SDK/ReadyOrNot_classes.hpp"

using namespace SDK;

void Cheats::ToggleGodMode() {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	RONC->bGodMode = !RONC->bGodMode;
}

void Cheats::ToggleInfAmmo(bool IsEnabled) {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	auto Character = (APlayerCharacter*)RONC;
	Character->GetEquippedWeapon()->bInfiniteAmmo = IsEnabled;
}

void Cheats::UpgradeWeaponStats()
{
	auto* Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto* RONC = Vars->ReadyOrNotChar;
	auto* Character = (APlayerCharacter*)RONC;
	auto* Gun = Character->GetEquippedWeapon();

	// Recoil removal
	Gun->RecoilMultiplierPitch = 0.0f;
	Gun->RecoilMultiplierYaw = 0.0f;
	Gun->FirstShotRecoil = 0.0f;
	Gun->RecoilFireStrength = 0.0f;
	Gun->RecoilFireStrengthFirst = 0.0f;
	Gun->RecoilAngleStrength = 0.0f;
	Gun->RecoilRandomness = 0.0f;
	Gun->RecoilFireADSModifier = 0.0f;
	Gun->RecoilAngleADSModifier = 0.0f;
	Gun->RecoilBuildupADSModifier = 0.0f;
	Gun->RecoilHasBuildup = false;
	Gun->RecoilBuildupDampStrength = 0.0f;
	Gun->Wobble = 0.0f;
	Gun->FirstShotRecoil = 0.0f;
	Gun->FirstShotResetTime = 0.0f;
	Gun->RecoilPattern = TArray<FRotator>();
	Gun->RecoilPositionBuildup = FVector();
	Gun->RecoilRotationBuildup = FRotator();
	Gun->VelocityRecoilMultiplier = 0.0f;
	Gun->ADSRecoilMultiplier = 0.0f;
	Gun->FireCameraShake = nullptr;
	Gun->FireCameraShakeInst = nullptr;
	Gun->Reload_CameraShake = nullptr;
	Gun->ReloadEmpty_CameraShake = nullptr;
	Gun->ProcRecoil_Trans = FVector();
	Gun->ProcRecoil_Rot = FRotator();
	Gun->ProcRecoil_Trans_Buildup = FVector();
	Gun->ProcRecoil_Rot_Buildup = FRotator();
	Gun->bCalculateProcRecoil = false;
	Gun->CurrentHighTimer = 0.0f;
	Gun->FireHighTimer = 0.0f;
	Gun->RecoilFireTime = 0.0f;
	Gun->RecoilReturnRate = 0.0f;
	Gun->RecoilReturnInterpSpeed = 0.0f;
	Gun->RecoilReturnPercentage = 0.0f;
	Gun->RecoilDampStrength = 0.0f;
	Character->WeaponBobRot = FRotator();
	Character->WeaponBobTrans = FVector();
	Gun->bUseFireLoopAnims = false;
	Gun->DisableOrEnableAnimation();
	Character->CameraBobRot = FRotator();
	Character->CameraBobTrans = FVector();
	Character->MeshspaceRecoilMovementMultiplier = FVector();
	Character->RecoilSpeed = 0.0f;
	Character->bFireLoop = false;

	// Spread removal
	Gun->SpreadPattern = FRotator();
	Gun->PendingSpread = FRotator();
	Gun->SpreadReturnRate = 0.0f;
	Gun->FirstShotSpread = 0.0f;
	Gun->VelocitySpreadMultiplier = 0.0f;
	Gun->VelocityRecoilMultiplier = 0.0f;
	Gun->ADSSpreadMultiplier = 0.0f;
	Gun->bIgnoreAmmoTypeSpread = true;

	// Misc
	Gun->AvailableFireModes.Clear();
	Gun->AvailableFireModes.Add(EFireMode::FM_Single);
	Gun->AvailableFireModes.Add(EFireMode::FM_Auto);
	Gun->FireRate = 0.001f;
	Gun->MuzzleFlashChance = 0;
	Gun->PenetrationDistance = 10000;
	Gun->RefireDelay = 0.0f;
}

//void Cheats::SilentAim(Variables* Vars)
//{
//	Vars->Level->Actors;
//	Iterators::TArrayIterator<TArray<class AActor*>> It(Vars->Level->Actors);
//}