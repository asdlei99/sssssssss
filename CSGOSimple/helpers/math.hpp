#pragma once

#include "../valve_sdk/sdk.hpp"
#include "../valve_sdk/csgostructs.hpp"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)

namespace Math
{
    void NormalizeAngles(QAngle& angles);
    void ClampAngles(QAngle& angles);
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
    void AngleVectors(const QAngle &angles, Vector& forward);
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up);
    void VectorAngles(const Vector& forward, QAngle& angles);
    bool WorldToScreen(const Vector& in, Vector& out);
	int TIME_TO_TICKS(int dt);
	int TICKS_TO_TIME(int t);

	void CalcAngle(Vector src, Vector target, QAngle &angle);
	float GetFov(const QAngle& viewAngle, const QAngle& aimAngle);
	float get3dDistance(Vector myCoords, Vector enemyCoords);
	QAngle calcAngle(Vector target);
	void AngleVectorsAV(const QAngle &angles, Vector& forward);
}