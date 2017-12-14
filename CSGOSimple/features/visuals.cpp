#include "visuals.hpp"

#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"

#include "../weapon_ids.hpp"

vgui::HFont esp_font;

// ESP Context
// This is used so that we dont have to calculate player color and position
// on each individual function over and over
struct
{
    C_BasePlayer* pl;
    bool          is_enemy;
    bool          is_visible;
    Color         clr;
    Vector        head_pos;
    Vector        feet_pos;
    RECT          bbox;
} esp_ctx;

RECT GetBBox(C_BaseEntity* ent)
{
    RECT rect{};
    auto collideable = ent->GetCollideable();

    if(!collideable)
        return rect;

    auto min = collideable->OBBMins();
    auto max = collideable->OBBMaxs();

    const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

    Vector points[] = {
        Vector(min.x, min.y, min.z),
        Vector(min.x, max.y, min.z),
        Vector(max.x, max.y, min.z),
        Vector(max.x, min.y, min.z),
        Vector(max.x, max.y, max.z),
        Vector(min.x, max.y, max.z),
        Vector(min.x, min.y, max.z),
        Vector(max.x, min.y, max.z)
    };

    Vector pointsTransformed[8];
    for(int i = 0; i < 8; i++) {
        Math::VectorTransform(points[i], trans, pointsTransformed[i]);
    }

    Vector pos = ent->m_vecOrigin();
    Vector screen_points[8] = {};

    for(int i = 0; i < 8; i++) {
        if(!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
            return rect;
    }

    auto left = screen_points[0].x;
    auto top = screen_points[0].y;
    auto right = screen_points[0].x;
    auto bottom = screen_points[0].y;

    for(int i = 1; i < 8; i++) {
        if(left > screen_points[i].x)
            left = screen_points[i].x;
        if(top < screen_points[i].y)
            top = screen_points[i].y;
        if(right < screen_points[i].x)
            right = screen_points[i].x;
        if(bottom > screen_points[i].y)
            bottom = screen_points[i].y;
    }
    return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}
//--------------------------------------------------------------------------------
bool Visuals::CreateFonts()
{
    esp_font = g_VGuiSurface->CreateFont_();

    g_VGuiSurface->SetFontGlyphSet(esp_font, "Consolas", 12, 0, NULL, NULL, FONTFLAG_OUTLINE);

    return true;
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
    esp_ctx.pl = pl;
    esp_ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
    esp_ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

    if(!esp_ctx.is_enemy && g_Options.esp_team_check)
        return false;

    esp_ctx.clr = g_Options.color_players;

    auto head = pl->GetHitboxPos(HITBOX_HEAD);
    auto origin = pl->m_vecOrigin();

    head.z += 15;

    if(!Math::WorldToScreen(head, esp_ctx.head_pos) ||
       !Math::WorldToScreen(origin, esp_ctx.feet_pos))
        return false;

    auto h = fabs(esp_ctx.head_pos.y - esp_ctx.feet_pos.y);
    auto w = h / 1.65f;

    esp_ctx.bbox.left = static_cast<long>(esp_ctx.feet_pos.x - w * 0.5f);
    esp_ctx.bbox.right = static_cast<long>(esp_ctx.bbox.left + w);
    esp_ctx.bbox.bottom = static_cast<long>(esp_ctx.feet_pos.y);
    esp_ctx.bbox.top = static_cast<long>(esp_ctx.head_pos.y);

    return true;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox()
{
    g_VGuiSurface->DrawSetColor(esp_ctx.clr);
    g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.right, esp_ctx.bbox.bottom);
    g_VGuiSurface->DrawSetColor(Color::Black);
    g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);
    g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left + 1, esp_ctx.bbox.top + 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderName()
{
    wchar_t buf[128];

    player_info_t info = esp_ctx.pl->GetPlayerInfo();

    if(MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 128) > 0) {
        int tw, th;
        g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

        g_VGuiSurface->DrawSetTextFont(esp_font);
        g_VGuiSurface->DrawSetTextColor(Color::White);
        g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.head_pos.y - th);
        g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
    }
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderFlags()
{
	int tw, th, y = 0;

	if (esp_ctx.pl->m_iAccount())
	{
		wchar_t buf[128];
		char amount[128];

		sprintf(amount, "$%d", esp_ctx.pl->m_iAccount());

		if (MultiByteToWideChar(CP_UTF8, 0, amount, -1, buf, 80) > 0) {
			int tw, th;
			g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(esp_font);
			g_VGuiSurface->DrawSetTextColor(Color(85, 255, 85, 255));
			g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.right + 2, esp_ctx.bbox.top - 2);

			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}

		y++;
	}

	if (esp_ctx.pl->m_bHasHelmet() || esp_ctx.pl->m_ArmorValue() != 0) {
		wchar_t* buf = L"";

		if (esp_ctx.pl->m_bHasHelmet() && esp_ctx.pl->m_ArmorValue() <= 0)
			buf = L"H";

		if (!esp_ctx.pl->m_bHasHelmet() && esp_ctx.pl->m_ArmorValue() >= 1)
			buf = L"K";

		if (esp_ctx.pl->m_bHasHelmet() && esp_ctx.pl->m_ArmorValue() >= 1)
			buf = L"HK";

		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(170, 170, 170, 255));
		g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.right + 2, esp_ctx.bbox.top - 2 + (y * 10));

		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));

		y++;
	}

	if (esp_ctx.pl->m_bIsScoped()) {
		const wchar_t* buf = L"ZOOM";

		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(0, 170, 170, 255));
		g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.right + 2, esp_ctx.bbox.top - 2 + (y * 10));

		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));

		y++;
	}

	if (esp_ctx.pl->HasC4()) {
		const wchar_t* buf = L"BOMB";

		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(255, 85, 85, 255));
		g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.right + 2, esp_ctx.bbox.top - 2 + (y * 10));

		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));

		y++;
	}

	if (esp_ctx.pl->m_flFlashDuration() > 0.f) {
		const wchar_t* buf = L"FLASH";

		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(255, 255, 255, 255));
		g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.right + 2, esp_ctx.bbox.top - 2 + (y * 10));

		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));

		y++;
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{
    auto  hp = esp_ctx.pl->m_iHealth();
    float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
    //float off = (box_h / 6.f) + 5;
    float off = 6;

    auto height = (((box_h * hp) / 100));

    int x = esp_ctx.bbox.left - off;
    int y = esp_ctx.bbox.top;
    int w = 4;
    int h = box_h;

    g_VGuiSurface->DrawSetColor(Color::Black);
    g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

    g_VGuiSurface->DrawSetColor(Color(170, 255, 170, 255));
    g_VGuiSurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 1);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderAmmo()
{
	auto  ammo = esp_ctx.pl->m_hActiveWeapon()->m_iClip1();
	float box_h = (float)fabs(esp_ctx.bbox.right - esp_ctx.bbox.left);
	//float off = (box_h / 6.f) + 5;

	auto width = (((box_h * ammo) / esp_ctx.pl->m_hActiveWeapon()->GetCSWeaponData()->iMaxClip1));

	int x = esp_ctx.bbox.left;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(x, esp_ctx.bbox.bottom + 2, x + h, esp_ctx.bbox.bottom + 2 + w);

	g_VGuiSurface->DrawSetColor(Color(60, 180, 255, 255));
	g_VGuiSurface->DrawFilledRect(x + 1, esp_ctx.bbox.bottom + 3, x + width - 1, esp_ctx.bbox.bottom + 1 + w);

}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeapon()
{
    wchar_t buf[80];

    auto weapon = esp_ctx.pl->m_hActiveWeapon();

    if(!weapon) return;

    if(MultiByteToWideChar(CP_UTF8, 0, g_WeaponIDs.GetNameFromID(weapon->m_Item().m_iItemDefinitionIndex()), -1, buf, 80) > 0) {
        int tw, th;
        g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

        g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color::White);
		if (g_Options.esp_ammo && !g_WeaponIDs.IsMisc(esp_ctx.pl->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex())) {
			g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.feet_pos.y + 7);
		}
		else {
			g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.feet_pos.y + 2);
		}
        g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
    }
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderWeapon(C_BaseCombatWeapon* ent)
{
    wchar_t buf[80];

    // We don't want to Render weapons that are being held
    if(ent->m_hOwnerEntity().IsValid())
        return;

    auto bbox = GetBBox(ent);

    if(bbox.right == 0 || bbox.bottom == 0)
        return;

	float saveY = bbox.bottom;
	bbox.bottom = bbox.top;
	bbox.top = saveY;

	g_VGuiSurface->DrawSetColor(g_Options.color_dropped_weapons);
	g_VGuiSurface->DrawOutlinedRect(bbox.left, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawOutlinedRect(bbox.left - 1, bbox.top - 1, bbox.right + 1, bbox.bottom + 1);
	g_VGuiSurface->DrawOutlinedRect(bbox.left + 1, bbox.top + 1, bbox.right - 1, bbox.bottom - 1);

    if(MultiByteToWideChar(CP_UTF8, 0, g_WeaponIDs.GetNameFromID(ent->m_Item().m_iItemDefinitionIndex()), -1, buf, 80) > 0) {
        int w = bbox.right - bbox.left;
        int tw, th;
        g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

        g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color::White);
        g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
        g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
    }
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderDefuseKit(C_BaseEntity* ent)
{
    if(ent->m_hOwnerEntity().IsValid())
        return;

    auto bbox = GetBBox(ent);

    if(bbox.right == 0 || bbox.bottom == 0)
        return;

	float saveY = bbox.bottom;
	bbox.bottom = bbox.top;
	bbox.top = saveY;

    g_VGuiSurface->DrawSetColor(g_Options.color_defuse_kits);
	g_VGuiSurface->DrawOutlinedRect(bbox.left, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawOutlinedRect(bbox.left - 1, bbox.top - 1, bbox.right + 1, bbox.bottom + 1);
	g_VGuiSurface->DrawOutlinedRect(bbox.left + 1, bbox.top + 1, bbox.right - 1, bbox.bottom - 1);

    const wchar_t* buf = L"Defuse Kit";

    int w = bbox.right - bbox.left;
    int tw, th;
    g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

    g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(Color::White);
    g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
    g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderPlantedC4(C_BaseEntity* ent)
{
    auto bbox = GetBBox(ent);

    if(bbox.right == 0 || bbox.bottom == 0)
        return;

	float saveY = bbox.bottom;
	bbox.bottom = bbox.top;
	bbox.top = saveY;

    g_VGuiSurface->DrawSetColor(g_Options.color_planted_c4);
	g_VGuiSurface->DrawOutlinedRect(bbox.left, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawOutlinedRect(bbox.left - 1, bbox.top - 1, bbox.right + 1, bbox.bottom + 1);
	g_VGuiSurface->DrawOutlinedRect(bbox.left + 1, bbox.top + 1, bbox.right - 1, bbox.bottom - 1);

    const wchar_t* buf = L"Planted C4";

    int w = bbox.right - bbox.left;
    int tw, th;
    g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

    g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(Color::White);
    g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
    g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}
