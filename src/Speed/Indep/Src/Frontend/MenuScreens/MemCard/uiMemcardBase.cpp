#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "types.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"

static uint32 gButtonIDs[3] = {0xb8a7c6cc, 0xb8a7c6cd, 0xb8a7c6ce};
static uint32 gButtonTextIDs[3] = {0xf9363f30, 0xfb8b67d1, 0xfde09072};

// ===== UIMemcardKeyboard =====

// UNSOLVED
UIMemcardKeyboard::UIMemcardKeyboard(ScreenConstructorData *sd) : MenuScreen(sd) {
    const u32 FEObj_MESSAGE_BLURB = 0x1e2640fa;
    const u32 FEObj_HEADER_TEXT_02 = 0x426c7b4d;
    m_pDisplayMsg = static_cast<FEString *>(FEngFindObject(GetPackageName(), FEObj_MESSAGE_BLURB));
    m_pDisplayMsgShadow = static_cast<FEString *>(FEngFindObject(GetPackageName(), FEHashUpper("message_blurb_shadow")));
    m_pTitleMaster = static_cast<FEString *>(FEngFindObject(GetPackageName(), FEObj_HEADER_TEXT_02));
    m_pOK = static_cast<FEString *>(FEngFindObject(GetPackageName(), gButtonIDs[0]));
    m_pCancel = static_cast<FEString *>(FEngFindObject(GetPackageName(), gButtonIDs[1]));
}

// UNSOLVED
void UIMemcardKeyboard::Setup() {
    const uint32 Hahs_FADEIN = 0x5b0d9106;

    FEngSetScript(GetPackageName(), gButtonIDs[0], Hahs_FADEIN, true);
    FEngSetScript(GetPackageName(), gButtonIDs[1], Hahs_FADEIN, true);
    FEngSetVisible(GetPackageName(), gButtonIDs[0]);
    FEngSetVisible(GetPackageName(), gButtonIDs[1]);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[1]);
}

void UIMemcardKeyboard::ShowKeyboard() {
    const u32 FEObj_DIM = FEHASH_DIM;
    const u32 FEObj_SHOW = FEHASH_SHOW;
    const u32 FEObj_PC_NAME_ENTRY = FEOBJ_EVENT_HANDLER;
    const u32 FEObjWidget = 0;

    FEngSetScript(GetPackageName(), FEObj_PC_NAME_ENTRY, FEObj_DIM, true);
    FEngBeginTextInput(0, 6, GetLocalizedString(0x70513bd4), GetLocalizedString(0xd48d95f), 7);
    FEDatabase->mFEKeyboardSettings.Mode = 5;
}

void UIMemcardKeyboard::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    const u32 FEObj_UNDIM = FEHASH_UNDIM;
    const u32 FEObj_HIDE = FEHASH_HIDE;
    const u32 FEObj_PC_NAME_ENTRY = FEOBJ_EVENT_HANDLER;
    if (msg == FEMSG_DECLINE_EDITED_TEXT) {
        FEngSetScript(GetPackageName(), FEObj_PC_NAME_ENTRY, FEObj_UNDIM, true);
    }
}

// ===== UIMemcardBase =====

UIMemcardBase::UIMemcardBase(ScreenConstructorData *sd)
    : UIMemcardKeyboard(sd),       //
      mIndex(1),                   //
      m_ExpectingInput(false),     //
      m_LoadedNetConfig(0),        //
      m_nMsgOptions(0),            //
      m_bVisible(false),           //
      m_bDelayedFailed(false),     //
      m_bInButtonAnimation(false), //
      m_pChild(nullptr),           //
      m_SimPausedForMemcard(false) {}

UIMemcardBase::~UIMemcardBase() {
    m_pDisplayMsg = nullptr;
    MemoryCard::GetInstance()->FEngLinkObjects(nullptr);
    if ((gMemcardSetup.GetExtraOptions() & 0x1000) != 0) {
        gMemcardSetup.Complete(gMemcardSetup.mLastMessage);
    }
}

void UIMemcardBase::Abort() {
    cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
}

bool UIMemcardBase::AddItem(const char *pName, const char *pDate, int size, int flag) {
    Item *pNode = new ("FEPkgMemcardFileItem", 0) Item();
    bStrNCpy(pNode->m_Name, pName, 0x1f);
    pNode->m_Name[31] = '\0';
    bStrCpy(pNode->m_Data, pDate);
    pNode->m_Size = size;
    pNode->m_Flag = static_cast<MemCardFileFlag>(flag);
    m_Items.AddTail(pNode);
    return true;
}

bool UIMemcardBase::IsProfile(const char *pName) {
    return bStrLen(pName) < 8;
}

void UIMemcardBase::EmptyFileList() {
    m_Items.DeleteAllElements();
}

void UIMemcardBase::InitCompleteDoList() {
    m_Items.DeleteAllElements();
    SetStringCheckingCard();
    MemoryCard::GetInstance()->RequestTask(7, nullptr);
    cFEng::Get()->QueuePackageMessage(FEHashUpper("SHOW LOADER"), GetPackageName(), nullptr);
}

