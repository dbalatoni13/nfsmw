#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
FEObject* FEngFindObject(const char* pkg_name, unsigned int obj_hash);
FEString* FEngFindString(const char* pkg_name, int name_hash);
FEImage* FEngFindImage(const char* pkg_name, int name_hash);
void FEngSetInvisible(FEObject* obj);
void FEngSetVisible(FEObject* obj);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
void FESetString(FEString* text, const short* string);
int FEPrintf(FEString* text, const char* fmt, ...);
int FEPrintf(const char* pkg_name, int object_hash, const char* fmt, ...);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
void FEngSetButtonState(const char* pkg_name, unsigned int button_hash, bool enabled);
unsigned long FEHashUpper(const char* str);
void FEngSetTextureHash(FEImage* img, unsigned int hash);

const char* GetLocalizedString(unsigned int hash);
void bStrCpy(unsigned short* to, const char* from);

extern bool IsMemcardEnabled;
extern GameFlowManager TheGameFlowManager;
extern unsigned int gMemcardSetupPreviousOp;

void MemcardExit(unsigned int msg);

extern "C" {
void ChangeToNextBootFlowScreen__15BootFlowManageri(void* self, int param);
void StartNewCareer__14CareerSettingsb(void* self, int bEnterGameplay);
void ResumeCareer__14CareerSettings(void* self);
int SetAudioModeFromMemoryCard__8EAXSound13eSndAudioMode(void* self, int mode);
void UpdateVolumes__8EAXSoundP13AudioSettingsf(void* self, void* settings, float vol);
void InitializeEATrax__Fb(int b);
void* Get__19uiRepSheetRivalFlow();
void Next__19uiRepSheetRivalFlow(void* self);
}

struct EAXSound;
extern EAXSound* g_pEAXSound;

// gButtonIDs and gButtonTextIDs are defined in uiMemcard.cpp (included before this file)

// ===== UIMemcardKeyboard =====

UIMemcardKeyboard::UIMemcardKeyboard(ScreenConstructorData* sd) : MenuScreen(sd) {
    m_pDisplayMsg = static_cast< FEString* >(FEngFindObject(GetPackageName(), 0x1e2640fa));
    const char* pkg = GetPackageName();
    unsigned int shadowHash = FEHashUpper("message_blurb_shadow");
    m_pDisplayMsgShadow = static_cast< FEString* >(FEngFindObject(pkg, shadowHash));
    m_pTitleMaster = static_cast< FEString* >(FEngFindObject(GetPackageName(), 0x426c7b4d));
    m_pOK = static_cast< FEString* >(FEngFindObject(GetPackageName(), gButtonIDs[0]));
    m_pCancel = static_cast< FEString* >(FEngFindObject(GetPackageName(), gButtonIDs[1]));
}

void UIMemcardKeyboard::Setup() {
    FEngSetScript(GetPackageName(), gButtonIDs[0], 0x5b0d9106, true);
    FEngSetScript(GetPackageName(), gButtonIDs[1], 0x5b0d9106, true);
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonIDs[0]));
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonIDs[1]));
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[1]);
}

void UIMemcardKeyboard::ShowKeyboard() {
    FEngSetScript(GetPackageName(), 0x47ff4e7c, 0x9e99, true);
    const char* title = GetLocalizedString(0x70513bd4);
    const char* prompt = GetLocalizedString(0xd48d95f);
    FEngBeginTextInput(0, 6, title, prompt, 7);
    FEDatabase->LoadSaveGame = static_cast< eLoadSaveGame >(5);
}

void UIMemcardKeyboard::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                             unsigned long param2) {
    if (msg == 0xC9D30688) {
        FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x03D8EABC, true);
    }
}

// ===== UIMemcardBase =====

UIMemcardBase::UIMemcardBase(ScreenConstructorData* sd)
    : UIMemcardKeyboard(sd) //
    , mIndex(1) //
{
    m_SimPausedForMemcard = false;
    m_ExpectingInput = false;
    m_LoadedNetConfig = 0;
    m_nMsgOptions = 0;
    m_bVisible = false;
    m_bDelayedFailed = false;
    m_bInButtonAnimation = false;
    m_pChild = nullptr;
}

UIMemcardBase::~UIMemcardBase() {
    m_pDisplayMsg = nullptr;
    MemoryCard::GetInstance()->m_pFEScreen = nullptr;
    if ((gMemcardSetup.mOp & 0x1000) != 0) {
        int savedLastMsg = gMemcardSetup.mLastMessage;
        if (gMemcardSetup.mTermFunc != nullptr) {
            gMemcardSetup.mTermFunc(gMemcardSetup.mTermFuncParam);
        }
        gMemcardSetup.mPreviousCommand = 0;
        gMemcardSetup.mOp = 0;
        gMemcardSetup.mLastMessage = savedLastMsg;
        gMemcardSetup.mMemScreen = nullptr;
        gMemcardSetup.mToScreen = nullptr;
        gMemcardSetup.mFromScreen = nullptr;
        gMemcardSetup.mTermFunc = nullptr;
        gMemcardSetup.mTermFuncParam = nullptr;
        gMemcardSetup.mSuccessMsg = 0;
        gMemcardSetup.mFailedMsg = 0;
        gMemcardSetup.mInBootFlow = false;
        gMemcardSetup.mPreviousPrompt = 0;
    }
}

