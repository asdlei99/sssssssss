#include "autowall.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "helpers\math.hpp"
#include "helpers\utils.hpp"
#include <algorithm>
#include "options.hpp"

#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10

using namespace std;

float GetHitgroupDamageMult(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 2.0f;
	case HITGROUP_CHEST:
		return 0.5f;
	case HITGROUP_STOMACH:
		return 0.75f;
	case HITGROUP_LEFTARM:
		return 0.5f;
	case HITGROUP_RIGHTARM:
		return 0.5f;
	case HITGROUP_LEFTLEG:
		return 0.375f;
	case HITGROUP_RIGHTLEG:
		return 0.375f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;

	}

	return 1.0f;
}

float GetHitgroupDamage(int iHitgroup)
{
	switch (iHitgroup)
	{
	case HITGROUP_GENERIC:
		return 1.0f;
	case HITGROUP_HEAD:
		return 4.0f;
	case HITGROUP_CHEST:
		return 1.0f;
	case HITGROUP_STOMACH:
		return 1.5f;
	case HITGROUP_LEFTARM:
		return 1.0f;
	case HITGROUP_RIGHTARM:
		return 1.0f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 1.0f;
	default:
		break;
	}

	return 1.0f;
}





bool TraceToExit(Vector& end, trace_t* enter_trace, Vector start, Vector dir, trace_t* exit_trace)
{
	auto distance = 0.0f;

	while (distance < 90.1f)
	{
		distance += 4.0f;
		end = start + dir * distance;

		auto point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		auto new_end = end - (dir * 4.0f);

		Ray_t ray;
		ray.Init(end, new_end);
		g_EngineTrace->TraceRay(ray, 0x4600400B, nullptr, exit_trace);

		if (exit_trace->hit_entity == nullptr)
			return false;

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
		{
			CTraceFilter filter;
			filter.pSkip = exit_trace->hit_entity;

			ray.Init(end, start);
			g_EngineTrace->TraceRay(ray, 0x600400B, &filter, exit_trace);

			if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}

			continue;
		}

		if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid) || exit_trace->startsolid)
		{
			if (exit_trace->hit_entity)
			{
				if (!DidHitNonWorldEntity(enter_trace->hit_entity))
					return true;
			}

			continue;
		}

		if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace->surface.flags >> 7) & 1))
			continue;

		if (exit_trace->plane.normal.Dot(dir) < 1.1f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}

	return false;
}
bool DidHitNonWorldEntity(C_BaseEntity* m_pEnt)
{
	return m_pEnt != nullptr && m_pEnt == g_EntityList->GetClientEntity(0);
}

void ScaleDamage(int Hitgroup, C_BaseEntity* pEntity, float weapon_armor_ratio, float& current_damage)
{
	if (!pEntity)
		return;

	current_damage *= GetHitgroupDamage(Hitgroup);
	int Armor = pEntity->m_ArmorValue();
	if (Armor > 0)
	{
		if (Hitgroup == HITGROUP_HEAD)
		{
			bool Helmet = pEntity->m_bHasHelmet();
			if (Helmet)
				current_damage *= (weapon_armor_ratio * 1.5f);
		}
		else
		{
			current_damage *= (weapon_armor_ratio * 0.5f);
		}
	}
}

bool HandleBulletPenetration(CCSWeaponInfo* wpn_data, FireBulletData& data)
{
	if (!wpn_data)
		return false;

	auto enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	if (!enter_surface_data)
		return false;

	int enter_material = enter_surface_data->game.material;
	auto enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= static_cast<float>(pow(wpn_data->flRangeModifier, (data.trace_length * 0.002)));

	if ((data.trace_length > 3000.0f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	auto exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	auto exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	if (!exit_surf_penetration_mod)
		return false;

	auto final_damage_modifier = 0.16f;
	auto combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	auto v34 = fmaxf(0.0f, 1.0f / combined_penetration_modifier);
	auto v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->flPenetration) * 1.25f);
	auto thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.f;

	auto lost_damage = fmaxf(0.0f, v35 + thickness) - 1.6f;
	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}



