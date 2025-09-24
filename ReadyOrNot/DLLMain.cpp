#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include <cstdio>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <chrono>
#include <imgui_internal.h>

#include "Cheats.h"
#include "Utils.h"

#include "SDK/ReadyOrNot_classes.hpp"
#include "SDK/Engine_classes.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

static bool InfAmmo = false;
static bool GodMode = false;
static bool AimbotEnabled = false;
static bool ShowMenu = true;
bool Cleaning = false;
bool AllowGameInput = true;
Variables* Vars = new Variables();

void SetVariablesRepeat(Variables& vars);
void HookSwapChain();
void Cleanup(std::thread& AutoVarsThread, Variables* Vars, HMODULE hModule);
void InitImGui(HWND hwnd);

typedef HRESULT(__stdcall* tPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
tPresent oPresent = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* pRenderTargetView = nullptr;
DXGI_SWAP_CHAIN_DESC sd = {};
static bool init = false;

// Add WndProc hook for input handling
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc = nullptr;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Handle ImGui input first
	if (ShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	if (AllowGameInput)
	{
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}
	else
		return true; // Block input to the game when AllowGameInput is false
}

HRESULT __stdcall hkPresent(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			HWND hwnd = FindWindow(L"UnrealWindow", nullptr);
			pDevice->GetImmediateContext(&pContext);

			pSwapChain = SwapChain;
			SwapChain->GetDesc(&sd);

			if (!hwnd) hwnd = GetForegroundWindow();

			InitImGui(hwnd);

			// Hook WndProc for input handling
			if (hwnd) {
				oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			}

			init = true;
		}
	}
	
	if (Cleaning)
	{
		HWND hwnd = FindWindow(L"UnrealWindow", nullptr);
		// Restore original WndProc
		if (hwnd && oWndProc) {
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
			oWndProc = nullptr;
		}

		if (SwapChain) {
			void** vTable = *reinterpret_cast<void***>(SwapChain);
			if (vTable && vTable[8]) {
				DWORD oldProtect;
				if (VirtualProtect(&vTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
					vTable[8] = (void*)oPresent;  // restore original
					VirtualProtect(&vTable[8], sizeof(void*), oldProtect, &oldProtect);
				}
			}
		}
	}

	if (!ImGui::GetCurrentContext())
		return oPresent(SwapChain, SyncInterval, Flags);



	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ShowMenu) {
		ImGui::Begin("Free Ready or Not Cheat by PeachMarrow12", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::Text("Hello, Have Fun Cheating!");

		if (ImGui::TreeNode("Configuration"))
		{
			ImGui::Checkbox("Allow Game Input", &AllowGameInput);

			ImGui::TreePop();
		}

		if (ImGui::Checkbox("GodMode", &GodMode))
		{
			Cheats::ToggleGodMode();
		}

		if (ImGui::Checkbox("Aimbot", &AimbotEnabled))
		{
			Cheats::ToggleAimbot();
		}

		if (ImGui::Checkbox("Infinite Ammo", &InfAmmo))
		{
			Cheats::ToggleInfAmmo();
		}

		if (ImGui::Button("Hide Window")) {
			ShowMenu = false;
		}
		ImGui::End();
	}
	else
	{
		ShowCursor(false);
		ImGui::GetIO().MouseDrawCursor = false;
	}

	if (pRenderTargetView) {
		pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

		// Set viewport - ADD THIS
		D3D11_VIEWPORT vp = {};
		vp.Width = (float)sd.BufferDesc.Width;
		vp.Height = (float)sd.BufferDesc.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pContext->RSSetViewports(1, &vp);
	}

	ImGui::Render();
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (AimbotEnabled)
		Cheats::Aimbot(Vars);

	return oPresent ? oPresent(SwapChain, SyncInterval, Flags) : S_OK;
}

