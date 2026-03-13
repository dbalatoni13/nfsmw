#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"

MenuZoneTrigger::MenuZoneTrigger(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IMenuZoneTrigger(pOutter)
{
}

void MenuZoneTrigger::Update(IPlayer *player) {
}