void UIMemcardBase::InitComplete() {
    if (!IsMemcardEnabled) {
        cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
        return;
    }
    SetMessageBlurbText(const_cast<char *>(" "));
    FEngSetInvisible(GetPackageName(), FEHashUpper("Button"));
    m_pDisplayMsg->Flags |= 0x80;
    if ((gMemcardSetup.GetExtraOptions() & 0x4000) != 0) {
        cFEng::Get()->QueueGameMessage(0x5afe12f4, gMemcardSetup.mFromScreen, 0xff);
    }
    if ((gMemcardSetup.GetExtraOptions() & 0x400000) != 0 ||
        ((gMemcardSetup.GetExtraOptions() & 0x10000) != 0 && gMemcardSetup.GetCommand() == 0xb0)) {
        cFEng::Get()->QueuePackageMessage(FEHashUpper("MEMCARD_ON"), GetPackageName(), nullptr);
    }
    switch (MemcardGetCurrentUIOperation()) {
        case 0x10:
        case 0x70:
            if (FEDatabase->bProfileLoaded && (gMemcardSetup.GetExtraOptions() & 0x20000) == 0) {
                ShowYesNo(0x87c7577e, 0x6000000);
                return;
            }
            InitCompleteDoList();
            break;
        case 0x20:
            MemcardExit(0x8867412d);
            break;
        case 0x30:
            SetStringCheckingCard();
            InitCompleteDoList();
            break;
        case 0x40:
        case 0x60:
            cFEng::Get()->QueueGameMessage(0x5a051729, nullptr, 0xff);
            break;
        case 0x50: {
            bStrCpy(m_FileName, FEDatabase->GetUserProfile(0)->GetProfileName());
            DoSaveFlow(6);
            break;
        }
        case 0x80:
            MemoryCard::GetInstance()->CheckCard(0);
            break;
        case 0xa0:
            if ((gMemcardSetup.mOp & 0x8000) != 0) {
                MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
                return;
            }
            SetStringCheckingCard();
            ShowYesNo(0x750eb45c, 0xc000000);
            break;
        case 0x90:
            m_SimPausedForMemcard = true;
            HandleAutoSaveError();
            break;
        case 0xd0:
            m_SimPausedForMemcard = true;
            HandleAutoSaveOverwriteMessage();
            break;
        case 0xb0:
            if (FEDatabase->bProfileLoaded) {
                if (MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                    SetScreenVisible(true, 0);
                    SetStringCheckingCard();
                    bStrCpy(m_FileName, FEDatabase->GetUserProfile(0)->GetProfileName());
                    MemoryCard::GetInstance()->StartAutoSave(true);
                    return;
                }
                gMemcardSetup.ClearCommand();
                gMemcardSetup.SetCommand(0x50);
                InitComplete();
            } else {
                gMemcardSetup.ClearCommand();
                gMemcardSetup.SetCommand(0x60);
                InitComplete();
            }
            break;
        case 0xf0:
            extern int IsMemcardEnabled;
            if (MemoryCard::IsCardAvailable() && IsMemcardEnabled) {
                InitCompleteDoList();
            } else {
                MemcardExit(0x8867412d);
            }
            break;
    }
}

void UIMemcardBase::ExitComplete() {
    gMemcardSetup.SendTermMessage(gMemcardSetup.mLastMessage);
    if ((FEDatabase->IsCareerManagerMode()) && TheGameFlowManager.IsInFrontend()) {
        FEDatabase->ResetGameMode();
        if (FEDatabase->bProfileLoaded &&
            !(((gMemcardSetup.GetCommand()) == 0x10 && gMemcardSetup.mLastMessage == 0x8867412d) || gMemcardSetup.mPreviousPrompt == 0x1000000 ||
              gMemcardSetup.mPreviousPrompt == 0x3000000 || gMemcardSetup.mPreviousPrompt == 0x5000000)) {
            if (FEDatabase->GetCareerSettings()->HasCareerStarted()) {
                FEDatabase->GetCareerSettings()->ResumeCareer();
            } else {
                FEDatabase->GetCareerSettings()->StartNewCareer(true);
            }
        } else {
            gMemcardSetup.ClearMethod();
            gMemcardSetup.SetMethod(1);
            FEDatabase->RestoreFromBackupDB();
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER_MANAGER);
        }
    }

    if ((gMemcardSetup.GetExtraOptions() & 0x400000) != 0) {
        uiRepSheetRivalFlow::Get()->Next();
    } else if ((gMemcardSetup.GetExtraOptions() & 0x10000) != 0) {
        if (TheGameFlowManager.IsInFrontend()) {
            cFEng::Get()->QueuePackagePop(1);
            if (FEDatabase->bProfileLoaded) {
                FEDatabase->ResetGameMode();
                FEDatabase->SetGameMode(eFE_GAME_MODE_CHALLENGE);
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen, 0, 0, false);
            }
        } else {
            new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
        }
    } else {
        switch (gMemcardSetup.GetMethod()) {
            case 1:
                if (m_SimPausedForMemcard) {
                    m_SimPausedForMemcard = false;
                    cFEng::Get()->QueuePackagePop(cFEng::Get()->IsPackagePushed("SMS_Mailboxes.fng") ? 1 : 0);
                } else {
                    cFEng::Get()->QueuePackagePop(1);
                }
                break;
            case 2:
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen, MemoryCard::GetInstance()->GetPlayerNum(), 0, false);
                break;
            case 3:
                cFEng::Get()->QueuePackagePop(1);
                cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mToScreen, MemoryCard::GetInstance()->GetPlayerNum(), 0, false);
                break;
        }
    }
    if (m_SimPausedForMemcard) {
        m_SimPausedForMemcard = false;
    }

    eSndAudioMode eaudiomode =
        g_pEAXSound->SetAudioModeFromMemoryCard(static_cast<eSndAudioMode>(FEDatabase->GetOptionsSettings()->TheAudioSettings.AudioMode));
    FEDatabase->GetOptionsSettings()->TheAudioSettings.AudioMode = eaudiomode;
    g_pEAXSound->UpdateVolumes(&FEDatabase->GetOptionsSettings()->TheAudioSettings, -1.0f);
    InitializeEATrax(true);

    FEPackage *pPackageWithControl = cFEng::Get()->FindPackage(gMemcardSetup.mMemScreen);
    if (pPackageWithControl != nullptr) {
        FEPackage *pParentPackage = pPackageWithControl->GetParentPackage();
        if (pParentPackage != nullptr) {
            pParentPackage->SetInputEnabled(true);
        }
    }

    gMemcardSetup.Complete(gMemcardSetup.mLastMessage);

    if (MemoryCard::GetInstance()->InBootSequence()) {
        BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
        MemoryCard::GetInstance()->EndBootSequence();
    }
    cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
    MemoryCard::GetInstance()->SetMemcardScreenShowing(false);
    MemoryCard::GetInstance()->SetMemcardScreenExiting(false);
    if (MemoryCard::GetInstance()->IsMonitorOn()) {
        MemoryCard::GetInstance()->SetMonitor(false);
    }
}

