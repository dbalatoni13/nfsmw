#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardHelper.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/MenuScreens/../Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

extern EA::Allocator::IAllocator &gMemoryAllocator;
extern MemcardCallbacks gMemcardCallbacks;

unsigned short gSaveType0[32];
unsigned short gSaveType1[32];
unsigned short gSaveType2[16];

// .data:0x8041BBB8 del original (0xC B), justo delante de _10MemoryCard.s_pThis:
// los tres punteros valen gSaveType0/1/2. El zPlatform extraido lo referencia
// como UND (MemoryCardImp.cpp lo declara con su nombre de ensamblador).
unsigned short *MemoryCardImp::gEntryType[3] = {gSaveType0, gSaveType1, gSaveType2};

MemoryCard *MemoryCard::s_pThis = nullptr;

#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_PLAYSTATION2)
// RealMemCard is built with -fshort-wchar on these platforms, so its wide string
// methods are mangled with `Uw`, while this unit (plain wchar_t) calls the `w`
// names. These C shims carry the `w` names and forward to the library ones; the
// original has them with exactly these names and void * parameters.
extern "C" {

void Delete__Q211RealmcIface16MemcardInterfacePCcPCUw(void *pthis, void *p1, void *p2);
void *__Q211RealmcIface8GameInfoPCUwUibT3(void *pthis, void *p0, void *p1, void *p2);
void Load__Q211RealmcIface16MemcardInterfacePCcPcT2PCUwPCQ211RealmcIface9TitleInfoT4(void *pthis, void *p0, void *p1, void *p2,
                                                                                     void *p3, void *p4, void *p5);

void Delete__Q211RealmcIface16MemcardInterfacePCcPCw(void *pthis, void *p1, void *p2) {
    Delete__Q211RealmcIface16MemcardInterfacePCcPCUw(pthis, p1, p2);
}

void *__Q211RealmcIface8GameInfoPCwUibT3(void *pthis, void *p0, void *p1, void *p2) {
    return __Q211RealmcIface8GameInfoPCUwUibT3(pthis, p0, p1, p2);
}

void Load__Q211RealmcIface16MemcardInterfacePCcPcT2PCwPCQ211RealmcIface9TitleInfo(void *pthis, void *p0, void *p1, void *p2, void *p3,
                                                                                  void *p4, void *p5) {
    Load__Q211RealmcIface16MemcardInterfacePCcPcT2PCUwPCQ211RealmcIface9TitleInfoT4(pthis, p0, p1, p2, p3, p4, p5);
}
}
#endif

void InitMemoryCard() {
    MemoryCard::s_pThis = new ("MemoryCard", __LINE__) MemoryCard();

    bStrCpy(gSaveType0, "");
    bStrCpy(gSaveType1, "");
    bStrCpy(gSaveType2, "");

    bStrCpy(MemoryCardImp::gContentName, "NFSMWSD");
    MemoryCard::s_pThis->Init();
}

