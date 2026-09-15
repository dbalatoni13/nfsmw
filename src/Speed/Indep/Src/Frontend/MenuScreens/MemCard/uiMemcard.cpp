#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcard.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

void UIMemcardBoot::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case FEHASH_INITCOMPLETE:
            HideAllButtons();
            MemoryCard::GetInstance()->ShowMessages(true);
            extern int IsMemcardEnabled;
            if (!IsMemcardEnabled) {
                cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
                return;
            }
            SetStringCheckingCard();
            MemoryCard::GetInstance()->BootupCheck(nullptr);
            SetScreenVisible(true, 0);
            m_bVisible = true;
            break;
        case 0x461a18ee:
            MemoryCard::GetInstance()->StartBootSequence();
            // fall through
        case 0x8867412d:
            BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            MemoryCard::GetInstance()->SetMemcardScreenInitialized(false);
            break;
    }
}

eMenuSoundTriggers UIMemcardBoot::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    return maybe;
}

MenuScreen *CreateMemCardBootScreen(ScreenConstructorData *sd) {
    UIMemcardBase *pRes = new ("MemCardBootScreen", 0) UIMemcardBoot(sd);
    pRes->Setup();
    return pRes;
}

MenuScreen *CreateMemcardMainMenu(ScreenConstructorData *sd) {
    UIMemcardMain *pRes = new ("UIMemcardMain", 0) UIMemcardMain(sd);
    pRes->Setup();
    return pRes;
}

UIMemcardMain::UIMemcardMain(ScreenConstructorData *sd) : UIMemcardBase(sd) {
    FEString *mpBlurb = FEngFindString(GetPackageName(), 0x1e2640fa);
    mpBlurb->Flags &= ~0x200;
}

void UIMemcardMain::DoSelect(const char *pName) {
    bStrCpy(m_FileName, pName);
    switch (m_pChild->GetListOp()) {
        case 0:
            MemoryCard::GetInstance()->RequestTask(5, m_FileName);
            SetStringCheckingCard();
            break;
        case 1:
            MemoryCard::GetInstance()->RequestTask(6, m_FileName);
            SetStringCheckingCard();
            break;
    }
    PopChild();
}

void UIMemcardMain::ListDone() {
    bool bCreateChild = false;
    MemoryCard::GetInstance()->ShowMessages(true);

    int nSize = m_Items.CountElements();

    switch (MemcardGetCurrentUIOperation()) {
        case 0x30:
            if (nSize == 0) {
                SetupPromptNoProfileFound();
            } else {
                bCreateChild = true;
            }
            break;
        case 0x10:
        case 0x70:
            if (nSize != 0) {
                bCreateChild = true;
            } else {
                SetupPromptNoProfileFound();
            }
            break;
        case 0x20:
            gMemcardSetup.mInBootFlow = true;
            if (nSize > 1) {
                bCreateChild = true;
            } else if (nSize == 1) {
                const char *pName = m_Items.GetHead()->m_Name;
                pName += MemoryCard::GetInstance()->GetPrefixLength();
                bStrCpy(m_FileName, pName);
                MemoryCard::GetInstance()->Load(m_FileName);
            } else {
                MemoryCard::GetInstance()->BootupCheck(nullptr);
            }
            break;
        case 0xf0:
            if (nSize > 1) {
                bCreateChild = true;
            } else if (nSize != 1) {
                DoSaveFlow(2);
            } else {
                const char *pName = m_Items.GetHead()->m_Name;
                pName += MemoryCard::GetInstance()->GetPrefixLength();
                bStrCpy(m_FileName, pName);
                MemoryCard::GetInstance()->Load(m_FileName);
            }
            break;
        case 0x40:
            if (FEDatabase->bProfileLoaded) {
                bStrCpy(const_cast<char *>(m_FileName), FEDatabase->GetUserProfile(0)->GetProfileName());
                DoSaveFlow(4);
            } else {
                DoSaveFlow(2);
            }
            break;
        case 0x60:
            if (FEDatabase->bProfileLoaded && (gMemcardSetup.mOp & 0x80000) == 0) {
                DoSaveFlow(1);
            } else {
                DoSaveFlow(2);
            }
            break;
    }

    if (bCreateChild) {
        ActivateChild();
    }
}

// UNSOLVED
void UIMemcardMain::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case 0x5a051729:
            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
            ListDone();
            break;
        case 0xa4bb7ae1:
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
            goto hide_loader;
        case 0xfe202e3b:
            DoSaveFlow(4);
            break;
        case 0x461a18ee:
            if (MemoryCard::GetInstance()->InBootSequence()) {
                PopChild();
            }
            FEDatabase->DeallocBackupDB();
            MemcardExit(0x461a18ee);
            goto hide_loader;
        case 0xa643dee3:
            if (MemoryCard::GetInstance()->IsAutoLoading()) {
                return;
            }
            cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
            break;
        case 0x15457de1:
            PopChild();
            break;
        case 0xc6c6b68f:
            DoSaveFlow(8);
            break;
        case 0x8867412d:
        case 0xdc12af2e:
            PopChild();
            if ((gMemcardSetup.mOp & 0x800) != 0 && FEDatabase->GetUserProfile(0)->IsProfileNamed()) {
                cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
                goto hide_loader;
            }
            if (FEDatabase->IsCareerManagerMode() && FEDatabase->bProfileLoaded && FEDatabase->GetGameplaySettings()->AutoSaveOn &&
                (gMemcardSetup.GetCommand()) != 0x10 && msg != 0xdc12af2e) {
                MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
            } else {
                if ((gMemcardSetup.GetCommand()) == 0x60 && msg == 0xdc12af2e) {
                    FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                    ShowOK(0xb04da4ad, 0x7000000);
                } else {
                    MemcardExit(msg);
                }
            }
            if (MemoryCard::GetInstance()->IsCheckingCardForAutoSave() || MemoryCard::GetInstance()->IsAutoSaving()) {
                MemoryCard::GetInstance()->EndAutoSave();
            }
            FEDatabase->DeallocBackupDB();
            goto hide_loader;
        case 0xb57fdb17:
            SetupPromptAutoSaveEnableFailedNoCard();
            break;
        case 0x8d0cc9f9:
            PopChild();
            SetStringCheckingCard();
            MemoryCard::GetInstance()->BootupCheck(nullptr);
            break;
        case FEMSG_SCREEN_TICK:
            if (!m_ExpectingInput) {
                return;
            }

            if (!FEngIsScriptSet(GetPackageName(), FEHashUpper("LOADER"), FEHashUpper("APPEAR"))) {
                return;
            }
            goto hide_loader;

            break;
    }
    return;

hide_loader:
    cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
    return;
}
