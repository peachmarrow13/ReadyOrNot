#include "pch.h"
#include "Cheats.h"
#include "Utils/Utils.h"

//#include "SDK/ReadyOrNot_classes.hpp"

using namespace SDK;

FRChatMessage Msg;
TArray<AActor*> ActorsCopy;

std::vector<bool> CheatToggles;

ImVec2 CheatOptionsWindowSize = ImVec2(0, 0);



void Cheats::ToggleInfAmmo() {
	if (!GVars.ReadyOrNotChar || !GVars.ReadyOrNotChar->GetEquippedWeapon()) return;
	ABaseMagazineWeapon* Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();
	Gun->bInfiniteAmmo = CVars.InfAmmo;

}

void Cheats::PenetrateWalls()
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: RemoveSpread ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: RemoveSpread ; Misc.cpp");
		return;
	}
	auto* Character = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	auto* Gun = Character->GetEquippedWeapon();

	Gun->CurrentAmmoType.PenetrationDistance = 100000;
	Gun->CurrentAmmoType.PenetrationLevel = 10;
	Gun->PenetrationDistance = 10000; // Move this

}

void Cheats::AddAutoFire()
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: AddAutoFire ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: AddAutoFire ; Misc.cpp");
		return;
	}
	auto* Character = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	auto* Gun = Character->GetEquippedWeapon();

	Gun->AvailableFireModes.Add(EFireMode::FM_Auto);
}

void Cheats::InstaKill()
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: InstaKill ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: InstaKill ; Misc.cpp");
		return;
	}

	auto* Character = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	auto* Gun = Character->GetEquippedWeapon();
	Gun->CurrentAmmoType.DismembermentDamage = 100000;
	Gun->CurrentAmmoType.Damage = 10000;
	Gun->CurrentAmmoType.DurabilityDamage = 10000;
}

void Cheats::SetFireRate(float FireRateMult)
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: SetFireRate ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: SetFireRate ; Misc.cpp");
		return;
	}

	auto* Character = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	auto* Gun = Character->GetEquippedWeapon();
	
	Gun->FireRate = 60 / FireRateMult; // Fire rate has to be adjusted from RPM to delay between shots

	//Gun->RefireDelay = 0.0f;
}

void Cheats::RemoveSpread()
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: RemoveSpread ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: RemoveSpread ; Misc.cpp");
		return;
	}

	ABaseMagazineWeapon* Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();

	// Spread removal
	Gun->SpreadPattern = FRotator();
	Gun->PendingSpread = FRotator();
	Gun->SpreadReturnRate = 0.0f;
	Gun->FirstShotSpread = 0.0f;
	Gun->VelocitySpreadMultiplier = 0.0f;
	Gun->VelocityRecoilMultiplier = 0.0f;
	Gun->ADSSpreadMultiplier = 0.0f;
	Gun->bIgnoreAmmoTypeSpread = true;
}

void Cheats::RemoveRecoil()
{
	if (!GVars.ReadyOrNotChar)
	{
		Utils::Error("Character Invalid: RemoveRecoil ; Misc.cpp");
		return;
	}
	if (!GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		Utils::Error("Weapon Invalid: RemoveRecoil ; Misc.cpp");
		return;
	}

	APlayerCharacter* Character = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	ABaseMagazineWeapon * Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();

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
	Character->CameraBobRot = FRotator();
	Character->CameraBobTrans = FVector();
	Character->MeshspaceRecoilMovementMultiplier = FVector();
	Character->RecoilSpeed = 0.0f;
}

void Cheats::SetPlayerSpeed()
{
	APlayerCharacter* PlayerChar = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);

	if (PlayerChar)
	{
		if (GVars.PlayerController->HasAuthority())
			PlayerChar->Server_SetWalkSpeed(240.0f * CVars.Speed, 240.0f * CVars.Speed); // 240 is default walk speed
		else
			PlayerChar->Client_SetWalkSpeed(240.0f * CVars.Speed, 240.0f * CVars.Speed); // 240 is default walk speed
	}
}

