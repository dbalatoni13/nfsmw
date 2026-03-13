#include "Speed/Indep/Src/Frontend/HUD/FeBustedMeter.hpp"

BustedMeter::BustedMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IBustedMeter(pOutter)
{
}

void BustedMeter::Update(IPlayer *player) {
}
