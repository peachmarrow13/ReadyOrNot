#pragma once

#include "Engine.h"
#include "Utils.h"

inline bool AimbotKeyDown = false;

struct BoneListStruct
{
	std::string HeadBone = "Head";
	std::string NeckBone = "neck_1";
	std::string ChestBone = "spine_2";
	std::string StomachBone = "spine_1";
	std::string PelvisBone = "pelvis";
	std::string LeftShoulderBone = "clavicle_LE";
	std::string LeftElbowBone = "lowerarm_LE";
	std::string LeftHandBone = "hand_LE";
	std::string RightShoulderBone = "clavicle_RI";
	std::string RightElbowBone = "lowerarm_RI";
	std::string RightHandBone = "hand_RI";
} inline BoneList;

struct EspSettingsstruct {
	bool ShowTeam = true;
	bool ShowBox = false;
	bool ShowTraps = true;
	bool ShowObjectives = false;
	bool ShowEnemyDistance = false;
	bool Bones = true;
	float BoneOpacity = 1.0f;
	ImVec4 SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, BoneOpacity);
	ImVec4 CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, BoneOpacity);
	ImVec4 DeadColor = ImVec4(0.0f, 0.0f, 0.0f, BoneOpacity);
	ImVec4 TeamColor = ImVec4(0.0f, 1.0f, 0.0f, BoneOpacity);
	ImVec4 ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, BoneOpacity);
	bool LOS = false;

} inline ESPSettings;

struct AimbotSettingsstruct {
	float MaxFOV = 15.0f;
	float MaxDistance = 100.0f;
	bool LOS = true;
	bool TargetCivilians = false;
	bool TargetDead = false;
	bool TargetArrested = false;
	bool TargetSurrendered = false;
	float MinDistance = 2.0f;
	bool Smooth = false;
	float SmoothingVector = 5.0f;
	bool DrawArrow = false;
	bool DrawFOV = false;
	bool RequireKeyHeld = true;
	ImGuiKey AimbotKey = ImGuiKey_MouseRight;
	float FOVThickness = 1.0f;
	float ArrowThickness = 2.0f;
	bool TargetAll = false;
} inline AimbotSettings;

struct SilentAimSettingsstruct {
	float HitChance = 100.0f;
	float MaxFOV = 15.0f;
	bool TargetCivilians = false;
	bool TargetDead = false;
	bool TargetSurrendered = false;
	bool TargetArrested = false;
	bool RequiresLOS = false;
	bool DrawFOV = false;
	bool DrawArrow = false;
	float ArrowThickness = 2.0f;
	float FOVThickness = 1.0f;
	bool TargetAll = false;
} inline SilentAimSettings;

struct CVarsstruct
{
	bool Debug = false;
	bool SecretFeatures = false;
	bool GodMode = false;
	bool InfAmmo = false;
	bool Aimbot = false;
	bool ESP = false;
	float Speed = 1;
	bool SpeedEnabled = false;
	bool SilentAim = false;
	bool NoClip = false;
	bool Reticle = false;
	bool TriggerBot = false;
	bool RenderOptions = false;
	float FOV = 120.0f;
	bool ListPlayers = false;
	bool ShootFromReticle = false;
	bool SaveDebugToFile = false;
	bool BulletTime = false;
} inline CVars;

struct MiscSettingsStruct {
	bool Reticle = false;
	ImVec4 ReticleColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	float ReticleSize = 5.0f;
	ImVec2 ReticlePosition = ImVec2(0.0f, 0.0f);
	bool ReticleWhenThrowing = false;
	bool TriggerBotTargetsCivilians = false;
	bool TriggerBotUsesSilentAim = false;
	bool CrossReticle = true;
	bool ShouldAutoSave = true;
	bool ShouldSaveCVars = true;
} inline MiscSettings;

struct Settingsstruct
{
	bool ShouldSave = true;
	bool ShouldLoad = true;
} inline Settings;

struct TextVarsstruct
{
	std::string SilentAimBone = BoneList.HeadBone;
	std::string AimbotBone = BoneList.HeadBone;
	std::string DebugFunctionNameMustInclude = "";
	std::string DebugFunctionObjectMustInclude = "";
} inline TextVars;

struct Cheats
{
	static void ToggleGodMode();
	static void ToggleInfAmmo();
	static void Aimbot();
	static void AddAutoFire();
	static void RemoveRecoil();
	static void RemoveSpread();
	static void SetFireRate(float FireRate);
	static void PenetrateWalls();
	static void InstaKill();
	static void RenderESP();
	static void SetPlayerSpeed();
	static void SilentAim(Params::BaseMagazineWeapon_OnFire* FireParams);
	static void AddMag();
	static void ArrestAll(ETeam Team); // Arrest all of a specific team
	static void KillAll(ETeam Team);   // Kill all of a specific team
	static void DrawReticle();
	static void GetAllEvidence();
	static void TriggerBot();
	static void RenderEnabledOptions();
	static void ChangeFOV();
	static void Lean();
	static void UnlockDoors();
	static void AutoWin();
	static void ListPlayers();
	static void ChangeGameRenderSettings();
	static void NoClipToggle();
	static void GoTo(FVector Location);
	static void SurrenderAll(ETeam Team); // Surrender all of a specific team
};