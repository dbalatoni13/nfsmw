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
    mpDataNosMeterIcon = FEngFindObject(GetPackageName(), 0x27DDF583);
    mpNosMeterBar = RegisterMultiImage(0xEDFB6D37);
}

void NitrousGauge::Update(IPlayer *player) {
    if (mpNosMeterBar != nullptr) {
        float min_angle = lbl_803E4D30;
        if (bStrICmp(GetPackageName(), lbl_803E4D20) == 0) {
            min_angle = lbl_803E4D34;
        } else if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            min_angle = lbl_803E4D38;
        }
        const float max_angle = min_angle;
        const float min_nos = min_angle;
        const float max_nos = min_angle;
        const float frac = mNos;
        const float angle = frac * -max_angle + max_angle;
        FEngSetMultiImageRot(mpNosMeterBar, angle);
    }
}

void NitrousGauge::SetNos(float nos) {
    if (nos <= lbl_803E4D3C) {
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
