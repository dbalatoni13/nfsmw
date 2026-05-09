#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

WrongWIndi::WrongWIndi(UTL::COM::Object *pOutter, const char *pkg_name, int player_number) : HudElement(pkg_name, 0x20), IWrongWay(pOutter) {
    mIsWrongWay = false;
    mTimeBeforeDisplaying = 0;
    mTimeBeforeClosing = 0;
    mpWrongWayImage = RegisterImage(FEHashUpper("WRONGWAYIMAGE"));
}

void WrongWIndi::Update(IPlayer *player) {
    if (mIsWrongWay) {
        if (mTimeBeforeDisplaying.IsSet()) {
            Timer diff = WorldTimer - mTimeBeforeDisplaying;
            if (diff.GetSeconds() >= 2.0f) {
                mTimeBeforeDisplaying.UnSet();
                if (FEDatabase->GetVideoSettings()->WideScreen) {
                    if (!FEngIsScriptSet(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x908e787e)) {
                        FEngSetScript(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x908e787e, true);
                    }
                } else {
                    if (!FEngIsScriptSet(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x47510b1e)) {
                        FEngSetScript(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x47510b1e, true);
                    }
                }
            }
        }
    } else {
        if (mTimeBeforeClosing.IsSet()) {
            Timer diff = WorldTimer - mTimeBeforeClosing;
            if (diff.GetSeconds() >= 2.0f) {
                mTimeBeforeClosing.UnSet();
                if (!FEngIsScriptSet(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x16a259)) {
                    FEngSetScript(reinterpret_cast<FEObject *>(mpWrongWayImage), 0x16a259, true);
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
