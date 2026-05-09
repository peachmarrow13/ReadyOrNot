#include "pch.h"
#include "Engine.h"

void** vTable = nullptr;

using tProcessEvent = void(*)(const UObject*, UFunction*, void*);
tProcessEvent oProcessEvent = nullptr;

inline tProcessEvent oPlayerProcessEvent = nullptr;
bool PlayerHooked = false;

void hkPlayerProcessEvent(const SDK::UObject* Object, SDK::UFunction* Function, void* Params)
{
	if (!Function)
		return oPlayerProcessEvent(Object, Function, Params);

	if (Function->Name.ToString().find("ReceiveTick") != std::string::npos)
	{
		GVars.AutoSetVariables();

		switch (CVars.QueuedAction)
		{
			case EQueuedAction::GiveAchievements:
			{
				Cheats::GiveAchievements();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::ToggleGodMode:
			{
				Cheats::ToggleGodMode();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::ToggleInfAmmo:
			{
				Cheats::ToggleInfAmmo();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::RemoveRecoil:
			{
				Cheats::RemoveRecoil();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::RemoveSpread:
			{
				Cheats::RemoveSpread();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::AddAutoFire:
			{
				Cheats::AddAutoFire();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::AddPenetration:
			{
				Cheats::PenetrateWalls();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::AddMagazine:
			{
				Cheats::AddMag();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::SetFireRate:
			{
				Cheats::SetFireRate(3000.0f);
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::InstaKill:
			{
				Cheats::InstaKill();
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
			case EQueuedAction::KillAllSuspects:
			{
				Cheats::KillAll(ETeam::TEAM_SUSPECT);
				CVars.QueuedAction = EQueuedAction::None;
				break;
			}
		}

		if (CVars.TriggerBot)
			Cheats::TriggerBot();

		if (CVars.SpeedEnabled)
			Cheats::SetPlayerSpeed();

		if (CVars.AntiSway)
			Cheats::AntiSway();

		Cheats::ProcessArrestQueue();

		if (CVars.BulletTime && GVars.World)
			GVars.World->K2_GetWorldSettings()->TimeDilation = CVars.BulletTimeSpeed; // Slow-mo
		else if (GVars.World)
			GVars.World->K2_GetWorldSettings()->TimeDilation = 1.0f; // Normal
	}

	oPlayerProcessEvent(Object, Function, Params);
}

void hkProcessEvent(const UObject* Object, UFunction* Function, void* Params)
{
	static int CallCount = 0;
	CallCount++;

	if (Function)
	{
		if ((Function->Name.ToString().find("ReceiveTick") != std::string::npos) && (CallCount % 100 == 0))
			GVars.AutoSetVariables();

		if (CVars.Debug)
		{
			const std::string FuncName = Function->GetName();
			const std::string ObjName = Object->GetName();

			bool bFunctionPass =
				TextVars.DebugFunctionNameMustInclude.empty() ||
				FuncName.find(TextVars.DebugFunctionNameMustInclude) != std::string::npos;

			bool bObjectPass =
				TextVars.DebugFunctionObjectMustInclude.empty() ||
				ObjName.find(TextVars.DebugFunctionObjectMustInclude) != std::string::npos;

			if (bFunctionPass && bObjectPass && !CVars.SaveDebugToFile)
			{
				printf(
					"Function: %s\nClass: %s\nObject: %s\n\n",
					FuncName.c_str(),
					Object->Class->GetName().c_str(),
					ObjName.c_str()
				);
			}
			if (bFunctionPass && bObjectPass && CVars.SaveDebugToFile)
			{
				std::ofstream debugFile("ProcessEventLog.txt", std::ios::app);
				if (debugFile.is_open())
				{
					debugFile << "Function: " << FuncName << "\n";
					debugFile << "Class: " << Object->Class->GetName() << "\n";
					debugFile << "Object: " << ObjName << "\n\n";
					debugFile.close();
				}
			}
		}

		if (!PlayerHooked && Function->Name.ToString().find("ReceiveTick") != std::string::npos && Object->IsA(APlayerCharacter::StaticClass()))
		{
			void** VTable = *reinterpret_cast<void***>(const_cast<UObject*>(Object));
			void* ProcessEventAddr = VTable[SDK::Offsets::ProcessEventIdx];

			MH_STATUS status = MH_CreateHook(
				ProcessEventAddr,
				&hkPlayerProcessEvent,
				reinterpret_cast<void**>(&oPlayerProcessEvent));

			if (status != MH_OK)
			{
				printf("[ERROR] Failed to create ProcessEvent hook! %d\n", status);
				throw std::runtime_error("Failed to create ProcessEvent hook");
			}

			status = MH_EnableHook(ProcessEventAddr);
			if (status != MH_OK)
			{
				printf("[ERROR] Failed to enable ProcessEvent hook!\n");
				throw std::runtime_error("Failed to enable ProcessEvent hook");
			}

			PlayerHooked = true;

			return oProcessEvent(Object, Function, Params);
		}

		if (CVars.InstantMultiTool && Function->GetName() == "GetMultitoolUseTime") // credit to CrimsonSpark for this instant multitool use time
		{
			oProcessEvent(Object, Function, Params);
			if (reinterpret_cast<Params::CanUseMultitoolOn_GetMultitoolUseTime*>(Params)->ReturnValue < 0.1f)
				return;

			reinterpret_cast<Params::CanUseMultitoolOn_GetMultitoolUseTime*>(Params)->ReturnValue = 0.01f;
			return;
		}
		else if (CVars.SilentAim || CVars.ShootFromReticle)
		{
			if (strcmp(Function->GetName().c_str(), "Server_OnFire") == 0)
			{
				bool OwnerIsLocalPlayer = reinterpret_cast<const ABaseMagazineWeapon*>(Object)->Owner == GVars.ReadyOrNotChar;

				if (OwnerIsLocalPlayer)
				{
					auto* FireParams =
						reinterpret_cast<Params::BaseMagazineWeapon_OnFire*>(Params);

					if (CVars.ShootFromReticle && OwnerIsLocalPlayer)
					{
						FVector SpawnLoc;
						FVector Direction;
						GVars.PlayerController->DeprojectScreenPositionToWorld(
							GVars.ScreenSize.x / 2.0f + MiscSettings.ReticlePosition.x,
							GVars.ScreenSize.y / 2.0f + MiscSettings.ReticlePosition.y,
							&SpawnLoc,
							&Direction
						);
						FireParams->SpawnLoc = SpawnLoc;
						FireParams->Direction = UKismetMathLibrary::Conv_VectorToRotator(Direction);

					}

					if (CVars.SilentAim && OwnerIsLocalPlayer)
						Cheats::SilentAim(FireParams);

					for (int i = 0; i < CVars.MultiFire; i++)
					{
						GVars.ReadyOrNotChar->GetEquippedWeapon()->Server_OnFire(FireParams->Direction, FireParams->SpawnLoc, 0);
					}
				}
			}
		}
	}

	// Call original
	oProcessEvent(Object, Function, Params);
}

bool Hooks::HookProcessEvent()
{
	void* ProcessEventAddr = reinterpret_cast<void*>(InSDKUtils::GetImageBase() + Offsets::ProcessEvent);

	if (MH_CreateHook(
		ProcessEventAddr,
		&hkProcessEvent,
		reinterpret_cast<void**>(&oProcessEvent)
	) != MH_OK)
	{
		return false;
	}

	if (MH_EnableHook(ProcessEventAddr) != MH_OK)
		return false;

	return true;
}
