#include "pch.h"
#include <Windows.h>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "Cheats.h"
#include "Utils.h"

#include "SDK/Engine_classes.hpp"
//#include "SDK/ReadyOrNot_classes.hpp"

#define VAR_NAME(x) #x

using namespace SDK;

FRChatMessage Msg;
TArray<AActor*> ActorsCopy;

std::vector<bool> CheatToggles;

ImVec2 CheatOptionsWindowSize = ImVec2(0, 0);

void Cheats::ToggleGodMode() {
	if (!GVars.PlayerController) return;
	if (!GVars.ReadyOrNotChar) return;
	auto* RONC = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);
	if (RONC->bGodMode != CVars.GodMode)
	{
		if (GVars.HasAuthority)
		{
			RONC->Server_ToggleGodMode();
		}
		else
			RONC->ToggleGodMode();
		RONC->IncreaseHealth(100000.0f);
		RONC->CharacterHealth->bEnableIncapacitation = false;
		RONC->CharacterHealth->bUnlimited = true;
		RONC->CharacterHealth->SetUnlimitedResource(true);
		RONC->CharacterHealth->EnableUnlimitedResource();
	}
}

void Cheats::ToggleInfAmmo() {
	if (!GVars.ReadyOrNotChar || !GVars.ReadyOrNotChar->GetEquippedWeapon()) 
		return;

	ABaseMagazineWeapon* Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();
	Gun->bInfiniteAmmo = CVars.InfAmmo;
	Gun->ReplenishAmmo();
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
	
	printf("Original Fire Rate: %f\n", Gun->FireRate);

	Gun->FireRate = 60.0f / FireRateMult; // Fire rate has to be adjusted from RPM to delay between shots
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
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController) return;

	APlayerCharacter* PlayerChar = reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar);

	if (PlayerChar)
	{
		if (GVars.HasAuthority)
			PlayerChar->Server_SetWalkSpeed(240.0f * CVars.Speed, 240.0f * CVars.Speed); // 240 is default walk speed
		else
			PlayerChar->Client_SetWalkSpeed(240.0f * CVars.Speed, 240.0f * CVars.Speed); // 240 is default walk speed
	}
}

void Cheats::AddMag()
{
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController || !GVars.HasAuthority) 
		return;

	auto* Gun = GVars.ReadyOrNotChar->GetEquippedWeapon();
	if (!Gun) return;

	FMagazine NewMag;
	NewMag.Ammo = Gun->MagazineCountDefault;
	NewMag.AmmoType = 1;
	Gun->Server_AddMagazine(NewMag);
}

enum class EArrestStage : uint8_t
{
	Surrender,
	Arrest,
	ArrestComplete,
	WaitForArrested,
	ReportToTOC
};

struct FPendingArrest
{
	ACyberneticCharacter* Char = nullptr;
	EArrestStage Stage = EArrestStage::Surrender;
	ULONGLONG LastStepMs = 0;
	int WaitTicks = 0;
};

std::vector<FPendingArrest> GPendingArrests;

constexpr ULONGLONG ArrestStepDelayMs = 120;
constexpr int MaxArrestOpsPerFrame = 2;
constexpr int MaxWaitTicks = 15;

bool MatchesTeam(AActor* Actor, ETeam Team)
{
	return (Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass())) ||
		(Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass())) ||
		(Team == ETeam::TEAM_SWAT && Actor->IsA(ASWATCharacter::StaticClass()));
}

