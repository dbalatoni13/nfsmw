#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x01000000ULL), //
      IGenericMessage(pOutter) {
    mPriority = GenericMessage_Priority_None;
    mNumFramesPlayed = 0;
    mFengHash = 0;
    mPlayOneFrame = false;
    bStrCpy(mStringBuffer, "");
    mpMessageFirstLine = reinterpret_cast<FEObject *>(RegisterGroup(0x32a7a521));
    mpIcon = RegisterObject(0x6dd754ec);
    RegisterObject(0xcaec9d04);
}

void GenericMessage::Update(IPlayer *player) {
    if (mPriority > GenericMessage_Priority_None) {
        if (!FEngIsScriptRunning(mpMessageFirstLine, mFengHash) || (mPlayOneFrame && mNumFramesPlayed != 0)) {
            if (!FEngIsScriptSet(mpMessageFirstLine, 0x16a259)) {
                FEngSetScript(mpMessageFirstLine, 0x16a259, true);
            }
            mPriority = GenericMessage_Priority_None;
            bStrCpy(mStringBuffer, "");
            if (!FEngIsScriptSet(mpIcon, 0x16a259)) {
                FEngSetScript(mpIcon, 0x16a259, true);
            }
        }
        mNumFramesPlayed++;
    }
}

bool GenericMessage::RequestGenericMessage(const char *string, bool singleFrame, unsigned int fengHash, unsigned int iconTextureHash,
                                           unsigned int iconFengHash, GenericMessage_Priority priority) {
    if (priority < mPriority) {
        return false;
    }
    if (!FEngIsScriptSet(GetPackageName(), 0xe0ba07ec, 0x1744b3)) {
        FEngSetScript(GetPackageName(), 0xe0ba07ec, 0x1744b3, true);
    }
    mPriority = priority;
    mNumFramesPlayed = 0;
    mPlayOneFrame = singleFrame;
    mFengHash = fengHash;
    if (string) {
        bSafeStrCpy(mStringBuffer, string, 0x40);
        if (fengHash) {
            if (!mPlayOneFrame) {
                FEngSetScript(mpMessageFirstLine, fengHash, true);
            } else {
                if (!FEngIsScriptSet(mpMessageFirstLine, fengHash)) {
                    FEngSetScript(mpMessageFirstLine, fengHash, true);
                }
            }
        }
        FEPrintf(GetPackageName(), 0x32a7a521, "%s", mStringBuffer);
    }
    if (iconFengHash == 0 || iconTextureHash == 0) {
        if (!FEngIsScriptSet(mpIcon, 0x16a259)) {
            FEngSetScript(mpIcon, 0x16a259, true);
        }
    } else {
        if (!mPlayOneFrame) {
            FEngSetScript(mpIcon, iconFengHash, true);
        } else {
            if (!FEngIsScriptSet(mpIcon, iconFengHash)) {
                FEngSetScript(mpIcon, iconFengHash, true);
            }
        }
        FEngSetTextureHash(static_cast<FEImage *>(mpIcon), iconTextureHash);
    }
    return true;
}

void GenericMessage::RequestGenericMessageZoomOut(unsigned int fengHash) {
    if (!FEngIsScriptSet(GetPackageName(), 0xe0ba07ec, 0xe1c034fc)) {
        FEngSetScript(GetPackageName(), 0xe0ba07ec, 0xe1c034fc, true);
    }
}

bool GenericMessage::IsGenericMessageShowing() {
    return mPriority > 0;
}
