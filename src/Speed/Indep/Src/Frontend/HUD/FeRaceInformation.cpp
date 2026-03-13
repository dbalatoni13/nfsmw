#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"

RaceInformation::RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IRaceInformation(pOutter)
{
}

void RaceInformation::Update(IPlayer *player) {
}