void Cheats::AddMag()
{
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController) return;
	auto* Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();
	if (!Gun) return;
	if (!GVars.PlayerController->HasAuthority()) return; // Server_AddMagazine is server-only
	FMagazine NewMag;
	NewMag.Ammo = 30;
	NewMag.AmmoType = 1;
	Gun->Server_AddMagazine(NewMag);
}

void Cheats::ArrestAll(ETeam Team)
{
	if (!GVars.Level) return;
	ULevel* Level = GVars.Level;
	if (Level) {
		ActorsCopy = Level->Actors; // snapshot to prevent mid-iteration changes causing crashes
		if (!ActorsCopy || ActorsCopy.Num() == 0) return;
		if (ActorsCopy)
		{
			for (AActor* Actor : ActorsCopy)
			{
				if (!Utils::IsValidActor(Actor)) continue;

				if (Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass()) || Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass()) || Team == ETeam::TEAM_SWAT && Actor->IsA(ASWATCharacter::StaticClass()))
				{
					AReadyOrNotCharacter* Char = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
					if (!Char) continue;
					if (!Char->VTable) continue;
					if (Char->IsArrested()) continue; // Can't re-arrest already arrested civilians or it will crash
					Char->Arrest(nullptr);
					Char->ArrestComplete(nullptr, nullptr);
					Char->Server_ReportToTOC(Char, false, false);
				}
			}
		}
	}
}

void Cheats::KillAll(ETeam Team)
{
	if (!GVars.Level) return;
	if (ULevel* Level = GVars.Level) {
		ActorsCopy = Level->Actors; // snapshot to prevent mid-iteration changes causing crashes
		if (!ActorsCopy || ActorsCopy.Num() == 0) return;
		if (ActorsCopy)
		{
			for (AActor* Actor : ActorsCopy)
			{
				if (!Utils::IsValidActor(Actor)) continue;

				if (Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass()) || Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass()) || Team == ETeam::TEAM_SWAT && Actor->IsA(ASWATCharacter::StaticClass()))
				{
					if (GVars.PlayerController->HasAuthority())
					{
						reinterpret_cast<APlayerCharacter*>(Actor)->Server_Kill();
						reinterpret_cast<APlayerCharacter*>(Actor)->Server_ReportToTOC(Actor, false, false);
					}
					else
					{
						reinterpret_cast<APlayerCharacter*>(Actor)->Kill();
						reinterpret_cast<APlayerCharacter*>(Actor)->Server_ReportToTOC(Actor, false, false);
					}
					
				}
			}
		}
	}
}

void Cheats::DrawReticle()
{
	if (!CVars.Reticle) return;

	if (!GVars.ReadyOrNotChar) return;

	if (MiscSettings.ReticleWhenThrowing && (!reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar)->bQuickThrowing || !(reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar)->GetEquippedItem() && (reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar)->GetEquippedItem()->ItemType == EItemType::IT_Grenade || reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar)->GetEquippedItem()->ItemType == EItemType::IT_GrenadeNonLethal)))) 
		return;

	if (MiscSettings.CrossReticle)
	{
		ImGui::GetBackgroundDrawList()->AddLine(
			ImVec2((GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x) - 5, GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y),
			ImVec2((GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x) + 5, GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y), Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));
		ImGui::GetBackgroundDrawList()->AddLine(
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x, (GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y) - 5),
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x, (GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y) + 5), Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));
	}
	else
	{
		ImGui::GetBackgroundDrawList()->AddCircleFilled(
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x, GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y),
			MiscSettings.ReticleSize,
			Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));
	}
}

