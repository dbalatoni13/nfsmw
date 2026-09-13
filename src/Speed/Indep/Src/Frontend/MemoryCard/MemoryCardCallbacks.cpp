#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardImp.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern char g_GC_Disk_GameName[];

void DisplayMessage(const wchar_t *msg, unsigned int count, const wchar_t **str);

void DisplayStatus(int i) {}

MemcardCallbacks gMemcardCallbacks;

MemoryCard *MemcardCallbacks::GetMemcard() {
    return MemoryCard::GetInstance();
}
UIMemcardBase *MemcardCallbacks::GetScreen() {
    return MemoryCard::GetInstance()->GetScreen();
}

void MemcardCallbacks::ShowMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t **options) {
    if (GetMemcard()->IsMemcardScreenExiting()) {
        return;
    }
    JLog(MJ_ShowMesssage);
    JLog(msg);
    JLog(nOptions);
    for (uint32 i = 0; i < nOptions; i++) {
        JLog(options[i]);
    }
    DisplayMessage(msg, nOptions, options);
    GetMemcard()->m_bWaitingForResponse = true;
    if (GetMemcard()->IsAutoSaving() && gMemcardSetup.GetCommand() != 0xb0) {
        if (nOptions == 0) {
            GetMemcard()->m_bWaitingForResponse = false;
        } else {
            GetMemcard()->m_PendingMessage = new ("Memcard Msg", 0) MemoryCardMessage(msg, nOptions, options);
            GetMemcard()->HandleAutoSaveError();
        }
        return;
    }

    switch (GetMemcard()->GetOp()) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            if (nOptions == 0) {
                return;
            }
    }

    UIMemcardBase *pScreen = GetScreen();
    if (pScreen != nullptr) {
        if (pScreen->IsInButtonAnimation()) {
            if (GetMemcard()->GetPendingMessage() != nullptr) {
                GetMemcard()->ReleasePendingMessage();
            }
            GetMemcard()->m_PendingMessage = new ("Memcard Msg", 0) MemoryCardMessage(msg, nOptions, options);
        } else {
            GetScreen()->ShowMessage(msg, nOptions, options[0], options[1], options[2]);
        }
    }
}

// UNSOLVED (dwarf unmatched)
void MemcardCallbacks::ClearMessage() {
    if (GetMemcard()->IsAutoSaving()) {
        return;
    }

    JLog(MJ_ClearMessage);
    switch (GetMemcard()->GetOp()) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            return;
    }

    UIMemcardBase *pScreen = GetScreen();
    if (pScreen != nullptr) {
        if (false && pScreen->IsInButtonAnimation()) {
            pScreen->SetMessage(nullptr);
        }
        GetMemcard();
    }
}

void MemcardCallbacks::BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults res) {
    JLog(MJ_BootupCheckDone);
    JLog(status);
    JLog(res.mEntryFound);
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_LastError = static_cast<unsigned short>(status);
    GetMemcard()->m_SpecialError = static_cast<unsigned short>(status);
    if ((status != RealmcIface::STATUS_OK && GetMemcard()->GetPendingMessage() != nullptr) || status == RealmcIface::STATUS_UNKNOWN) {
        GetMemcard()->ReleasePendingMessage();
        GetMemcard()->BootupCheck(GetMemcard()->m_bAutoLoading && !FEDatabase->bProfileLoaded ? GetScreen()->m_FileName : nullptr);
        return;
    }
    GetMemcard()->m_pImp->BootupCheckDone(status, &res);
    GetMemcard()->SetBootFound(res.mEntryFound);
    if (GetMemcard()->m_bRetryBootCheck) {
        GetScreen()->SetStringCheckingCard();
    } else {
        cFEng::Get()->QueueGameMessage(0x461a18ee, GetScreen()->GetPackageName(), 0xff);
    }
}

void MemcardCallbacks::SaveCheckDone(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    JLog(MJ_SaveCheckDone);
}

