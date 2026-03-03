#include "pch.h"
#include "Localization.h"
#include <windows.h>
#include <unordered_map>

namespace Localization
{
    Language CurrentLanguage = Language::English;
    
    // Using unordered_map for performance, string -> {Language -> string}
    static std::unordered_map<std::string, std::unordered_map<Language, std::string>> Dictionary;

    Language GetSystemLanguage()
    {
        LANGID LangID = GetUserDefaultUILanguage();
        if (PRIMARYLANGID(LangID) == LANG_CHINESE)
        {
            return Language::Chinese;
        }
        return Language::English;
    }

    void AddString(const std::string& key, const std::string& en, const std::string& zh)
    {
        Dictionary[key][Language::English] = en;
        Dictionary[key][Language::Chinese] = zh;
    }

    const char* T(const std::string& key)
    {
        auto it = Dictionary.find(key);
        if (it != Dictionary.end())
        {
            return it->second[CurrentLanguage].c_str();
        }
        return key.c_str(); // Fallback to key itself
    }

    void Initialize()
    {
        CurrentLanguage = GetSystemLanguage();
        
        // Populate dictionary with some core strings as examples
        AddString("MENU_TITLE", "Ready or Not Free Cheat - PeachMarrow12", (const char*)u8"Ready or Not 免费辅助 - PeachMarrow12");
        AddString("TAB_ABOUT", "About", (const char*)u8"关于");
        AddString("TAB_PLAYER", "Player", (const char*)u8"玩家");
        AddString("TAB_WEAPON", "Weapon", (const char*)u8"武器");
        AddString("TAB_WORLD", "World", (const char*)u8"世界");
        AddString("TAB_MISC", "Misc", (const char*)u8"杂项");
        AddString("TAB_CONFIG", "Config", (const char*)u8"配置");
    
        AddString("UNKNOWN_TRAP", "Unknown Trap", (const char*)u8"未知陷阱");
        AddString("EXPLOSIVE_TRAP", "Explosive Trap", (const char*)u8"爆炸陷阱");
        AddString("FLASHBANG_TRAP", "Flashbang Trap", (const char*)u8"闪光弹陷阱");
        AddString("ALARM_TRAP", "Alarm Trap", (const char*)u8"报警器陷阱");
        AddString("ACTIVE_FEATURES_LIST", "Active Features List", (const char*)u8"已开启功能列表");
        AddString("ACTIVE_FEATURES", "Active Features:", (const char*)u8"已开启功能:");
        AddString("GODMODE", "GodMode", (const char*)u8"无敌模式");
        AddString("INF_AMMO", "Inf Ammo", (const char*)u8"无限弹药");
        AddString("AIMBOT", "Aimbot", (const char*)u8"自瞄");
        AddString("ESP", "ESP", (const char*)u8"透视");
        AddString("SPEED_X_1F", "Speed x%.1f", (const char*)u8"速度修改 x%.1f");
        AddString("SILENT_AIM", "Silent Aim", (const char*)u8"静默自瞄");
        AddString("NOCLIP", "NoClip", (const char*)u8"穿墙模式");
        AddString("CUSTOM_RETICLE", "Custom Reticle", (const char*)u8"自定义准星");
        AddString("TRIGGERBOT", "TriggerBot", (const char*)u8"自动射击");
        AddString("PLAYER_LIST", "Player List", (const char*)u8"玩家列表");
        AddString("CURRENT_PLAYERS", "Current Players:", (const char*)u8"当前玩家:");
        AddString("GODMODE", "GodMode", (const char*)u8"无敌");
        AddString("TELEPORT_TO_ME", "Teleport to Me", (const char*)u8"传送到我这");
        AddString("GIVE_SUPER_SPEED", "Give Super Speed", (const char*)u8"赋予极速");
        AddString("HOST_ONLY_IF_YOU_ARE_NOT_THE_HOST_THIS_FEATURE_WILL_NOT_FUNCTION", "Host Only - If you are not the host, this feature will not function.", (const char*)u8"仅限房主 (Host Only) - 如果你不是房主，此功能将不起作用。");
        AddString("LANGUAGE", "Language", (const char*)u8"语言 (Language)");
        AddString("READY_OR_NOT_FREE_CHEAT_PEACHMARROW12", "Ready or Not Free Cheat - PeachMarrow12", (const char*)u8"Ready or Not 免费辅助 - PeachMarrow12");
        AddString("VERSION_D_D_D", "Version %d.%d.%d", (const char*)u8"版本 %d.%d.%d");
        AddString("THANKS_FOR_USING_THIS_CHEAT_S", "Thanks for using this cheat, %s!", (const char*)u8"感谢使用此辅助, %s!");
        AddString("USERNAME_NOT_FOUND_BUT_THANKS_FOR_USING_ANYWAY", "Username not found, but thanks for using anyway!", (const char*)u8"未找到用户名，但仍感谢使用！");
        AddString("CURRENT_AUTH_HOST_AUTHORITY", "Current Auth: Host (Authority)", (const char*)u8"当前权限: 主机 (Authority/Host)");
        AddString("CURRENT_AUTH_CLIENT", "Current Auth: Client", (const char*)u8"当前权限: 客户机 (Client)");
        AddString("CURRENT_AUTH_WAITING_FOR_CONTROLLER", "Current Auth: Waiting for controller...", (const char*)u8"当前权限: 等待控制器初始化...");
        AddString("GODMODE", "GodMode", (const char*)u8"无敌模式 (GodMode)");
        AddString("AIMBOT", "Aimbot", (const char*)u8"自瞄 (Aimbot)");
        AddString("SILENT_AIM", "Silent Aim", (const char*)u8"静默自瞄 (Silent Aim)");
        AddString("NOT_PERFECT_YET_USE_WITH_CAUTION", "Not perfect yet, use with caution.", (const char*)u8"尚不完善，请谨慎使用。");
        AddString("ESP", "ESP", (const char*)u8"透视 (ESP)");
        AddString("SPEED", "Speed", (const char*)u8"移动速度");
        AddString("ENABLE_SPEED", "Enable Speed", (const char*)u8"开启速度修改");
        AddString("FOV", "FOV", (const char*)u8"视野 (FOV)");
        AddString("WEAPON", "Weapon", (const char*)u8"武器");
        AddString("INFINITE_AMMO", "Infinite Ammo", (const char*)u8"无限弹药");
        AddString("REMOVE_RECOIL", "Remove Recoil", (const char*)u8"移除后坐力");
        AddString("REMOVE_SPREAD", "Remove Spread", (const char*)u8"移除扩散");
        AddString("ADD_FULLAUTO", "Add FullAuto", (const char*)u8"添加全自动");
        AddString("ADD_PENETRATION", "Add Penetration", (const char*)u8"添加穿墙");
        AddString("INSTAKILL", "InstaKill", (const char*)u8"秒杀");
        AddString("INCREASE_FIRERATE", "Increase FireRate", (const char*)u8"增加射速");
        AddString("SHOOT_FROM_RETICLE", "Shoot from Reticle", (const char*)u8"准星处射击");
        AddString("ADD_MAGAZINE", "Add Magazine", (const char*)u8"添加弹匣");
        AddString("TRIGGERBOT", "TriggerBot", (const char*)u8"自动射击 (TriggerBot)");
        AddString("WORLD", "World", (const char*)u8"世界");
        AddString("KILL_ALL_SUSPECTS", "Kill All Suspects", (const char*)u8"杀死所有嫌疑人");
        AddString("SURRENDER_ALL_SUSPECTS", "Surrender All Suspects", (const char*)u8"降伏所有嫌疑人");
        AddString("ARREST_ALL_SUSPECTS", "Arrest All Suspects", (const char*)u8"逮捕所有嫌疑人");
        AddString("KILL_ALL_CIVILIANS", "Kill All Civilians", (const char*)u8"杀死所有平民");
        AddString("SURRENDER_ALL_CIVILIANS", "Surrender All Civilians", (const char*)u8"降伏所有平民");
        AddString("ARREST_ALL_CIVILIANS", "Arrest All Civilians", (const char*)u8"逮捕所有平民");
        AddString("COLLECT_ALL_EVIDENCE", "Collect All Evidence", (const char*)u8"收集所有证据");
        AddString("AUTO_WIN", "Auto Win", (const char*)u8"自动获胜");
        AddString("UNLOCK_ALL_DOORS", "Unlock All Doors", (const char*)u8"解锁所有门");
        AddString("BULLET_TIME", "Bullet Time", (const char*)u8"子弹时间");
        AddString("MISC", "Misc", (const char*)u8"杂项");
        AddString("SAVE_SETTINGS", "Save Settings", (const char*)u8"保存设置");
        AddString("LOAD_SETTINGS", "Load Settings", (const char*)u8"加载设置");
        AddString("THIS_ONLY_SAVES_LOADS_CONFIGURATION_NOT_WHICH_FEATURES_ARE_ENABLED", "This only saves/loads configuration, not which features are enabled.", (const char*)u8"这些只保存和加载配置，而不是哪些功能已启用。");
        AddString("DEBUG", "Debug", (const char*)u8"调试 (Debug)");
        AddString("ENABLE_DEBUG_OPTIONS_USED_FOR_DEVELOPMENT_AND_BUG_FINDING_PROBABLY_USELESS_FOR_YOU", "Enable debug options used for development and bug finding. Probably useless for you.", (const char*)u8"这只是开启了一些我用来查找错误和有用信息的菜单选项。对你来说可能没用。");
        AddString("DEBUG_FUNC_CONTAINS", "Debug Func Contains", (const char*)u8"调试函数名包含");
        AddString("DEBUG_OBJ_CONTAINS", "Debug Obj Contains", (const char*)u8"调试对象名包含");
        AddString("PRINT_ACTORS", "Print Actors", (const char*)u8"打印 Actor");
        AddString("CONFIG", "Config", (const char*)u8"配置");
        AddString("AIMBOT_SETTINGS", "Aimbot Settings", (const char*)u8"自瞄设置 (Aimbot Settings)");
        AddString("AIMBOT_FOV", "Aimbot FOV", (const char*)u8"自瞄视野 (Aimbot FOV)");
        AddString("REQUIRE_LOS", "Require LOS", (const char*)u8"需要可见性 (LOS)");
        AddString("TARGET_MUST_BE_VISIBLE_REQUIRES_LINE_OF_SIGHT_CHECK", "Target must be visible; requires line-of-sight check.", (const char*)u8"目标必须可见；需要视线检查。");
        AddString("TARGET_CIVILIANS", "Target Civilians", (const char*)u8"目标平民");
        AddString("TARGET_DEAD", "Target Dead", (const char*)u8"目标已死亡单位");
        AddString("TARGET_ARRESTED", "Target Arrested", (const char*)u8"目标已逮捕单位");
        AddString("TARGET_ALL", "Target All", (const char*)u8"目标所有单位");
        AddString("MAX_DISTANCE", "Max Distance", (const char*)u8"最大距离");
        AddString("MIN_DISTANCE", "Min Distance", (const char*)u8"最小距离");
        AddString("SMOOTH_AIM", "Smooth Aim", (const char*)u8"平滑自瞄");
        AddString("SMOOTHING", "Smoothing", (const char*)u8"平滑系数");
        AddString("DRAW_ARROW", "Draw Arrow", (const char*)u8"显示指向箭头");
        AddString("DRAW_FOV", "Draw FOV", (const char*)u8"显示视野范围");
        AddString("TARGET_BONE", "Target Bone", (const char*)u8"目标骨骼");
        AddString("REQUIRE_KEY_HELD", "Require Key Held", (const char*)u8"需要热键配合");
        AddString("AIMBOT_KEY", "Aimbot Key", (const char*)u8"选择自瞄按键");
        AddString("AIMBOT_ONLY_ACTIVATES_WHILE_HOLDING_THIS_KEY", "Aimbot only activates while holding this key", (const char*)u8"仅在按住此键时激活自瞄");
        AddString("EXCLUDE_TARGET_SUSPECTS", "Exclude Target Suspects", (const char*)u8"排除任务目标嫌疑人");
        AddString("REGULAR_AIMBOT_WILL_NOT_TARGET_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED", "Regular aimbot will not target mission-critical suspects that should be arrested.", (const char*)u8"开启后常规自瞄不会瞄准需要逮捕的任务目标嫌疑人。");
        AddString("ESP_SETTINGS", "ESP Settings", (const char*)u8"透视设置");
        AddString("SHOW_TEAM", "Show Team", (const char*)u8"显示队友");
        AddString("SHOW_BOX", "Show Box", (const char*)u8"显示方框");
        AddString("SHOW_TRAPS", "Show Traps", (const char*)u8"显示陷阱");
        AddString("SHOW_DISTANCE", "Show Distance", (const char*)u8"显示敌方距离");
        AddString("SHOW_NAME", "Show Name", (const char*)u8"显示敌方名称");
        AddString("SHOW_BONES", "Show Bones", (const char*)u8"显示骨骼");
        AddString("BULLET_TRACERS", "Bullet Tracers", (const char*)u8"显示子弹弹道");
        AddString("RAINBOW_TRACERS", "Rainbow Tracers", (const char*)u8"彩虹弹道效果");
        AddString("TRACER_DURATION", "Tracer Duration", (const char*)u8"弹道留存时间");
        AddString("TRACER_COLOR", "Tracer Color", (const char*)u8"子弹弹道颜色");
        AddString("SUSPECT_COLOR", "Suspect Color", (const char*)u8"嫌疑人颜色");
        AddString("CIVILIAN_COLOR", "Civilian Color", (const char*)u8"平民颜色");
        AddString("DEAD_COLOR", "Dead Color", (const char*)u8"死亡单位颜色");
        AddString("TEAM_COLOR", "Team Color", (const char*)u8"队友颜色");
        AddString("ARRESTED_COLOR", "Arrested Color", (const char*)u8"被捕单位颜色");
        AddString("TARGET_SUSPECT_COLOR", "Target Suspect Color", (const char*)u8"任务目标嫌疑人颜色");
        AddString("ACTIVE_OBJECTIVE_COLOR", "Active Objective Color", (const char*)u8"活跃任务目标颜色");
        AddString("COMPLETED_OBJECTIVE_COLOR", "Completed Objective Color", (const char*)u8"已完成任务目标颜色");
        AddString("ONLY_SHOW_VISIBLE_LOS", "Only Show Visible (LOS)", (const char*)u8"仅显示可见目标 (LOS)");
        AddString("BONE_OPACITY", "Bone Opacity", (const char*)u8"骨骼透明度");
        AddString("SHOW_OBJECTIVES", "Show Objectives", (const char*)u8"显示任务目标");
        AddString("OBJECTIVES_DO_NOT_SHOW_ACTUAL_PHYSICAL_LOCATIONS", "Objectives do not show actual physical locations", (const char*)u8"任务目标不显示实际位置");
        AddString("SILENT_AIM_SETTINGS", "Silent Aim Settings", (const char*)u8"静默自瞄设置 (Silent Aim Settings)");
        AddString("TARGET_SURRENDERED", "Target Surrendered", (const char*)u8"目标已投降单位");
        AddString("SILENT_AIM_FOV", "Silent Aim FOV", (const char*)u8"静默自瞄视野");
        AddString("FOV_THICKNESS", "FOV Thickness", (const char*)u8"视野线粗细");
        AddString("DRAW_ARROW", "Draw Arrow", (const char*)u8"显示追踪线");
        AddString("ARROW_THICKNESS", "Arrow Thickness", (const char*)u8"追踪线粗细");
        AddString("HIT_CHANCE", "Hit Chance", (const char*)u8"命中概率");
        AddString("MAGIC_BULLET", "Magic Bullet", (const char*)u8"魔法子弹 (Magic Bullet)");
        AddString("BULLETS_SPAWN_DIRECTLY_ON_TARGETS_RECOMMENDED_FOR_HOST_ONLY_MAY_BE_UNSTABLE_OR_NON_FUNCTIONAL_FOR_CLIENTS", "Bullets spawn directly on targets. Recommended for Host only; may be unstable or non-functional for Clients.", (const char*)u8"将子弹直接生成在目标身上。建议仅在房主(Host)时开启，客机开启可能无效或不稳定。");
        AddString("SILENT_AIM_WILL_NOT_TARGET_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED", "Silent aim will not target mission-critical suspects that should be arrested.", (const char*)u8"开启后静默自瞄不会瞄准需要逮捕的任务目标嫌疑人。");
        AddString("MISC_SETTINGS", "Misc Settings", (const char*)u8"杂项设置 (Misc Settings)");
        AddString("RETICLE_SETTINGS", "Reticle Settings", (const char*)u8"准星设置");
        AddString("RETICLE", "Reticle", (const char*)u8"准星");
        AddString("RETICLE_COLOR", "Reticle Color", (const char*)u8"准星颜色");
        AddString("RETICLE_POSITION", "Reticle Position", (const char*)u8"准星位置");
        AddString("RETICLE_SIZE", "Reticle Size", (const char*)u8"准星大小");
        AddString("CROSS_RETICLE", "Cross Reticle", (const char*)u8"使用十字准星");
        AddString("ONLY_SHOW_WHEN_THROWING", "Only Show when Throwing", (const char*)u8"仅在投掷手雷时显示准星");
        AddString("TRIGGERBOT_SETTINGS", "TriggerBot Settings", (const char*)u8"自动射击设置 (TriggerBot)");
        AddString("TARGET_CIVILIANS", "Target Civilians", (const char*)u8"自动射击平民");
        AddString("USE_SILENT_AIM", "Use Silent Aim", (const char*)u8"自动射击使用静默自瞄");
        AddString("ENSURES_HITS_ON_TARGETS", "Ensures hits on targets", (const char*)u8"确保你能命中目标");
        AddString("EXCLUDE_TARGET_SUSPECTS", "Exclude Target Suspects", (const char*)u8"自动射击排除任务目标");
        AddString("TRIGGERBOT_WILL_NOT_SHOOT_MISSION_CRITICAL_SUSPECTS_THAT_SHOULD_BE_ARRESTED", "Triggerbot will not shoot mission-critical suspects that should be arrested.", (const char*)u8"开启后自动射击不会射击需要逮捕的任务目标嫌疑人。");
        AddString("MISC", "Misc", (const char*)u8"其他");
        AddString("DRAW_ACTIVE_FEATURES", "Draw Active Features", (const char*)u8"显示已开启的功能");
        AddString("SHOW_PLAYER_LIST", "Show Player List", (const char*)u8"显示玩家列表");
        AddString("AUTO_SAVE_SETTINGS", "Auto Save Settings", (const char*)u8"自动保存设置");
        AddString("SAVE_ACTIVE_FEATURES_STATE", "Save Active Features State", (const char*)u8"保存已启用的功能状态");
        AddString("KEYBINDS", "Keybinds", (const char*)u8"按键绑定");
        AddString("TRIGGERBOT_KEY", "TriggerBot Key", (const char*)u8"选择自动射击按键");
        AddString("ESP_KEY", "ESP Key", (const char*)u8"选择透视按键");
        AddString("AIM_KEY", "Aim Key", (const char*)u8"选择锁定目标按键");
        AddString("SECRET_FEATURES", "Secret Features", (const char*)u8"秘密功能 (Secret Features)");
        AddString("THIS_OPTION_IS_FOR_DEVELOPMENT_ONLY", "This option is for development only.", (const char*)u8"此选项仅供开发使用。");
        AddString("YOU_CAN_FIND_ME_ON_UNKNOWNCHEATS_ME_ID_PEACHMARROW13", "You can find me on UnknownCheats.me, ID: Peachmarrow13.", (const char*)u8"你可以在 UnknownCheats.me 上找到我，ID 为 Peachmarrow13。");
        AddString("THIS_CHEAT_IS_RELEASED_FOR_FREE_ON_UNKNOWNCHEATS_ME_DO_NOT_DOWNLOAD_FROM_ELSEWHERE", "This cheat is released for free on UnknownCheats.me, do not download from elsewhere.", (const char*)u8"此辅助在 UnknownCheats.me 免费发布，请勿从其他任何地方下载。");
    }
}
