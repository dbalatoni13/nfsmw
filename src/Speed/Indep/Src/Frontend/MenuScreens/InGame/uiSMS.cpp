#include "uiSMS.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;
struct FEGroup;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEObject* FEngFindGroup(const char* pkg_name, unsigned int obj_hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
FEString* FEngFindString(const char* pkg_name, int name_hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);

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
        FEngSetLanguageHash(pText, FEngHashString("SMS_SUBJECT_%d", msg->GetHandle()));
        if (datum->IsChecked()) {
            FEngSetVisible(reinterpret_cast<FEObject*>(pIcon));
        } else {
            FEngSetInvisible(reinterpret_cast<FEObject*>(pIcon));
        }
    }
}

uiSMS::uiSMS(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 1, 6, true) {
    bInitCompleted = false;
    bVoiceMsg = true;
    button_pressed = 0;
    bAutoPlay = false;
    bWaitingForMemcard = true;
    for (int i = 0; i < 2; i++) {
        last_msg[i] = 0xFF;
    }
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        AddSMSSlot(i + 1);
    }
    Setup();
    the_sms_msg = reinterpret_cast<SMSMessage*>(sd->Arg);
    if (the_sms_msg == nullptr) {
        if (GetCurrentDatum() == nullptr) {
            the_sms_msg = nullptr;
        } else {
            the_sms_msg = static_cast<SMSDatum*>(GetCurrentDatum())->my_msg;
        }
        FEngSetScript(GetPackageName(), 0x2CF801C2, 0x5079C8F8, true);
    } else {
        bAutoPlay = true;
    }
    if (the_sms_msg != nullptr) {
        bVoiceMsg = the_sms_msg->IsVoice();
        if (!the_sms_msg->IsVoice()) {
            last_msg[1] = the_sms_msg->GetHandle();
        } else {
            last_msg[0] = the_sms_msg->GetHandle();
        }
    }
    if (MemoryCard::GetInstance()->IsAutoSaving() || MemoryCard::GetInstance()->AutoSaveRequested()) {
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SMS_SAVING"), GetPackageName(), nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SMS_SAVING2"), nullptr, nullptr);
    }
}

void uiSMS::Setup() {
    bool new_voice = false;
    bool new_text = false;
    ClearData();
    bTList<SMSSortNode> msgs;
    for (int i = 0x95; i >= 0; i--) {
        SMSMessage* msg = FEDatabase->GetCareerSettings()->GetSMSMessage(i);
        if (msg->IsValid() && (msg->IsRead() || msg->IsUnRead())) {
            SMSSortNode* node = new (__FILE__, __LINE__) SMSSortNode(msg);
            msgs.AddTail(node);
        }
    }
    msgs.Sort(SortSMS);
    int index = 0;
    for (int i = 0; i < msgs.CountElements(); i++) {
        SMSMessage* msg = msgs.GetNode(i)->the_msg;
        if (msg->IsUnRead()) {
            if (msg->IsVoice()) {
                new_voice = true;
            } else {
                new_text = true;
            }
        }
        if (bVoiceMsg) {
            if (msg->IsVoice()) {
                AddSMSDatum(msg);
            }
        } else {
            if (!msg->IsVoice()) {
                AddSMSDatum(msg);
            }
        }
    }
    if (!bVoiceMsg) {
        if (new_voice) {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x1CA7C0, true);
        } else {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x16A259, true);
        }
        if (!new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x16A259, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x249DB7B7, true);
        }
    } else {
        if (new_voice) {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x249DB7B7, true);
        } else {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x16A259, true);
        }
        if (new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x1CA7C0, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x16A259, true);
        }
    }
    for (int i = 0; i < GetNumDatum(); i++) {
        SMSDatum* datum = static_cast<SMSDatum*>(GetDatumAt(i));
        if (bVoiceMsg) {
            if (datum->my_msg->GetHandle() == last_msg[0]) {
                index = i;
            }
        } else {
            if (datum->my_msg->GetHandle() == last_msg[1]) {
                index = i;
            }
        }
    }
    SetInitialPosition(index);
    if (GetCurrentDatum() == nullptr) {
        the_sms_msg = nullptr;
    } else {
        the_sms_msg = static_cast<SMSDatum*>(GetCurrentDatum())->my_msg;
    }
    RefreshHeader();
}

void uiSMS::AddSMSDatum(SMSMessage* msg) {
    if (bVoiceMsg) {
        if (last_msg[0] == 0xFF) {
            last_msg[0] = msg->GetHandle();
        }
    } else {
        if (last_msg[1] == 0xFF) {
            last_msg[1] = msg->GetHandle();
        }
    }
    SMSDatum* datum = new (__FILE__, __LINE__) SMSDatum(msg);
    AddDatum(datum);
    if (msg->IsUnRead()) {
        ArrayDatum* d = GetDatumAt(GetNumDatum() - 1);
        d->SetChecked(true);
    }
}

void uiSMS::AddSMSSlot(unsigned int index) {
    unsigned int grp_hash = FEngHashString("SMS_GROUP_%d", index);
    unsigned int img_hash = FEngHashString("SMS_ICON_%d", index);
    unsigned int txt_hash = FEngHashString("SMS_TEXT_%d", index);
    FEObject* grp = FEngFindGroup(GetPackageName(), grp_hash);
    FEImage* img = FEngFindImage(GetPackageName(), img_hash);
    FEString* txt = FEngFindString(GetPackageName(), txt_hash);
    SMSSlot* slot = new (__FILE__, __LINE__) SMSSlot(reinterpret_cast<FEGroup*>(grp), img, txt);
    AddSlot(slot);
}

void uiSMS::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (!bVoiceMsg) {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C3, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, 0x7AB5521A, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, 0x249DB7B7, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C2, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, 0x249DB7B7, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, 0x7AB5521A, true);
    }
    if (GetNumDatum() < 1) {
        FEngSetScript(GetPackageName(), 0x07890734, 0x16A259, true);
    }
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
    bVoiceMsg = !bVoiceMsg;
    Setup();
}
