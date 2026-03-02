#include "pch.h"
#include "Engine.h"
#include "Cheats.h"

using tProcessEvent = void(*)(const UObject*, UFunction*, void*);
tProcessEvent oProcessEvent = nullptr;

extern std::atomic<bool> Cleaning;
extern std::atomic<int> g_PresentCount;

void hkProcessEvent(const UObject* Object, UFunction* Function, void* Params)
{
	if (!Object || !Function || Cleaning.load()) {
		if (oProcessEvent) oProcessEvent(Object, Function, Params);
		return;
	}

	try {
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

			if (bFunctionPass && bObjectPass)
			{
				if (!CVars.SaveDebugToFile)
				{
					printf("Function: %s | Object: %s\n", FuncName.c_str(), ObjName.c_str());
				}
				else
				{
					std::ofstream debugFile("ProcessEventLog.txt", std::ios::app);
					if (debugFile.is_open())
					{
						debugFile << "Function: " << FuncName << " | Class: " << (Object->Class ? Object->Class->GetName() : "None") << " | Object: " << ObjName << "\n";
						debugFile.close();
					}
				}
			}
		}

		if (CVars.SilentAim || CVars.ShootFromReticle)
		{
			// Optimization: Check function name hash or string only if needed
			if (Function->Name.ComparisonIndex != 0) // Basic valid check for name
			{
				const std::string FuncName = Function->GetName();
				if (FuncName == "Server_OnFire")
				{
					if (Object->IsA(ABaseMagazineWeapon::StaticClass()))
					{
						auto* Weapon = static_cast<const ABaseMagazineWeapon*>(Object);
						auto* FireParams = static_cast<Params::BaseMagazineWeapon_OnFire*>(Params);

						if (Weapon->Owner == GVars.ReadyOrNotChar)
						{
							if (CVars.ShootFromReticle && GVars.PlayerController && Utils::IsValidActor(GVars.PlayerController))
							{
								FVector SpawnLoc;
								FVector Direction;
								if (GVars.PlayerController->DeprojectScreenPositionToWorld(
									GVars.ScreenSize.x / 2.0f + MiscSettings.ReticlePosition.x,
									GVars.ScreenSize.y / 2.0f + MiscSettings.ReticlePosition.y,
									&SpawnLoc,
									&Direction
								)) {
									FireParams->SpawnLoc = SpawnLoc;
									FireParams->Direction = UKismetMathLibrary::Conv_VectorToRotator(Direction);
								}
							}

							if (CVars.SilentAim)
								Cheats::SilentAim(FireParams);
						}
					}
				}
			}
		}
	} catch (...) {
		// Suppress exceptions in hook and just proceed to original
	}

	// ALWAYS call original
	if (oProcessEvent) oProcessEvent(Object, Function, Params);
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

