#include "pch.h"
#include "Engine.h"
#include "GUI/Menu.h"
#include "Config/ConfigManager.h"
#include <atomic>

extern bool init;
extern int Frames;
extern bool SettingsLoaded;
extern bool menu_key_pressed;
extern std::atomic<int> g_PresentCount;
extern std::atomic<bool> Cleaning;
extern std::atomic<bool> Resizing;
extern WNDPROC oWndProc;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (!Cleaning.load())
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		if (GUI::ShowMenu) {
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
	if (Cleaning.load())
		return Engine::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	Resizing.store(true);
	
	// Wait for any active Present processing to finish
	while (g_PresentCount.load() > 0)
		Sleep(1); 

	if (Engine::pRenderTargetView) {
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}

	if (ImGui::GetCurrentContext())
		ImGui_ImplDX11_InvalidateDeviceObjects();

	HRESULT hr = Engine::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	if (SUCCEEDED(hr))
	{
		pSwapChain->GetDesc(&Engine::sd);

		ID3D11Texture2D* pBackBuffer = nullptr;
		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
			Engine::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &Engine::pRenderTargetView);
			pBackBuffer->Release();
		}

		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplDX11_CreateDeviceObjects();
			ImGui::GetIO().DisplaySize = ImVec2((float)Width, (float)Height);
		}
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

	g_PresentCount.fetch_add(1);

	// Recursive protection
	static std::atomic<bool> inside_present{ false };
	if (inside_present.exchange(true))
	{
		HRESULT hr = Engine::oPresent(SwapChain, SyncInterval, Flags);
		g_PresentCount.fetch_sub(1);
		return hr;
	}

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

	if (!init || !ImGui::GetCurrentContext() || !Engine::pRenderTargetView) {
		inside_present.store(false);
		HRESULT hr = Engine::oPresent(SwapChain, SyncInterval, Flags);
		g_PresentCount.fetch_sub(1);
		return hr;
	}

	// Safe guard: Ensure World and Engine are fully ready before updating GVars
	if (!Engine::pDevice || !Engine::pContext || !Engine::pRenderTargetView) {
		inside_present.store(false);
		HRESULT hr = Engine::oPresent(SwapChain, SyncInterval, Flags);
		g_PresentCount.fetch_sub(1);
		return hr;
	}

	// Update display size and GVars
	ImGui::GetIO().DisplaySize = ImVec2((float)Engine::sd.BufferDesc.Width, (float)Engine::sd.BufferDesc.Height);
	GVars.ScreenSize = ImGui::GetIO().DisplaySize;

	GVars.AutoSetVariables();

	// Robust Input Handling
	static bool insert_was_down = false;
	bool insert_is_down = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
	if (insert_is_down && !insert_was_down)
	{
		GUI::ShowMenu = !GUI::ShowMenu;
		ImGui::GetIO().MouseDrawCursor = GUI::ShowMenu;
	}
	insert_was_down = insert_is_down;

	if (Frames > 0 && Frames % 600 == 0 && MiscSettings.ShouldAutoSave && SettingsLoaded)
	{
		ConfigManager::SaveSettings();
	}
	Frames++;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	try {
		GUI::RenderMenu();
		
		AimbotKeyDown = (AimbotSettings.AimbotKey != ImGuiKey_None) ? ImGui::IsKeyDown(AimbotSettings.AimbotKey) : true;

		if (CVars.RenderOptions) Cheats::RenderEnabledOptions();
		if (CVars.ListPlayers) Cheats::ListPlayers();

		if (GVars.World && GVars.World->K2_GetWorldSettings())
			GVars.World->K2_GetWorldSettings()->TimeDilation = CVars.BulletTime ? 0.3f : 1.0f;

		if (CVars.ESP) Cheats::RenderESP();
		
		if (CVars.SilentAim && GVars.ReadyOrNotChar && Utils::IsValidActor(GVars.ReadyOrNotChar))
		{
			if (SilentAimSettings.DrawFOV)
				Utils::DrawFOV(SilentAimSettings.MaxFOV, SilentAimSettings.FOVThickness);

			if (SilentAimSettings.DrawArrow)
			{
				AActor* TargetActor = Utils::GetBestTarget(GVars.PlayerController, SilentAimSettings.TargetCivilians, SilentAimSettings.TargetArrested, SilentAimSettings.TargetSurrendered, SilentAimSettings.TargetDead, SilentAimSettings.MaxFOV, SilentAimSettings.RequiresLOS, TextVars.SilentAimBone, SilentAimSettings.TargetAll);

				if (TargetActor && Utils::IsValidActor(TargetActor))
				{
					AReadyOrNotCharacter* TargetChar = reinterpret_cast<AReadyOrNotCharacter*>(TargetActor);
					if (TargetChar->Mesh)
					{
						std::wstring WideString = UtfN::StringToWString(TextVars.SilentAimBone);
						FName BoneName = UKismetStringLibrary::Conv_StringToName(WideString.c_str());
						FVector TargetLocation = TargetChar->Mesh->GetBoneTransform(BoneName, ERelativeTransformSpace::RTS_World).Translation;

						Utils::DrawSnapLine(TargetLocation, SilentAimSettings.ArrowThickness);
					}
				}
			}
		}

		if (CVars.Reticle) Cheats::DrawReticle();
		if (CVars.Aimbot) Cheats::Aimbot();
		if (CVars.TriggerBot) Cheats::TriggerBot();
		if (CVars.SpeedEnabled) Cheats::SetPlayerSpeed();

		if (GUI::TriggerBotKey != ImGuiKey_None && ImGui::IsKeyPressed(GUI::TriggerBotKey, false))
			CVars.TriggerBot = !CVars.TriggerBot;

		if (GUI::ESPKey != ImGuiKey_None && ImGui::IsKeyPressed(GUI::ESPKey, false))
			CVars.ESP = !CVars.ESP;

	} catch (...) {}

	ImGui::Render();

	if (Engine::pRenderTargetView) {
		Engine::pContext->OMSetRenderTargets(1, &Engine::pRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	
	inside_present.store(false);
	HRESULT hr = Engine::oPresent(SwapChain, SyncInterval, Flags);
	g_PresentCount.fetch_sub(1);
	return hr;
}



static void Cleanup(HMODULE hModule)
{
	Cleaning.store(true);

	ConfigManager::SaveSettings();

	CVars.Aimbot = false;
	CVars.ESP = false;
	CVars.SilentAim = false;
	CVars.TriggerBot = false;
	CVars.Reticle = false;
	CVars.SpeedEnabled = false;
	CVars.GodMode = false;
	CVars.InfAmmo = false;

	if (GVars.World)
		GVars.World->K2_GetWorldSettings()->TimeDilation = 1.0f;

	Sleep(100);

	while (g_PresentCount.load() != 0)
		Sleep(50);

	if (oWndProc) {
		HWND hwnd = FindWindowA("UnrealWindow", "ReadyOrNot  ");
		if (hwnd)
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	}

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (Engine::pRenderTargetView) {
		Engine::pRenderTargetView->Release();
		Engine::pRenderTargetView = nullptr;
	}

	std::cout << "Cleaning up...\n";

	// Clean up console
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
}

DWORD MainThread(HMODULE hModule)
{
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);

	std::cout << "Cheat Injecting...\n";

	extern std::atomic<HMODULE> g_hModule;
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

	ConfigManager::LoadSettings();

	Hooks::HookProcessEvent();

	while (!Cleaning.load())
		Sleep(100);
	
	Cleanup(hModule);

	return 0;
}
