#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
FEObject* FEngFindObject(const char* pkg_name, unsigned int obj_hash);
FEString* FEngFindString(const char* pkg_name, int name_hash);
void FEngSetInvisible(FEObject* obj);
void FEngSetVisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
int FEPrintf(const char* pkg_name, int object_hash, const char* fmt, ...);
int FEngSNPrintf(char* buffer, int buf_size, const char* fmt, ...);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
void FEngSetButtonState(const char* pkg_name, unsigned int button_hash, bool enabled);
unsigned long FEHashUpper(const char* str);
bool FEngIsScriptSet(const char* pkg_name, unsigned long obj_hash, unsigned long script_hash);
int FEngMapJoyParamToJoyport(int param);

extern bool IsMemcardEnabled;
extern GameFlowManager TheGameFlowManager;

void MemcardExit(unsigned int msg);
unsigned int MemcardGetCurrentUIOperation();

extern "C" void ChangeToNextBootFlowScreen__15BootFlowManageri(void* self, int param);

static unsigned int gButtonIDs[3] = {0xb8a7c6cc, 0xb8a7c6cd, 0xb8a7c6ce};
static unsigned int gButtonTextIDs[3] = {0xf9363f30, 0xfb8b67d1, 0xfde09072};

static const unsigned int sOpName[] = {0x841c21af, 0xe85326e2};

// ===== FEMemWidget =====

struct FEMemWidget : public ScrollerDatum {
    MemCardFileFlag m_Flag;
    int m_Size;
    UIMemcardList* m_pParent;

    FEMemWidget() {}
    ~FEMemWidget() override;
};

FEMemWidget::~FEMemWidget() {}

// ===== UIMemcardList forward =====

struct UIMemcardList : public MenuScreen {
    enum ListOp {
        MCLO_Load = 0,
        MCLO_Delete = 1,
    };

    Scrollerina m_SaveGameList;
    int m_Initialized;
    int m_ListOp;
    unsigned int m_LastMsg;
    FEMemWidget* m_pCreateNew;

    UIMemcardList(ScreenConstructorData* sd);
    ~UIMemcardList() override;

    int GetSize() { return m_SaveGameList.GetNumData(); }
    bool IsReady() { return m_Initialized != 0; }
    ListOp GetListOp() { return static_cast< ListOp >(m_ListOp); }

    const char* GetFileName(int find);
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    FEMemWidget* AddItem(const char* pName, const char* pDate, int size, int flag);
};

// ===== UIMemcardBoot =====

struct UIMemcardBoot : public UIMemcardBase {
    UIMemcardBoot(ScreenConstructorData* sd) : UIMemcardBase(sd) {}
    ~UIMemcardBoot() override;

    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg,
                                          eMenuSoundTriggers maybe) override;
};

UIMemcardBoot::~UIMemcardBoot() {}

eMenuSoundTriggers UIMemcardBoot::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return maybe;
}

void UIMemcardBoot::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                         unsigned long param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
    case 0x35f8620b:
        HideAllButtons();
        MemoryCard::GetInstance()->ShowMessages(true);
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
        ChangeToNextBootFlowScreen__15BootFlowManageri(BootFlowManager::Get(), 0xff);
        MemoryCard::GetInstance()->m_bInitialized = false;
        break;
    }
}

MenuScreen* CreateMemCardBootScreen(ScreenConstructorData* sd) {
    UIMemcardBoot* boot = new UIMemcardBoot(sd);
    FEString* blurb = FEngFindString(boot->GetPackageName(), 0x1e2640fa);
    blurb->Flags &= ~0x200;
    boot->Setup();
    return boot;
}

// ===== UIMemcardMain =====

struct UIMemcardMain : public UIMemcardBase {
    ~UIMemcardMain() override;

    void SetPopupWindow(UIMemcardList* pChild) { m_pChild = pChild; }

    UIMemcardMain(ScreenConstructorData* sd);
    void DoSelect(const char* pName) override;
    void ListDone();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
};

UIMemcardMain::~UIMemcardMain() {}

UIMemcardMain::UIMemcardMain(ScreenConstructorData* sd) : UIMemcardBase(sd) {
    FEString* blurb = FEngFindString(GetPackageName(), 0x1e2640fa);
    blurb->Flags &= ~0x200;
}

