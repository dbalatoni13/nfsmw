#include "uiSMS.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);

uiSMS::uiSMS(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 3, 3, true) {
    button_pressed = 0;
    bVoiceMsg = false;
    bAutoPlay = false;
    bWaitingForMemcard = false;
    bInitCompleted = false;
    Setup();
}

void uiSMS::Setup() {
    ClearData();
    SetInitialPosition(0);
    bInitCompleted = true;
    RefreshHeader();
}

void uiSMS::AddSMSDatum(SMSMessage* sms) {
}

void uiSMS::AddSMSSlot(unsigned int hash) {
}

void uiSMS::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
}

void uiSMS::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackagePop(0);
    }
}

eMenuSoundTriggers uiSMS::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
}

void uiSMS::ScrollBoxes(eScrollDir dir) {
}
