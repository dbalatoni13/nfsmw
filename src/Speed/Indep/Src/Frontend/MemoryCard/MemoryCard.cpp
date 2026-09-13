#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "realmemcard/3.04.01-layer2/include/common/realmemcard/memcard_interface.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardImp.hpp"

// TODO d:/packages/realcore/6.24.00/source/std/cmn/locale.cpp
void LOCALE_create(void *data, int param);
enum LOCALESTATE {
    LOCALE_LANGUAGEID = 0,
    LOCALE_LANGUAGECOUNT = 1,
    LOCALE_STRINGCOUNT = 2,
    LOCALE_HASINDEX = 3,
};
void LOCALE_setstate(void *data, LOCALESTATE state, int param);
const char *LOCALE_getstrA(void *data, int strID);

// TODO move
void Realmc::SystemInterface::Clear() {
    mAllocator = nullptr;
    mThread = nullptr;
    mMutex = nullptr;
    mGetStrCallback = nullptr;
}

extern IAllocator *gMemoryAllocator;
extern MemcardCallbacks gMemcardCallbacks;

MemoryCard *MemoryCard::s_pThis = nullptr;

void CaptureJoyOp(MemoryCardJoyLoggableEvents op) {
    Joylog::AddData(static_cast<int>(op), 8, JOYLOG_CHANNEL_MEMORY_CARD);
}

MemoryCardJoyLoggableEvents ReplayJoyOp() {
    MemoryCardJoyLoggableEvents l_Op = static_cast<MemoryCardJoyLoggableEvents>(Joylog::GetData(8, JOYLOG_CHANNEL_MEMORY_CARD));
    IJoyHelper::EmulateMemoryCardLibrary(l_Op);
    return l_Op;
}

// TODO: these expect to be wchar_t and unsigned wchar_t
void RealmcIface::MemcardInterface::Delete(const char *entryName, const wchar_t *contentName) {
    RealmcIface::MemcardInterface::Delete(entryName, reinterpret_cast<const unsigned short *>(contentName));
}

RealmcIface::GameInfo::GameInfo(const wchar_t *gameTitle, unsigned int titleId, bool multipleSaveTypesUsed, bool multitapSupported)
// : RealmcIface::GameInfo::GameInfo(reinterpret_cast<const unsigned short *>(gameTitle), titleId, multipleSaveTypesUsed, multitapSupported)
{}

void RealmcIface::MemcardInterface::Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const TitleInfo *titleInfo) {}

void InitMemoryCard() {
    MemoryCard::s_pThis = new ("MemoryCard", 0) MemoryCard();
    bStrCpy(gSaveType0, "");
    bStrCpy(gSaveType1, "");
    bStrCpy(gSaveType2, "");
    bStrCpy(MemoryCardImp::gContentName, "NFSMWSD");
    MemoryCard::s_pThis->Init();
}

MemoryCardMessage::MemoryCardMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t **options) {
    bStrCpy(reinterpret_cast<uint16 *>(mMsg), reinterpret_cast<const uint16 *>(msg));
    mnOptions = nOptions;
    for (uint32 i = 0; i < nOptions; i++) {
        bStrCpy(reinterpret_cast<uint16 *>(mOptions[i]), reinterpret_cast<const uint16 *>(options[i]));
    }
}

// UNSOLVED
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
    char *pIcon = static_cast<char *>(bGetFile("memcard/icon1.raw", nullptr, 0));
    char *pBanner = static_cast<char *>(bGetFile("memcard/banner.raw", nullptr, 0));
    m_pRMIcon = new ("GCIconDataInfo", 0) GCIconDataInfo();
    m_pRMIcon->numIconFrames = 0;
    m_pRMIcon->imageData = nullptr;
    m_pRMBanner = new ("GCBannerDataInfo", 0) GCBannerDataInfo();
    m_pRMIcon->numIconFrames = 1;
    m_pRMIcon->imageData = pIcon;
    m_pRMIcon->animationLoop = GC_ANIMATION_LOOP_NONE;
    m_pRMBanner->imageData = nullptr;
    m_pRMBanner->imageData = pBanner;
    m_pRMBanner->imageFormat = GC_IMAGE_FORMAT_CI8;
}

bool MemoryCard::IsCardAvailable() {
    if (GetInstance() != nullptr) {
        if (GetInstance()->m_LastError == 0 || GetInstance()->m_LastError == 11)
            return true;
        return false;
    }
    return false;
}

