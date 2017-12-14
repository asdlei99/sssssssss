#include "ragebot.hpp"
#include "valve_sdk\sdk.hpp"

#include "helpers\math.hpp"
#include "helpers\utils.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "options.hpp"
#include "autowall.hpp"

#include "weapon_ids.hpp"

int curBox;

float Ragebot::GetHitchance()
{
	C_BaseCombatWeapon* pWeapon = g_LocalPlayer->m_hActiveWeapon();

	float hitchance = 101;

	if (!pWeapon) 
		return 0;

	float inaccuracy = pWeapon->GetInaccuracy();

	if (inaccuracy == 0) 
		inaccuracy = 0.0000001;

	inaccuracy = 1 / inaccuracy;
	hitchance = inaccuracy;

	return hitchance;
}

void Ragebot::OnCreateMove(CUserCmd * cmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || GetClosest(cmd) == -1 || (g_WeaponIDs.IsMisc(g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()) && g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex() != g_WeaponIDs.ZEUS_X27))
	{
		return;
	}

	C_BasePlayer* target = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(GetClosest(cmd)));

	if (WEAPON_REVOLVER && g_Options.ragebot_autorevolver)
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			cmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}

	if (target != nullptr)
	{
		if (IsValid(target))
		{
			if (g_Options.ragebot_auto_scope)
			{
				if (!g_LocalPlayer->m_bIsScoped() && g_WeaponIDs.HasScope(g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()))
				{
					cmd->buttons |= IN_ATTACK2;
				}
			}

			if (cmd->buttons & IN_USE || !g_LocalPlayer->m_hActiveWeapon()->CanFire() || GetHitchance() < g_Options.ragebot_hitchance)
			{
				return;
			}

			QAngle angle = Math::calcAngle(target->GetHitboxPos(curBox));
			angle.normalize();

			if (!g_Options.ragebot_silent)
			{
				g_EngineClient->SetViewAngles(angle);
			}

			cmd->viewangles = angle;

			if (g_Options.ragebot_auto_shoot && g_LocalPlayer->m_hActiveWeapon()->CanFire())
			{
				cmd->buttons |= IN_ATTACK;
			}
		}
	}
}

void Ragebot::UpdateHitbox(C_BasePlayer* player)
{
	curBox = GetHitbox(player);
}

bool Ragebot::IsValid(C_BasePlayer* entity)
{
	if (!entity)
		return false;
	if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_Options.ragebot_target_team)
		return false;
	if (entity->m_bGunGameImmunity())
		return false;
	if (entity->IsDormant())
		return false;
	if (entity->m_iHealth() < 1)
		return false;
	if (g_Options.ragebot_auto_wall)
	{
		if (!g_LocalPlayer->CanSeePlayer(entity, curBox))
		{
			Vector point = entity->GetHitboxPos(curBox);
			float dmg = g_Options.ragebot_minimum_damage == 100 ? entity->m_iHealth() : g_Options.ragebot_minimum_damage;
			if (GetDamage(point) < dmg)
			{
				return false;
			}
		}
	}
	else
	{
		if (!g_LocalPlayer->CanSeePlayer(entity, entity->GetHitboxPos(curBox)))
			return false;
	}
	if (entity == g_LocalPlayer)
		return false;
	if (Math::get3dDistance(g_LocalPlayer->GetBonePos(8), entity->GetHitboxPos(curBox)) > g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData()->flRange)
		return false;
	return true;
}

bool IsValidNoHBCheck(C_BasePlayer* entity)
{
	if (!entity)
		return false;
	if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_Options.ragebot_target_team)
		return false;
	if (entity->m_bGunGameImmunity())
		return false;
	if (entity->IsDormant())
		return false;
	if (entity->m_iHealth() < 1)
		return false;
	if (entity == g_LocalPlayer)
		return false;
	if (Math::get3dDistance(g_LocalPlayer->GetBonePos(8), entity->GetHitboxPos(curBox)) > g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData()->flRange)
		return false;
	return true;
}

int Ragebot::GetClosest(CUserCmd* cmd)
{
	float curDist = -1;
	int closest = -1;

	for (auto i = 0; i < g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!IsValidNoHBCheck(entity))
			continue;

		UpdateHitbox(entity);

		if (!IsValid(entity))
			continue;

		QAngle angle;
		g_EngineClient->GetViewAngles(angle);

		double dif = Math::GetFov(angle, Math::calcAngle(entity->GetHitboxPos(curBox)));

		if (dif < curDist || curDist == -1 && dif <= g_Options.ragebot_fov / 2)
		{
			curDist = dif;
			closest = i;
		}
	}

	return closest;
}

int GetDefinedHitbox()
{
	switch (g_Options.ragebot_hitbox)
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
	return HITBOX_HEAD;
}

int Ragebot::GetHitbox(C_BasePlayer* player)
{
	int hitboxes[] = { HITBOX_HEAD, HITBOX_NECK, HITBOX_LOWER_NECK, HITBOX_LEFT_UPPER_ARM, HITBOX_RIGHT_UPPER_ARM, HITBOX_LEFT_FOREARM, HITBOX_RIGHT_FOREARM, HITBOX_UPPER_CHEST, HITBOX_CHEST, HITBOX_LOWER_CHEST, HITBOX_STOMACH, HITBOX_PELVIS, HITBOX_LEFT_THIGH, HITBOX_RIGHT_THIGH, HITBOX_LEFT_CALF, HITBOX_RIGHT_CALF, HITBOX_LEFT_FOOT, HITBOX_RIGHT_FOOT };
	int curHitBox = GetDefinedHitbox();

	if (g_Options.ragebot_hitscan)
	{
		Vector point = player->GetHitboxPos(curHitBox);
		float dmg = g_Options.ragebot_minimum_damage == 100 ? player->m_iHealth() : g_Options.ragebot_minimum_damage;

		if (GetDamage(point) >= dmg || g_LocalPlayer->CanSeePlayer(player, curHitBox))
		{
			return curHitBox;
		}
		else
		{
			for (int i = 0; i < 18; i++)
			{
				Vector point = player->GetHitboxPos(hitboxes[i]);
				float dmg = g_Options.ragebot_minimum_damage == 100 ? player->m_iHealth() : g_Options.ragebot_minimum_damage;

				if (GetDamage(point) >= dmg || g_LocalPlayer->CanSeePlayer(player, hitboxes[i]))
				{
					return hitboxes[i];
				}
			}
		}
	}
	else
	{
		return curHitBox;
	}

	return curHitBox;
}