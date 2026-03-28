# Ready or Not Cheat

https://discord.gg/YkWmxdhyJ7

> **⚠️ Disclaimer:** Do not grief or ruin the experience for others.

---

## Table of Contents
- [Features](#features)
- [Quick Start](#quick-start)
- [Known Issues](#known-issues)
- [Build Instructions](#build-instructions)
- [Troubleshooting](#troubleshooting)
- [Credits](#credits)
- [License](#license)

---

## Quick Start

### Prerequisites
- Ready or Not game installed
- DirectX 11 mode enabled (not DX12)
- Game running in **Windowed** or **Borderless** mode (not fullscreen)
- A DLL injector (most standard injectors work)

### Setup Instructions

1. **Configure the game:**
   - Go to Ready or Not game properties
   - Set launch option to DirectX 11
   - Ensure the game is NOT running in fullscreen mode

2. **Injection process:**
   - Launch Ready or Not
   - Wait until you reach the main menu screen
   - Use your injector to inject the DLL into `ReadyOrNotSteam-Win64-Shipping.exe`
   - Press `Insert` to open/close the cheat menu
   - Press `End` to uninject the DLL

---
## Features

### Core Controls
- **Insert Key** - Toggle cheat menu on/off
- **End Key** - Uninject the DLL from the game

### Combat Features
- **Aimbot**
  - Snap lines and visual targeting
  - Line of Sight (LOS) checks
  - Field of View (FOV) adjustments
  - Configurable max/min distance
  - Smoothing for natural appearance
  - Target bone selection
  - Targeting filters (civilians, dead, arrested, etc.)

- **Silent Aim**
  - Configurable hit chance
  - FOV and LOS checks
  - Target bone selection
  - Snap line visualization
  - Advanced targeting options
  - Shoot from reticle

- **TriggerBot**
  - LOS verification
  - Silent aim integration
  - Customizable targeting filters

### Visual Features
- **ESP**
  - Team indicators
  - Bounding boxes
  - Skeleton visualization
  - Trap detection
  - Distance display
  - Customizable bone colors and opacity
  - Line of sight indicators
  - Objective ESP

- **Custom Reticle**
  - Cross or dot styles
  - Adjustable size and color
  - Position customization
  - Optional "show only when throwing" mode

### Weapon Enhancements
- **No Recoil** - Eliminate weapon kickback
- **No Spread** - Perfect bullet accuracy
- **Auto Fire** - Add automatic fire mode to any weapon
- **Fire Rate Control** - Customize weapon firing speed
- **Infinite Ammo** - Unlimited ammunition
- **Infinite Bullet Penetration** - Shoot through any material
- **Infinite Bullet Damage** - Maximum damage output
- **Add Magazine** - Instant magazine addition

### Player Enhancements
- **God Mode** - Invulnerability
- **Speed Cheat** - Enhanced movement speed

### Mission Utilities
- **Auto Win** - Automatically complete missions
- **Unlock All Doors** - Open any locked door instantly
- **Arrest All** - Arrest all civilians or suspects
- **Kill All** - Eliminate all civilians or suspects (for specific scenarios)

### Multiplayer Utilities
- **Player List**
  - Grant God Mode to players
  - Grant Infinite Ammo to players
  - Teleport players to your location
  - Grant Increased Movement Speed

### Configuration
- **Auto Save/Load** - Automatically save and restore settings
- **Manual Save/Load** - Save and load configurations on demand
- **Show Enabled Options** - Display active features

---
## Known Issues

- **Mission Retry Bug:** Clicking "Try Again" after completing a mission may cause an infinite loading state. Exit to menu instead, then restart the mission.

> **Tip:** If you encounter any of these issues, uninject the DLL (press `End`), restart the game, and reinject.

---

## Build Instructions

This project is written in C++ and requires Visual Studio and Windows to build.

### Requirements
- **Visual Studio 2022 (or higher)**
- **Windows SDK** (latest version)
- **DirectX 11 SDK**
- **ImGui** (already included in the repository)
- **MinHook** (already included in the repository)

### Building the Project

1. Clone this repository to your local machine
2. Open `ReadyOrNot.sln` in Visual Studio
3. Select the `Release` configuration from the dropdown
4. Build the solution
5. The compiled DLL will be located in the `x64/Release` folder

### Build Notes
- Always build in `Release` mode
- The DLL is designed to be injected into Ready or Not while running in DirectX 11 mode
- Injection should occur at or after the main menu screen

---

## Troubleshooting

### Game Crashes on Injection
- Ensure you're running the game in DirectX 11 mode (not DX12)
- Verify the game is in Windowed or Borderless mode (not fullscreen)
- Make sure you're injecting at the main menu, not during loading

### Cheat Menu Won't Open
- Press `Insert` key to toggle the menu
- Ensure the DLL was successfully injected (check your injector's logs)
- Try reinjecting the DLL

### Features Not Working
- Some features may only work in specific game modes
- Verify you're using the latest version of the cheat

### Build Errors
- Ensure you have all required dependencies installed
- Try cleaning the solution and rebuilding (`Build → Clean Solution`, then rebuild)
- Verify your Visual Studio installation includes C++ development tools

**Only contact me if you have tried troubleshooting yourself.**

---

## Credits

This project uses the following open-source libraries and tools:

- **[ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI library
- **[MinHook](https://github.com/TsudaKageyu/minhook)** - Minimalistic x86/x64 API hooking library
- **[Dumper-7](https://github.com/Encryqed/Dumper-7)** - Unreal Engine SDK generator

---
## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for complete details.
