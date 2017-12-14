#pragma once

#define WEAPON(num, var) int var = num;

class CWeaponIDs
{
public:
	WEAPON(31, ZEUS_X27);
	WEAPON(49, C4_EXPLOSIVE);

	WEAPON(43, FLASHBANG);
	WEAPON(44, HIGH_EXPLOSIVE_GRENADE);
	WEAPON(45, SMOKE_GRENADE);
	WEAPON(46, MOLOTOV);
	WEAPON(47, DECOY_GRENADE);
	WEAPON(48, INCENDIARY_GRENADE);

	WEAPON(7, AK_47);
	WEAPON(8, AUG);
	WEAPON(10, FAMAS);
	WEAPON(13, GALIL_AR);
	WEAPON(60, M4A1_S);
	WEAPON(16, M4A4);
	WEAPON(39, SG_553);
	WEAPON(14, M249);
	WEAPON(28, NEGEV);

	WEAPON(17, MAC_10);
	WEAPON(33, MP7);
	WEAPON(34, MP9);
	WEAPON(26, PP_BIZON);
	WEAPON(19, P90);
	WEAPON(24, UMP_45);

	WEAPON(9, AWP);
	WEAPON(11, G3SG1);
	WEAPON(38, SCAR_20);
	WEAPON(40, SSG_08);

	WEAPON(27, MAG_7);
	WEAPON(35, NOVA);
	WEAPON(29, SAWED_OFF);
	WEAPON(25, XM1014);

	WEAPON(61, USP_S);
	WEAPON(64, R8_REVOLVER);
	WEAPON(63, CZ75_AUTO);
	WEAPON(1, DESERT_EAGLE);
	WEAPON(2, DUAL_BERETTAS);
	WEAPON(3, FIVE_SEVEN);
	WEAPON(4, GLOCK_18);
	WEAPON(32, P2000);
	WEAPON(36, P250);
	WEAPON(30, TEC_9);

	WEAPON(42, CT_KNIFE);
	WEAPON(59, T_KNIFE);
	WEAPON(500, BAYONET);
	WEAPON(505, FLIP_KNIFE);
	WEAPON(506, GUT_KNIFE);
	WEAPON(507, KARAMBIT);
	WEAPON(508, M9_BAYONET);
	WEAPON(509, HUNTSMAN_KNIFE);
	WEAPON(512, FALCHION_KNIFE);
	WEAPON(514, BOWIE_KNIFE);
	WEAPON(515, BUTTERFLY_KNIFE);
	WEAPON(516, SHADOW_DAGGERS);

	bool IsKnife(int id)
	{
		return id == CT_KNIFE || id == T_KNIFE || id == BAYONET || id == FLIP_KNIFE || id == GUT_KNIFE || id == KARAMBIT || id == M9_BAYONET || id == HUNTSMAN_KNIFE || id == FALCHION_KNIFE || id == BOWIE_KNIFE || id == BUTTERFLY_KNIFE || id == SHADOW_DAGGERS;
	}

	bool IsPistol(int id)
	{
		return id == USP_S || id == CZ75_AUTO || id == TEC_9 || id == DESERT_EAGLE || id == DUAL_BERETTAS || id == FIVE_SEVEN || id == GLOCK_18 || id == P2000 || id == P250 || id == R8_REVOLVER;
	}

	bool IsSMG(int id)
	{
		return id == UMP_45 || id == PP_BIZON || id == MAC_10 || id == MP7 || id == MP9 || id == P90;
	}

	bool IsRifle(int id)
	{
		return id == AK_47 || id == AUG || id == M4A1_S || id == M4A4 || id == FAMAS || id == GALIL_AR || id == SG_553;
	}

	bool IsSniper(int id)
	{
		return id == SCAR_20 || id == SSG_08 || id == G3SG1 || id == AWP;
	}

	bool IsShotgun(int id)
	{
		return id == SAWED_OFF || id == NOVA || id == XM1014 || id == MAG_7;
	}

	bool IsHeavy(int id)
	{
		return id == M249 || id == NEGEV;
	}

	bool IsMisc(int id)
	{
		return id == C4_EXPLOSIVE || id == BAYONET || id == BOWIE_KNIFE || id == BUTTERFLY_KNIFE || id == SHADOW_DAGGERS || id == CT_KNIFE || id == DECOY_GRENADE || id == FALCHION_KNIFE || id == FLASHBANG || id == FLIP_KNIFE || id == HIGH_EXPLOSIVE_GRENADE || id == GUT_KNIFE || id == HUNTSMAN_KNIFE || id == INCENDIARY_GRENADE || id == KARAMBIT || id == M9_BAYONET || id == MOLOTOV || id == SMOKE_GRENADE || id == T_KNIFE || id == ZEUS_X27;
	}

