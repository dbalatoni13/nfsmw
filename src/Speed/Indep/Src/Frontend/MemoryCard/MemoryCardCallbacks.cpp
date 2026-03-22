#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void DisplayUnicode(const wchar_t* str) {
    const short* pWChar = reinterpret_cast< const short* >(str);
    if (*pWChar == 0) {
        return;
    }
    do {
        pWChar++;
    } while (*pWChar != 0);
}

void DisplayMessage(const wchar_t* msg, unsigned int count, const wchar_t** str) {
    DisplayUnicode(msg);
    if (count != 0) {
        for (unsigned int i = 0; i < count; i++) {
            DisplayUnicode(str[i]);
        }
    }
}

extern char g_GC_Disk_GameName[];

void DisplayStatus(int i) {}

MemcardCallbacks gMemcardCallbacks;

MemoryCard* MemcardCallbacks::GetMemcard() { return MemoryCard::GetInstance(); }
UIMemcardBase* MemcardCallbacks::GetScreen() { return MemoryCard::GetInstance()->GetScreen(); }

void MemcardCallbacks::ShowMessage(const wchar_t* msg, unsigned int nOptions,
                                   const wchar_t** options) {
    if (GetMemcard()->IsMemcardScreenExiting()) {
        return;
    }
    JLog(MJ_ShowMesssage);
    Joylog::AddOrGetData(
        reinterpret_cast<unsigned short*>(const_cast<wchar_t*>(msg)),
        JOYLOG_CHANNEL_MEMORY_CARD);
    unsigned int loggedOptions = Joylog::AddOrGetData(nOptions, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    for (unsigned int i = 0; i < loggedOptions; i++) {
        Joylog::AddOrGetData(
            reinterpret_cast<unsigned short*>(const_cast<wchar_t*>(options[i])),
            JOYLOG_CHANNEL_MEMORY_CARD);
    }
    DisplayMessage(msg, loggedOptions, options);
    GetMemcard()->SetWaitingForResponse(true);
    if (GetMemcard()->IsAutoSaving() && gMemcardSetup.GetMethod() != 0xb0) {
        if (loggedOptions == 0) {
            GetMemcard()->SetWaitingForResponse(false);
        } else {
            GetMemcard()->m_PendingMessage =
                new (__FILE__, __LINE__) MemoryCardMessage(msg, loggedOptions, options);
            GetMemcard()->HandleAutoSaveError();
        }
    } else {
        int op = GetMemcard()->GetOp();
        switch (op) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            if (loggedOptions == 0) {
                break;
            }
            // fallthrough
        default: {
            UIMemcardBase* pScreen = GetScreen();
            if (pScreen != nullptr) {
                if (pScreen->IsInButtonAnimation()) {
                    if (GetMemcard()->GetPendingMessage() != nullptr) {
                        GetMemcard()->ReleasePendingMessage();
                    }
                    GetMemcard()->m_PendingMessage =
                        new (__FILE__, __LINE__)
                            MemoryCardMessage(msg, loggedOptions, options);
                } else {
                    GetScreen()->ShowMessage(msg, loggedOptions, options[0],
                                             options[1], options[2]);
                }
            }
            break;
        }
        }
    }
}

void MemcardCallbacks::ClearMessage() {
    if (!GetMemcard()->IsAutoSaving()) {
        JLog(MJ_ClearMessage);
        int op = GetMemcard()->GetOp();
        switch (op) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            break;
        default: {
            UIMemcardBase* pScreen = GetScreen();
            if (pScreen != nullptr) {
                GetMemcard();
            }
            break;
        }
        }
    }
}

