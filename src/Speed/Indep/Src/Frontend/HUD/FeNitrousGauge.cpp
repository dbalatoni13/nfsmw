#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
int bStrICmp(const char *s1, const char *s2);

extern const char lbl_803E4D20[];
extern const float lbl_803E4D1C;
extern const float lbl_803E4D30;
extern const float lbl_803E4D34;
extern const float lbl_803E4D38;

NitrousGauge::NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x80000800ULL) //
    , INos(pOutter) //
    , mNos(lbl_803E4D1C) //
{
    RegisterGroup(0x87C38E97);
    mpDataNosMeterIcon = FEngFindObject(pPackageName, 0x27DDF583);
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
    if (mNos == nos) {
        return;
    }
    mNos = nos;
}
