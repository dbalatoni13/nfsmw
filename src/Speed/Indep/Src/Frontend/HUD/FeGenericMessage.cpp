#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"

extern bool FEngIsScriptRunning(FEObject *object, unsigned int script_hash);
extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
extern char *bStrCpy(char *dst, const char *src);
extern char *bSafeStrCpy(char *dst, const char *src, int maxlen);
extern int FEPrintf(const char *pkg_name, int obj_hash, const char *fmt, ...);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x01000000ULL) //
    , IGenericMessage(pOutter)
{
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
        if (!FEngIsScriptRunning(mpMessageFirstLine, mFengHash) ||
            (mPlayOneFrame && mNumFramesPlayed != 0)) {
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

GenericMessage_Priority GenericMessage::GetCurrentGenericMessagePriority() {
    return mPriority;
}

void GenericMessage::RequestGenericMessageZoomOut(unsigned int fengHash) {
    if (!FEngIsScriptSet(GetPackageName(), 0xe0ba07ec, 0xe1c034fc)) {
        FEngSetScript(GetPackageName(), 0xe0ba07ec, 0xe1c034fc, true);
    }
}

bool GenericMessage::RequestGenericMessage(const char *string, bool singleFrame,
    unsigned int fengHash, unsigned int iconTextureHash, unsigned int iconFengHash,
    GenericMessage_Priority priority) {
    if (priority < mPriority) {
        return false;
    }
    if (!FEngIsScriptSet(pPackageName, 0xe0ba07ec, 0x1744b3)) {
        FEngSetScript(pPackageName, 0xe0ba07ec, 0x1744b3, true);
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
        FEPrintf(pPackageName, 0x32a7a521, "%s", mStringBuffer);
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

bool GenericMessage::IsGenericMessageShowing() {
    return mPriority > 0;
}
