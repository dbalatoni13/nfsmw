#include "uiSMSMessage.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEngFont;
struct FEString;

struct Module {
    virtual ~Module();
    virtual void Init();
    virtual void LoadBanks();
    virtual int TestSentenceRuleCallback();
    virtual int SetSentenceRuleCallback();
    virtual int EventRuleCallback();
    virtual int GetNumBanks();
    virtual unsigned int GetBankOffset(int bnum);
    virtual void Update();
    virtual const char* GetFilename();
    virtual bool QueStream(int stream_type, void (*callback)(), bool trigger);
    virtual unsigned int SampleRequestCallback();
    virtual bool IsStreamQueued();
    virtual const char* GetCSIptr();
    virtual int GetChannel();
    virtual const char* GetEventDat();
    virtual bool IsDataLoaded();
    virtual bool PlayStream(int stream_id);
    virtual void ReleaseResource();
};

namespace Speech {
namespace Manager {
Module* GetSpeechModule(int id);
void ClearPlayback();
} // namespace Manager
} // namespace Speech

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEString* FEngFindString(const char* pkg_name, int hash);
FEngFont* FindFont(unsigned int hash);
unsigned int FEngHashString(const char* format, ...);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void SoundPause(bool pause, eSNDPAUSE_REASON reason);
void SetSoundControlState(bool state, eSNDCTLSTATE ctl, const char* name);

namespace MiscSpeed {
void SMSCellCall(int handle);
}

uiSMSMessage::uiSMSMessage(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , ScrollBar(sd->PackageFilename, "scrollbar", true, true, false) {
    the_msg = reinterpret_cast< SMSMessage* >(sd->Arg);
    new ESndGameState(0xd, true);
    SoundPause(true, static_cast< eSNDPAUSE_REASON >(0xb));
    SetSoundControlState(false, static_cast< eSNDCTLSTATE >(4), "SMSMesUnPause");
    SetSoundControlState(true, static_cast< eSNDCTLSTATE >(5), "SMSMes");
    Setup();
}

uiSMSMessage::~uiSMSMessage() {
    new ESndGameState(0xd, false);
    SoundPause(false, static_cast< eSNDPAUSE_REASON >(0xb));
    SetSoundControlState(true, static_cast< eSNDCTLSTATE >(4), "SMSMesPause");
    SetSoundControlState(false, static_cast< eSNDCTLSTATE >(5), "SMSMes");
}

void uiSMSMessage::Setup() {
    FEString* pString = FEngFindString(GetPackageName(), FEHashUpper("MESSAGE_TEXT_1"));
    m_TextScroller.Initialise(this, static_cast< int >(pString->MaxWidth), 10,
                              "MESSAGE_TEXT_%d", FindFont(pString->Handle));
    m_TextScroller.UseScrollBar(&ScrollBar);
    m_TextScroller.SetTextHash(FEngHashString("SMS_MESSAGE_%d", the_msg->GetHandle()));
    if (!the_msg->IsVoice()) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x2a631207));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x914614e5));
    }
    Module* cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
        Speech::Manager::ClearPlayback();
    }
    RefreshHeader();
}

void uiSMSMessage::RefreshHeader() {
    FEngSetLanguageHash(GetPackageName(), 0xfeced617, FEngHashString("SMS_MESSAGE_%d_FROM", the_msg->GetHandle()));
    FEngSetLanguageHash(GetPackageName(), 0x2c167533, FEngHashString("SMS_MESSAGE_%d_SUBJECT", the_msg->GetHandle()));
}

eMenuSoundTriggers uiSMSMessage::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0x610fb237 && !the_msg->IsVoice()) {
        return static_cast< eMenuSoundTriggers >(-1);
    }
    return maybe;
}

void uiSMSMessage::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    m_TextScroller.HandleNotificationMessage(msg);
    switch (msg) {
    case 0x34dc1bcf:
        break;
    case 0x35f8620b: {
        if (the_msg->IsVoice() && the_msg->IsUnRead()) {
            MiscSpeed::SMSCellCall(the_msg->GetHandle());
        }
        the_msg->ClearFlags();
        the_msg->SetFlag(4);
        break;
    }
    case 0xe1fde1d1: {
        Module* cop_speech = Speech::Manager::GetSpeechModule(1);
        if (cop_speech != nullptr) {
            cop_speech->ReleaseResource();
            Speech::Manager::ClearPlayback();
        }
        cFEng::Get()->QueuePackagePop(1);
        break;
    }
    case 0xc519bfc3: {
        if (the_msg->IsVoice()) {
            Module* cop_speech = Speech::Manager::GetSpeechModule(1);
            if (cop_speech != nullptr) {
                cop_speech->ReleaseResource();
                Speech::Manager::ClearPlayback();
            }
            MiscSpeed::SMSCellCall(the_msg->GetHandle());
        }
        break;
    }
    case 0xc519bfc4: {
        DialogInterface::ShowTwoButtons(
            GetPackageName(), "InGameDialog.fng",
            static_cast< eDialogTitle >(1),
            0x70e01038, 0x417b25e4,
            0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
            static_cast< eDialogFirstButtons >(1),
            0x8c3c2171);
        break;
    }
    case 0xd05fc3a3: {
        the_msg->ClearFlags();
        cFEng::Get()->QueuePackageMessage(0x911ab364, GetPackageName(), nullptr);
        break;
    }
    }
}