void MemoryCard::SetExtraParam(SaveType t, const char *filename, void *buf, unsigned int size) {
    if (GetInstance() == nullptr)
        return;
    GetInstance()->m_ReqFilename = filename;
    GetInstance()->m_Type = t;
    GetInstance()->m_pBuffer = static_cast<char *>(buf);
    GetInstance()->m_DataSize = size;
}

void MemoryCard::InitCommand(int op) {
    m_ReqOp = 0;
    m_bWaitingForResponse = false;
    m_LastError = 0;
    m_MemOp = op;
}

void MemoryCard::RequestTask(int op, const char *name) {
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
    if (GetInstance() != nullptr &&
        (!GetInstance()->m_pIMemcard->IsResettable() || GetInstance()->IsAutoSaveIconVisible() ||
         ((((void)GetInstance()->IsAutoSaving()), GetInstance()->IsAutoSaving()) && !GetInstance()->IsWaitingForResponse())))
        return true;
    return false;
}

// UNSOLVED
void MemoryCard::Init() {
    static Realmc::SystemInterface iSystem;
    static Realmc::SystemInterface *pSystem;
    static MemoryCardImp sMemcardImp;
    if (pSystem == nullptr) {
        iSystem.mAllocator = gMemoryAllocator;
        iSystem.mThread = new ("MemcardThread", 0) MyThread();
        MyMutex *pMutex = new ("MemcardMutex", 0) MyMutex();
        pSystem = &iSystem;
        iSystem.mMutex = pMutex;
        iSystem.mGetStrCallback = GetLocaleString;
    }
    m_pImp = &sMemcardImp;
    bStrCpy(reinterpret_cast<unsigned short *>(m_GameTitle), "Need"
                                                             "\xA0"
                                                             "for"
                                                             "\xA0"
                                                             "Speed"
                                                             "\x99"
                                                             "\xA0"
                                                             "Most"
                                                             "\xA0"
                                                             "Wanted");
    GameInfo *pGameInfo = new ("GameInfo", 0) GameInfo(reinterpret_cast<unsigned short *>(m_GameTitle), 0, false, false);
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
    if (GetInstance() == nullptr)
        return;
    char sPath[64];
    bStrCpy(sPath, "memcard/LOCALE_");
    switch (eLang) {
        case eLANGUAGE_LARGEST:
        case eLANGUAGE_LABELS:
            bStrCat(sPath, sPath, "ENGLISH.loc");
            break;
        default:
            bStrCat(sPath, sPath, GetLanguageName(eLang));
            bStrCat(sPath, sPath, ".loc");
            break;
    }

    if (GetInstance()->m_pLocaleFileHandler == nullptr)
        GetInstance()->m_pLocaleFileHandler = bMalloc(0x2000, "Locale buffer", __LINE__, 0);
    unsigned int currentsize = bFileSize(sPath);
    bFile *file = bOpen(sPath, 1, 1);
    bRead(file, GetInstance()->m_pLocaleFileHandler, currentsize);
    bClose(file);
    LOCALE_create(GetInstance()->m_pLocaleFileHandler, 1);
    LOCALE_setstate(GetInstance()->m_pLocaleFileHandler, LOCALE_LANGUAGEID, 0);
    bStrCpy(gSaveType0, GetLocalizedString(0xe6f55df0));
}

int MemoryCard::GetPrefixLength() {
    return bStrLen(m_pImp->GetPrefix());
}
const char *MemoryCard::GetPrefix() {
    return m_pImp->GetPrefix();
}
const char *MemoryCard::GetLocaleString(int strID) {
    return LOCALE_getstrA(GetInstance()->m_pLocaleFileHandler, strID);
}

void MemoryCard::SetMessageMode(unsigned int msg, bool flag) {
    if (GetInstance() != nullptr)
        GetInstance()->m_pIMemcard->SetMessage(flag ? RealmcIface::MESSAGE_SHOW : RealmcIface::MESSAGE_HIDE, msg);
}

