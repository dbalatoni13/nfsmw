#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
int bStrICmp(const char *s1, const char *s2);

extern const char lbl_803E4D20[];
extern const float lbl_803E4D1C;
extern const float lbl_803E4D30;
extern const float lbl_803E4D34;
extern const float lbl_803E4D38;
extern const float lbl_803E4D3C;

NitrousGauge::NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x80000800ULL) //
    , INos(pOutter) //
    , mNos(lbl_803E4D1C) //
{
    RegisterGroup(0x87C38E97);
    mpDataNosMeterIcon = FEngFindObject(pkg_name, 0x27DDF583);
    mpNosMeterBar = RegisterMultiImage(0xEDFB6D37);
}

void NitrousGauge::Update(IPlayer *player) {
    if (mpNosMeterBar != nullptr) {
        float maxAngle = lbl_803E4D30;
        if (bStrICmp(pPackageName, lbl_803E4D20) != 0) {
            if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                maxAngle = lbl_803E4D38;
            }
        } else {
            maxAngle = lbl_803E4D34;
        }
        FEngSetMultiImageRot(mpNosMeterBar, mNos * -maxAngle + maxAngle);
    }
}

void NitrousGauge::SetNos(float nos) {
    if (nos > lbl_803E4D3C) {
        if (mNos == nos) {
            return;
        }
        if (nos >= mNos) {
            if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x03826A28)) {
                FEngSetScript(mpDataNosMeterIcon, 0x03826A28, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x77031C70)) {
                FEngSetScript(mpDataNosMeterIcon, 0x77031C70, true);
            }
        }
    } else {
        if (!FEngIsScriptSet(mpDataNosMeterIcon, 0x1744B3)) {
            FEngSetScript(mpDataNosMeterIcon, 0x1744B3, true);
        }
    }
    mNos = nos;
}
