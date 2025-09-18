#include "Utils.h"
#include <Windows.h>
#include "SDK/Engine_classes.hpp"

using namespace SDK;

// Helper to get UWorld
UWorld* Utils::GetWorldSafe()
{
GetWorldStart:
    UEngine* Engine = UEngine::GetEngine();
    if (!Engine) {
        printf("[Error] Engine not found!\n");
        Sleep(400);
        goto GetWorldStart; // Retry getting the world
    }

    UGameViewportClient* Viewport = Engine->GameViewport;
    if (!Viewport) {
        printf("[Error] GameViewport not found!\n");
        Sleep(400);
        goto GetWorldStart; // Retry getting the world
    }

    UWorld* World = Viewport->World;
    if (!World) {
        printf("[Error] World not found!\n");
        Sleep(400);
        goto GetWorldStart; // Retry getting the world
    }

    return World;
}

APlayerController* Utils::GetPlayerController()
{
    while (true) {
        UWorld* World = Utils::GetWorldSafe();
        if (!World) return nullptr; // Error already logged in GetWorldSafe
        UGameInstance* GameInstance = World->OwningGameInstance;
        if (!GameInstance) {
            printf("[Error] GameInstance not found!\n");
            Sleep(400);
        }
        if (GameInstance->LocalPlayers.Num() <= 0) {
            printf("[Error] No LocalPlayers in GameInstance!\n");
        }
        ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
        if (!LocalPlayer) {
            printf("[Error] LocalPlayer is null!\n");
            Sleep(400);
        }
        APlayerController* PlayerController = LocalPlayer->PlayerController;
        if (!PlayerController) {
            printf("[Error] PlayerController not found!\n");
            Sleep(400);
        }
        return PlayerController;
    }
}

Variables* Utils::GetVariables()
{
    static Variables Vars;
    Variables::AutoSetVariables(Vars);
    return &Vars;
}