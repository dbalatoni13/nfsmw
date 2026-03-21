#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern unsigned short gSaveType0[];
extern unsigned short gSaveType1[];
extern unsigned short gSaveType2[];
extern IAllocator* gMemoryAllocator;
extern MemcardCallbacks gMemcardCallbacks;

void bStrCpy(unsigned short* to, const char* from);
void bStrCpy(unsigned short* to, const unsigned short* from);
void bStrNCpy(unsigned short* to, const char* from, int n);
char* bStrCat(char* dest, const char* s1, const char* s2);

const char* GetLanguageName(eLanguages lang);
const char* GetLocalizedString(unsigned int hash);
void LOCALE_create(void* data, int param);
void LOCALE_setstate(void* data, int state, int param);
const char* LOCALE_getstrA(void* data, int strID);

bool FEngIsScriptSet(const char* pkg_name, unsigned long obj_hash, unsigned long script_hash);

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
extern RealmcIface::GameInfo* RealmcIfaceGameInfoCtorUnsignedShort(RealmcIface::GameInfo* self,
                                                                   const unsigned short* gameTitle,
                                                                   unsigned int titleId,
                                                                   bool multipleSaveTypesUsed,
                                                                   bool multitapSupported)
    asm("__Q211RealmcIface8GameInfoPCUwUibT3");
extern void RealmcIfaceMemcardInterfaceLoadUnsignedShort(RealmcIface::MemcardInterface* self,
                                                         const char* entryName,
                                                         char* header,
                                                         char* body,
                                                         const unsigned short* contentName,
                                                         const RealmcIface::TitleInfo* titleInfo)
    asm("Load__Q211RealmcIface16MemcardInterfacePCcPcT2PCUwPCQ211RealmcIface9TitleInfoT4");
#endif

void CaptureJoyOp(MemoryCardJoyLoggableEvents op) {
    Joylog::AddData(static_cast< int >(op), 8, JOYLOG_CHANNEL_MEMORY_CARD);
}

int ReplayJoyOp() {
    MemoryCardJoyLoggableEvents l_Op =
        static_cast< MemoryCardJoyLoggableEvents >(Joylog::GetData(8, JOYLOG_CHANNEL_MEMORY_CARD));
    IJoyHelper::EmulateMemoryCardLibrary(l_Op);
    return l_Op;
}

void Realmc::SystemInterface::Clear() {
    mAllocator = nullptr;
    mThread = nullptr;
    mMutex = nullptr;
    mGetStrCallback = nullptr;
}

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
RealmcIface::GameInfo::GameInfo(const wchar_t* gameTitle, unsigned int titleId,
                                bool multipleSaveTypesUsed, bool multitapSupported) {
    RealmcIfaceGameInfoCtorUnsignedShort(this, reinterpret_cast< const unsigned short * >(gameTitle),
                                         titleId, multipleSaveTypesUsed, multitapSupported);
}

void RealmcIface::MemcardInterface::Load(const char* entryName, char* header, char* body,
                                         const wchar_t* contentName,
                                         const RealmcIface::TitleInfo* titleInfo) {
    RealmcIfaceMemcardInterfaceLoadUnsignedShort(
        this, entryName, header, body, reinterpret_cast< const unsigned short * >(contentName),
        titleInfo);
}

void RealmcIface::MemcardInterface::Delete(const char* entryName, const wchar_t* contentName) {
    Delete(entryName, reinterpret_cast< const unsigned short * >(contentName));
}
#endif

