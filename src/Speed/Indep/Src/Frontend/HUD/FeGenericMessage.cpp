#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x01000000), //
      IGenericMessage(pOutter) {
    mPriority = GenericMessage_Priority_None;
    mNumFramesPlayed = 0;
    mFengHash = 0;
    mPlayOneFrame = false;
    bStrCpy(mStringBuffer, "");
    mpMessageFirstLine = RegisterGroup(0x32a7a521);
    mpIcon = RegisterObject(0x6dd754ec);
    RegisterObject(0xcaec9d04);
}

void GenericMessage::Update(IPlayer *player) {
    if (mPriority > GenericMessage_Priority_None) {
        if (!FEngIsScriptRunning(mpMessageFirstLine, mFengHash) || (mPlayOneFrame && mNumFramesPlayed != 0)) {
            if (!FEngIsScriptSet(mpMessageFirstLine, FEHASH_HIDE)) {
                FEngSetScript(mpMessageFirstLine, FEHASH_HIDE, true);
            }
            mPriority = GenericMessage_Priority_None;
            bStrCpy(mStringBuffer, "");
            if (!FEngIsScriptSet(mpIcon, FEHASH_HIDE)) {
                FEngSetScript(mpIcon, FEHASH_HIDE, true);
            }
        }
        mNumFramesPlayed++;
    }
}

bool GenericMessage::RequestGenericMessage(const char *string, bool singleFrame, uint32 fengHash, uint32 iconTextureHash, uint32 iconFengHash,
                                           GenericMessage_Priority priority) {
    if (priority < mPriority) {
        return false;
    }
    if (!FEngIsScriptSet(GetPackageName(), 0xe0ba07ec, FEHASH_INIT)) {
        FEngSetScript(GetPackageName(), 0xe0ba07ec, FEHASH_INIT, true);
    }
    mPriority = priority;
    mNumFramesPlayed = 0;
    mPlayOneFrame = singleFrame;
    mFengHash = fengHash;
    if (string != nullptr) {
        bSafeStrCpy(mStringBuffer, string, sizeof(mStringBuffer));
        if (fengHash) {
            if (mPlayOneFrame) {
                if (!FEngIsScriptSet(mpMessageFirstLine, fengHash)) {
                    FEngSetScript(mpMessageFirstLine, fengHash, true);
                }
            } else {
                FEngSetScript(mpMessageFirstLine, fengHash, true);
            }
        }
        FEPrintf(GetPackageName(), 0x32a7a521, "%s", mStringBuffer);
    }
    if (iconFengHash != 0 && iconTextureHash != 0) {
        if (mPlayOneFrame) {
            if (!FEngIsScriptSet(mpIcon, iconFengHash)) {
                FEngSetScript(mpIcon, iconFengHash, true);
            }
        } else {
            FEngSetScript(mpIcon, iconFengHash, true);
        }
        FEngSetTextureHash(static_cast<FEImage *>(mpIcon), iconTextureHash);
    } else {
        if (!FEngIsScriptSet(mpIcon, FEHASH_HIDE)) {
            FEngSetScript(mpIcon, FEHASH_HIDE, true);
        }
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
