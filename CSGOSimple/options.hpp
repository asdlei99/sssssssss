#pragma once

#include <string>
#include "valve_sdk/Misc/Color.hpp"

#define OPTION(type, var, val) type var = val

class Config
{
public:
	//
	// MENU
	// 
	OPTION(bool, menu_rage_window, false);
	OPTION(bool, menu_visuals_window, false);
	OPTION(bool, menu_miscellaneous_window, false);

	//
	// RAGEBOT
	//
	OPTION(bool, ragebot_enabled, false);
	OPTION(bool, ragebot_silent, false);
	OPTION(bool, ragebot_resolver, false);
	OPTION(bool, ragebot_auto_wall, false);
	OPTION(bool, ragebot_autorevolver, false);
	OPTION(bool, ragebot_target_team, false);
	OPTION(bool, ragebot_auto_shoot, false);
	OPTION(bool, ragebot_auto_scope, false);
	OPTION(bool, ragebot_hitscan, false);
	OPTION(int, ragebot_hitbox, 0);
	OPTION(int, ragebot_fov, 1);
	OPTION(int, ragebot_minimum_damage, 1);
	OPTION(int, ragebot_hitchance, 1);

	//
	// ANTI AIMBOT
	//
	OPTION(bool, anti_aim_enabled, false);
	OPTION(int, anti_aim_pitch, 0);
	OPTION(int, anti_aim_yaw, 0);
	OPTION(int, anti_aim_yaw_offset, 0);
	OPTION(int, anti_aim_fake_yaw, 0);
	OPTION(int, anti_aim_fake_yaw_offset, 0);
	OPTION(int, anti_aim_spin_speed, 1);

	// 
	// ESP
	// 
	OPTION(bool, esp_enabled, false);
	OPTION(bool, esp_team_check, false);
	OPTION(bool, esp_box, false);
	OPTION(bool, esp_name, false);
	OPTION(bool, esp_flags, false);
	OPTION(bool, esp_health, false);
	OPTION(bool, esp_weapon, false);
	OPTION(bool, esp_ammo, false);
	OPTION(bool, esp_dropped_weapons, false);
	OPTION(bool, esp_defuse_kit, false);
	OPTION(bool, esp_planted_c4, false);

	// 
	// GLOW
	// 
	OPTION(bool, glow_enabled, false);
	OPTION(bool, glow_team_check, false);
	OPTION(bool, glow_players, false);
	OPTION(bool, glow_dropped_weapons, false);
	OPTION(bool, glow_defuse_kits, false);
	OPTION(bool, glow_planted_c4, false);

	//
	// OTHER VISUALS
	//
	OPTION(bool, other_no_scope, false);
	OPTION(bool, other_no_visual_recoil, false);
	OPTION(int, other_viewmodel_fov, 68);
	OPTION(bool, other_thirdperson, false);
	OPTION(bool, other_angle_lines, false);
	OPTION(bool, other_nightmode, false);
	OPTION(bool, other_no_flash, false);
	OPTION(bool, other_no_smoke, false);
	OPTION(bool, other_lby_indicator, false);

	//
	// MISC
	//
	OPTION(bool, misc_bunnyhop, false);
	OPTION(bool, misc_auto_strafe, false);
	OPTION(bool, misc_hitmarkers, false);
	OPTION(bool, misc_hitmarkerssound, false);
	OPTION(bool, fakelag, false);
	OPTION(float, fakelagamount, false);
	OPTION(bool, misc_rank_revealer, false);
	OPTION(int, misc_clantag_spammer, 0);

	// 
	// COLORS
	// 
	OPTION(Color, color_players, Color(255, 255, 255));
	OPTION(Color, color_dropped_weapons, Color(85, 85, 85));
	OPTION(Color, color_defuse_kits, Color(0, 125, 255));
	OPTION(Color, color_planted_c4, Color(85, 255, 85));
};

extern Config g_Options;
extern bool   g_Unload;
