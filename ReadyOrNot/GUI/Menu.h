#pragma once
#include "pch.h"
#include "Cheats.h"

namespace GUI
{
    extern bool ShowMenu;
    extern ImGuiKey TriggerBotKey;
    extern ImGuiKey ESPKey;
    extern ImGuiKey AimButton;

    void RenderMenu();
    void AddDefaultTooltip(const char* desc);
    void HostOnlyTooltip();
}


