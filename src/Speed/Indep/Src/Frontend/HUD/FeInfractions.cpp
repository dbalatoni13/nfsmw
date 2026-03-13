#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"

Infractions::Infractions(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IInfractions(pOutter)
{
}

void Infractions::Update(IPlayer *player) {
}
