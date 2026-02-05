#include "Engine.h"

#define MAJORVERSION 2
#define MINORVERSION 4
#define PATCHVERSION 3

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

// Add WndProc hook for input handling
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc = nullptr;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!Cleaning.load())
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		if (ShowMenu) {
			// Block these messages from the game when menu is open
			switch (uMsg) {
				case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
				case WM_MOUSEMOVE: case WM_MOUSEWHEEL: case WM_CHAR:
				case WM_KEYDOWN: case WM_KEYUP: case WM_SYSKEYDOWN: case WM_SYSKEYUP:
					return true;
			}
		}

		if (uMsg == WM_KEYUP && wParam == VK_END) {
			Cleaning.store(true);
			return TRUE;
		}
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall Engine::hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	Resizing.store(true);
	while (g_PresentCount.load() != 0)
		Sleep(0); 

	if (Engine::pRenderTargetView) {
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}

	ImGui_ImplDX11_InvalidateDeviceObjects();

	HRESULT hr = Engine::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	pSwapChain->GetDesc(&Engine::sd);

	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
		Engine::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &Engine::pRenderTargetView);
		pBackBuffer->Release();
	}

	ImGui_ImplDX11_CreateDeviceObjects();

	if (ImGui::GetCurrentContext()) {
		ImGui::GetIO().DisplaySize = ImVec2((float)Width, (float)Height);
	}

	Resizing.store(false);
	return hr;
}

static bool menu_key_pressed = false;

