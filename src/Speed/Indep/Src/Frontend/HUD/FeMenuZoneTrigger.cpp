#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);

MenuZoneTrigger::MenuZoneTrigger(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IMenuZoneTrigger(pOutter)
{
}

void MenuZoneTrigger::Update(IPlayer *player) {
}

bool MenuZoneTrigger::IsPlayerInsideTrigger() {
    return FEngIsScriptSet(mEventIcon, 0x280164f);
}

void MenuZoneTrigger::ExitTrigger() {
    mZoneType = nullptr;
    mbInsideTrigger = false;
    mpRaceActivity = nullptr;
    HideDPadButton();
}

bool MenuZoneTrigger::IsType(const char *t) {
    return bStrCmp(mZoneType, t) == 0;
}

bool MenuZoneTrigger::ShouldSeeMenuZoneCluster() {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(&GRaceStatus::Get()) + 0x1AA4) == 0;
}
