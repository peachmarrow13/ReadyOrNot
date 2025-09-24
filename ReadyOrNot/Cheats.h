#pragma once

#include "Utils.h"

class Cheats
{
public:
	static void ToggleGodMode();
	static void ToggleInfAmmo(bool IsEnabled);
	static void ToggleAimbot();
	static void Aimbot(Variables* Vars);
	static void UpgradeWeaponStats();
};