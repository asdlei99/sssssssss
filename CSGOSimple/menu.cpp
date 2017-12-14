#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "options.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"

static ConVar* cl_mouseenable = nullptr;

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
    {
        auto clr = ImVec4{
            v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
        };

        if(ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

void Menu::Initialize()
{
    _visible = true;

    cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

    ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	CreateStyle();
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
    if(!_visible)
        return;

    ImGui_ImplDX9_NewFrame();

    ImGui::GetIO().MouseDrawCursor = _visible;

    ImGui::PushStyle(_style);

	if (ImGui::BeginMainMenuBar())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8 * 2.0f, 4 * 2.0f));

		ImGui::Selectable("Rage Window", &g_Options.menu_rage_window, 0, ImVec2(ImGui::CalcTextSize("Rage Window", NULL, true).x, 0.f));
		ImGui::SameLine();


		ImGui::Selectable("Visuals Window", &g_Options.menu_visuals_window, 0, ImVec2(ImGui::CalcTextSize("Visuals Window", NULL, true).x, 0.f));
		ImGui::SameLine();

		ImGui::Selectable("Miscellaneous Window", &g_Options.menu_miscellaneous_window, 0, ImVec2(ImGui::CalcTextSize("Miscellaneous Window", NULL, true).x, 0.f));

		ImGui::PopStyleVar();
		ImGui::EndMainMenuBar();
	}

	if (g_Options.menu_rage_window)
	{
		ImGui::Begin("Rage", &g_Options.menu_rage_window, ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			static char* hitboxes[] = { "Head", "Neck", "Chest", "Pelvis" };

			static char* pitches[] = { "None", "Up", "Down", "Minimal" };
			static char* yaws[] = { "None", "Backwards", "Left", "Right", "Left Random", "Right Random", "Local View", "Spin", "Lower Body", "Freestanding" };

			if (ImGui::TreeNode("Aimbot"))
			{
				ImGui::Checkbox("Enabled", &g_Options.ragebot_enabled);
				ImGui::Checkbox("Target Team", &g_Options.ragebot_target_team);
				ImGui::Checkbox("Silent", &g_Options.ragebot_silent);
				ImGui::Checkbox("Auto Shoot", &g_Options.ragebot_auto_shoot);
				ImGui::Checkbox("Auto Scope", &g_Options.ragebot_auto_scope);
				ImGui::Checkbox("Auto Wall", &g_Options.ragebot_auto_wall);
				//ImGui::Checkbox("Auto Revolver", &g_Options.ragebot_autorevolver);
				ImGui::Checkbox("Hitscan", &g_Options.ragebot_hitscan);
				ImGui::Checkbox("Resolver", &g_Options.ragebot_resolver);
				ImGui::Combo("Hitbox", &g_Options.ragebot_hitbox, hitboxes, IM_ARRAYSIZE(hitboxes));
				ImGui::SliderInt("FOV", &g_Options.ragebot_fov, 1, 360);
				ImGui::SliderInt("Minimum Damage", &g_Options.ragebot_minimum_damage, 1, 100);
				ImGui::SliderInt("Hitchance", &g_Options.ragebot_hitchance, 1, 100);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Anti Aim"))
			{
				ImGui::Checkbox("Enabled", &g_Options.anti_aim_enabled);
				ImGui::Combo("Pitch", &g_Options.anti_aim_pitch, pitches, IM_ARRAYSIZE(pitches));
				ImGui::Combo("Yaw", &g_Options.anti_aim_yaw, yaws, IM_ARRAYSIZE(yaws));
				ImGui::SliderInt("Yaw Offset", &g_Options.anti_aim_yaw_offset, -180, 180);
				ImGui::Combo("Fake Yaw", &g_Options.anti_aim_fake_yaw, yaws, IM_ARRAYSIZE(yaws));
				ImGui::SliderInt("Fake Yaw Offset", &g_Options.anti_aim_fake_yaw_offset, -180, 180);
				ImGui::SliderInt("Spin Speed", &g_Options.anti_aim_spin_speed, 1, 100);
				ImGui::TreePop();
			}
		}
		ImGui::End(); //End Rage window
	}

	if (g_Options.menu_visuals_window)
	{
		ImGui::Begin("Visuals", &g_Options.menu_visuals_window, ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			if (ImGui::TreeNode("ESP"))
			{
				ImGui::Checkbox("Enabled", &g_Options.esp_enabled);
				ImGui::Checkbox("Team Check", &g_Options.esp_team_check);
				ImGui::Checkbox("Box", &g_Options.esp_box);
				ImGui::Checkbox("Name", &g_Options.esp_name);
				ImGui::Checkbox("Flags", &g_Options.esp_flags);
				ImGui::Checkbox("Health", &g_Options.esp_health);
				ImGui::Checkbox("Weapon", &g_Options.esp_weapon);
				ImGui::Checkbox("Ammo", &g_Options.esp_ammo);
				ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons);
				ImGui::Checkbox("Defuse Kits", &g_Options.esp_defuse_kit);
				ImGui::Checkbox("Planted C4", &g_Options.esp_planted_c4);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Glow"))
			{
				ImGui::Checkbox("Enabled", &g_Options.glow_enabled);
				ImGui::Checkbox("Team Check", &g_Options.glow_team_check);
				ImGui::Checkbox("Players", &g_Options.glow_players);
				ImGui::Checkbox("Dropped Weapons", &g_Options.glow_dropped_weapons);
				ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits);
				ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Other"))
			{
				ImGui::Checkbox("No Scope", &g_Options.other_no_scope);
				ImGui::Checkbox("No Visual Recoil", &g_Options.other_no_visual_recoil);
				ImGui::Checkbox("Thirdperson", &g_Options.other_thirdperson);
				ImGui::Checkbox("Angle Lines", &g_Options.other_angle_lines);
				ImGui::Checkbox("Nightmode", &g_Options.other_nightmode);
				ImGui::Checkbox("No Flash", &g_Options.other_no_flash);
				ImGui::Checkbox("No Smoke", &g_Options.other_no_smoke);
				ImGui::Checkbox("LBY Indicator", &g_Options.other_lby_indicator);
				ImGui::SliderInt("Viewmodel FOV", &g_Options.other_viewmodel_fov, 68, 120);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Colors"))
			{
				ImGuiEx::ColorEdit4("Players", &g_Options.color_players);
				ImGuiEx::ColorEdit4("Dropped Weapons", &g_Options.color_dropped_weapons);
				ImGuiEx::ColorEdit4("Defuse Kits", &g_Options.color_defuse_kits);
				ImGuiEx::ColorEdit4("Planted C4", &g_Options.color_planted_c4);
				ImGui::TreePop();
			}
		}
		ImGui::End(); //End Visuals window
	}

	if (g_Options.menu_miscellaneous_window)
	{
		ImGui::Begin("Miscellaneous", &g_Options.menu_miscellaneous_window, ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			static char* clantags[] = { "None", "sinner", "AIMWARE.net", "HookTronic.TOP", ">cantresolve.us", "gamesense", "eagle", "Styles > all", "superior", "Barbossa", "Luigi" };

			ImGui::Checkbox("Bunnyhop", &g_Options.misc_bunnyhop);
			ImGui::Checkbox("Auto Strafe", &g_Options.misc_auto_strafe);
			ImGui::Checkbox("Hitmarkers", &g_Options.misc_hitmarkers);
			ImGui::Checkbox("Hitmarkers sound", &g_Options.misc_hitmarkerssound);
			ImGui::Checkbox("Rank Revealer", &g_Options.misc_rank_revealer);
			ImGui::Checkbox("FakeLag", &g_Options.fakelag);
			ImGui::SliderFloat("FakeLag Amount", &g_Options.fakelagamount, 1, 17);
			ImGui::Combo("Clantag Spammer", &g_Options.misc_clantag_spammer, clantags, IM_ARRAYSIZE(clantags));
		}
		ImGui::End(); //End Miscellaneous window
	}

    ImGui::PopStyle();

    ImGui::Render();
}

void Menu::Show()
{
    _visible = true;
    cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
    _visible = false;
    cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
    _visible = !_visible;
    cl_mouseenable->SetValue(!_visible);
}

void Menu::CreateStyle()
{
	_style.Colors[ImGuiCol_Text] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	_style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	_style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
	_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
	_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.59f);
	_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	_style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
	_style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	_style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);

	_style.WindowRounding = 0.f;
	_style.FramePadding = ImVec2(4, 1);
	_style.ScrollbarSize = 10.f;
	_style.ScrollbarRounding = 0.f;
	_style.GrabMinSize = 5.f;
}