void IJoyHelper::EmulateMemoryCardLibrary(int aJoyOp) {
    char* pBuf = new char[0x400];
    char* pBuf1 = pBuf + 1;
    const wchar_t* pOptions[4];
    pOptions[0] = reinterpret_cast< const wchar_t* >(pBuf + 0x338);
    pOptions[1] = reinterpret_cast< const wchar_t* >(pBuf + 0x36a);
    pOptions[2] = reinterpret_cast< const wchar_t* >(pBuf + 0x39c);
    pOptions[3] = reinterpret_cast< const wchar_t* >(pBuf + 0x3ce);
    RealmcIface::CardInfo lCardInfo;
    RealmcIface::EntryInfo lEntryInfo;
    lEntryInfo.mName = pBuf;
    switch (aJoyOp) {
    case 1:
        gMemcardCallbacks.ShowMessage(reinterpret_cast< const wchar_t* >(pBuf), 0, pOptions);
        break;
    case 2:
        gMemcardCallbacks.ClearMessage();
        break;
    case 3: {
        RealmcIface::BootupCheckResults lBootRes;
        lBootRes.Clear();
        gMemcardCallbacks.BootupCheckDone(static_cast< RealmcIface::CardStatus >(0), lBootRes);
        break;
    }
    case 4:
        gMemcardCallbacks.SaveCheckDone(static_cast< RealmcIface::TaskResult >(0),
                                        static_cast< RealmcIface::CardStatus >(0));
        break;
    case 5:
        gMemcardCallbacks.SaveDone(pBuf);
        break;
    case 6:
        gMemcardCallbacks.CheckLoadedData(pBuf);
        break;
    case 7:
        gMemcardCallbacks.LoadDone(pBuf);
        break;
    case 8:
        gMemcardCallbacks.DeleteDone(pBuf);
        break;
    case 9:
        gMemcardCallbacks.ClearEntries();
        break;
    case 10:
        gMemcardCallbacks.FoundEntry(&lEntryInfo);
        break;
    case 0xb:
        gMemcardCallbacks.FindEntriesDone(static_cast< RealmcIface::CardStatus >(0));
        break;
    case 0xc:
        gMemcardCallbacks.Retry(static_cast< RealmcIface::CardStatus >(0));
        break;
    case 0xd:
        gMemcardCallbacks.Failed(static_cast< RealmcIface::TaskResult >(0),
                                 static_cast< RealmcIface::CardStatus >(0));
        break;
    case 0xe:
        gMemcardCallbacks.CardChecked(&lCardInfo);
        break;
    case 0xf:
        gMemcardCallbacks.CardRemoved();
        break;
    case 0x10:
        gMemcardCallbacks.SetAutosaveDone(static_cast< RealmcIface::TaskResult >(0),
                                          static_cast< RealmcIface::CardStatus >(0),
                                          static_cast< RealmcIface::AutosaveState >(0));
        break;
    case 0x11:
        gMemcardCallbacks.LoadReady(pBuf, 0, 0, pBuf1, pBuf1);
        break;
    case 0x12:
        gMemcardCallbacks.SetMonitorDone(static_cast< RealmcIface::CardStatus >(0),
                                         static_cast< RealmcIface::MonitorState >(1));
        break;
    }
    if (pBuf != nullptr) {
        delete[] pBuf;
    }
}

void InitMemoryCard() {
    MemoryCard::s_pThis = new MemoryCard();
    bStrCpy(gSaveType0, "");
    bStrCpy(gSaveType1, "");
    bStrCpy(gSaveType2, "");
    bStrCpy(MemoryCardImp::gContentName, "");
    MemoryCard::s_pThis->Init();
}

MemoryCardMessage::MemoryCardMessage(const wchar_t* msg, unsigned int nOptions,
                                     const wchar_t** options) {
    bStrCpy(reinterpret_cast< unsigned short* >(mMsg),
            reinterpret_cast< const unsigned short* >(msg));
    mnOptions = nOptions;
    for (unsigned int i = 0; i < nOptions; i++) {
        bStrCpy(reinterpret_cast< unsigned short* >(mOptions[i]),
                reinterpret_cast< const unsigned short* >(options[i]));
    }
}

