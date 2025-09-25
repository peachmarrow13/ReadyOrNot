#include "Cheats.h"
#include "Utils.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include <chrono>

static bool ESPEnabled = false;
TArray<AActor*> ActorstoDraw;
static float LastActorUpdateTime = 0.f;
const float ActorUpdateInterval = 1.f; // update every 1 second
static float CurrentTime = 0.0f;
static std::chrono::high_resolution_clock::time_point LastFrameTime = std::chrono::high_resolution_clock::now();
bool IsSuspect = false;
int32 ViewportX = 0.0f;
int32 ViewportY = 0.0f;

auto SuspectColor = IM_COL32(255, 0, 0, 255);
auto DeadColor = IM_COL32(0, 0, 0, 255);
auto CivilianColor = IM_COL32(0, 0, 255, 255);
auto TeamColor = IM_COL32(0, 255, 0, 255);
auto ArrestColor = IM_COL32(255, 255, 0, 255);

auto RenderColor = IM_COL32(255, 255, 255, 255);

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

void Cheats::ToggleESP() {
	ESPEnabled = !ESPEnabled;
}

void Cheats::RenderESP(Variables* Vars)
{
    if (!ESPEnabled) return;
    if (!Vars || !Vars->PlayerController || !Vars->Character) return;

    ULevel* Level = Vars->Level;
    if (!Level) return;

    for (AActor* Actor : Level->Actors)
    {
        if (!Actor) continue;

        AReadyOrNotCharacter* TargetActor = nullptr;

        if (Actor->IsA(ASuspectCharacter::StaticClass()) or Actor->IsA(ACivilianCharacter::StaticClass()))
        {
			TargetActor = (AReadyOrNotCharacter*)Actor;
        }
        else
            continue;

        if (!TargetActor || TargetActor == Vars->Character) continue;

        if (TargetActor->IsSuspect())
            IsSuspect = true;
        else
			IsSuspect = false;

        if (IsSuspect) RenderColor = SuspectColor;
		else RenderColor = CivilianColor;

        if (TargetActor->IsDeadOrUnconscious()) RenderColor = DeadColor;
        if (TargetActor->IsArrested()) RenderColor = ArrestColor;

        USkeletalMeshComponent* Mesh = TargetActor->Mesh;
        if (!Mesh) continue;

        if (!IsSuspect and Mesh->GetBoneName(45).ToString() == "Head")
        {
            memcpy(CivilianSkeletonBones, CivilianSkeletonBones_2, sizeof(CivilianSkeletonBones_2));
        }
        else
            memcpy(CivilianSkeletonBones, CivilianSkeletonBones_1, sizeof(CivilianSkeletonBones_1));

        if (IsSuspect and Mesh->GetBoneName(45).ToString() == "Head")
        {
            memcpy(SuspectSkeletonBones, SuspectSkeletonBones_2, sizeof(SuspectSkeletonBones_2));
        }
        else
            memcpy(SuspectSkeletonBones, SuspectSkeletonBones_1, sizeof(SuspectSkeletonBones_1));

        for (auto& pair : IsSuspect ? SuspectSkeletonBones : CivilianSkeletonBones)
        {
            FName ParentName = Mesh->GetBoneName(pair.Parent);
			FName ChildName = Mesh->GetBoneName(pair.Child);

            FVector ParentPos = Mesh->GetBoneTransform(ParentName, ERelativeTransformSpace::RTS_World).Translation;
            FVector ChildPos = Mesh->GetBoneTransform(ChildName, ERelativeTransformSpace::RTS_World).Translation;

            FVector2D ParentScreen, ChildScreen;
            if (Vars->PlayerController->ProjectWorldLocationToScreen(ParentPos, &ParentScreen, false) &&
                Vars->PlayerController->ProjectWorldLocationToScreen(ChildPos, &ChildScreen, false))
            {
                Vars->PlayerController->GetViewportSize(&ViewportX, &ViewportY);
				if (ParentScreen.X == 0.f && ParentScreen.Y == 0.f or ParentScreen.X > ViewportX or ParentScreen.Y > ViewportY) continue;
                ImGui::GetBackgroundDrawList()->AddLine(
                    ImVec2(ParentScreen.X, ParentScreen.Y),
                    ImVec2(ChildScreen.X, ChildScreen.Y),
                    RenderColor,
                    1.5f
                );
           }
        }
    }
}