void MemcardCallbacks::SaveDone(const char *filename) {
    JLog(MJ_SaveDone);
    JLog(filename);
    if (GetMemcard()->IsTypeProfile()) {
        bFree(GetMemcard()->m_pBuffer);
    }
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_pBuffer = nullptr;
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    FEDatabase->bProfileLoaded = true;
    FEDatabase->SetOptionsDirty(false);
    GetMemcard()->m_bCardRemoved = false;
    if (GetMemcard()->m_bManualSave && gMemcardSetup.GetCommand() != 0xb0) {
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            GetMemcard()->SetRetryAutoSave(false);
            GetMemcard()->SetAutoSaveEnabled(true);
        } else {
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        }
    } else if (GetMemcard()->IsAutoSaving() || gMemcardSetup.GetCommand() == 0xb0) {
        GetMemcard()->m_bAutoSaveCardPulled = false;
#ifndef EA_BUILD_A124
        if (GetMemcard()->m_bFoundAutoSaveFile) {
            FEDatabase->bAutoSaveOverwriteConfirmed = true;
        }
#endif
        if (GetMemcard()->IsRetryingAutoSave()) {
            GetMemcard()->ShowMessages(false);
            GetMemcard()->SetRetryAutoSave(false);
            GetMemcard()->SetAutoSaveEnabled(true);
        }
        GetMemcard()->EndAutoSave();
        if (gMemcardSetup.GetCommand() == 0xb0) {
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        }
    }
}

RealmcIface::DataStatus MemcardCallbacks::CheckLoadedData(const char *data) {
    JLog(MJ_CheckLoadedData);
    return RealmcIface::DATA_OK;
}

// UNSOLVED (dwarf unmatched)
void MemcardCallbacks::LoadDone(const char *filename) {
    JLog(MJ_LoadDone);
    JLog(filename);
    char *header = GetMemcard()->GetHeader();
    if (Joylog::IsReplaying()) {
        Joylog::GetData(header, 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        JLog(header, 8);
    }
    char *data = GetMemcard()->GetData();
    unsigned int size = GetMemcard()->GetSize();
    if (Joylog::IsReplaying()) {
        Joylog::GetData(data, size, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        JLog(data, size);
    }
    unsigned int *pHeader = reinterpret_cast<unsigned int *>(GetMemcard()->GetHeader());
    uint32 iStoredVersion = pHeader[0];
    uint32 iStoredSize = pHeader[1];
    MemoryCard::GetInstance()->m_MemOp = MemoryCard::MO_NONE;
    if (iStoredVersion == 0x10d && iStoredSize == GetMemcard()->GetSize() && GetMemcard()->IsTypeProfile()) {
        if (FEDatabase->LoadUserProfileFromBuffer(GetMemcard()->GetData(), GetMemcard()->GetSize(), GetMemcard()->GetPlayerNum())) {
            FEDatabase->DeallocBackupDB();
            if (GetMemcard()->GetPlayerNum() != 0) {
                if (GetMemcard()->m_pBuffer != nullptr) {
                    bFree(GetMemcard()->m_pBuffer);
                    GetMemcard()->m_pBuffer = nullptr;
                }
                cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
                return;
            }
            FEDatabase->bProfileLoaded = true;
            GetMemcard()->m_bCardRemoved = false;
            if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
                if (GetMemcard()->m_pBuffer != nullptr) {
                    bFree(GetMemcard()->m_pBuffer);
                    GetMemcard()->m_pBuffer = nullptr;
                }
                GetMemcard()->SetAutoSaveEnabled(true);
            } else {
                cFEng::Get()->QueueGameMessage(gMemcardSetup.GetCommand() == 0x20 ? 0xa4bb7ae1 : 0x461a18ee, nullptr, 0xff);
            }
        } else {
            GetMemcard()->ShowMessages(false);
            FEDatabase->RestoreFromBackupDB();
            cFEng::Get()->QueueGameMessage(0xf35d144e, nullptr, 0xff);
        }
    } else {
        FEDatabase->RestoreFromBackupDB();
        cFEng::Get()->QueueGameMessage(0xf35d144e, nullptr, 0xff);
    }
    if (GetMemcard()->m_pBuffer != nullptr) {
        bFree(GetMemcard()->m_pBuffer);
        GetMemcard()->m_pBuffer = nullptr;
    }
    FEDatabase->DeallocBackupDB();
}

void MemcardCallbacks::DeleteDone(const char *filename) {
    JLog(MJ_DeleteDone);
    JLog(filename);
    int idx = GetMemcard()->GetPrefixLength();
    if (bStrCmp(filename + idx, FEDatabase->GetUserProfile(0)->GetProfileName()) == 0) {
        FEDatabase->DefaultProfile();
        FEDatabase->bProfileLoaded = false;
    }
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    cFEng::Get()->QueueGameMessage(0x461a18ee, GetScreen()->GetPackageName(), 0xff);
}

void MemcardCallbacks::ClearEntries() {
    JLog(MJ_ClearEntries);
}

// UNSOLVED
void MemcardCallbacks::FoundEntry(const RealmcIface::EntryInfo *info) {
    JLog(MJ_FoundEntry);
    JLog(info);
    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->m_bOldSaveFileExists = true;
        return;
    } else if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bFoundAutoSaveFile = true;
        return;
    }
    if (bStrNCmp(g_GC_Disk_GameName, info->mGameCode, 4) != 0) {
        return;
    }
    unsigned int iSize = GetMemcard()->GetSize();
    int iGuessSize = info->mUserDataSize;
    unsigned int fDefault = 0;
    if (info->mStatus != RealmcIface::STATUS_OK) {
        fDefault = 2;
    }
    if (GetMemcard()->IsTypeProfile()) {
        unsigned int sec = GetMemcard()->m_DataSize;
        GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault);
    } else {
        if (info->mStatus != RealmcIface::STATUS_OK) {
            return;
        }
        unsigned int iOffset = GetMemcard()->m_EntryCount * 0x10;
        char *pNameBuf = GetMemcard()->m_pBuffer + iOffset;
        bStrNCpy(pNameBuf, info->mName, 0x10);
    }
    GetMemcard()->m_EntryCount++;
}

