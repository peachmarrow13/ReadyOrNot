#include "pch.h"
#include "Hooks/EngineHooks.h"
#include <atomic>

// Global state variables

bool init = false;
int Frames = 0;
bool SettingsLoaded = false;
bool menu_key_pressed = false;

std::atomic<HMODULE> g_hModule{ nullptr };
std::atomic<int> g_PresentCount{ 0 };
std::atomic<bool> Cleaning{ false };
std::atomic<bool> Resizing{ false };

WNDPROC oWndProc = nullptr;

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