#include "pch.h"
#include "ConfigManager.h"


extern bool SettingsLoaded;

void ConfigManager::SaveSettings()
{
	if (!SettingsLoaded) return;

	// Save MiscSettings (binary)
	std::ofstream MiscSettingsfile("MiscSettings.bin", std::ios::binary);
	if (MiscSettingsfile.is_open())
	{
		MiscSettingsfile.write(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));
		MiscSettingsfile.close();
	}

	// Save AimbotSettings (binary)
	std::ofstream AimbotSettingsfile("AimbotSettings.bin", std::ios::binary);
	if (AimbotSettingsfile.is_open())
	{
		AimbotSettingsfile.write(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));
		AimbotSettingsfile.close();
	}

	// Save ESPSettings (binary)
	std::ofstream ESPSettingsfile("ESPSettings.bin", std::ios::binary);
	if (ESPSettingsfile.is_open())
	{
		ESPSettingsfile.write(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));
		ESPSettingsfile.close();
	}

	// Save SilentAimSettings (binary)
	std::ofstream SilentAimSettingsfile("SilentAimSettings.bin", std::ios::binary);
	if (SilentAimSettingsfile.is_open())
	{
		SilentAimSettingsfile.write(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));
		SilentAimSettingsfile.close();
	}

	// Save TextVars strings
	std::ofstream TextVarsfile("TextVars.bin", std::ios::binary);
	if (TextVarsfile.is_open())
	{
		// Save AimbotBone
		size_t len = TextVars.AimbotBone.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.AimbotBone.data(), len);

		// Save SilentAimBone
		len = TextVars.SilentAimBone.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.SilentAimBone.data(), len);

		len = TextVars.DebugFunctionNameMustInclude.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.DebugFunctionNameMustInclude.data(), len);

		len = TextVars.DebugFunctionObjectMustInclude.size();
		TextVarsfile.write(reinterpret_cast<char*>(&len), sizeof(len));
		TextVarsfile.write(TextVars.DebugFunctionObjectMustInclude.data(), len);

		TextVarsfile.close();
	}

	if (MiscSettings.ShouldSaveCVars)
	{
		std::ofstream CVarsinfile("CVars.bin", std::ios::binary);
		if (CVarsinfile.is_open())
		{
			CVarsinfile.seekp(0);

			CVarsinfile.write(reinterpret_cast<char*>(&CVars), sizeof(CVars));

			CVarsinfile.close();
		}
	}
}

void ConfigManager::LoadSettings()
{
	if (!Settings.ShouldLoad)
		return;

	{
		std::ifstream file("MiscSettings.bin", std::ios::binary);
		if (file.is_open()) {
			file.read(reinterpret_cast<char*>(&MiscSettings), sizeof(MiscSettings));
			file.close();
			// Sync with Localization system
			Localization::CurrentLanguage = MiscSettings.CurrentLanguage;
		}
	}

	{
		std::ifstream file("AimbotSettings.bin", std::ios::binary);
		if (file.is_open()) {
			file.read(reinterpret_cast<char*>(&AimbotSettings), sizeof(AimbotSettings));
			file.close();
		}
	}

	{
		std::ifstream file("ESPSettings.bin", std::ios::binary);
		if (file.is_open()) {
			file.read(reinterpret_cast<char*>(&ESPSettings), sizeof(ESPSettings));
			file.close();
		}
	}

	{
		std::ifstream file("SilentAimSettings.bin", std::ios::binary);
		if (file.is_open()) {
			file.read(reinterpret_cast<char*>(&SilentAimSettings), sizeof(SilentAimSettings));
			file.close();
		}
	}

	{
		std::ifstream file("TextVars.bin", std::ios::binary);
		if (file.is_open()) {
			size_t len;
			if (file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
				TextVars.AimbotBone.resize(len);
				file.read(TextVars.AimbotBone.data(), len);
			}
			if (file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
				TextVars.SilentAimBone.resize(len);
				file.read(TextVars.SilentAimBone.data(), len);
			}
			if (file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
				TextVars.DebugFunctionNameMustInclude.resize(len);
				file.read(TextVars.DebugFunctionNameMustInclude.data(), len);
			}
			if (file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
				TextVars.DebugFunctionObjectMustInclude.resize(len);
				file.read(TextVars.DebugFunctionObjectMustInclude.data(), len);
			}
			file.close();
		}
	}

	if (MiscSettings.ShouldSaveCVars)
	{
		std::ifstream file("CVars.bin", std::ios::binary);
		if (file.is_open()) {
			file.read(reinterpret_cast<char*>(&CVars), sizeof(CVars));
			file.close();
		}
	}

	SettingsLoaded = true;
	std::cout << "[Settings] Configuration loaded successfully.\n";
}
