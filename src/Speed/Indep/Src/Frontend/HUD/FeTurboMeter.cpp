#include "Speed/Indep/Src/Frontend/HUD/FeTurboMeter.hpp"

TurboMeter::TurboMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITurbometer(pOutter)
{
}

void TurboMeter::Update(IPlayer *player) {
}
