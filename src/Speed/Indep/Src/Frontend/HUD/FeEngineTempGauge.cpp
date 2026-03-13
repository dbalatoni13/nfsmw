#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned long FEHashUpper(const char *name);
int bStrICmp(const char *s1, const char *s2);

extern const char lbl_803E4D20[];
extern const char lbl_803E4DB0[];
extern const char lbl_803E4DC8[];
extern const char lbl_803E4DE0[];
extern const float lbl_803E4DF0;
extern const float lbl_803E4DF4;
extern const float lbl_803E4DF8;
extern const float lbl_803E4DFC;
extern const float lbl_803E4E00;
extern const float lbl_803E4E04;
extern const float lbl_803E4E08;

EngineTempGauge::EngineTempGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40) //
    , IEngineTempGauge(pOutter) //
    , mEngineTemp(lbl_803E4DF0) //
    , mEngineTempChanged(true) //
{
    RegisterGroup(FEHashUpper(lbl_803E4DB0));
    mpWarningLight = FEngFindObject(GetPackageName(), FEHashUpper(lbl_803E4DC8));
    mpEngineTempGaugeBar = RegisterMultiImage(FEHashUpper(lbl_803E4DE0));
}

void EngineTempGauge::Update(IPlayer *player) {
    if (!mEngineTempChanged) {
        return;
    }
    mEngineTempChanged = false;

    float maxAngle = lbl_803E4DF4;
    if (bStrICmp(pPackageName, lbl_803E4D20) != 0) {
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            maxAngle = lbl_803E4E00;
        }
    } else {
        maxAngle = lbl_803E4DF8;
    }

    if (mpEngineTempGaugeBar != nullptr) {
        FEngSetMultiImageRot(mpEngineTempGaugeBar, mEngineTemp * -maxAngle + maxAngle);
    }

    if (mEngineTemp < lbl_803E4DFC) {
        if (!FEngIsScriptSet(mpWarningLight, 0x1744B3)) {
            FEngSetScript(mpWarningLight, 0x1744B3, true);
        }
    } else {
        if (mEngineTemp < lbl_803E4E04) {
            if (!FEngIsScriptSet(mpWarningLight, 0x77031C70)) {
                FEngSetScript(mpWarningLight, 0x77031C70, true);
            }
        } else {
            if (!FEngIsScriptSet(mpWarningLight, 0xDA600155)) {
                FEngSetScript(mpWarningLight, 0xDA600155, true);
            }
        }
    }
}

void EngineTempGauge::SetEngineTemp(float temp) {
    if (mEngineTemp == temp) {
        return;
    }
    mEngineTemp = temp;
    mEngineTempChanged = true;
}
