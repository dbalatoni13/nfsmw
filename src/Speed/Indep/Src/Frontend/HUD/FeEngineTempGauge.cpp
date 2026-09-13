#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

float warningPulseMinRpm = 0.5f;

EngineTempGauge::EngineTempGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40), //
      IEngineTempGauge(pOutter),  //
      mEngineTemp(0.0f),          //
      mEngineTempChanged(true)    //
{
    RegisterGroup(FEHashUpper("Engine_Heat_Meter_Group"));
    mpWarningLight = FEngFindObject(GetPackageName(), FEHashUpper("ENGINE_HEAT_ICON_GROUP"));
    mpEngineTempGaugeBar = RegisterMultiImage(FEHashUpper("ENGINE_METER"));
}

void EngineTempGauge::Update(IPlayer *player) {
    if (!mEngineTempChanged) {
        return;
    }
    mEngineTempChanged = false;

    if (mpEngineTempGaugeBar != nullptr) {
        const float min_angle = -26.5f;
        const float max_angle = 26.5f;
        const float min_engineheat = 0;
        const float max_engineheat = 0;
        const float frac = 0;
        const float angle = mEngineTemp;
        FEngSetMultiImageRot(mpEngineTempGaugeBar, angle * max_angle + min_angle);

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

    if (mpWarningLight != nullptr) {
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