void Cheats::ArrestAll(ETeam Team)
{
	if (!GVars.Level || !GVars.ReadyOrNotChar || !GVars.PlayerController || !GVars.HasAuthority)
		return;

	ULevel* Level = GVars.Level;
	if (!Level)
		return;

	ActorsCopy = Level->Actors;
	if (!ActorsCopy || ActorsCopy.Num() == 0)
		return;

	for (AActor* Actor : ActorsCopy)
	{
		if (!Actor || !Utils::IsValidActor(Actor) || !GVars.ReadyOrNotChar)
			continue;

		if (!MatchesTeam(Actor, Team))
			continue;

		ACyberneticCharacter* Char = reinterpret_cast<ACyberneticCharacter*>(Actor);
		if (!Char || Char->IsArrested())
			continue;

		bool bAlreadyQueued = false;
		for (const FPendingArrest& Entry : GPendingArrests)
		{
			if (Entry.Char == Char)
			{
				bAlreadyQueued = true;
				break;
			}
		}
		if (bAlreadyQueued)
			continue;

		Char->ForceComplianceStrength = 0.0f;
		Char->AbuseCount = 0;

		FPendingArrest NewEntry;
		NewEntry.Char = Char;
		NewEntry.Stage = EArrestStage::Surrender;
		NewEntry.LastStepMs = 0;
		NewEntry.WaitTicks = 0;
		GPendingArrests.push_back(NewEntry);
	}
}

void Cheats::ProcessArrestQueue()
{
	if (GPendingArrests.empty())
		return;

	if (!GVars.ReadyOrNotChar || !GVars.PlayerController || !GVars.HasAuthority)
	{
		GPendingArrests.clear();
		return;
	}

	const ULONGLONG Now = GetTickCount64();
	int OpsThisFrame = 0;

	for (size_t i = 0; i < GPendingArrests.size() && OpsThisFrame < MaxArrestOpsPerFrame; )
	{
		FPendingArrest& Entry = GPendingArrests[i];
		ACyberneticCharacter* Char = Entry.Char;

		if (!Char || !Utils::IsValidActor(Char))
		{
			GPendingArrests.erase(GPendingArrests.begin() + static_cast<long long>(i));
			continue;
		}

		if (Now - Entry.LastStepMs < ArrestStepDelayMs)
		{
			++i;
			continue;
		}

		switch (Entry.Stage)
		{
		case EArrestStage::Surrender:
			Char->Surrender();
			Entry.Stage = EArrestStage::Arrest;
			Entry.LastStepMs = Now;
			++OpsThisFrame;
			++i;
			break;

		case EArrestStage::Arrest:
			Char->Arrest(GVars.ReadyOrNotChar);
			Entry.Stage = EArrestStage::ArrestComplete;
			Entry.LastStepMs = Now;
			++OpsThisFrame;
			++i;
			break;

		case EArrestStage::ArrestComplete:
			Char->ArrestComplete(GVars.ReadyOrNotChar, nullptr);
			Entry.Stage = EArrestStage::WaitForArrested;
			Entry.LastStepMs = Now;
			++OpsThisFrame;
			++i;
			break;

		case EArrestStage::WaitForArrested:
			if (Char->IsArrested())
			{
				Entry.Stage = EArrestStage::ReportToTOC;
				Entry.LastStepMs = Now;
			}
			else
			{
				Entry.WaitTicks++;
				if (Entry.WaitTicks >= MaxWaitTicks)
				{
					Char->Arrest(GVars.ReadyOrNotChar);
					Char->ArrestComplete(GVars.ReadyOrNotChar, nullptr);
					Entry.WaitTicks = 0;
					Entry.LastStepMs = Now;
					++OpsThisFrame;
				}
			}
			++i;
			break;

		case EArrestStage::ReportToTOC:
			if (Char->IsArrested())
				GVars.ReadyOrNotChar->Server_ReportToTOC(Char, false, false);

			GPendingArrests.erase(GPendingArrests.begin() + static_cast<long long>(i));
			++OpsThisFrame;
			break;
		}
	}
}