bool SimulateFireBullet(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, FireBulletData& data)
{
	if (!pLocal || !pWeapon)
		return false;

	data.penetrate_count = 4;
	data.trace_length = 0.0f;

	auto wpn_data = pWeapon->GetCSWeaponData();
	if (!wpn_data)
		return false;

	data.current_damage = static_cast<float>(wpn_data->iDamage);

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->flRange - data.trace_length;
		auto vecEnd = data.src + data.direction * 40.f;

		Ray_t ray;
		ray.Init(data.src, vecEnd + data.direction * data.trace_length_remaining);
		g_EngineTrace->TraceRay(ray, 0x4600400B, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && pLocal->m_iTeamNum() != data.enter_trace.hit_entity->m_iTeamNum())
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= static_cast<float>(pow(wpn_data->flRangeModifier, data.trace_length * 0.002));
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.hit_entity, wpn_data->flArmorRatio, data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(wpn_data, data))
			break;
	}

	return false;
}

void sinCos2(float radians, PFLOAT sine, PFLOAT cosine)
{
	__asm
	{
		fld dword ptr[radians]
		fsincos
		mov edx, dword ptr[cosine]
		mov eax, dword ptr[sine]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void angleVectors2(QAngle angles, Vector& f)
{
	float sp, sy, sr, cp, cy, cr;

	sinCos2(DEG2RAD(angles[0]), &sp, &cp);
	sinCos2(DEG2RAD(angles[1]), &sy, &cy);
	sinCos2(DEG2RAD(angles[2]), &sr, &cr);

	f[0] = cp * cy;
	f[1] = cp * sy;
	f[2] = -sp;
}

bool AutoWall(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point)
{
	if (!pLocal)
		return false;
	if (!g_LocalPlayer->IsAlive())
		return false;

	FireBulletData data;
	data.src = g_LocalPlayer->GetEyePos();
	data.filter.pSkip = pLocal;

	QAngle vecAngles;
	Math::CalcAngle(data.src, point, vecAngles);
	angleVectors2(vecAngles, data.direction);
	data.direction.NormalizeInPlace();

	if (!SimulateFireBullet(pLocal, pWeapon, data))
		return false;

	return (data.current_damage >= g_Options.ragebot_minimum_damage);
}

float GetDamage(Vector& point)
{
	if (!g_LocalPlayer)
		return false;
	if (!g_LocalPlayer->IsAlive())
		return false;
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	FireBulletData data;
	data.src = g_LocalPlayer->GetEyePos();
	data.filter.pSkip = g_LocalPlayer;

	QAngle vecAngles;
	Math::CalcAngle(data.src, point, vecAngles);
	angleVectors2(vecAngles, data.direction);
	data.direction.NormalizeInPlace();

	if (!SimulateFireBullet(g_LocalPlayer, pWeapon, data))
		return false;
	return data.current_damage;
}
bool SimulateBullet(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point, C_BaseEntity* target)
{
	if (!pLocal)
		return false;
	if (!g_LocalPlayer->IsAlive())
		return false;

	auto damage = 1.0f;
	Ray_t ray;
	trace_t tr;
	CTraceFilter filter;
	auto vStart = g_LocalPlayer->GetEyePos();
	auto WepInfo = pWeapon->GetCSWeaponData();
	static bool(__thiscall *BulletHandler)(C_BaseEntity* LocalPlayer, float& penetration, int& SurfaceMat, int* usestaticvalues, trace_t* ray, Vector* normaldelta,
		float _setto0f_8, float surfacepenetration, float damagemultiplier, int unknown, int _setto0x1002_12, float penetration2, int* hitsleft,
		Vector* ResultPos, float hitx, float hity, float* fldamage) = nullptr;
	if (!BulletHandler)
		BulletHandler = reinterpret_cast<decltype(BulletHandler)>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), " 55 8B EC 83 EC 70 8B 55 34"));
	auto SurfaceData = g_PhysSurface->GetSurfaceData(tr.surface.surfaceProps);
	auto staticvalue = 1;
	auto Normal = (point - vStart);
	Normal.NormalizeInPlace();
	auto leftLength = Normal.Length();
	int matID = SurfaceData->game.material;
	auto v04 = 4;
	auto result(vStart);
	do
	{
		ray.Init(result, point);
		g_EngineTrace->TraceRay(ray, 0x4600400B, &filter, &tr);
		if (tr.hitgroup != 0)break;
		if (BulletHandler(pLocal, WepInfo->flPenetration, matID, &staticvalue, &tr, &Normal,
			0.f, SurfaceData->game.flPenetrationModifier, SurfaceData->game.flDamageModifier, 0, 0x1002, WepInfo->flPenetration, &v04,
			&result, 0.f, 0.f, &damage))
			break;
	} while (v04 > 0);
	damage *= powf(WepInfo->flRangeModifier, vStart.DistToSqr(result) * 0.002);
	if (tr.hitgroup != 0)
		return tr.hit_entity && tr.hit_entity->EntIndex() == target->EntIndex() && damage >= g_Options.ragebot_minimum_damage;
	return false;
}
float SimulateBulletDamage(C_BaseEntity* pLocal, C_BaseCombatWeapon* pWeapon, Vector& point, C_BaseEntity* target)
{
	if (!pLocal)
		return false;
	if (!g_LocalPlayer->IsAlive())
		return false;

	float damage = pWeapon->GetCSWeaponData()->iDamage;
	Ray_t ray;
	trace_t tr;
	CTraceFilter filter;
	filter.pSkip = pLocal;
	auto vStart = g_LocalPlayer->GetEyePos();
	auto WepInfo = pWeapon->GetCSWeaponData();
	static bool(__thiscall *BulletHandler)(C_BaseEntity* LocalPlayer, float& penetration, int& SurfaceMat, int* usestaticvalues, trace_t* tray, Vector* normaldelta,
		float _setto0f_8, float surfacepenetration, float damagemultiplier, int unknown, int _setto0x1002_12, float penetration2, int* hitsleft,
		Vector* ResultPos, float hitx, float hity, float* fldamage) = nullptr;
	if (!BulletHandler)
		BulletHandler = reinterpret_cast<decltype(BulletHandler)>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), " 55 8B EC 83 EC 70 8B 55 34"));
	auto SurfaceData = g_PhysSurface->GetSurfaceData(tr.surface.surfaceProps);
	auto staticvalue = 1;
	auto Normal = (point - vStart);
	Normal.NormalizeInPlace();
	auto leftLength = Normal.Length();
	int matID = SurfaceData->game.material;
	auto v04 = 4;
	auto result(vStart);
	while (v04 > 0)
	{
		ray.Init(result, point);
		g_EngineTrace->TraceRay(ray, 0x4600400B, &filter, &tr);

		if (tr.hitgroup != 0)
		{
			break;
		}

		if (BulletHandler(pLocal, WepInfo->flPenetration, matID, &staticvalue, &tr, &Normal,
			0.f, SurfaceData->game.flPenetrationModifier, SurfaceData->game.flDamageModifier, 0, 0x1002, WepInfo->flPenetration, &v04,
			&result, 0.f, 0.f, &damage))
			break;
	}
	damage *= powf(WepInfo->flRangeModifier, vStart.DistToSqr(result) * 0.002);
	if (tr.hitgroup != 0)
		return  damage;
	return false;
}