MemoryCardMessage::MemoryCardMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options) {
    bStrCpy(mMsg, reinterpret_cast<const unsigned short *>(msg));
    mnOptions = nOptions;
    for (unsigned int i = 0; i < nOptions; i++) {
        bStrCpy(mOptions[i], reinterpret_cast<const unsigned short *>(options[i]));
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
    m_bFoundAutoSaveFile = false;
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

    void *pIcon = bGetFile("memcard/icon1.raw", nullptr, 0);

    void *pBanner = bGetFile("memcard/banner.raw", nullptr, 0);

    m_pRMIcon = BNEW GCIconDataInfo();
    m_pRMBanner = BNEW GCBannerDataInfo();

    m_pRMIcon->mNumIcons = 1;
    m_pRMIcon->mIconData = pIcon;
    m_pRMIcon->mSpeed = 0;

    m_pRMBanner->mBannerData = pBanner;
    m_pRMBanner->mFormat = 0;
}

bool MemoryCard::IsCardAvailable() {
    MemoryCard *pThis = GetInstance();

    if (pThis != nullptr) {
        unsigned short err = pThis->m_LastError;

        if (err == 0 || err == 11) {
            return true;
        }

        return false;
    }

    return false;
}

void MemoryCard::SetExtraParam(SaveType t, const char *filename, void *buf, unsigned int size) {
    MemoryCard *pThis = GetInstance();

    if (pThis != nullptr) {
        pThis->m_ReqFilename = filename;
        pThis->m_Type = t;
        pThis->m_pBuffer = static_cast<char *>(buf);
        pThis->m_DataSize = size;
    }
}

void MemoryCard::InitCommand(int op) {
    m_ReqOp = 0;

    m_bWaitingForResponse = false;

    m_MemOp = op;
    m_LastError = 0;
}

void MemoryCard::RequestTask(int op, const char *name) {
    m_ReqFilename = name;
    m_ReqOp = op;
}

void MemoryCard::ProcessTask() {
    if (m_pFEScreen == nullptr) {
        m_ReqOp = 0;
        return;
    }

    {
        switch (m_ReqOp) {
            case 6:
                Delete(m_ReqFilename);
                break;
            case 5:
                Load(m_ReqFilename);
                break;
            case 7:
                List(nullptr, nullptr);
                break;
        }
    }

    m_ReqOp = 0;
}

bool MemoryCard::IsCardBusy() {
    if (GetInstance() != nullptr) {
        if (GetInstance()->m_pIMemcard->IsResettable() == 0 || GetInstance()->IsAutoSaveIconVisible() ||
            (GetInstance()->m_bInAutoSave && !GetInstance()->m_bWaitingForResponse)) {
            return true;
        }
    }

    return false;
}

void MemoryCard::Init() {
    static Realmc::SystemInterface iSystem;
    static MemoryCardImp sMemcardImp;
    static Realmc::SystemInterface *pSystem = nullptr;

    if (pSystem == nullptr) {
        iSystem.mAllocator = &gMemoryAllocator;
        iSystem.mThread = BNEW MyThread;
        iSystem.mMutex = BNEW MyMutex;
        iSystem.mLocaleGetter = MemoryCard::GetLocaleString;
        pSystem = &iSystem;
    }

    m_pImp = &sMemcardImp;

    bStrCpy(m_GameTitle, "Need" "\xA0" "for" "\xA0" "Speed" "\x99" "\xA0" "Most" "\xA0" "Wanted");

    unsigned int titleId = 0;

    m_pGameInfo = BNEW RealmcIface::GameInfo(reinterpret_cast<const wchar_t *>(m_GameTitle), titleId, false, false);
    m_pIMemcard = RealmcIface::MemcardInterface::CreateInstance(&iSystem, &gMemcardCallbacks, m_pGameInfo);

    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 1);

    m_TimeOffsetSec = 0;
    m_pLocaleFileHandler = nullptr;
}

void MemoryCard::StartBootSequence() {
    m_bInBootSequence = true;
    gMemcardSetup.mOp = 32;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_HIDE, 0x4000);
}

void MemoryCard::EndBootSequence() {
    m_bInBootSequence = false;
    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 0x4000);
}

void MemoryCard::LoadLocale(eLanguages eLang) {
    if (GetInstance() == nullptr) {
        return;
    }

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

    if (GetInstance()->m_pLocaleFileHandler == nullptr) {
        GetInstance()->m_pLocaleFileHandler = bMalloc(0x2000, __FILE__, __LINE__, 0);
    }

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
    if (GetInstance() != nullptr) {
        GetInstance()->m_pIMemcard->SetMessage((RealmcIface::MessageState)(flag == 0), msg);
    }
}

void MemoryCard::Tick(int TickCount) {
    if (m_MemOp == 0 && m_ReqOp != 0) {
        ProcessTask();
    }

    if (m_bAutoSaveRequested && m_bHUDLoaded && GManager::Exists() && !GManager::Get().GetHasPendingSMS()) {
        m_bHUDLoaded = false;
        m_bAutoSaveRequested = false;
        StartAutoSave(false);
    }

    if (Joylog::IsReplaying()) {
        MemoryCardJoyLoggableEvents l_JoyOp;
        do {
            l_JoyOp = static_cast<MemoryCardJoyLoggableEvents>(ReplayJoyOp());
        } while (l_JoyOp != MJ_None);
    } else {
        m_pIMemcard->Update(TickCount);
        if (Joylog::IsCapturing()) {
            CaptureJoyOp(MJ_None);
        }
    }

    if (FEDatabase != nullptr && !FEDatabase->IsOptionsMode()) {
        if (cFEng::Get()->IsPackagePushed("MC_Main_GC.fng") || cFEng::Get()->IsPackagePushed("InGame_MC_Main_GC.fng") ||
            IsAutoSaveIconVisible()) {
            if (FEManager::Get()->IsAllowingControllerError() || TheGameFlowManager.IsInGame()) {
                if (cFEng::Get()->IsPackagePushed("InGamePhotoMaster.fng") || cFEng::Get()->IsPackagePushed("PostRace_Pursuit.fng")) { m_bNonSilentAutoSave = true; }
                m_bNeedToAllowControllerErrors = true;
                FEManager::Get()->AllowControllerError(false);
                FEManager::Get()->SuppressControllerError(true);
            }
        } else if (m_bNeedToAllowControllerErrors) {
            m_bNeedToAllowControllerErrors = false;
            if (!FEManager::Get()->IsAllowingControllerError()) {
                if (m_bNonSilentAutoSave) {
                    m_bNonSilentAutoSave = false;
                } else {
                    FEManager::Get()->AllowControllerError(true);
                    FEManager::Get()->SuppressControllerError(false);
                }
            }
        }
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

    m_BootupParams.mNumSaveTypes = 1;
    m_BootupParams.mSaveReqs = m_pImp->GetSaveReqArray();
    m_BootupParams.mValidCardIds = 1;

    InitCommand(MO_BootUp);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->BootupCheck(&m_BootupParams, 0, nullptr, nullptr);
    }
}