void UIMemcardBase::Abort() {
    cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
}

void UIMemcardBase::DoSelect(const char* pFileName) {}

bool UIMemcardBase::AddItem(const char* pName, const char* pDate, int size, int flag) {
    Item* pItem = new Item();
    bStrNCpy(pItem->m_Name, pName, 0x1f);
    pItem->m_Name[31] = '\0';
    bStrCpy(pItem->m_Data, pDate);
    pItem->m_Size = size;
    pItem->m_Flag = static_cast< MemCardFileFlag >(flag);
    m_Items.AddTail(pItem);
    return true;
}

bool UIMemcardBase::IsProfile(const char* pName) {
    int len = bStrLen(pName);
    return len < 8;
}

void UIMemcardBase::EmptyFileList() {
    while (!m_Items.IsEmpty()) {
        Item* pItem = m_Items.GetHead();
        pItem->Remove();
        delete pItem;
    }
}

void UIMemcardBase::Setup() {
    FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x774e4dd9);
    FEngSetLanguageHash(m_pDisplayMsg, 0x99054304);
    MemoryCard::GetInstance()->FEngLinkObjects(this);
    SetIcon(0x6948e2b3);
}

void UIMemcardBase::SetStringCheckingCard() {
    SetScreenVisible(true, 0);
    SetMessageBlurbText(static_cast< unsigned int >(0x99054304));
    cFEng* pFeng = cFEng::Get();
    unsigned long hash = FEHashUpper("0_BUTTONS");
    pFeng->QueuePackageMessage(hash, GetPackageName(), nullptr);
    HideAllButtons();
    m_ExpectingInput = false;
}

void UIMemcardBase::HideAllButtons() {
    m_bAnyButtonVisible = false;
    for (int i = 0; i <= 2; i++) {
        ShowButton(i, false, nullptr);
    }
    FEngSetScript(GetPackageName(), 0x07f9dca9, 0x0016a259, true);
}

void UIMemcardBase::ShowButton(int idx, bool bShow, short* pText) {
    if (bShow) {
        m_bAnyButtonVisible = true;
        if (pText != nullptr) {
            FESetString(static_cast< FEString* >(
                FEngFindObject(GetPackageName(), gButtonTextIDs[idx])), pText);
        }
        FEngSetButtonState(GetPackageName(), gButtonIDs[idx], true);
        FEngSetVisible(FEngFindObject(GetPackageName(), gButtonIDs[idx]));
        FEngSetVisible(FEngFindObject(GetPackageName(), gButtonTextIDs[idx]));
        FEngSetScript(GetPackageName(), 0x57689fdd, 0xde6eff34, true);
    } else {
        FEngSetButtonState(GetPackageName(), gButtonIDs[idx], false);
        FEngSetInvisible(FEngFindObject(GetPackageName(), gButtonIDs[idx]));
        FEngSetInvisible(FEngFindObject(GetPackageName(), gButtonTextIDs[idx]));
    }
}

void UIMemcardBase::SetButtonText(short* b1, short* b2, short* b3) {
    if (b3 != nullptr) {
        m_nMsgOptions = 3;
        ShowButton(0, true, b1);
        ShowButton(1, true, b2);
        ShowButton(2, true, b3);
    } else if (b2 != nullptr) {
        m_nMsgOptions = 2;
        ShowButton(0, true, b1);
        ShowButton(1, true, b2);
        ShowButton(2, false, nullptr);
    } else if (b1 != nullptr) {
        m_nMsgOptions = 1;
        ShowButton(0, true, b1);
        ShowButton(1, false, nullptr);
        ShowButton(2, false, nullptr);
    }
    int active = 0;
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[active]);
    m_ExpectingInput = true;
    gMemcardSetup.mPreviousPrompt = gMemcardSetup.mOp & 0xf000000;
    gMemcardSetup.mOp = gMemcardSetup.mOp & 0xf0ffffff;
}

void UIMemcardBase::SetMessage(short* pMsg) {
    if (pMsg == nullptr) {
        SetMessageBlurbText(const_cast< char* >(""));
        HideAllButtons();
    } else {
        SetMessageBlurbText(pMsg);
        m_pDisplayMsg->Flags |= 2;
        FEngSetScript(GetPackageName(), 0x47ff4e7c, 0xe18da018, true);
    }
}

void UIMemcardBase::SetMessageBlurbText(short* pText) {
    FESetString(m_pDisplayMsg, pText);
    if (m_pDisplayMsgShadow != nullptr) {
        FESetString(m_pDisplayMsgShadow, pText);
    }
    FindScreenSize(reinterpret_cast< const wchar_t* >(pText));
}

