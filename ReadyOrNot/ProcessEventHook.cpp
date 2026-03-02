#include "pch.h"
#include "Engine.h"

void** vTable = nullptr;

using tProcessEvent = void(*)(const UObject*, UFunction*, void*);
tProcessEvent oProcessEvent = nullptr;


void hkProcessEvent(const UObject* Object, UFunction* Function, void* Params)
{
	if (Function)
	{
		if (CVars.Debug)
		{
			printf("Function Index: %d", Function->Index);
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

		if (CVars.SilentAim || CVars.ShootFromReticle)
		{
			if (strcmp(Function->GetName().c_str(), "Server_OnFire") == 0)
			{
				
				auto* FireParams =
					reinterpret_cast<Params::BaseMagazineWeapon_OnFire*>(Params);

				bool OwnerIsLocalPlayer = reinterpret_cast<const ABaseMagazineWeapon*>(Object)->Owner == GVars.ReadyOrNotChar;

				if (CVars.ShootFromReticle && OwnerIsLocalPlayer && GVars.PlayerController && Utils::IsValidActor(GVars.PlayerController))
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