// r56-fe: ShouldDoAutoSave va detras de BootupCheck (zFe.o 0x020130), no
// detras de Tick.
bool MemoryCard::ShouldDoAutoSave(bool bForce) {
    if (!bForce) {
        if (m_bCancelNextAutoSave) {
            m_bCancelNextAutoSave = false;
            return false;
        }

        if (FEDatabase->IsOnlineMode() || FEDatabase->IsLanMode()) {
            return false;
        }

        if (!IsMemcardEnabled || !IsAutoSaveEnabled) {
            return false;
        }
        if (!FEDatabase->GetUserProfile(0)->GetOptions()->TheGameplaySettings.AutoSaveOn && !m_bCardRemoved) {
            return false;
        }
        if (!FEDatabase->IsFinalEpicChase()) {
            if (GRaceStatus::Exists()) {
                if (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetIsBossRace()) {
                    return false;
                }
            }
        }
    }

    return true;
}

void MemoryCard::StartAutoSave(bool bForce) {
    if (ShouldDoAutoSave(bForce)) {
        if (FEDatabase->bProfileLoaded) {
            if (gMemcardSetup.GetCommand() != MCO_AutoSave) {
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
    }
}

void MemoryCard::DoAutoSave() {
    m_bCheckingCardForAutoSave = false;

    if (gMemcardSetup.GetCommand() == MCO_AutoSave) {
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
        DialogInterface::ShowOneButton("", "", static_cast<eDialogTitle>(2), LANGUAGE_COMMON_OK, 0x34dc1bec, 0xc5e2beac);
    }

    FEDatabase->GetCareerSettings()->AwardOneTimeCashBonus(m_bOldSaveFileExists);
}

void MemoryCard::SetMonitor(bool bEnabled) {
    InitCommand(MO_SetMonitor);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->SetMonitor(bEnabled ? RealmcIface::MONITOR_ON : RealmcIface::MONITOR_OFF);
    }

    if (!bEnabled && Joylog::IsReplaying()) {
        ReplayJoyOp();
    }
}

void MemoryCard::SetAutoSaveEnabled(bool bEnabled) {
    char entryname[16];

    bStrCpy(entryname, FEDatabase->GetMultiplayerProfile(0)->GetProfileName());

    SetExtraParam(ST_PROFILE, entryname, nullptr, FEDatabase->GetUserProfileSaveSize(false));

    bStrCat(m_Filename, m_pImp->GetPrefix(), entryname);

    bStrNCpy(MemoryCardImp::gContentName, entryname, 16);

    if (GetScreen() != nullptr && gMemcardSetup.GetCommand() == MCO_EnableAutoSave) {
        GetScreen()->SetStringCheckingCard();

        ShowMessages(true);
    } else {
        ShowMessages(false);
    }

    m_pIMemcard->SetMessage(RealmcIface::MESSAGE_SHOW, 1);

    if (bEnabled) {
        gMemcardSetup.ClearCommand();
        gMemcardSetup.SetCommand(MCO_EnableAutoSave);
    } else {
        m_bDisablingAutoSaveForSave = true;
    }

    InitCommand(MO_AutoSave);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->SetAutosave(bEnabled ? RealmcIface::AUTOSAVE_ENABLE : RealmcIface::AUTOSAVE_DISABLE, 0, nullptr, entryname,
                                 RealmcIface::CARD_UNKNOWN);
    }

    if (!bEnabled && Joylog::IsReplaying()) {
        ReplayJoyOp();
    }
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
    m_pIMemcard->SetMessage((RealmcIface::MessageState)(bShow == 0), 0xffffffff);
}

void MemoryCard::CheckCard(int iSlot) {
    InitCommand(MO_CheckCard);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->CheckCard(RealmcIface::CARD_UNKNOWN);
    }
}

