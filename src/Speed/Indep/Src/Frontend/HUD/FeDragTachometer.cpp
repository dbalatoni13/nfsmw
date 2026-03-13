#include "Speed/Indep/Src/Frontend/HUD/FeDragTachometer.hpp"

DragTachometer::DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITachometer(pOutter) //
    , ITachometerDrag(pOutter)
{
}

void DragTachometer::Update(IPlayer *player) {
}