trace_t trace;
trace_t trace2;
// pPaste Starts here, blame yoss and sigman for pPASTE
bool bTraceExit(Vector &vStartyPosy, Vector &vDir)
{
	float multiply = 0;

	while (multiply < 90.1f)
	{
		multiply += 4.0f;

		auto vDaStart = (vDir * multiply) + vStartyPosy;
		auto vDaEnd = vDaStart - (vDir * 4.0f);

		auto point_contents = (g_EngineTrace->GetPointContents(vDaStart, 0x4600400B));

		Ray_t ray2;

		if ((point_contents & 0x600400B) && (!(point_contents & 0x40000000)))
		{
			continue;
		}

		ray2.Init(vDaStart, vDaEnd);
		g_EngineTrace->TraceRay(ray2, 0x4600400B, nullptr, &trace2);

		if (trace2.startsolid && (trace2.surface.flags & 0x8000))
		{
			auto pNewTraceEnt = trace2.hit_entity;

			CTraceFilter tracefilter_new;
			tracefilter_new.pSkip = pNewTraceEnt;

			Ray_t ray3;
			ray3.Init(vDaStart, vStartyPosy);
			g_EngineTrace->TraceRay(ray3, 0x600400B, reinterpret_cast<CTraceFilter*>(&tracefilter_new), &trace2);

			if ((trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) && !trace2.startsolid)
			{
				vDaStart = trace2.endpos;
				return true;
			}

			continue;
		}

		if (!(trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) || trace2.startsolid)
		{
			auto pBreakEnt = trace.hit_entity;

			if (pBreakEnt)
			{
				auto pWorldEnt = g_EntityList->GetClientEntity(0);

				if (pBreakEnt != pWorldEnt)
				{
					static DWORD dwEntBreakable = NULL;

					if (dwEntBreakable == NULL)
					{
						dwEntBreakable = (DWORD)Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68");
					}
					bool bRet;

					__asm
					{
						MOV ECX, pBreakEnt
						CALL dwEntBreakable
						MOV bRet, AL
					}
					if (bRet)
						return true;
				}
			}

			continue;
		}

		if (((trace2.surface.flags >> 7) & 1) && !((trace.surface.flags >> 7) & 1))
			continue;

		auto fWanker = ((trace2.plane.normal.x * vDir.x) + (trace2.plane.normal.y * vDir.y) + (trace2.plane.normal.z * vDir.z));

		if (fWanker < 1.1f)
		{
			auto fMody = trace2.fraction * 4.0f;

			auto vGody = vDaStart - (vDir * fMody);

			vDaStart = vGody;
			return true;
		}
	}
	return false;
}