eMenuSoundTriggers UIMemcardBase::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (m_bAnyButtonVisible) {
        return maybe;
    }
    if (msg == 0x48122792 || msg == 0x4ac5e165) {
        return UISND_NONE;
    }
    return MenuScreen::NotifySoundMessage(msg, maybe);
}

// UNSOLVED
void UIMemcardBase::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    if (msg != __BUTTON_PRESSED__ && MemoryCard::GetInstance()->GetOp() == 0) {
        UIMemcardKeyboard::NotificationMessage(msg, obj, param1, param2);
    }

    const uint32 FEObj_Header_text_02 = 0;
    const u32 FEObj_Init = 0;
    const u32 FEObj_HIGHLIGHTCOMPLETE = 0;
    const u32 FEObj_UNHIGHLIGHTCOMPLETE = 0;

    switch (msg) {
        case FEHASH_EXITCOMPLETE:
            ExitComplete();
            break;
        case 0x3a2be557:
        case FEHASH_INITCOMPLETE:
            InitComplete();
            break;
        case 0xda5b8712:
            bStrCpy(m_FileName, FEngGetEditedString());
            FEDatabase->GetUserProfile(0)->SetProfileName(m_FileName, true);
            FEDatabase->DeallocBackupDB();
            FEDatabase->bProfileLoaded = true;
            DoSaveFlow(4);
            break;
        case 0xc9d30688:
            if ((gMemcardSetup.GetCommand()) == 0x60 && !FEDatabase->bProfileLoaded) {
                DoSaveFlow(2);
            } else if ((gMemcardSetup.GetCommand() & 0x60) != 0 && FEDatabase->bProfileLoaded) {
                DoSaveFlow(1);
            } else {
                FEPrintf(m_pDisplayMsg, "");
                m_bDelayedFailed = true;
            }
            break;
        case FEMSG_SCREEN_TICK:
            if (m_bDelayedFailed) {
                m_bDelayedFailed = false;
                cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
            }
            break;
        case 0xc502df5d:
            m_bInButtonAnimation = true;
            TranslateButton(obj);
            break;
        case __BUTTON_PRESSED__:
            m_bInButtonAnimation = false;
            gMemcardSetup.mLastController = param1;
            HandleButtonPressed(__BUTTON_PRESSED__, obj, param1, param2, false);
            break;
        case 0xf35d144e:
            SetupPromptCorruptProfile();
            break;
        case 0x54b3ac6c:
            SetScreenVisible(false, 0);
            cFEng::Get()->QueuePackagePush("MC_List.fng", 0, 0, false);
            break;
    }
}

