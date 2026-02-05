#pragma once

//#pragma pack(push, 8)
#include "SDK/ReadyOrNot_parameters.hpp"
#include "SDK/ReadyOrNot_classes.hpp"
#include "SDK/Engine_classes.hpp"
#include "SDK/CoreUObject_classes.hpp"
#include "SDK/CoreUObject_parameters.hpp"
#include "SDK/Engine_parameters.hpp"
#include "SDK/Basic.hpp"
//#pragma pack(pop)

#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include <atomic>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <imgui_internal.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <numbers>

#include "minhook/include/MinHook.h"
#include "Cheats.h"
#include "Utils.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

struct Engine
{
	static bool HookPresent();
	static bool HookResizeBuffers();
	static bool InitImGui(HWND hwnd);
	typedef HRESULT(__stdcall* tPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static tPresent oPresent;
	static LPVOID PresentAddr;
	static IDXGISwapChain* pSwapChain;
	static ID3D11Device* pDevice;
	static ID3D11DeviceContext* pContext;
	static ID3D11RenderTargetView* pRenderTargetView;
	static DXGI_SWAP_CHAIN_DESC sd;
	// Function pointer type for IDXGISwapChain::ResizeBuffers
	typedef HRESULT(__stdcall* tResizeBuffers)(
		IDXGISwapChain* pSwapChain,
		UINT BufferCount,
		UINT Width,
		UINT Height,
		DXGI_FORMAT NewFormat,
		UINT SwapChainFlags
		);

	// Original pointer
	static tResizeBuffers oResizeBuffers;

	static HRESULT __stdcall hkPresent(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
};

struct Hooks
{
	static void** vTable;
	static bool HookProcessEvent();
};