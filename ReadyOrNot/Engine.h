#pragma once

#include "pch.h"

#include "minhook/include/MinHook.h"
#include "Cheats.h"
#include "Utils/Utils.h"

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