void UIMemcardBase::HandleButtonPressed(u32 msg, FEObject *obj, u32 param1, u32 param2, bool bPadBack) {
    bool bYes = (obj->NameHash == gButtonIDs[0]) && !bPadBack;
    int nPrompt = gMemcardSetup.GetPrompt();
    gMemcardSetup.ClearPrompt();
    HideAllButtons();

    switch (nPrompt) {
        case 0x1000000:
            if (bYes) {
                FEDatabase->AllocBackupDB(true);
                if ((gMemcardSetup.GetExtraOptions() & 0x40000) == 0) {
                    if ((gMemcardSetup.GetExtraOptions() & 0x200000) == 0) {
                        FEDatabase->DefaultProfile();
                    }
                }
                if ((gMemcardSetup.GetExtraOptions() & 0x80000) != 0) {
                    FEDatabase->GetCareerSettings()->StartNewCareer(false);
                }
                if ((gMemcardSetup.GetCommand()) == 0x20) {
                    gMemcardSetup.ClearCommand();
                    gMemcardSetup.SetCommand(0x60);
                }
                DoSaveFlow(10);
                return;
            } else {
                if ((gMemcardSetup.GetExtraOptions() & 0x80000) != 0) {
                    FEDatabase->RestoreFromBackupDB();
                }
                if (true) {
                    cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
                } else {
                    // Unknown, stripped
                    cFEng::Get();
                    GetPackageName();
                }
            }
            break;
        case 0x4000000:
            if (bYes) {
                DoSaveFlow(12);
            } else {
                if ((gMemcardSetup.GetCommand()) == 0x60) {
                    FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                }
                cFEng::Get()->QueueGameMessage(0xdc12af2e, GetPackageName(), 0xff);
            }
            break;
        case 0x5000000:
            if (bYes) {
                FEDatabase->AllocBackupDB(true);
                if ((gMemcardSetup.GetExtraOptions() & 0x40000) == 0) {
                    if ((gMemcardSetup.GetExtraOptions() & 0x200000) == 0) {
                        FEDatabase->DefaultProfile();
                    }
                }
                DoSaveFlow(10);
            } else {
                MemcardExit(0x8867412d);
            }
            break;
        case 0x6000000:
            if (bYes) {
                InitCompleteDoList();
            } else {
                cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
            }
            break;
        case 0x7000000:
            cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
            break;
        case 0x8000000:
            DoSaveFlow(11);
            break;
        case 0x9000000:
            cFEng::Get()->QueuePackageMessage(0x40E73793, GetPackageName(), nullptr);
            DoSaveFlow(3);
            break;
        case 0xa000000:
            if (bYes) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
            } else {
                MemoryCard::GetInstance()->SetRetryAutoSave(true);
                FEDatabase->GetGameplaySettings()->AutoSaveOn = true;
                gMemcardSetup.ClearCommand();
                MemoryCard::GetInstance()->ShowMessages(true);
                gMemcardSetup.SetCommand(0x50);
                DoSaveFlow(12);
            }
            break;
        case 0xb000000:
            if ((gMemcardSetup.GetCommand()) == 0xa0) {
                if ((gMemcardSetup.GetExtraOptions() & 0x8000) == 0) {
                    gMemcardSetup.ClearMethod();
                    gMemcardSetup.SetMethod(1);
                }
            }
            cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
            break;
        case 0xc000000:
            if (bYes) {
                MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
            } else {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
                cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
            }
            break;
        case 0x3000000:
        case 0xd000000:
            cFEng::Get()->QueueGameMessage(0x8867412d, GetPackageName(), 0xff);
            break;
        default:
            SetStringCheckingCard();
            MemoryCardMessage *pMsg = MemoryCard::GetInstance()->GetPendingMessage();
            if (pMsg != nullptr) {
                ShowMessage(pMsg);
            }
            if (MemoryCard::GetInstance()->GetOp() == 7) {
                cFEng::Get()->QueuePackageMessage(FEHashUpper("SHOW LOADER"), GetPackageName(), nullptr);
            }
            break;
    }
}

void UIMemcardBase::HideAllButtons() {
    m_bAnyButtonVisible = false;
    for (int i = 0; i <= 2; i++) {
        ShowButton(i, false, nullptr);
    }

    const u32 FEObj_CursorPosition = 0x07f9dca9;

    FEngSetScript(GetPackageName(), FEObj_CursorPosition, FEHASH_HIDE, true);
}

void UIMemcardBase::ShowButton(int idx, bool bShow, short *pText) {
    if (bShow) {
        m_bAnyButtonVisible = true;
        if (pText != nullptr) {
            FEString *pObj = static_cast<FEString *>(FEngFindObject(GetPackageName(), gButtonTextIDs[idx]));
            FESetString(pObj, pText);
        }
        FEngEnableButton(GetPackageName(), gButtonTextIDs[idx]);
        FEngSetVisible(GetPackageName(), gButtonTextIDs[idx]);
        FEngSetVisible(GetPackageName(), gButtonIDs[idx]);
        FEngSetScript(GetPackageName(), 0x57689fdd, FEHASH_FORWARD, true);
    } else {
        FEngDisableButton(GetPackageName(), gButtonIDs[idx]);
        FEngSetInvisible(GetPackageName(), gButtonIDs[idx]);
        FEngSetInvisible(GetPackageName(), gButtonTextIDs[idx]);
    }
}

void UIMemcardBase::SetButtonText(short *b1, short *b2, short *b3) {
    int active = 0;
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
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[active]);
    m_ExpectingInput = true;
    gMemcardSetup.ClearPrompt();
}

void UIMemcardBase::SetMessage(i16 *pMsg) {
    if (pMsg == nullptr) {
        SetMessageBlurbText(const_cast<char *>(""));
        HideAllButtons();
    } else {
        SetMessageBlurbText(pMsg);
        m_pDisplayMsg->Flags |= 2;

        const u32 FEObj_MEMCARD = 0x47ff4e7c;
        FEngSetScript(GetPackageName(), FEObj_MEMCARD, 0xe18da018, true);
    }
}