HRESULT __stdcall Engine::hkPresent(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags)
{
	if (Cleaning.load() || Resizing.load())
		return Engine::oPresent(SwapChain, SyncInterval, Flags);

	g_PresentCount.fetch_add(1);

	if (!init)
	{
		if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Engine::pDevice)))
		{
			Engine::pSwapChain = SwapChain;
			SwapChain->GetDesc(&Engine::sd);
			HWND hwnd = Engine::sd.OutputWindow;

			Engine::pDevice->GetImmediateContext(&Engine::pContext);
			Engine::HookResizeBuffers();
			Engine::InitImGui(hwnd);

			if (hwnd) {
				oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			}

			init = true;
		}
	}

	if (!init || !ImGui::GetCurrentContext()) {
		g_PresentCount.fetch_sub(1);
		return Engine::oPresent(SwapChain, SyncInterval, Flags);
	}

	// Update display size and GVars from REAL swapchain every frame
	DXGI_SWAP_CHAIN_DESC desc;
	SwapChain->GetDesc(&desc);
	ImGui::GetIO().DisplaySize = ImVec2((float)desc.BufferDesc.Width, (float)desc.BufferDesc.Height);
	GVars.ScreenSize = ImGui::GetIO().DisplaySize;

	GVars.AutoSetVariables();

	// Menu Toggle Logic (Using GetAsyncKeyState for reliability)
	if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
		if (!menu_key_pressed) {
			ShowMenu = !ShowMenu;
			ImGui::GetIO().MouseDrawCursor = ShowMenu;
			menu_key_pressed = true;
		}
	} else {
		menu_key_pressed = false;
	}

	if (Frames % 300 == 0 && MiscSettings.ShouldAutoSave)
	{
		SaveSettings();
	}

	if (Frames % 60 == 0 && !CVars.SecretFeatures)
	{
		if (GVars.PlayerController && GVars.PlayerController->PlayerState)
		{
			auto PlayerState = GVars.PlayerController->PlayerState;
			auto PlayerName = PlayerState->GetPlayerName().ToString();
			if (PlayerName == "PeachMarrow12" || PlayerName == "DiaperBlastrPC")
				CVars.SecretFeatures = true;
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ShowMenu) {
		ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
		ImGui::Begin((const char*)u8"Ready or Not 免费辅助 - PeachMarrow12", nullptr, ImGuiWindowFlags_NoCollapse);

		ImGui::SeparatorText("Hello, Have Fun Cheating!");

		if  (ImGui::BeginTabBar("MainTabBar"))
		{
			if (ImGui::BeginTabItem((const char*)u8"关于"))
			{
				ImGui::Text((const char*)u8"Ready or Not 免费辅助 - PeachMarrow12");
				ImGui::Text((const char*)u8"版本 %d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);

				if (GVars.PlayerController && GVars.PlayerController->PlayerState)
				{
					APlayerState* PlayerState = GVars.PlayerController->PlayerState;
					std::string PlayerName = PlayerState->GetPlayerName().ToString();
					ImGui::Text((const char*)u8"感谢使用此辅助, %s!", PlayerName.c_str());
				}
				else
				{
					ImGui::Text((const char*)u8"未找到用户名，但仍感谢使用！");
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem((const char*)u8"玩家"))
			{
				if (ImGui::Checkbox((const char*)u8"无敌模式 (GodMode)", &CVars.GodMode))
					Cheats::ToggleGodMode();
				HostOnlyTooltip();

				ImGui::Checkbox((const char*)u8"自瞄 (Aimbot)", &CVars.Aimbot);

				ImGui::Checkbox((const char*)u8"静默自瞄 (Silent Aim)", &CVars.SilentAim);
				AddDefaultTooltip((const char*)u8"尚不完善，请谨慎使用。");
				HostOnlyTooltip();

				ImGui::Checkbox((const char*)u8"透视 (ESP)", &CVars.ESP);

				ImGui::SliderFloat((const char*)u8"移动速度", &CVars.Speed, 1, 30, "%.1f");
				ImGui::SameLine();
				ImGui::Checkbox((const char*)u8"开启速度修改", &CVars.SpeedEnabled);

				if (ImGui::SliderFloat((const char*)u8"视野 (FOV)", &CVars.FOV, 0.1f, 179.9f))
				{
					Cheats::ChangeFOV();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem((const char*)u8"武器"))
			{
				if (ImGui::Checkbox((const char*)u8"无限弹药", &CVars.InfAmmo))
					Cheats::ToggleInfAmmo();
				HostOnlyTooltip();

				if (ImGui::Button((const char*)u8"移除后坐力"))
					Cheats::RemoveRecoil();

				if (ImGui::Button((const char*)u8"移除扩散"))
					Cheats::RemoveSpread();

				if (ImGui::Button((const char*)u8"添加全自动"))
					Cheats::AddAutoFire();

				if (ImGui::Button((const char*)u8"添加穿墙"))
					Cheats::PenetrateWalls();

				if (ImGui::Button((const char*)u8"秒杀"))
					Cheats::InstaKill();

				if (ImGui::Button((const char*)u8"增加射速"))
					Cheats::SetFireRate(0.001f);
				
				ImGui::Checkbox((const char*)u8"准星处射击", &CVars.ShootFromReticle);

				if (ImGui::Button((const char*)u8"添加弹匣"))
					Cheats::AddMag();

				ImGui::Checkbox((const char*)u8"自动射击 (TriggerBot)", &CVars.TriggerBot);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem((const char*)u8"世界"))
			{
				if (ImGui::Button((const char*)u8"杀死所有嫌疑人"))
					Cheats::KillAll(ETeam::TEAM_SUSPECT);

				ImGui::SameLine();

				if (ImGui::Button((const char*)u8"降伏所有嫌疑人"))
					Cheats::SurrenderAll(ETeam::TEAM_SUSPECT);

				ImGui::SameLine();

				if (ImGui::Button((const char*)u8"逮捕所有嫌疑人"))
					Cheats::ArrestAll(ETeam::TEAM_SUSPECT);
				HostOnlyTooltip();

				if (ImGui::Button((const char*)u8"杀死所有平民"))
					Cheats::KillAll(ETeam::TEAM_CIVILIAN);

				ImGui::SameLine();

				if (ImGui::Button((const char*)u8"降伏所有平民"))
					Cheats::SurrenderAll(ETeam::TEAM_CIVILIAN);

				ImGui::SameLine();

				if (ImGui::Button((const char*)u8"逮捕所有平民"))
					Cheats::ArrestAll(ETeam::TEAM_CIVILIAN);
				HostOnlyTooltip();

				if (ImGui::Button((const char*)u8"收集所有证据"))
					Cheats::GetAllEvidence();

				if (ImGui::Button((const char*)u8"自动获胜"))
					Cheats::AutoWin();

				if (ImGui::Button((const char*)u8"解锁所有门"))
					Cheats::UnlockDoors();

				ImGui::Checkbox((const char*)u8"子弹时间", &CVars.BulletTime);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem((const char*)u8"杂项"))
			{
				if (ImGui::Button((const char*)u8"保存设置"))
					SaveSettings();
				ImGui::SameLine();

				if (ImGui::Button((const char*)u8"加载设置"))
					LoadSettings();
				AddDefaultTooltip((const char*)u8"这些只保存和加载配置，而不是哪些功能已启用。");

				ImGui::Checkbox((const char*)u8"调试 (Debug)", &CVars.Debug);
				AddDefaultTooltip((const char*)u8"这只是开启了一些我用来查找错误和有用信息的菜单选项。对你来说可能没用。");

				ImGui::InputText((const char*)u8"调试函数名包含", &TextVars.DebugFunctionNameMustInclude);
				ImGui::InputText((const char*)u8"调试对象名包含", &TextVars.DebugFunctionObjectMustInclude);

				if (CVars.Debug)
					if (ImGui::Button((const char*)u8"打印 Actor"))
						Utils::PrintActors(nullptr);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem((const char*)u8"配置"))
			{
				if (ImGui::TreeNode((const char*)u8"自瞄设置 (Aimbot Settings)"))
				{
					ImGui::SliderFloat((const char*)u8"自瞄视野 (Aimbot FOV)", &AimbotSettings.MaxFOV, 0.01f, 180.0f, "%.1f");

					ImGui::Checkbox((const char*)u8"需要可见性 (LOS)", &AimbotSettings.LOS);
					AddDefaultTooltip((const char*)u8"目标必须可见；需要视线检查。");

					ImGui::Checkbox((const char*)u8"目标平民", &AimbotSettings.TargetCivilians);

					ImGui::Checkbox((const char*)u8"目标已死亡单位", &AimbotSettings.TargetDead);

					ImGui::Checkbox((const char*)u8"目标已逮捕单位", &AimbotSettings.TargetArrested);

					ImGui::Checkbox((const char*)u8"目标所有单位", &AimbotSettings.TargetAll);

					ImGui::SliderFloat((const char*)u8"最大距离", &AimbotSettings.MaxDistance, 0.0f, 300.0f, "%.1f");

					ImGui::SliderFloat((const char*)u8"最小距离", &AimbotSettings.MinDistance, 0.0f, 100.0f, "%.1f");

					ImGui::Checkbox((const char*)u8"平滑自瞄", &AimbotSettings.Smooth);

					ImGui::SliderFloat((const char*)u8"平滑系数", &AimbotSettings.SmoothingVector, 1.0f, 20.0f, "%.2f");

					ImGui::Checkbox((const char*)u8"显示指向箭头", &AimbotSettings.DrawArrow);

					ImGui::Checkbox((const char*)u8"显示视野范围", &AimbotSettings.DrawFOV);

					if (ImGui::BeginCombo((const char*)u8"目标骨骼", TextVars.AimbotBone.c_str()))
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

					ImGui::Checkbox((const char*)u8"需要热键配合", &AimbotSettings.RequireKeyHeld);

					const char* ABpreview = ImGui::GetKeyName(AimbotSettings.AimbotKey);
					if (!ABpreview) ABpreview = "None";

					if (ImGui::BeginCombo((const char*)u8"选择自瞄按键", ABpreview))
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
						AddDefaultTooltip((const char*)u8"仅在按住此键时激活自瞄");
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode((const char*)u8"透视设置 (ESP Settings)"))
				{
					ImGui::Checkbox((const char*)u8"显示队友", &ESPSettings.ShowTeam);

					ImGui::Checkbox((const char*)u8"显示方框", &ESPSettings.ShowBox);

					ImGui::Checkbox((const char*)u8"显示陷阱", &ESPSettings.ShowTraps);

					ImGui::Checkbox((const char*)u8"显示敌方距离", &ESPSettings.ShowEnemyDistance);

					ImGui::Checkbox((const char*)u8"显示骨骼", &ESPSettings.Bones);

					ImGui::ColorEdit4((const char*)u8"嫌疑人颜色", (float*)&ESPSettings.SuspectColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4((const char*)u8"平民颜色", (float*)&ESPSettings.CivilianColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4((const char*)u8"死亡单位颜色", (float*)&ESPSettings.DeadColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4((const char*)u8"队友颜色", (float*)&ESPSettings.TeamColor, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorEdit4((const char*)u8"被捕单位颜色", (float*)&ESPSettings.ArrestColor, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox((const char*)u8"仅显示可见目标 (LOS)", &ESPSettings.LOS);

					if (ImGui::SliderFloat((const char*)u8"骨骼透明度", &ESPSettings.BoneOpacity, 0.0f, 1.0f, "%.2f"))
					{
						ESPSettings.SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, ESPSettings.BoneOpacity);
						ESPSettings.DeadColor = ImVec4(0.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.TeamColor = ImVec4(0.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
						ESPSettings.ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
					}

					ImGui::Checkbox((const char*)u8"显示任务目标", &ESPSettings.ShowObjectives);
					AddDefaultTooltip((const char*)u8"任务目标不显示实际位置");
					ImGui::TreePop();
				}

				if (ImGui::TreeNode((const char*)u8"静默自瞄设置 (Silent Aim Settings)"))
				{
					ImGui::Checkbox((const char*)u8"目标平民", &SilentAimSettings.TargetCivilians);

					ImGui::Checkbox((const char*)u8"目标所有单位", &SilentAimSettings.TargetAll);

					ImGui::Checkbox((const char*)u8"目标已死亡单位", &SilentAimSettings.TargetDead);

					ImGui::Checkbox((const char*)u8"目标已投降单位", &SilentAimSettings.TargetSurrendered);

					ImGui::Checkbox((const char*)u8"目标已逮捕单位", &SilentAimSettings.TargetArrested);

					ImGui::SliderFloat((const char*)u8"静默自瞄视野", &SilentAimSettings.MaxFOV, 0, 180.0f, "%.1f");

					ImGui::Checkbox((const char*)u8"显示视野范围", &SilentAimSettings.DrawFOV);

					ImGui::SliderFloat((const char*)u8"视野线粗细", &SilentAimSettings.FOVThickness, 0.1f, 10.0f, "%.2f");

					ImGui::Checkbox((const char*)u8"显示追踪线", &SilentAimSettings.DrawArrow);

					ImGui::SliderFloat((const char*)u8"追踪线粗细", &SilentAimSettings.ArrowThickness, 0.1f, 10.0f, "%.2f");

					ImGui::Checkbox((const char*)u8"需要可见性 (LOS)", &SilentAimSettings.RequiresLOS);

					ImGui::SliderFloat((const char*)u8"命中概率", &SilentAimSettings.HitChance, 0.0f, 100, "%.1f");

					if (ImGui::BeginCombo((const char*)u8"目标骨骼", TextVars.SilentAimBone.c_str()))
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

				if (ImGui::TreeNode((const char*)u8"杂项设置 (Misc Settings)"))
				{
					ImGui::SeparatorText((const char*)u8"准星设置");

					ImGui::Checkbox((const char*)u8"准星", &CVars.Reticle);

					ImGui::ColorEdit4((const char*)u8"准星颜色", (float*)&MiscSettings.ReticleColor);

					ImGui::DragFloat2((const char*)u8"准星位置", (float*)&MiscSettings.ReticlePosition, 1, -100, 100);

					ImGui::SliderFloat((const char*)u8"准星大小", &MiscSettings.ReticleSize, 1, 15);

					ImGui::Checkbox((const char*)u8"使用十字准星", &MiscSettings.CrossReticle);

					ImGui::Checkbox((const char*)u8"仅在投掷手雷时显示准星", &MiscSettings.ReticleWhenThrowing);

					ImGui::SeparatorText((const char*)u8"自动射击设置 (TriggerBot)");

					ImGui::Checkbox((const char*)u8"自动射击平民", &MiscSettings.TriggerBotTargetsCivilians);

					ImGui::Checkbox((const char*)u8"自动射击使用静默自瞄", &MiscSettings.TriggerBotUsesSilentAim);
					AddDefaultTooltip((const char*)u8"确保你能命中目标");

					ImGui::SeparatorText((const char*)u8"其他");

					ImGui::Checkbox((const char*)u8"显示已开启的功能", &CVars.RenderOptions);

					ImGui::Checkbox((const char*)u8"显示玩家列表", &CVars.ListPlayers);

					ImGui::Checkbox((const char*)u8"自动保存设置", &MiscSettings.ShouldAutoSave);
					ImGui::SameLine();
					ImGui::Checkbox((const char*)u8"保存已启用的功能状态", &MiscSettings.ShouldSaveCVars);

					ImGui::SeparatorText((const char*)u8"按键绑定");

					// Create a combo box
					const char* TBpreview = ImGui::GetKeyName(TriggerBotKey);
					if (!TBpreview) TBpreview = "None";

					if (ImGui::BeginCombo((const char*)u8"选择自动射击按键", TBpreview))
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

					if (ImGui::BeginCombo((const char*)u8"选择透视按键", ESPpreview))
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

					if (ImGui::BeginCombo((const char*)u8"选择锁定目标按键", AimPreview))
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
				if (ImGui::BeginTabItem((const char*)u8"秘密功能 (Secret Features)"))
				{
					ImGui::Text((const char*)u8"此选项仅供开发使用。");
					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}

		ImGui::Separator();
		ImGui::Text((const char*)u8"你可以在 UnknownCheats.me 上找到我，ID 为 Peachmarrow13。");
		ImGui::SameLine();
		ImGui::Text((const char*)u8"此辅助在 UnknownCheats.me 免费发布，请勿从其他任何地方下载。");
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

	if (CVars.BulletTime)
		GVars.World->K2_GetWorldSettings()->TimeDilation = 0.3f; // Slow-mo
	else
		GVars.World->K2_GetWorldSettings()->TimeDilation = 1.0f; // Normal

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

	if (CVars.TriggerBot)
		Cheats::TriggerBot();

	if (CVars.SpeedEnabled)
		Cheats::SetPlayerSpeed();

	ImGui::Render();

	if (Engine::pRenderTargetView) {
		Engine::pContext->OMSetRenderTargets(1, &Engine::pRenderTargetView, nullptr);

		D3D11_VIEWPORT vp = {};
		vp.Width = ImGui::GetIO().DisplaySize.x;
		vp.Height = ImGui::GetIO().DisplaySize.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		Engine::pContext->RSSetViewports(1, &vp);
	}
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (TriggerBotKey != ImGuiKey_None && ImGui::IsKeyPressed(TriggerBotKey, false))
	{
		CVars.TriggerBot = !CVars.TriggerBot;
	}

	if (ESPKey != ImGuiKey_None && ImGui::IsKeyPressed(ESPKey, false))
	{
		CVars.ESP = !CVars.ESP;
	}

	g_PresentCount.fetch_sub(1);

	return Engine::oPresent ? Engine::oPresent(SwapChain, SyncInterval, Flags) : S_OK;
}

DWORD MainThread(HMODULE hModule)
{
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);

	std::cout << "Cheat Injecting...\n";

	g_hModule.store(hModule);

	int Attempts = 0;

	while (!UEngine::GetEngine() && Attempts < 50)
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

	if (!Engine::HookPresent())
	{
		printf("[ERROR] Failed to initialize engine hooks.\n");
		Cleaning.store(true);
		Cleanup(hModule);
	}
	else
		printf("Engine hooks initialized successfully.\n");

	Sleep(1000); // Wait a second to ensure everything is loaded	

	std::cout << "Cheat Injected\n";

	LoadSettings();

	Hooks::HookProcessEvent();

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
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		Cleaning.store(true);
		break;
	}
	
	return TRUE;
}

void SaveSettings()
{
	//if (!Settings.ShouldSave)
		//return;

	// Save MiscSettings (binary)
	std::ofstream MiscSettingsfile("MiscSettings.bin", std::ios::binary);
	if (MiscSettingsfile.is_open())
	{
		MiscSettingsfile.write(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));
		MiscSettingsfile.close();
	}

	// Save AimbotSettings (binary)
	std::ofstream AimbotSettingsfile("AimbotSettings.bin", std::ios::binary);
	if (AimbotSettingsfile.is_open())
	{
		AimbotSettingsfile.write(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));
		AimbotSettingsfile.close();
	}

	// Save ESPSettings (binary)
	std::ofstream ESPSettingsfile("ESPSettings.bin", std::ios::binary);
	if (ESPSettingsfile.is_open())
	{
		ESPSettingsfile.write(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));
		ESPSettingsfile.close();
	}

	// Save SilentAimSettings (binary)
	std::ofstream SilentAimSettingsfile("SilentAimSettings.bin", std::ios::binary);
	if (SilentAimSettingsfile.is_open())
	{
		SilentAimSettingsfile.write(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));
		SilentAimSettingsfile.close();
	}

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

	if (MiscSettings.ShouldSaveCVars)
	{
		std::ofstream CVarsinfile("CVars.bin", std::ios::binary);
		if (CVarsinfile.is_open())
		{
			CVarsinfile.seekp(0);

			CVarsinfile.write(reinterpret_cast<char*>(&CVars), sizeof(CVars));

			CVarsinfile.close();
		}
	}
}


void LoadSettings()
{
	if (!Settings.ShouldLoad)
		return;

	std::ifstream MiscSettingsinfile("MiscSettings.bin", std::ios::binary);

	if (!MiscSettingsinfile.is_open()) return;

	MiscSettingsinfile.seekg(0);

	MiscSettingsinfile.read(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));

	MiscSettingsinfile.close();

	std::ifstream AimbotSettingsinfile("AimbotSettings.bin", std::ios::binary);

	if (!AimbotSettingsinfile.is_open()) return;

	AimbotSettingsinfile.seekg(0);

	AimbotSettingsinfile.read(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));

	AimbotSettingsinfile.close();

	std::ifstream ESPSettingsinfile("ESPSettings.bin", std::ios::binary);

	if (!ESPSettingsinfile.is_open()) return;

	ESPSettingsinfile.seekg(0);

	ESPSettingsinfile.read(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));

	ESPSettingsinfile.close();

	std::ifstream SilentAimSettingsinfile("SilentAimSettings.bin", std::ios::binary);

	if (!SilentAimSettingsinfile.is_open()) return;

	SilentAimSettingsinfile.seekg(0);

	SilentAimSettingsinfile.read(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));

	SilentAimSettingsinfile.close();

	std::ifstream TextVarsinfile("TextVars.bin", std::ios::binary);

	if (!TextVarsinfile.is_open()) return;

	size_t len;
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
		if (!CVarsinfile.is_open()) return;

		CVarsinfile.seekg(0);

		CVarsinfile.read(reinterpret_cast<char*>(&CVars), sizeof(CVars));

		CVarsinfile.close();
	}
}

void Cleanup(HMODULE hModule)
{
	Cleaning.store(true);
	std::cout << "Cleaning up...\n";

	SaveSettings();

	CVars.Aimbot = false;
	CVars.ESP = false;
	CVars.GodMode = false;
	CVars.InfAmmo = false;
	CVars.SpeedEnabled = false;
	CVars.Speed = 1;
	Cheats::ToggleGodMode();
	Cheats::ToggleInfAmmo();
	Cheats::SetPlayerSpeed();

	if (UEngine::GetEngine())
	{
		void** objvTable = *reinterpret_cast<void***>(UEngine::GetEngine());
		MH_DisableHook(objvTable[Offsets::ProcessEventIdx]);
		MH_RemoveHook(objvTable[Offsets::ProcessEventIdx]);
	}

	MH_DisableHook(Engine::PresentAddr);
	MH_RemoveHook(Engine::PresentAddr);

	while (g_PresentCount.load() > 0)
		Sleep(1);  // wait until all Present calls finish

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

	MH_Uninitialize();

	// Clean up DirectX resources
	if (Engine::pRenderTargetView) {
		printf("Releasing render target view...\n");
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}
	if (Engine::pContext) {
		printf("Releasing device context...\n");
		Engine::pContext->Release();
		Engine::pContext = nullptr;
	}
	if (Engine::pDevice) {
		printf("Releasing device...\n");
		Engine::pDevice->Release();
		Engine::pDevice = nullptr;
	}
	if (Engine::pSwapChain) {
		printf("Releasing swap chain...\n");
		Engine::pSwapChain->Release();
		Engine::pSwapChain = nullptr;
	}

	std::cout << "Cleanup complete. Unloading DLL...\n";

	// Clean up console
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
}