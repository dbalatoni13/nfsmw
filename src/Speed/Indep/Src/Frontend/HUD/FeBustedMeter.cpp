#include "Speed/Indep/Src/Frontend/HUD/FeBustedMeter.hpp"

BustedMeter::BustedMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x800000) //
    , IBustedMeter(pOutter) //
    , mInPursuit(false) //
    , mTimeUntilBusted(0.0f) //
    , mIsBusted(false) //
    , mBustedFlasherShown(false)
{
}

void BustedMeter::Update(IPlayer *player) {
}

void BustedMeter::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}

void BustedMeter::SetIsHiding(bool isHiding) {
    mIsHiding = isHiding;
}

void BustedMeter::SetTimeUntilBusted(float time) {
    mTimeUntilBusted = time;
}

void BustedMeter::SetIsBusted(bool isBusted) {
    mIsBusted = isBusted;
    if (!isBusted) {
        if (mBustedFlasherShown) {
            mBustedFlasherShown = false;
        }
    }
}
