#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"

RadarDetector::RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IRadarDetector(pOutter)
{
}

void RadarDetector::Update(IPlayer *player) {
}
