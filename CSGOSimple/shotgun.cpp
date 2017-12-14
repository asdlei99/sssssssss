#include "shotgun.hpp"

#include "helpers/math.hpp"

#include "options.hpp"

double Shotgun::GetAngleDifference(double alpha, double beta)
{
	double phi = abs((long)(beta - alpha)) % 360;
	double distance = phi > 180 ? 360 - phi : phi;
	return distance;
}

QAngle Shotgun::SmoothAngle(QAngle angles, QAngle destination, double x, double y)
{
	QAngle smoothed = destination.operator -= (angles);

	smoothed.normalize();

	smoothed.pitch /= x;
	smoothed.yaw /= y;
	smoothed.operator += (angles);

	smoothed.normalize();

	return smoothed;
}

QAngle Shotgun::CalculateAngle(Vector target)
{
	QAngle angle = QAngle();
	Vector source = g_LocalPlayer->GetEyePos();
	Vector delta = Vector(source.x - target.x, source.y - target.y, source.z - target.z);

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.pitch = RAD2DEG(atan(delta.z / hyp));
	angle.yaw = RAD2DEG(atan(delta.y / delta.x));

	if (g_Options.shotgun_rcs)
	{
		angle.pitch -= g_LocalPlayer->m_aimPunchAngle().pitch * 2;
		angle.yaw -= g_LocalPlayer->m_aimPunchAngle().yaw * 2;
	}

	if (delta.x >= 0)
	{
		angle.yaw = angle.yaw + 180;
	}

	return angle;
}

int Shotgun::GetHitbox(C_BasePlayer* player)
{
	switch (g_Options.sniper_hitbox)
	{
	case 0:
		return HITBOX_HEAD;
	case 1:
		return HITBOX_NECK;
	case 2:
		return HITBOX_CHEST;
	case 3:
		return HITBOX_PELVIS;
	}
}

void Shotgun::OnCreateMove(CUserCmd * cmd)
{
	if (!g_LocalPlayer && !g_LocalPlayer->IsAlive() || GetClosest(cmd) == -1 || g_LocalPlayer->m_hActiveWeapon()->IsKnife() || g_LocalPlayer->m_hActiveWeapon()->IsGrenade())
	{
		return;
	}
	if (cmd->buttons & IN_USE || !(cmd->buttons & IN_ATTACK))
	{
		return;
	}
	C_BasePlayer* target = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(GetClosest(cmd)));

	if (target)
	{
		if (IsValid(target))
		{
			QAngle angle = SmoothAngle(cmd->viewangles, CalculateAngle(target->GetHitboxPos(GetHitbox(target))), g_Options.shotgun_smooth_x, g_Options.shotgun_smooth_y);

			angle.normalize();

			if (!g_Options.shotgun_silent)
			{
				g_EngineClient->SetViewAngles(angle);
			}
			else {
				cmd->viewangles = angle;
			}
		}
	}
}


bool Shotgun::IsValid(C_BasePlayer* entity)
{
	if (!entity)
		return false;
	if (!g_LocalPlayer->CanSeePlayer(entity, entity->GetHitboxPos(GetHitbox(entity))))
		return false;
	if (entity == g_LocalPlayer)
		return false;
	if (entity->m_bGunGameImmunity())
		return false;
	if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_Options.shotgun_target_team)
		return false;
	if (entity->IsDormant())
		return false;
	if (entity->m_iHealth() < 1)
		return false;
	return true;
}

int Shotgun::GetClosest(CUserCmd* cmd)
{
	float curDist = -1;
	int closest = -1;

	for (auto i = 0; i < g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!IsValid(entity))
		{
			continue;
		}

		double dif = Math::GetFov(cmd->viewangles, CalculateAngle(entity->GetHitboxPos(GetHitbox(entity))));

		if (dif < curDist || curDist == -1 && dif < g_Options.shotgun_fov)
		{
			curDist = dif;
			closest = i;
		}
	}
	return closest;
}