void MemcardCallbacks::FindEntriesDone(RealmcIface::CardStatus status) {
    JLog(MJ_FindEntriesDone);
    JLog(status);
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->SetListingForCreate(false);
    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->EndListingOldSaveFiles();
    } else if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bCheckingCardForOverwrite = false;
        if (GetMemcard()->m_bFoundAutoSaveFile) {
            GetMemcard()->HandleAutoSaveOverwriteMessage();
        } else {
            GetMemcard()->DoAutoSave();
        }
    } else {
        cFEng::Get()->QueueGameMessage(0x5a051729, GetScreen()->GetPackageName(), 0xff);
        GetMemcard()->SetBootFound(GetMemcard()->m_EntryCount > 0);
    }
}

void MemcardCallbacks::Retry(RealmcIface::CardStatus status) {
    JLog(MJ_Retry);
    JLog(status);
    if (GetScreen() != nullptr) {
        GetScreen()->SetStringCheckingCard();
        if (GetMemcard()->GetOp() == MemoryCard::MO_List) {
            GetScreen()->EmptyFileList();
        }
    }
}

// UNSOLVED
void MemcardCallbacks::Failed(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    JLog(MJ_Failed);
    JLog(status);
    JLog(result);
    if (GetMemcard()->IsWaitingForResponse() && (GetMemcard()->GetOp() == MemoryCard::MO_Delete || GetMemcard()->GetOp() == MemoryCard::MO_Load)) {
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        if (GetMemcard()->GetOp() == MemoryCard::MO_Delete) {
            GetMemcard()->Delete(nullptr);
        } else {
            GetMemcard()->Load(nullptr);
        }
        return;
    }
    unsigned int msg = 0x8867412d;
    if (GetMemcard()->m_pBuffer != nullptr) {
        bFree(GetMemcard()->m_pBuffer);
        GetMemcard()->m_pBuffer = nullptr;
    }
    if (GetMemcard()->m_pImp->GetSaveInfo() != nullptr) {
        GetMemcard()->m_pImp->DestructSaveInfo();
    }
    if (GetMemcard()->IsAutoSaving() || GetMemcard()->IsCheckingCardForAutoSave()) {
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        GetMemcard()->EndAutoSave();
        if (gMemcardSetup.GetMethod() == 0xb0) {
            cFEng::Get()->QueueGameMessage(0x8867412d, nullptr, 0xff);
        }
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        return;
    }
    if (GetMemcard()->IsListingOldSaveFiles()) {
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        GetMemcard()->EndListingOldSaveFiles();
        return;
    }
    if (GetMemcard()->IsRetryingAutoSave()) {
        GetMemcard()->SetRetryAutoSave(false);
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        if (result == RealmcIface::RESULT_CANCELLED || status == RealmcIface::STATUS_CARD_DAMAGED) {
            msg = 0xfe202e3b;
        }
    }
    if (gMemcardSetup.GetMethod() == 0x60 && GetMemcard()->GetOp() == MemoryCard::MO_List) {
        GetMemcard()->SetListingForCreate(false);
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        cFEng::Get()->QueueGameMessage(0x5a051729, GetScreen()->GetPackageName(), 0xff);
        return;
    }
    int op = GetMemcard()->GetOp();
    switch (op) {
        case MemoryCard::MO_AutoSave:
            break;

        case MemoryCard::MO_Save:
            if (status == RealmcIface::STATUS_NO_CARD)
                goto failed_check_autosave;
            if (static_cast<unsigned int>(status) >= static_cast<unsigned int>(RealmcIface::STATUS_NO_CARD)) {
                if (static_cast<unsigned int>(status) <= static_cast<unsigned int>(RealmcIface::STATUS_CARD_FULL)) {
                    if (static_cast<unsigned int>(status) >= static_cast<unsigned int>(RealmcIface::STATUS_WRONG_DEVICE))
                        goto failed_check_autosave;
                }
            }
            goto failed_skip_autosave;
        failed_check_autosave:
            if (gMemcardSetup.GetMethod() == 0x60) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            }
        failed_skip_autosave:
            msg = 0xdc12af2e;
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;

        case MemoryCard::MO_Load:
            if (GetMemcard()->IsTypeProfile()) {
                bFree(GetMemcard()->m_pBuffer);
            }
            GetMemcard()->m_pBuffer = nullptr;
            GetMemcard()->m_SpecialError = static_cast<unsigned short>(status);
            break;

        case MemoryCard::MO_BootUp:
            GetMemcard()->m_pImp->DestructSaveInfo();
            break;

        case MemoryCard::MO_List:
            if (GetMemcard()->InBootSequence()) {
                msg = 0x8867412d;
            }
            break;
    }
    GetMemcard()->m_LastError = static_cast<unsigned short>(status);
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    DisplayStatus(static_cast<int>(status));
    if (status == RealmcIface::STATUS_FILE_CORRUPTED) {
        GetMemcard()->BootupCheck(nullptr);
        GetMemcard()->m_bRetryBootCheck = true;
    } else {
        cFEng::Get()->QueueGameMessage(msg, GetScreen()->GetPackageName(), 0xff);
    }
}

