#pragma once

#include "valve_sdk\math\Vector.hpp"
#include "valve_sdk\sdk.hpp"

class C_BasePlayer;
class CUserCmd;

namespace SMG
{
	void OnCreateMove(CUserCmd* cmd);

	double GetAngleDifference(double alpha, double beta);

	QAngle SmoothAngle(QAngle angles, QAngle destination, double x, double y);

	QAngle CalculateAngle(Vector target);

	int GetHitbox(C_BasePlayer* player);

	int GetClosest(CUserCmd* cmd);

	bool IsValid(C_BasePlayer* entity);
}