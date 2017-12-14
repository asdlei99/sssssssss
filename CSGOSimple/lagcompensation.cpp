#include "lagcompensation.hpp"
#include "helpers\math.hpp"
#include "valve_sdk\csgostructs.hpp"

#define LC_NONE				0
#define LC_ALIVE			(1<<0)

#define LC_ORIGIN_CHANGED	(1<<8)
#define LC_ANGLES_CHANGED	(1<<9)
#define LC_SIZE_CHANGED		(1<<10)
#define LC_ANIMATION_CHANGED (1<<11)

#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
// Allow 4 units of error ( about 1 / 8 bbox width )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )

template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&g_GlobalVars->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&g_GlobalVars->tickcount);
}

float inline GetEstimatedServerTime(C_BasePlayer *pLocalPlayer)
{

	return pLocalPlayer->m_nTickBase() * (float)((uintptr_t)& g_GlobalVars->interval_per_tick);
}

float GetNetworkLatency()
{
}

void CLagCompensation::Record()
{
	auto pLP = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	int iTickBase = pLP->m_nTickBase();

	float flCurrentTime = g_GlobalVars->interval_per_tick * iTickBase;

	for (int s = 1; s <= g_GlobalVars->maxClients; ++s)
	{
		m_PlayerRecords[s].ValidRecord = false;
		C_BasePlayer* pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(s));
		if (!pEntity)
			continue;

		if (!pEntity->IsAlive() || !pEntity->IsAlive() || !pEntity->IsDormant())
			continue;

		float flSimTime = pLP->m_flSimulationTime();

		// Origin delta differance rejection
		if (m_PlayerRecords[s].m_vecPreviousOrigin.Length() > 0)
		{
			Vector delta = pEntity->GetAbsOrigin() - m_PlayerRecords[s].m_vecPreviousOrigin; //
			if (delta.Length2DSqr() > 64)
			{
				m_PlayerRecords[s].m_vecPreviousOrigin = pEntity->GetAbsOrigin();
				m_PlayerRecords[s].m_flPreviousSimulationTime = pEntity->m_flSimulationTime();
				continue;
			}
		}
		else
		{
			m_PlayerRecords[s].m_vecPreviousOrigin = pEntity->GetAbsOrigin();
		}
		m_PlayerRecords[s].m_vecOrigin = pEntity->GetAbsOrigin();
		m_PlayerRecords[s].m_flSimulationTime = pEntity->m_flSimulationTime();

		// Time differance rejection
		if (fabs(flSimTime - flCurrentTime) > 1.0f)
			continue;

		float latency = GetNetworkLatency();
		const float SV_MAXUNLAG = 1.0f;
		float m_flLerpTime = g_CVar->FindVar("cl_interp_ratio")->GetFloat() / g_CVar->FindVar("cl_updaterate")->GetFloat();
		int lerpTicks = Math::TIME_TO_TICKS(m_flLerpTime);
		g_GlobalVars->tickcount += lerpTicks;
		float correct = clamp(latency + Math::TICKS_TO_TIME(lerpTicks), 0.0f, SV_MAXUNLAG);
		int targettick = g_GlobalVars->interval_per_tick - lerpTicks;
		float deltaTime = correct - Math::TICKS_TO_TIME(GetEstimatedServerTickCount(latency) - targettick);

		if (fabs(deltaTime) > 0.2f)
		{
			continue;
		}

		m_PlayerRecords[s].m_flSimulationTime = flSimTime;
		m_PlayerRecords[s].ValidRecord = true;

		//BacktrackEntity(pEntity, targettick);
	}
}
/*
void CLagCompensation::BacktrackPlayer(C_BaseEntity *pPlayer, float flTargetTime)
{
Vector org, mins, maxs;
QAngle ang;

int pl_index = pPlayer->EntIndex();

auto m_pCurrentPlayer = pPlayer;

float frac = 0.0f;
if (m_PlayerRecords[pl_index].ValidRecord &&
(m_PlayerRecords[pl_index].m_flSimulationTime < flTargetTime) &&
(m_PlayerRecords[pl_index].m_flSimulationTime < m_PrevPlayerTable[pl_index].m_flSimulationTime))
{
// we didn't find the exact time but have a valid previous record
// so interpolate between these two records;

Assert(prevRecord->m_flSimulationTime > record->m_flSimulationTime);
Assert(flTargetTime < prevRecord->m_flSimulationTime);

// calc fraction between both records
frac = (flTargetTime - m_PlayerRecords[pl_index].m_flSimulationTime) /
(m_PrevPlayerTable[pl_index].m_flSimulationTime - m_PlayerRecords[pl_index].m_flSimulationTime);

Assert(frac > 0 && frac < 1); // should never extrapolate

ang = Lerp(frac, m_PlayerRecords[pl_index].m_vecAngles, m_PrevPlayerTable[pl_index].m_vecAngles);
org = Lerp(frac, m_PlayerRecords[pl_index].m_vecOrigin, m_PrevPlayerTable[pl_index].m_vecOrigin);
}
else
{
// we found the exact record or no other record to interpolate with
// just copy these values since they are the best we have
ang = m_PlayerRecords[pl_index].m_vecAngles;
org = m_PlayerRecords[pl_index].m_vecOrigin;
}


// See if this represents a change for the player
int flags = 0;
SDK::LagRecord *restore = &m_RestoreData[pl_index];
SDK::LagRecord *change = &m_ChangeData[pl_index];

SDK::QAngle angdiff = pPlayer->GetAbsAngles() - ang;
SDK::Vector orgdiff = pPlayer->GetAbsOrigin() - org;

// Always remember the pristine simulation time in case we need to restore it.
restore->m_flSimulationTime = pPlayer->GetSimulationTime();

flags |= LC_ANGLES_CHANGED;
restore->m_vecAngles = pPlayer->GetAbsAngles();
//pPlayer->GetAbsAngles(ang);
change->m_vecAngles = ang;



flags |= LC_ORIGIN_CHANGED;
restore->m_vecOrigin = pPlayer->GetAbsOrigin();
*pPlayer->m_vecOrigin = org;
change->m_vecOrigin = org;

/*char text[256]; Q_snprintf( text, sizeof(text), "time %.2f", flTargetTime );
pPlayer->DrawServerHitboxes( 10 );
NDebugOverlay::Text( org, text, false, 10 );
NDebugOverlay::EntityBounds( pPlayer, 255, 0, 0, 32, 10 );

//m_RestorePlayer.Set(pl_index); //remember that we changed this player
m_bNeedToRestore = true;  // we changed at least one player
restore->m_fFlags = flags; // we need to restore these flags
change->m_fFlags = flags; // we have changed these flags
*/
template <class T>
FORCEINLINE T Lerp(float flPercent, T const &A, T const &B)
{
	return A + (B - A) * flPercent;
}
