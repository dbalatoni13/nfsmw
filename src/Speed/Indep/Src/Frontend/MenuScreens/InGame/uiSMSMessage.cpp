#include "uiSMSMessage.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/CTextScroller.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/CTextScroller.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
const char* GetLocalizedString(unsigned int hash);

uiSMSMessage::uiSMSMessage(ScreenConstructorData* sd)
    : MenuScreen(sd) {
    pScrollBar = nullptr;
    Setup();
}

uiSMSMessage::~uiSMSMessage() {
    delete pScrollBar;
}

void uiSMSMessage::Setup() {
    RefreshHeader();
}

void uiSMSMessage::RefreshHeader() {
}

eMenuSoundTriggers uiSMSMessage::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return maybe;
}

void uiSMSMessage::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackagePop(0);
    }
}