MemoryCard::MemoryCard() {
    m_MemOp = 0;
    m_bWaitingForResponse = false;
    m_pIMemcard = nullptr;
    m_PendingMessage = nullptr;
    m_BootupParams.Clear();
    m_Type = ST_PROFILE;
    m_bBootFoundFile = false;
    m_bAutoSave = false;
    m_bInAutoSave = false;
    m_bCheckingCardForAutoSave = false;
    m_bFoundAutoSaveFile = false;
    m_bCheckingCardForOverwrite = false;
    m_bAutoSaveRequested = false;
    m_bAutoSaveCardPulled = false;
    m_ReqOp = 0;
    m_bInBootSequence = true;
    m_bRetryBootCheck = false;
    m_bManualSave = false;
    m_bAutoSaveCardPulledDuringSave = false;
    m_bOldSaveFileExists = false;
    m_bListingOldSaveFiles = false;
    m_bMemcardScreenShowing = false;
    m_bCardRemoved = false;
    m_bRetryAutoSave = false;
    m_bInitialized = false;
    m_bDisablingAutoSaveForSave = false;
    m_bAutoLoading = false;
    m_bListingForCreate = false;
    m_bHUDLoaded = false;
    m_bCancelNextAutoSave = false;
    m_bMonitorOn = false;
    m_bAutoSaveIconShowing = false;
    m_bNeedToAllowControllerErrors = false;
    m_bNonSilentAutoSave = false;
    m_bAutoLoadDone = false;
    m_bMemcardScreenExiting = false;
    m_nPlayer = 0;
    char* pIcon = static_cast< char* >(bGetFile("GCSaveIcon.tpl", nullptr, 0));
    char* pBanner = static_cast< char* >(bGetFile("GCSaveBanner.tpl", nullptr, 0));
    GCIconDataInfo* pIconData = new GCIconDataInfo();
    m_pRMIcon = pIconData;
    pIconData->numIconFrames = 0;
    pIconData->imageData = nullptr;
    GCBannerDataInfo* pBannerData = new GCBannerDataInfo();
    m_pRMBanner = pBannerData;
    pBannerData->imageData = nullptr;
    pIconData->numIconFrames = 1;
    pIconData->imageData = pIcon;
    pIconData->animationLoop = static_cast< GCAnimationImageLoop >(0);
    pBannerData->imageData = pBanner;
    pBannerData->imageFormat = static_cast< GCImageFormat >(0);
}

bool MemoryCard::IsCardAvailable() {
    if (GetInstance()) {
        if (GetInstance()->m_LastError == 0 || GetInstance()->m_LastError == 11)
            return true;
        return false;
    }
    return false;
}

void MemoryCard::SetExtraParam(SaveType t, const char* filename, void* buf, unsigned int size) {
    if (GetInstance() == nullptr) return;
    GetInstance()->m_ReqFilename = filename;
    GetInstance()->m_Type = t;
    GetInstance()->m_pBuffer = static_cast< char* >(buf);
    GetInstance()->m_DataSize = size;
}

void MemoryCard::InitCommand(int op) {
    m_ReqOp = 0;
    m_bWaitingForResponse = false;
    m_LastError = 0;
    m_MemOp = op;
}

void MemoryCard::RequestTask(int op, const char* name) {
    m_ReqFilename = name;
    m_ReqOp = op;
}

void MemoryCard::ProcessTask() {
    if (GetScreen() == nullptr) {
        m_ReqOp = 0;
        return;
    }
    switch (m_ReqOp) {
    case MO_Delete:
        Delete(m_ReqFilename);
        break;
    case MO_Load:
        Load(m_ReqFilename);
        break;
    case MO_List:
        List(nullptr, nullptr);
        break;
    }
    m_ReqOp = 0;
}

bool MemoryCard::IsCardBusy() {
    if (GetInstance() != nullptr
        && (!GetInstance()->m_pIMemcard->IsResettable()
            || GetInstance()->IsAutoSaveIconVisible()
            || ((((void)GetInstance()->IsAutoSaving()), GetInstance()->IsAutoSaving())
                && !GetInstance()->IsWaitingForResponse())))
        return true;
    return false;
}

void MemoryCard::Init() {
    static Realmc::SystemInterface iSystem;
    static int bSystemCleared;
    static Realmc::SystemInterface* pSystem;
    if (!bSystemCleared) {
        iSystem.Clear();
        bSystemCleared = 1;
    }
    static MemoryCardImp sMemcardImp;
    if (pSystem == nullptr) {
        iSystem.mAllocator = gMemoryAllocator;
        iSystem.mThread = new (__FILE__, __LINE__) MyThread();
        MyMutex* pMutex = new (__FILE__, __LINE__) MyMutex();
        pSystem = &iSystem;
        iSystem.mMutex = pMutex;
        iSystem.mGetStrCallback = GetLocaleString;
    }
    m_pImp = &sMemcardImp;
    bStrCpy(reinterpret_cast< unsigned short* >(m_GameTitle), "Need for Speed Most Wanted");
    GameInfo* pGameInfo =
        new (__FILE__, __LINE__) GameInfo(reinterpret_cast< unsigned short* >(m_GameTitle), 0, false, false);
    m_pGameInfo = pGameInfo;
    m_pIMemcard = RealmcIface::MemcardInterface::CreateInstance(&iSystem, &gMemcardCallbacks, pGameInfo);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 1);
    m_TimeOffsetSec = 0;
    m_pLocaleFileHandler = nullptr;
}

