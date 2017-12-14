#pragma once

class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;

namespace Visuals
{
    namespace Player
    {
        bool Begin(C_BasePlayer* pl);

        void RenderBox();
        void RenderName();
		void RenderFlags();
        void RenderHealth();
        void RenderAmmo();
        void RenderWeapon();
    }

    namespace Misc
    {
        void RenderWeapon(C_BaseCombatWeapon* ent);
        void RenderDefuseKit(C_BaseEntity* ent);
        void RenderPlantedC4(C_BaseEntity* ent);
    }

    bool CreateFonts();
}