void UIMemcardBase::SetMessageBlurbText(char* pText) {
    int wText[1024];
    FEPrintf(m_pDisplayMsg, pText);
    if (m_pDisplayMsgShadow != nullptr) {
        FEPrintf(m_pDisplayMsgShadow, pText);
    }
    bStrCpy(reinterpret_cast< unsigned short* >(wText), pText);
    FindScreenSize(reinterpret_cast< const wchar_t* >(wText));
}

void UIMemcardBase::SetMessageBlurbText(unsigned int textHash) {
    FEngSetLanguageHash(m_pDisplayMsg, textHash);
    if (m_pDisplayMsgShadow != nullptr) {
        FEngSetLanguageHash(m_pDisplayMsgShadow, textHash);
    }
    const char* str = GetLocalizedString(textHash);
    unsigned short buf[2048];
    bStrCpy(buf, str);
    FindScreenSize(reinterpret_cast< const wchar_t* >(buf));
}

void UIMemcardBase::ShowOK(unsigned int textHash, unsigned int flag) {
    cFEng* pFeng = cFEng::Get();
    unsigned long msg = FEHashUpper("HIDE LOADER");
    pFeng->QueuePackageMessage(msg, GetPackageName(), nullptr);
    SetMessageBlurbText(textHash);
    gMemcardSetup.mOp = gMemcardSetup.mOp | static_cast< int >(flag & 0xf000000);
    ShowButton(0, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[0], 0x417b2601);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    ShowButton(1, false, nullptr);
    ShowButton(2, false, nullptr);
    m_ExpectingInput = true;
    SetScreenVisible(true, 1);
}

void UIMemcardBase::ShowYesNo(unsigned int textHash, unsigned int flag) {
    cFEng* pFeng = cFEng::Get();
    unsigned long msg = FEHashUpper("HIDE LOADER");
    pFeng->QueuePackageMessage(msg, GetPackageName(), nullptr);
    SetMessageBlurbText(textHash);
    gMemcardSetup.mOp = gMemcardSetup.mOp | static_cast< int >(flag & 0xf000000);
    ShowButton(0, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[0], 0x417b25e4);
    ShowButton(1, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[1], 0x70e01038);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    ShowButton(2, false, nullptr);
    m_ExpectingInput = true;
    SetScreenVisible(true, 2);
}

void UIMemcardBase::SetScreenVisible(bool bVisible, int nButtons) {
    if (m_bVisible != bVisible) {
        cFEng* pFeng = cFEng::Get();
        m_bVisible = bVisible;
        unsigned long msg = bVisible ? 0xc0f2ae7cUL : 0x4f3559b5UL;
        pFeng->QueuePackageMessage(msg, GetPackageName(), nullptr);
        if (bVisible) {
            pFeng = cFEng::Get();
            unsigned long resetMsg = FEHashUpper("INITIALIZE_SCREEN");
            pFeng->QueuePackageMessage(resetMsg, GetPackageName(), nullptr);
        }
        MemoryCard::GetInstance()->m_bHUDLoaded = m_bVisible;
    }
    if (bVisible) {
        char buf[36];
        bSPrintf(buf, "%d_BUTTONS", nButtons);
        cFEng* pFeng = cFEng::Get();
        unsigned long hash = FEHashUpper(buf);
        pFeng->QueuePackageMessage(hash, GetPackageName(), nullptr);
    }
}

void UIMemcardBase::SetIcon(unsigned int iconHash) {
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xd4f4069), iconHash);
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xfac88427), iconHash);
}

void UIMemcardBase::TranslateButton(FEObject* obj) {
    if (obj->Flags & 1) {
        return;
    }
    unsigned long nameHash = obj->NameHash;
    if (nameHash == gButtonIDs[0]) {
        MemoryCard::GetInstance()->MessageDone(static_cast< RealmcIface::MessageChoices >(1));
    } else if (nameHash == gButtonIDs[1]) {
        MemoryCard::GetInstance()->MessageDone(static_cast< RealmcIface::MessageChoices >(2));
    } else if (nameHash == gButtonIDs[2]) {
        MemoryCard::GetInstance()->MessageDone(static_cast< RealmcIface::MessageChoices >(3));
    }
    m_ExpectingInput = false;
}

void UIMemcardBase::SetupPromptNoProfileFound() {
    ShowOK(0xba373453, 0x3000000);
}

void UIMemcardBase::SetupPromptSaveConfirm() {
    unsigned int textHash;
    if ((gMemcardSetup.mOp & 0x8000) != 0) {
        textHash = 0x391a0aac;
    } else if ((gMemcardSetup.mOp & 0x40000) != 0) {
        textHash = 0xb0af33a5;
    } else if ((gMemcardSetup.mOp & 0x200000) != 0) {
        textHash = 0xd80818f8;
    } else {
        textHash = 0x39b3ccba;
    }
    const char* localStr = GetLocalizedString(textHash);
    ShowYesNo(0x39b3ccba, 0x4000000);
    char buf[512];
    bSPrintf(buf, localStr, m_FileName, m_FileName);
    SetMessageBlurbText(buf);
}