void MemoryCard::Tick(int TickCount) {
    if (m_MemOp == 0 && m_ReqOp != 0)
        ProcessTask();
    if (m_bAutoSaveRequested && m_bHUDLoaded && GManager::Exists() && !GManager::Get().GetHasPendingSMS()) {
        m_bHUDLoaded = false;
        m_bAutoSaveRequested = false;
        StartAutoSave(false);
    }
    if (Joylog::IsReplaying()) {
        MemoryCardJoyLoggableEvents l_JoyOp;
        do {
            l_JoyOp = static_cast<MemoryCardJoyLoggableEvents>(ReplayJoyOp());
        } while (l_JoyOp != 0);
    } else {
        m_pIMemcard->Update(TickCount);
        if (Joylog::IsCapturing())
            CaptureJoyOp(MJ_None);
    }
    if (FEDatabase == nullptr)
        return;
    if (FEDatabase->IsOptionsMode())
        return;
    if (cFEng::Get()->IsPackagePushed("ScreenPrintf") || cFEng::Get()->IsPackagePushed("MemoryCard.fng") || IsAutoSaveIconVisible()) {
        if (!FEManager::Get()->IsAllowingControllerError() && !TheGameFlowManager.IsInGame())
            return;
        if (cFEng::Get()->IsPackagePushed("IG_Pause.fng") || cFEng::Get()->IsPackagePushed("AutoSaveIcon.fng"))
            m_bNonSilentAutoSave = true;
        m_bNeedToAllowControllerErrors = true;
        FEManager::Get()->AllowControllerError(false);
        FEManager::Get()->SuppressControllerError(true);
    } else {
        if (!m_bNeedToAllowControllerErrors)
            return;
        m_bNeedToAllowControllerErrors = false;
        if (FEManager::Get()->IsAllowingControllerError())
            return;
        if (m_bNonSilentAutoSave) {
            m_bNonSilentAutoSave = false;
            return;
        }
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

void MemoryCard::BootupCheck(const char *entry) {
    bStrCpy(m_BootupFilename, "");
    m_pImp->ConstructSaveInfo(ST_PROFILE, "", FEDatabase->GetUserProfileSaveSize(false));
    m_BootupParams.mEntryNamePattern = m_BootupFilename;
    m_BootupParams.mSaveReqs = reinterpret_cast<RealmcIface::SaveReq **>(m_pImp->GetSaveReqArray());
    m_BootupParams.mNumSaveTypes = 1;
    m_BootupParams.mValidCardIds = 1;
    InitCommand(MO_BootUp);
    if (!Joylog::IsReplaying())
        m_pIMemcard->BootupCheck(&m_BootupParams, 0, static_cast<const char **>(nullptr), static_cast<wchar_t *>(nullptr));
}

bool MemoryCard::ShouldDoAutoSave(bool bForce) {
    if (bForce) {
        return true;
    }
    if (m_bCancelNextAutoSave) {
        m_bCancelNextAutoSave = false;
        return false;
    }
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        return false;
    }
    if (!IsMemcardEnabled || !IsAutoSaveEnabled) {
        return false;
    }
    if (!FEDatabase->IsAutoSave() && !m_bCardRemoved) {
        return false;
    }

    if (!FEDatabase->IsFinalEpicChase() && GRaceStatus::Exists()) {
        GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
        if (raceParms != nullptr && raceParms->GetIsBossRace()) {
            return false;
        }
    }
    return true;
}

void MemoryCard::StartAutoSave(bool bForce) {
    if (!ShouldDoAutoSave(bForce)) {
        return;
    }
    if (!FEDatabase->bProfileLoaded) {
        return;
    }
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) != 0xb0) {
        ShowAutoSaveIcon();
        gMemcardSetup.mOp = 0;
    }
    if (m_bCardRemoved) {
        HandleAutoSaveError();
    } else {
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
    } else {
        ShowOnlyAutoSaveMessages();
    }
    Save(FEDatabase->GetUserProfile(0)->GetProfileName());
}

void MemoryCard::EndAutoSave() {
    if (!m_bRetryAutoSave) {
        m_MemOp = 0;
    }
    m_bCheckingCardForAutoSave = false;
    m_bCheckingCardForOverwrite = false;
    m_bInAutoSave = false;
    FEManager::Get()->SuppressControllerError(false);
    ShowMessages(true);
    HideAutoSaveIcon();
}

void MemoryCard::StartListingOldSaveFiles() {
    m_bListingOldSaveFiles = true;
    ListOldSaveFilesNGC();
}

void MemoryCard::EndListingOldSaveFiles() {
    m_bListingOldSaveFiles = false;
    if (m_bOldSaveFileExists) {
        cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
        DialogInterface::ShowOneButton("", "", dialog_info, 0x417b2601, 0x34dc1bec, 0xc5e2beac);
    }
    FEDatabase->GetCareerSettings()->AwardOneTimeCashBonus(m_bOldSaveFileExists);
}