float Invasive(Vector vEnd)
{
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon) return 0.f;
	auto wiWeaponInfo = pWeapon->GetCSWeaponData();
	auto vStart = g_LocalPlayer->GetEyePos();

	static trace_t traceEmpty;

	trace = traceEmpty;

	static CTraceFilter* pTraceFilter;

	Vector vSource = vStart, vDir = (vEnd - vStart), vClip;
	vDir.NormalizeInPlace();

	float fTmpLength, fSumDist, fPow;

	float iCurrentDamage = wiWeaponInfo->iDamage;


	auto fDamageCheck = 1.0f;

	auto iPenetration = 4;

	static DWORD dwCliptracetoplayers = NULL;

	if (dwCliptracetoplayers == NULL)
	{
		dwCliptracetoplayers = (DWORD)Utils::PatternScan(GetModuleHandleW(L"client.dll"), "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC 00 00 00 00 8B 43 10");

	}


	auto fRange = Vector(vEnd - vStart).Length();


	while (iCurrentDamage > 0.0f)
	{
		Ray_t ray;
		ray.Init(vSource, vEnd);

		g_EngineTrace->TraceRay(ray, 0x4600400B, pTraceFilter, &trace);

		vClip = (vDir * 40.0f) + vEnd;


		_asm
		{
			MOV EAX, pTraceFilter
			LEA ECX, trace
			PUSH ECX
			PUSH EAX
			PUSH 0x4600400B
			LEA EDX, vClip
			LEA ECX, vSource
			CALL dwCliptracetoplayers
			ADD ESP, 0x0C
		}


		if (trace.fraction != 1.0f)
		{

			auto pSurfaceData = g_PhysSurface->GetSurfaceData(trace.surface.surfaceProps);

			auto material = *reinterpret_cast<USHORT*>(reinterpret_cast<DWORD>(pSurfaceData) + 84);
			auto fPenetrationPowerModifier = *reinterpret_cast<PFLOAT>(reinterpret_cast<DWORD>(pSurfaceData) + 76);

			if (iPenetration == 4)
				fTmpLength = fRange * trace.fraction;
			else
				fTmpLength = (fSumDist + ((fRange - fSumDist) * trace.fraction));

			fPow = (pow(wiWeaponInfo->flRangeModifier, (fTmpLength * 0.002f)));

			iCurrentDamage = iCurrentDamage * fPow;
			bool a5 = (trace.contents >> 3) & 1;

			bool v104 = (trace.surface.flags >> 7) & 1;

			auto v17 = false;

			if (iPenetration == 0 && !a5 && !v104 && material != 71 && material != 89)
				v17 = true;

			if (wiWeaponInfo->flPenetration < 0.0000001f || iPenetration < 1)
				v17 = true;

			trace2 = traceEmpty;

			auto vStartyPosy = trace.endpos;

			if (!bTraceExit(vStartyPosy, vDir)
				&& !(g_EngineTrace->GetPointContents(vStartyPosy, 0x600400B) & 0x600400B))
			{
				break;
			}

			if (v17)
			{
				break;
			}

			auto pSurfaceData2 = g_PhysSurface->GetSurfaceData(trace2.surface.surfaceProps);

			auto material2 = *reinterpret_cast<USHORT*>(reinterpret_cast<DWORD>(pSurfaceData2) + 84);
			auto fPenetrationPowerModifer2 = *reinterpret_cast<PFLOAT>(reinterpret_cast<DWORD>(pSurfaceData2) + 76);

			auto v70 = 0.16f;
			float v31;

			if (a5 || v104)
			{
				if (material != 89 && material != 71)
					v31 = 1.0f;
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
			}
			else
			{
				if (material != 89)
				{
					if (material != 71)
					{
						if (material == 70)
						{
							v31 = wiWeaponInfo->flPenetration;
						}
						else
						{

							v31 = (fPenetrationPowerModifer2 + fPenetrationPowerModifier) * 0.5f;
							v70 = 0.16f;
						}
					}
					else
					{
						v31 = 3.0f;
						v70 = 0.05f;
					}
				}
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
			}

			if (material == material2)
			{
				if (material2 == 87 || material2 == 85)
					v31 = 3.0f;
				else
				{
					if (material2 == 76)
						v31 = 2.0f;
				}
			}

			auto v34 = 1.0f;
			v34 /= v31;

			if (v34 < 0.0f)
				v34 = 0.0f;

			auto fNewTmpLength = Vector(trace2.endpos - trace.endpos).Length();

			auto fMine = 3.0f;
			fMine /= wiWeaponInfo->flPenetration;
			fMine *= 1.25f;

			auto v55 = 0.0f;

			if (fMine > 0.0f)
				v55 = fMine;

			auto v35 = v34;
			v35 *= 3.0f;
			v35 *= v55;

			v35 += (iCurrentDamage * v70);

			auto fShiz = fNewTmpLength;
			fShiz *= fNewTmpLength;
			fShiz *= v34;
			fShiz /= 24.0f;

			auto fBisc = (v35 + fShiz);

			auto v69 = 0.0f;

			if (fBisc > 0.0f)
				v69 = fBisc;

			auto fNewy = (iCurrentDamage - v69);

			iCurrentDamage = fNewy;

			if (fNewy < fDamageCheck)
				break;

			vSource = trace2.endpos;

			fSumDist = fTmpLength + fNewTmpLength;

			--iPenetration;
		}
		else
			return iCurrentDamage;
	}

}

float bIsPointPenetrable(Vector vEnd)
{
	return GetDamage(vEnd);
}