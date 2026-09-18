#include "uiSMS.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

struct FEObject;
struct FEGroup;

// [fe3] FEObject* FEngFindGroup(const char* pkg_name, unsigned int obj_hash);
unsigned int FEngHashString(const char* format, ...);
// [fe3] const char* GetLocalizedString(unsigned int hash);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
unsigned int bStringHash(const char* str);

static SMSMessage* the_msg = nullptr;

struct SMSDatum : public ArrayDatum {
    SMSMessage* my_msg; // offset 0x24, size 0x4

    SMSDatum(SMSMessage* msg)
        : ArrayDatum(0, 0) //
        , my_msg(msg)
    {}

    ~SMSDatum() override {}

    void NotificationMessage(u32 msg, FEObject* pObj, u32 param1, u32 param2) override;
};

struct SMSSortNode : public bTNode<SMSSortNode> {
    SMSMessage* the_msg; // offset 0x8, size 0x4

    SMSSortNode(SMSMessage* msg)
        : the_msg(msg)
    {}

    ~SMSSortNode() {}
};

void SMSDatum::NotificationMessage(u32 msg, FEObject* pObj, u32 param1, u32 param2) {
    if (msg != 0x0C407210) {
        return;
    }
    the_msg = my_msg;
}


void SMSSlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum == nullptr) {
        return;
    }

    SMSMessage *msg = static_cast<SMSDatum *>(datum)->my_msg;
    FEngSetLanguageHash(pText, msg->GetSubjectHash());
    if (datum->IsChecked()) {
        FEngSetVisible(pIcon);
    } else {
        FEngSetInvisible(pIcon);
    }
}

uiSMS::uiSMS(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 1, 6, true) {
    button_pressed = 0;
    bVoiceMsg = true;
    bAutoPlay = false;
    bWaitingForMemcard = true;
    bInitCompleted = false;
    SetClickToSelectMode(true);
    for (int i = 0; i < 2; i++) {
        last_msg[i] = 0xFF;
    }
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        AddSMSSlot(i + 1);
    }
    Setup();
    the_msg = reinterpret_cast<SMSMessage*>(sd->Arg);
    if (the_msg == nullptr) {
        if (GetCurrentDatum() != nullptr) {
            the_msg = static_cast<SMSDatum*>(GetCurrentDatum())->my_msg;
        } else {
            the_msg = nullptr;
        }
        FEngSetScript(GetPackageName(), 0x2CF801C2, FEHASH_APPEAR, true);
    } else {
        bAutoPlay = true;
    }
    if (the_msg != nullptr) {
        bVoiceMsg = the_msg->IsVoice();
        if (the_msg->IsVoice()) {
            last_msg[0] = the_msg->GetHandle();
        } else {
            last_msg[1] = the_msg->GetHandle();
        }
    }
    if (MemoryCard::GetInstance()->IsAutoSaving() || MemoryCard::GetInstance()->AutoSaveRequested()) {
        cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE_INCOMING"), GetPackageName(), nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SMS_MAILBOX"), nullptr, nullptr);
    }
}



int SortSMS(SMSSortNode* before, SMSSortNode* after) {
    return after->the_msg->GetSortOrder() < before->the_msg->GetSortOrder();
}

void uiSMS::Setup() {
    bool new_voice = false;
    bool new_text = false;
    ClearData();
    bTList<SMSSortNode> msgs;
    for (int i = 0x95; i >= 0; i--) {
        SMSMessage* msg = FEDatabase->GetCareerSettings()->GetSMSMessage(i);
        if (msg->IsValid() && (msg->IsRead() || msg->IsUnRead())) {
            SMSSortNode* node = new ("SMSSortNode", 0) SMSSortNode(msg);
            msgs.AddTail(node);
        }
    }
    msgs.Sort(SortSMS);
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
    if (bVoiceMsg) {
        if (new_voice) {
            FEngSetScript(GetPackageName(), 0x19161CCC, FEHASH_HIGHLIGHT, true);
        } else {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x16A259, true);
        }
        if (new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x1CA7C0, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x16A259, true);
        }
    } else {
        if (new_voice) {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x1CA7C0, true);
        } else {
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x16A259, true);
        }
        if (new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, FEHASH_HIGHLIGHT, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x16A259, true);
        }
    }
    int index = 0;
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
    if (GetCurrentDatum()) {
        the_msg = static_cast<SMSDatum*>(GetCurrentDatum())->my_msg;
    } else {
        the_msg = nullptr;
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
    SMSDatum* datum = new ("SMSDatum", 0) SMSDatum(msg);
    AddDatum(datum);
    if (msg->IsUnRead()) {
        ArrayDatum* d = GetDatumAt(GetNumDatum() - 1);
        d->SetChecked(true);
    }
}