void MemcardCallbacks::CardChanged(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    if ((result == RealmcIface::RESULT_RETRY && status == RealmcIface::STATUS_CARD_CHANGED) || status == RealmcIface::STATUS_OK) {
        cFEng::Get()->QueueGameMessage(0x3a2be557, nullptr, 0xff);
    } else if (result == RealmcIface::RESULT_CANCELLED) {
        cFEng::Get()->QueueGameMessage(0x8867412d, nullptr, 0xff);
    }
}

// UNSOLVED
void MemcardCallbacks::CardChecked(const RealmcIface::CardInfo *info) {
    JLog(MJ_CardChecked);
    JLog(info);
    unsigned int msg = 0x8867412d;
    if (GetMemcard()->IsCheckingCardForAutoSave()) {
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        GetMemcard()->m_LastError = *reinterpret_cast<const unsigned short *>(reinterpret_cast<const char *>(info) + 6);
        int cardStatus = info->mStatus;
        switch (cardStatus) {
            case RealmcIface::STATUS_CARD_CHANGED:
            case RealmcIface::STATUS_CARD_DAMAGED:
            case RealmcIface::STATUS_WRONG_DEVICE:
            case RealmcIface::STATUS_CARD_FULL:
            case RealmcIface::STATUS_ACCESS_DENIED:
                GetMemcard()->m_bFoundAutoSaveFile = true;
                GetMemcard()->DoAutoSave();
                return;
            case RealmcIface::STATUS_OK:
#ifndef EA_BUILD_A124
                if (!FEDatabase->bAutoSaveOverwriteConfirmed) {
                    GetMemcard()->m_bCheckingCardForAutoSave = false;
                    GetMemcard()->m_bCheckingCardForOverwrite = true;
                    GetMemcard()->ShowMessages(true);
                    char filter[32];
                    UserProfile *profile = FEDatabase->GetMultiplayerProfile(0);
                    bStrCat(filter, GetMemcard()->GetPrefix(), profile->GetProfileName());
                    GetMemcard()->m_bFoundAutoSaveFile = false;
                    GetMemcard()->List(filter, nullptr);
                    return;
                }
#endif
                GetMemcard()->DoAutoSave();
                return;

            case RealmcIface::STATUS_NO_CARD:
                GetMemcard()->HandleAutoSaveError();
                return;
            default:
                return;
        }
    } else {
        MemoryCard::SetMessageMode(1, true);
        if (info->mStatus == RealmcIface::STATUS_OK) {
            msg = 0x461a18ee;
        }
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        GetMemcard()->m_LastError = *reinterpret_cast<const unsigned short *>(reinterpret_cast<const char *>(info) + 6);
        UIMemcardBase *pScreen = GetScreen();
        if (msg == 0) {
            return;
        }
        if (pScreen == nullptr) {
            return;
        }
        cFEng::Get()->QueueGameMessage(msg, pScreen->GetPackageName(), 0xff);
    }
}

