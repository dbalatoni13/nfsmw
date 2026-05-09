#include "uiSMS.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

SMSMessage *the_sms_msg;

void SMSDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg != 0x0C407210) {
        return;
    }
    the_sms_msg = my_msg;
}

void SMSSlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        SMSMessage *msg = static_cast<SMSDatum *>(datum)->my_msg;
        FEngSetLanguageHash(text, FEngHashString("SMS_SUBJECT_%d", msg->GetHandle()));
        if (datum->IsChecked()) {
            FEngSetVisible(reinterpret_cast<FEObject *>(icon));
        } else {
            FEngSetInvisible(reinterpret_cast<FEObject *>(icon));
        }
    }
}

uiSMS::uiSMS(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 1, 6, true) {
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
    the_sms_msg = reinterpret_cast<SMSMessage *>(sd->Arg);
    if (the_sms_msg == nullptr) {
        if (GetCurrentDatum() == nullptr) {
            the_sms_msg = nullptr;
        } else {
            the_sms_msg = static_cast<SMSDatum *>(GetCurrentDatum())->my_msg;
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

int SortSMS(SMSSortNode *before, SMSSortNode *after) {
    return after->the_msg->GetSortOrder() < before->the_msg->GetSortOrder();
}

void uiSMS::Setup() {
    bool new_voice = false;
    bool new_text = false;
    ClearData();
    bTList<SMSSortNode> msgs;
    for (int i = 0x95; i >= 0; i--) {
        SMSMessage *msg = FEDatabase->GetCareerSettings()->GetSMSMessage(i);
        if (msg->IsValid() && (msg->IsRead() || msg->IsUnRead())) {
            SMSSortNode *node = new (__FILE__, __LINE__) SMSSortNode(msg);
            msgs.AddTail(node);
        }
    }
    msgs.Sort(SortSMS);
    int index = 0;
    for (int i = 0; i < msgs.CountElements(); i++) {
        SMSMessage *msg = msgs.GetNode(i)->the_msg;
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
            FEngSetScript(GetPackageName(), 0x19161CCC, 0x249DB7B7, true);
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
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x249DB7B7, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x16A259, true);
        }
    }
    for (int i = 0; i < GetNumDatum(); i++) {
        SMSDatum *datum = static_cast<SMSDatum *>(GetDatumAt(i));
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
        the_sms_msg = static_cast<SMSDatum *>(GetCurrentDatum())->my_msg;
    } else {
        the_sms_msg = nullptr;
    }
    RefreshHeader();
}

void uiSMS::AddSMSDatum(SMSMessage *msg) {
    if (bVoiceMsg) {
        if (last_msg[0] == 0xFF) {
            last_msg[0] = msg->GetHandle();
        }
    } else {
        if (last_msg[1] == 0xFF) {
            last_msg[1] = msg->GetHandle();
        }
    }
    SMSDatum *datum = new (__FILE__, __LINE__) SMSDatum(msg);
    AddDatum(datum);
    if (msg->IsUnRead()) {
        ArrayDatum *d = GetDatumAt(GetNumDatum() - 1);
        d->SetChecked(true);
    }
}

void uiSMS::AddSMSSlot(uint32 index) {
    ArrayScroller *scroller = this;
    uint32 grp_hash = FEngHashString("SMS_GROUP_%d", index);
    uint32 img_hash = FEngHashString("SMS_ICON_%d", index);
    uint32 txt_hash = FEngHashString("SMS_TEXT_%d", index);
    FEObject *grp = FEngFindGroup(GetPackageName(), grp_hash);
    FEImage *img = FEngFindImage(GetPackageName(), img_hash);
    FEString *txt = FEngFindString(GetPackageName(), txt_hash);
    SMSSlot *slot = new (__FILE__, __LINE__) SMSSlot(reinterpret_cast<FEGroup *>(grp), img, txt);
    scroller->AddSlot(slot);
}

void uiSMS::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (bVoiceMsg) {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C2, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, 0x249DB7B7, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, 0x7AB5521A, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4A2EEBC8, 0x1B20C3, true);
        FEngSetScript(GetPackageName(), 0x8A6AD1C1, 0x7AB5521A, true);
        FEngSetScript(GetPackageName(), 0x8F2FAD70, 0x249DB7B7, true);
    }
    if (GetNumDatum() < 1) {
        FEngSetScript(GetPackageName(), 0x07890734, 0x16A259, true);
    }
}

void uiSMS::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
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
            if (the_sms_msg != nullptr && bAutoPlay) {
                FEngSetScript(GetPackageName(), 0x47ff4e7c, bStringHash("READ"), true);
                cFEng::Get()->QueuePackagePush("SMS_Message.fng", reinterpret_cast<int>(the_sms_msg), 0, false);
            }
            bInitCompleted = false;
            break;
        case 0x35f8620b:
            bInitCompleted = true;
            break;
        case 0x775ce5df:
            if (!the_sms_msg->IsValid()) {
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
            SMSDatum *datum = static_cast<SMSDatum *>(GetCurrentDatum());
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
            if (the_sms_msg == nullptr) {
                goto fallthrough_msg;
            }
            if (!the_sms_msg->IsValid()) {
                goto fallthrough_msg;
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4, 0xd05fc3a3,
                                            0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0x8c3c2171);
            break;
        case 0xd05fc3a3: {
            cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            SMSDatum *datum = static_cast<SMSDatum *>(GetCurrentDatum());
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
            cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            break;
        case 0xe1fde1d1:
            if (button_pressed != 0x0c407210) {
                break;
            }
            if (GetCurrentDatum() == nullptr) {
                break;
            }
            cFEng::Get()->QueuePackagePush("SMS_Message.fng", reinterpret_cast<int>(the_sms_msg), 0, false);
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