void Cheats::GetAllEvidence()
{
	if (!GVars.Level) return;

	ActorsCopy = GVars.Level->Actors;
	if (!ActorsCopy || ActorsCopy.Num() == 0) return;

	AReadyOrNotCharacter* RONC = nullptr;

	TAllocatedArray<ABaseWeapon*> Weapons(40);

	for (AActor* Actor : ActorsCopy)
	{
		if (!Utils::IsValidActor(Actor)) continue;

		if (Actor->IsA(AReadyOrNotCharacter::StaticClass()))
			RONC = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
		
		if (Actor->IsA(ABaseWeapon::StaticClass()))
		{
			ABaseWeapon* Weapon = reinterpret_cast<ABaseWeapon*>(Actor);

			// Verify both weapon and component are valid
			if (Weapon && Weapon->EvidenceComponent && Weapon->EvidenceComponent->CanBeCollected())
			{
				Weapons.Add(Weapon);
			}
		}
	}

	for (int i = 0; i < Weapons.Num(); i++)
	{
		if (!Weapons[i]) continue;
		if (GVars.ReadyOrNotChar)
			GVars.ReadyOrNotChar->PickupEvidence(Weapons[i]);
		else if (RONC)
			RONC->PickupEvidence(Weapons[i]);
	}
}

void Cheats::TriggerBot()
{
	if (!CVars.TriggerBot) return;
	if (!GVars.PlayerController || !GVars.ReadyOrNotChar) return;

	FHitResult HitResult;

	if (UKismetSystemLibrary::LineTraceSingle(
		GVars.World,
		GVars.PlayerController->PlayerCameraManager->GetCameraLocation(),
		GVars.PlayerController->PlayerCameraManager->GetCameraLocation() + (GVars.PlayerController->PlayerCameraManager->GetActorForwardVector() * 10000.0f),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration,
		&HitResult,
		true,
		FLinearColor(255, 0, 0, 255),
		FLinearColor(255, 255, 255, 255),
		5.0f
	))
	{
		if (HitResult.bBlockingHit && HitResult.HitObjectHandle.Actor.Get())
		{
			AActor* HitActor = HitResult.HitObjectHandle.Actor.Get();
			if (HitActor && (HitActor->IsA(ASuspectCharacter::StaticClass()) || MiscSettings.TriggerBotTargetsCivilians && HitActor->IsA(ACivilianCharacter::StaticClass())))
			{
				if (reinterpret_cast<AReadyOrNotCharacter*>(HitActor)->IsDeadOrUnconscious() || reinterpret_cast<AReadyOrNotCharacter*>(HitActor)->IsIncapacitated() || reinterpret_cast<AReadyOrNotCharacter*>(HitActor)->IsArrestedOrSurrendered())
					return;

				if (MiscSettings.TriggerBotUsesSilentAim && GVars.ReadyOrNotChar->GetEquippedWeapon())
				{
					GVars.ReadyOrNotChar->GetEquippedWeapon()->OnFire(FRotator(), HitResult.ImpactPoint);
					return;
				}
				if (GVars.ReadyOrNotChar->GetEquippedWeapon())
				{
					GVars.ReadyOrNotChar->GetEquippedWeapon()->OnFire(
						GVars.PlayerController->PlayerCameraManager->GetCameraRotation(), // Direction: if we don't set this the bullet just chills
						GVars.PlayerController->PlayerCameraManager->GetCameraLocation()); // Start location
					return;
				}
			}
		}
	}
}

