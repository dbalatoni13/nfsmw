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
        mTimeBeforeDisplaying.SetTime(lbl_803E4F28);
    } else {
        if (mTimeBeforeDisplaying > Timer(0)) {
            mTimeBeforeDisplaying -= dt;
        } else {
            FEngSetVisible(mpWrongWayImage);
            mTimeBeforeClosing.SetTime(lbl_803E4F28);
        }
    }
}

void WrongWIndi::SetWrongWay(bool wrong_way) {
    mIsWrongWay = wrong_way;
}
