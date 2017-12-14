#pragma once

class C_BasePlayer;
class CUserCmd;

namespace AntiAim
{
	void OnCreateMove(CUserCmd* cmd, bool &bSendPacket);
}