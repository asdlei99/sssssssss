#include "clantagspammer.hpp"

#include "valve_sdk/csgostructs.hpp"

#include "options.hpp"
#include "helpers/utils.hpp"

void ClantagSpammer::OnCreateMove(CUserCmd* cmd)
{
	if (!g_LocalPlayer)
		return;

	int serverTime = (float) g_GlobalVars->interval_per_tick * g_LocalPlayer->m_nTickBase() * 2.5;

	switch (g_Options.misc_clantag_spammer) {
	case 0:
		Utils::SetClantag("");
		break; // none
	case 1:
		switch (serverTime % 19) {
		case 1: Utils::SetClantag("            s"); break;
		case 2: Utils::SetClantag("           si"); break;
		case 3: Utils::SetClantag("          sin"); break;
		case 4: Utils::SetClantag("         sinn"); break;
		case 5: Utils::SetClantag("        sinne"); break;
		case 6: Utils::SetClantag("       sinner"); break;
		case 7: Utils::SetClantag("      sinner "); break;
		case 8: Utils::SetClantag("     sinner  "); break;
		case 9: Utils::SetClantag("    sinner   "); break;
		case 10: Utils::SetClantag("   sinner    "); break;
		case 11: Utils::SetClantag("  sinner     "); break;
		case 12: Utils::SetClantag(" sinner      "); break;
		case 13: Utils::SetClantag("sinner       "); break;
		case 14: Utils::SetClantag("inner        "); break;
		case 15: Utils::SetClantag("nner         "); break;
		case 16: Utils::SetClantag("ner          "); break;
		case 17: Utils::SetClantag("er           "); break;
		case 18: Utils::SetClantag("r            "); break;
		case 19: Utils::SetClantag("             "); break;

		}
		break; // sinner
	case 2:
		switch (serverTime % 12) {
		case 1: Utils::SetClantag("IMWARE.net A"); break;
		case 2: Utils::SetClantag("MWARE.net AI"); break;
		case 3: Utils::SetClantag("WARE.net AIM"); break;
		case 4: Utils::SetClantag("ARE.net AIMW"); break;
		case 5: Utils::SetClantag("RE.net AIMWA"); break;
		case 6: Utils::SetClantag("E.net AIMWAR"); break;
		case 7: Utils::SetClantag(".net AIMWARE"); break;
		case 8: Utils::SetClantag("net AIMWARE."); break;
		case 9: Utils::SetClantag("et AIMWARE.n"); break;
		case 10: Utils::SetClantag("t AIMWARE.ne"); break;
		case 11: Utils::SetClantag(" AIMWARE.net"); break;
		case 12: Utils::SetClantag("AIMWARE.net "); break;
		}
		break; // aimware.net
	case 3:
		Utils::SetClantag("HookTronic.TOP");
		break; // hooktronic.top
	case 4:
		Utils::SetClantag(">cantresolve.us");
		break; // >cantresolve.us
	case 5: 
		switch (serverTime % 22) {
		case 1: Utils::SetClantag("             g"); break;
		case 2: Utils::SetClantag("            ga"); break;
		case 3: Utils::SetClantag("           gam"); break;
		case 4: Utils::SetClantag("          game"); break;
		case 5: Utils::SetClantag("         games"); break;
		case 6: Utils::SetClantag("        gamese"); break;
		case 7: Utils::SetClantag("       gamesen"); break;
		case 8: Utils::SetClantag("      gamesens"); break;
		case 9: Utils::SetClantag("     gamesense"); break;
		case 10: Utils::SetClantag("   gamesense "); break;
		case 11: Utils::SetClantag("  gamesense  "); break;
		case 12: Utils::SetClantag(" gamesense   "); break;
		case 13: Utils::SetClantag("gamesense    "); break;
		case 14: Utils::SetClantag("amesense     "); break;
		case 15: Utils::SetClantag("mesense      "); break;
		case 16: Utils::SetClantag("esense       "); break;
		case 17: Utils::SetClantag("sense        "); break;
		case 18: Utils::SetClantag("ense         "); break;
		case 19: Utils::SetClantag("nse          "); break;
		case 20: Utils::SetClantag("se           "); break;
		case 21: Utils::SetClantag("e            "); break;
		case 22: Utils::SetClantag("             "); break;
		}
		break; // gamesense
	case 6:
		switch (serverTime % 6) {
		case 1: Utils::SetClantag("agle e"); break;
		case 2: Utils::SetClantag("gle ea"); break;
		case 3: Utils::SetClantag("le eag"); break;
		case 4: Utils::SetClantag("e eagl"); break;
		case 5: Utils::SetClantag(" eagle"); break;
		case 6: Utils::SetClantag("eagle "); break;
		}
		break; // eagle
	case 7:
		Utils::SetClantag("Styles > all");
		break; // styles > all
	case 8:
		switch (serverTime % 6) {
		case 1: Utils::SetClantag("uper s"); break;
		case 2: Utils::SetClantag("per su"); break;
		case 3: Utils::SetClantag("er sup"); break;
		case 4: Utils::SetClantag("r supe"); break;
		case 5: Utils::SetClantag("	super"); break;
		case 6: Utils::SetClantag("super "); break;
		}
		break; // superior
	case 9:
		static int tick;

		tick++;

		if (tick > 0 && tick < 15)
		{
			Utils::SetClantag("Barbossa");
		}
		else if (tick > 30 && tick < 45)
		{
			Utils::SetClantag("B arbossa");
		}
		else if (tick > 60 && tick < 75)
		{
			Utils::SetClantag("B a rbossa");
		}
		else if (tick > 90 && tick < 105)
		{
			Utils::SetClantag("B a r bossa");
		}
		else if (tick > 120 && tick < 135)
		{
			Utils::SetClantag("B a r b ossa");
		}
		else if (tick > 150 && tick < 165)
		{
			Utils::SetClantag("B a r b o ssa");
		}
		else if (tick > 180 && tick < 195)
		{
			Utils::SetClantag("B a r b o s sa");
		}
		else if (tick > 210 && tick < 225)
		{
			Utils::SetClantag("B a r b o s s a");
		}
		else if (tick > 240 && tick < 255)
		{
			Utils::SetClantag("B a r b o s sa");
		}
		else if (tick > 270 && tick < 285)
		{
			Utils::SetClantag("B a r b o ssa");
		}
		else if (tick > 300 && tick < 315)
		{
			Utils::SetClantag("B a r b ossa");
		}
		else if (tick > 330 && tick < 345)
		{
			Utils::SetClantag("B a r bossa");
		}
		else if (tick > 360 && tick < 375)
		{
			Utils::SetClantag("B a rbossa");
		}
		else if (tick > 390 && tick < 405)
		{
			Utils::SetClantag("B arbossa");
		}
		else if (tick > 420 && tick < 435)
		{
			Utils::SetClantag("Barbossa");
		}

		if (tick > 435)
		{
			tick = 0;
		}
		break; // barbossa
	case 10:
		switch (serverTime % 25) {
		case 0:Utils::SetClantag("              "); break;
		case 1:Utils::SetClantag("   L          "); break;
		case 2:Utils::SetClantag("   Lu         "); break;
		case 3:Utils::SetClantag("   Lui        "); break;
		case 4:Utils::SetClantag("Lui           "); break;
		case 5:Utils::SetClantag("         g    "); break;
		case 6:Utils::SetClantag("        gi    "); break;
		case 7:Utils::SetClantag("           gi"); break;
		case 8:Utils::SetClantag(" L u i g i "); break;
		case 9:Utils::SetClantag("             "); break;
		case 10:Utils::SetClantag(" L u i g i "); break;
		case 11:Utils::SetClantag("             "); break;
		case 12:Utils::SetClantag(" i g i u L "); break;
		case 13:Utils::SetClantag("             "); break;
		case 14:Utils::SetClantag(" i g i u L "); break;
		case 15:Utils::SetClantag("             "); break;
		case 16:Utils::SetClantag("Lui       gi"); break;
		case 17:Utils::SetClantag(" Lui     gi "); break;
		case 18:Utils::SetClantag("  Lui   gi  "); break;
		case 19:Utils::SetClantag("   Lui gi   "); break;
		case 20:Utils::SetClantag("    Luigi    "); break;
		case 21:Utils::SetClantag("   Lui gi   "); break;
		case 22:Utils::SetClantag("  Lui   gi  "); break;
		case 23:Utils::SetClantag(" Lui     gi "); break;
		case 24:Utils::SetClantag("Lui       gi"); break;
		case 25:Utils::SetClantag("   Luigi   "); break;
		}
		break; // luigi
	}
}