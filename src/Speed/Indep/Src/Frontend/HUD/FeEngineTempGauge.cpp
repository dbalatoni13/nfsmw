#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned long FEHashUpper(const char *name);

extern const char lbl_803E4DB0[];
extern const char lbl_803E4DC8[];
extern const char lbl_803E4DE0[];
extern const float lbl_803E4DF0;
extern float warningPulseMinRpm;

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

    if (mpEngineTempGaugeBar) {
        const float min_angle = -26.5f;
        const float max_angle = 26.5f;
        FEngSetMultiImageRot(mpEngineTempGaugeBar, mEngineTemp * max_angle + min_angle);

        if (mEngineTemp > warningPulseMinRpm) {
            if (!FEngIsScriptSet(mpEngineTempGaugeBar, FEHashUpper("OVERHEAT_PULSE"))) {
                FEngSetScript(mpEngineTempGaugeBar, FEHashUpper("OVERHEAT_PULSE"), true);
            }
        } else {
            if (!FEngIsScriptSet(mpEngineTempGaugeBar, FEHashUpper("INIT"))) {
                FEngSetScript(mpEngineTempGaugeBar, FEHashUpper("INIT"), true);
            }
        }
    }

    if (mpWarningLight) {
        if (mEngineTemp > warningPulseMinRpm) {
            if (!FEngIsScriptSet(mpWarningLight, FEHashUpper("OVERHEAT_PULSE"))) {
                FEngSetScript(mpWarningLight, FEHashUpper("OVERHEAT_PULSE"), true);
            }
        } else if (mEngineTemp > 0.1f) {
            if (!FEngIsScriptSet(mpWarningLight, FEHashUpper("ACTIVATE"))) {
                FEngSetScript(mpWarningLight, FEHashUpper("ACTIVATE"), true);
            }
        } else {
            if (FEngIsScriptSet(mpWarningLight, FEHashUpper("INIT"))) {
                return;
            }
            FEngSetScript(mpWarningLight, FEHashUpper("INIT"), true);
            return;
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