void UIMemcardBase::ShowOK(uint32 language_main, uint32 flag) {
    cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
    SetMessageBlurbText(language_main);
    gMemcardSetup.SetPrompt(flag);
    ShowButton(0, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[0], 0x417b2601);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    ShowButton(1, false, nullptr);
    ShowButton(2, false, nullptr);
    m_ExpectingInput = true;
    SetScreenVisible(true, 1);
}

void UIMemcardBase::ShowYesNo(uint32 language_main, uint32 flag) {
    cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
    SetMessageBlurbText(language_main);
    gMemcardSetup.SetPrompt(flag);
    ShowButton(0, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[0], LANGUAGE_COMMON_NO);
    ShowButton(1, true, nullptr);
    FEngSetLanguageHash(GetPackageName(), gButtonTextIDs[1], LANGUAGE_COMMON_YES);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    ShowButton(2, false, nullptr);
    m_ExpectingInput = true;
    SetScreenVisible(true, 2);
}

void UIMemcardBase::SetScreenVisible(bool visible, int nButtons) {
    if (m_bVisible != visible) {
        m_bVisible = visible;
        cFEng::Get()->QueuePackageMessage(visible ? 0xc0f2ae7c : 0x4f3559b5, GetPackageName(), nullptr);
        if (visible) {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("INITIALIZE_SCREEN"), GetPackageName(), nullptr);
        }
        MemoryCard::GetInstance()->SetMemcardScreenInitialized(m_bVisible);
    }
    if (visible) {
        char script[32];
        bSPrintf(script, "%d_BUTTONS", nButtons);
        cFEng::Get()->QueuePackageMessage(FEHashUpper(script), GetPackageName(), nullptr);
    }
}

void UIMemcardBase::SetIcon(uint32 icon) {
    FEngSetTextureHash(GetPackageName(), 0xd4f4069, icon);
    FEngSetTextureHash(GetPackageName(), 0xfac88427, icon);
}

void UIMemcardBase::TranslateButton(FEObject *obj) {
    if (obj->Flags & 1) {
        return;
    }
    if (obj->NameHash == gButtonIDs[0]) {
        MemoryCard::GetInstance()->MessageDone(RealmcIface::CHOICE_OPTION1);
    } else if (obj->NameHash == gButtonIDs[1]) {
        MemoryCard::GetInstance()->MessageDone(RealmcIface::CHOICE_OPTION2);
    } else if (obj->NameHash == gButtonIDs[2]) {
        MemoryCard::GetInstance()->MessageDone(RealmcIface::CHOICE_OPTION3);
    }
    m_ExpectingInput = false;
}

void UIMemcardBase::SetupPromptNoProfileFound() {
    ShowOK(0xba373453, 0x3000000);
}

void UIMemcardBase::SetupPromptSaveConfirm() {
    char *fmt;
    if ((gMemcardSetup.GetExtraOptions() & 0x8000) != 0) {
        fmt = GetLocalizedString(0x391a0aac);
    } else {
        uint32 fmtHash;
        if ((gMemcardSetup.GetExtraOptions() & 0x40000) != 0) {
            fmtHash = 0xb0af33a5;
        } else if ((gMemcardSetup.GetExtraOptions() & 0x200000) != 0) {
            fmtHash = 0xd80818f8;
        } else {
            fmtHash = 0x39b3ccba;
        }
        fmt = GetLocalizedString(fmtHash);
    }
    ShowYesNo(0x39b3ccba, 0x4000000);
    char text[512];
    bSPrintf(text, fmt, m_FileName, m_FileName);
    SetMessageBlurbText(text);
}

void UIMemcardBase::SetupAutoSaveConfirmPrompt() {
    gMemcardSetup.SetPrompt(0xa000000);
    SetMessageBlurbText(GetLocalizedString(0xa0b434a2));
    FEngEnableButton(GetPackageName(), gButtonIDs[0]);
    FEngSetVisible(GetPackageName(), gButtonIDs[0]);
    FEngSetVisible(GetPackageName(), gButtonTextIDs[0]);
    FEPrintf(GetPackageName(), static_cast<int>(gButtonTextIDs[0]), GetLocalizedString(LANGUAGE_COMMON_NO));
    FEngEnableButton(GetPackageName(), gButtonIDs[1]);
    FEngSetVisible(GetPackageName(), gButtonIDs[1]);
    FEngSetVisible(GetPackageName(), gButtonTextIDs[1]);
    FEPrintf(GetPackageName(), static_cast<int>(gButtonTextIDs[1]), GetLocalizedString(0x2b07a03d));
    FEngDisableButton(GetPackageName(), gButtonIDs[2]);
    FEngSetInvisible(GetPackageName(), gButtonIDs[2]);
    FEngSetInvisible(GetPackageName(), gButtonTextIDs[2]);
    FEngSetCurrentButton(GetPackageName(), gButtonIDs[0]);
    FEngSetScript(GetPackageName(), FEHashUpper("HANDLER"), FEHashUpper("FORWARD"), true);
    SetScreenVisible(true, 2);
}

