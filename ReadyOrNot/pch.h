#ifndef PCH_H
#define PCH_H

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>
#include <fstream>
#include <unordered_map>
#include <numbers>

// SDK Headers
#include "SDK/ReadyOrNot_parameters.hpp"
#include "SDK/ReadyOrNot_classes.hpp"
#include "SDK/Engine_classes.hpp"
#include "SDK/CoreUObject_classes.hpp"
#include "SDK/CoreUObject_parameters.hpp"
#include "SDK/Engine_parameters.hpp"
#include "SDK/Basic.hpp"

// ImGui Headers
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <imgui_internal.h>

#endif //PCH_H
