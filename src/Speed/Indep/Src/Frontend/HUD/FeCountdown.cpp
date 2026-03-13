#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"

Countdown::Countdown(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ICountdown(pOutter)
{
}

void Countdown::Update(IPlayer *player) {
}
