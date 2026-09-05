#include "pch.h"
#include "Engine.h"
#include <kiero/kiero.h>

#define MAJORVERSION 2
#define MINORVERSION 5
#define PATCHVERSION 2

static const std::pair<const char*, int> KeyNames[] = {
{"Lbutton", VK_LBUTTON},
{"Rbutton", VK_RBUTTON},
{"Cancel", VK_CANCEL},
{"Mbutton", VK_MBUTTON},
{"Xbutton1", VK_XBUTTON1},
{"Xbutton2", VK_XBUTTON2},
{"Back", VK_BACK},
{"Tab", VK_TAB},
{"Clear", VK_CLEAR},
{"Enter", VK_RETURN},
{"Shift", VK_SHIFT},
{"Ctrl", VK_CONTROL},
{"Alt", VK_MENU},
{"Pause", VK_PAUSE},
{"Capital", VK_CAPITAL},
{"Escape", VK_ESCAPE},
{"Accept", VK_ACCEPT},
{"Space", VK_SPACE},
{"Next", VK_NEXT},
{"End", VK_END},
{"Home", VK_HOME},
{"Left", VK_LEFT},
{"Up", VK_UP},
{"Right", VK_RIGHT},
{"Down", VK_DOWN},
{"Select", VK_SELECT},
{"Print", VK_PRINT},
{"Execute", VK_EXECUTE},
{"Snapshot", VK_SNAPSHOT},
{"Insert", VK_INSERT},
{"Delete", VK_DELETE},
{"Help", VK_HELP},
{"Lwin", VK_LWIN},
{"Rwin", VK_RWIN},
{"Apps", VK_APPS},
{"Sleep", VK_SLEEP},
{"Numpad0", VK_NUMPAD0},
{"Numpad1", VK_NUMPAD1},
{"Numpad2", VK_NUMPAD2},
{"Numpad3", VK_NUMPAD3},
{"Numpad4", VK_NUMPAD4},
{"Numpad5", VK_NUMPAD5},
{"Numpad6", VK_NUMPAD6},
{"Numpad7", VK_NUMPAD7},
{"Numpad8", VK_NUMPAD8},
{"Numpad9", VK_NUMPAD9},
{"Multiply", VK_MULTIPLY},
{"Add", VK_ADD},
{"Separator", VK_SEPARATOR},
{"Subtract", VK_SUBTRACT},
{"Decimal", VK_DECIMAL},
{"Divide", VK_DIVIDE},
{"F1", VK_F1},
{"F2", VK_F2},
{"F3", VK_F3},
{"F4", VK_F4},
{"F5", VK_F5},
{"F6", VK_F6},
{"F7", VK_F7},
{"F8", VK_F8},
{"F9", VK_F9},
{"F10", VK_F10},
{"F11", VK_F11},
{"F12", VK_F12},
{"Numlock", VK_NUMLOCK},
{"Scroll", VK_SCROLL},
{"Lshift", VK_LSHIFT},
{"Rshift", VK_RSHIFT},
{"Lcontrol", VK_LCONTROL},
{"Rcontrol", VK_RCONTROL},
{"Lmenu", VK_LMENU},
{"Rmenu", VK_RMENU},
{"Oem1", VK_OEM_1},
{"OemPlus", VK_OEM_PLUS},
{"OemComma", VK_OEM_COMMA},
{"OemMinus", VK_OEM_MINUS},
{"OemPeriod", VK_OEM_PERIOD},
{"Oem2", VK_OEM_2},
{"Oem3", VK_OEM_3},
{"GamepadA", VK_GAMEPAD_A},
{"GamepadB", VK_GAMEPAD_B},
{"GamepadX", VK_GAMEPAD_X},
{"GamepadY", VK_GAMEPAD_Y},
{"GamepadRightShoulder", VK_GAMEPAD_RIGHT_SHOULDER},
{"GamepadLeftShoulder", VK_GAMEPAD_LEFT_SHOULDER},
{"GamepadLeftTrigger", VK_GAMEPAD_LEFT_TRIGGER},
{"GamepadRightTrigger", VK_GAMEPAD_RIGHT_TRIGGER},
{"GamepadDpadUp", VK_GAMEPAD_DPAD_UP},
{"GamepadDpadDown", VK_GAMEPAD_DPAD_DOWN},
{"GamepadDpadLeft", VK_GAMEPAD_DPAD_LEFT},
{"GamepadDpadRight", VK_GAMEPAD_DPAD_RIGHT},
{"GamepadMenu", VK_GAMEPAD_MENU},
{"GamepadView", VK_GAMEPAD_VIEW},
{"GamepadLeftThumbstickButton", VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON},
{"GamepadRightThumbstickButton", VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON},
{"GamepadLeftThumbstickUp", VK_GAMEPAD_LEFT_THUMBSTICK_UP},
{"GamepadLeftThumbstickDown", VK_GAMEPAD_LEFT_THUMBSTICK_DOWN},
{"GamepadLeftThumbstickRight", VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT},
{"GamepadLeftThumbstickLeft", VK_GAMEPAD_LEFT_THUMBSTICK_LEFT},
{"GamepadRightThumbstickUp", VK_GAMEPAD_RIGHT_THUMBSTICK_UP},
{"GamepadRightThumbstickDown", VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN},
{"GamepadRightThumbstickRight", VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT},
{"GamepadRightThumbstickLeft", VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT},
{"OemClear", VK_OEM_CLEAR}
};

