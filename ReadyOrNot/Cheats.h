#pragma once

#include <imgui.h>

#include "Utils.h"

struct EspSettings {
	bool ShowTeam = true;
	bool ShowBox = false;
	ImVec4 SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 DeadColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 TeamColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

} inline ESPSettings;

struct AimbotSettings {
	float MaxFOV = 15.0f;
	bool LOS = true;
	bool TargetCivilians = false;
	bool TargetDead = false;
	bool TargetArrested = false;
	float MinDistance = 50.0f;
} inline AimbotSettings;

class Cheats
{
public:
	static void ToggleGodMode();
	static void ToggleInfAmmo();
	static void ToggleAimbot();
	static void Aimbot(Variables* Vars);
	static void UpgradeWeaponStats();
	static void ToggleESP();
	static void RenderESP(Variables* Vars);
};