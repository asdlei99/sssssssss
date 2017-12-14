#include "engineprediction.hpp"

#include "valve_sdk\csgostructs.hpp"
#include "valve_sdk\sdk.hpp"
#include "helpers\utils.hpp"

CMoveData m_MoveData;
float m_flOldCurtime;
float m_flOldFrametime;

int* m_pPredictionRandomSeed;

void EnginePrediction::StartPrediction(CUserCmd* cmd) {
	C_BasePlayer* pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	static bool bInit = false;

	if (!bInit) {
		m_pPredictionRandomSeed = *(int**)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
		bInit = true;
	}

	*m_pPredictionRandomSeed = cmd->random_seed;

	m_flOldCurtime = g_GlobalVars->curtime;
	m_flOldFrametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = pLocal->m_nTickBase() * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(pLocal);

	memset(&m_MoveData, 0, sizeof(m_MoveData));

	g_MoveHelper->SetHost(pLocal);
	g_Prediction->SetupMove(pLocal, cmd, g_MoveHelper, &m_MoveData);
	g_GameMovement->ProcessMovement(pLocal, &m_MoveData);
	g_Prediction->FinishMove(pLocal, cmd, &m_MoveData);
}

void EnginePrediction::EndPrediction() {
	C_BasePlayer* pLocal = (C_BasePlayer*) g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	g_GameMovement->FinishTrackPredictionErrors(pLocal);
	g_MoveHelper->SetHost(0);

	*m_pPredictionRandomSeed = -1;

	g_GlobalVars->curtime = m_flOldCurtime;
	g_GlobalVars->frametime = m_flOldFrametime;
}