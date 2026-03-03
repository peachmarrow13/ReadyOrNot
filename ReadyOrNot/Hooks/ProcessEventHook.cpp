#include "pch.h"
#include "Engine.h"
#include "Cheats.h"

using tProcessEvent = void(*)(const UObject*, UFunction*, void*);
tProcessEvent oProcessEvent = nullptr;

extern std::atomic<bool> Cleaning;
extern std::atomic<int> g_PresentCount;

void hkProcessEvent(const UObject* Object, UFunction* Function, void* Params)
{
	static thread_local bool bInsideHook = false;
	if (!Object || !Function || Cleaning.load() || bInsideHook) {
		if (oProcessEvent) oProcessEvent(Object, Function, Params);
		return;
	}

	bInsideHook = true;

	try {
        // Debug logging (Move inside condition or throttle if needed)
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

		if (CVars.SilentAim || CVars.ShootFromReticle || ESPSettings.BulletTracers)
		{
			if (Function->Name.ComparisonIndex != 0)
			{
				if (Object->IsA(ABaseMagazineWeapon::StaticClass()))
				{
					const std::string FuncName = Function->GetName();
					if (FuncName == "Server_OnFire")
					{
						auto* Weapon = static_cast<const ABaseMagazineWeapon*>(Object);
						auto* FireParams = static_cast<Params::BaseMagazineWeapon_OnFire*>(Params);

						if (Weapon && FireParams && GVars.ReadyOrNotChar && Utils::IsValidActor(GVars.ReadyOrNotChar) && Weapon->Owner == GVars.ReadyOrNotChar)
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

							if (ESPSettings.BulletTracers)
							{
								BulletTracer NewTracer;
								NewTracer.Start = FireParams->SpawnLoc;
								// Direction is an FRotator. We need to convert it to a vector and multiply by a reasonable distance, e.g., 10000.
								FVector DirectionVec = Utils::FRotatorToVector(FireParams->Direction);
								NewTracer.End = FVector(NewTracer.Start.X + DirectionVec.X * 10000.0, 
														NewTracer.Start.Y + DirectionVec.Y * 10000.0, 
														NewTracer.Start.Z + DirectionVec.Z * 10000.0);
								
								// Perform LineTrace to find the actual impact point
								TArray<AActor*> IgnoreActors;
								IgnoreActors.Add(GVars.ReadyOrNotChar);
								
								FHitResult HitResult;
								bool bDidHit = UKismetSystemLibrary::LineTraceSingle(
									GVars.World,
									NewTracer.Start,
									NewTracer.End,
									ETraceTypeQuery::TraceTypeQuery1,
									true,
									IgnoreActors,
									EDrawDebugTrace::None,
									&HitResult,
									true,
									FLinearColor(),
									FLinearColor(),
									1.0f
								);
								
								if (bDidHit) {
									NewTracer.End = HitResult.Location;
									NewTracer.bHit = true;
								} else {
									NewTracer.bHit = false;
								}

								NewTracer.CreationTime = ImGui::GetTime();
								
								{
									std::lock_guard<std::mutex> lock(TracerMutex);
									BulletTracersList.push_back(NewTracer);
								}
							}
						}
					}
				}
			}
		}
	} catch (...) {
		// Suppress exceptions
	}

	bInsideHook = false;
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

