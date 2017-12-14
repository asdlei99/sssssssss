#include "hooks.hpp"

#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bunnyhop.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "helpers/math.hpp"
#include "ragebot.hpp"
#include "antiaim.hpp"
#include "clantagspammer.hpp"
#include "spoofedconvar.hpp"
#include "autostrafe.hpp"
#include "hitmarkers.hpp"
#include "weapon_ids.hpp"
#include "lagcompensation.hpp"
#include "pistol.hpp"
#include "rifle.hpp"
#include "shotgun.hpp"
#include "sniper.hpp"
#include "smg.hpp"
#include "nightmode.hpp"
#include "engineprediction.hpp"
std::vector<const char*> smoke_materials = {
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
};
namespace Hooks
{
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook clientmode_hook;

	vgui::HFont indicator_font;

	QAngle angles;
	QAngle realYaw;
	QAngle fakeYaw;

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);

		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

		vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);

		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);

		Visuals::CreateFonts();

		indicator_font = g_VGuiSurface->CreateFont_();

		g_VGuiSurface->SetFontGlyphSet(indicator_font, "Verdana", 30, 650, NULL, NULL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

		hitmarkers::singleton()->initialize();
	    nightmode::singleton()->initialize();
	}

	CLagCompensation lagfix;
	float RandomFloat(float min, float max)
	{
		float r = (float)rand() / (float)RAND_MAX;
		return min + r * (max - min);
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();

		Glow::Get().Shutdown();

		hitmarkers::singleton()->uninitialize();
		nightmode::singleton()->uninitialize();
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");

		static bool spoofed = false;
		static ConVar* sv_cheats_var;
		static SpoofedConvar* sv_cheats;

		if (!spoofed)
		{
			sv_cheats_var = g_CVar->FindVar("sv_cheats");
			if (sv_cheats_var)
			{
				sv_cheats = new SpoofedConvar(sv_cheats_var);
				sv_cheats->Spoof();
				sv_cheats->SetInt(1);
				spoofed = true;
			}
		}
		
		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.other_viewmodel_fov);

		Menu::Get().Render();

		if (g_Options.misc_rank_revealer && InputSys::Get().IsKeyDown(VK_TAB))
			Utils::RankRevealAll();

		return oEndScene(device);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			Menu::Get().OnDeviceReset();
			Visuals::CreateFonts();
		}

		return hr;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);

		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;

		QAngle old_viewangles;
		float old_forwardmove;
		float old_sidemove;

		nightmode::singleton()->on_createmove();

		ClantagSpammer::OnCreateMove(cmd);

		if (g_Options.misc_auto_strafe) {
			AutoStrafe::OnCreateMove(cmd);
		}

		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_Options.other_thirdperson) {
			if (!*(bool*)((DWORD)g_Input + 0xA5)) {
				g_EngineClient->ClientCmd_Unrestricted("thirdperson", 0);
			}
		}
		else {
			if (*(bool*)((DWORD)g_Input + 0xA5)) {
				g_EngineClient->ClientCmd_Unrestricted("firstperson", 0);
			}
		}

		old_viewangles = cmd->viewangles;
		old_forwardmove = cmd->forwardmove;
		old_sidemove = cmd->sidemove;

		if (g_Options.anti_aim_enabled) {
			AntiAim::OnCreateMove(cmd, bSendPacket);
		}

		if (g_LocalPlayer && !g_Options.anti_aim_enabled)
		{
			g_EngineClient->GetViewAngles(angles);
		}

		if (g_LocalPlayer)
		{
			if (!bSendPacket)
			{
				if (g_Options.anti_aim_enabled)
				{
					angles = cmd->viewangles;
				}
				realYaw = QAngle(0.f, cmd->viewangles.yaw, 0.f);
			}
			else
			{
				fakeYaw = QAngle(0.f, cmd->viewangles.yaw, 0.f);
			}
		}

		if (g_Options.misc_bunnyhop) {
			Bunnyhop::OnCreateMove(cmd);
		}

		EnginePrediction::StartPrediction(cmd);

		if (g_Options.ragebot_enabled) {
			Ragebot::OnCreateMove(cmd);
		}

		EnginePrediction::EndPrediction();

		float yaw_delta = cmd->viewangles.yaw - old_viewangles.yaw;
		float f1;
		float f2;

		if (old_viewangles.yaw < 0.f)
			f1 = 360.0f + old_viewangles.yaw;
		else
			f1 = old_viewangles.yaw;

		if (cmd->viewangles.yaw < 0.0f)
			f2 = 360.0f + cmd->viewangles.yaw;
		else
			f2 = cmd->viewangles.yaw;

		if (f2 < f1)
			yaw_delta = abs(f2 - f1);
		else
			yaw_delta = 360.0f - abs(f1 - f2);
		yaw_delta = 360.0f - yaw_delta;

		cmd->forwardmove = cos(DEG2RAD(yaw_delta)) * old_forwardmove + cos(DEG2RAD(yaw_delta + 90.f)) * old_sidemove;
		cmd->sidemove = sin(DEG2RAD(yaw_delta)) * old_forwardmove + sin(DEG2RAD(yaw_delta + 90.f)) * old_sidemove;

		cmd->viewangles.normalize();

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto zoomId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

		const auto panelName = g_VGuiPanel->GetName(panel);

		if (!zoomId) {
			if (!strcmp(panelName, "HudZoom")) {
				zoomId = panel;
			}
		}
		else
		{
			if (zoomId == panel && g_Options.other_no_scope)
				return;
		}

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) {
			if (g_EngineClient->IsInGame() && !g_EngineClient->IsTakingScreenshot()) {

				if (!g_LocalPlayer)
					return;

				if (g_Options.esp_enabled) {
					for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
						auto entity = C_BasePlayer::GetPlayerByIndex(i);

						if (!entity)
							continue;

						if (entity == g_LocalPlayer)
							continue;

						if (i < 65 && !entity->IsDormant() && entity->IsAlive()) {
							// Begin will calculate player screen coordinate, bounding box, etc
							// If it returns false it means the player is not inside the screen
							// or is an ally (and team check is enabled)
							if (Visuals::Player::Begin(entity)) {
								if (g_Options.esp_box)     Visuals::Player::RenderBox();
								if (g_Options.esp_weapon) Visuals::Player::RenderWeapon();
								if (g_Options.esp_ammo && entity->m_hActiveWeapon() && !g_WeaponIDs.IsMisc(entity->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex())) Visuals::Player::RenderAmmo();
								if (g_Options.esp_name)     Visuals::Player::RenderName();
								if (g_Options.esp_health)    Visuals::Player::RenderHealth();
								if (g_Options.esp_flags)    Visuals::Player::RenderFlags();
							}
						}
						else if (g_Options.esp_dropped_weapons && entity->IsWeapon()) {
							Visuals::Misc::RenderWeapon((C_BaseCombatWeapon*)entity);
						}
						else if (g_Options.esp_defuse_kit && entity->IsDefuseKit()) {
							Visuals::Misc::RenderDefuseKit(entity);
						}
						else if (entity->IsPlantedC4()) {
							if (g_Options.esp_planted_c4)
								Visuals::Misc::RenderPlantedC4(entity);
						}
					}
				}

				if (g_Options.other_no_scope && g_LocalPlayer->m_bIsScoped()) {
					int w, h;

					g_EngineClient->GetScreenSize(w, h);

					g_VGuiSurface->DrawSetColor(Color::Black);

					int cx = w / 2;
					int cy = h / 2;

					g_VGuiSurface->DrawLine(cx - 6969, cy, cx + 6969, cy);
					g_VGuiSurface->DrawLine(cx, cy - 6969, cx, cy + 6969);
				}

				if (g_Options.anti_aim_enabled && g_Options.other_angle_lines && g_LocalPlayer->IsAlive()) {
					Vector src3D, dst3D, forward, src, dst;
					trace_t tr;
					Ray_t ray;
					CTraceFilter filter;

					filter.pSkip = g_LocalPlayer;
					Math::AngleVectors(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0), forward);
					src3D = g_LocalPlayer->m_vecOrigin();
					dst3D = src3D + (forward * 50.f);

					ray.Init(src3D, dst3D);

					g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

					if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
						return;

					g_VGuiSurface->DrawSetColor(Color(255, 170, 0));
					g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);

					Math::AngleVectors(realYaw, forward);
					dst3D = src3D + (forward * 50.f);

					ray.Init(src3D, dst3D);

					g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

					if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
						return;

					g_VGuiSurface->DrawSetColor(Color::Green);
					g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);

					Math::AngleVectors(fakeYaw, forward);
					dst3D = src3D + (forward * 50.f);

					ray.Init(src3D, dst3D);

					g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

					if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
						return;

					g_VGuiSurface->DrawSetColor(Color::Red);
					g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
				}

				hitmarkers::singleton()->on_paint();

				if (g_Options.other_lby_indicator && g_LocalPlayer && g_LocalPlayer->IsAlive())
				{
					const wchar_t* buf = L"LBY";

					int w, h;

					g_EngineClient->GetScreenSize(w, h);

					g_VGuiSurface->DrawSetTextFont(indicator_font);

					if ((g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_LocalPlayer->m_vecVelocity().Length2D() < 1.f || !(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
					{
						g_VGuiSurface->DrawSetTextColor(Color(165, 250, 10));
					}
					else
					{
						g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0));
					}

					g_VGuiSurface->DrawSetTextPos(8, h - 70 );
					g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPlaySound(const char* name)
	{
		static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

		oPlaySound(g_VGuiSurface, name);
	}
	//--------------------------------------------------------------------------------
	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, a1);
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);

		static QAngle m_aimPunchAngle, m_viewPunchAngle;

		if (g_LocalPlayer) {
			if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			{
				if (g_Options.other_no_flash)
				{
					g_LocalPlayer->m_flFlashMaxAlpha() = 0.f;
				}
				else {
					g_LocalPlayer->m_flFlashMaxAlpha() = 255.f;
				}
			}

			if (g_LocalPlayer->IsAlive())
			{
				if (stage == FRAME_RENDER_START) {
					IClientEntity *pLocal = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

					if (*(bool*)((DWORD)g_Input + 0xA5))
						*(QAngle*)((DWORD)pLocal + 0x31C8) = angles;
				}

				if (g_Options.other_no_visual_recoil) {
					m_aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();
					m_viewPunchAngle = g_LocalPlayer->m_viewPunchAngle();

					g_LocalPlayer->m_aimPunchAngle() = QAngle{ 0.f, 0.f, 0.f };
					g_LocalPlayer->m_viewPunchAngle() = QAngle{ 0.f, 0.f, 0.f };
				}
			}
		}

		ofunc(g_CHLClient, stage);

		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_Options.other_no_visual_recoil) {
			g_LocalPlayer->m_aimPunchAngle() = m_aimPunchAngle;
			g_LocalPlayer->m_viewPunchAngle() = m_viewPunchAngle;
		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);

		const auto mdl = pInfo.pModel;

		if (strstr(mdl->szName, "models/player") != nullptr)
		{
			auto ent = C_BasePlayer::GetPlayerByIndex(pInfo.entity_index);

			if (g_LocalPlayer && ent)
			{
				if (g_LocalPlayer->IsAlive() && g_LocalPlayer->m_bIsScoped() && ent == C_BasePlayer::GetPlayerByIndex(g_LocalPlayer->EntIndex()))
				{
					if (g_Options.other_thirdperson)
					{
						g_RenderView->SetBlend(0.3f);
					}
					else 
					{
						g_RenderView->SetBlend(1.f);
					}
				}
			}
		}

		ofunc(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
}
