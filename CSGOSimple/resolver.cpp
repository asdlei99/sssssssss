#include "resolver.hpp"
#include "options.hpp"

#include "valve_sdk/csgostructs.hpp"

bool IsMoving(C_BasePlayer* player)
{
	return player->m_vecVelocity().Length2D() > 0.1f && player->m_fFlags() & FL_ONGROUND;
}

bool IsFakeWalking(C_BasePlayer* player)
{
	return IsMoving(player) && player->m_vecVelocity().Length2D() < 35.f;
}

RecvVarProxyFn Resolver::oYawProxy;

void ResolverYawProxy(const CRecvProxyData* data, void *ent, void* out)
{
	if (g_LocalPlayer && ent) {
		C_BasePlayer* player = (C_BasePlayer*)ent;

		float m_flYaw = data->m_Value.m_Float;

		if (g_Options.ragebot_enabled && g_Options.ragebot_resolver && player && player->IsAlive() && !player->IsDormant() && ent != g_LocalPlayer)
		{
			if (IsMoving(player))
			{
				m_flYaw = player->m_flLowerBodyYawTarget();
			}
			else if (IsFakeWalking(player))
			{
				m_flYaw = m_flYaw + 180;
				m_flYaw = m_flYaw - 180;
			}
			else
			{
				float delta = m_flYaw - player->m_flLowerBodyYawTarget();

				if (player->m_flLowerBodyYawTarget() != player->m_flOldLowerBodyYawTarget || fabsf(delta) >= 35.f)
				{
					m_flYaw = player->m_flLowerBodyYawTarget();
					player->m_flOldLowerBodyYawTarget = player->m_flLowerBodyYawTarget();
				}
				else
				{
					if (fabsf(delta) < 35.f && fabsf(delta) > 0.f)
					{
						m_flYaw = player->m_flLowerBodyYawTarget() + delta;
					}
					if (fabsf(delta) < -35.f && fabsf(delta) > 0.f);
				}
			}

			*(float*)out = m_flYaw;
		}
		else
		{
			Resolver::oYawProxy(data, ent, out);
		}
	}
}

void Resolver::Initialize()
{
	oYawProxy = NetvarSys::Get().GetNetvarProp("DT_CSPlayer", "m_angEyeAngles[1]")->GetProxyFn();
	NetvarSys::Get().GetNetvarProp("DT_CSPlayer", "m_angEyeAngles[1]")->SetProxyFn(ResolverYawProxy);
}

void Resolver::Uninitialize()
{
	NetvarSys::Get().GetNetvarProp("DT_CSPlayer", "m_angEyeAngles[1]")->SetProxyFn(oYawProxy);
}