void UIMemcardBase::SetupPromptForSave() {
    ShowYesNo(0x83f4bb3e, 0x4000000);
    char *fmt = GetLocalizedString(gMemcardSetup.GetExtraOptions() & 0x200000 ? 0xd80818f8 : 0x83f4bb3e);
    char text[512];
    bSPrintf(text, fmt, m_FileName, m_FileName);
    SetMessageBlurbText(text);
}

void UIMemcardBase::SetupPromptCorruptProfile() {
    ShowOK(0x821e4444, 0xd000000);
    char text[512];
    char *fmt = GetLocalizedString(0x821e4444);
    bSPrintf(text, fmt, m_FileName);
    SetMessageBlurbText(text);
}

void UIMemcardBase::SetupPromptAutoSaveEnableFailedNoCard() {
    uint32 msg = 0xb000000;
    ShowOK(0x9e85bba8, msg);
}

void UIMemcardBase::Setup() {
    const uint32 FEObj_Header_text_02 = 0x99054304;
    FEngSetLanguageHash(GetPackageName(), FEHASH_HEADERTEXT, 0x774e4dd9);
    FEngSetLanguageHash(m_pDisplayMsg, FEObj_Header_text_02);
    MemoryCard::GetInstance()->FEngLinkObjects(this);
    SetIcon(0x6948e2b3);
}

void UIMemcardBase::SetStringCheckingCard() {
    SetScreenVisible(true, 0);
    SetMessageBlurbText(0x99054304);
    cFEng::Get()->QueuePackageMessage(FEHashUpper("0_BUTTONS"), GetPackageName(), nullptr);
    HideAllButtons();
    m_ExpectingInput = false;
}

void UIMemcardBase::ShowKeyboard() {
    SetScreenVisible(false, 0);
    HideAllButtons();
    UIMemcardKeyboard::ShowKeyboard();
}

void UIMemcardBase::DoSaveFlow(int flow) {
    if (flow != 0) {
        m_Flow = flow;
    } else {
        if (!FEDatabase->GetUserProfile(0)->IsProfileNamed()) {
            m_Flow = 2;
        }
    }
    uint32 msg;
    switch (m_Flow) {
        case 9:
            ShowOK(0xd9783c57, 0x3000000);
            break;
        case 1:
            ShowYesNo(0x7209349f, 0x5000000);
            break;
        case 2:
            if ((gMemcardSetup.GetExtraOptions() & 0x80000) != 0) {
                msg = 0xbadd522c;
            } else if ((gMemcardSetup.GetExtraOptions() & 0x10000) != 0) {
                msg = 0x93c25b3d;
            } else if ((gMemcardSetup.GetExtraOptions() & 0x8000) != 0) {
                msg = 0xf8448956;
            } else {
                msg = 0xbe97590f;
            }
            ShowYesNo(msg, 0x1000000);
            break;

        case 3:
            ShowKeyboard();
            break;
        case 6:
            SetupPromptSaveConfirm();
            break;
        case 4:
            SetupPromptForSave();
            break;
        case 12:
            MemoryCard::GetInstance()->SetAutoSaveEnabled(false);
            break;
        case 8:
            FEDatabase->GetUserProfile(0)->SetProfileName(m_FileName, true);
            MemoryCard::GetInstance()->Save(m_FileName);
            SetStringCheckingCard();
            break;
        case 10:
            cFEng::Get()->QueuePackageMessage(0x1c8ace, GetPackageName(), nullptr);
            msg = GetAutoSaveWarning();
            ShowOK(msg, 0x9000000);
            break;
        case 11:
            msg = GetAutoSaveWarning2();
            ShowOK(msg, 0x9000000);
            break;
    }
}

void UIMemcardBase::SetMessageBlurbText(short *pText) {
    FESetString(m_pDisplayMsg, pText);
    if (m_pDisplayMsgShadow != nullptr) {
        FESetString(m_pDisplayMsgShadow, pText);
    }
    FindScreenSize(reinterpret_cast<const wchar_t *>(pText));
}

void UIMemcardBase::SetMessageBlurbText(char *pText) {
    int wText[1024];
    FEPrintf(m_pDisplayMsg, pText);
    if (m_pDisplayMsgShadow != nullptr) {
        FEPrintf(m_pDisplayMsgShadow, pText);
    }
    bStrCpy(reinterpret_cast<unsigned short *>(wText), pText);
    FindScreenSize(reinterpret_cast<const wchar_t *>(wText));
}

void UIMemcardBase::SetMessageBlurbText(unsigned int textHash) {
    FEngSetLanguageHash(m_pDisplayMsg, textHash);
    if (m_pDisplayMsgShadow != nullptr) {
        FEngSetLanguageHash(m_pDisplayMsgShadow, textHash);
    }
    char *locString = GetLocalizedString(textHash);
    wchar_t wLocString[1024];
    bStrCpy(reinterpret_cast<u16 *>(wLocString), locString);
    FindScreenSize(wLocString);
}