void Cheats::KillAll(ETeam Team)
{
	if (!GVars.Level) return;
	if (ULevel* Level = GVars.Level) {
		ActorsCopy = Level->Actors; // snapshot to prevent mid-iteration changes causing crashes
		if (!ActorsCopy || ActorsCopy.Num() == 0) return;

		for (AActor* Actor : ActorsCopy)
		{
			if (!Actor || !Utils::IsValidActor(Actor) || !GVars.ReadyOrNotChar) continue;

			if ((Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass())) ||
				(Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass())) ||
				(Team == ETeam::TEAM_SWAT && Actor->IsA(ASWATCharacter::StaticClass())))
			{
				if (GVars.HasAuthority)
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

void Cheats::DrawReticle()
{
	if (!CVars.Reticle || !GVars.ReadyOrNotChar) return;

	if (MiscSettings.ReticleWhenThrowing && !reinterpret_cast<APlayerCharacter*>(GVars.ReadyOrNotChar)->bQuickThrowing) 
		return;

	if (MiscSettings.CrossReticle)
	{
		ImGui::GetBackgroundDrawList()->AddLine(
			ImVec2((GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x) - 5,
				GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y),
			ImVec2((GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x) + 5,
				GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y), Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));

		ImGui::GetBackgroundDrawList()->AddLine(
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x,
				(GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y) - 5),
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x,
				(GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y) + 5), Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));
	}
	else
	{
		ImGui::GetBackgroundDrawList()->AddCircleFilled(
			ImVec2(GVars.ScreenSize.x / 2 + MiscSettings.ReticlePosition.x, GVars.ScreenSize.y / 2 + MiscSettings.ReticlePosition.y),
			MiscSettings.ReticleSize, Utils::ConvertImVec4toU32(MiscSettings.ReticleColor));
	}
}

void Cheats::GetAllEvidence()
{
	if (!GVars.Level || !GVars.ReadyOrNotChar)
		return;

	ActorsCopy = GVars.Level->Actors;
	if (!ActorsCopy || ActorsCopy.Num() == 0)
		return;

	for (AActor* Actor : ActorsCopy)
	{
		if (!Actor || !Utils::IsValidActor(Actor)) 
			continue;
		
		if (Actor->IsA(ABaseWeapon::StaticClass()))
		{
			ABaseWeapon* Weapon = reinterpret_cast<ABaseWeapon*>(Actor);

			if (Weapon && Weapon->EvidenceComponent && Weapon->EvidenceComponent->CanBeCollected())
			{
				GVars.ReadyOrNotChar->PickupEvidence(Weapon);
			}
		}
	}
}

