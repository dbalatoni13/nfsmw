#include "Speed/Indep/Src/Frontend/HUD/FeDragTachometer.hpp"

extern const float lbl_803E5868;
extern const float lbl_803E586C;
extern const float lbl_803E5870;
extern const float lbl_803E5874;

float DragTachometer::CalcAngleForRPMDrag(float rpm, float redline) {
    float factor = rpm / ChooseMaxRpmTextureNumber(redline);
    if (factor < lbl_803E5868) {
        factor = lbl_803E5868;
    }
    if (factor > lbl_803E586C) {
        factor = lbl_803E586C;
    }
    float min_angle = lbl_803E5870;
    float max_angle = lbl_803E5874;
    float fRange = max_angle - min_angle;
    return factor * fRange + min_angle;
}

DragTachometer::DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ITachometer(pOutter) //
    , ITachometerDrag(pOutter)
{
}

void DragTachometer::Update(IPlayer *player) {
}

void DragTachometer::SetRpm(float rpm) {
    mRpm = rpm;
}

void DragTachometer::SetInPerfectLaunchRange(bool inRange) {
    mInPerfectLaunchRange = inRange;
}

void DragTachometer::SetShifting(bool shifting) {
    mGearShifting = shifting;
}

void DragTachometer::SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) {
    if (gear != mGear) {
        mGear = gear;
    }
}