static int FindKeyIndex(int VirtualKey)
{
	for (int Index = 0; Index < IM_ARRAYSIZE(KeyNames); ++Index)
	{
		if (KeyNames[Index].second == VirtualKey)
			return Index;
	}

	return 0;
}

bool KeyGetter(void* Data, int Index, const char** OutText)
{
	if (!Data || !OutText || Index < 0 || Index >= IM_ARRAYSIZE(KeyNames))
		return false;

	auto* Items = static_cast<std::pair<const char*, int>*>(Data);
	*OutText = Items[Index].first;
	return true;
}

static const std::pair<const char*, std::string> BoneOptions[] = {
	{"Head", BoneList.HeadBone},
	{"Neck", BoneList.NeckBone},
	{"Chest", BoneList.ChestBone},
	{"Stomach", BoneList.StomachBone},
	{"Pelvis", BoneList.PelvisBone},
	{"Left Shoulder", BoneList.LeftShoulderBone},
	{"Left Elbow", BoneList.LeftElbowBone},
	{"Left Hand", BoneList.LeftHandBone},
	{"Right Shoulder", BoneList.RightShoulderBone},
	{"Right Elbow", BoneList.RightElbowBone},
	{"Right Hand", BoneList.RightHandBone}
};

static bool ShowMenu = true;
bool init = false;

int Frames = 0;

float FireRate = 1;

std::atomic<HMODULE> g_hModule{ nullptr };
std::atomic<int> g_PresentCount{ 0 };
std::atomic<bool> Cleaning{ false };
std::atomic<bool> Resizing{ false };

static void Cleanup(HMODULE hModule);
void SaveSettings();
void LoadSettings();

static ImGuiKey TriggerBotKey = ImGuiKey_None;
static ImGuiKey ESPKey = ImGuiKey_None;
static ImGuiKey AimButton = ImGuiKey_None;

static std::string PlayerName;

// Add WndProc hook for input handling
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc = nullptr;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!Cleaning.load())
	{
		if (ImGui::GetCurrentContext()) {
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		}

		if (uMsg == WM_KEYUP) {
			if (wParam == MiscSettings.MenuButton) {
				ShowMenu = !ShowMenu;
				std::cout << "Menu: " << (ShowMenu ? "ON" : "OFF") << "\n";
				ImGui::GetIO().MouseDrawCursor = ShowMenu;
				ShowCursor(ShowMenu);
				return TRUE;
			}

			if (wParam == MiscSettings.UninjectButton) {
				Cleaning.store(true);
				return TRUE;
			}
		}

		if (uMsg == WM_KEYUP) {

		}

		if (uMsg == WM_SETCURSOR) {
			if (!ShowMenu) {   // only block cursor when menu is hidden
				SetCursor(NULL);
				return TRUE;    // prevent Windows from drawing it
			}
		}
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall Engine::hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	Resizing.store(true);
	while (g_PresentCount.load() != 0)
		Sleep(0);

	// Call original function
	HRESULT hr = Engine::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	
	Engine::pContext->OMSetRenderTargets(0, nullptr, nullptr);
	if (Engine::pRenderTargetView) {
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
		Engine::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &Engine::pRenderTargetView);
		pBackBuffer->Release();
	}
	
	Resizing.store(false);

	return hr;
}