void MemoryCard::StartBootSequence() {
    m_bInBootSequence = true;
    gMemcardSetup.mOp = 0x20;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x4000);
}

void MemoryCard::EndBootSequence() {
    m_bInBootSequence = false;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 0x4000);
}

void MemoryCard::LoadLocale(eLanguages eLang) {
    if (s_pThis == nullptr) return;
    char sPath[64];
    bStrCpy(sPath, "FRONTEND/MC_");
    if (eLang <= eLANGUAGE_LABELS) {
        if (eLang < eLANGUAGE_LARGEST) {
            goto lang_code;
        }
        bStrCat(sPath, sPath, "English.bin");
    } else {
    lang_code:
        const char* langName = GetLanguageName(eLang);
        bStrCat(sPath, sPath, langName);
        bStrCat(sPath, sPath, ".bin");
    }
    MemoryCard* pThis = s_pThis;
    if (pThis->m_pLocaleFileHandler == nullptr)
        pThis->m_pLocaleFileHandler = bMalloc(0x2000, 0);
    unsigned int currentsize = bFileSize(sPath);
    bFile* file = bOpen(sPath, 1, 1);
    bRead(file, s_pThis->m_pLocaleFileHandler, currentsize);
    bClose(file);
    LOCALE_create(s_pThis->m_pLocaleFileHandler, 1);
    LOCALE_setstate(s_pThis->m_pLocaleFileHandler, 0, 0);
    unsigned short* dest = gSaveType0;
    const char* str = GetLocalizedString(0xe6f55df0);
    bStrCpy(dest, str);
}

int MemoryCard::GetPrefixLength() { return bStrLen(m_pImp->GetPrefix()); }
const char* MemoryCard::GetPrefix() { return m_pImp->GetPrefix(); }
const char* MemoryCard::GetLocaleString(int strID) { return LOCALE_getstrA(GetInstance()->m_pLocaleFileHandler, strID); }

void MemoryCard::SetMessageMode(unsigned int msg, bool flag) {
    if (GetInstance() != nullptr)
        GetInstance()->m_pIMemcard->SetMessage(flag ? RealmcIface::MESSAGE_SHOW : RealmcIface::MESSAGE_HIDE, msg);
}

void MemoryCard::Tick(int TickCount) {
    if (m_MemOp == 0 && m_ReqOp != 0) ProcessTask();
    if (m_bAutoSaveRequested && m_bHUDLoaded && GManager::Exists() && !GManager::Get().GetHasPendingSMS()) {
        m_bHUDLoaded = false;
        m_bAutoSaveRequested = false;
        StartAutoSave(false);
    }
    if (Joylog::IsReplaying()) {
        MemoryCardJoyLoggableEvents l_JoyOp;
        do { l_JoyOp = static_cast< MemoryCardJoyLoggableEvents >(ReplayJoyOp()); } while (l_JoyOp != 0);
    } else {
        m_pIMemcard->Update(TickCount);
        if (Joylog::IsCapturing()) CaptureJoyOp(MJ_None);
    }
    if (FEDatabase == nullptr) return;
    if (FEDatabase->IsOptionsMode()) return;
    if (cFEng::Get()->IsPackagePushed("ScreenPrintf")
        || cFEng::Get()->IsPackagePushed("MemoryCard.fng")
        || IsAutoSaveIconVisible()) {
        if (!FEManager::Get()->IsAllowingControllerError() && !TheGameFlowManager.IsInGame()) return;
        if (cFEng::Get()->IsPackagePushed("IG_Pause.fng") || cFEng::Get()->IsPackagePushed("AutoSaveIcon.fng"))
            m_bNonSilentAutoSave = true;
        m_bNeedToAllowControllerErrors = true;
        FEManager::Get()->AllowControllerError(false);
        FEManager::Get()->SuppressControllerError(true);
    } else {
        if (!m_bNeedToAllowControllerErrors) return;
        m_bNeedToAllowControllerErrors = false;
        if (FEManager::Get()->IsAllowingControllerError()) return;
        if (m_bNonSilentAutoSave) { m_bNonSilentAutoSave = false; return; }
        FEManager::Get()->AllowControllerError(true);
        FEManager::Get()->SuppressControllerError(false);
    }
}

