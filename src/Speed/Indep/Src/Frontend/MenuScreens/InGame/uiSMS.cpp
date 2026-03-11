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
void FEngSetLanguageHash(FEString* text, unsigned int hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);


SMSMessage* the_sms_msg;

struct SMSDatum : public ArrayDatum {
    SMSMessage* my_msg; // offset 0x24, size 0x4

    SMSDatum(SMSMessage* msg)
        : ArrayDatum(0, 0) //
        , my_msg(msg)
    {}

    ~SMSDatum() override {}

    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) override;
};

struct SMSSortNode : public bTNode<SMSSortNode> {
    SMSMessage* the_msg; // offset 0x8, size 0x4

    SMSSortNode(SMSMessage* msg)
        : the_msg(msg)
    {}

    ~SMSSortNode() {}
};

void SMSDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {
    if (msg != 0x0C407210) {
        return;
    }
    the_sms_msg = my_msg;
}

int SortSMS(SMSSortNode* before, SMSSortNode* after) {
    return after->the_msg->GetSortOrder() < before->the_msg->GetSortOrder();
}

void SMSSlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        SMSMessage* msg = static_cast<SMSDatum*>(datum)->my_msg;
        FEString* text = *reinterpret_cast<FEString**>(reinterpret_cast<char*>(this) + 0x18);
        FEngSetLanguageHash(text, FEngHashString("SMS_SUBJECT_%d", *reinterpret_cast<unsigned char*>(msg)));
        if (datum->IsChecked()) {
            FEngSetVisible(pIcon);
        } else {
            FEngSetInvisible(pIcon);
        }
    }
}

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