void MemoryCard::SetMonitor(bool bEnabled) {
    InitCommand(MO_SetMonitor);
    if (!Joylog::IsReplaying())
        m_pIMemcard->SetMonitor(bEnabled ? RealmcIface::MONITOR_ON : RealmcIface::MONITOR_OFF);
    if (!bEnabled && Joylog::IsReplaying())
        ReplayJoyOp();
}

void MemoryCard::SetAutoSaveEnabled(bool bEnabled) {
    char entryname[16];
    bStrCpy(entryname, FEDatabase->GetMultiplayerProfile(0)->GetProfileName());
    SetExtraParam(ST_PROFILE, entryname, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    bStrCat(m_Filename, m_pImp->GetPrefix(), entryname);
    bStrNCpy(MemoryCardImp::gContentName, entryname, 16);
    if ((GetScreen() != nullptr) && gMemcardSetup.GetCommand() == 0xa0) {
        GetScreen()->SetStringCheckingCard();
        ShowMessages(true);
    } else {
        ShowMessages(false);
    }
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 1);
    if (bEnabled) {
        gMemcardSetup.ClearCommand();
        gMemcardSetup.SetCommand(0xa0);
    } else {
        m_bDisablingAutoSaveForSave = true;
    }
    InitCommand(MO_AutoSave);
    if (!Joylog::IsReplaying())
        m_pIMemcard->SetAutosave(bEnabled ? RealmcIface::AUTOSAVE_ENABLE : RealmcIface::AUTOSAVE_DISABLE, 0, nullptr, entryname,
                                 RealmcIface::CARD_UNKNOWN);
    if (!bEnabled && Joylog::IsReplaying())
        ReplayJoyOp();
}

void MemoryCard::ShowOnlyAutoSaveMessages() {
    m_bManualSave = false;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, RealmcIface::ID_NOCARD);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, RealmcIface::ID_BADCARD);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, RealmcIface::ID_SAVE_FAILED);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_CHECKINGFORCARD);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_SAVE_READY);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_SAVE_WARNING);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_SAVE_COMPLETE);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_LOAD_READY);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_LOAD_WARNING);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_LOAD_COMPLETE);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_LOAD_FAILED);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_DELETE_READY);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_DELETE_WARNING);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_DELETE_COMPLETE);
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, RealmcIface::ID_DELETE_FAILED);
}

void MemoryCard::ShowMessages(bool bShow) {
    m_bManualSave = bShow;
    m_pIMemcard->SetMessage(bShow ? RealmcIface::MESSAGE_SHOW : RealmcIface::MESSAGE_HIDE, 0xffffffff);
}

void MemoryCard::CheckCard(int iSlot) {
    RealmcIface::CardId id;
    id = RealmcIface::CARD_UNKNOWN;
    InitCommand(MO_CheckCard);
    if (!Joylog::IsReplaying())
        m_pIMemcard->CheckCard(id);
}

void MemoryCard::Save(const char *entryName) {
    SetExtraParam(ST_PROFILE, entryName, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    if (m_pImp->GetSaveInfo() == nullptr) {
        m_pImp->ConstructSaveInfo(ST_PROFILE, entryName, GetSize());
        bStrCat(m_Filename, m_pImp->GetPrefix(), entryName);
    }
    bStrNCpy(MemoryCardImp::gContentName, entryName, 16);
    m_pBuffer = static_cast<char *>(bMalloc(GetSize(), "Save Buffer", __LINE__, BMEMORY_TOP_BIT));
    FEDatabase->SaveUserProfileToBuffer(GetData(), GetSize());
    m_Header[0] = 0x10d;
    m_Header[1] = GetSize();
    InitCommand(MO_Save);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->Save(m_Filename, GetHeader(), GetData(), reinterpret_cast<const RealmcIface::SaveInfo *>(m_pImp->GetSaveInfo()),
                          static_cast<const RealmcIface::TitleInfo *>(nullptr));
    }
}

void MemoryCard::List(const char *filter, RealmcIface::TitleInfo *titleInfo) {
    SetExtraParam(ST_PROFILE, nullptr, nullptr, 0);
    m_EntryCount = 0;
    bStrCat(m_Filename, m_pImp->GetPrefix(), "*");
    InitCommand(MO_List);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->FindEntries(filter != nullptr ? filter : m_Filename, titleInfo);
    } else {
        ReplayJoyOp();
    }
}

