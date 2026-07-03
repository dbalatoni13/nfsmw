#include "Speed/Indep/Src/Frontend/HUD/FeAutoSaveIcon.hpp"

AutoSaveIcon::AutoSaveIcon(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IAutoSaveIcon(pOutter)
{
}

void AutoSaveIcon::Update(IPlayer *player) {
}