void UIMemcardBase::SetupAutoSaveConfirmPrompt() {
    gMemcardSetup.mOp = gMemcardSetup.mOp | 0xa000000;
    const char* mainText = GetLocalizedString(0xa0b434a2);
    SetMessageBlurbText(const_cast< char* >(mainText));
    FEngSetButtonState(GetPackageName(), gButtonIDs[0], true);
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonIDs[0]));
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonTextIDs[0]));
    const char* yesStr = GetLocalizedString(0x417b25e4);
    FEPrintf(GetPackageName(), static_cast< int >(gButtonTextIDs[0]), yesStr);
    FEngSetButtonState(GetPackageName(), gButtonIDs[1], true);
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonIDs[1]));
    FEngSetVisible(FEngFindObject(GetPackageName(), gButtonTextIDs[1]));
    const char* noStr = GetLocalizedString(0x2b07a03d);
    FEPrintf(GetPackageName(), static_cast< int >(gButtonTextIDs[1]), noStr);
    FEngSetButtonState(GetPackageName(), gButtonIDs[2], false);
    FEngSetInvisible(FEngFindObject(GetPackageName(), gButtonIDs[2]));
    FEngSetInvisible(FEngFindObject(GetPackageName(), gButtonTextIDs[2]));
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    unsigned long handlerHash = FEHashUpper("HANDLER");
    unsigned long forwardHash = FEHashUpper("FORWARD");
    FEngSetScript(GetPackageName(), handlerHash, forwardHash, true);
    SetScreenVisible(true, 2);
}

void UIMemcardBase::SetupPromptForSave() {
    ShowYesNo(0x83f4bb3e, 0x4000000);
    unsigned int textHash = 0x83f4bb3e;
    if ((gMemcardSetup.mOp & 0x200000) != 0) {
        textHash = 0xd80818f8;
    }
    const char* localStr = GetLocalizedString(textHash);
    char buf[512];
    bSPrintf(buf, localStr, m_FileName, m_FileName);
    SetMessageBlurbText(buf);
}

void UIMemcardBase::SetupPromptCorruptProfile() {
    ShowOK(0x821e4444, 0xd000000);
    const char* localStr = GetLocalizedString(0x821e4444);
    char buf[512];
    bSPrintf(buf, localStr, m_FileName);
    SetMessageBlurbText(buf);
}

void UIMemcardBase::SetupPromptAutoSaveEnableFailedNoCard() {
    ShowOK(0x9e85bba8, 0xb000000);
}

void UIMemcardBase::ShowKeyboard() {
    SetScreenVisible(false, 0);
    HideAllButtons();
    UIMemcardKeyboard::ShowKeyboard();
}

void UIMemcardBase::FindScreenSize(const wchar_t* msg) {
    cFEng::Get()->QueuePackageMessage(0x79b0c1c7, GetPackageName(), nullptr);
}

void UIMemcardBase::ShowMessage(MemoryCardMessage* msg) {
    ShowMessage(reinterpret_cast< const wchar_t* >(msg->mMsg), msg->mnOptions,
                reinterpret_cast< const wchar_t* >(msg->mOptions[0]),
                reinterpret_cast< const wchar_t* >(msg->mOptions[1]),
                reinterpret_cast< const wchar_t* >(msg->mOptions[2]));
    MemoryCard::GetInstance()->ReleasePendingMessage();
}

void UIMemcardBase::ShowMessage(const wchar_t* msg, unsigned int nOptions,
                                const wchar_t* option1, const wchar_t* option2,
                                const wchar_t* option3) {
    PopChild();
    HideAllButtons();
    SetMessage(reinterpret_cast< short* >(const_cast< wchar_t* >(msg)));
    switch (nOptions) {
    case 2:
        SetButtonText(reinterpret_cast< short* >(const_cast< wchar_t* >(option1)),
                      reinterpret_cast< short* >(const_cast< wchar_t* >(option2)),
                      nullptr);
        break;
    case 3:
        SetButtonText(reinterpret_cast< short* >(const_cast< wchar_t* >(option1)),
                      reinterpret_cast< short* >(const_cast< wchar_t* >(option2)),
                      reinterpret_cast< short* >(const_cast< wchar_t* >(option3)));
        break;
    case 1:
        SetButtonText(reinterpret_cast< short* >(const_cast< wchar_t* >(option1)),
                      nullptr, nullptr);
        break;
    case 0:
    default:
        MemoryCard::GetInstance()->SetWaitingForResponse(false);
        break;
    }
    SetScreenVisible(true, nOptions);
    cFEng* pFEng = cFEng::Get();
    if (nOptions == 0) {
        pFEng->QueuePackageMessage(FEHashUpper("SHOW LOADER"), GetPackageName(), nullptr);
    } else {
        pFEng->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
    }
}

void UIMemcardBase::ActivateChild() {
    MemoryCard::GetInstance()->SetMonitor(true);
}

