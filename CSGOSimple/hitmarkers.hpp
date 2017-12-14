#include "valve_sdk\csgostructs.hpp"
#include "hooks.hpp"
#include "helpers/utils.hpp"

class hitmarkers
{
	class player_hurt_listener : public IGameEventListener2
	{
	public:
		void start()
		{
			g_GameEvents->AddListener(this, "player_hurt", false);
		}

		void stop()
		{
			g_GameEvents->RemoveListener(this);
		}

		void FireGameEvent(IGameEvent *event) override
		{
			hitmarkers::singleton()->on_fire_event(event);
		}

		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static hitmarkers* singleton()
	{
		static hitmarkers* instance = new hitmarkers;
		return instance;
	}

	void initialize()
	{
		_listener.start();
	}

	void uninitialize()
	{
		_listener.stop();
	}

	void on_fire_event(IGameEvent* event)
	{
		if (!strcmp(event->GetName(), "player_hurt")) {
			C_BasePlayer* hurt = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
			C_BasePlayer* attacker = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")));

			if (hurt != g_LocalPlayer && attacker == g_LocalPlayer)
			{
				if (g_Options.misc_hitmarkers)
				{
					if (g_Options.misc_hitmarkerssound)
					{
						Utils::ConsoleCommandSound("buttons\\arena_switch_press_02");
					}
					_flHurtTime = g_GlobalVars->curtime;
				}
			}
		}
	}

	void on_paint()
	{
		auto curtime = g_GlobalVars->curtime;
		auto lineSize = 8;

		if (g_Options.misc_hitmarkers)
		{
			if (_flHurtTime + 0.25f >= curtime)
			{
				int screenSizeX, screenCenterX;
				int screenSizeY, screenCenterY;

				g_EngineClient->GetScreenSize(screenSizeX, screenSizeY);

				screenCenterX = screenSizeX / 2;
				screenCenterY = screenSizeY / 2;

				g_VGuiSurface->DrawSetColor(200, 200, 200, 255);

				g_VGuiSurface->DrawLine(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4));
				g_VGuiSurface->DrawLine(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4));
				g_VGuiSurface->DrawLine(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4));
				g_VGuiSurface->DrawLine(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4));
			}
		}
	}
private:
	player_hurt_listener    _listener;
	float                   _flHurtTime;
};