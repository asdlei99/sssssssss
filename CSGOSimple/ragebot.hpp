#pragma once

#include "valve_sdk\math\Vector.hpp"
#include "valve_sdk\sdk.hpp"

class C_BasePlayer;
class CUserCmd;

namespace Ragebot
{
	void OnCreateMove(CUserCmd* cmd);

	float GetHitchance();

	void UpdateHitbox(C_BasePlayer* player);

	int GetClosest(CUserCmd* cmd);

	bool IsValid(C_BasePlayer* entity);

	int GetHitbox(C_BasePlayer* entity);
}