void MemoryCard::Load(const char *filename) {
    SetExtraParam(ST_PROFILE, filename, nullptr, FEDatabase->GetUserProfileSaveSize(false));
    FEDatabase->AllocBackupDB(true);
    m_pBuffer = static_cast<char *>(bMalloc(m_DataSize, "Load Buffer", __LINE__, 0x40));
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
            m_pIMemcard->Load(m_Filename, nullptr, nullptr, reinterpret_cast<const wchar_t *>(MemoryCardImp::gContentName),
                              static_cast<const RealmcIface::TitleInfo *>(nullptr));
        }
    }
}

void MemoryCard::Delete(const char *filename) {
    InitCommand(MO_Delete);
    if (filename != nullptr) {
        bStrNCpy(MemoryCardImp::gContentName, filename, 16);
        bStrCat(m_Filename, m_pImp->GetPrefix(), filename);
    }
    if (!Joylog::IsReplaying())
        m_pIMemcard->Delete(m_Filename, reinterpret_cast<const wchar_t *>(MemoryCardImp::gContentName));
}

void MemoryCard::ListOldSaveFilesNGC() {
    RealmcIface::TitleInfo titleInfo;
    titleInfo.Init(RealmcIface::TITLE_ALTERNATE, 0, RealmcIface::NAME_ENTRY, RealmcIface::FORMAT_LAYER2);
    GetInstance()->ShowMessages(false);
    List("NFSU2*", &titleInfo);
}

void MemoryCard::ReleasePendingMessage() {
    if (m_PendingMessage != nullptr) {
        delete m_PendingMessage;
        m_PendingMessage = nullptr;
    }
}

void MemoryCard::HandleAutoSaveError() {
    UIMemcardBase *pScreen = GetScreen();
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) == 0xb0 || pScreen != nullptr)
        pScreen->HandleAutoSaveError();
    else
        MemcardEnter(nullptr, nullptr, 0x91, nullptr, nullptr, 0, 0);
}

void MemoryCard::HandleAutoSaveOverwriteMessage() {
    UIMemcardBase *pScreen = GetScreen();
    if ((((void)gMemcardSetup.GetCommand()), gMemcardSetup.mOp & 0xf0) == 0xb0 || pScreen != nullptr)
        pScreen->HandleAutoSaveOverwriteMessage();
    else
        MemcardEnter(nullptr, nullptr, 0xd1, nullptr, nullptr, 0, 0);
}

void MemoryCard::ShowAutoSaveIcon() {
    if (m_bAutoSaveIconShowing)
        return;
    m_bAutoSaveIconShowing = true;
    if (!cFEng::Get()->IsPackagePushed("Autosave_Overlay.fng")) {
        cFEng::Get()->PushNoControlPackage("Autosave_Overlay.fng", FE_PACKAGE_PRIORITY_CLOSEST);
    }
    cFEng::Get()->QueuePackageMessage(FEHashUpper("SAVE"), "Autosave_Overlay.fng", nullptr);
    uint32 msg;
    bool bWidescreen = FEDatabase->GetVideoSettings()->WideScreen;
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
        msg = FEHashUpper(bWidescreen ? "LEFT_WIDE" : "LEFT_NORM");
    } else {
        if (cFEng::Get()->IsPackagePushed("SMS_MailBoxes.fng") || GManager::Get().GetHasPendingSMS()) {
            msg = FEHashUpper("SMS_MAILBOX");
            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE_INCOMING"), nullptr, nullptr);
        } else {
            msg = FEHashUpper(bWidescreen ? "RIGHT_WIDE" : "RIGHT_NORM");
        }
    }
    cFEng::Get()->QueuePackageMessage(msg, "Autosave_Overlay.fng", nullptr);
}

void MemoryCard::HideAutoSaveIcon() {
    if (m_bAutoSaveIconShowing) {
        m_bAutoSaveIconShowing = false;
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SAVE_DONE"), "Autosave_Overlay.fng", nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SHOW_INCOMING"), nullptr, nullptr);
    }
}

bool MemoryCard::IsAutoSaveIconVisible() {
    if (m_bAutoSaveIconShowing || FEngIsScriptSet("Autosave_Overlay.fng", FEHashUpper("AUTOSAVE_ICON_GROUP"), FEHashUpper("LEAVE")) ||
        FEngIsScriptSet("Autosave_Overlay.fng", FEHashUpper("AUTOSAVE_ICON_GROUP"), FEHashUpper("APPEAR"))) {
        return true;
    }
    return false;
}
