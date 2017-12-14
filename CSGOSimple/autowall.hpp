#pragma once

#include "valve_sdk\math\Vector.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "valve_sdk\interfaces\IEngineTrace.hpp"

struct FireBulletData
{
	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

bool AutoWall(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point);

float GetDamage(Vector& point);

float GetHitgroupDamage(int iHitgroup);

void ScaleDamage(int Hitgroup, C_BaseEntity* pEntity, float weapon_armor_ratio, float& current_damage);

static bool DidHitNonWorldEntity(C_BaseEntity* m_pEnt);

bool TraceToExit(Vector& end, trace_t* enter_trace, Vector start, Vector dir, trace_t* exit_trace);

bool SimulateFireBullet(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, FireBulletData& data);

bool HandleBulletPenetration(CCSWeaponInfo* wpn_data, FireBulletData& data);

bool SimulateBullet(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point, C_BaseEntity* target);

float SimulateBulletDamage(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point, C_BaseEntity* target);

float Invasive(Vector vEnd);

float bIsPointPenetrable(Vector vEnd);