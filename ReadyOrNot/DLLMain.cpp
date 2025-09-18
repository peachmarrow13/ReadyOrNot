#include <cstdio>
#include <iostream>
#include <thread>
#include <Windows.h>
#include "Cheats.h"
#include "Utils.h"
#include "SDK/Engine_classes.hpp"
#include <d3d11.h>
#include <dxgi.h>
#include <chrono>
#include "SDK/ReadyOrNot_classes.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

bool InfAmmo = false;
void SetVariablesRepeat(Variables& vars);
void HookSwapChain();
void Cleanup(std::thread& AutoVarsThread, Variables* Vars, HMODULE hModule);
bool Cleaning = false;
Variables* Vars = new Variables();

typedef HRESULT(__stdcall* tPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
tPresent oPresent = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
DXGI_SWAP_CHAIN_DESC sd = {};

static float FrameTime = 0.0f;
static int FrameCount = 0;
static float fps = 0;
std::chrono::high_resolution_clock::time_point LastFrameTime;
std::chrono::high_resolution_clock::time_point CurrentFrameTime;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	//// Run per-frame logic here
	//FrameCount++;
	//if (FrameCount >= 4)
	//{
	//	auto CurrentFrameTime = std::chrono::high_resolution_clock::now();
	//	float delta = std::chrono::duration<float>(CurrentFrameTime - LastFrameTime).count();
	//	float avgDelta = delta / FrameCount;      // average time per frame
	//	float fps = 1.0f / avgDelta;

	//	printf("FPS: %.2f\r", fps);

	//	LastFrameTime = CurrentFrameTime;
	//	FrameCount = 0;  // reset for next batch
	//}

	Cheats::Aimbot(Vars);

	if (Cleaning)
	{
		if (pSwapChain) {
			void** vTable = *reinterpret_cast<void***>(pSwapChain);
			if (vTable && vTable[8]) {
				DWORD oldProtect;
				if (VirtualProtect(&vTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
					vTable[8] = (void*)oPresent;  // restore original
					VirtualProtect(&vTable[8], sizeof(void*), oldProtect, &oldProtect);
				}
			}
		}
	}

	return oPresent ? oPresent(pSwapChain, SyncInterval, Flags)
		: S_OK;
}

// Attach hook
void HookPresent()
{
	while (!pSwapChain) {
		std::cerr << "SwapChain is null...\n";
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

	std::thread AutoVarsThread(SetVariablesRepeat, std::ref(*Vars));

	while (!Vars->World) {
		Vars->World = Utils::GetWorldSafe();
		Sleep(100);
	}

	while (!Vars->PlayerController) {
		Vars->PlayerController = Utils::GetPlayerController();
		Sleep(100);
	}

	Sleep(1000); // Wait a second to ensure everything is loaded

	HookSwapChain(); // Create a dummy device and swapchain to get the vtable
	HookPresent(); // Hook the Present function

	std::cout << "Cheat Injected\n";

	while (!Cleaning)
	{
		if (GetAsyncKeyState(VK_F1) & 1) // Toggle God Mode with F1
		{
			Cheats::ToggleGodMode();
			std::cout << "God Mode: " << (Vars->ReadyOrNotChar->bGodMode ? "ON" : "OFF") << "\n";
		}
		if (GetAsyncKeyState(VK_END) & 1) // Exit with END key
		{
			std::cout << "Exiting...\n";
			Cleaning = true;
			break;
		}
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			InfAmmo = !InfAmmo;
			std::cout << "Infinite Ammo: " << (InfAmmo ? "ON" : "OFF") << "\n";
		}
		if (InfAmmo && Vars->ReadyOrNotChar)
		{
			Cheats::RefillAmmo();
		}
		if (GetAsyncKeyState(VK_F3) & 1) // Toggle Aimbot with F3
		{
			Cheats::ToggleAimbot();
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
	case DLL_PROCESS_DETACH: // Useless but why not
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

void HookSwapChain()
{
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = GetForegroundWindow(); // can be dummy hwnd
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

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

	Sleep(100); // Give some time for the thread to finish

	// Clean up DirectX resources
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