#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"

#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
unsigned long FEHashUpper(const char *name);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);

extern const char lbl_803E4F18[];
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
    if (mIsWrongWay) {
        if (mTimeBeforeDisplaying.IsSet()) {
            Timer diff = WorldTimer - mTimeBeforeDisplaying;
            if (diff.GetSeconds() >= 2.0f) {
                mTimeBeforeDisplaying.UnSet();
                if (FEDatabase->GetVideoSettings()->WideScreen) {
                    if (!FEngIsScriptSet(mpWrongWayImage, 0x908e787e)) {
                        FEngSetScript(mpWrongWayImage, 0x908e787e, true);
                    }
                } else {
                    if (!FEngIsScriptSet(mpWrongWayImage, 0x47510b1e)) {
                        FEngSetScript(mpWrongWayImage, 0x47510b1e, true);
                    }
                }
            }
        }
    } else {
        if (mTimeBeforeClosing.IsSet()) {
            Timer diff = WorldTimer - mTimeBeforeClosing;
            if (diff.GetSeconds() >= 2.0f) {
                mTimeBeforeClosing.UnSet();
                if (!FEngIsScriptSet(mpWrongWayImage, 0x16a259)) {
                    FEngSetScript(mpWrongWayImage, 0x16a259, true);
                }
            }
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