void MemoryCard::Save(const char *entryName) {
    SetExtraParam(ST_PROFILE, entryName, nullptr, FEDatabase->GetUserProfileSaveSize(false));

    if (m_pImp->GetSaveInfo() == nullptr) {
        m_pImp->ConstructSaveInfo(ST_PROFILE, entryName, m_DataSize);
        bStrCat(m_Filename, m_pImp->GetPrefix(), entryName);
    }

    bStrNCpy(MemoryCardImp::gContentName, entryName, 16);

    m_pBuffer = static_cast<char *>(bMalloc(m_DataSize, __FILE__, __LINE__, 0x40));
    FEDatabase->SaveUserProfileToBuffer(m_pBuffer, m_DataSize);

    m_Header[0] = 0x10d;
    m_Header[1] = m_DataSize;

    InitCommand(MO_Save);
    if (!Joylog::IsReplaying()) {
        m_pIMemcard->Save(m_Filename, reinterpret_cast<const char *>(m_Header), m_pBuffer, m_pImp->GetSaveInfo(), nullptr);
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

    m_pBuffer = static_cast<char *>(bMalloc(m_DataSize, __FILE__, __LINE__, 0x40));

    if (filename != nullptr) {
        bStrNCpy(MemoryCardImp::gContentName, filename, 16);
        bStrCat(m_Filename, m_pImp->GetPrefix(), filename);
    }

    InitCommand(MO_Load);

    if (!Joylog::IsReplaying()) {
        if (m_bInBootSequence) {
            m_bAutoLoading = true;
            BootupCheck(filename);
        } else {
            m_pIMemcard->Load(m_Filename, nullptr, nullptr, reinterpret_cast<const wchar_t *>(MemoryCardImp::gContentName), nullptr);
        }
    }
}

void MemoryCard::Delete(const char *filename) {
    InitCommand(MO_Delete);
    if (filename != nullptr) {
        bStrNCpy(MemoryCardImp::gContentName, filename, 16);
        bStrCat(m_Filename, m_pImp->GetPrefix(), filename);
    }

    if (!Joylog::IsReplaying()) {
        m_pIMemcard->Delete(m_Filename, reinterpret_cast<const wchar_t *>(MemoryCardImp::gContentName));
    }
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
    UIMemcardBase *pScreen = m_pFEScreen;

    if (gMemcardSetup.GetCommand() == MCO_AutoSave || pScreen != nullptr) {
        pScreen->HandleAutoSaveError();
    } else {
        MemcardEnter(nullptr, nullptr, 0x91, nullptr, nullptr, 0, 0);
    }
}

void MemoryCard::HandleAutoSaveOverwriteMessage() {
    UIMemcardBase *pScreen = m_pFEScreen;

    if (gMemcardSetup.GetCommand() == MCO_AutoSave || pScreen != nullptr) {
        pScreen->HandleAutoSaveOverwriteMessage();
    } else {
        MemcardEnter(nullptr, nullptr, 0xd1, nullptr, nullptr, 0, 0);
    }
}

void MemoryCard::ShowAutoSaveIcon() {
    if (!m_bAutoSaveIconShowing) {
        m_bAutoSaveIconShowing = true;

        if (!cFEng::Get()->IsPackagePushed("Autosave_Overlay.fng")) {
            cFEng::Get()->PushNoControlPackage("Autosave_Overlay.fng", static_cast<FE_PACKAGE_PRIORITY>(0x68));
        }

        cFEng::Get()->QueuePackageMessage(FEHashUpper("SAVE"), "Autosave_Overlay.fng", nullptr);

        unsigned int msg;
        bool bWidescreen = FEDatabase->GetVideoSettings()->WideScreen;

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr &&
            GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
            msg = FEHashUpper(bWidescreen ? "LEFT_WIDE" : "LEFT_NORM");
        } else if (cFEng::Get()->IsPackagePushed("SMS_MailBoxes.fng") || GManager::Get().GetHasPendingSMS()) {
            msg = FEHashUpper("SMS_MAILBOX");
            cFEng::Get()->QueuePackageMessage(FEHashUpper("HIDE_INCOMING"), nullptr, nullptr);
        } else {
            msg = FEHashUpper(bWidescreen ? "RIGHT_WIDE" : "RIGHT_NORM");
        }

        cFEng::Get()->QueuePackageMessage(msg, "Autosave_Overlay.fng", nullptr);
    }
}

void MemoryCard::HideAutoSaveIcon() {
    if (m_bAutoSaveIconShowing) {
        m_bAutoSaveIconShowing = false;
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SAVE_DONE"), "Autosave_Overlay.fng", nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("SHOW_INCOMING"), nullptr, nullptr);
    }
}

bool MemoryCard::IsAutoSaveIconVisible() {
    if (m_bAutoSaveIconShowing ||
        FEngIsScriptSet("Autosave_Overlay.fng", FEHashUpper("AUTOSAVE_ICON_GROUP"), FEHashUpper("LEAVE")) ||
        FEngIsScriptSet("Autosave_Overlay.fng", FEHashUpper("AUTOSAVE_ICON_GROUP"), FEHashUpper("APPEAR"))) {
        return true;
    }

    return false;
}