// Attach hook
void HookPresent()
{
	if (!pSwapChain) {
		std::cout << "[ERROR] SwapChain is null...\n";
		Cleaning = true;
		Sleep(30);
		return;
	}

	void** vTable = *reinterpret_cast<void***>(pSwapChain);
	oPresent = (tPresent)vTable[8];

	DWORD oldProtect;
	VirtualProtect(&vTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
	vTable[8] = (void*)&hkPresent;
	VirtualProtect(&vTable[8], sizeof(void*), oldProtect, &oldProtect);
}


DWORD MainThread(HMODULE hModule)
{
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);
	freopen_s(&Dummy, "CONIN$", "r", stdin);

	std::cout << "Cheat Injecting...\n";

	std::thread AutoVarsThread(SetVariablesRepeat, std::ref(*Vars));

	int FailAmount = 0;

	while (!Vars->World and FailAmount < 8) {
		Vars->World = Utils::GetWorldSafe();
		FailAmount++;
		Sleep(100);
	}

	while (!Vars->PlayerController and FailAmount < 8) {
		Vars->PlayerController = Utils::GetPlayerController();
		FailAmount++;
		Sleep(100);
	}

	if (FailAmount >= 8) {
		std::cout << "[ERROR] Failed to get essential game pointers. Exiting...\n";
		Cleaning = true;
		Sleep(2000);
		Cleanup(AutoVarsThread, Vars, hModule);
		return 0;
	}
	Sleep(1000); // Wait a second to ensure everything is loaded

	HookSwapChain(); // Create a dummy device and swapchain to get the vtable
	HookPresent(); // Hook the Present function

	AReadyOrNotPlayerController* PC = (AReadyOrNotPlayerController*)Vars->PlayerController;

	std::cout << "Cheat Injected\n";

	GodMode = Vars->ReadyOrNotChar ? Vars->ReadyOrNotChar->bGodMode : false;

	while (!Cleaning)
	{
		if (GetAsyncKeyState(VK_END) & 1) // Exit with END key
		{
			std::cout << "Exiting...\n";
			Cleaning = true;
			break;
		}

		if (GetAsyncKeyState(VK_INSERT) & 1) // Toggle Cheat Menu with INSERT key
		{
			ShowMenu = !ShowMenu;
			std::cout << "Menu: " << (ShowMenu ? "ON" : "OFF") << "\n";
		}
		
		if (GetAsyncKeyState(VK_F1) & 1) // Toggle God Mode with F1
		{
			GodMode = !GodMode;
			std::cout << "God Mode: " << (GodMode ? "ON" : "OFF") << "\n";
			Cheats::ToggleGodMode();
		}

		if (GetAsyncKeyState(VK_F2) & 1)
		{
			InfAmmo = !InfAmmo;
			std::cout << "Infinite Ammo: " << (InfAmmo ? "ON" : "OFF") << "\n";
			Cheats::ToggleInfAmmo();
		}

		if (GetAsyncKeyState(VK_F3) & 1) // Toggle Aimbot with F3
		{
			AimbotEnabled = !AimbotEnabled;
			std::cout << "Aimbot: " << (AimbotEnabled ? "ON" : "OFF") << "\n";
			Cheats::ToggleAimbot();
		}

		if (GetAsyncKeyState(VK_F4) & 1) // Sets weapon stats with F4 : no recoil, no spread, etc.
		{
			Cheats::UpgradeWeaponStats();
			std::cout << "Weapon Upgraded\n";
		}

		Sleep(100);
	}

	Cleanup(AutoVarsThread, Vars, hModule);
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
		break;
	}
	
	return TRUE;
}

void SetVariablesRepeat(Variables& vars) {
	while (!Cleaning) {
		Variables::AutoSetVariables(vars);
		Sleep(100);
	}
	return;
}

void InitImGui(HWND hwnd)
{
	if (!pDevice || !pContext) {
		std::cout << "[ERROR] Device or Context is null...\n";
		Cleaning = true;
		Sleep(30);
		return;
	}

	if (!hwnd) {
		std::cout << "[ERROR] HWND is null...\n";
		Cleaning = true;
		Sleep(30);
		return;
	}

	if (Cleaning) return;

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer backends
	if (!ImGui_ImplWin32_Init(hwnd)) {
		std::cout << "[ERROR] ImGui_ImplWin32_Init failed\n";
		return;
	}
	if (!ImGui_ImplDX11_Init(pDevice, pContext)) {
		std::cout << "[ERROR] ImGui_ImplDX11_Init failed\n";
		ImGui_ImplWin32_Shutdown();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Controller Controls
	io.Fonts->AddFontDefault();
	io.MouseDrawCursor = true;  // Let ImGui draw the cursor
	io.WantCaptureMouse = true; // ImGui should capture mouse when over windows

	ImGui::StyleColorsDark();

	if (pSwapChain) { // Create render target if we have a valid swapchain
		ID3D11Texture2D* pBackBuffer = nullptr;
		HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
		if (SUCCEEDED(hr)) {
			hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
			if (SUCCEEDED(hr)) {
				std::cout << "[InitImGui] Render target view created successfully\n";
			}
			else {
				std::cout << "[ERROR] Failed to create render target view: " << std::hex << hr << std::endl;
			}
			pBackBuffer->Release();
		}
		else {
			std::cout << "[ERROR] Failed to get back buffer: " << std::hex << hr << std::endl;
		}
	}

	std::cout << "[InitImGui] ImGui initialized successfully\n";
}

void HookSwapChain()
{
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate = { 240, 1 };
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = FindWindow(L"UnrealWindow", nullptr);
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL FeatureLevel;
	D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;

	while (!pSwapChain) {
		if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			&FeatureLevelsRequested, 1, D3D11_SDK_VERSION,
			&sd, &pSwapChain, &pDevice, &FeatureLevel, &pContext)))
		{
			void** vTable = *reinterpret_cast<void***>(pSwapChain);
			// Present = vTable[8]
			// ResizeBuffers = vTable[13]
		}
		else
		{
			break;
		}
	}
}

static void Cleanup(std::thread& AutoVarsThread, Variables* Vars, HMODULE hModule)
{
	Cleaning = true;
	std::cout << "Cleaning up...\n";
	// Properly join the thread before exiting
	if (AutoVarsThread.joinable()) {
		AutoVarsThread.join();
	}

	Sleep(300); // Give some time for the thread to finish

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Clean up DirectX resources
	if (pRenderTargetView) {
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}
	if (pContext) {
		pContext->Release();
		pContext = nullptr;
	}
	if (pDevice) {
		pDevice->Release();
		pDevice = nullptr;
	}
	if (pSwapChain) {
		pSwapChain->Release();
		pSwapChain = nullptr;
	}
	if (oPresent) {
		oPresent = nullptr;
	}

	// Clean up Variables
	if (Vars) {
		delete Vars;
		Vars = nullptr;
	}

	std::cout << "Cleanup complete. Unloading DLL...\n";
	Sleep(200);

	// Clean up console
	FreeConsole();

	// Exit the thread and unload the DLL
	FreeLibraryAndExitThread(hModule, 0);
}