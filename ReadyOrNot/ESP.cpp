#include "Cheats.h"
#include "Utils.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <vcruntime_string.h>
#include <algorithm>

struct BonePair { int Parent; int Child; };
BonePair SuspectSkeletonBones_1[] = {
    {50, 51}, // neck_1 -> Head
    {9, 50}, // spine_3 -> neck_1
    {8, 9}, // spine_2 -> spine_3
    {7, 8}, // spine_1 -> spine_2
    {10, 11}, // clavicle_LE -> upperarm_LE
    {11, 12}, // upperarm_LE -> lowerarm_LE
    {12, 15}, // lowerarm_LE -> hand_LE
    {62, 63}, // clavicle_RI -> upperarm_RI
    {63, 64}, // upperarm_RI -> lowerarm_RI
    {64, 67}, // lowerarm_RI -> hand_RI
    {103, 104}, // thigh_LE -> calf_LE
    {104, 107}, // calf_LE -> foot_LE
    {115, 116}, // thigh_RI -> calf_RI
    {116, 119}, // calf_RI -> foot_RI
};

BonePair SuspectSkeletonBones_2[] = {
    {44, 45}, // Neck -> Head
    {5, 44},  // Spine3 -> Neck
    {3, 5},   // Spine2 -> Spine3
    {2, 3},   // Spine1 -> Spine2
    {6, 7},   // Left clavicle -> UpperArm_LE
    {7, 8},   // UpperArm_LE -> LowerArm_LE
    {8, 11},  // LowerArm_LE -> Hand_LE
    {49, 50}, // Right clavicle -> UpperArm_RI
    {50, 51}, // UpperArm_RI -> LowerArm_RI
    {51, 54}, // LowerArm_RI -> Hand_RI
    {111, 112}, // Thigh_LE -> Calf_LE
    {112, 115}, // Calf_LE -> Foot_LE
    {99, 100},  // Thigh_RI -> Calf_RI
    {100, 103}  // Calf_RI -> Foot_RI
};

BonePair SuspectSkeletonBones[] = {
    {44, 45}, // Neck -> Head
    {5, 44},  // Spine3 -> Neck
    {3, 5},   // Spine2 -> Spine3
    {2, 3},   // Spine1 -> Spine2
    {6, 7},   // Left clavicle -> UpperArm_LE
    {7, 8},   // UpperArm_LE -> LowerArm_LE
    {8, 11},  // LowerArm_LE -> Hand_LE
    {49, 50}, // Right clavicle -> UpperArm_RI
    {50, 51}, // UpperArm_RI -> LowerArm_RI
    {51, 54}, // LowerArm_RI -> Hand_RI
    {111, 112}, // Thigh_LE -> Calf_LE
    {112, 115}, // Calf_LE -> Foot_LE
    {99, 100},  // Thigh_RI -> Calf_RI
    {100, 103}  // Calf_RI -> Foot_RI
};

BonePair CivilianSkeletonBones_1[] = {
    {50, 51}, // Neck -> Head
    {9, 50},  // Spine3 -> Neck
    {7, 9},   // Spine2 -> Spine3
    {1, 7},   // Spine1 -> Spine2
    {10, 11}, // Left clavicle -> UpperArm_LE
    {11, 12}, // UpperArm_LE -> LowerArm_LE
    {12, 15}, // LowerArm_LE -> Hand_LE
    {62, 63}, // Right clavicle -> UpperArm_RI
    {63, 64}, // UpperArm_RI -> LowerArm_RI
    {64, 67}, // LowerArm_RI -> Hand_RI
    {103, 104}, // Thigh_LE -> Calf_LE
    {104, 107}, // Calf_LE -> Foot_LE
    {115, 116}, // Thigh_RI -> Calf_RI
    {116, 119}  // Calf_RI -> Foot_RI
};

