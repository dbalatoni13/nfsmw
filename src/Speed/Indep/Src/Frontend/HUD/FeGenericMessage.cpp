#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"

extern bool FEngIsScriptRunning(FEObject *object, unsigned int script_hash);
extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
extern char *bStrCpy(char *dst, const char *src);

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IGenericMessage(pOutter)
{
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
    if (!FEngIsScriptSet(pPackageName, 0xe0ba07ec, 0xe1c034fc)) {
        FEngSetScript(pPackageName, 0xe0ba07ec, 0xe1c034fc, true);
    }
}

bool GenericMessage::IsGenericMessageShowing() {
    return mPriority > 0;
}