void Cheats::RenderEnabledOptions()
{
	if (!CVars.RenderOptions) return;
	float Hue = fmodf((float)ImGui::GetTime() * 0.2f, 1.0f); // cycles every 5s
	ImVec4 Color = ImColor::HSV(Hue, 1.f, 1.f);

	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin((const char*)u8"已开启功能列表", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowPos(ImVec2(10, 30));

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (const char*)u8"已开启功能:");
	if (CVars.GodMode)
		ImGui::TextColored(Color, (const char*)u8"无敌模式");
	if (CVars.InfAmmo)
		ImGui::TextColored(Color, (const char*)u8"无限弹药");
	if (CVars.Aimbot)
		ImGui::TextColored(Color, (const char*)u8"自瞄");
	if (CVars.ESP)
		ImGui::TextColored(Color, (const char*)u8"透视");
	if (CVars.SpeedEnabled)
		ImGui::TextColored(Color, (const char*)u8"速度修改 x%.1f", CVars.Speed);
	if (CVars.SilentAim)
		ImGui::TextColored(Color, (const char*)u8"静默自瞄");
	if (CVars.NoClip)
		ImGui::TextColored(Color, (const char*)u8"穿墙模式");
	if (CVars.Reticle)
		ImGui::TextColored(Color, (const char*)u8"自定义准星");
	if (CVars.TriggerBot)
		ImGui::TextColored(Color, (const char*)u8"自动射击");

	CheatOptionsWindowSize = ImGui::GetWindowSize();

	ImGui::End();
}

void Cheats::Lean()
{
	if (!GVars.ReadyOrNotChar) return;

	GVars.ReadyOrNotChar->QuickLeanAmount = 100000.0f;
	GVars.ReadyOrNotChar->QuickLeanIntensity = 1000000.0f;
}

void Cheats::ChangeFOV()
{
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController) return;

		GVars.PlayerController->FOV(CVars.FOV);
}

void Cheats::AutoWin()
{
	if (!GVars.GameState || !GVars.ReadyOrNotChar) return;

	AReadyOrNotGameState* GameState = GVars.GameState;

	ArrestAll(ETeam::TEAM_CIVILIAN);
	ArrestAll(ETeam::TEAM_SUSPECT);
	GetAllEvidence();

	for (AEvidenceActor* Evidence : GameState->AllEvidenceActors)
	{
		if (!Evidence) continue;
		if (Evidence->EvidenceComponent&& Evidence->EvidenceComponent->CanBeCollected())
		{
			for (FScoreBonus& Bonus : Evidence->ScoringComponent->ScoringData.Bonuses)
			{
				Bonus.bEnabled = true;
				Bonus.bGiven = true;
				Bonus.Score = 10000;
			}
			for (FScorePenalty& Penalty : Evidence->ScoringComponent->ScoringData.Penalties)
			{
				Penalty.bEnabled = false;
				Penalty.bGiven = false;
				Penalty.Score = 0;
			}
			Evidence->OnEvidenceStateChanged(EEvidenceActorState::Collected);
		}
	}
	for (AReportableActor* Actor : GameState->AllReportableActors)
	{
		GVars.ReadyOrNotChar->Server_ReportToTOC(Actor, false ,false);
		GVars.ReadyOrNotChar->Server_ReportTarget(Actor);
		Actor->InteractableComponent->OnInteract(((APlayerCharacter*)GVars.ReadyOrNotChar));
	}
	for (AObjective* Objective : GameState->MissionObjectives)
	{
		if (!Objective) continue;
		for (FScoreBonus &Bonus : Objective->ScoringComponent->ScoringData.Bonuses)
		{
			Bonus.bEnabled = true;
			Bonus.bGiven = true;
			Bonus.Score = 10000;
			
		}
		for (FScorePenalty& Penalty : Objective->ScoringComponent->ScoringData.Penalties)
		{
			Penalty.bEnabled = false;
			Penalty.bGiven = false;
			Penalty.Score = 0;
		}
	}
}

void Cheats::UnlockDoors()
{
	if (!GVars.GameState) return;

	AReadyOrNotGameState* GameState = GVars.GameState;

	if (!GameState) return;

	for (ADoor* Door : GameState->AllDoors)
	{
		if (!Door) continue;

		Door->bLocked = false;
	}
}

