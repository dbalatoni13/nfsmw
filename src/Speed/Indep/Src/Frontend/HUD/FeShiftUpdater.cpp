#include "Speed/Indep/Src/Frontend/HUD/FeShiftUpdater.hpp"

ShiftUpdater::ShiftUpdater(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IShiftUpdater(pOutter)
{
}

void ShiftUpdater::Update(IPlayer *player) {
}