void uiSMS::AddSMSSlot(uint32 index) {
    uint32 grp_hash = FEngHashString("MESSAGE_GROUP_%d", index);
    uint32 img_hash = FEngHashString("NEW_MESSAGE_ICON_%d", index);
    uint32 txt_hash = FEngHashString("MESSAGE_TEXT_%d", index);
    FEGroup *grp = FEngFindGroup(GetPackageName(), grp_hash);
    FEImage *img = FEngFindImage(GetPackageName(), img_hash);
    FEString *txt = FEngFindString(GetPackageName(), txt_hash);
    this->AddSlot(new ("SMSSlot", 0) SMSSlot(grp, img, txt));
}

void uiSMS::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (bVoiceMsg) {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C2, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, FEHASH_HIGHLIGHT, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, FEHASH_UNHIGHLIGHT, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C3, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, FEHASH_HIGHLIGHT, true);
    }
    if (GetNumDatum() < 1) {
        FEngSetScript(GetPackageName(), 0x07890734, 0x16A259, true);
    }
}

void uiSMS::NotificationMessage(u32 msg, FEObject* obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
    case 0xc98356ba:
        if (cFEng::Get()->IsPackagePushed("InGame_MC_Main_GC.fng")) {
            bWaitingForMemcard = true;
        } else {
            bWaitingForMemcard = false;
        }
        if (bWaitingForMemcard) {
            break;
        }
        if (!bInitCompleted) {
            break;
        }
        if (the_msg != nullptr && bAutoPlay) {
            FEngSetScript(GetPackageName(), 0x47ff4e7c, bStringHash("READ"), true);
            cFEng::Get()->QueuePackagePush("SMS_Message.fng", reinterpret_cast<int>(the_msg), 0, false);
        }
        bInitCompleted = false;
        break;
    case 0x35f8620b:
        bInitCompleted = true;
        break;
    case 0x775ce5df:
        if (!the_msg->IsValid()) {
            for (int i = 0; i < 2; i++) {
                last_msg[i] = 0xFF;
            }
        }
        Setup();
        break;
    case 0x0c407210:
        if (GetCurrentDatum() == nullptr) {
            goto fallthrough_msg;
        }
        button_pressed = 0x0c407210;
        FEngSetScript(GetPackageName(), 0x47ff4e7c, bStringHash("READ"), true);
        break;
    case 0x72619778:
    case 0x911c0a4b: {
        SMSDatum* datum = static_cast<SMSDatum*>(GetCurrentDatum());
        if (datum == nullptr) {
            break;
        }
        if (bVoiceMsg) {
            last_msg[0] = datum->my_msg->GetHandle();
        } else {
            last_msg[1] = datum->my_msg->GetHandle();
        }
        break;
    }
    case 0x9120409e:
        ScrollBoxes(static_cast<eScrollDir>(-1));
        break;
    case 0xb5971bf1:
        ScrollBoxes(static_cast<eScrollDir>(1));
        break;
    case 0xc519bfc4:
        if (the_msg == nullptr) {
            goto fallthrough_msg;
        }
        if (!the_msg->IsValid()) {
            goto fallthrough_msg;
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng",
            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
            0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
            static_cast<eDialogFirstButtons>(1), 0x8c3c2171);
        break;
    case 0xd05fc3a3: {
        cFEng::Get()->QueuePackageMessage(__ENABLE_INPUTS__, GetPackageName(), nullptr);
        SMSDatum* datum = static_cast<SMSDatum*>(GetCurrentDatum());
        if (datum == nullptr) {
            break;
        }
        datum->my_msg->ClearFlags();
        Setup();
        break;
    }
    case 0x34dc1bcf:
    case 0x1fab5998:
    fallthrough_msg:
        cFEng::Get()->QueuePackageMessage(__ENABLE_INPUTS__, GetPackageName(), nullptr);
        break;
    case 0xe1fde1d1:
        if (button_pressed != 0x0c407210) {
            break;
        }
        if (GetCurrentDatum() == nullptr) {
            break;
        }
        cFEng::Get()->QueuePackagePush("SMS_Message.fng", reinterpret_cast<int>(the_msg), 0, false);
        break;
    case 0x911ab364:
        button_pressed = 0x911ab364;
        cFEng::Get()->QueuePackagePop(0);
        break;
    }
}

eMenuSoundTriggers uiSMS::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == 0x9120409e || msg == 0xb5971bf1 || msg == 0x48122792 || msg == 0x4ac5e165) {
        return maybe;
    }
    return ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
}

void uiSMS::ScrollBoxes(eScrollDir dir) {
    bVoiceMsg = !bVoiceMsg;
    Setup();
}
