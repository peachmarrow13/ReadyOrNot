#pragma once

#include "Utils.h"

class Cheats
{
public:
	static void ToggleGodMode();
	static void ToggleInfAmmo();
	static void ToggleAimbot();
	static void ChangeAimbotSettings(float MaxFOV, bool LOS);
	static void Aimbot(Variables* Vars);
	static void UpgradeWeaponStats();
	static void ToggleESP();
	static void RenderESP(Variables* Vars);
};