BonePair CivilianSkeletonBones_2[] = {
    {44, 45}, // neck_1 -> Head
    {5, 44}, // spine_3 -> neck_1
    {4, 5}, // spine_2 -> spine_3
    {3, 4}, // spine_1 -> spine_2
    {6, 7}, // clavicle_LE -> upperarm_LE
    {7, 8}, // upperarm_LE -> lowerarm_LE
    {8, 11}, // lowerarm_LE -> hand_LE
    {49, 50}, // clavicle_RI -> upperarm_RI
    {50, 51}, // upperarm_RI -> lowerarm_RI
    {51, 54}, // lowerarm_RI -> hand_RI
    {111, 112}, // thigh_LE -> calf_LE
    {112, 115}, // calf_LE -> foot_LE
    {99, 100}, // thigh_RI -> calf_RI
    {100, 103}, // calf_RI -> foot_RI
};

BonePair CivilianSkeletonBones[] = {
    {44, 45}, // neck_1 -> Head
    {5, 44}, // spine_3 -> neck_1
    {4, 5}, // spine_2 -> spine_3
    {3, 4}, // spine_1 -> spine_2
    {6, 7}, // clavicle_LE -> upperarm_LE
    {7, 8}, // upperarm_LE -> lowerarm_LE
    {8, 11}, // lowerarm_LE -> hand_LE
    {49, 50}, // clavicle_RI -> upperarm_RI
    {50, 51}, // upperarm_RI -> lowerarm_RI
    {51, 54}, // lowerarm_RI -> hand_RI
    {111, 112}, // thigh_LE -> calf_LE
    {112, 115}, // calf_LE -> foot_LE
    {99, 100}, // thigh_RI -> calf_RI
    {100, 103}, // calf_RI -> foot_RI
};

bool IsSuspect = false;
bool IsSwat = false;
bool IsPlayer = false;
int32 ViewportX = 0.0f;
int32 ViewportY = 0.0f;

auto RenderColor = IM_COL32(255, 255, 255, 255);