void UIMemcardMain::DoSelect(const char* pName) {
    bStrCpy(m_FileName, pName);
    int listOp = m_pChild->m_ListOp;
    switch (listOp) {
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

    unsigned int uiOp = MemcardGetCurrentUIOperation();

    switch (uiOp) {
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
            const char* pName = m_Items.GetHead()->m_Name;
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
            const char* pName = m_Items.GetHead()->m_Name;
            pName += MemoryCard::GetInstance()->GetPrefixLength();
            bStrCpy(m_FileName, pName);
            MemoryCard::GetInstance()->Load(m_FileName);
        }
        break;
    case 0x40:
        if (FEDatabase->bProfileLoaded) {
            const char* pName = m_FileName;
            const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
            bStrCpy(const_cast<char*>(pName), profileName);
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

void UIMemcardMain::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                        unsigned long param2) {
    UIMemcardBase::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
    case 0x5a051729: {
        cFEng* feng = cFEng::Get();
        unsigned long hideHash = FEHashUpper("HIDE LOADER");
        feng->QueuePackageMessage(hideHash, GetPackageName(), nullptr);
        ListDone();
        break;
    }
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
        if ((gMemcardSetup.mOp & 0x800) != 0 &&
            FEDatabase->GetUserProfile(0)->IsProfileNamed()) {
            cFEng::Get()->QueueGameMessage(0x461a18ee, GetPackageName(), 0xff);
            goto hide_loader;
        }
        if (FEDatabase->IsCareerManagerMode() &&
            FEDatabase->bProfileLoaded &&
            FEDatabase->GetGameplaySettings()->AutoSaveOn != 0 &&
            (gMemcardSetup.mOp & 0xf0) != 0x10 &&
            msg != 0xdc12af2e) {
            MemoryCard::GetInstance()->SetAutoSaveEnabled(true);
        } else {
            if ((gMemcardSetup.mOp & 0xf0) == 0x60 && msg == 0xdc12af2e) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = 0;
                ShowOK(0xb04da4ad, 0x7000000);
            } else {
                MemcardExit(msg);
            }
        }
        if (MemoryCard::GetInstance()->IsCheckingCardForAutoSave() ||
            MemoryCard::GetInstance()->IsAutoSaving()) {
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
    case 0xc98356ba:
        if (!m_ExpectingInput) {
            return;
        }
        {
            const char* packageName = GetPackageName();
            unsigned long handlerHash = FEHashUpper("LOADER");
            unsigned long appearHash = FEHashUpper("APPEAR");
            if (!FEngIsScriptSet(packageName, handlerHash, appearHash)) {
                return;
            }
            goto hide_loader;
        }
        break;
    }
    return;

hide_loader: {
        cFEng* feng = cFEng::Get();
        unsigned long hideHash = FEHashUpper("HIDE LOADER");
        feng->QueuePackageMessage(hideHash, GetPackageName(), nullptr);
        return;
    }
}

MenuScreen* CreateMemcardMainMenu(ScreenConstructorData* sd) {
    UIMemcardMain* screen = new UIMemcardMain(sd);
    screen->Setup();
    return screen;
}

// ===== UIMemcardList =====

UIMemcardList::UIMemcardList(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , m_SaveGameList(GetPackageName(), "", "Scrollbar", true, true, false, false) //
{
    m_Initialized = 0;
    const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
    FEPrintf(GetPackageName(), 0xeb406fec, profileName);

    for (int i = 1; i < 9; i++) {
        char buffer[32];
        ScrollerSlot* slot = new (__FILE__, __LINE__) ScrollerSlot();
        slot->pBacking = nullptr;
        slot->vSize.y = 0.0f;
        slot->vSize.x = 0.0f;
        slot->vTopLeft.y = 0.0f;
        slot->vTopLeft.x = 0.0f;
        slot->bEnabled = true;

        FEngSNPrintf(buffer, 0x20, "option_name_%d", i);
        slot->AddData(FEngFindString(GetPackageName(), FEHashUpper(buffer)));

        FEngSNPrintf(buffer, 0x20, "option_data_%d", i);
        slot->AddData(FEngFindString(GetPackageName(), FEHashUpper(buffer)));

        FEngSNPrintf(buffer, 0x20, "option_mouse_%d", i);
        slot->SetBacking(FEngFindObject(GetPackageName(), FEHashUpper(buffer)));
        slot->Hide();
        m_SaveGameList.AddSlot(slot);
    }

    m_ListOp = static_cast< int >((gMemcardSetup.mOp & 0xf0) == 0x30);
    FEngSetLanguageHash(GetPackageName(), 0x48d4fcae, sOpName[m_ListOp]);
    FEngSetLanguageHash(GetPackageName(), 0x426c7b4d, sOpName[m_ListOp]);
}

UIMemcardList::~UIMemcardList() {}

void UIMemcardList::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                         unsigned long param2) {
    switch (msg) {
    case 0x35f8620b: {
        Scrollerina& saveGameList = m_SaveGameList;
        saveGameList.SetSelected(saveGameList.Slots.GetHead());
        if (saveGameList.SelectedSlot != nullptr) {
            saveGameList.SelectedSlot->SetScript(0x249db7b7);
        }
        MemoryCard::GetInstance()->GetScreen()->m_ExpectingInput = true;
        m_Initialized++;
        if (MemoryCard::GetInstance()->InBootSequence()) {
            FEngSetLanguageHash(GetPackageName(), 0xb8a7c6cd, 0x1a294dad);
        }
        break;
    }
    case 0xc98356ba:
        if (m_Initialized == 0) {
            m_Initialized = 1;
            UIMemcardBase* parent = MemoryCard::GetInstance()->GetScreen();
            UIMemcardBase::Item* pItem = parent->m_Items.GetHead();
            while (pItem != parent->m_Items.EndOfList()) {
                int prefixLen = MemoryCard::GetInstance()->GetPrefixLength();
                const char* name = pItem->m_Name + prefixLen;
                if (parent->IsProfile(name)) {
                    AddItem(name, pItem->m_Data, pItem->m_Size, pItem->m_Flag);
                }
                pItem = pItem->GetNext();
            }
            FEngSetScript("MC_List.fng", 0x47ff4e7c, 0x13c37b, true);
        }
        break;
    case 0x911ab364:
        if (MemoryCard::GetInstance()->InBootSequence()) {
            cFEng::Get()->QueueGameMessage(0x8d0cc9f9, "MC_Main_GC.fng", 0xff);
        } else {
            cFEng::Get()->QueueGameMessage(0x8867412d,
                MemoryCard::GetInstance()->GetScreen()->GetPackageName(), 0xff);
        }
        gMemcardSetup.mLastController = param1;
        break;
    case 0x72619778:
        gMemcardSetup.mLastController = param1;
        m_SaveGameList.ScrollPrev();
        break;
    case 0x911c0a4b:
        gMemcardSetup.mLastController = param1;
        m_SaveGameList.ScrollNext();
        break;
    case 0x406415e3: {
        gMemcardSetup.mLastController = param1;
        cFrontendDatabase* database = FEDatabase;
        bool isMultitap = false;
        if (database->IsSplitScreenMode()) {
            isMultitap = database->iNumPlayers == 2;
        }
        if (!isMultitap) {
            MemoryCard* memoryCard = MemoryCard::GetInstance();
            int playerNum = memoryCard->GetPlayerNum();
            signed char port = static_cast< signed char >(FEngMapJoyParamToJoyport(static_cast< int >(param1)));
            database->SetPlayersJoystickPort(playerNum, port);
        }
        MemoryCard::GetInstance()->SetMonitor(false);
        break;
    }
    case 0xeb29392a:
        if (m_LastMsg == 0x406415e3) {
            UIMemcardBase* parent = MemoryCard::GetInstance()->GetScreen();
            parent->DoSelect(m_SaveGameList.SelectedDatum->Strings.GetNode(0)->String);
        }
        break;
    }
    m_LastMsg = msg;
}

FEMemWidget* UIMemcardList::AddItem(const char* pName, const char* pDate, int size, int flag) {
    FEMemWidget* widget = new FEMemWidget();
    widget->m_Flag = static_cast< MemCardFileFlag >(flag);
    widget->m_Size = size;
    widget->AddData(pName, 0);
    widget->AddData(pDate, 0);
    m_SaveGameList.AddData(widget);
    m_SaveGameList.Enable(widget);
    m_SaveGameList.Update(true);
    return widget;
}

MenuScreen* CreateMemcardListFiles(ScreenConstructorData* sd) {
    UIMemcardList* screen = new UIMemcardList(sd);
    MemoryCard::GetInstance()->GetScreen()->m_pChild = screen;
    return screen;
}

// ===== MemcardEnter / MemcardExit =====

void MemcardEnter(const char* from, const char* to, unsigned int op,
                  void (*termFunc)(void*), void* termParam,
                  unsigned int successMsg, unsigned int failedMsg) {
    gMemcardSetup.mOp = op;
    gMemcardSetup.mFromScreen = from;
    gMemcardSetup.mToScreen = to;
    gMemcardSetup.mTermFunc = termFunc;
    gMemcardSetup.mTermFuncParam = termParam;
    gMemcardSetup.mSuccessMsg = successMsg;
    gMemcardSetup.mFailedMsg = failedMsg;
    gMemcardSetup.mMemScreen = nullptr;
    MemoryCard::GetInstance()->ShowMessages(true);
    MemoryCard::GetInstance()->SetPlayerNum((op >> 17) & 1);
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        gMemcardSetup.mMemScreen = "MC_Main_GC.fng";
    } else {
        gMemcardSetup.mMemScreen = "InGame_MC_Main_GC.fng";
    }
    int cmd = gMemcardSetup.mOp & 0xf;
    switch (cmd) {
    case 2:
        cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mMemScreen, 0, 0, false);
        break;
    case 1:
    case 3:
        cFEng::Get()->QueuePackagePush(gMemcardSetup.mMemScreen, 0, 0, false);
        break;
    }
    MemoryCard::GetInstance()->SetMemcardScreenShowing(true);
}

void MemcardExit(unsigned int msg) {
    gMemcardSetup.mLastMessage = msg;
    if (!MemoryCard::GetInstance()->m_bInitialized) {
        cFEng* feng = cFEng::Get();
        unsigned long hash = FEHashUpper("EXIT_COMPLETE");
        feng->QueueGameMessage(hash, gMemcardSetup.mMemScreen, 0xff);
    } else {
        cFEng* feng = cFEng::Get();
        unsigned long hash = FEHashUpper("LEAVE_SCREEN");
        feng->QueuePackageMessage(hash, gMemcardSetup.mMemScreen, nullptr);
    }
    MemoryCard::GetInstance()->m_bInitialized = false;
    MemoryCard::GetInstance()->SetMemcardScreenExiting(true);
}
