#pragma once
#include "valve_sdk\csgostructs.hpp"


struct LagRecord
{
public:
	int						m_fFlags;

	// Player position, orientation and bbox
	Vector					m_vecOrigin;
	QAngle					m_vecAngles;

	Vector m_vecPreviousOrigin;
	QAngle m_vecPreviousAngles;

	bool ValidRecord;
	float					m_flSimulationTime;
	float					m_flPreviousSimulationTime;

	LagRecord()
	{
		m_fFlags = 0;
		m_vecOrigin.Init();
		m_vecAngles.Init();
		m_vecPreviousOrigin.Init();
		m_vecPreviousAngles.Init();
		m_flSimulationTime = -1;
		ValidRecord = false;
	}

	LagRecord(const LagRecord& src)
	{
		m_fFlags = src.m_fFlags;
		m_vecOrigin = src.m_vecOrigin;
		m_vecAngles = src.m_vecAngles;
		m_vecPreviousAngles = src.m_vecPreviousAngles;
		m_vecPreviousOrigin = src.m_vecPreviousOrigin;
		ValidRecord = src.ValidRecord;
	}
};

class CLagCompensation
{
private:
	LagRecord m_PlayerRecords[];
public:
	void Record();
	int GetBestTick(C_BaseEntity* pPlayer);
	bool IsBacktrackable(C_BaseEntity* pPlayer);
	void BacktrackPlayer(C_BaseEntity *pPlayer, float flTargetTime);
	void Main(int record_index, CUserCmd* pCmd);
};