	bool HasScope(int id)
	{
		return id == SCAR_20 || id == SSG_08 || id == G3SG1 || id == AWP || id == SG_553 || id == AUG;
	}

	bool IsGrenade(int id)
	{
		return id == FLASHBANG || id == HIGH_EXPLOSIVE_GRENADE || id == MOLOTOV || id == SMOKE_GRENADE || id == DECOY_GRENADE || id == INCENDIARY_GRENADE;
	}

	char* GetNameFromID(int id)
	{
		if (id == AK_47)
		{
			return "AK-47";
		}
		if (id == AUG)
		{
			return "AUG";
		}
		if (id == M4A1_S)
		{
			return "M4A1-S";
		}
		if (id == M4A4)
		{
			return "M4A4";
		}
		if (id == FAMAS)
		{
			return "FAMAS";
		}
		if (id == M249)
		{
			return "M249";
		}
		if (id == NEGEV)
		{
			return "Negev";
		}
		if (id == GALIL_AR)
		{
			return "Galil AR";
		}
		if (id == SG_553)
		{
			return "SG 553";
		}
		if (id == MAC_10)
		{
			return "MAC-10";
		}
		if (id == MP7)
		{
			return "MP7";
		}
		if (id == MP9)
		{
			return "MP9";
		}
		if (id == PP_BIZON)
		{
			return "PP-Bizon";
		}
		if (id == P90)
		{
			return "P90";
		}
		if (id == UMP_45)
		{
			return "UMP-45";
		}
		if (id == AWP)
		{
			return "AWP";
		}
		if (id == G3SG1)
		{
			return "G3SG1";
		}
		if (id == SCAR_20)
		{
			return "SCAR-20";
		}
		if (id == SSG_08)
		{
			return "SSG 08";
		}
		if (id == MAG_7)
		{
			return "MAG-7";
		}
		if (id == NOVA)
		{
			return "Nova";
		}
		if (id == SAWED_OFF)
		{
			return "Sawed-Off";
		}
		if (id == XM1014)
		{
			return "XM1014";
		}
		if (id == USP_S)
		{
			return "USP-S";
		}
		if (id == R8_REVOLVER)
		{
			return "R8 Revolver";
		}
		if (id == CZ75_AUTO)
		{
			return "CZ75-Auto";
		}
		if (id == DESERT_EAGLE)
		{
			return "Desert Eagle";
		}
		if (id == DUAL_BERETTAS)
		{
			return "Dual Berettas";
		}
		if (id == FIVE_SEVEN)
		{
			return "Five-SeveN";
		}
		if (id == GLOCK_18)
		{
			return "Glock-18";
		}
		if (id == P2000)
		{
			return "P2000";
		}
		if (id == P250)
		{
			return "P250";
		}
		if (id == TEC_9)
		{
			return "Tec-9";
		}
		if (id == ZEUS_X27)
		{
			return "Zeus x27";
		}
		if (id == C4_EXPLOSIVE)
		{
			return "C4 Explosive";
		}
		if (id == FLASHBANG)
		{
			return "Flashbang";
		}
		if (id == HIGH_EXPLOSIVE_GRENADE)
		{
			return "High Explosive Grenade";
		}
		if (id == SMOKE_GRENADE)
		{
			return "Smoke Grenade";
		}
		if (id == MOLOTOV)
		{
			return "Molotov";
		}
		if (id == DECOY_GRENADE)
		{
			return "Decoy Grenade";
		}
		if (id == INCENDIARY_GRENADE)
		{
			return "Incendiary Grenade";
		}
		if (id == CT_KNIFE)
		{
			return "Knife";
		}
		if (id == T_KNIFE)
		{
			return "Knife";
		}
		if (id == BAYONET)
		{
			return "Bayonet";
		}
		if (id == FLIP_KNIFE)
		{
			return "Flip Knife";
		}
		if (id == GUT_KNIFE)
		{
			return "Gut Knife";
		}
		if (id == KARAMBIT)
		{
			return "Karambit";
		}
		if (id == M9_BAYONET)
		{
			return "M9 Bayonet";
		}
		if (id == HUNTSMAN_KNIFE)
		{
			return "Huntsman Knife";
		}
		if (id == FALCHION_KNIFE)
		{
			return "Falchion Knife";
		}
		if (id == BOWIE_KNIFE)
		{
			return "Bowie Knife";
		}
		if (id == BUTTERFLY_KNIFE)
		{
			return "Butterfly Knife";
		}
		if (id == SHADOW_DAGGERS)
		{
			return "Shadow Daggers";
		}
		return "???";
	}
};

extern CWeaponIDs g_WeaponIDs;