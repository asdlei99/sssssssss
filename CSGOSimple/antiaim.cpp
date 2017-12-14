#include "antiaim.hpp"

#include "valve_sdk/csgostructs.hpp"

#include "weapon_ids.hpp"
#include "options.hpp"
#include "helpers\utils.hpp"
#include "helpers/math.hpp"

// b1g unknowncheats pasta
float GetBestHeadAngle(C_BasePlayer* player, float yaw)
{
	float Back, Right, Left;

	Vector src3D, dst3D, forward, right, up, src, dst;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	QAngle engineViewAngles;
	g_EngineClient->GetViewAngles(engineViewAngles);

	engineViewAngles.pitch = 0;

	Math::AngleVectors(engineViewAngles, forward, right, up);

	filter.pSkip = player;
	src3D = player->GetEyePos();
	dst3D = src3D + (forward * 384);

	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	Back = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);

	g_EngineTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);

	Right = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);

	g_EngineTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);

	Left = (tr.endpos - tr.startpos).Length();

	if (Left > Right)
	{
		return (yaw - 90);
	}
	else if (Right > Left)
	{
		return (yaw + 90);
	}
	else if (Back > Right || Back > Left)
	{
		return (yaw - 180);
	}
	return 0;
}

float GetBestHeadAngleFAKE(C_BasePlayer* player, float yaw)
{
	float Back, Right, Left;

	Vector src3D, dst3D, forward, right, up, src, dst;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	QAngle engineViewAngles;
	g_EngineClient->GetViewAngles(engineViewAngles);

	engineViewAngles.pitch = 0;

	Math::AngleVectors(engineViewAngles, forward, right, up);

	filter.pSkip = player;
	src3D = player->GetEyePos();
	dst3D = src3D + (forward * 384);

	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	Back = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);

	g_EngineTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);

	Right = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);

	g_EngineTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);

	Left = (tr.endpos - tr.startpos).Length();

	if (Left > Right)
	{
		return (yaw + 90);
	}
	else if (Right > Left)
	{
		return (yaw - 90);
	}
	else if (Back > Right || Back > Left)
	{
		return (yaw + 180);
	}
	return 0;
}

void AntiAim::OnCreateMove(CUserCmd* cmd, bool &bSendPacket)
{
	static int choked = -1;

	static QAngle angles;
	static float yaw;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !g_LocalPlayer->m_hActiveWeapon() || g_LocalPlayer->m_bGunGameImmunity() || cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK)
		return;

	if (g_WeaponIDs.IsGrenade(g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()))
	{
		if (g_LocalPlayer->m_hActiveWeapon()->m_fThrowTime() > 0.f)
			return;
	}

	switch (g_Options.anti_aim_pitch)
	{
	case 0: break; // none
	case 1: cmd->viewangles.pitch = -89.0F; break; // up
	case 2: cmd->viewangles.pitch = 89.0F; break; // down
	case 3: cmd->viewangles.pitch = 85.0F; break; // minimal
	}

	choked++;

	if (choked < 1)
	{
		bSendPacket = false;

		switch (g_Options.anti_aim_yaw)
		{
		case 0: break; // none
		case 1: cmd->viewangles.yaw -= 180.0F; break; // backwards
		case 2: cmd->viewangles.yaw += 90.0F; break; // left
		case 3: cmd->viewangles.yaw -= 90.0F; break; // right
		case 4: cmd->viewangles.yaw += 90.0F + rand() % (64 - -64 + 1) + -64; break; // left random skeet meme(tm)
		case 5: cmd->viewangles.yaw -= 90.0F + rand() % (64 - -64 + 1) + -64; break; // left random skeet meme(tm)
		case 6:
			g_EngineClient->GetViewAngles(angles);

			cmd->viewangles.yaw = angles.yaw;
			break; // local view
		case 7:
			yaw += g_Options.anti_aim_spin_speed;

			if (yaw > 180) {
				yaw = -180;
			}

			cmd->viewangles.yaw = yaw;
			break; // spin
		case 8: 
			cmd->viewangles.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
			break; // lower body
		case 9:
			g_EngineClient->GetViewAngles(angles);

			cmd->viewangles.yaw = GetBestHeadAngle(g_LocalPlayer, angles.yaw);
			break; // freestanding
		}

		cmd->viewangles.yaw += g_Options.anti_aim_yaw_offset;
	}
	else
	{
		bSendPacket = true;

		switch (g_Options.anti_aim_fake_yaw)
		{
		case 0: break; // none
		case 1: cmd->viewangles.yaw -= 180.0F; break; // backwards
		case 2: cmd->viewangles.yaw += 90.0F; break; // left
		case 3: cmd->viewangles.yaw -= 90.0F; break; // right
		case 4: cmd->viewangles.yaw += 90.0F + rand() % (64 - -64 + 1) + -64; break; // left random skeet meme(tm)
		case 5: cmd->viewangles.yaw -= 90.0F + rand() % (64 - -64 + 1) + -64; break; // left random skeet meme(tm)
		case 6:
			g_EngineClient->GetViewAngles(angles);

			cmd->viewangles.yaw = angles.yaw;
			break; // local view
		case 7:
			yaw += g_Options.anti_aim_spin_speed;

			if (yaw > 180) {
				yaw = -180;
			}

			cmd->viewangles.yaw = yaw;
			break; // spin
		case 8:
			cmd->viewangles.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
			break; // lower body
		case 9:
			g_EngineClient->GetViewAngles(angles);

			cmd->viewangles.yaw = GetBestHeadAngleFAKE(g_LocalPlayer, angles.yaw);
			break; // freestanding
		}

		cmd->viewangles.yaw += g_Options.anti_aim_fake_yaw_offset;

		choked = -1;
	}

	cmd->viewangles.normalize();
}