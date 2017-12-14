#pragma once

#include "valve_sdk\math\Vector.hpp"
#include "valve_sdk\sdk.hpp"

class C_BasePlayer;


namespace Resolver
{
	void Initialize();

	void Uninitialize();

	extern RecvVarProxyFn oYawProxy;
}