void MemcardCallbacks::CardRemoved() {
    JLog(MJ_CardRemoved);
    GetMemcard()->m_bAutoSaveCardPulled = true;
    if (GetMemcard()->GetOp() == MemoryCard::MO_Save) {
        GetMemcard()->m_bAutoSaveCardPulledDuringSave = true;
    }
    if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->HandleAutoSaveError();
    } else {
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            if (!MemoryCard::GetInstance()->IsAutoSaving()) {
                GetMemcard()->m_bCardRemoved = true;
            }
        }
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        if (FEDatabase->IsOptionsMode()) {
            cFEng::Get()->QueueGameMessage(0x7e998e5e, nullptr, 0xff);
        }
#ifndef EA_BUILD_A124
        FEDatabase->bAutoSaveOverwriteConfirmed = false;
#endif
    }
}

// UNSOLVED
void MemcardCallbacks::SetAutosaveDone(RealmcIface::TaskResult res, RealmcIface::CardStatus status, RealmcIface::AutosaveState flag) {
    JLog(MJ_SetAutosaveDone);
    JLog(res);
    JLog(status);
    JLog(reinterpret_cast<unsigned int &>(flag));
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_bAutoSave = (flag == RealmcIface::AUTOSAVE_ENABLE);
    GetMemcard()->m_bAutoSaveCardPulled = false;
    GetMemcard()->m_bAutoSaveCardPulledDuringSave = false;
    if (GetMemcard()->m_bDisablingAutoSaveForSave) {
        GetMemcard()->m_bDisablingAutoSaveForSave = false;
        GetMemcard()->ShowMessages(true);
        cFEng::Get()->QueueGameMessage(0xc6c6b68f, GetMemcard()->IsMemcardScreenShowing() ? gMemcardSetup.mMemScreen : nullptr, 0xff);
        return;
    }
    unsigned int msg = 0x461a18ee;
    if (status != RealmcIface::STATUS_OK && flag != RealmcIface::AUTOSAVE_ENABLE) {
        if (status == RealmcIface::STATUS_NO_CARD) {
            msg = 0xb57fdb17;
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        } else {
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        }
    }
    if (gMemcardSetup.mPreviousCommand == 0x20) {
        msg = 0xa4bb7ae1;
    }
    if (GetMemcard()->IsAutoSaving()) {
        if (flag != RealmcIface::AUTOSAVE_ENABLE && FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            GetMemcard()->m_bCardRemoved = true;
        }
        GetMemcard()->EndAutoSave();
    } else {
        cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
    }
    if (flag == RealmcIface::AUTOSAVE_ENABLE) {
#ifndef EA_BUILD_A124
        if (gMemcardSetup.GetCommand() == 0xa0 && FEDatabase->IsOptionsMode()) {
            FEDatabase->bAutoSaveOverwriteConfirmed = false;
        }
#endif
        FEDatabase->GetGameplaySettings()->AutoSaveOn = true;
        GetMemcard()->m_bCardRemoved = false;
    }
}

// UNSOLVED
void MemcardCallbacks::SetMonitorDone(RealmcIface::CardStatus status, RealmcIface::MonitorState state) {
    JLog(MJ_Retry);
    JLog(status);
    JLog(state);
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_bMonitorOn = (static_cast<unsigned int>(state) - 1u < 2u);
    unsigned int msg;
    if (state == RealmcIface::MONITOR_ON) {
        if (status == RealmcIface::STATUS_OK) {
            msg = 0x54b3ac6c;
        } else {
            msg = 0x8867412d;
        }
    } else {
        if (cFEng::Get()->IsPackagePushed("MemCard.fng")) {
            msg = 0xeb29392a;
        } else {
            msg = 0;
            if (MemoryCard::GetInstance()->IsMemcardScreenShowing()) {
                msg = 0x8867412d;
            }
        }
    }
    cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
}

