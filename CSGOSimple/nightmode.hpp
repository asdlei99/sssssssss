#include "valve_sdk\csgostructs.hpp"

#include "hooks.hpp"
#include "helpers/utils.hpp"
#include "options.hpp"

bool preformed = false;

class nightmode
{
	class game_newmap_listener : public IGameEventListener2
	{
	public:
		void start()
		{
			g_GameEvents->AddListener(this, "game_newmap", false);
		}

		void stop()
		{
			g_GameEvents->RemoveListener(this);
		}

		void FireGameEvent(IGameEvent *event) override
		{
			nightmode::singleton()->on_fire_event(event);
		}

		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static nightmode* singleton()
	{
		static nightmode* instance = new nightmode;
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
		if (!strcmp(event->GetName(), "game_newmap")) {
			preformed = false;
		}
	}

	void on_createmove()
	{
		static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
		static ConVar* r_drawspecificstaticprop = g_CVar->FindVar("r_drawspecificstaticprop");

		r_drawspecificstaticprop->SetValue(0);
		sv_skyname->m_nFlags &= ~FCVAR_CHEAT;

		if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return;

		if (g_Options.other_nightmode)
		{
			sv_skyname->SetValue("sky_csgo_night02");

			if (!preformed)
			{
				for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
				{
					IMaterial* material = g_MatSystem->GetMaterial(i);

					if (!material || material->IsErrorMaterial())
						continue;

					if (strstr(material->GetTextureGroupName(), "World"))
					{
						material->ColorModulate(0.15f, 0.15f, 0.15f);
					}

					if (strstr(material->GetTextureGroupName(), "StaticProp"))
					{
						material->ColorModulate(0.35f, 0.35f, 0.35f);
					}

					preformed = true;
				}
			}
		}
		else
		{
			sv_skyname->SetValue("sky_cs15_daylight02_hdr");

			if (preformed)
			{
				for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
				{
					IMaterial* material = g_MatSystem->GetMaterial(i);

					if (!material || material->IsErrorMaterial())
						continue;

					if (strstr(material->GetTextureGroupName(), "World") || strstr(material->GetTextureGroupName(), "StaticProp"))
					{
						material->ColorModulate(1.f, 1.f, 1.f);
					}

					preformed = false;
				}
			}
		}
	}
private:
	game_newmap_listener    _listener;
};