void MemoryCard::MessageDone(RealmcIface::MessageChoices nInput) {
    if (m_bWaitingForResponse) {
        m_pIMemcard->MessageDone(nInput);
        m_bWaitingForResponse = false;
    }
}

void MemoryCard::BootupCheck(const char* entry) {
    bStrCpy(m_BootupFilename, "");
    m_pImp->ConstructSaveInfo(ST_PROFILE, "", FEDatabase->GetUserProfileSaveSize(false));
    m_BootupParams.mEntryNamePattern = m_BootupFilename;
    m_BootupParams.mSaveReqs = reinterpret_cast< RealmcIface::SaveReq** >(m_pImp->GetSaveReqArray());
    m_BootupParams.mNumSaveTypes = 1;
    m_BootupParams.mValidCardIds = 1;
    InitCommand(MO_BootUp);
    if (!Joylog::IsReplaying())
        m_pIMemcard->BootupCheck(&m_BootupParams, 0, static_cast< const char** >(nullptr), static_cast< wchar_t* >(nullptr));
}

bool MemoryCard::ShouldDoAutoSave(bool bForce) {
    if (bForce) return true;
    if (m_bCancelNextAutoSave) { m_bCancelNextAutoSave = false; return false; }
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) return false;
    if (!IsMemcardEnabled || !IsAutoSaveEnabled) return false;
    if (FEDatabase->GetGameplaySettings()->AutoSaveOn || m_bCardRemoved) {
        if (!FEDatabase->IsFinalEpicChase() && GRaceStatus::Exists()
            && GRaceStatus::Get().GetRaceParameters() != nullptr
            && GRaceStatus::Get().GetRaceParameters()->GetIsBossRace())
            return false;
        return true;
    }
    return false;
}

void MemoryCard::StartAutoSave(bool bForce) {
    if (!ShouldDoAutoSave(bForce)) return;
    if (!FEDatabase->bProfileLoaded) return;
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) != 0xb0) {
        ShowAutoSaveIcon();
        gMemcardSetup.mOp = 0;
    }
    if (m_bCardRemoved) { HandleAutoSaveError(); }
    else {
        m_bInAutoSave = true;
        m_bCheckingCardForAutoSave = true;
        FEManager::Get()->SuppressControllerError(true);
        ShowMessages(false);
        CheckCard(0);
    }
}

void MemoryCard::DoAutoSave() {
    m_bCheckingCardForAutoSave = false;
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) == 0xb0) {
        ShowMessages(true);
        m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x100);
    } else { ShowOnlyAutoSaveMessages(); }
    Save(FEDatabase->GetUserProfile(0)->GetProfileName());
}

void MemoryCard::EndAutoSave() {
    if (!m_bRetryAutoSave) m_MemOp = 0;
    m_bCheckingCardForAutoSave = false;
    m_bCheckingCardForOverwrite = false;
    m_bInAutoSave = false;
    FEManager::Get()->SuppressControllerError(false);
    ShowMessages(true);
    HideAutoSaveIcon();
}

void MemoryCard::StartListingOldSaveFiles() { m_bListingOldSaveFiles = true; ListOldSaveFilesNGC(); }

void MemoryCard::EndListingOldSaveFiles() {
    m_bListingOldSaveFiles = false;
    if (m_bOldSaveFileExists) {
        cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
        DialogInterface::ShowOneButton("", "", static_cast< eDialogTitle >(2), 0x417b2601, 0x34dc1bec, 0xc5e2beac);
    }
    FEDatabase->GetCareerSettings()->AwardOneTimeCashBonus(m_bOldSaveFileExists);
}

void MemoryCard::SetMonitor(bool bEnabled) {
    InitCommand(MO_SetMonitor);
    if (!Joylog::IsReplaying())
        m_pIMemcard->SetMonitor(bEnabled ? RealmcIface::MONITOR_ON : RealmcIface::MONITOR_OFF);
    if (!bEnabled && Joylog::IsReplaying()) ReplayJoyOp();
}