void UIMemcardBase::PopChild() {
    if (m_pChild != nullptr && cFEng::Get()->IsPackagePushed("MC_List.fng")) {
        cFEng::Get()->QueuePackagePop(1);
    }
    m_pChild = nullptr;
}

void UIMemcardBase::HandleAutoSaveError() {
    if (!MemoryCard::GetInstance()->IsCheckingCardForAutoSave() &&
        !MemoryCard::GetInstance()->IsCheckingCardForOverwrite()) {
        if ((gMemcardSetup.mOp & 0xf0) != 0xb0) {
            gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf) | 1;
        }
        gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
        gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf0) | 0x50;
    }
    char* dst = m_FileName;
    const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
    bStrCpy(dst, profileName);
    if (MemoryCard::GetInstance()->IsCheckingCardForAutoSave() ||
        MemoryCard::GetInstance()->IsCheckingCardForOverwrite() ||
        MemoryCard::GetInstance()->WasCardRemovedWithAutoSaveEnabled()) {
        MemoryCard::GetInstance()->ReleasePendingMessage();
        SetupAutoSaveConfirmPrompt();
        MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
    } else {
        MemoryCard::GetInstance()->SetRetryAutoSave(true);
        ShowMessage(MemoryCard::GetInstance()->GetPendingMessage());
    }
    MemoryCard::GetInstance()->EndAutoSave();
}

void UIMemcardBase::HandleAutoSaveOverwriteMessage() {
    char* dst = m_FileName;
    const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
    bStrCpy(dst, profileName);
    MemoryCard::GetInstance()->EndAutoSave();
    FEDatabase->bAutoSaveOverwriteConfirmed = true;
    gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
    gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf0) | 0x50;
    MemoryCard::GetInstance()->ShowMessages(true);
    DoSaveFlow(12);
}

void UIMemcardBase::DoSaveFlow(int flow) {
    m_Flow = flow;
    if (flow == 6) {
        SetupPromptSaveConfirm();
    } else if (flow == 2) {
        unsigned int textHash;
        if ((gMemcardSetup.mOp & 0x80000) != 0) {
            textHash = 0xbadd522c;
        } else if ((gMemcardSetup.mOp & 0x10000) != 0) {
            textHash = 0x93c25b3d;
        } else if ((gMemcardSetup.mOp & 0x8000) != 0) {
            textHash = 0xf8448956;
        } else if ((gMemcardSetup.mOp & 0x200000) != 0) {
            textHash = 0xd80818f8;
        } else {
            textHash = 0xbe97590f;
        }
        ShowYesNo(textHash, 0x1000000);
    } else if (flow == 1) {
        ShowYesNo(0x7209349f, 0x5000000);
    } else if (flow == 4) {
        SetupPromptForSave();
    } else if (flow == 8) {
        FEDatabase->CurrentUserProfiles[0]->SetProfileName(m_FileName, true);
        MemoryCard::GetInstance()->Save(m_FileName);
        SetStringCheckingCard();
    } else if (flow == 12) {
        MemoryCard::GetInstance()->SetAutoSaveEnabled(false);
    }
}

eMenuSoundTriggers UIMemcardBase::NotifySoundMessage(unsigned long msg,
                                                      eMenuSoundTriggers maybe) {
    if (m_bAnyButtonVisible) {
        return maybe;
    }
    if (msg == 0x48122792 || msg == 0x4ac5e165) {
        return UISND_NONE;
    }
    return maybe;
}

void UIMemcardBase::InitCompleteDoList() {
    while (!m_Items.IsEmpty()) {
        Item* pItem = m_Items.GetHead();
        pItem->Remove();
        delete pItem;
    }
    SetStringCheckingCard();
    MemoryCard::GetInstance()->RequestTask(7, nullptr);
    cFEng* pFeng = cFEng::Get();
    unsigned long hash = FEHashUpper("SHOW LOADER");
    pFeng->QueuePackageMessage(hash, GetPackageName(), nullptr);
}

