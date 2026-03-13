#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"

TimeExtension::TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITimeExtension(pOutter)
{
}

void TimeExtension::Update(IPlayer *player) {
}
