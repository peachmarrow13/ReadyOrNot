#include "Cheats.h"
#include "Utils.h"
#include "SDK/Engine_classes.hpp"
#include "SDK/ReadyOrNot_classes.hpp"

void Cheats::ToggleGodMode() {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	RONC->bGodMode = !RONC->bGodMode;
}

void Cheats::RefillAmmo() {
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	auto Character = (APlayerCharacter*)RONC;
	Character->ReplenishAllMagazineAmmo();
}

void Cheats::ToggleRecoil()
{
	auto Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController) return;
	if (!Vars || !Vars->ReadyOrNotChar) return;
	auto RONC = Vars->ReadyOrNotChar;
	auto Character = (APlayerCharacter*)RONC;
	// Recoil removal
	Character->GetEquippedWeapon()->RecoilMultiplierPitch = 0.0f;
	Character->GetEquippedWeapon()->RecoilMultiplierYaw = 0.0f;
	Character->GetEquippedWeapon()->FirstShotRecoil = 0.0f;
	Character->GetEquippedWeapon()->RecoilFireStrength = 0.0f;
	Character->GetEquippedWeapon()->RecoilFireStrengthFirst = 0.0f;
	Character->GetEquippedWeapon()->RecoilAngleStrength = 0.0f;
	Character->GetEquippedWeapon()->RecoilRandomness = 0.0f;
	Character->GetEquippedWeapon()->RecoilFireADSModifier = 0.0f;
	Character->GetEquippedWeapon()->RecoilAngleADSModifier = 0.0f;
	Character->GetEquippedWeapon()->RecoilBuildupADSModifier = 0.0f;
	Character->GetEquippedWeapon()->RecoilHasBuildup = false;
	Character->GetEquippedWeapon()->RecoilBuildupDampStrength = 0.0f;
	Character->GetEquippedWeapon()->Wobble = 0.0f;

	// Spread removal
	Character->GetEquippedWeapon()->SpreadPattern = FRotator();
	Character->GetEquippedWeapon()->PendingSpread = FRotator();
	Character->GetEquippedWeapon()->SpreadReturnRate = 0.0f;
	Character->GetEquippedWeapon()->FirstShotSpread = 0.0f;
	Character->GetEquippedWeapon()->VelocitySpreadMultiplier = 0.0f;
	Character->GetEquippedWeapon()->VelocityRecoilMultiplier = 0.0f;
	Character->GetEquippedWeapon()->ADSSpreadMultiplier = 0.0f;
	Character->GetEquippedWeapon()->bIgnoreAmmoTypeSpread = true;


}