void MemcardCallbacks::BootupCheckDone(RealmcIface::CardStatus status,
                                       RealmcIface::BootupCheckResults res) {
    JLog(MJ_BootupCheckDone);
    status = static_cast<RealmcIface::CardStatus>(
        Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    res.mEntryFound =
        Joylog::AddOrGetData(static_cast<unsigned int>(res.mEntryFound), 1,
                             JOYLOG_CHANNEL_MEMORY_CARD) != 0;
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_LastError = static_cast<unsigned short>(status);
    GetMemcard()->m_SpecialError = static_cast<unsigned short>(status);
    if ((status != RealmcIface::STATUS_OK &&
         GetMemcard()->GetPendingMessage() != nullptr) ||
        status == RealmcIface::STATUS_UNKNOWN) {
        GetMemcard()->ReleasePendingMessage();
        MemoryCard* mc = GetMemcard();
        const char* entry;
        if (GetMemcard()->IsAutoLoading() && !FEDatabase->bProfileLoaded) {
            entry = GetScreen()->m_FileName;
        } else {
            entry = nullptr;
        }
        mc->BootupCheck(entry);
        return;
    }
    GetMemcard()->m_pImp->BootupCheckDone(status, &res);
    GetMemcard()->SetBootFound(res.mEntryFound);
    if (GetMemcard()->m_bRetryBootCheck) {
        GetScreen()->SetStringCheckingCard();
    } else {
        cFEng* feng = cFEng::Get();
        UIMemcardBase* scr = GetScreen();
        feng->QueueGameMessage(0x461a18ee, scr->GetPackageName(), 0xff);
    }
}

void MemcardCallbacks::SaveCheckDone(RealmcIface::TaskResult result,
                                     RealmcIface::CardStatus status) {
    JLog(MJ_SaveCheckDone);
}

void MemcardCallbacks::SaveDone(const char* filename) {
    JLog(MJ_SaveDone);
    Joylog::AddOrGetData(const_cast<char*>(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetMemcard()->IsTypeProfile()) {
        bFree(GetMemcard()->m_pBuffer);
    }
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_pBuffer = nullptr;
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    FEDatabase->bProfileLoaded = true;
    FEDatabase->bIsOptionsDirty = false;
    GetMemcard()->m_bCardRemoved = false;
    if (GetMemcard()->IsManualSave() && gMemcardSetup.GetMethod() != 0xb0) {
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            GetMemcard()->m_bRetryAutoSave = false;
            GetMemcard()->SetAutoSaveEnabled(true);
        } else {
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        }
    } else if (GetMemcard()->IsAutoSaving() || gMemcardSetup.GetMethod() == 0xb0) {
        GetMemcard()->m_bAutoSaveCardPulled = false;
        if (GetMemcard()->m_bFoundAutoSaveFile) {
            FEDatabase->bAutoSaveOverwriteConfirmed = true;
        }
        if (GetMemcard()->m_bRetryAutoSave) {
            GetMemcard()->ShowMessages(false);
            GetMemcard()->m_bRetryAutoSave = false;
            GetMemcard()->SetAutoSaveEnabled(true);
        }
        GetMemcard()->EndAutoSave();
        if (gMemcardSetup.GetMethod() == 0xb0) {
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        }
    }
}

RealmcIface::DataStatus MemcardCallbacks::CheckLoadedData(const char* data) {
    JLog(MJ_CheckLoadedData);
    return RealmcIface::DATA_OK;
}

void MemcardCallbacks::LoadDone(const char* filename) {
    JLog(MJ_LoadDone);
    Joylog::AddOrGetData(const_cast<char*>(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    char* header = GetMemcard()->GetHeader();
    if (Joylog::IsReplaying()) {
        Joylog::GetData(header, 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        Joylog::AddData(header, 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    char* data = GetMemcard()->GetData();
    unsigned int size = GetMemcard()->GetSize();
    if (Joylog::IsReplaying()) {
        Joylog::GetData(data, size, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        Joylog::AddData(data, size, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    unsigned int* pHeader =
        reinterpret_cast<unsigned int*>(GetMemcard()->GetHeader());
    unsigned int iStoredVersion = pHeader[0];
    unsigned int iStoredSize = pHeader[1];
    MemoryCard::GetInstance()->m_MemOp = MemoryCard::MO_NONE;
    if (iStoredVersion == 0x10d && iStoredSize == GetMemcard()->GetSize() &&
        GetMemcard()->IsTypeProfile()) {
        bool isProfileValid = FEDatabase->LoadUserProfileFromBuffer(
            GetMemcard()->GetData(), GetMemcard()->GetSize(),
            GetMemcard()->GetPlayerNum());
        if (isProfileValid) {
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
                cFEng* feng = cFEng::Get();
                unsigned int msg = 0x461a18ee;
                if (gMemcardSetup.GetMethod() == 0x20) {
                    msg = 0xa4bb7ae1;
                }
                feng->QueueGameMessage(msg, nullptr, 0xff);
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

void MemcardCallbacks::DeleteDone(const char* filename) {
    JLog(MJ_DeleteDone);
    Joylog::AddOrGetData(const_cast<char*>(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    int idx = GetMemcard()->GetPrefixLength();
    if (bStrCmp(filename + idx,
                FEDatabase->GetUserProfile(0)->GetProfileName()) == 0) {
        FEDatabase->DefaultProfile();
        FEDatabase->bProfileLoaded = false;
    }
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    cFEng::Get()->QueueGameMessage(0x461a18ee, GetScreen()->GetPackageName(),
                                   0xff);
}

void MemcardCallbacks::ClearEntries() {
    JLog(MJ_ClearEntries);
}

void MemcardCallbacks::FoundEntry(const RealmcIface::EntryInfo* info) {
    JLog(MJ_FoundEntry);
    Joylog::AddOrGetData(const_cast<char*>(info->mName), JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::EntryInfo*>(info)->mStatus =
        static_cast<RealmcIface::CardStatus>(Joylog::AddOrGetData(
            static_cast<unsigned int>(info->mStatus), 0x10,
            JOYLOG_CHANNEL_MEMORY_CARD));
    const_cast<RealmcIface::EntryInfo*>(info)->mEntryBlocks =
        Joylog::AddOrGetData(info->mEntryBlocks, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::EntryInfo*>(info)->mUserDataSize =
        Joylog::AddOrGetData(info->mUserDataSize, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::EntryInfo*>(info)->mTime.mCreated =
        Joylog::AddOrGetData(info->mTime.mCreated, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::EntryInfo*>(info)->mTime.mLastAccessed =
        Joylog::AddOrGetData(info->mTime.mLastAccessed, 0x20,
                             JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::EntryInfo*>(info)->mTime.mLastModified =
        Joylog::AddOrGetData(info->mTime.mLastModified, 0x20,
                             JOYLOG_CHANNEL_MEMORY_CARD);
    Joylog::AddOrGetData(const_cast<char*>(info->mCompanyCode),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    Joylog::AddOrGetData(const_cast<char*>(info->mGameCode),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetMemcard()->IsListingOldSaveFiles()) {
        GetMemcard()->m_bOldSaveFileExists = true;
    } else if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bFoundAutoSaveFile = true;
    } else {
        if (bStrNCmp(g_GC_Disk_GameName, info->mGameCode, 4) == 0) {
            unsigned int fDefault = 0;
            unsigned int iSize = GetMemcard()->GetSize();
            int iGuessSize = info->mUserDataSize;
            if (info->mStatus != RealmcIface::STATUS_OK) {
                fDefault = 2;
            }
            if (GetMemcard()->IsTypeProfile()) {
                unsigned int sec = GetMemcard()->GetSize();
                GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault);
            } else {
                if (info->mStatus != RealmcIface::STATUS_OK) {
                    return;
                }
                int idx = GetMemcard()->m_EntryCount;
                bStrNCpy(GetMemcard()->m_pBuffer + idx * 0x10, info->mName,
                         0x10);
            }
            GetMemcard()->m_EntryCount++;
        }
    }
}

void MemcardCallbacks::FindEntriesDone(RealmcIface::CardStatus status) {
    JLog(MJ_FindEntriesDone);
    Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10,
                         JOYLOG_CHANNEL_MEMORY_CARD);
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_bListingForCreate = false;
    if (GetMemcard()->IsListingOldSaveFiles()) {
        GetMemcard()->EndListingOldSaveFiles();
    } else if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bCheckingCardForOverwrite = false;
        if (GetMemcard()->m_bFoundAutoSaveFile) {
            GetMemcard()->HandleAutoSaveOverwriteMessage();
        } else {
            GetMemcard()->DoAutoSave();
        }
    } else {
        cFEng::Get()->QueueGameMessage(0x5a051729,
                                       GetScreen()->GetPackageName(), 0xff);
        GetMemcard()->SetBootFound(GetMemcard()->m_EntryCount > 0);
    }
}

void MemcardCallbacks::Retry(RealmcIface::CardStatus status) {
    JLog(MJ_Retry);
    Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10,
                         JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetScreen() != nullptr) {
        GetScreen()->SetStringCheckingCard();
        if (GetMemcard()->GetOp() == MemoryCard::MO_List) {
            GetScreen()->EmptyFileList();
        }
    }
}

void MemcardCallbacks::Failed(RealmcIface::TaskResult result,
                              RealmcIface::CardStatus status) {
    JLog(MJ_Failed);
    JLog(status);
    JLog(result);
    if (GetMemcard()->IsWaitingForResponse() &&
        (GetMemcard()->GetOp() == MemoryCard::MO_Delete ||
         GetMemcard()->GetOp() == MemoryCard::MO_Load)) {
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
    if (GetMemcard()->IsAutoSaving() ||
        GetMemcard()->IsCheckingCardForAutoSave()) {
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
        if (result == RealmcIface::RESULT_CANCELLED ||
            status == RealmcIface::STATUS_CARD_DAMAGED) {
            msg = 0xfe202e3b;
        }
    }
    if (gMemcardSetup.GetMethod() == 0x60 &&
        GetMemcard()->GetOp() == MemoryCard::MO_List) {
        GetMemcard()->SetListingForCreate(false);
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        cFEng::Get()->QueueGameMessage(0x5a051729,
                                       GetScreen()->GetPackageName(), 0xff);
        return;
    }
    int op = GetMemcard()->GetOp();
    switch (op) {
        case MemoryCard::MO_AutoSave:
            break;

        case MemoryCard::MO_Save:
            if (status == RealmcIface::STATUS_NO_CARD)
                goto failed_check_autosave;
            if (static_cast<unsigned int>(status) >=
                static_cast<unsigned int>(RealmcIface::STATUS_NO_CARD)) {
                if (static_cast<unsigned int>(status) <=
                    static_cast<unsigned int>(RealmcIface::STATUS_CARD_FULL)) {
                    if (static_cast<unsigned int>(status) >=
                        static_cast<unsigned int>(RealmcIface::STATUS_WRONG_DEVICE))
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
        cFEng::Get()->QueueGameMessage(msg, GetScreen()->GetPackageName(),
                                       0xff);
    }
}

void MemcardCallbacks::CardChanged(RealmcIface::TaskResult result,
                                   RealmcIface::CardStatus status) {
    if ((result == RealmcIface::RESULT_RETRY &&
         status == RealmcIface::STATUS_CARD_CHANGED) ||
        status == RealmcIface::STATUS_OK) {
        cFEng::Get()->QueueGameMessage(0x3a2be557, nullptr, 0xff);
    } else if (result == RealmcIface::RESULT_CANCELLED) {
        cFEng::Get()->QueueGameMessage(0x8867412d, nullptr, 0xff);
    }
}

void MemcardCallbacks::CardChecked(const RealmcIface::CardInfo* info) {
    JLog(MJ_CardChecked);
    const_cast<RealmcIface::CardInfo*>(info)->mCardId =
        static_cast<RealmcIface::CardId>(Joylog::AddOrGetData(
            static_cast<unsigned int>(info->mCardId), 0x20,
            JOYLOG_CHANNEL_MEMORY_CARD));
    const_cast<RealmcIface::CardInfo*>(info)->mStatus =
        static_cast<RealmcIface::CardStatus>(Joylog::AddOrGetData(
            static_cast<unsigned int>(info->mStatus), 0x10,
            JOYLOG_CHANNEL_MEMORY_CARD));
    const_cast<RealmcIface::CardInfo*>(info)->mFreeSpace =
        Joylog::AddOrGetData(info->mFreeSpace, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::CardInfo*>(info)->mFreeFiles =
        Joylog::AddOrGetData(info->mFreeFiles, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::CardInfo*>(info)->mTotalSpace =
        Joylog::AddOrGetData(info->mTotalSpace, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    const_cast<RealmcIface::CardInfo*>(info)->mFreeSpaceOverLimit =
        Joylog::AddOrGetData(static_cast<unsigned int>(info->mFreeSpaceOverLimit),
                             1, JOYLOG_CHANNEL_MEMORY_CARD) != 0;
    const_cast<RealmcIface::CardInfo*>(info)->mTotalSpaceOverLimit =
        Joylog::AddOrGetData(
            static_cast<unsigned int>(info->mTotalSpaceOverLimit), 1,
            JOYLOG_CHANNEL_MEMORY_CARD) != 0;
    unsigned int msg = 0x8867412d;
    if (GetMemcard()->IsCheckingCardForAutoSave()) {
        GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
        GetMemcard()->m_LastError =
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(info) + 6);
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
        case RealmcIface::STATUS_OK: {
            if (!FEDatabase->bAutoSaveOverwriteConfirmed) {
                GetMemcard()->m_bCheckingCardForAutoSave = false;
                GetMemcard()->m_bCheckingCardForOverwrite = true;
                GetMemcard()->ShowMessages(true);
                char filter[32];
                UserProfile* profile = FEDatabase->GetMultiplayerProfile(0);
                bStrCat(filter, GetMemcard()->GetPrefix(),
                        profile->GetProfileName());
                GetMemcard()->m_bFoundAutoSaveFile = false;
                GetMemcard()->List(filter, nullptr);
                return;
            }
            GetMemcard()->DoAutoSave();
            return;
        }
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
        GetMemcard()->m_LastError =
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(info) + 6);
        UIMemcardBase* pScreen = GetScreen();
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
        FEDatabase->bAutoSaveOverwriteConfirmed = false;
    }
}

void MemcardCallbacks::SetAutosaveDone(RealmcIface::TaskResult res,
                                       RealmcIface::CardStatus status,
                                       RealmcIface::AutosaveState flag) {
    JLog(MJ_SetAutosaveDone);
    Joylog::AddOrGetData(static_cast<unsigned int>(res), 8,
                         JOYLOG_CHANNEL_MEMORY_CARD);
    status = static_cast<RealmcIface::CardStatus>(
        Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    flag = static_cast<RealmcIface::AutosaveState>(
        Joylog::AddOrGetData(static_cast<unsigned int>(flag), 0x20,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_bAutoSave = (flag == RealmcIface::AUTOSAVE_ENABLE);
    GetMemcard()->m_bAutoSaveCardPulled = false;
    GetMemcard()->m_bAutoSaveCardPulledDuringSave = false;
    if (GetMemcard()->m_bDisablingAutoSaveForSave) {
        GetMemcard()->m_bDisablingAutoSaveForSave = false;
        GetMemcard()->ShowMessages(true);
        cFEng::Get()->QueueGameMessage(0xc6c6b68f,
                                       GetMemcard()->IsMemcardScreenShowing()
                                           ? gMemcardSetup.mMemScreen
                                           : nullptr,
                                       0xff);
    } else {
        unsigned int msg = 0x461a18ee;
        if (status != RealmcIface::STATUS_OK &&
            flag != RealmcIface::AUTOSAVE_ENABLE) {
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
            if (flag != RealmcIface::AUTOSAVE_ENABLE &&
                FEDatabase->GetGameplaySettings()->AutoSaveOn) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                GetMemcard()->m_bCardRemoved = true;
            }
            GetMemcard()->EndAutoSave();
        } else {
            cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
        }
        if (flag == RealmcIface::AUTOSAVE_ENABLE) {
            if (gMemcardSetup.GetMethod() == 0xa0 &&
                FEDatabase->IsOptionsMode()) {
                FEDatabase->bAutoSaveOverwriteConfirmed = false;
            }
            FEDatabase->GetGameplaySettings()->AutoSaveOn = true;
            GetMemcard()->m_bCardRemoved = false;
        }
    }
}

void MemcardCallbacks::SetMonitorDone(RealmcIface::CardStatus status,
                                      RealmcIface::MonitorState state) {
    JLog(MJ_SetMonitorDone);
    status = static_cast<RealmcIface::CardStatus>(
        Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    state = static_cast<RealmcIface::MonitorState>(
        Joylog::AddOrGetData(static_cast<unsigned int>(state), 0x10,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    GetMemcard()->m_MemOp = MemoryCard::MO_NONE;
    GetMemcard()->m_bMonitorOn =
        (static_cast<unsigned int>(state) - 1u < 2u);
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

RealmcIface::TaskStatus MemcardCallbacks::LoadReady(const char* entryName,
                                                    unsigned int headerSize,
                                                    unsigned int bodySize,
                                                    char*& headerData,
                                                    char*& bodyData) {
    JLog(MJ_LoadReady);
    Joylog::AddOrGetData(const_cast<char*>(entryName),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    RealmcIface::TaskStatus res = RealmcIface::TASK_CANCEL;
    headerSize = Joylog::AddOrGetData(headerSize, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    bodySize = Joylog::AddOrGetData(bodySize, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    if (headerSize == 8 && bodySize == GetMemcard()->GetSize()) {
        res = RealmcIface::TASK_CONTINUE;
        bodyData = GetMemcard()->GetData();
        headerData = GetMemcard()->GetHeader();
    }
    return res;
}