void MemoryCard::SetAutoSaveEnabled(bool bEnabled) {
    char entryname[16];
    bStrCpy(entryname, FEDatabase->GetMultiplayerProfile(0)->GetProfileName());
    SetExtraParam(ST_PROFILE, entryname, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    bStrCat(m_Filename, m_pImp->GetPrefix(), entryname);
    bStrNCpy(MemoryCardImp::gContentName, entryname, 16);
    if (GetScreen() && gMemcardSetup.GetCommand() == 0xa0) {
        GetScreen()->SetStringCheckingCard();
        ShowMessages(true);
    } else {
        ShowMessages(false);
    }
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 1);
    if (bEnabled) {
        gMemcardSetup.mPreviousCommand = gMemcardSetup.mOp & 0xf0;
        gMemcardSetup.ClearCommand();
        gMemcardSetup.SetCommand(0xa0);
    } else {
        m_bDisablingAutoSaveForSave = true;
    }
    InitCommand(MO_AutoSave);
    if (!Joylog::IsReplaying())
        m_pIMemcard->SetAutosave(bEnabled ? RealmcIface::AUTOSAVE_ENABLE : RealmcIface::AUTOSAVE_DISABLE, 0, nullptr, entryname, RealmcIface::CARD_UNKNOWN);
    if (!bEnabled && Joylog::IsReplaying()) ReplayJoyOp();
}

void MemoryCard::ShowOnlyAutoSaveMessages() {
    m_bManualSave = false;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 2);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 4);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 0x800);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 1);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x100);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x200);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x400);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x1000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x2000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x4000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x8000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x10000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x20000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x40000);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x80000);
}

void MemoryCard::ShowMessages(bool bShow) {
    m_bManualSave = bShow;
    m_pIMemcard->SetMessage(bShow ? RealmcIface::MESSAGE_SHOW : RealmcIface::MESSAGE_HIDE, 0xffffffff);
}

void MemoryCard::CheckCard(int iSlot) {
    RealmcIface::CardId id;
    id = RealmcIface::CARD_UNKNOWN;
    InitCommand(MO_CheckCard);
    if (!Joylog::IsReplaying()) m_pIMemcard->CheckCard(id);
}

void MemoryCard::Save(const char* entryName) {
    SetExtraParam(ST_PROFILE, entryName, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    if (m_pImp->GetSaveInfo() == nullptr) {
        m_pImp->ConstructSaveInfo(ST_PROFILE, entryName, GetSize());
        bStrCat(m_Filename, m_pImp->GetPrefix(), entryName);
    }
    bStrNCpy(MemoryCardImp::gContentName, entryName, 16);
    m_pBuffer = static_cast< char* >(bMalloc(GetSize(), nullptr, 0, 0x40));
    FEDatabase->SaveUserProfileToBuffer(GetData(), GetSize());
    m_Header[0] = 0x10d;
    m_Header[1] = GetSize();
    InitCommand(MO_Save);
    if (!Joylog::IsReplaying())
        m_pIMemcard->Save(m_Filename, GetHeader(), GetData(),
                          reinterpret_cast< const RealmcIface::SaveInfo* >(m_pImp->GetSaveInfo()),
                          static_cast< const RealmcIface::TitleInfo* >(nullptr));
}

void MemoryCard::List(const char* filter, RealmcIface::TitleInfo* titleInfo) {
    SetExtraParam(ST_PROFILE, nullptr, nullptr, 0);
    m_EntryCount = 0;
    const char* prefix = m_pImp->GetPrefix();
    bStrCat(m_Filename, prefix, "*");
    InitCommand(MO_List);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->FindEntries(filter != nullptr ? filter : m_Filename, titleInfo);
    } else { ReplayJoyOp(); }
}