void UIMemcardBase::InitComplete() {
    if (!IsMemcardEnabled) {
        cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
        return;
    }
    SetMessageBlurbText(const_cast< char* >(" "));
    unsigned long btnHash = FEHashUpper("Button");
    FEngSetInvisible(FEngFindObject(GetPackageName(), btnHash));
    m_pDisplayMsg->Flags |= 0x80;
    if ((gMemcardSetup.mOp & 0x4000) != 0) {
        cFEng::Get()->QueueGameMessage(0x5afe12f4, gMemcardSetup.mToScreen, 0xff);
    }
    if ((gMemcardSetup.mOp & 0x400000) != 0 ||
        ((gMemcardSetup.mOp & 0x10000) != 0 && (gMemcardSetup.mOp & 0xf0) == 0xb0)) {
        unsigned long memcardOnHash = FEHashUpper("MEMCARD_ON");
        cFEng::Get()->QueuePackageMessage(memcardOnHash, GetPackageName(), nullptr);
    }
    unsigned int uiOp = gMemcardSetup.mOp & 0xf0;
    if (uiOp == 0x10 || uiOp == 0x70) {
        if (FEDatabase->bProfileLoaded && (gMemcardSetup.mOp & 0x20000) == 0) {
            ShowYesNo(0x87c7577e, 0x6000000);
            return;
        }
        InitCompleteDoList();
    } else if (uiOp == 0x20) {
        MemcardExit(0x8867412d);
    } else if (uiOp == 0x30) {
        SetStringCheckingCard();
        InitCompleteDoList();
    } else if (uiOp == 0x40) {
        cFEng::Get()->QueueGameMessage(0x5a051729, nullptr, 0xff);
    } else if (uiOp == 0x50) {
        const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
        bStrCpy(m_FileName, profileName);
        DoSaveFlow(6);
    } else if (uiOp == 0x60) {
        cFEng::Get()->QueueGameMessage(0x5a051729, nullptr, 0xff);
    } else if (uiOp == 0x80) {
        MemoryCard::GetInstance()->CheckCard(0);
    } else if (uiOp == 0x90) {
        m_SimPausedForMemcard = true;
        HandleAutoSaveError();
    } else if (uiOp == 0xa0) {
        if ((gMemcardSetup.mOp & 0x8000) != 0) {
            MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
            return;
        }
        SetStringCheckingCard();
        ShowYesNo(0x750eb45c, 0xc000000);
    } else if (uiOp == 0xb0) {
        if (!FEDatabase->bProfileLoaded) {
            gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
            gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf0) | 0x60;
            InitComplete();
            return;
        }
        if (MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
            SetScreenVisible(true, 0);
            SetStringCheckingCard();
            const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
            bStrCpy(m_FileName, profileName);
            MemoryCard::GetInstance()->StartAutoSave(true);
            return;
        }
        gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
        gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf0) | 0x50;
        InitComplete();
    } else if (uiOp == 0xd0) {
        m_SimPausedForMemcard = true;
        HandleAutoSaveOverwriteMessage();
    } else if (uiOp == 0xf0) {
        if (!MemoryCard::IsCardAvailable() || !IsMemcardEnabled) {
            MemcardExit(0x8867412d);
            return;
        }
        InitCompleteDoList();
    }
}

void UIMemcardBase::ExitComplete() {
    int lastMsg = gMemcardSetup.mLastMessage;
    if ((gMemcardSetup.mOp & 0x100) != 0) {
        cFEng::Get()->QueuePackageMessage(lastMsg, gMemcardSetup.mToScreen, nullptr);
    }
    if ((gMemcardSetup.mOp & 0x400) != 0) {
        unsigned int gameMsg;
        if (lastMsg == 0x461a18ee) {
            gameMsg = gMemcardSetup.mSuccessMsg;
        } else {
            gameMsg = gMemcardSetup.mFailedMsg;
        }
        cFEng::Get()->QueueGameMessage(gameMsg, gMemcardSetup.mToScreen, 0xff);
    }
    if (FEDatabase->MatchesGameMode(0x100) &&
        TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        FEDatabase->ResetGameMode();
        if (!FEDatabase->bProfileLoaded ||
            ((gMemcardSetup.mOp & 0xf0) == 0x10 &&
             static_cast< unsigned int >(lastMsg) == 0x8867412d) ||
            gMemcardSetup.mPreviousPrompt == 0x1000000 ||
            gMemcardSetup.mPreviousPrompt == 0x3000000 ||
            gMemcardSetup.mPreviousPrompt == 0x5000000) {
            gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf) | 1;
            FEDatabase->RestoreFromBackupDB();
            FEDatabase->SetGameMode(static_cast< eFEGameModes >(0x100));
        } else if (!(FEDatabase->CurrentUserProfiles[0]->GetCareer()->SpecialFlags & 1)) {
            StartNewCareer__14CareerSettingsb(FEDatabase->CurrentUserProfiles[0]->GetCareer(), 1);
        } else {
            ResumeCareer__14CareerSettings(FEDatabase->CurrentUserProfiles[0]->GetCareer());
        }
    }

    if ((gMemcardSetup.mOp & 0x400000) == 0) {
        if ((gMemcardSetup.mOp & 0x10000) == 0) {
            unsigned int cmd = gMemcardSetup.mOp & 0xf;
            if (cmd == 2) {
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen,
                    MemoryCard::GetInstance()->GetPlayerNum(), 0, false);
            } else if (cmd == 1) {
                bool popExtra;
                if (!m_SimPausedForMemcard) {
                    popExtra = true;
                } else {
                    m_SimPausedForMemcard = false;
                    popExtra = cFEng::Get()->IsPackagePushed("SMS_Mailboxes.fng");
                }
                cFEng::Get()->QueuePackagePop(popExtra ? 1 : 0);
            } else if (cmd == 3) {
                cFEng::Get()->QueuePackagePop(1);
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen,
                    MemoryCard::GetInstance()->GetPlayerNum(), 0, false);
            }
        } else if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
            cFEng::Get()->QueuePackagePop(1);
            if (FEDatabase->bProfileLoaded) {
                FEDatabase->ResetGameMode();
                FEDatabase->SetGameMode(static_cast< eFEGameModes >(2));
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen, 0, 0, false);
            }
        }
    } else {
        void* rivalFlow = Get__19uiRepSheetRivalFlow();
        Next__19uiRepSheetRivalFlow(rivalFlow);
    }
    if (m_SimPausedForMemcard) {
        m_SimPausedForMemcard = false;
    }

    int audioMode = FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode;
    int newMode = SetAudioModeFromMemoryCard__8EAXSound13eSndAudioMode(g_pEAXSound, audioMode);
    FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode = newMode;
    UpdateVolumes__8EAXSoundP13AudioSettingsf(
        g_pEAXSound, &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings, 1.0f);
    InitializeEATrax__Fb(1);

    FEPackage* pkg = cFEng::Get()->FindPackage(gMemcardSetup.mMemScreen);
    if (pkg != nullptr && pkg->pParentPackage != nullptr) {
        pkg->pParentPackage->bInputEnabled = true;
    }

    if (gMemcardSetup.mTermFunc != nullptr) {
        gMemcardSetup.mTermFunc(gMemcardSetup.mTermFuncParam);
    }
    int savedMsg = gMemcardSetup.mLastMessage;
    gMemcardSetup.mOp = 0;
    gMemcardSetup.mMemScreen = nullptr;
    gMemcardSetup.mToScreen = nullptr;
    gMemcardSetup.mFromScreen = nullptr;
    gMemcardSetup.mTermFunc = nullptr;
    gMemcardSetup.mTermFuncParam = nullptr;
    gMemcardSetup.mSuccessMsg = 0;
    gMemcardSetup.mFailedMsg = 0;
    gMemcardSetup.mInBootFlow = false;
    gMemcardSetup.mPreviousCommand = 0;
    gMemcardSetup.mPreviousPrompt = 0;
    gMemcardSetup.mLastMessage = savedMsg;

    if (MemoryCard::GetInstance()->InBootSequence()) {
        ChangeToNextBootFlowScreen__15BootFlowManageri(BootFlowManager::Get(), 0xff);
        MemoryCard::GetInstance()->EndBootSequence();
    }
    cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
    MemoryCard::GetInstance()->SetMemcardScreenExiting(false);
    MemoryCard::GetInstance()->SetMemcardScreenShowing(false);
    if (MemoryCard::GetInstance()->IsMonitorOn()) {
        MemoryCard::GetInstance()->SetMonitor(false);
    }
}

