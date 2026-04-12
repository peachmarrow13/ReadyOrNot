#include "pch.h"
#include "Engine.h"

DXGI_SWAP_CHAIN_DESC Engine::sd = {};
Engine::tPresent Engine::oPresent = nullptr;
LPVOID Engine::PresentAddr = nullptr;
IDXGISwapChain* Engine::pSwapChain = nullptr;
ID3D11Device* Engine::pDevice = nullptr;
ID3D11DeviceContext* Engine::pContext = nullptr;
ID3D11RenderTargetView* Engine::pRenderTargetView = nullptr;
Engine::tResizeBuffers Engine::oResizeBuffers;

bool HookPresentLocal();

bool Engine::HookPresent()
{
	if (!HookPresentLocal())
		return false;

	return true;
}

bool Engine::InitImGui()
{
	HWND hwnd = FindWindow(L"UnrealWindow", nullptr);
	if (!Engine::pDevice || !Engine::pContext) {
		std::cout << "[ERROR] Device or Context is null...\n";
		Sleep(30);
		return false;
	}

	if (!hwnd) {
		std::cout << "[ERROR] HWND is null...\n";
		Sleep(30);
		return false;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (!ImGui::GetCurrentContext()) 
	{
		std::cout << "[ERROR] ImGui::CreateContext failed\n";
		return false;
	}

	// Setup Platform/Renderer backends
	if (!ImGui_ImplWin32_Init(hwnd)) {
		std::cout << "[ERROR] ImGui_ImplWin32_Init failed\n";
		return false;
	}
	if (!ImGui_ImplDX11_Init(Engine::pDevice, Engine::pContext)) {
		std::cout << "[ERROR] ImGui_ImplDX11_Init failed\n";
		ImGui_ImplWin32_Shutdown();
		return false;
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Controller Controls
	io.Fonts->AddFontDefault();
	io.MouseDrawCursor = true;  // Let ImGui draw the cursor

	ImGui::StyleColorsDark();

	if (Engine::pSwapChain) { // Create render target if we have a valid swapchain
		ID3D11Texture2D* pBackBuffer = nullptr;
		HRESULT hr = Engine::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
		if (SUCCEEDED(hr)) {
			hr = Engine::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &Engine::pRenderTargetView);
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
	return true;
}

// Attach Hook
bool Engine::HookResizeBuffers()
{
	//if (!Engine::pSwapChain) return false;

	void** vTable = *reinterpret_cast<void***>(Engine::pSwapChain);
	Engine::oResizeBuffers = (Engine::tResizeBuffers)vTable[13]; // store original

	DWORD oldProtect;
	VirtualProtect(&vTable[13], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
	vTable[13] = (void*)&Engine::hkResizeBuffers; // replace with my hook
	VirtualProtect(&vTable[13], sizeof(void*), oldProtect, &oldProtect);
	return true;
}

bool HookPresentLocal()
{
	WNDCLASSA wc = {};
	wc.lpfnWndProc = DefWindowProcA;
	wc.hInstance = GetModuleHandleA(nullptr);
	wc.lpszClassName = "DummyWindowClass";
	if (!RegisterClassA(&wc))
	{
		if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return false;
	}

	HWND hDummyWnd = CreateWindowA(
		"DummyWindowClass", "Dummy",
		WS_OVERLAPPEDWINDOW,
		0, 0, 1, 1,
		nullptr, nullptr,
		GetModuleHandleA(nullptr),
		nullptr
	);

	if (!hDummyWnd)
		return false;

	ZeroMemory(&Engine::sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	Engine::sd.BufferCount = 1;
	Engine::sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Engine::sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Engine::sd.OutputWindow = hDummyWnd;
	Engine::sd.SampleDesc.Count = 1;
	Engine::sd.SampleDesc.Quality = 0;
	Engine::sd.Windowed = TRUE;
	Engine::sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Engine::sd.BufferDesc.Width = 1;
	Engine::sd.BufferDesc.Height = 1;
	Engine::sd.BufferDesc.RefreshRate.Numerator = 60;
	Engine::sd.BufferDesc.RefreshRate.Denominator = 1;

	if (!Engine::sd.OutputWindow)
	{
		printf("[ERROR] Failed to create dummy window...\n");
		return false;
	}

	D3D_FEATURE_LEVEL FeatureLevel;
	D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;

	if (Engine::pSwapChain)
	{
		void** vTable = *reinterpret_cast<void***>(Engine::pSwapChain);

		if (!vTable)
		{
			printf("[ERROR] Failed to get SwapChain vTable...\n");
			return false;
		}

		Engine::PresentAddr = (LPVOID)vTable[8];
		Engine::oPresent = (Engine::tPresent)vTable[8]; // store original

		MH_CreateHook(Engine::PresentAddr, (LPVOID)&Engine::hkPresent, reinterpret_cast<LPVOID*>(&Engine::oPresent));
		MH_EnableHook(Engine::PresentAddr);

		Engine::pSwapChain->Release();
		if (Engine::pContext) Engine::pContext->Release();
		if (Engine::pDevice) Engine::pDevice->Release();

		return true;
	}
	else
	{
		if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			&FeatureLevelsRequested, 1, D3D11_SDK_VERSION,
			&Engine::sd, &Engine::pSwapChain, &Engine::pDevice, &FeatureLevel, &Engine::pContext)))
		{
			// void** vTable = *reinterpret_cast<void***>(Engine::pSwapChain);
			// Present = vTable[8]
			// ResizeBuffers = vTable[13]

			void** vTable;

			if (Engine::pSwapChain)
				vTable = *reinterpret_cast<void***>(Engine::pSwapChain);
			else
			{
				printf("[ERROR] pSwapChain is null after creation...\n");
				return false;
			}

			if (!vTable)
			{
				printf("[ERROR] Failed to get SwapChain vTable...\n");
				return false;
			}

			Engine::PresentAddr = (LPVOID)vTable[8];
			Engine::oPresent = (Engine::tPresent)vTable[8]; // store original

			MH_CreateHook(Engine::PresentAddr, (LPVOID)&Engine::hkPresent, reinterpret_cast<LPVOID*>(&Engine::oPresent));
			MH_EnableHook(Engine::PresentAddr);

			//DWORD oldProtect;
			//VirtualProtect(&vTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
			//vTable[8] = (void*)&Engine::hkPresent; // replace with my hook
			//VirtualProtect(&vTable[8], sizeof(void*), oldProtect, &oldProtect);

			DestroyWindow(Engine::sd.OutputWindow);

			Engine::pSwapChain->Release();
			Engine::pContext->Release();
			Engine::pDevice->Release();

			return true;
		}
		else
		{
			printf("[ERROR] D3D11CreateDeviceAndSwapChain failed...\n");
			if (Engine::sd.OutputWindow) DestroyWindow(Engine::sd.OutputWindow);

			if (Engine::pSwapChain) Engine::pSwapChain->Release();
			if (Engine::pContext) Engine::pContext->Release();
			if (Engine::pDevice) Engine::pDevice->Release();

			return false;
		}
	}

	return false;
}
