#include "autostrafe.hpp"

#include "valve_sdk/csgostructs.hpp"

void AutoStrafe::OnCreateMove(CUserCmd* cmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) 
		return;

	if ((GetAsyncKeyState(VK_SPACE) && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)))
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1)
		{
			cmd->sidemove = cmd->mousedx < 0.f ? -450.f : 450.f;
		}
		else {
			cmd->forwardmove = (1800.f * 4.f) / g_LocalPlayer->m_vecVelocity().Length2D();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (cmd->forwardmove > 450.f)
				cmd->forwardmove = 450.f;
		}
	}
}