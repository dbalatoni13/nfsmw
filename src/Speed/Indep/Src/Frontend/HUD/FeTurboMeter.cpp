#include "Speed/Indep/Src/Frontend/HUD/FeTurboMeter.hpp"

TurboMeter::TurboMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITurbometer(pOutter)
{
}

void TurboMeter::Update(IPlayer *player) {
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
