#include "pch.h"
#include "Menu.h"

#include "Config/ConfigManager.h"

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
	AddDefaultTooltip((const char*)u8"仅限房主 (Host Only) - 如果你不是房主，此功能将不起作用。");
}


void GUI::RenderMenu()
{
	if (!ShowMenu) return;

	ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin((const char*)u8"Ready or Not 免费辅助 - PeachMarrow12", nullptr, ImGuiWindowFlags_NoCollapse);

	ImGui::SeparatorText("Hello, Have Fun Cheating!");

	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem((const char*)u8"关于"))
		{
			ImGui::Text((const char*)u8"Ready or Not 免费辅助 - PeachMarrow12");
			ImGui::Text((const char*)u8"版本 %d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);

			if (GVars.PlayerController && GVars.PlayerController->PlayerState)
			{
				APlayerState* PlayerState = GVars.PlayerController->PlayerState;
				std::string PlayerName = PlayerState->GetPlayerName().ToString();
				ImGui::Text((const char*)u8"感谢使用此辅助, %s!", PlayerName.c_str());
			}
			else
			{
				ImGui::Text((const char*)u8"未找到用户名，但仍感谢使用！");
			}

			if (GVars.PlayerController)
			{
				if (GVars.PlayerController->HasAuthority())
					ImGui::TextColored(ImVec4(0, 1, 0, 1), (const char*)u8"当前权限: 主机 (Authority/Host)");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), (const char*)u8"当前权限: 客户机 (Client)");
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), (const char*)u8"当前权限: 等待控制器初始化...");
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem((const char*)u8"玩家"))
		{
			if (ImGui::Checkbox((const char*)u8"无敌模式 (GodMode)", &CVars.GodMode))
				Cheats::ToggleGodMode();
			HostOnlyTooltip();

			ImGui::Checkbox((const char*)u8"自瞄 (Aimbot)", &CVars.Aimbot);

			ImGui::Checkbox((const char*)u8"静默自瞄 (Silent Aim)", &CVars.SilentAim);
			AddDefaultTooltip((const char*)u8"尚不完善，请谨慎使用。");
			HostOnlyTooltip();

			ImGui::Checkbox((const char*)u8"透视 (ESP)", &CVars.ESP);

			ImGui::SliderFloat((const char*)u8"移动速度", &CVars.Speed, 1, 30, "%.1f");
			ImGui::SameLine();
			ImGui::Checkbox((const char*)u8"开启速度修改", &CVars.SpeedEnabled);

			if (ImGui::SliderFloat((const char*)u8"视野 (FOV)", &CVars.FOV, 0.1f, 179.9f))
			{
				Cheats::ChangeFOV();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem((const char*)u8"武器"))
		{
			if (ImGui::Checkbox((const char*)u8"无限弹药", &CVars.InfAmmo))
				Cheats::ToggleInfAmmo();
			HostOnlyTooltip();

			if (ImGui::Button((const char*)u8"移除后坐力"))
				Cheats::RemoveRecoil();

			if (ImGui::Button((const char*)u8"移除扩散"))
				Cheats::RemoveSpread();

			if (ImGui::Button((const char*)u8"添加全自动"))
				Cheats::AddAutoFire();

			if (ImGui::Button((const char*)u8"添加穿墙"))
				Cheats::PenetrateWalls();

			if (ImGui::Button((const char*)u8"秒杀"))
				Cheats::InstaKill();

			if (ImGui::Button((const char*)u8"增加射速"))
				Cheats::SetFireRate(0.001f);

			ImGui::Checkbox((const char*)u8"准星处射击", &CVars.ShootFromReticle);

			if (ImGui::Button((const char*)u8"添加弹匣"))
				Cheats::AddMag();

			ImGui::Checkbox((const char*)u8"自动射击 (TriggerBot)", &CVars.TriggerBot);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem((const char*)u8"世界"))
		{
			if (ImGui::Button((const char*)u8"杀死所有嫌疑人"))
				Cheats::KillAll(ETeam::TEAM_SUSPECT);

			ImGui::SameLine();

			if (ImGui::Button((const char*)u8"降伏所有嫌疑人"))
				Cheats::SurrenderAll(ETeam::TEAM_SUSPECT);

			ImGui::SameLine();

			if (ImGui::Button((const char*)u8"逮捕所有嫌疑人"))
				Cheats::ArrestAll(ETeam::TEAM_SUSPECT);
			HostOnlyTooltip();

			if (ImGui::Button((const char*)u8"杀死所有平民"))
				Cheats::KillAll(ETeam::TEAM_CIVILIAN);

			ImGui::SameLine();

			if (ImGui::Button((const char*)u8"降伏所有平民"))
				Cheats::SurrenderAll(ETeam::TEAM_CIVILIAN);

			ImGui::SameLine();

			if (ImGui::Button((const char*)u8"逮捕所有平民"))
				Cheats::ArrestAll(ETeam::TEAM_CIVILIAN);
			HostOnlyTooltip();

			if (ImGui::Button((const char*)u8"收集所有证据"))
				Cheats::GetAllEvidence();

			if (ImGui::Button((const char*)u8"自动获胜"))
				Cheats::AutoWin();

			if (ImGui::Button((const char*)u8"解锁所有门"))
				Cheats::UnlockDoors();

			ImGui::Checkbox((const char*)u8"子弹时间", &CVars.BulletTime);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem((const char*)u8"杂项"))
		{
			if (ImGui::Button((const char*)u8"保存设置"))
				ConfigManager::SaveSettings();
			ImGui::SameLine();

			if (ImGui::Button((const char*)u8"加载设置"))
				ConfigManager::LoadSettings();
			AddDefaultTooltip((const char*)u8"这些只保存和加载配置，而不是哪些功能已启用。");

			ImGui::Checkbox((const char*)u8"调试 (Debug)", &CVars.Debug);
			AddDefaultTooltip((const char*)u8"这只是开启了一些我用来查找错误和有用信息的菜单选项。对你来说可能没用。");

			ImGui::InputText((const char*)u8"调试函数名包含", &TextVars.DebugFunctionNameMustInclude);
			ImGui::InputText((const char*)u8"调试对象名包含", &TextVars.DebugFunctionObjectMustInclude);

			if (CVars.Debug)
				if (ImGui::Button((const char*)u8"打印 Actor"))
					Utils::PrintActors(nullptr);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem((const char*)u8"配置"))
		{
			if (ImGui::TreeNode((const char*)u8"自瞄设置 (Aimbot Settings)"))
			{
				ImGui::SliderFloat((const char*)u8"自瞄视野 (Aimbot FOV)", &AimbotSettings.MaxFOV, 0.01f, 180.0f, "%.1f");

				ImGui::Checkbox((const char*)u8"需要可见性 (LOS)", &AimbotSettings.LOS);
				AddDefaultTooltip((const char*)u8"目标必须可见；需要视线检查。");

				ImGui::Checkbox((const char*)u8"目标平民", &AimbotSettings.TargetCivilians);

				ImGui::Checkbox((const char*)u8"目标已死亡单位", &AimbotSettings.TargetDead);

				ImGui::Checkbox((const char*)u8"目标已逮捕单位", &AimbotSettings.TargetArrested);

				ImGui::Checkbox((const char*)u8"目标所有单位", &AimbotSettings.TargetAll);

				ImGui::SliderFloat((const char*)u8"最大距离", &AimbotSettings.MaxDistance, 0.0f, 300.0f, "%.1f");

				ImGui::SliderFloat((const char*)u8"最小距离", &AimbotSettings.MinDistance, 0.0f, 100.0f, "%.1f");

				ImGui::Checkbox((const char*)u8"平滑自瞄", &AimbotSettings.Smooth);

				ImGui::SliderFloat((const char*)u8"平滑系数", &AimbotSettings.SmoothingVector, 1.0f, 20.0f, "%.2f");

				ImGui::Checkbox((const char*)u8"显示指向箭头", &AimbotSettings.DrawArrow);

				ImGui::Checkbox((const char*)u8"显示视野范围", &AimbotSettings.DrawFOV);

				if (ImGui::BeginCombo((const char*)u8"目标骨骼", TextVars.AimbotBone.c_str()))
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

				ImGui::Checkbox((const char*)u8"需要热键配合", &AimbotSettings.RequireKeyHeld);

				const char* ABpreview = ImGui::GetKeyName(AimbotSettings.AimbotKey);
				if (!ABpreview) ABpreview = "None";

				if (ImGui::BeginCombo((const char*)u8"选择自瞄按键", ABpreview))
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
					AddDefaultTooltip((const char*)u8"仅在按住此键时激活自瞄");
				}

				ImGui::Checkbox((const char*)u8"排除任务目标嫌疑人", &AimbotSettings.ExcludeTargetSuspects);
				AddDefaultTooltip((const char*)u8"开启后常规自瞄不会瞄准需要逮捕的任务目标嫌疑人。");

				ImGui::TreePop();
			}

			if (ImGui::TreeNode((const char*)u8"透视设置"))
			{
				ImGui::Checkbox((const char*)u8"显示队友", &ESPSettings.ShowTeam);

				ImGui::Checkbox((const char*)u8"显示方框", &ESPSettings.ShowBox);

				ImGui::Checkbox((const char*)u8"显示陷阱", &ESPSettings.ShowTraps);

				ImGui::Checkbox((const char*)u8"显示敌方距离", &ESPSettings.ShowEnemyDistance);

				ImGui::Checkbox((const char*)u8"显示敌方名称", &ESPSettings.ShowEnemyName);

				ImGui::Checkbox((const char*)u8"显示骨骼", &ESPSettings.Bones);

				ImGui::Checkbox((const char*)u8"显示子弹弹道", &ESPSettings.BulletTracers);
				
				ImGui::Checkbox((const char*)u8"彩虹弹道效果", &ESPSettings.TracerRainbow);

				ImGui::SliderFloat((const char*)u8"弹道留存时间", &ESPSettings.TracerDuration, 0.5f, 10.0f, "%.1f");

				ImGui::ColorEdit4((const char*)u8"子弹弹道颜色", (float*)&ESPSettings.TracerColor);

				ImGui::ColorEdit4((const char*)u8"嫌疑人颜色", (float*)&ESPSettings.SuspectColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"平民颜色", (float*)&ESPSettings.CivilianColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"死亡单位颜色", (float*)&ESPSettings.DeadColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"队友颜色", (float*)&ESPSettings.TeamColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"被捕单位颜色", (float*)&ESPSettings.ArrestColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"任务目标嫌疑人颜色", (float*)&ESPSettings.TargetSuspectColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"活跃任务目标颜色", (float*)&ESPSettings.ObjectiveActiveColor, ImGuiColorEditFlags_NoInputs);

				ImGui::ColorEdit4((const char*)u8"已完成任务目标颜色", (float*)&ESPSettings.ObjectiveCompletedColor, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox((const char*)u8"仅显示可见目标 (LOS)", &ESPSettings.LOS);

				if (ImGui::SliderFloat((const char*)u8"骨骼透明度", &ESPSettings.BoneOpacity, 0.0f, 1.0f, "%.2f"))
				{
					ESPSettings.SuspectColor = ImVec4(1.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.CivilianColor = ImVec4(0.0f, 0.0f, 1.0f, ESPSettings.BoneOpacity);
					ESPSettings.DeadColor = ImVec4(0.0f, 0.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.TeamColor = ImVec4(0.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
					ESPSettings.ArrestColor = ImVec4(1.0f, 1.0f, 0.0f, ESPSettings.BoneOpacity);
				}

				ImGui::Checkbox((const char*)u8"显示任务目标", &ESPSettings.ShowObjectives);
				AddDefaultTooltip((const char*)u8"任务目标不显示实际位置");
				ImGui::TreePop();
			}

			if (ImGui::TreeNode((const char*)u8"静默自瞄设置 (Silent Aim Settings)"))
			{
				ImGui::Checkbox((const char*)u8"目标平民", &SilentAimSettings.TargetCivilians);

				ImGui::Checkbox((const char*)u8"目标所有单位", &SilentAimSettings.TargetAll);

				ImGui::Checkbox((const char*)u8"目标已死亡单位", &SilentAimSettings.TargetDead);

				ImGui::Checkbox((const char*)u8"目标已投降单位", &SilentAimSettings.TargetSurrendered);

				ImGui::Checkbox((const char*)u8"目标已逮捕单位", &SilentAimSettings.TargetArrested);

				ImGui::SliderFloat((const char*)u8"静默自瞄视野", &SilentAimSettings.MaxFOV, 0, 180.0f, "%.1f");

				ImGui::Checkbox((const char*)u8"显示视野范围", &SilentAimSettings.DrawFOV);

				ImGui::SliderFloat((const char*)u8"视野线粗细", &SilentAimSettings.FOVThickness, 0.1f, 10.0f, "%.2f");

				ImGui::Checkbox((const char*)u8"显示追踪线", &SilentAimSettings.DrawArrow);

				ImGui::SliderFloat((const char*)u8"追踪线粗细", &SilentAimSettings.ArrowThickness, 0.1f, 10.0f, "%.2f");

				ImGui::Checkbox((const char*)u8"需要可见性 (LOS)", &SilentAimSettings.RequiresLOS);

				ImGui::SliderFloat((const char*)u8"命中概率", &SilentAimSettings.HitChance, 0.0f, 100, "%.1f");

				if (ImGui::BeginCombo((const char*)u8"目标骨骼", TextVars.SilentAimBone.c_str()))
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
				ImGui::Checkbox((const char*)u8"魔法子弹 (Magic Bullet)", &SilentAimSettings.MagicBullet);
				AddDefaultTooltip((const char*)u8"将子弹直接生成在目标身上。建议仅在房主(Host)时开启，客机开启可能无效或不稳定。");
				HostOnlyTooltip();

				ImGui::Checkbox((const char*)u8"排除任务目标嫌疑人", &SilentAimSettings.ExcludeTargetSuspects);
				AddDefaultTooltip((const char*)u8"开启后静默自瞄不会瞄准需要逮捕的任务目标嫌疑人。");

				ImGui::TreePop();
			}

			if (ImGui::TreeNode((const char*)u8"杂项设置 (Misc Settings)"))
			{
				ImGui::SeparatorText((const char*)u8"准星设置");

				ImGui::Checkbox((const char*)u8"准星", &CVars.Reticle);

				ImGui::ColorEdit4((const char*)u8"准星颜色", (float*)&MiscSettings.ReticleColor);

				ImGui::DragFloat2((const char*)u8"准星位置", (float*)&MiscSettings.ReticlePosition, 1, -100, 100);

				ImGui::SliderFloat((const char*)u8"准星大小", &MiscSettings.ReticleSize, 1, 15);

				ImGui::Checkbox((const char*)u8"使用十字准星", &MiscSettings.CrossReticle);

				ImGui::Checkbox((const char*)u8"仅在投掷手雷时显示准星", &MiscSettings.ReticleWhenThrowing);

				ImGui::SeparatorText((const char*)u8"自动射击设置 (TriggerBot)");

				ImGui::Checkbox((const char*)u8"自动射击平民", &MiscSettings.TriggerBotTargetsCivilians);

				ImGui::Checkbox((const char*)u8"自动射击使用静默自瞄", &MiscSettings.TriggerBotUsesSilentAim);
				AddDefaultTooltip((const char*)u8"确保你能命中目标");

				ImGui::Checkbox((const char*)u8"自动射击排除任务目标", &MiscSettings.TriggerBotExcludeTargetSuspects);
				AddDefaultTooltip((const char*)u8"开启后自动射击不会射击需要逮捕的任务目标嫌疑人。");

				ImGui::SeparatorText((const char*)u8"其他");

				ImGui::Checkbox((const char*)u8"显示已开启的功能", &CVars.RenderOptions);

				ImGui::Checkbox((const char*)u8"显示玩家列表", &CVars.ListPlayers);

				ImGui::Checkbox((const char*)u8"自动保存设置", &MiscSettings.ShouldAutoSave);
				ImGui::SameLine();
				ImGui::Checkbox((const char*)u8"保存已启用的功能状态", &MiscSettings.ShouldSaveCVars);

				ImGui::SeparatorText((const char*)u8"按键绑定");

				// Create a combo box
				const char* TBpreview = ImGui::GetKeyName(TriggerBotKey);
				if (!TBpreview) TBpreview = "None";

				if (ImGui::BeginCombo((const char*)u8"选择自动射击按键", TBpreview))
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

				if (ImGui::BeginCombo((const char*)u8"选择透视按键", ESPpreview))
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

				if (ImGui::BeginCombo((const char*)u8"选择锁定目标按键", AimPreview))
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
			if (ImGui::BeginTabItem((const char*)u8"秘密功能 (Secret Features)"))
			{
				ImGui::Text((const char*)u8"此选项仅供开发使用。");
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::Separator();
	ImGui::Text((const char*)u8"你可以在 UnknownCheats.me 上找到我，ID 为 Peachmarrow13。");
	ImGui::SameLine();
	ImGui::Text((const char*)u8"此辅助在 UnknownCheats.me 免费发布，请勿从其他任何地方下载。");
	ImGui::End();
}
