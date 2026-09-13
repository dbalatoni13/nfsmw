#include "uiSMSMessage.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"

uiSMSMessage::uiSMSMessage(ScreenConstructorData *sd) : MenuScreen(sd), ScrollBar(sd->PackageFilename, "scrollbar", true, true, false) {
    the_msg = reinterpret_cast<SMSMessage *>(sd->Arg);
    new ESndGameState(0xd, true);
    SoundPause(true, eSNDPAUSE_SMS_MESSAGE);
    SetSoundControlState(false, SNDSTATE_FE_UPSCREEN, "SMSMesUnPause");
    SetSoundControlState(true, SNDSTATE_FE_SMS_MESSAGE, "SMSMes");
    Setup();
}

uiSMSMessage::~uiSMSMessage() {
    new ESndGameState(0xd, false);
    SoundPause(false, eSNDPAUSE_SMS_MESSAGE);
    SetSoundControlState(true, SNDSTATE_FE_UPSCREEN, "SMSMesPause");
    SetSoundControlState(false, SNDSTATE_FE_SMS_MESSAGE, "SMSMes");
}

void uiSMSMessage::Setup() {
    FEString *pString = FEngFindString(GetPackageName(), FEHashUpper("MESSAGE_TEXT_1"));
    m_TextScroller.Initialise(this, static_cast<int>(pString->MaxWidth), 10, "MESSAGE_TEXT_%d", FindFont(pString->Handle));
    m_TextScroller.UseScrollBar(&ScrollBar);
    m_TextScroller.SetTextHash(the_msg->GetMsgHash());
    if (!the_msg->IsVoice()) {
        FEngSetInvisible(GetPackageName(), 0x2a631207);
        FEngSetInvisible(GetPackageName(), 0x914614e5);
    }
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
        Speech::Manager::ClearPlayback();
    }
    RefreshHeader();
}

void uiSMSMessage::RefreshHeader() {
    FEngSetLanguageHash(GetPackageName(), 0xfeced617, the_msg->GetFromHash());
    FEngSetLanguageHash(GetPackageName(), 0x2c167533, the_msg->GetSubjectHash());
}

eMenuSoundTriggers uiSMSMessage::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == 0x610fb237 && !the_msg->IsVoice()) {
        return UISND_NONE;
    }
    return maybe;
}

// UNSOLVED
void uiSMSMessage::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    m_TextScroller.HandleNotificationMessage(msg);
    switch (msg) {
        case dialog_message_no:
            break;
        case FEHASH_INITCOMPLETE: {
            if (the_msg->IsVoice() && the_msg->IsUnRead()) {
                MiscSpeech::SMSCellCall(the_msg->GetHandle());
            }
            the_msg->ClearFlags();
            the_msg->SetFlag(4);
            break;
        }
        case FEHASH_EXITCOMPLETE: {
            Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
            if (cop_speech != nullptr) {
                cop_speech->ReleaseResource();
                Speech::Manager::ClearPlayback();
            }
            cFEng::Get()->QueuePackagePop(1);
            break;
        }
        case __PAD_BUTTON4__: {
            if (the_msg->IsVoice()) {
                Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
                if (cop_speech != nullptr) {
                    cop_speech->ReleaseResource();
                    Speech::Manager::ClearPlayback();
                }
                MiscSpeech::SMSCellCall(the_msg->GetHandle());
            }
            break;
        }
        case __PAD_BUTTON5__: {
            DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
                                            dialog_message_yes, dialog_message_no, dialog_message_no, first_dialog_button2, 0x8c3c2171);
            break;
        }
        case dialog_message_yes: {
            the_msg->ClearFlags();
            cFEng::Get()->QueuePackageMessage(__PAD_BACK__, GetPackageName(), nullptr);
            break;
        }
    }
}