void Cheats::RenderESP()
{
	if (!CVars.ESP) return;
    if (!GVars.PlayerController || !GVars.Level) return;

    ULevel* Level = GVars.Level;
    if (!Level) return; 

	if (ESPSettings.ShowObjectives && GVars.GameState)
	{
		AReadyOrNotGameState* GameState = GVars.GameState;
		if (!GameState) return;

        TArray<AReportableActor*> AllReportableActors = GameState->AllReportableActors;
        for (AReportableActor* ReportableActor : AllReportableActors)
		{
	        if (!ReportableActor || !Utils::IsValidActor(ReportableActor)) continue;

            FVector2D ObjectiveScreen;
            
            if (GVars.PlayerController->ProjectWorldLocationToScreen(ReportableActor->K2_GetActorLocation(), &ObjectiveScreen, true))
            {
                ImU32 ObjectiveColor = ReportableActor->bReportableEnabled ? Colors::Gray : Colors::Green;
		        ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ObjectiveScreen.X, ObjectiveScreen.Y), 3, ObjectiveColor);
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(ObjectiveScreen.X + 5, ObjectiveScreen.Y - 5), ObjectiveColor, ReportableActor->ReportableName.ToString().c_str());
            }
	    }
	}

	TArray<AActor*> ActorsCopy = Level->Actors; // snapshot to prevent mid-iteration changes causing crashes
    for (AActor* Actor : ActorsCopy)
    {
    	if (!Actor) continue;

        if (ESPSettings.ShowTraps)
        {
	        FVector2D TrapScreen;

        	if (Actor->IsA(ATrapActor::StaticClass()) && GVars.PlayerController->ProjectWorldLocationToScreen(Actor->K2_GetActorLocation(), &TrapScreen, true))
        	{
				const char* TrapTypeName = (const char*)u8"未知陷阱";
				if (((ATrapActor*)Actor)->TrapType == ETrapType::Explosive)
					TrapTypeName = (const char*)u8"爆炸陷阱";
				else if (((ATrapActor*)Actor)->TrapType == ETrapType::Flashbang)
                    TrapTypeName = (const char*)u8"闪光弹陷阱";
				else if (((ATrapActor*)Actor)->TrapType == ETrapType::Alarm)
					TrapTypeName = (const char*)u8"报警器陷阱";

        		ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(TrapScreen.X, TrapScreen.Y), 3, Colors::Red);
        		ImGui::GetBackgroundDrawList()->AddText(ImVec2(TrapScreen.X + 5, TrapScreen.Y - 5), Colors::Red, TrapTypeName);
        		continue;
        	}
        }

        if (true) // Change to CVars.ShowGunESP
        {

        }

        AReadyOrNotCharacter* TargetActor = nullptr;

        if (Actor->IsA(ASuspectCharacter::StaticClass()) || Actor->IsA(ACivilianCharacter::StaticClass()))
        {
			TargetActor = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
			IsSwat = false;
            IsPlayer = false;
        }
        else if (Actor->IsA(APlayerCharacter::StaticClass()))
        {
            TargetActor = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
            IsSuspect = false;
            IsSwat = true;
            IsPlayer = true;
        }
		else if (ESPSettings.ShowTeam && Actor->IsA(ASWATCharacter::StaticClass())) 
		{
			TargetActor = reinterpret_cast<AReadyOrNotCharacter*>(Actor);
            IsSuspect = false;
			IsSwat = true;
            IsPlayer = false;
		}
    	else
			continue;

        if (!TargetActor) continue;

        if (GVars.ReadyOrNotChar && GVars.ReadyOrNotChar == TargetActor) continue;

        if (!IsSwat && TargetActor->IsSuspect())
            IsSuspect = true;
        else
			IsSuspect = false;

        if (IsSuspect) RenderColor = Utils::ConvertImVec4toU32(ESPSettings.SuspectColor);
		else RenderColor = Utils::ConvertImVec4toU32(ESPSettings.CivilianColor);

		if (IsSwat) RenderColor = Utils::ConvertImVec4toU32(ESPSettings.TeamColor);

        if (TargetActor->IsDeadOrUnconscious() || TargetActor->IsIncapacitated()) RenderColor = Utils::ConvertImVec4toU32(ESPSettings.DeadColor);
        else if (!IsSwat && TargetActor->IsArrestedOrSurrendered() || !IsSwat && TargetActor->bIsBeingArrested) RenderColor = Utils::ConvertImVec4toU32(ESPSettings.ArrestColor);

        USkeletalMeshComponent* Mesh = TargetActor->Mesh;
        if (!Mesh) continue;

        if (!IsSuspect && Mesh->GetBoneName(45).ToString() == "Head")
        {
            memcpy(CivilianSkeletonBones, CivilianSkeletonBones_2, sizeof(CivilianSkeletonBones_2));
        }
        else
            memcpy(CivilianSkeletonBones, CivilianSkeletonBones_1, sizeof(CivilianSkeletonBones_1));

        if (IsSuspect && Mesh->GetBoneName(45).ToString() == "Head")
        {
            memcpy(SuspectSkeletonBones, SuspectSkeletonBones_2, sizeof(SuspectSkeletonBones_2));
        }
        else
            memcpy(SuspectSkeletonBones, SuspectSkeletonBones_1, sizeof(SuspectSkeletonBones_1));

        std::vector<FVector2D> BonePositions = {};

        for (auto& pair : IsSuspect ? SuspectSkeletonBones : CivilianSkeletonBones)
        {
            FName ParentName = Mesh->GetBoneName(pair.Parent);
			FName ChildName = Mesh->GetBoneName(pair.Child);

            FVector ParentPos = Mesh->GetBoneTransform(ParentName, ERelativeTransformSpace::RTS_World).Translation;
            FVector ChildPos = Mesh->GetBoneTransform(ChildName, ERelativeTransformSpace::RTS_World).Translation;
            FVector2D ParentScreen, ChildScreen, ActorScreen;

            if (ESPSettings.LOS)
            {
                FVector Start = GVars.POV->Location;
                FVector End = ParentPos;

                TArray<AActor*> IgnoreActors;
                IgnoreActors.Add(GVars.ReadyOrNotChar);

                FHitResult HitResult;
                bool bHit = UKismetSystemLibrary::LineTraceSingle(
                    GVars.World,
                    Start,
                    End,
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

                AActor* HitActor = nullptr;
                HitActor = HitResult.Component->GetOwner();

                bool bHasLOS = !HitResult.bBlockingHit || HitActor == TargetActor;
                if (!bHasLOS)
                    continue;
            }

            if (ESPSettings.Bones)
            {
                
                if (GVars.PlayerController->ProjectWorldLocationToScreen(ParentPos, &ParentScreen, true) &&
                    GVars.PlayerController->ProjectWorldLocationToScreen(ChildPos, &ChildScreen, true))
                {
                    if (ESPSettings.ShowBox)
                    {
                        BonePositions.push_back(ParentScreen);
                        BonePositions.push_back(ChildScreen);
                    }

                    GVars.PlayerController->GetViewportSize(&ViewportX, &ViewportY);
                    if (ParentScreen.X == 0.f && ParentScreen.Y == 0.f or ParentScreen.X > ViewportX or ParentScreen.Y > ViewportY) continue;
                    ImGui::GetBackgroundDrawList()->AddLine(
                        ImVec2(ParentScreen.X, ParentScreen.Y),
                        ImVec2(ChildScreen.X, ChildScreen.Y),
                        RenderColor,
                        1.5f
                    );
                }
            }

            if (ESPSettings.ShowEnemyDistance)
            {
                FVector ActorLocation = TargetActor->K2_GetActorLocation();
				float Distance = GVars.POV->Location.GetDistanceToInMeters(ActorLocation);
                if (Distance < 0.0f) continue;
                FVector2D DistanceScreen;
                if (GVars.PlayerController->ProjectWorldLocationToScreen(ActorLocation, &DistanceScreen, true))
                {
                    char DistanceText[32];
                    snprintf(DistanceText, sizeof(DistanceText), "%.1f m", Distance);
                    ImGui::GetBackgroundDrawList()->AddText(
                        ImVec2(DistanceScreen.X, DistanceScreen.Y + 35),
                        RenderColor,
                        DistanceText
                    );
                }
			}
            if (ESPSettings.ShowTeam && IsPlayer && TargetActor && TargetActor->PlayerState && TargetActor->PlayerState->GetPlayerName() && GVars.PlayerController->ProjectWorldLocationToScreen(Actor->K2_GetActorLocation(), &ActorScreen, true))
            {
                ImGui::GetBackgroundDrawList()->AddText(
                    ImVec2(ActorScreen.X, ActorScreen.Y + 50),
                    RenderColor,
                    TargetActor->PlayerState->GetPlayerName().ToString().c_str()
                );
            }
        }
        if (ESPSettings.ShowBox)
        {
			FVector2D TopLeft, BottomRight;

			for (FVector2D BonePos : BonePositions)
            {
				if (BonePos.X < TopLeft.X || TopLeft.X == 0)
                    TopLeft.X = BonePos.X;

				if (BonePos.Y < TopLeft.Y || TopLeft.Y == 0)
					TopLeft.Y = BonePos.Y;

                if (BonePos.X > BottomRight.X)
					BottomRight.X = BonePos.X;

				if (BonePos.Y > BottomRight.Y)
					BottomRight.Y = BonePos.Y;
            }
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(TopLeft.X, TopLeft.Y),
                ImVec2(BottomRight.X, BottomRight.Y),
                RenderColor,
                0.0f,
                0,
                1.5f
            );
        }
    }
}