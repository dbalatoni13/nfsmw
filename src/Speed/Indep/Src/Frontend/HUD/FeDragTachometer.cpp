#include "Speed/Indep/Src/Frontend/HUD/FeDragTachometer.hpp"

DragTachometer::DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITachometer(pOutter) //
    , ITachometerDrag(pOutter)
{
}

void DragTachometer::Update(IPlayer *player) {
}

void DragTachometer::SetRpm(float rpm) {
    mRpm = rpm;
}

void DragTachometer::SetInPerfectLaunchRange(bool inRange) {
    mInPerfectLaunchRange = inRange;
}

void DragTachometer::SetShifting(bool shifting) {
    mGearShifting = shifting;
}
