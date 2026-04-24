#include "Speed/Indep/Src/Frontend/HUD/FeTurboMeter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

extern void FEngSetRotationZ(FEObject *object, float angle);

TurboMeter::TurboMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x80020000) //
    , ITurbometer(pOutter) //
    , mUpdated(true) //
    , mInductionPsi(0.0f)
{
    pTurboGroup = RegisterGroup(FEHashUpper("TURBO_GROUP"));
    pTurboNeedle = FEngFindObject(GetPackageName(), FEHashUpper("3rdperson_TurboDial"));
    pTurboDialLines = FEngFindObject(GetPackageName(), FEHashUpper("TURBO_LINES"));
}

void TurboMeter::Update(IPlayer *player) {
    if (!mUpdated) {
        return;
    }
    mUpdated = false;
    float angle;
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceType() == GRace::kRaceType_Drag) {
        angle = CalcNeedleAngle(mInductionPsi, 200.0f, 270.0f);
    } else {
        angle = CalcNeedleAngle(mInductionPsi, -45.0f, 45.0f);
    }
    FEngSetRotationZ(pTurboNeedle, angle);
}

float TurboMeter::CalcNeedleAngle(float psi, float min_angle, float max_angle) {
    const float cMinPsi = -20.0f;
    const float cMaxPsi = 20.0f;
    float boost = (psi - cMinPsi) / (cMaxPsi - cMinPsi);
    float angle = -(min_angle + boost * (max_angle - min_angle));
    return angle;
}

void TurboMeter::SetInductionPsi(float psi) {
    if (mInductionPsi != psi) {
        mInductionPsi = psi;
        mUpdated = true;
    }
}