void Cheats::ListPlayers()
{
	if (!GVars.GameState || !GVars.PlayerController) return;

	float Hue = fmodf((float)ImGui::GetTime() * 0.2f, 1.0f); // cycles every 5s
	ImVec4 Color = ImColor::HSV(Hue, 1.f, 1.f);

	ImGui::SetNextWindowBgAlpha(0.3f);


	ImGui::SetNextWindowPos(ImVec2(10, CheatOptionsWindowSize.y + 30));

	ImGui::Begin((const char*)u8"玩家列表", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (const char*)u8"当前玩家:");

	TArray<APlayerCharacter*> Players = GVars.GameState->AllPlayerCharacters;
	if (!Players || Players.Num() == 0)
	{
		ImGui::End();
		return;
	}

	for (APlayerCharacter* Player : Players)
	{
		if (!Player || !Utils::IsValidActor(Player)) continue;
		if (!Player->PlayerState) continue;
		if (!Player->PlayerState->GetPlayerName()) continue;

		ImGui::TextColored(Color, "%s", Player->PlayerState->GetPlayerName().ToString());

		if (Player->GetController() && Player->GetController() == GVars.PlayerController || Player == GVars.ReadyOrNotChar) continue; // skip ourselves

		if (GVars.PlayerController && GVars.PlayerController->HasAuthority())
		{
			ImGui::SameLine();
			std::string ID = Player->PlayerState->GetPlayerName().ToString();
			ImGui::PushID((ID + "GodMode").c_str());
			if (ImGui::Checkbox((const char*)u8"无敌", &Utils::GetPlayerCheats(Player).GodMode))
			{
				Player->bGodMode = Utils::GetPlayerCheats(Player).GodMode;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID((ID + "InfAmmo").c_str());
			if (ImGui::Checkbox((const char*)u8"无限弹药", &Utils::GetPlayerCheats(Player).InfAmmo))
			{
				if (Player->GetEquippedWeapon())
				{
					Player->GetEquippedWeapon()->bInfiniteAmmo = Utils::GetPlayerCheats(Player).InfAmmo;
				}
					
			}
			ImGui::PopID();
			ImGui::PushID((ID + "Teleport").c_str());
			if (ImGui::Button((const char*)u8"传送到我这"))
			{
				if (GVars.ReadyOrNotChar)
					Player->Server_TeleportPlayerToLocation(GVars.ReadyOrNotChar->K2_GetActorLocation(), GVars.ReadyOrNotChar->K2_GetActorLocation());
			}
			ImGui::PopID();
			ImGui::PushID((ID + "Speed").c_str());
			ImGui::SameLine();
			if (ImGui::Button((const char*)u8"赋予极速"))
			{
				Player->Server_SetWalkSpeed(240.0f * 10, 1000);
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void Cheats::NoClipToggle()
{
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController)
	{
		Utils::Error("[ERROR]: NoClipToggle ; Invalid Player Character or Controller");
		return;
	}

	GVars.ReadyOrNotChar->GetMovementComponent()->MovementState.bCanFly = true;
	auto* RONMovementComponent = GVars.ReadyOrNotChar->CharacterMovement;
	auto* RONCharacter = GVars.ReadyOrNotChar;

	RONMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 5);
	RONCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void Cheats::SurrenderAll(ETeam Team)
{
	if (!GVars.Level) return;
	ULevel* Level = GVars.Level;
	if (Level) {
		ActorsCopy = Level->Actors; // snapshot to prevent mid-iteration changes causing crashes
		if (!ActorsCopy || ActorsCopy.Num() == 0) return;
		if (ActorsCopy)
		{
			for (AActor* Actor : ActorsCopy)
			{
				if (!Actor || !Utils::IsValidActor(Actor)) continue;
				if (Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass()) || Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass()))
				{
					AReadyOrNotCharacter* Char = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
					if (!Char) continue;
					if (Char->IsArrestedOrSurrendered()) continue; // Can't re-surrender already surrendered civilians or it will crash
					Char->OnRep_Surrendered();
					Char->bSurrendered = true;
					Char->bSurrenderComplete = true;
					Char->OnRep_Surrendered();
				}
			}
		}
	}
}
