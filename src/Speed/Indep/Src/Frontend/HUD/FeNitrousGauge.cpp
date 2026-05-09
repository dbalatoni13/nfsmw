#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

NitrousGauge::NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x80000800ULL) //
      ,
      INos(pOutter) //
      ,
      mNos(0) //
{
    RegisterGroup(0x87C38E97);
    mpDataNosMeterIcon = FEngFindObject(GetPackageName(), 0x27DDF583);
    mpNosMeterBar = RegisterMultiImage(0xEDFB6D37);
}

void NitrousGauge::Update(IPlayer *player) {
    if (mpNosMeterBar != nullptr) {
        float min_angle = 175.0f;
        if (bStrICmp(GetPackageName(), "HUD_Drag.fng") == 0) {
            min_angle = -48.0f;
        } else if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            min_angle = 180.0f;
        }

        FEngSetMultiImageRot(mpNosMeterBar, mNos * -min_angle + min_angle);
    }
}

void NitrousGauge::SetNos(float nos) {
    if (nos <= 0) {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x1744B3)) {
            FEngSetScript(mpDataNosMeterIcon, 0x1744B3, true);
        }
    } else if (nos < mNos) {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x77031C70)) {
            FEngSetScript(mpDataNosMeterIcon, 0x77031C70, true);
        }
    } else {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x03826A28)) {
            FEngSetScript(mpDataNosMeterIcon, 0x03826A28, true);
        }
    }
    mNos = nos;
}
