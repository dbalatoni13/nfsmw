#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"

float RadarDetector::mStaticRange;

RadarDetector::RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IRadarDetector(pOutter)
{
}

void RadarDetector::Update(IPlayer *player) {
}

void RadarDetector::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}

void RadarDetector::SetIsCoolingDown(bool coolingDown) {
    mIsCoolingDown = coolingDown;
}

void RadarDetector::SetTarget(RadarTarget targetType, float range, float direction) {
    mTargetType = targetType;
    mDirection = direction;
    mRange = range;
    mStaticRange = range;
}