void UIMemcardBase::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                         unsigned long param2) {
    if (msg != 0xc407210 && MemoryCard::GetInstance()->GetOp() == 0) {
        UIMemcardKeyboard::NotificationMessage(msg, obj, param1, param2);
    }
    if (msg == 0xc502df5d) {
        m_bInButtonAnimation = true;
        TranslateButton(reinterpret_cast< FEObject* >(param1));
    } else if (msg == 0x35f8620b || msg == 0x3a2be557) {
        InitComplete();
    } else if (msg == 0xc407210) {
        m_bInButtonAnimation = false;
        gMemcardSetup.mLastController = param2;
        HandleButtonPressed(0xc407210, obj, param1, param2, false);
    } else if (msg == 0x54b3ac6c) {
        SetScreenVisible(false, 0);
        cFEng::Get()->QueuePackagePush("MC_List.fng", 0, 0, false);
    } else if (msg == 0xda5b8712) {
        const char* editStr = FEngGetEditedString();
        bStrCpy(m_FileName, editStr);
        FEDatabase->CurrentUserProfiles[0]->SetProfileName(m_FileName, true);
        FEDatabase->DeallocBackupDB();
        FEDatabase->bProfileLoaded = true;
        DoSaveFlow(4);
    } else if (msg == 0xc98356ba) {
        if (m_bDelayedFailed) {
            m_bDelayedFailed = false;
            cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
        }
    } else if (msg == 0xc9d30688) {
        if ((gMemcardSetup.mOp & 0xf0) == 0x60 && !FEDatabase->bProfileLoaded) {
            DoSaveFlow(2);
        } else if ((gMemcardSetup.mOp & 0x60) == 0 || !FEDatabase->bProfileLoaded) {
            FEPrintf(m_pDisplayMsg, "");
            m_bDelayedFailed = true;
        } else {
            DoSaveFlow(1);
        }
    } else if (msg == 0xe1fde1d1) {
        ExitComplete();
    } else if (msg == 0xf35d144e) {
        SetupPromptCorruptProfile();
    }
}

