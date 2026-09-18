#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcard.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

void UIMemcardBoot::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case 0x35F8620B:

            HideAllButtons();
            MemoryCard::GetInstance()->ShowMessages(true);

            if (IsMemcardEnabled == 0) {

                cFEng::Get()->QueueGameMessage(0x461A18EE, GetPackageName(), 0xff);

            } else {

                SetStringCheckingCard();

                MemoryCard::GetInstance()->BootupCheck(nullptr);

                SetScreenVisible(true, 0);
                m_bVisible = true;
            }
            break;

        case 0x461A18EE:

            MemoryCard::GetInstance()->StartBootSequence();
            // cae al caso siguiente: el original no tiene `b` detras de
            // StartBootSequence, y duplicar la cola mide peor (88,46%).

        case 0x8867412D:

            BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            MemoryCard::GetInstance()->SetMemcardScreenInitialized(false);
            break;
    }
}

eMenuSoundTriggers UIMemcardBoot::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    return maybe;
}

MenuScreen *CreateMemCardBootScreen(ScreenConstructorData *sd) {
    UIMemcardBoot *pScreen = new ("MemCardBootScreen", 0) UIMemcardBoot(sd);

    pScreen->Setup();

    return pScreen;
}

MenuScreen *CreateMemcardMainMenu(ScreenConstructorData *sd) {
    UIMemcardMain *pScreen = new ("UIMemcardMain", 0) UIMemcardMain(sd);

    pScreen->Setup();

    return pScreen;
}

UIMemcardMain::UIMemcardMain(ScreenConstructorData *sd) : UIMemcardBase(sd) {
    FEngFindString(GetPackageName(), 0x1E2640FA)->Flags &= ~0x200;
}

void UIMemcardMain::DoSelect(const char *pName) {
    bStrCpy(m_FileName, pName);

    switch (m_pChild->GetListOp()) {
        case UIMemcardList::MCLO_Load:
            MemoryCard::GetInstance()->RequestTask(5, m_FileName);
            SetStringCheckingCard();
            break;
        case UIMemcardList::MCLO_Delete:
            MemoryCard::GetInstance()->RequestTask(6, m_FileName);
            SetStringCheckingCard();
            break;
    }

    PopChild();
}

void UIMemcardMain::ListDone() {
    MemoryCard::GetInstance()->ShowMessages(true);

    bool bCreateChild = false;

    int nSize = m_Items.CountElements();

    switch (MemcardGetCurrentUIOperation()) {
        case MCO_DeleteList:
            if (nSize == 0) {
                SetupPromptNoProfileFound();
            } else {
                bCreateChild = true;
            }
            break;

        case MCO_LoadList:
        case MCO_CarLotLoad:
            if (nSize != 0) {
                bCreateChild = true;
            } else {
                SetupPromptNoProfileFound();
            }
            break;

        case MCO_BootList:
            gMemcardSetup.mInBootFlow = true;

            if (nSize > 1) {
                bCreateChild = true;
            } else if (nSize == 1) {
                char *pFile = m_Items.GetHead()->m_Name;

                bStrCpy(m_FileName, pFile + MemoryCard::GetInstance()->GetPrefixLength());
                MemoryCard::GetInstance()->Load(m_FileName);
            } else {
                MemoryCard::GetInstance()->BootupCheck(nullptr);
            }
            break;

        case 0xF0:
            if (nSize > 1) {
                bCreateChild = true;
            } else if (nSize == 1) {
                char *pFile = m_Items.GetHead()->m_Name;

                bStrCpy(m_FileName, pFile + MemoryCard::GetInstance()->GetPrefixLength());
                MemoryCard::GetInstance()->Load(m_FileName);
            } else {
                DoSaveFlow(MCSF_PromptForCreate);
            }
            break;

        case MCO_SaveCreate:
            if (FEDatabase->bProfileLoaded) {
                bStrCpy(m_FileName, FEDatabase->GetUserProfile(0)->GetProfileName());
                DoSaveFlow(MCSF_PromptForSave);
            } else {
                DoSaveFlow(MCSF_PromptForCreate);
            }
            break;

        case MCO_CreateNew:
            if (FEDatabase->bProfileLoaded && (gMemcardSetup.mOp & MCE_NewCareer) == 0) {
                DoSaveFlow(MCSF_PromptForDestoryCreate);
            } else {
                DoSaveFlow(MCSF_PromptForCreate);
            }
            break;
    }

    if (bCreateChild) {
        ActivateChild();
    }
}

void UIMemcardMain::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {

        case 0x5A051729:
            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);

            ListDone();
            break;

        case 0xA4BB7AE1:
            cFEng::Get()->QueueGameMessage(0x461A18EE, nullptr, 0xff);

            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);

            break;

        case 0xFE202E3B:
            DoSaveFlow(MCSF_PromptForSave);
            break;

        case 0x461A18EE:
            if (MemoryCard::GetInstance()->InBootSequence()) {

                PopChild();
            }

            FEDatabase->DeallocBackupDB();
            MemcardExit(0x461A18EE);

            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
            break;

        case 0xA643DEE3:
            if (!MemoryCard::GetInstance()->IsAutoLoading()) {

                cFEng::Get()->QueueGameMessage(0x461A18EE, GetPackageName(), 0xff);
            }
            break;

        case 0x15457DE1:
            PopChild();
            break;

        case 0xC6C6B68F:
            DoSaveFlow(MCSF_DoSave);

            break;

        case 0x8867412D:
        case 0xDC12AF2E:
            PopChild();

            if ((gMemcardSetup.mOp & MCE_ProfileOnly) != 0 && FEDatabase->GetUserProfile(0)->IsProfileNamed()) {

                cFEng::Get()->QueueGameMessage(0x461A18EE, GetPackageName(), 0xff);

            } else {

                if (FEDatabase->IsCareerManagerMode() && FEDatabase->bProfileLoaded && FEDatabase->GetGameplaySettings()->AutoSaveOn &&
                    gMemcardSetup.GetCommand() != MCO_LoadList && msg != 0xDC12AF2E) {

                    MemoryCard::GetInstance()->SetAutoSaveEnabled(true);

                } else if (gMemcardSetup.GetCommand() == MCO_CreateNew && msg == 0xDC12AF2E) {

                    FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                    ShowOK(LANGUAGE_MC_AUTOSAVE_DISABLED, 0x07000000);

                } else {

                    MemcardExit(msg);
                }

                if (MemoryCard::GetInstance()->IsCheckingCardForAutoSave() || MemoryCard::GetInstance()->IsAutoSaving()) {

                    MemoryCard::GetInstance()->EndAutoSave();
                }

                FEDatabase->DeallocBackupDB();
            }

            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);

            break;

        case 0xB57FDB17:
            SetupPromptAutoSaveEnableFailedNoCard();
            break;

        case 0x8D0CC9F9:
            PopChild();
            SetStringCheckingCard();
            MemoryCard::GetInstance()->BootupCheck(nullptr);
            break;

        case 0xC98356BA:
            if (m_ExpectingInput && FEngIsScriptSet(GetPackageName(), FEHashUpper("LOADER"), FEHashUpper("APPEAR"))) {

                cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE LOADER"), GetPackageName(), nullptr);
            }
            break;

        // Case vacio: no emite codigo pero cuenta en el arbol del switch, y es
        // lo que mueve el pivote del subarbol izquierdo a 0x461A18EE.
        case 0x35F8620B:
            break;
    }
}
