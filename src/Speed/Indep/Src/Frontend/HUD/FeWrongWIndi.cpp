#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

WrongWIndi::WrongWIndi(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x20), IWrongWay(pOutter), mIsWrongWay(false) {
    mpWrongWayImage = RegisterImage(FEHashUpper("WRONGWAYIMAGE"));
}

void WrongWIndi::Update(IPlayer *player) {
    if (mIsWrongWay) {
        if (mTimeBeforeDisplaying.IsSet()) {
            if ((WorldTimer - mTimeBeforeDisplaying).GetSeconds() >= 2.0f) {
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
            if ((WorldTimer - mTimeBeforeClosing).GetSeconds() >= 2.0f) {
                mTimeBeforeClosing.UnSet();
                if (!FEngIsScriptSet(mpWrongWayImage, FEHASH_HIDE)) {
                    FEngSetScript(mpWrongWayImage, FEHASH_HIDE, true);
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