HRESULT __stdcall Engine::hkPresent(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags)
{
	if (Cleaning.load())
		return Engine::oPresent(SwapChain, SyncInterval, Flags);
	
	if (Resizing.load())
		return Engine::oPresent(SwapChain, SyncInterval, Flags);

	struct PresentGuardStruct
	{
		PresentGuardStruct() { g_PresentCount.fetch_add(1); }
		~PresentGuardStruct() { g_PresentCount.fetch_sub(1); }
	} PresentGuard;

	if (MiscSettings.ShouldAutoSave && Frames % 900 == 0) // Every 900 frames, save settings
	{
		SaveSettings();
	}

	if (!init)
	{
		if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Engine::pDevice)))
		{
			printf("[hkPresent] Device acquired successfully\n");
			Engine::pDevice->GetImmediateContext(&Engine::pContext);

			Engine::pSwapChain = SwapChain;
			SwapChain->GetDesc(&Engine::sd);

			HWND hwnd = Engine::GetGameWindow();
			if (!hwnd)
			{
				printf("Failed to get GameWindow\n");
				return Engine::oPresent(SwapChain, SyncInterval, Flags);
			}

			if (!Engine::InitImGui())
			{
				printf("[hkPresent] Failed to initialize ImGui\n");
				return Engine::oPresent(SwapChain, SyncInterval, Flags);
			}
			printf("[hkPresent] ImGui initialized successfully\n");
			
			if (hwnd) 
				oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			
			init = true;
		}
		else
			printf("[Warning] Failed to acquire device\n");

		if (GVars.PlayerController && GVars.PlayerController->PlayerState)
		{
			APlayerState* PlayerState = GVars.PlayerController->PlayerState;
			if (PlayerName.empty())
				PlayerName = PlayerState->GetPlayerName().ToString();
		}
	}

	if (!Engine::oPresent)
	{
		printf("oPresent is dead.\n");
		return 0;
	}

	if (!ImGui::GetCurrentContext())
	{
		printf("[ERROR] ImGui context not found!\n");
		return Engine::oPresent(SwapChain, SyncInterval, Flags);
	}

	if (GVars.ScreenSize.x != ImGui::GetIO().DisplaySize.x || GVars.ScreenSize.y != ImGui::GetIO().DisplaySize.y)
	{
		GVars.ScreenSize = ImGui::GetIO().DisplaySize;
	}

	// Start the ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ShowMenu) {
		ImGui::Begin("Free Ready or Not Cheat by PeachMarrow12", nullptr, ImGuiWindowFlags_NoCollapse);

		if  (ImGui::BeginTabBar("MainTabBar"))
		{
			if (ImGui::BeginTabItem("About"))
			{
				ImGui::Text("Free Ready or Not Cheat by PeachMarrow12");
				ImGui::Text("Version %d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);

				if (GVars.PlayerController)
				{
					ImGui::Text("Thank you for using my cheat %s!", PlayerName.c_str());
				}
				else
				{
					ImGui::Text("Username not found but thanks anyways!");
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Player"))
			{
				if (ImGui::Checkbox("GodMode", &CVars.GodMode))
					CVars.QueuedAction = EQueuedAction::ToggleGodMode;
				HostOnlyTooltip();

				ImGui::Checkbox("Aimbot", &CVars.Aimbot);

				ImGui::Checkbox("Silent Aim", &CVars.SilentAim);
				AddDefaultTooltip("Not fully fleshed out so use with caution.");
				HostOnlyTooltip();

				ImGui::Checkbox("ESP", &CVars.ESP);

				ImGui::SliderFloat("Player Speed", &CVars.Speed, 1.0f, 30.0f, "%.1f");
				ImGui::SameLine();
				ImGui::Checkbox("Enable Speed", &CVars.SpeedEnabled);

				if (ImGui::SliderFloat("FOV", &CVars.FOV, 10.0f, 179.9f))
				{
					Cheats::ChangeFOV();
				}

				if (ImGui::Button("Give all Achievements"))
					CVars.QueuedAction = EQueuedAction::GiveAchievements;

				ImGui::Checkbox("Instant MultiTool", &CVars.InstantMultiTool);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Weapon"))
			{
				if (ImGui::Checkbox("Infinite Ammo", &CVars.InfAmmo))
					CVars.QueuedAction = EQueuedAction::ToggleInfAmmo;
				HostOnlyTooltip();

				if (ImGui::InputInt("Multi Fire", &CVars.MultiFire, 0, 50))
				{
					if (CVars.MultiFire < 0) CVars.MultiFire = 0;
					if (CVars.MultiFire > 20) CVars.MultiFire = 20;
				}

				if (ImGui::Button("Remove Recoil"))
					CVars.QueuedAction = EQueuedAction::RemoveRecoil;

				ImGui::Checkbox("Anti Sway", &CVars.AntiSway);

				if (ImGui::Button("Remove Spread"))
					CVars.QueuedAction = EQueuedAction::RemoveSpread;

				if (ImGui::Button("Add Auto Fire"))
					CVars.QueuedAction = EQueuedAction::AddAutoFire;

				if (ImGui::Button("Add Penetration"))
					CVars.QueuedAction = EQueuedAction::AddPenetration;

				if (ImGui::Button("Insta Kill"))
					CVars.QueuedAction = EQueuedAction::InstaKill;

				if (ImGui::Button("Increase Fire Rate"))
					CVars.QueuedAction = EQueuedAction::SetFireRate;
				
				ImGui::Checkbox("Shoot From Reticle", &CVars.ShootFromReticle);

				if (ImGui::Button("Add Magazine"))
					CVars.QueuedAction = EQueuedAction::AddMagazine;

				ImGui::Checkbox("TriggerBot", &CVars.TriggerBot);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("World"))
			{
				if (ImGui::Button("Kill All Suspects"))
					CVars.QueuedAction = EQueuedAction::KillAllSuspects;

				ImGui::SameLine();

				if (ImGui::Button("Surrender All Suspects"))
					Cheats::SurrenderAll(ETeam::TEAM_SUSPECT);
				HostOnlyTooltip();

				ImGui::SameLine();

				if (ImGui::Button("Arrest All Suspects"))
					Cheats::ArrestAll(ETeam::TEAM_SUSPECT);
				HostOnlyTooltip();

				if (ImGui::Button("Kill All Civilians"))
					Cheats::KillAll(ETeam::TEAM_CIVILIAN);

				ImGui::SameLine();

				if (ImGui::Button("Surrender All Civilians"))
					Cheats::SurrenderAll(ETeam::TEAM_CIVILIAN);
				HostOnlyTooltip();

				ImGui::SameLine();

				if (ImGui::Button("Arrest All Civilians"))
					Cheats::ArrestAll(ETeam::TEAM_CIVILIAN);
				HostOnlyTooltip();

				if (ImGui::Button("Collect All Evidence"))
					Cheats::GetAllEvidence();

				if (ImGui::Button("AutoWin"))
					Cheats::AutoWin();
				HostOnlyTooltip();

				if (ImGui::Button("Unlock All Doors"))
					Cheats::UnlockDoors();
				HostOnlyTooltip();

				ImGui::Checkbox("Bullet Time", &CVars.BulletTime);
				HostOnlyTooltip();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				if (ImGui::Button("Save Settings"))
					SaveSettings();
				ImGui::SameLine();

				if (ImGui::Button("Load Settings"))
					LoadSettings();
				AddDefaultTooltip("These only save and load the configs not which cheats are enabled.");

				ImGui::Checkbox("Debug", &CVars.Debug);
				AddDefaultTooltip("This just enables options in the menu I use for finding bugs and useful information. This is most likely useless to you.");

				ImGui::InputText("Debug Func Name Must Include", &TextVars.DebugFunctionNameMustInclude);
				ImGui::InputText("Debug Func Obj Name Must Include", &TextVars.DebugFunctionObjectMustInclude);

				if (CVars.Debug)
					if (ImGui::Button("Print Actors"))
						Utils::PrintActors(nullptr);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Configuration"))
			{
				if (ImGui::TreeNode("Aimbot Settings"))
				{
					ImGui::SliderFloat("Aimbot FOV", &AimbotSettings.MaxFOV, 0.01f, 180.0f, "%.1f");

					ImGui::Checkbox("Should Aimbot require LOS", &AimbotSettings.LOS);
					AddDefaultTooltip("Targets must be visible; line - of - sight required.");

					ImGui::Checkbox("Target Civilians", &AimbotSettings.TargetCivilians);

					ImGui::Checkbox("Target Dead", &AimbotSettings.TargetDead);

					ImGui::Checkbox("Target Arrested", &AimbotSettings.TargetArrested);

					ImGui::Checkbox("Target Surrendered", &AimbotSettings.TargetSurrendered);

					ImGui::Checkbox("Target All", &AimbotSettings.TargetAll);

					ImGui::SliderFloat("Max Distance", &AimbotSettings.MaxDistance, 0.0f, 300.0f, "%.1f");

					ImGui::SliderFloat("Minimum Distance", &AimbotSettings.MinDistance, 0.0f, 100.0f, "%.1f");

					ImGui::Checkbox("Smoothing", &AimbotSettings.Smooth);

					ImGui::SliderFloat("Smoothing Vector", &AimbotSettings.SmoothingVector, 1.0f, 20.0f, "%.2f");

					ImGui::Checkbox("Prediction", &AimbotSettings.Prediction);
					ImGui::SliderFloat("Prediction Strength", &AimbotSettings.PredictionMultiplier, 0.0f, 2.0f, "%.2f");

					ImGui::Checkbox("Target Lock", &AimbotSettings.TargetLock);

					ImGui::Checkbox("Draw Arrow", &AimbotSettings.DrawArrow);

					ImGui::Checkbox("Draw FOV", &AimbotSettings.DrawFOV);

					if (ImGui::BeginCombo("Target Bone", TextVars.AimbotBone.c_str()))
					{
						for (int i = 0; i < IM_ARRAYSIZE(BoneOptions); i++)
						{
							bool is_selected = (TextVars.AimbotBone == BoneOptions[i].second);
							if (ImGui::Selectable(BoneOptions[i].first, is_selected))
							{
								TextVars.AimbotBone = BoneOptions[i].second;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus(); // make the selected item visible
						}
						ImGui::EndCombo();
					}

					ImGui::Checkbox("Require HotKey", &AimbotSettings.RequireKeyHeld);
					
					const char* ABpreview = ImGui::GetKeyName(AimbotSettings.AimbotKey);
					if (!ABpreview) ABpreview = "None";

					if (ImGui::BeginCombo("Select Key for Aimbot", ABpreview))
					{
						for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
						{
							ImGuiKey current = static_cast<ImGuiKey>(key);
							const char* keyName = ImGui::GetKeyName(current);
							if (!keyName || !*keyName) continue; // skip empty names

							bool isSelected = (AimbotSettings.AimbotKey == current);
							if (ImGui::Selectable(keyName, isSelected))
								AimbotSettings.AimbotKey = current;

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
						AddDefaultTooltip("Only activate the aimbot while this key is held");
					}
					
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("ESP Settings"))
				{
					ImGui::Checkbox("Show Team", &ESPSettings.ShowTeam);

					ImGui::Checkbox("Show Box", &ESPSettings.ShowBox);

					ImGui::Checkbox("Show Traps", &ESPSettings.ShowTraps);

					ImGui::Checkbox("Show Enemy Distance", &ESPSettings.ShowEnemyDistance);

					ImGui::Checkbox("Show Bones", &ESPSettings.Bones);

					ImGui::ColorEdit4("Suspect Color", (float*)&ESPSettings.SuspectColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4("Civilian Color", (float*)&ESPSettings.CivilianColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4("Dead Color", (float*)&ESPSettings.DeadColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4("Team Color", (float*)&ESPSettings.TeamColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4("Arrested Color", (float*)&ESPSettings.ArrestColor, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("LOS", &ESPSettings.LOS);

					if (ImGui::SliderFloat("Bone Opacity", &ESPSettings.BoneOpacity, 0.0f, 1.0f, "%.2f"))
					{
						ESPSettings.SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, ESPSettings.BoneOpacity);
						ESPSettings.DeadColor = ImVec4(0.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.TeamColor = ImVec4(0.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
					}

					ImGui::Checkbox("Show Objectives", &ESPSettings.ShowObjectives);
					AddDefaultTooltip("The Objectives don't show the actual location");
					ImGui::TreePop();
				}
		
				if (ImGui::TreeNode("Silent Aim Settings"))
				{
					ImGui::Checkbox("Target Civilians", &SilentAimSettings.TargetCivilians);

					ImGui::Checkbox("Target All", &SilentAimSettings.TargetAll);

					ImGui::Checkbox("Target Dead", &SilentAimSettings.TargetDead);

					ImGui::Checkbox("Target Surrendered", &SilentAimSettings.TargetSurrendered);

					ImGui::Checkbox("Target Arrested", &SilentAimSettings.TargetArrested);

					ImGui::SliderFloat("Silent Aim FOV", &SilentAimSettings.MaxFOV, 0, 180.0f, "%.1f");

					ImGui::Checkbox("Draw FOV", &SilentAimSettings.DrawFOV);

					ImGui::SliderFloat("FOV Line Thickness", &SilentAimSettings.FOVThickness, 0.1f, 10.0f, "%.2f");

					ImGui::Checkbox("Draw Snap line", &SilentAimSettings.DrawArrow);

					ImGui::SliderFloat("Snap Line Thickness", &SilentAimSettings.ArrowThickness, 0.1f, 10.0f, "%.2f");

					ImGui::Checkbox("Require LOS", &SilentAimSettings.RequiresLOS);

					ImGui::SliderFloat("Hit Chance", &SilentAimSettings.HitChance, 0.0f, 100, "%.1f");

					if (ImGui::BeginCombo("Target Bone", TextVars.SilentAimBone.c_str()))
					{
						for (int i = 0; i < IM_ARRAYSIZE(BoneOptions); i++)
						{
							bool is_selected = (TextVars.SilentAimBone == BoneOptions[i].second);
							if (ImGui::Selectable(BoneOptions[i].first, is_selected))
							{
								TextVars.SilentAimBone = BoneOptions[i].second;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus(); // make the selected item visible
						}
						ImGui::EndCombo();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Misc Settings"))
				{
					static int MenuButtonCurrentIndex = FindKeyIndex(MiscSettings.MenuButton);

					if (ImGui::Combo("Menu Toggle Key", &MenuButtonCurrentIndex, KeyGetter, (void*)KeyNames, IM_ARRAYSIZE(KeyNames)))
					{
						MiscSettings.MenuButton = KeyNames[MenuButtonCurrentIndex].second;
					}

					static int UninjectButtonCurrentIndex = FindKeyIndex(MiscSettings.UninjectButton);

					if (ImGui::Combo("Uninject Key", &UninjectButtonCurrentIndex, KeyGetter, (void*)KeyNames, IM_ARRAYSIZE(KeyNames)))
					{
						MiscSettings.UninjectButton = KeyNames[UninjectButtonCurrentIndex].second;
					}

					ImGui::InputFloat("Bullet Time Speed", &CVars.BulletTimeSpeed);

					ImGui::SeparatorText("Reticle Settings");

					ImGui::Checkbox("Reticle", &CVars.Reticle);

					ImGui::ColorEdit4("Reticle Color", (float*)&MiscSettings.ReticleColor);

					ImGui::DragFloat2("Reticle Position", (float*)&MiscSettings.ReticlePosition, 1, -100, 100);

					ImGui::SliderFloat("Reticle Size", &MiscSettings.ReticleSize, 1, 15);

					ImGui::Checkbox("Use a Cross Reticle", &MiscSettings.CrossReticle);

					ImGui::Checkbox("Only Show Reticle while Throwing a Grenade", &MiscSettings.ReticleWhenThrowing);

					ImGui::SeparatorText("TriggerBot Settings");

					ImGui::Checkbox("TriggerBot Shoots Civilians", &MiscSettings.TriggerBotTargetsCivilians);

					ImGui::Checkbox("TriggerBot Uses SilentAim", &MiscSettings.TriggerBotUsesSilentAim);
					AddDefaultTooltip("Guarantees you will hit the target");

					ImGui::SeparatorText("Other");

					ImGui::Checkbox("Show Enabled Options", &CVars.RenderOptions);

					ImGui::Checkbox("List Players", &CVars.ListPlayers);

					ImGui::Checkbox("Should Auto Save Settings", &MiscSettings.ShouldAutoSave);
					ImGui::SameLine();
					ImGui::Checkbox("Should Save Enabled Cheats", &MiscSettings.ShouldSaveCVars);

					ImGui::SeparatorText("KeyBinds");

					// Create a combo box
					const char* TBpreview = ImGui::GetKeyName(TriggerBotKey);
					if (!TBpreview) TBpreview = "None";

					if (ImGui::BeginCombo("Select Key for TriggerBot", TBpreview))
					{
						for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
						{
							ImGuiKey current = static_cast<ImGuiKey>(key);
							const char* keyName = ImGui::GetKeyName(current);
							if (!keyName || !*keyName) continue; // skip empty names

							bool isSelected = (TriggerBotKey == current);
							if (ImGui::Selectable(keyName, isSelected))
								TriggerBotKey = current;

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					const char* ESPpreview = ImGui::GetKeyName(ESPKey);
					if (!ESPpreview) ESPpreview = "None";

					if (ImGui::BeginCombo("Select Key for ESP", ESPpreview))
					{
						for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
						{
							ImGuiKey current = static_cast<ImGuiKey>(key);
							const char* keyName = ImGui::GetKeyName(current);
							if (!keyName || !*keyName) continue; // skip empty names

							bool isSelected = (ESPKey == current);
							if (ImGui::Selectable(keyName, isSelected))
								ESPKey = current;

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					const char* AimPreview = ImGui::GetKeyName(AimButton);
					if (!AimPreview) AimPreview = "None";

					if (ImGui::BeginCombo("Select AimLock Button", AimPreview))
					{
						for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
						{
							ImGuiKey current = static_cast<ImGuiKey>(key);
							const char* keyName = ImGui::GetKeyName(current);
							if (!keyName || !*keyName) continue; // skip empty names

							bool isSelected = (AimButton == current);
							if (ImGui::Selectable(keyName, isSelected))
								AimButton = current;

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					
					ImGui::TreePop();
				}


				ImGui::EndTabItem();
			}

			if (CVars.SecretFeatures)
			{
				if (ImGui::BeginTabItem("Secret Features"))
				{
					ImGui::Text("This is for development.");
					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}

		ImGui::Separator();
		ImGui::Text("You can find me on UnknownCheats.me as Peachmarrow13.");
		ImGui::SameLine();
		ImGui::Text("This cheat was released on UnknownCheats.me for free, do not pay for this.");
		ImGui::End();
	}

	if (AimbotSettings.AimbotKey != ImGuiKey_None)
	{
		AimbotKeyDown = ImGui::IsKeyDown(AimbotSettings.AimbotKey);
	}
	else
		AimbotKeyDown = true;

	if (CVars.RenderOptions)
		Cheats::RenderEnabledOptions();

	if (CVars.ListPlayers)
		Cheats::ListPlayers();

	if (CVars.ESP)
		Cheats::RenderESP();

	if (CVars.SilentAim)
	{
		if (SilentAimSettings.DrawFOV)
			Utils::DrawFOV(SilentAimSettings.MaxFOV, SilentAimSettings.FOVThickness);

		AActor* TargetActor =
			Utils::GetBestTarget(
				GVars.PlayerController,
				SilentAimSettings.TargetCivilians,
				SilentAimSettings.TargetArrested,
				SilentAimSettings.TargetSurrendered,
				SilentAimSettings.TargetDead,
				SilentAimSettings.MaxFOV,
				SilentAimSettings.RequiresLOS,
				TextVars.SilentAimBone,
				SilentAimSettings.TargetAll);

		if (TargetActor)
		{
			auto* RONC = GVars.ReadyOrNotChar;

			std::wstring WideString = UtfN::StringToWString(TextVars.SilentAimBone);
			FName BoneName = UKismetStringLibrary::Conv_StringToName(WideString.c_str());

			FVector TargetLocation = ((AReadyOrNotCharacter*)TargetActor)->Mesh->GetBoneTransform(BoneName, ERelativeTransformSpace::RTS_World).Translation;

			if (SilentAimSettings.DrawArrow)
				Utils::DrawSnapLine(TargetLocation, SilentAimSettings.ArrowThickness);
		}
	}

	if (CVars.Reticle)
		Cheats::DrawReticle();

	if (CVars.Aimbot)
		Cheats::Aimbot();

	if (Engine::pRenderTargetView) {
		Engine::pContext->OMSetRenderTargets(1, &Engine::pRenderTargetView, nullptr);

		D3D11_VIEWPORT vp = {};
		vp.Width = (float)Engine::sd.BufferDesc.Width;
		vp.Height = (float)Engine::sd.BufferDesc.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		Engine::pContext->RSSetViewports(1, &vp);
	}

	ImGui::Render();
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (TriggerBotKey != ImGuiKey_None && ImGui::IsKeyPressed(TriggerBotKey, false))
	{
		CVars.TriggerBot = !CVars.TriggerBot;
	}

	if (ESPKey != ImGuiKey_None && ImGui::IsKeyPressed(ESPKey, false))
	{
		CVars.ESP = !CVars.ESP;
	}

	return Engine::oPresent ? Engine::oPresent(SwapChain, SyncInterval, Flags) : S_OK;
}

static DWORD WINAPI MainThread(LPVOID Parameter)
{
	HMODULE hModule = static_cast<HMODULE>(Parameter);
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);

	printf("Cheat Injecting...\n");

	g_hModule.store(hModule);

	int Attempts = 0;

	while (!UEngine::GetEngine() && Attempts < 100)
	{
		Attempts++;
		printf("Waiting for game to load...\n");
		Sleep(100);
	}

	MH_STATUS Status = MH_Initialize();
	if (Status != MH_OK)
	{
		printf("[ERROR] MinHook failed to init: %d", Status);
		Cleaning.store(true);
		Cleanup(hModule);
	}

	Sleep(1000); // Wait a second to ensure everything is loaded	

	if (Engine::HookPresent() != kiero::Status::Success)
	{
		printf("[ERROR] Failed to initialize engine hooks.\n");
		Cleaning.store(true);
		Cleanup(hModule);
	}
	else
		printf("Engine hooks initialized successfully.\n");

	printf("Cheat Injected\n");

	LoadSettings();

	if (!Hooks::HookProcessEvent())
	{
		printf("[ERROR] Failed to initialize ProcessEvent hook.\n");
		Cleaning.store(true);
		Cleanup(hModule);
		return 1;
	}

	while (!Cleaning.load())
		Sleep(100);

	Cleanup(hModule);

	return 0;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		if (HANDLE MainThreadHandle = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr))
			CloseHandle(MainThreadHandle);
		break;
	case DLL_PROCESS_DETACH:
		Cleaning.store(true);
		break;
	}
	
	return TRUE;
}

void SaveSettings()
{
	return;
	// Save MiscSettings (binary)
	std::ofstream MiscSettingsfile("MiscSettings.bin", std::ios::binary);
	if (MiscSettingsfile.is_open())
	{
		MiscSettingsfile.write(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));
		MiscSettingsfile.close();
	}
	else
		printf("[ERROR] Could not open MiscSettings.bin for writing!\n");

	// Save AimbotSettings (binary)
	std::ofstream AimbotSettingsfile("AimbotSettings.bin", std::ios::binary);
	if (AimbotSettingsfile.is_open())
	{
		AimbotSettingsfile.write(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));
		AimbotSettingsfile.close();
	}
	else
		printf("[ERROR] Could not open AimbotSettings.bin for writing!\n");

	// Save ESPSettings (binary)
	std::ofstream ESPSettingsfile("ESPSettings.bin", std::ios::binary);
	if (ESPSettingsfile.is_open())
	{
		ESPSettingsfile.write(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));
		ESPSettingsfile.close();
	}
	else
		printf("[ERROR] Could not open ESPSettings.bin for writing!\n");

	// Save SilentAimSettings (binary)
	std::ofstream SilentAimSettingsfile("SilentAimSettings.bin", std::ios::binary);
	if (SilentAimSettingsfile.is_open())
	{
		SilentAimSettingsfile.write(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));
		SilentAimSettingsfile.close();
	}
	else
		printf("[ERROR] Could not open SilentAimSettings.bin for writing!\n");

	// Save TextVars strings
	std::ofstream TextVarsfile("TextVars.bin", std::ios::binary);
	if (TextVarsfile.is_open())
	{
		// Save AimbotBone
		size_t len = TextVars.AimbotBone.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.AimbotBone.data(), len);

		// Save SilentAimBone
		len = TextVars.SilentAimBone.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.SilentAimBone.data(), len);

		len = TextVars.DebugFunctionNameMustInclude.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.DebugFunctionNameMustInclude.data(), len);

		len = TextVars.DebugFunctionObjectMustInclude.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.DebugFunctionObjectMustInclude.data(), len);

		TextVarsfile.close();
	}
	else
		printf("[ERROR] Could not open TextVars.bin for writing!\n");

	if (MiscSettings.ShouldSaveCVars)
	{
		std::ofstream CVarsinfile("CVars.bin", std::ios::binary);
		if (CVarsinfile.is_open())
		{
			CVarsinfile.seekp(0);

			CVarsinfile.write(reinterpret_cast<char*>(&CVars), sizeof(CVars));

			CVarsinfile.close();
		}
		else
			printf("[ERROR] Could not open CVars.bin for writing!\n");
	}
}


void LoadSettings()
{
	if (!Settings.ShouldLoad)
		return;

	std::ifstream MiscSettingsinfile("MiscSettings.bin", std::ios::binary);

	if (!MiscSettingsinfile.is_open()) 
	{
		printf("[INFO] MiscSettings.bin not found, using defaults.\n");
		return;
	}

	MiscSettingsinfile.seekg(0);

	MiscSettingsinfile.read(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));

	MiscSettingsinfile.close();

	std::ifstream AimbotSettingsinfile("AimbotSettings.bin", std::ios::binary);

	if (!AimbotSettingsinfile.is_open()) 
	{
		printf("[INFO] AimbotSettings.bin not found, using defaults.\n");
		return;
	}

	AimbotSettingsinfile.seekg(0);

	AimbotSettingsinfile.read(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));

	AimbotSettingsinfile.close();

	std::ifstream ESPSettingsinfile("ESPSettings.bin", std::ios::binary);

	if (!ESPSettingsinfile.is_open()) 
	{
		printf("[INFO] ESPSettings.bin not found, using defaults.\n");
		return;
	}

	ESPSettingsinfile.seekg(0);

	ESPSettingsinfile.read(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));

	ESPSettingsinfile.close();

	std::ifstream SilentAimSettingsinfile("SilentAimSettings.bin", std::ios::binary);

	if (!SilentAimSettingsinfile.is_open()) 
	{
		printf("[INFO] SilentAimSettings.bin not found, using defaults.\n");
		return;
	}

	SilentAimSettingsinfile.seekg(0);

	SilentAimSettingsinfile.read(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));

	SilentAimSettingsinfile.close();

	std::ifstream TextVarsinfile("TextVars.bin", std::ios::binary);

	if (!TextVarsinfile.is_open()) 
	{
		printf("[INFO] TextVars.bin not found, using defaults.\n");
		return;
	}

	size_t len = 0;
	TextVarsinfile.read(reinterpret_cast<char*>(&len), sizeof(len));
	TextVars.AimbotBone.resize(len);
	TextVarsinfile.read(TextVars.AimbotBone.data(), len);

	TextVarsinfile.read(reinterpret_cast<char*>(&len), sizeof(len));
	TextVars.SilentAimBone.resize(len);
	TextVarsinfile.read(TextVars.SilentAimBone.data(), len);

	TextVarsinfile.read(reinterpret_cast<char*>(&len), sizeof(len));
	TextVars.DebugFunctionNameMustInclude.resize(len);
	TextVarsinfile.read(TextVars.DebugFunctionNameMustInclude.data(), len);

	TextVarsinfile.read(reinterpret_cast<char*>(&len), sizeof(len));
	TextVars.DebugFunctionObjectMustInclude.resize(len);
	TextVarsinfile.read(TextVars.DebugFunctionObjectMustInclude.data(), len);

	TextVarsinfile.close();

	if (MiscSettings.ShouldSaveCVars)
	{
		std::ifstream CVarsinfile("CVars.bin", std::ios::binary);
		if (!CVarsinfile.is_open()) 
		{
			printf("[INFO] CVars.bin not found, using defaults.\n");
			return;
		}

		CVarsinfile.seekg(0);

		CVarsinfile.read(reinterpret_cast<char*>(&CVars), sizeof(CVars));

		CVarsinfile.close();
	}
}

void Cleanup(HMODULE hModule)
{
	Cleaning.store(true);
	std::cout << "Cleaning up...\n";

	GVars.Cleanup();

	if (UEngine::GetEngine())
	{
		void** objvTable = *reinterpret_cast<void***>(UEngine::GetEngine());
		MH_DisableHook(objvTable[Offsets::ProcessEventIdx]);
		MH_RemoveHook(objvTable[Offsets::ProcessEventIdx]);
	}

	while (g_PresentCount.load() != 0)
		_mm_pause();

	kiero::shutdown();

	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(Engine::oPresent); // BTW MH_ALL_HOOKS doesn't work on removing hooks.
	MH_RemoveHook(Engine::oResizeBuffers);
	MH_Uninitialize();

	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	HWND hwnd = FindWindow(L"UnrealWindow", nullptr);
	if (hwnd && oWndProc)
	{
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
		oWndProc = nullptr;
	}

	if (Engine::pContext) {
		Engine::pContext->OMSetRenderTargets(0, nullptr, nullptr);
		Engine::pContext->ClearState();
		Engine::pContext->Flush();
	}

	if (Engine::pRenderTargetView) 
	{
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}

	Engine::pSwapChain = nullptr;

	if (Engine::pContext) 
	{
		Engine::pContext->Release();
		Engine::pContext = nullptr;
	}

	if (Engine::pDevice) 
	{
		Engine::pDevice->Release();
		Engine::pDevice = nullptr;
	}

	std::cout << "Cleanup complete. Unloading DLL...\n";
	std::flush(std::cout);

	// Clean up console
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
}