RealmcIface::TaskStatus MemcardCallbacks::LoadReady(const char *entryName, unsigned int headerSize, unsigned int bodySize, char *&headerData,
                                                    char *&bodyData) {
    JLog(MJ_LoadReady);
    JLog(entryName);
    RealmcIface::TaskStatus res = RealmcIface::TASK_CANCEL;
    JLog(headerSize);
    JLog(bodySize);
    if (headerSize == 8 && bodySize == GetMemcard()->GetSize()) {
        res = RealmcIface::TASK_CONTINUE;
        bodyData = GetMemcard()->m_pBuffer;
        headerData = GetMemcard()->GetHeader();
    }
    return res;
}

// UNSOLVED
void IJoyHelper::EmulateMemoryCardLibrary(int aJoyOp) {
    char *pBuf = new ("MemcardJoyLogBuffer", 0) char[0x400];
    char *pBuf1 = pBuf + 1;
    const wchar_t *pOptions[4];
    pOptions[0] = reinterpret_cast<const wchar_t *>(pBuf + 0x338);
    pOptions[1] = reinterpret_cast<const wchar_t *>(pBuf + 0x36a);
    pOptions[2] = reinterpret_cast<const wchar_t *>(pBuf + 0x39c);
    pOptions[3] = reinterpret_cast<const wchar_t *>(pBuf + 0x3ce);
    RealmcIface::CardInfo lCardInfo;
    RealmcIface::EntryInfo lEntryInfo;
    lEntryInfo.mName = pBuf;
    switch (aJoyOp) {
        case 1:
            gMemcardCallbacks.ShowMessage(reinterpret_cast<const wchar_t *>(pBuf), 0, pOptions);
            break;
        case 2:
            gMemcardCallbacks.ClearMessage();
            break;
        case 3: {
            RealmcIface::BootupCheckResults lBootRes;
            lBootRes.Clear();
            gMemcardCallbacks.BootupCheckDone(static_cast<RealmcIface::CardStatus>(0), lBootRes);
            break;
        }
        case 4:
            gMemcardCallbacks.SaveCheckDone(static_cast<RealmcIface::TaskResult>(0), static_cast<RealmcIface::CardStatus>(0));
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
            gMemcardCallbacks.FindEntriesDone(static_cast<RealmcIface::CardStatus>(0));
            break;
        case 0xc:
            gMemcardCallbacks.Retry(static_cast<RealmcIface::CardStatus>(0));
            break;
        case 0xd:
            gMemcardCallbacks.Failed(static_cast<RealmcIface::TaskResult>(0), static_cast<RealmcIface::CardStatus>(0));
            break;
        case 0xe:
            gMemcardCallbacks.CardChecked(&lCardInfo);
            break;
        case 0xf:
            gMemcardCallbacks.CardRemoved();
            break;
        case 0x10:
            gMemcardCallbacks.SetAutosaveDone(static_cast<RealmcIface::TaskResult>(0), static_cast<RealmcIface::CardStatus>(0),
                                              static_cast<RealmcIface::AutosaveState>(0));
            break;
        case 0x11:
            gMemcardCallbacks.LoadReady(pBuf, 0, 0, pBuf1, pBuf1);
            break;
        case 0x12:
            gMemcardCallbacks.SetMonitorDone(static_cast<RealmcIface::CardStatus>(0), static_cast<RealmcIface::MonitorState>(1));
            break;
    }
    if (pBuf != nullptr) {
        delete[] pBuf;
    }
}

void DisplayUnicode(const wchar_t *str) {
    const short *pWChar = reinterpret_cast<const short *>(str);
    if (*pWChar == 0) {
        return;
    }
    do {
        pWChar++;
    } while (*pWChar != 0);
}

void DisplayMessage(const wchar_t *msg, unsigned int count, const wchar_t **str) {
    DisplayUnicode(msg);
    if (count != 0) {
        for (unsigned int i = 0; i < count; i++) {
            DisplayUnicode(str[i]);
        }
    }
}