void MemoryCard::Load(const char* filename) {
    SetExtraParam(ST_PROFILE, filename, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    FEDatabase->AllocBackupDB(true);
    m_pBuffer = static_cast< char* >(bMalloc(m_DataSize, nullptr, 0, 0x40));
    if (filename != nullptr) {
        bStrNCpy(MemoryCardImp::gContentName, filename, 16);
        bStrCat(m_Filename, m_pImp->GetPrefix(), filename);
    }
    InitCommand(MO_Load);
    if (!Joylog::IsReplaying()) {
        if (InBootSequence()) {
            m_bAutoLoading = true;
            BootupCheck(filename);
        } else {
            m_pIMemcard->Load(m_Filename, static_cast< char* >(nullptr), static_cast< char* >(nullptr),
                              reinterpret_cast< const wchar_t* >(MemoryCardImp::gContentName),
                              static_cast< const RealmcIface::TitleInfo* >(nullptr));
        }
    }
}

void MemoryCard::Delete(const char* filename) {
    InitCommand(MO_Delete);
    if (filename != nullptr) {
        bStrNCpy(MemoryCardImp::gContentName, filename, 16);
        bStrCat(m_Filename, m_pImp->GetPrefix(), filename);
    }
    if (!Joylog::IsReplaying())
        m_pIMemcard->Delete(m_Filename, reinterpret_cast< const wchar_t* >(MemoryCardImp::gContentName));
}

void MemoryCard::ListOldSaveFilesNGC() {
    RealmcIface::TitleInfo titleInfo;
    titleInfo.Init(
        static_cast< RealmcIface::TitleType >(1),
        0,
        static_cast< RealmcIface::NameType >(0),
        static_cast< RealmcIface::DataFormat >(0));
    GetInstance()->ShowMessages(false);
    List("NFSMW*", &titleInfo);
}

void MemoryCard::ReleasePendingMessage() {
    if (m_PendingMessage != nullptr) { delete m_PendingMessage; m_PendingMessage = nullptr; }
}

void MemoryCard::HandleAutoSaveError() {
    UIMemcardBase* pScreen = GetScreen();
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) == 0xb0 || pScreen != nullptr)
        pScreen->HandleAutoSaveError();
    else
        MemcardEnter(nullptr, nullptr, 0x91, nullptr, nullptr, 0, 0);
}

void MemoryCard::HandleAutoSaveOverwriteMessage() {
    UIMemcardBase* pScreen = GetScreen();
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) == 0xb0 || pScreen != nullptr)
        pScreen->HandleAutoSaveOverwriteMessage();
    else
        MemcardEnter(nullptr, nullptr, 0xd1, nullptr, nullptr, 0, 0);
}

void MemoryCard::ShowAutoSaveIcon() {
    if (m_bAutoSaveIconShowing) return;
    m_bAutoSaveIconShowing = true;
    if (!cFEng::Get()->IsPackagePushed("AutoSaveIcon.fng"))
        cFEng::Get()->PushNoControlPackage("AutoSaveIcon.fng", static_cast< FE_PACKAGE_PRIORITY >(0x68));
    cFEng* feng = cFEng::Get();
    unsigned int msg = FEHashUpper("FadeIn");
    feng->QueuePackageMessage(msg, "AutoSaveIcon.fng", nullptr);
    bool bWidescreen = FEDatabase->GetVideoSettings()->WideScreen;
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr
        && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
        const char* script;
        if (bWidescreen) script = "SAVE_DDAY_16_9";
        else script = "SAVE_DDAY_4_3";
        msg = FEHashUpper(script);
    } else {
        if (cFEng::Get()->IsPackagePushed("SMS_HUD.fng") || GManager::Get().GetHasPendingSMS()) {
            unsigned int hideMsg = FEHashUpper("HideSMSIcon");
            cFEng::Get()->QueuePackageMessage(hideMsg, nullptr, nullptr);
            goto queue;
        }
        const char* script;
        if (bWidescreen) script = "SAVE_REG_16_9";
        else script = "SAVE_REG_4_3";
        msg = FEHashUpper(script);
    }
queue:
    cFEng::Get()->QueuePackageMessage(msg, "AutoSaveIcon.fng", nullptr);
}

void MemoryCard::HideAutoSaveIcon() {
    if (m_bAutoSaveIconShowing) {
        m_bAutoSaveIconShowing = false;
        cFEng::Get()->QueuePackageMessage(FEHashUpper("FadeOut"), "AutoSaveIcon.fng", nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("ShowSMSIcon"), nullptr, nullptr);
    }
}

bool MemoryCard::IsAutoSaveIconVisible() {
    if (!m_bAutoSaveIconShowing) {
        const char *pkg = "AutoSaveIcon.fng";
        const char *iconName = "AUTOSAVE_ICON";
        unsigned int obj = FEHashUpper(iconName);
        unsigned int script1 = FEHashUpper("FadeIn");
        if (!FEngIsScriptSet(pkg, obj, script1)) {
            obj = FEHashUpper(iconName);
            unsigned int script2 = FEHashUpper("Idle");
            if (!FEngIsScriptSet(pkg, obj, script2))
                return false;
        }
    }
    return true;
}
