#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"

#include "Speed/Indep/Src/FEng/FEImage.h"

void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
unsigned long FEHashUpper(const char *name);

extern const char lbl_803E4F18[];
extern const float lbl_803E4F24;
extern const float lbl_803E4F28;

WrongWIndi::WrongWIndi(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x20) //
    , IWrongWay(pOutter) //
    , mIsWrongWay(false) //
    , mTimeBeforeDisplaying(0) //
    , mTimeBeforeClosing(0) //
{
    mpWrongWayImage = RegisterImage(FEHashUpper(lbl_803E4F18));
}

void WrongWIndi::Update(IPlayer *player) {
    Timer dt(WorldTimeElapsed);
    if (!mIsWrongWay) {
        if (mTimeBeforeClosing > Timer(0)) {
            mTimeBeforeClosing -= dt;
        } else {
            FEngSetInvisible(mpWrongWayImage);
        }
        mTimeBeforeDisplaying = WorldTimer;
    } else {
        if (mTimeBeforeDisplaying > Timer(0)) {
            mTimeBeforeDisplaying -= dt;
        } else {
            FEngSetVisible(mpWrongWayImage);
            mTimeBeforeClosing = WorldTimer;
        }
    }
}

void WrongWIndi::SetWrongWay(bool isWrongWay) {
    if (mIsWrongWay == isWrongWay) {
        return;
    }
    if (isWrongWay) {
        mTimeBeforeDisplaying = WorldTimer;
        mTimeBeforeClosing.UnSet();
    } else {
        mTimeBeforeClosing = WorldTimer;
        mTimeBeforeDisplaying.UnSet();
    }
    mIsWrongWay = isWrongWay;
}