void UIMemcardBase::HandleButtonPressed(unsigned long msg, FEObject* obj, unsigned long param1,
                                         unsigned long param2, bool bPadBack) {
    FEObject* btnObj = reinterpret_cast< FEObject* >(param1);
    bool isSecondBtn = btnObj->NameHash == gButtonIDs[1];
    int promptFlags = gMemcardSetup.mOp & 0xf000000;
    gMemcardSetup.mOp = gMemcardSetup.mOp & 0xf0ffffff;
    gMemcardSetup.mPreviousPrompt = promptFlags;
    HideAllButtons();

    if (promptFlags == 0x7000000) {
        cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
    } else if (promptFlags == 0x1000000) {
        if (isSecondBtn && !bPadBack) {
            FEDatabase->AllocBackupDB(true);
            if ((gMemcardSetup.mOp & 0x40000) == 0 && (gMemcardSetup.mOp & 0x200000) == 0) {
                FEDatabase->DefaultProfile();
            }
            if ((gMemcardSetup.mOp & 0x80000) != 0) {
                StartNewCareer__14CareerSettingsb(
                    FEDatabase->CurrentUserProfiles[0]->GetCareer(), 0);
            }
            if ((gMemcardSetup.mOp & 0xf0) == 0x20) {
                gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf0) | 0x60;
                gMemcardSetup.mPreviousCommand = 0x20;
            }
        } else {
            if ((gMemcardSetup.mOp & 0x80000) != 0) {
                FEDatabase->RestoreFromBackupDB();
            }
            cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
        }
    } else if (promptFlags == 0x3000000 || promptFlags == 0xd000000) {
        cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
    } else if (promptFlags == 0x4000000) {
        if (isSecondBtn && !bPadBack) {
            DoSaveFlow(12);
        } else {
            if ((gMemcardSetup.mOp & 0xf0) == 0x60) {
                FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode = 0;
            }
            cFEng::Get()->QueueGameMessage(0xdc12af2e, GetPackageName(), 0xff);
        }
    } else if (promptFlags == 0x5000000) {
        if (isSecondBtn && !bPadBack) {
            FEDatabase->AllocBackupDB(true);
            if ((gMemcardSetup.mOp & 0x40000) == 0 && (gMemcardSetup.mOp & 0x200000) == 0) {
                FEDatabase->DefaultProfile();
            }
        } else {
            MemcardExit(0x8867412d);
        }
    } else if (promptFlags == 0x6000000) {
        if (isSecondBtn && !bPadBack) {
            InitCompleteDoList();
        } else {
            cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
        }
    } else if (promptFlags == 0x9000000) {
        DoSaveFlow(3);
    } else if (promptFlags == 0xa000000) {
        if (isSecondBtn && !bPadBack) {
            FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode = 0;
            cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
        } else {
            MemoryCard::GetInstance()->SetRetryAutoSave(true);
            FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode = 1;
            gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
            gMemcardSetup.mOp = gMemcardSetup.mOp & ~0xf0;
            MemoryCard::GetInstance()->ShowMessages(true);
            gMemcardSetup.mOp = gMemcardSetup.mOp | 0x50;
            DoSaveFlow(12);
        }
    } else if (promptFlags == 0xb000000) {
        if ((gMemcardSetup.mOp & 0xf0) == 0xa0 && (gMemcardSetup.mOp & 0x8000) == 0) {
            gMemcardSetup.mOp = (gMemcardSetup.mOp & ~0xf) | 1;
        }
        cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
        cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
    } else if (promptFlags == 0xc000000) {
        if (isSecondBtn && !bPadBack) {
            MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
        } else {
            FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings.AudioMode = 0;
            cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        }
    } else {
        SetStringCheckingCard();
        if (MemoryCard::GetInstance()->GetPendingMessage() != nullptr) {
            ShowMessage(MemoryCard::GetInstance()->GetPendingMessage());
        }
        if (MemoryCard::GetInstance()->GetOp() == 7) {
            unsigned long hash = FEHashUpper("SHOW LOADER");
            cFEng::Get()->QueuePackageMessage(hash, GetPackageName(), nullptr);
        }
    }
}

void UIMemcardBase::NotificationMessageGoThroughAll(unsigned long msg, FEObject* obj,
                                                     unsigned long param1, unsigned long param2) {
    NotificationMessage(msg, obj, param1, param2);
}

void UIMemcardBase::SetupPromptSaveCorrupt() {}
void UIMemcardBase::SetupPromptOverwrite() {}
void UIMemcardBase::SetupPromptDelete() {}
void UIMemcardBase::SetupPromptLoadingCorrupt() {}
void UIMemcardBase::SetupPromptFormatCard() {}
void UIMemcardBase::SetupPromptAutoSaveEnable() {}
void UIMemcardBase::SetupPromptAutoSaveDisable() {}
void UIMemcardBase::SetupPromptOverwriteNoSaves() {}
void UIMemcardBase::SetupPromptAutoSaveEnableFailed() {}
int UIMemcardBase::BuildDeleteList(const char* pName, const char** pList) { return 0; }
UIMemcardBase::Item* UIMemcardBase::FindItem(const char* pName) { return nullptr; }

unsigned int UIMemcardBase::GetAutoSaveWarning() {
    return 0xb39899c2;
}

unsigned int UIMemcardBase::GetAutoSaveWarning2() {
    return 0x2386f454;
}
