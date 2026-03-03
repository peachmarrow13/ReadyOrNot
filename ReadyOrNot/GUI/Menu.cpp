#include "pch.h"
#include "Menu.h"

#include "Config/ConfigManager.h"
#include "Utils/Localization.h"

#define MAJORVERSION 2
#define MINORVERSION 4
#define PATCHVERSION 3

namespace GUI
{
    bool ShowMenu = true;
    ImGuiKey TriggerBotKey = ImGuiKey_None;
    ImGuiKey ESPKey = ImGuiKey_None;
    ImGuiKey AimButton = ImGuiKey_None;
}

static const std::pair<const char*, std::string> BoneOptions[] = {
	{"Head", BoneList.HeadBone},
	{"Neck", BoneList.NeckBone},
	{"Chest", BoneList.ChestBone},
	{"Stomach", BoneList.StomachBone},
	{"Pelvis", BoneList.PelvisBone},
	{"Left Shoulder", BoneList.LeftShoulderBone},
	{"Left Elbow", BoneList.LeftElbowBone},
	{"Left Hand", BoneList.LeftHandBone},
	{"Right Shoulder", BoneList.RightShoulderBone},
	{"Right Elbow", BoneList.RightElbowBone},
	{"Right Hand", BoneList.RightHandBone}
};

void GUI::AddDefaultTooltip(const char* desc)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void GUI::HostOnlyTooltip()
{
	AddDefaultTooltip(Localization::T("HOST_ONLY_IF_YOU_ARE_NOT_THE_HOST_THIS_FEATURE_WILL_NOT_FUNCTION"));
}


