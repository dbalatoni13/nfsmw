#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

NitrousGauge::NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x80000800ULL), //
      INos(pOutter),                       //
      mNos(0)                              //
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

        const float max_angle = 0.0f;
        const float min_nos = 0.0f;
        const float max_nos = 1.0f;
        const float frac = (mNos - min_nos) / (max_nos - min_nos);
        const float angle = min_angle + frac * (max_angle - min_angle);
        FEngSetMultiImageRot(mpNosMeterBar, angle);
    }
}

void NitrousGauge::SetNos(float nos) {
    if (nos <= 0) {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x1744B3)) {
            FEngSetScript(mpDataNosMeterIcon, 0x1744B3, true);
        }
    } else if (nos < mNos) {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, FEHASH_PULSE)) {
            FEngSetScript(mpDataNosMeterIcon, FEHASH_PULSE, true);
        }
    } else {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, FEHASH_ACTIVATE)) {
            FEngSetScript(mpDataNosMeterIcon, FEHASH_ACTIVATE, true);
        }
    }
    mNos = nos;
}
