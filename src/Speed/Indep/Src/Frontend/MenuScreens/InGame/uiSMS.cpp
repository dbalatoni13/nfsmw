#include "uiSMS.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

SMSMessage *the_msg = nullptr;

void SMSDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg != __BUTTON_PRESSED__) {
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
    FEngSetLanguageHash(text, msg->GetSubjectHash());
    if (datum->IsChecked()) {
        FEngSetVisible(icon);
    } else {
        FEngSetInvisible(icon);
    }
}

uiSMS::uiSMS(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 1, 6, true) {
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
    the_msg = reinterpret_cast<SMSMessage *>(sd->Arg);
    if (the_msg == nullptr) {
        SMSDatum *datum = static_cast<SMSDatum *>(GetCurrentDatum());
        if (datum != nullptr) {
            the_msg = datum->my_msg;
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
            msgs.AddTail(new ("SMSSortNode", 0) SMSSortNode(msg));
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
            FEngSetScript(GetPackageName(), 0x19161CCC, FEHASH_HIDE, true);
        }
        if (new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, FEHASH_SHOW, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, FEHASH_HIDE, true);
        }
    } else {
        if (new_voice) {
            FEngSetScript(GetPackageName(), 0x19161CCC, FEHASH_SHOW, true);
        } else {
            FEngSetScript(GetPackageName(), 0x19161CCC, FEHASH_HIDE, true);
        }
        if (new_text) {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, 0x249DB7B7, true);
        } else {
            FEngSetScript(GetPackageName(), 0x0D6FD6F9, FEHASH_HIDE, true);
        }
    }
    index = 0;
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
    SMSDatum *datum = static_cast<SMSDatum *>(GetCurrentDatum());
    if (datum != nullptr) {
        the_msg = datum->my_msg;
    } else {
        the_msg = nullptr;
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
    AddDatum(new ("SMSDatum", 0) SMSDatum(msg));
    if (msg->GetFlags() & SMS_FLAG_UNREAD) {
        GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
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

    const u32 FEObj_POS1 = 0; // TODO
    const u32 FEObj_POS2 = 0;

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
        FEngSetScript(GetPackageName(), 0x07890734, FEHASH_HIDE, true);
    }
}

// UNSOLVED
void uiSMS::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case FEMSG_SCREEN_TICK:
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
        case FEHASH_INITCOMPLETE:
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
        case __BUTTON_PRESSED__:
            if (GetCurrentDatum() == nullptr) {
                goto fallthrough_msg;
            }
            button_pressed = __BUTTON_PRESSED__;
            FEngSetScript(GetPackageName(), 0x47ff4e7c, bStringHash("READ"), true);
            break;
        case __PAD_UP__:
        case __PAD_DOWN__: {
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
        case __PAD_LEFT__:
            ScrollBoxes(eSD_PREV);
            break;
        case __PAD_RIGHT__:
            ScrollBoxes(eSD_NEXT);
            break;
        case __PAD_BUTTON5__:
            if (the_msg == nullptr) {
                goto fallthrough_msg;
            }
            if (!the_msg->IsValid()) {
                goto fallthrough_msg;
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
                                            dialog_message_yes, dialog_message_no, dialog_message_no, first_dialog_button2, 0x8c3c2171);
            break;
        case dialog_message_yes: {
            cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            SMSDatum *datum = static_cast<SMSDatum *>(GetCurrentDatum());
            if (datum == nullptr) {
                break;
            }
            datum->my_msg->ClearFlags();
            Setup();
            break;
        }
        case dialog_message_no:
        case 0x1fab5998:
        fallthrough_msg:
            cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            break;
        case FEHASH_EXITCOMPLETE:
            if (button_pressed != __BUTTON_PRESSED__) {
                break;
            }
            if (GetCurrentDatum() == nullptr) {
                break;
            }
            cFEng::Get()->QueuePackagePush("SMS_Message.fng", reinterpret_cast<int>(the_msg), 0, false);
            break;
        case __PAD_BACK__:
            button_pressed = __PAD_BACK__;
            cFEng::Get()->QueuePackagePop(0);
            break;
    }
}

eMenuSoundTriggers uiSMS::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == __PAD_LEFT__ || msg == __PAD_RIGHT__ || msg == 0x48122792 || msg == 0x4ac5e165) {
        return maybe;
    }
    return ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
}

void uiSMS::ScrollBoxes(eScrollDir dir) {
    bVoiceMsg = !bVoiceMsg;
    Setup();
}