void GUI::RenderMenu()
{
	if (!ShowMenu) return;

	ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin(Localization::T("MENU_TITLE"), nullptr, ImGuiWindowFlags_NoCollapse);

	ImGui::SeparatorText("Hello, Have Fun Cheating!");

	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem(Localization::T("TAB_ABOUT")))
		{
			static const char* HostLangs[] = { "English", "简体中文" };
			int CurrentLangIdx = (int)MiscSettings.CurrentLanguage;
			if (ImGui::Combo(Localization::T("LANGUAGE"), &CurrentLangIdx, HostLangs, IM_ARRAYSIZE(HostLangs)))
			{
				MiscSettings.CurrentLanguage = (Language)CurrentLangIdx;
				Localization::CurrentLanguage = MiscSettings.CurrentLanguage;
			}

			ImGui::Separator();

			ImGui::Text(Localization::T("READY_OR_NOT_FREE_CHEAT_PEACHMARROW12"));
			ImGui::Text(Localization::T("VERSION_D_D_D"), MAJORVERSION, MINORVERSION, PATCHVERSION);

			if (GVars.PlayerController && GVars.PlayerController->PlayerState)
			{
				APlayerState* PlayerState = GVars.PlayerController->PlayerState;
				std::string PlayerName = PlayerState->GetPlayerName().ToString();
				ImGui::Text(Localization::T("THANKS_FOR_USING_THIS_CHEAT_S"), PlayerName.c_str());
			}
			else
			{
				ImGui::Text(Localization::T("USERNAME_NOT_FOUND_BUT_THANKS_FOR_USING_ANYWAY"));
			}

			if (GVars.PlayerController)
			{
				if (GVars.PlayerController->HasAuthority())
					ImGui::TextColored(ImVec4(0, 1, 0, 1), Localization::T("CURRENT_AUTH_HOST_AUTHORITY"));
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), Localization::T("CURRENT_AUTH_CLIENT"));
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), Localization::T("CURRENT_AUTH_WAITING_FOR_CONTROLLER"));
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(Localization::T("TAB_PLAYER")))
		{
			if (ImGui::Checkbox(Localization::T("GODMODE"), &CVars.GodMode))
				Cheats::ToggleGodMode();
			HostOnlyTooltip();

			ImGui::Checkbox(Localization::T("AIMBOT"), &CVars.Aimbot);

			ImGui::Checkbox(Localization::T("SILENT_AIM"), &CVars.SilentAim);
			AddDefaultTooltip(Localization::T("NOT_PERFECT_YET_USE_WITH_CAUTION"));
			HostOnlyTooltip();

			ImGui::Checkbox(Localization::T("ESP"), &CVars.ESP);

			ImGui::SliderFloat(Localization::T("SPEED"), &CVars.Speed, 1, 30, "%.1f");
			ImGui::SameLine();
			ImGui::Checkbox(Localization::T("ENABLE_SPEED"), &CVars.SpeedEnabled);

			if (ImGui::SliderFloat(Localization::T("FOV"), &CVars.FOV, 0.1f, 179.9f))
			{
				Cheats::ChangeFOV();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(Localization::T("WEAPON")))
		{
			if (ImGui::Checkbox(Localization::T("INFINITE_AMMO"), &CVars.InfAmmo))
				Cheats::ToggleInfAmmo();
			HostOnlyTooltip();

			if (ImGui::Button(Localization::T("REMOVE_RECOIL")))
				Cheats::RemoveRecoil();

			if (ImGui::Button(Localization::T("REMOVE_SPREAD")))
				Cheats::RemoveSpread();

			if (ImGui::Button(Localization::T("ADD_FULLAUTO")))
				Cheats::AddAutoFire();

			if (ImGui::Button(Localization::T("ADD_PENETRATION")))
				Cheats::PenetrateWalls();

			if (ImGui::Button(Localization::T("INSTAKILL")))
				Cheats::InstaKill();

			if (ImGui::Button(Localization::T("INCREASE_FIRERATE")))
				Cheats::SetFireRate(0.001f);

			ImGui::Checkbox(Localization::T("SHOOT_FROM_RETICLE"), &CVars.ShootFromReticle);

			if (ImGui::Button(Localization::T("ADD_MAGAZINE")))
				Cheats::AddMag();

			ImGui::Checkbox(Localization::T("TRIGGERBOT"), &CVars.TriggerBot);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(Localization::T("WORLD")))
		{
			if (ImGui::Button(Localization::T("KILL_ALL_SUSPECTS")))
				Cheats::KillAll(ETeam::TEAM_SUSPECT);

			ImGui::SameLine();

			if (ImGui::Button(Localization::T("SURRENDER_ALL_SUSPECTS")))
				Cheats::SurrenderAll(ETeam::TEAM_SUSPECT);

			ImGui::SameLine();

			if (ImGui::Button(Localization::T("ARREST_ALL_SUSPECTS")))
				Cheats::ArrestAll(ETeam::TEAM_SUSPECT);
			HostOnlyTooltip();

			if (ImGui::Button(Localization::T("KILL_ALL_CIVILIANS")))
				Cheats::KillAll(ETeam::TEAM_CIVILIAN);

			ImGui::SameLine();

			if (ImGui::Button(Localization::T("SURRENDER_ALL_CIVILIANS")))
				Cheats::SurrenderAll(ETeam::TEAM_CIVILIAN);

			ImGui::SameLine();

			if (ImGui::Button(Localization::T("ARREST_ALL_CIVILIANS")))
				Cheats::ArrestAll(ETeam::TEAM_CIVILIAN);
			HostOnlyTooltip();

			if (ImGui::Button(Localization::T("COLLECT_ALL_EVIDENCE")))
				Cheats::GetAllEvidence();

			if (ImGui::Button(Localization::T("AUTO_WIN")))
				Cheats::AutoWin();

			if (ImGui::Button(Localization::T("UNLOCK_ALL_DOORS")))
				Cheats::UnlockDoors();

			ImGui::Checkbox(Localization::T("BULLET_TIME"), &CVars.BulletTime);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(Localization::T("MISC")))
		{
			if (ImGui::Button(Localization::T("SAVE_SETTINGS")))
				ConfigManager::SaveSettings();
			ImGui::SameLine();

			if (ImGui::Button(Localization::T("LOAD_SETTINGS")))
				ConfigManager::LoadSettings();
			AddDefaultTooltip(Localization::T("THIS_ONLY_SAVES_LOADS_CONFIGURATION_NOT_WHICH_FEATURES_ARE_ENABLED"));

			ImGui::Checkbox(Localization::T("DEBUG"), &CVars.Debug);
			AddDefaultTooltip(Localization::T("ENABLE_DEBUG_OPTIONS_USED_FOR_DEVELOPMENT_AND_BUG_FINDING_PROBABLY_USELESS_FOR_YOU"));

			ImGui::InputText(Localization::T("DEBUG_FUNC_CONTAINS"), &TextVars.DebugFunctionNameMustInclude);
			ImGui::InputText(Localization::T("DEBUG_OBJ_CONTAINS"), &TextVars.DebugFunctionObjectMustInclude);

			if (CVars.Debug)
				if (ImGui::Button(Localization::T("PRINT_ACTORS")))
					Utils::PrintActors(nullptr);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(Localization::T("CONFIG")))
		{
			if (ImGui::TreeNode(Localization::T("AIMBOT_SETTINGS")))
			{
				ImGui::SliderFloat(Localization::T("AIMBOT_FOV"), &AimbotSettings.MaxFOV, 0.01f, 180.0f, "%.1f");

				ImGui::Checkbox(Localization::T("REQUIRE_LOS"), &AimbotSettings.LOS);
				AddDefaultTooltip(Localization::T("TARGET_MUST_BE_VISIBLE_REQUIRES_LINE_OF_SIGHT_CHECK"));

				ImGui::Checkbox(Localization::T("TARGET_CIVILIANS"), &AimbotSettings.TargetCivilians);

				ImGui::Checkbox(Localization::T("TARGET_DEAD"), &AimbotSettings.TargetDead);

				ImGui::Checkbox(Localization::T("TARGET_ARRESTED"), &AimbotSettings.TargetArrested);

				ImGui::Checkbox(Localization::T("TARGET_ALL"), &AimbotSettings.TargetAll);

				ImGui::SliderFloat(Localization::T("MAX_DISTANCE"), &AimbotSettings.MaxDistance, 0.0f, 300.0f, "%.1f");

				ImGui::SliderFloat(Localization::T("MIN_DISTANCE"), &AimbotSettings.MinDistance, 0.0f, 100.0f, "%.1f");

				ImGui::Checkbox(Localization::T("SMOOTH_AIM"), &AimbotSettings.Smooth);

				ImGui::SliderFloat(Localization::T("SMOOTHING"), &AimbotSettings.SmoothingVector, 1.0f, 20.0f, "%.2f");

				ImGui::Checkbox(Localization::T("DRAW_ARROW"), &AimbotSettings.DrawArrow);

				ImGui::Checkbox(Localization::T("DRAW_FOV"), &AimbotSettings.DrawFOV);

				if (ImGui::BeginCombo(Localization::T("TARGET_BONE"), TextVars.AimbotBone.c_str()))
				{
					for (int i = 0; i < IM_ARRAYSIZE(BoneOptions); i++)
					{
						bool is_selected = (TextVars.AimbotBone == BoneOptions[i].second);
						if (ImGui::Selectable(BoneOptions[i].first, is_selected))
						{
							TextVars.AimbotBone = BoneOptions[i].second;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus(); // make the selected item visible
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox(Localization::T("REQUIRE_KEY_HELD"), &AimbotSettings.RequireKeyHeld);

				const char* ABpreview = ImGui::GetKeyName(AimbotSettings.AimbotKey);
				if (!ABpreview) ABpreview = "None";

				if (ImGui::BeginCombo(Localization::T("AIMBOT_KEY"), ABpreview))
				{
					for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
					{
						ImGuiKey current = static_cast<ImGuiKey>(key);
						const char* keyName = ImGui::GetKeyName(current);
						if (!keyName || !*keyName) continue; // skip empty names

						bool isSelected = (AimbotSettings.AimbotKey == current);
						if (ImGui::Selectable(keyName, isSelected))
							AimbotSettings.AimbotKey = current;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
					AddDefaultTooltip(Localization::T("AIMBOT_ONLY_ACTIVATES_WHILE_HOLDING_THIS_KEY"));
				}

				ImGui::Checkbox(Localization::T("EXCLUDE_TARGET_SUSPECTS"), &AimbotSettings.ExcludeTargetSuspects);
				AddDefaultTooltip(Localization::T("REGULAR_AIMBOT_WILL_NOT_TARGET_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED"));

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(Localization::T("ESP_SETTINGS")))
			{
				ImGui::Checkbox(Localization::T("SHOW_TEAM"), &ESPSettings.ShowTeam);

				ImGui::Checkbox(Localization::T("SHOW_BOX"), &ESPSettings.ShowBox);

				ImGui::Checkbox(Localization::T("SHOW_TRAPS"), &ESPSettings.ShowTraps);

				ImGui::Checkbox(Localization::T("SHOW_DISTANCE"), &ESPSettings.ShowEnemyDistance);

				ImGui::Checkbox(Localization::T("SHOW_NAME"), &ESPSettings.ShowEnemyName);

				ImGui::Checkbox(Localization::T("SHOW_BONES"), &ESPSettings.Bones);

				ImGui::Checkbox(Localization::T("BULLET_TRACERS"), &ESPSettings.BulletTracers);
				
				ImGui::Checkbox(Localization::T("RAINBOW_TRACERS"), &ESPSettings.TracerRainbow);

				ImGui::SliderFloat(Localization::T("TRACER_DURATION"), &ESPSettings.TracerDuration, 0.5f, 10.0f, "%.1f");

				ImGui::ColorEdit4(Localization::T("TRACER_COLOR"), (float*)&ESPSettings.TracerColor);

				ImGui::ColorEdit4(Localization::T("SUSPECT_COLOR"), (float*)&ESPSettings.SuspectColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("CIVILIAN_COLOR"), (float*)&ESPSettings.CivilianColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("DEAD_COLOR"), (float*)&ESPSettings.DeadColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("TEAM_COLOR"), (float*)&ESPSettings.TeamColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("ARRESTED_COLOR"), (float*)&ESPSettings.ArrestColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("TARGET_SUSPECT_COLOR"), (float*)&ESPSettings.TargetSuspectColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("ACTIVE_OBJECTIVE_COLOR"), (float*)&ESPSettings.ObjectiveActiveColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4(Localization::T("COMPLETED_OBJECTIVE_COLOR"), (float*)&ESPSettings.ObjectiveCompletedColor, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox(Localization::T("ONLY_SHOW_VISIBLE_LOS"), &ESPSettings.LOS);

				if (ImGui::SliderFloat(Localization::T("BONE_OPACITY"), &ESPSettings.BoneOpacity, 0.0f, 1.0f, "%.2f"))
				{
					ESPSettings.SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, ESPSettings.BoneOpacity);
					ESPSettings.DeadColor = ImVec4(0.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.TeamColor = ImVec4(0.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
				}

				ImGui::Checkbox(Localization::T("SHOW_OBJECTIVES"), &ESPSettings.ShowObjectives);
				AddDefaultTooltip(Localization::T("OBJECTIVES_DO_NOT_SHOW_ACTUAL_PHYSICAL_LOCATIONS"));
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(Localization::T("SILENT_AIM_SETTINGS")))
			{
				ImGui::Checkbox(Localization::T("TARGET_CIVILIANS"), &SilentAimSettings.TargetCivilians);

				ImGui::Checkbox(Localization::T("TARGET_ALL"), &SilentAimSettings.TargetAll);

				ImGui::Checkbox(Localization::T("TARGET_DEAD"), &SilentAimSettings.TargetDead);

				ImGui::Checkbox(Localization::T("TARGET_SURRENDERED"), &SilentAimSettings.TargetSurrendered);

				ImGui::Checkbox(Localization::T("TARGET_ARRESTED"), &SilentAimSettings.TargetArrested);

				ImGui::SliderFloat(Localization::T("SILENT_AIM_FOV"), &SilentAimSettings.MaxFOV, 0, 180.0f, "%.1f");

				ImGui::Checkbox(Localization::T("DRAW_FOV"), &SilentAimSettings.DrawFOV);

				ImGui::SliderFloat(Localization::T("FOV_THICKNESS"), &SilentAimSettings.FOVThickness, 0.1f, 10.0f, "%.2f");

				ImGui::Checkbox(Localization::T("DRAW_ARROW"), &SilentAimSettings.DrawArrow);

				ImGui::SliderFloat(Localization::T("ARROW_THICKNESS"), &SilentAimSettings.ArrowThickness, 0.1f, 10.0f, "%.2f");

				ImGui::Checkbox(Localization::T("REQUIRE_LOS"), &SilentAimSettings.RequiresLOS);

				ImGui::SliderFloat(Localization::T("HIT_CHANCE"), &SilentAimSettings.HitChance, 0.0f, 100, "%.1f");

				if (ImGui::BeginCombo(Localization::T("TARGET_BONE"), TextVars.SilentAimBone.c_str()))
				{
					for (int i = 0; i < IM_ARRAYSIZE(BoneOptions); i++)
					{
						bool is_selected = (TextVars.SilentAimBone == BoneOptions[i].second);
						if (ImGui::Selectable(BoneOptions[i].first, is_selected))
						{
							TextVars.SilentAimBone = BoneOptions[i].second;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus(); // make the selected item visible
					}
					ImGui::EndCombo();
				}
				ImGui::Checkbox(Localization::T("MAGIC_BULLET"), &SilentAimSettings.MagicBullet);
				AddDefaultTooltip(Localization::T("BULLETS_SPAWN_DIRECTLY_ON_TARGETS_RECOMMENDED_FOR_HOST_ONLY_MAY_BE_UNSTABLE_OR_NON_FUNCTIONAL_FOR_CLIENTS"));
				HostOnlyTooltip();

				ImGui::Checkbox(Localization::T("EXCLUDE_TARGET_SUSPECTS"), &SilentAimSettings.ExcludeTargetSuspects);
				AddDefaultTooltip(Localization::T("SILENT_AIM_WILL_NOT_TARGET_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED"));

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(Localization::T("MISC_SETTINGS")))
			{
				ImGui::SeparatorText(Localization::T("RETICLE_SETTINGS"));

				ImGui::Checkbox(Localization::T("RETICLE"), &CVars.Reticle);

				ImGui::ColorEdit4(Localization::T("RETICLE_COLOR"), (float*)&MiscSettings.ReticleColor);

				ImGui::DragFloat2(Localization::T("RETICLE_POSITION"), (float*)&MiscSettings.ReticlePosition, 1, -100, 100);

				ImGui::SliderFloat(Localization::T("RETICLE_SIZE"), &MiscSettings.ReticleSize, 1, 15);

				ImGui::Checkbox(Localization::T("CROSS_RETICLE"), &MiscSettings.CrossReticle);

				ImGui::Checkbox(Localization::T("ONLY_SHOW_WHEN_THROWING"), &MiscSettings.ReticleWhenThrowing);

				ImGui::SeparatorText(Localization::T("TRIGGERBOT_SETTINGS"));

				ImGui::Checkbox(Localization::T("TARGET_CIVILIANS"), &MiscSettings.TriggerBotTargetsCivilians);

				ImGui::Checkbox(Localization::T("USE_SILENT_AIM"), &MiscSettings.TriggerBotUsesSilentAim);
				AddDefaultTooltip(Localization::T("ENSURES_HITS_ON_TARGETS"));

				ImGui::Checkbox(Localization::T("EXCLUDE_TARGET_SUSPECTS"), &MiscSettings.TriggerBotExcludeTargetSuspects);
				AddDefaultTooltip(Localization::T("TRIGGERBOT_WILL_NOT_SHOOT_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED"));

				ImGui::SeparatorText(Localization::T("MISC"));

				ImGui::Checkbox(Localization::T("DRAW_ACTIVE_FEATURES"), &CVars.RenderOptions);

				ImGui::Checkbox(Localization::T("SHOW_PLAYER_LIST"), &CVars.ListPlayers);

				ImGui::Checkbox(Localization::T("AUTO_SAVE_SETTINGS"), &MiscSettings.ShouldAutoSave);
				ImGui::SameLine();
				ImGui::Checkbox(Localization::T("SAVE_ACTIVE_FEATURES_STATE"), &MiscSettings.ShouldSaveCVars);

				ImGui::SeparatorText(Localization::T("KEYBINDS"));

				// Create a combo box
				const char* TBpreview = ImGui::GetKeyName(TriggerBotKey);
				if (!TBpreview) TBpreview = "None";

				if (ImGui::BeginCombo(Localization::T("TRIGGERBOT_KEY"), TBpreview))
				{
					for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
					{
						ImGuiKey current = static_cast<ImGuiKey>(key);
						const char* keyName = ImGui::GetKeyName(current);
						if (!keyName || !*keyName) continue; // skip empty names

						bool isSelected = (TriggerBotKey == current);
						if (ImGui::Selectable(keyName, isSelected))
							TriggerBotKey = current;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				const char* ESPpreview = ImGui::GetKeyName(ESPKey);
				if (!ESPpreview) ESPpreview = "None";

				if (ImGui::BeginCombo(Localization::T("ESP_KEY"), ESPpreview))
				{
					for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
					{
						ImGuiKey current = static_cast<ImGuiKey>(key);
						const char* keyName = ImGui::GetKeyName(current);
						if (!keyName || !*keyName) continue; // skip empty names

						bool isSelected = (ESPKey == current);
						if (ImGui::Selectable(keyName, isSelected))
							ESPKey = current;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				const char* AimPreview = ImGui::GetKeyName(AimButton);
				if (!AimPreview) AimPreview = "None";

				if (ImGui::BeginCombo(Localization::T("AIM_KEY"), AimPreview))
				{
					for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
					{
						ImGuiKey current = static_cast<ImGuiKey>(key);
						const char* keyName = ImGui::GetKeyName(current);
						if (!keyName || !*keyName) continue; // skip empty names

						bool isSelected = (AimButton == current);
						if (ImGui::Selectable(keyName, isSelected))
							AimButton = current;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::TreePop();
			}


			ImGui::EndTabItem();
		}

		if (CVars.SecretFeatures)
		{
			if (ImGui::BeginTabItem(Localization::T("SECRET_FEATURES")))
			{
				ImGui::Text(Localization::T("THIS_OPTION_IS_FOR_DEVELOPMENT_ONLY"));
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::Separator();
	ImGui::Text(Localization::T("YOU_CAN_FIND_ME_ON_UNKNOWNCHEATS_ME_ID_PEACHMARROW13"));
	ImGui::SameLine();
	ImGui::Text(Localization::T("THIS_CHEAT_IS_RELEASED_FOR_FREE_ON_UNKNOWNCHEATS_ME_DO_NOT_DOWNLOAD_FROM_ELSEWHERE"));
	ImGui::End();
}