void Cheats::TriggerBot()
{
	if (!CVars.TriggerBot || !GVars.PlayerController || !GVars.ReadyOrNotChar)
		return;

	ABaseMagazineWeapon* Weapon = GVars.ReadyOrNotChar->GetEquippedWeapon();
	if (!Weapon)
		return;

	FHitResult HitResult;

	if (UKismetSystemLibrary::LineTraceSingle(
		GVars.World,
		GVars.PlayerController->PlayerCameraManager->GetCameraLocation(),
		GVars.PlayerController->PlayerCameraManager->GetCameraLocation() + (GVars.PlayerController->PlayerCameraManager->GetActorForwardVector() * 10000.0f),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		&HitResult,
		true,
		FLinearColor(0, 0, 0, 0),
		FLinearColor(0, 0, 0, 0),
		5.0f
	))
	{
		if (HitResult.bBlockingHit && HitResult.HitObjectHandle.Actor.Get())
		{
			AActor* HitActor = HitResult.HitObjectHandle.Actor.Get();
			if (HitActor && (HitActor->IsA(ASuspectCharacter::StaticClass()) || MiscSettings.TriggerBotTargetsCivilians && HitActor->IsA(ACivilianCharacter::StaticClass())))
			{
				AReadyOrNotCharacter* Target = reinterpret_cast<AReadyOrNotCharacter*>(HitActor);

				if (Target->IsDeadOrUnconscious()
					|| Target->IsArrestedOrSurrendered()
					|| Target->IsDowned()
					|| Target->IsIncapacitated()
					|| Target->IsSurrendered())
					return;

				if (MiscSettings.TriggerBotUsesSilentAim)
				{
					Weapon->OnFire(FRotator(), HitResult.ImpactPoint);
					return;
				}
				else
				{
					Weapon->OnFire(
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
	float Hue = fmodf(ImGui::GetTime() * 0.2, 1.0f); // cycles every 5s
	ImVec4 Color = ImColor::HSV(Hue, 1.f, 1.f);

	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Enabled Options", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowPos(ImVec2(10, 30));

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Enabled Options:");
	if (CVars.GodMode)
		ImGui::TextColored(Color, "God Mode");
	if (CVars.InfAmmo)
		ImGui::TextColored(Color, "Infinite Ammo");
	if (CVars.Aimbot)
		ImGui::TextColored(Color, "Aimbot");
	if (CVars.ESP)
		ImGui::TextColored(Color, "ESP");
	if (CVars.SpeedEnabled)
		ImGui::TextColored(Color, "Speed x%.1f", CVars.Speed);
	if (CVars.SilentAim)
		ImGui::TextColored(Color, "Silent Aim");
	if (CVars.Reticle)
		ImGui::TextColored(Color, "Reticle");
	if (CVars.TriggerBot)
		ImGui::TextColored(Color, "Trigger Bot");
	if (CVars.AntiSway)
		ImGui::TextColored(Color, "Anti Sway");
	if (CVars.BulletTime)
		ImGui::TextColored(Color, "Bullet Time");
	if (CVars.ShootFromReticle)
		ImGui::TextColored(Color, "Shoot From Reticle");

	CheatOptionsWindowSize = ImGui::GetWindowSize();

	ImGui::End();
}

void Cheats::ChangeFOV()
{
	if (!GVars.ReadyOrNotChar || !GVars.PlayerController) return;

		GVars.PlayerController->FOV(CVars.FOV);
}

void Cheats::AutoWin()
{
	if (!GVars.GameState || !GVars.ReadyOrNotChar || !GVars.PlayerController || !GVars.HasAuthority) return;

	AReadyOrNotGameState* GameState = GVars.GameState;

	ArrestAll(ETeam::TEAM_CIVILIAN);
	ArrestAll(ETeam::TEAM_SUSPECT);
	GetAllEvidence();

	for (AEvidenceActor* Evidence : GameState->AllEvidenceActors)
	{
		if (!Evidence) 
			continue;

		if (Evidence->EvidenceComponent && Evidence->EvidenceComponent->CanBeCollected())
		{
			for (FScoreBonus& Bonus : Evidence->ScoringComponent->ScoringData.Bonuses)
			{
				Bonus.bEnabled = true;
				Bonus.bGiven = true;
				Bonus.Score = 10000;
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
		if (!Objective) 
			continue;

		for (FScoreBonus &Bonus : Objective->ScoringComponent->ScoringData.Bonuses)
		{
			Bonus.bEnabled = true;
			Bonus.bGiven = true;
			Bonus.Score = 10000;
		}

		Objective->ObjectiveCompleted();
		Objective->OnObjectiveCompleted();
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
	if (!GVars.GameState || !GVars.PlayerController || !GVars.Level) return;

	float Hue = fmodf(ImGui::GetTime() * 0.2, 1.0f); // cycles every 5s
	ImVec4 Color = ImColor::HSV(Hue, 1.f, 1.f);

	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::SetNextWindowPos(ImVec2(10, CheatOptionsWindowSize.y + 30));

	ImGui::Begin("Players", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Players:");

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

		ImGui::TextColored(Color, "%s", Player->PlayerState->GetPlayerName().ToString().c_str());

		if (Player->GetController() && Player->GetController() == GVars.PlayerController || Player == GVars.ReadyOrNotChar) continue; // skip ourselves

		if (GVars.PlayerController && GVars.HasAuthority)
		{
			ImGui::SameLine();
			std::string ID = Player->PlayerState->GetPlayerName().ToString();
			ImGui::PushID((ID + "GodMode").c_str());
			if (ImGui::Checkbox("GodMode", &Utils::GetPlayerCheats(Player).GodMode))
			{
				Player->bGodMode = Utils::GetPlayerCheats(Player).GodMode;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID((ID + "InfAmmo").c_str());
			if (ImGui::Checkbox("InfAmmo", &Utils::GetPlayerCheats(Player).InfAmmo))
			{
				if (Player->GetEquippedWeapon())
				{
					Player->GetEquippedWeapon()->bInfiniteAmmo = Utils::GetPlayerCheats(Player).InfAmmo;
				}
					
			}
			ImGui::PopID();
			ImGui::PushID((ID + "Teleport").c_str());
			if (ImGui::Button("Teleport To Self"))
			{
				if (GVars.ReadyOrNotChar)
					Player->Server_TeleportPlayerToLocation(GVars.ReadyOrNotChar->K2_GetActorLocation(), GVars.ReadyOrNotChar->K2_GetActorLocation());
			}
			ImGui::PopID();
			ImGui::PushID((ID + "Speed").c_str());
			ImGui::SameLine();
			if (ImGui::Button("Speed"))
			{
				Player->Server_SetWalkSpeed(240.0f * 10, 1000);
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void Cheats::SurrenderAll(ETeam Team)
{
	if (!GVars.Level || !GVars.PlayerController || !GVars.HasAuthority) 
		return; // Crashes if you aren't host because we use a replicate function.

	ULevel* Level = GVars.Level;

	if (Level) {
		if (!ActorsCopy || ActorsCopy.Num() == 0) 
			return;

		for (AActor* Actor : ActorsCopy)
		{
			if (!Actor || !Utils::IsValidActor(Actor))
				continue;

			if (Team == ETeam::TEAM_CIVILIAN && Actor->IsA(ACivilianCharacter::StaticClass()) || Team == ETeam::TEAM_SUSPECT && Actor->IsA(ASuspectCharacter::StaticClass()))
			{
				AReadyOrNotCharacter* Char = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
				if (!Char) 
					continue;
				if (Char->IsArrestedOrSurrendered())
					continue;
				Char->bSurrendered = true;
				Char->bSurrenderComplete = true;
				Char->OnRep_Surrendered();
				Sleep(10); // Without this, if there are a lot of actors it can cause some weird replication issues that can crash the game.
			}
		}
	}
}

void Cheats::AntiSway()
{
	if (GVars.ReadyOrNotChar && GVars.ReadyOrNotChar->GetEquippedWeapon())
	{
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobScaleH = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->DrawCameraShake = nullptr;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->bCalculateProcRecoil = false;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->InertiaDragStrafeRotation = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->InertiaDragStrafeLocation = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->InertiaDragAimLocation = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->InertiaDragAimRotation = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobScaleV = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobSpeedScaleH = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobSpeedScaleV = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobAmplitudeBaseSpeed = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobAmplitudeWalkScale = 0.0f;
		GVars.ReadyOrNotChar->GetEquippedWeapon()->CameraBobAmplitudeSprintScale = 0.0f;
	}
}

void Cheats::GiveAchievements()
{
	if (!GVars.ReadyOrNotChar) return;

	AReadyOrNotPlayerState* PlayerState = reinterpret_cast<AReadyOrNotPlayerState*>(GVars.ReadyOrNotChar->PlayerState);

	for (int i = 0; i < 68; i++)
	{
		PlayerState->Client_GrantAchievement(static_cast<EAchievement>(i));
		if (i < 47)
			PlayerState->Client_IncreaseAchievementStat(static_cast<EAchievementStats>(i), 100000, GVars.ReadyOrNotChar, GVars.ReadyOrNotChar);
	}
	/*for (int i = 0; i < 47; i++)
	{
		PlayerState->Client_IncreaseAchievementStat(static_cast<EAchievementStats>(i), 100000, GVars.ReadyOrNotChar, GVars.ReadyOrNotChar);
	}*/
}