void UIMemcardBase::FindScreenSize(const wchar_t *msg) {
    FEngFont *font = FindFont(0x545570c6);
    float numLines = static_cast<float>(bStrLen(reinterpret_cast<const uint16 *>(msg))) * font->GetHeight();

    const int MAX_SIZE_SMALL = 2200.0f;
    const int MAX_SIZE_MED = 4400.0f;
    const u32 FEObj_dialogsmall = 0x79b0c1c7;
    const u32 FEObj_dialogmedium = 0xa13adcaf;
    const u32 FEObj_dialoglarge = 0x792bc959;

    if (numLines < MAX_SIZE_SMALL) {
        cFEng::Get()->QueuePackageMessage(FEObj_dialogsmall, GetPackageName(), nullptr);
    } else if (numLines < MAX_SIZE_MED) {
        cFEng::Get()->QueuePackageMessage(FEObj_dialogmedium, GetPackageName(), nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(FEObj_dialoglarge, GetPackageName(), nullptr);
    }
}

uint32 UIMemcardBase::GetAutoSaveWarning() {
    return 0xb39899c2;
}

uint32 UIMemcardBase::GetAutoSaveWarning2() {
    return 0x2386f454;
}

void UIMemcardBase::ShowMessage(MemoryCardMessage *msg) {
    ShowMessage(reinterpret_cast<const wchar_t *>(msg->mMsg), msg->mnOptions, reinterpret_cast<const wchar_t *>(msg->mOptions[0]),
                reinterpret_cast<const wchar_t *>(msg->mOptions[1]), reinterpret_cast<const wchar_t *>(msg->mOptions[2]));
    MemoryCard::GetInstance()->ReleasePendingMessage();
}

// UNSOLVED
void UIMemcardBase::ShowMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t *option1, const wchar_t *option2, const wchar_t *option3) {
    PopChild();
    HideAllButtons();
    SetMessage(reinterpret_cast<i16 *>(const_cast<wchar_t *>(msg)));
    switch (nOptions) {
        case 1:
            SetButtonText(reinterpret_cast<short *>(const_cast<wchar_t *>(option1)), nullptr, nullptr);
            break;
        case 2:
            SetButtonText(reinterpret_cast<short *>(const_cast<wchar_t *>(option1)), reinterpret_cast<short *>(const_cast<wchar_t *>(option2)),
                          nullptr);
            break;
        case 3:
            SetButtonText(reinterpret_cast<short *>(const_cast<wchar_t *>(option1)), reinterpret_cast<short *>(const_cast<wchar_t *>(option2)),
                          reinterpret_cast<short *>(const_cast<wchar_t *>(option3)));
            break;
        default:
            MemoryCard::GetInstance()->SetWaitingForResponse(false);
            break;
    }
    SetScreenVisible(true, nOptions);
    cFEng::Get()->QueuePackageMessage(FEHashUpper(nOptions == 0 ? "SHOW LOADER" : "HIDE LOADER"), GetPackageName(), nullptr);
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
    if (!MemoryCard::GetInstance()->IsCheckingCardForAutoSave() && !MemoryCard::GetInstance()->IsCheckingCardForOverwrite()) {
        if ((gMemcardSetup.GetCommand()) != 0xb0) {
            gMemcardSetup.ClearMethod();
            gMemcardSetup.SetMethod(1);
        }
        gMemcardSetup.ClearCommand();
        gMemcardSetup.SetCommand(0x50);
    }
    bStrCpy(m_FileName, FEDatabase->GetUserProfile(0)->GetProfileName());
    if (MemoryCard::GetInstance()->IsCheckingCardForAutoSave() || MemoryCard::GetInstance()->IsCheckingCardForOverwrite() ||
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
    bStrCpy(m_FileName, FEDatabase->GetUserProfile(0)->GetProfileName());
    MemoryCard::GetInstance()->EndAutoSave();
#ifndef EA_BUILD_A124
    FEDatabase->bAutoSaveOverwriteConfirmed = true;
#endif
    gMemcardSetup.ClearCommand();
    gMemcardSetup.SetCommand(0x50);
    MemoryCard::GetInstance()->ShowMessages(true);
    DoSaveFlow(12);
}

// ===== UIMemcardList =====

UIMemcardList::UIMemcardList(ScreenConstructorData *sd)
    : MenuScreen(sd),                                                             //
      m_SaveGameList(GetPackageName(), "", "Scrollbar", true, true, false, false) //
{
    const uint32 FEObj_Header_text_02 = 0;
    const uint32 FEObj_BACK = 0;
    const uint32 FEObj_Player_Name = 0xeb406fec;
    const u32 FEObj_CREATE_BUTTON = 0;

    m_Initialized = 0;
    FEPrintf(GetPackageName(), FEObj_Player_Name, FEDatabase->GetUserProfile(0)->GetProfileName());

    for (int i = 1; i < 9; i++) {
        char tempStr[32];
        ScrollerSlot *pSlot = new ("ScrollerSlot", 0) ScrollerSlot();

        FEngSNPrintf(tempStr, 0x20, "option_name_%d", i);
        pSlot->AddData(FEngFindString(GetPackageName(), FEHashUpper(tempStr)));

        FEngSNPrintf(tempStr, 0x20, "option_data_%d", i);
        pSlot->AddData(FEngFindString(GetPackageName(), FEHashUpper(tempStr)));

        FEngSNPrintf(tempStr, 0x20, "option_mouse_%d", i);
        pSlot->SetBacking(FEngFindObject(GetPackageName(), FEHashUpper(tempStr)));
        pSlot->Hide();
        m_SaveGameList.AddSlot(pSlot);
    }

    m_ListOp = static_cast<int>((gMemcardSetup.GetCommand()) == 0x30);

    static uint32 sOpName[2] = {0x841c21af, 0xe85326e2};
    FEngSetLanguageHash(GetPackageName(), 0x48d4fcae, sOpName[m_ListOp]);
    FEngSetLanguageHash(GetPackageName(), 0x426c7b4d, sOpName[m_ListOp]);
}

UIMemcardList::~UIMemcardList() {}

MenuScreen *CreateMemcardListFiles(ScreenConstructorData *sd) {
    UIMemcardList *pRes = new ("UIMemcardList", 0) UIMemcardList(sd);
    static_cast<UIMemcardMain *>(MemoryCard::GetInstance()->GetScreen())->SetPopupWindow(pRes);
    return pRes;
}

// total size: 0x24
class FEMemWidget : public ScrollerDatum {
  public:
    static const int MAX_SIZE; // size: 0x4, address: 0xFFFFFFFF
    MemCardFileFlag m_Flag;    // offset 0x18, size 0x4
    int m_Size;                // offset 0x1C, size 0x4
    UIMemcardList *m_pParent;  // offset 0x20, size 0x4

    FEMemWidget() {}
    ~FEMemWidget() override {}

    void Act(const char *parent_pkg, uint32 data) {
        MemoryCard::GetInstance()->GetScreen()->DoSelect(reinterpret_cast<ScrollerDatum *>(data)->Strings.GetNode(0)->String);
    }
    bool IsCorrupt() {}
    int GetSize() const {}
    const char *GetFileName() {}
};

void UIMemcardList::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE: {
            UIMemcardBase *pParent;
            m_SaveGameList.SetSelected(m_SaveGameList.GetFirstSlot());
            m_SaveGameList.HighlightSelected();
            MemoryCard::GetInstance()->GetScreen()->m_ExpectingInput = true;
            m_Initialized++;
            if (MemoryCard::GetInstance()->InBootSequence()) {
                FEngSetLanguageHash(GetPackageName(), 0xb8a7c6cd, 0x1a294dad);
            }
            break;
        }
        case FEMSG_SCREEN_TICK:
            if (m_Initialized != 0) {
                m_Initialized = 1;
                UIMemcardBase *pParent = MemoryCard::GetInstance()->GetScreen();
                for (UIMemcardBase::Item *pNode = pParent->m_Items.GetHead(); pNode != pParent->m_Items.EndOfList(); pNode = pNode->GetNext()) {
                    const char *pName = pNode->m_Name + MemoryCard::GetInstance()->GetPrefixLength();
                    if (pParent->IsProfile(pName)) {
                        AddItem(pName, pNode->m_Data, pNode->m_Size, pNode->m_Flag);
                    }
                }
                FEngSetScript("MC_List.fng", 0x47ff4e7c, 0x13c37b, true);
            }
            break;
        case __PAD_BACK__:
            if (MemoryCard::GetInstance()->InBootSequence()) {
                cFEng::Get()->QueueGameMessage(0x8d0cc9f9, "MC_Main_GC.fng", 0xff);
            } else {
                cFEng::Get()->QueueGameMessage(0x8867412d, MemoryCard::GetInstance()->GetScreen()->GetPackageName(), 0xff);
            }
            gMemcardSetup.mLastController = param1;
            break;
        case __PAD_UP__:
            gMemcardSetup.mLastController = param1;
            m_SaveGameList.ScrollPrev();
            break;
        case __PAD_DOWN__:
            gMemcardSetup.mLastController = param1;
            m_SaveGameList.ScrollNext();
            break;
        case __PAD_ACCEPT__:
            gMemcardSetup.mLastController = param1;
            if (!FEDatabase->IsSplitScreenMode()) {
                FEDatabase->SetPlayersJoystickPort(MemoryCard::GetInstance()->GetPlayerNum(), FEngMapJoyParamToJoyport(param1));
            }
            MemoryCard::GetInstance()->SetMonitor(false);
            break;
        case 0xeb29392a:
            if (m_LastMsg == __PAD_ACCEPT__) {
                // UNSOLVED (dwarf)
                m_pCreateNew->Act(GetPackageName(), reinterpret_cast<uint32>(m_SaveGameList.GetSelectedDatum()));
            }
            break;
    }
    m_LastMsg = msg;
}

FEMemWidget *UIMemcardList::AddItem(const char *pName, const char *pDate, int size, int flag) {
    FEMemWidget *pDatum = new ("FEMemWidget", 0) FEMemWidget();
    pDatum->m_Flag = static_cast<MemCardFileFlag>(flag);
    pDatum->m_Size = size;
    pDatum->AddData(pName, 0);
    pDatum->AddData(pDate, 0);
    m_SaveGameList.AddData(pDatum);
    m_SaveGameList.Enable(pDatum);
    m_SaveGameList.Update(true);
    return pDatum;
}
