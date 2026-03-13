#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"

extern bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IGenericMessage(pOutter)
{
}

void GenericMessage::Update(IPlayer *player) {
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
