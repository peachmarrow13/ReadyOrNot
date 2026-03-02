#pragma once
#include <windows.h>

DWORD MainThread(HMODULE hModule);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
