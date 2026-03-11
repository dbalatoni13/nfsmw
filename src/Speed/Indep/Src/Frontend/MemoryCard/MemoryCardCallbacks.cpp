#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void DisplayMessage(const wchar_t* msg, unsigned int nOptions, const wchar_t** options);

extern MemcardCallbacks gMemcardCallbacks;
extern const char* g_GC_Disk_GameName;

void DisplayStatus(int i) {}

MemoryCard* MemcardCallbacks::GetMemcard() {
    return MemoryCard::GetInstance();
}

UIMemcardBase* MemcardCallbacks::GetScreen() {
    return MemoryCard::GetInstance()->GetScreen();
}

void DisplayUnicode(const wchar_t* str) {
    const short* pWChar = reinterpret_cast< const short* >(str);
    if (*pWChar == 0) {
        return;
    }
    do {
        pWChar++;
    } while (*pWChar != 0);
}

void MemcardCallbacks::ShowMessage(const wchar_t* msg, unsigned int nOptions,
                                   const wchar_t** options) {
    if (GetMemcard()->IsMemcardScreenExiting()) {
        return;
    }
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_ShowMesssage);
    }
    Joylog::AddOrGetData(reinterpret_cast< unsigned short* >(const_cast< wchar_t* >(msg)),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    unsigned int nOpts = Joylog::AddOrGetData(nOptions, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    for (unsigned int i = 0; i < nOpts; i++) {
        Joylog::AddOrGetData(
            reinterpret_cast< unsigned short* >(const_cast< wchar_t* >(options[i])),
            JOYLOG_CHANNEL_MEMORY_CARD);
    }
    DisplayMessage(msg, nOpts, options);
    GetMemcard()->SetWaitingForResponse(true);
    if (!GetMemcard()->IsAutoSaving() || gMemcardSetup.GetMethod() == 0xb) {
        if ((GetMemcard()->GetOp() > 9 || GetMemcard()->GetOp() < 8 || nOpts != 0) &&
            GetScreen() != nullptr) {
            if (!GetScreen()->m_bInButtonAnimation) {
                GetScreen()->ShowMessage(
                    reinterpret_cast< const int* >(msg), nOpts,
                    reinterpret_cast< const int* >(options[0]),
                    reinterpret_cast< const int* >(options[1]),
                    reinterpret_cast< const int* >(options[2]));
            } else {
                if (GetMemcard()->GetPendingMessage() != nullptr) {
                    GetMemcard()->ReleasePendingMessage();
                }
                GetMemcard()->m_PendingMessage = new MemoryCardMessage(
                    reinterpret_cast< const int* >(msg), nOpts,
                    reinterpret_cast< const int** >(options));
            }
        }
    } else if (nOpts == 0) {
        GetMemcard()->SetWaitingForResponse(false);
    } else {
        GetMemcard()->m_PendingMessage = new MemoryCardMessage(
            reinterpret_cast< const int* >(msg), nOpts,
            reinterpret_cast< const int** >(options));
        GetMemcard()->HandleAutoSaveError();
    }
}

void MemcardCallbacks::ClearMessage() {
    if (GetMemcard()->IsAutoSaving()) {
        return;
    }
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_ClearMessage);
    }
    if ((GetMemcard()->GetOp() > 9 || GetMemcard()->GetOp() < 8) && GetScreen() != nullptr) {
        GetMemcard();
    }
}

void MemcardCallbacks::BootupCheckDone(RealmcIface::CardStatus status,
                                       RealmcIface::BootupCheckResults res) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_BootupCheckDone);
    }
    int iStatus = Joylog::AddOrGetData(static_cast< unsigned int >(status), 16,
                                       JOYLOG_CHANNEL_MEMORY_CARD);
    int found = Joylog::AddOrGetData(static_cast< unsigned int >(res.mEntryFound), 1,
                                     JOYLOG_CHANNEL_MEMORY_CARD);
    res.mEntryFound = (found != 0);
    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_pImp->DestructSaveInfo();
    unsigned short uStatus = static_cast< unsigned short >(iStatus);
    GetMemcard()->m_LastError = uStatus;
    GetMemcard()->m_SpecialError = uStatus;
    if ((iStatus == 0 || GetMemcard()->GetPendingMessage() == nullptr) &&
        iStatus != static_cast< int >(RealmcIface::CARD_UNKNOWN)) {
        GetMemcard()->m_pImp->BootupCheckDone(
            static_cast< RealmcIface::CardStatus >(iStatus), &res);
        GetMemcard()->m_bBootFoundFile = res.mEntryFound;
        if (!GetMemcard()->m_bRetryBootCheck) {
            cFEng::Get()->QueueGameMessage(0x461a18ee,
                                           GetScreen()->GetPackageName(), 0xff);
        } else {
            GetScreen()->SetStringCheckingCard();
        }
    } else {
        GetMemcard()->ReleasePendingMessage();
        MemoryCard* mc = GetMemcard();
        const char* entry;
        if (!GetMemcard()->IsAutoLoading() || FEDatabase->bProfileLoaded) {
            entry = nullptr;
        } else {
            entry = GetScreen()->m_FileName;
        }
        mc->BootupCheck(entry);
    }
}

void MemcardCallbacks::SaveCheckDone(RealmcIface::TaskResult result,
                                     RealmcIface::CardStatus status) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_SaveCheckDone);
    }
}

void MemcardCallbacks::SaveDone(const char* filename) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_SaveDone);
    }
    Joylog::AddOrGetData(const_cast< char* >(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetMemcard()->m_Type == MemoryCard::ST_PROFILE) {
        bFree(GetMemcard()->m_pBuffer);
    }
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_pBuffer = nullptr;
    GetMemcard()->m_MemOp = 0;
    FEDatabase->bProfileLoaded = true;
    FEDatabase->bIsOptionsDirty = false;
    GetMemcard()->m_bCardRemoved = false;
    if (!GetMemcard()->IsManualSave() || gMemcardSetup.GetMethod() == 0xb) {
        if (GetMemcard()->IsAutoSaving() || gMemcardSetup.GetMethod() == 0xb) {
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
            if (gMemcardSetup.GetMethod() == 0xb) {
                cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
            }
        }
    } else {
        if (!FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            cFEng::Get()->QueueGameMessage(0x461a18ee, nullptr, 0xff);
        } else {
            GetMemcard()->m_bRetryAutoSave = false;
            GetMemcard()->SetAutoSaveEnabled(true);
        }
    }
}

RealmcIface::DataStatus MemcardCallbacks::CheckLoadedData(const char* data) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_CheckLoadedData);
    }
    return RealmcIface::DATA_OK;
}

void MemcardCallbacks::LoadDone(const char* filename) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_LoadDone);
    }
    Joylog::AddOrGetData(const_cast< char* >(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    MemoryCard* mc = GetMemcard();
    if (Joylog::IsReplaying()) {
        Joylog::GetData(mc->m_Header, 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        Joylog::AddData(mc->m_Header, 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    char* pBuffer = GetMemcard()->m_pBuffer;
    unsigned int dataSize = GetMemcard()->m_DataSize;
    if (Joylog::IsReplaying()) {
        Joylog::GetData(pBuffer, dataSize, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    if (Joylog::IsCapturing()) {
        Joylog::AddData(pBuffer, dataSize, JOYLOG_CHANNEL_MEMORY_CARD);
    }
    int header0 = GetMemcard()->m_Header[0];
    int header1 = GetMemcard()->m_Header[1];
    MemoryCard::s_pThis->m_MemOp = 0;
    if (header0 == 0x10d &&
        header1 == static_cast< int >(GetMemcard()->m_DataSize) &&
        GetMemcard()->m_Type == MemoryCard::ST_PROFILE) {
        char* buf = GetMemcard()->m_pBuffer;
        unsigned int size = GetMemcard()->m_DataSize;
        int player = GetMemcard()->m_nPlayer;
        if (!FEDatabase->LoadUserProfileFromBuffer(buf, size, player)) {
            GetMemcard()->ShowMessages(false);
            FEDatabase->RestoreFromBackupDB();
            unsigned int msg = 0xf35d144e;
            cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
        } else {
            FEDatabase->DeallocBackupDB();
            if (GetMemcard()->m_nPlayer != 0) {
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
                goto cleanup;
            }
            unsigned int msg = 0x461a18ee;
            if (gMemcardSetup.GetMethod() == 0x2) {
                msg = 0xa4bb7ae1;
            }
            cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
        }
    } else {
        FEDatabase->RestoreFromBackupDB();
        cFEng::Get()->QueueGameMessage(0xf35d144e, nullptr, 0xff);
    }
cleanup:
    if (GetMemcard()->m_pBuffer != nullptr) {
        bFree(GetMemcard()->m_pBuffer);
        GetMemcard()->m_pBuffer = nullptr;
    }
    FEDatabase->DeallocBackupDB();
}

void MemcardCallbacks::DeleteDone(const char* filename) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_DeleteDone);
    }
    Joylog::AddOrGetData(const_cast< char* >(filename), JOYLOG_CHANNEL_MEMORY_CARD);
    GetMemcard();
    int prefixLen = GetMemcard()->GetPrefixLength();
    const char* profileName = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
    if (bStrCmp(filename + prefixLen, profileName) == 0) {
        FEDatabase->DefaultProfile();
        FEDatabase->bProfileLoaded = false;
    }
    GetMemcard()->m_MemOp = 0;
    cFEng::Get()->QueueGameMessage(0x461a18ee, GetScreen()->GetPackageName(), 0xff);
}

void MemcardCallbacks::ClearEntries() {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_ClearEntries);
    }
}

void MemcardCallbacks::FoundEntry(const RealmcIface::EntryInfo* info) {
    RealmcIface::EntryInfo* pInfo = const_cast< RealmcIface::EntryInfo* >(info);
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_FoundEntry);
    }
    Joylog::AddOrGetData(pInfo->mName, JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mStatus = static_cast< RealmcIface::CardStatus >(
        Joylog::AddOrGetData(static_cast< unsigned int >(pInfo->mStatus), 16,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    pInfo->mEntryBlocks = Joylog::AddOrGetData(pInfo->mEntryBlocks, 32,
                                               JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mUserDataSize = Joylog::AddOrGetData(pInfo->mUserDataSize, 32,
                                                JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mTime.mCreated = Joylog::AddOrGetData(pInfo->mTime.mCreated, 32,
                                                 JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mTime.mLastAccessed = Joylog::AddOrGetData(pInfo->mTime.mLastAccessed, 32,
                                                      JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mTime.mLastModified = Joylog::AddOrGetData(pInfo->mTime.mLastModified, 32,
                                                      JOYLOG_CHANNEL_MEMORY_CARD);
    Joylog::AddOrGetData(reinterpret_cast< char* >(pInfo->mCompanyCode),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    Joylog::AddOrGetData(reinterpret_cast< char* >(pInfo->mGameCode),
                         JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->m_bOldSaveFileExists = true;
    } else if (GetMemcard()->m_bCheckingCardForOverwrite) {
        GetMemcard()->m_bFoundAutoSaveFile = true;
    } else {
        if (bStrNCmp(g_GC_Disk_GameName, pInfo->mGameCode, 4) == 0) {
            int flag = 0;
            GetMemcard();
            unsigned int size = pInfo->mUserDataSize;
            if (pInfo->mStatus != RealmcIface::STATUS_OK) {
                flag = 2;
            }
            if (GetMemcard()->m_Type == MemoryCard::ST_PROFILE) {
                GetMemcard();
                GetScreen()->AddItem(pInfo->mName, "", size, flag);
            } else {
                if (pInfo->mStatus != RealmcIface::STATUS_OK) {
                    return;
                }
                int idx = GetMemcard()->m_EntryCount;
                bStrNCpy(GetMemcard()->m_pBuffer + idx * 16, pInfo->mName, 16);
            }
            GetMemcard()->m_EntryCount++;
        }
    }
}

void MemcardCallbacks::FindEntriesDone(RealmcIface::CardStatus status) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_FindEntriesDone);
    }
    Joylog::AddOrGetData(static_cast< unsigned int >(status), 16, JOYLOG_CHANNEL_MEMORY_CARD);
    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_bListingForCreate = false;
    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->EndListingOldSaveFiles();
    } else {
        if (GetMemcard()->m_bCheckingCardForOverwrite) {
            GetMemcard()->m_bCheckingCardForOverwrite = false;
            if (!GetMemcard()->m_bFoundAutoSaveFile) {
                GetMemcard()->DoAutoSave();
            } else {
                GetMemcard()->HandleAutoSaveOverwriteMessage();
            }
        } else {
            cFEng::Get()->QueueGameMessage(0x5a051729, GetScreen()->GetPackageName(), 0xff);
            GetMemcard()->m_bBootFoundFile = (GetMemcard()->m_EntryCount > 0);
        }
    }
}

void MemcardCallbacks::Retry(RealmcIface::CardStatus status) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_Retry);
    }
    Joylog::AddOrGetData(static_cast< unsigned int >(status), 16, JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetScreen() != nullptr) {
        GetScreen()->SetStringCheckingCard();
        if (GetMemcard()->GetOp() == 7) {
            GetScreen()->EmptyFileList();
        }
    }
}

void MemcardCallbacks::Failed(RealmcIface::TaskResult result,
                              RealmcIface::CardStatus status) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_Failed);
    }
    unsigned int uStatus = Joylog::AddOrGetData(static_cast< unsigned int >(status), 16,
                                                JOYLOG_CHANNEL_MEMORY_CARD);
    int iResult = Joylog::AddOrGetData(static_cast< unsigned int >(result), 8,
                                       JOYLOG_CHANNEL_MEMORY_CARD);
    if (GetMemcard()->IsWaitingForResponse() &&
        (GetMemcard()->GetOp() == 6 || GetMemcard()->GetOp() == 5)) {
        GetMemcard()->m_MemOp = 0;
        if (GetMemcard()->GetOp() == 6) {
            GetMemcard()->Delete(nullptr);
            return;
        }
        GetMemcard()->Load(nullptr);
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
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->EndAutoSave();
        if (gMemcardSetup.GetMethod() == 0xb) {
            cFEng::Get()->QueueGameMessage(0x8867412d, nullptr, 0xff);
        }
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        return;
    }
    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->EndListingOldSaveFiles();
        return;
    }
    if (GetMemcard()->m_bRetryAutoSave) {
        GetMemcard()->m_bRetryAutoSave = false;
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        if (iResult == 2 || uStatus == 4) {
            msg = 0xfe202e3b;
        }
    }
    if (gMemcardSetup.GetMethod() == 0x6 && GetMemcard()->GetOp() == 7) {
        GetMemcard()->m_bListingForCreate = false;
        GetMemcard()->m_MemOp = 0;
        cFEng::Get()->QueueGameMessage(0x5a051729, GetScreen()->GetPackageName(), 0xff);
        return;
    }
    int op = GetMemcard()->GetOp();
    unsigned short uStat = static_cast< unsigned short >(uStatus);
    if (op == 4) {
    } else if (op < 5) {
        if (op == 1) {
            GetMemcard()->m_pImp->DestructSaveInfo();
        } else if (op != 3) {
        } else {
            if ((uStatus == 1 || (uStatus != 0 && uStatus < 7 && uStatus > 4)) &&
                gMemcardSetup.GetMethod() == 0x6) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            }
            msg = 0xdc12af2e;
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            goto free_buffer;
        }
    } else if (op != 5) {
        if (op == 7 && GetMemcard()->m_bInBootSequence) {
            msg = 0x8867412d;
        }
        goto set_error;
    }
free_buffer:
    if (GetMemcard()->m_Type == MemoryCard::ST_PROFILE) {
        bFree(GetMemcard()->m_pBuffer);
    }
    GetMemcard()->m_pBuffer = nullptr;
    GetMemcard()->m_SpecialError = uStat;
set_error:
    GetMemcard()->m_LastError = uStat;
    GetMemcard()->m_MemOp = 0;
    DisplayStatus(uStatus);
    if (uStatus == 0xd) {
        GetMemcard()->BootupCheck(nullptr);
        GetMemcard()->m_bRetryBootCheck = true;
    } else {
        cFEng::Get()->QueueGameMessage(msg, GetScreen()->GetPackageName(), 0xff);
    }
}

void MemcardCallbacks::CardChanged(RealmcIface::TaskResult result,
                                   RealmcIface::CardStatus status) {
    if ((result == RealmcIface::RESULT_RETRY && status == RealmcIface::STATUS_CARD_UNFORMATTED) ||
        status == RealmcIface::STATUS_OK) {
        cFEng::Get()->QueueGameMessage(0x3a2be557, nullptr, 0xff);
    } else if (result == RealmcIface::RESULT_CANCELLED) {
        cFEng::Get()->QueueGameMessage(0x8867412d, nullptr, 0xff);
    }
}

void MemcardCallbacks::CardChecked(const RealmcIface::CardInfo* info) {
    RealmcIface::CardInfo* pInfo = const_cast< RealmcIface::CardInfo* >(info);
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_CardChecked);
    }
    pInfo->mCardId = static_cast< RealmcIface::CardId >(
        Joylog::AddOrGetData(static_cast< unsigned int >(pInfo->mCardId), 32,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    pInfo->mStatus = static_cast< RealmcIface::CardStatus >(
        Joylog::AddOrGetData(static_cast< unsigned int >(pInfo->mStatus), 16,
                             JOYLOG_CHANNEL_MEMORY_CARD));
    pInfo->mFreeSpace = Joylog::AddOrGetData(pInfo->mFreeSpace, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mFreeFiles = Joylog::AddOrGetData(pInfo->mFreeFiles, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mTotalSpace = Joylog::AddOrGetData(pInfo->mTotalSpace, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    int freeOverLimit = Joylog::AddOrGetData(
        static_cast< unsigned int >(pInfo->mFreeSpaceOverLimit), 1, JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mFreeSpaceOverLimit = (freeOverLimit != 0);
    int totalOverLimit = Joylog::AddOrGetData(
        static_cast< unsigned int >(pInfo->mTotalSpaceOverLimit), 1, JOYLOG_CHANNEL_MEMORY_CARD);
    pInfo->mTotalSpaceOverLimit = (totalOverLimit != 0);
    if (!GetMemcard()->IsCheckingCardForAutoSave()) {
        MemoryCard::SetMessageMode(1, true);
        unsigned int msg = 0x8867412d;
        if (pInfo->mStatus == RealmcIface::STATUS_OK) {
            msg = 0x461a18ee;
        }
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->m_LastError = static_cast< unsigned short >(pInfo->mStatus);
        if (msg == 0) {
            return;
        }
        if (GetScreen() == nullptr) {
            return;
        }
        cFEng::Get()->QueueGameMessage(msg, GetScreen()->GetPackageName(), 0xff);
    } else {
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->m_LastError = static_cast< unsigned short >(pInfo->mStatus);
        unsigned int cardStatus = pInfo->mStatus;
        if (cardStatus != 2) {
            if (cardStatus < 3) {
                if (cardStatus != 0) {
                    if (cardStatus != 1) {
                        return;
                    }
                    GetMemcard()->HandleAutoSaveError();
                    return;
                }
                if (!FEDatabase->bAutoSaveOverwriteConfirmed) {
                    GetMemcard()->m_bCheckingCardForAutoSave = false;
                    GetMemcard()->m_bCheckingCardForOverwrite = true;
                    GetMemcard()->ShowMessages(true);
                    char entryname[32];
                    const char* prefix = GetMemcard()->GetPrefix();
                    const char* profileName =
                        FEDatabase->CurrentUserProfiles[0]->GetProfileName();
                    bStrCat(entryname, prefix, profileName);
                    GetMemcard()->m_bFoundAutoSaveFile = false;
                    GetMemcard()->List(entryname, nullptr);
                    return;
                }
                goto doAutoSave;
            }
            if (cardStatus > 7) {
                return;
            }
            if (cardStatus < 4) {
                return;
            }
        }
        GetMemcard()->m_bFoundAutoSaveFile = true;
    doAutoSave:
        GetMemcard()->DoAutoSave();
    }
}

void MemcardCallbacks::CardRemoved() {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_CardRemoved);
    }
    GetMemcard()->m_bAutoSaveCardPulled = true;
    if (GetMemcard()->GetOp() == 3) {
        GetMemcard()->m_bAutoSaveCardPulledDuringSave = true;
    }
    if (!GetMemcard()->m_bCheckingCardForOverwrite) {
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
    } else {
        GetMemcard()->HandleAutoSaveError();
    }
}

void MemcardCallbacks::SetAutosaveDone(RealmcIface::TaskResult res,
                                       RealmcIface::CardStatus status,
                                       RealmcIface::AutosaveState flag) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_SetAutosaveDone);
    }
    Joylog::AddOrGetData(static_cast< unsigned int >(res), 8, JOYLOG_CHANNEL_MEMORY_CARD);
    unsigned int uStatus = Joylog::AddOrGetData(static_cast< unsigned int >(status), 16,
                                                JOYLOG_CHANNEL_MEMORY_CARD);
    int iFlag = Joylog::AddOrGetData(static_cast< unsigned int >(flag), 32,
                                     JOYLOG_CHANNEL_MEMORY_CARD);
    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_bAutoSave = (iFlag == 1);
    GetMemcard()->m_bAutoSaveCardPulled = false;
    GetMemcard()->m_bAutoSaveCardPulledDuringSave = false;
    if (!GetMemcard()->m_bDisablingAutoSaveForSave) {
        unsigned int msg = 0x461a18ee;
        if (uStatus != 0 && iFlag != 1) {
            if (uStatus < 9 && uStatus < 4 && uStatus != 2 && (uStatus > 2 || uStatus == 1)) {
                msg = 0xb57fdb17;
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            } else {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            }
        }
        if (gMemcardSetup.mPreviousCommand == 0x20) {
            msg = 0xa4bb7ae1;
        }
        if (!GetMemcard()->IsAutoSaving()) {
            cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
        } else {
            if (iFlag != 1 && FEDatabase->GetGameplaySettings()->AutoSaveOn) {
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                GetMemcard()->m_bCardRemoved = true;
            }
            GetMemcard()->EndAutoSave();
        }
        if (iFlag == 1) {
            if (gMemcardSetup.GetMethod() == 0xa && FEDatabase->IsOptionsMode()) {
                FEDatabase->bAutoSaveOverwriteConfirmed = false;
            }
            FEDatabase->GetGameplaySettings()->AutoSaveOn = true;
            GetMemcard()->m_bCardRemoved = false;
        }
    } else {
        GetMemcard()->m_bDisablingAutoSaveForSave = false;
        GetMemcard()->ShowMessages(true);
        const char* pkg = nullptr;
        if (GetMemcard()->IsMemcardScreenShowing()) {
            pkg = gMemcardSetup.mMemScreen;
        }
        cFEng::Get()->QueueGameMessage(0xc6c6b68f, pkg, 0xff);
    }
}

void MemcardCallbacks::SetMonitorDone(RealmcIface::CardStatus status,
                                      RealmcIface::MonitorState state) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_SetMonitorDone);
    }
    int iStatus = Joylog::AddOrGetData(static_cast< unsigned int >(status), 16,
                                       JOYLOG_CHANNEL_MEMORY_CARD);
    int iState = Joylog::AddOrGetData(static_cast< unsigned int >(state), 16,
                                      JOYLOG_CHANNEL_MEMORY_CARD);
    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_bMonitorOn = (static_cast< unsigned int >(iState) - 1 < 2);
    unsigned int msg;
    if (iState == 1) {
        if (iStatus == 0) {
            msg = 0x54b3ac6c;
        } else {
            msg = 0x8867412d;
        }
    } else {
        if (cFEng::Get()->IsPackagePushed("MemoryCard.fng")) {
            msg = 0xeb29392a;
        } else if (MemoryCard::s_pThis->m_bMemcardScreenShowing) {
            msg = 0x8867412d;
        } else {
            goto send;
        }
    }
send:
    cFEng::Get()->QueueGameMessage(msg, nullptr, 0xff);
}

RealmcIface::TaskStatus MemcardCallbacks::LoadReady(const char* entryName,
                                                    unsigned int headerSize,
                                                    unsigned int bodySize,
                                                    char*& headerData,
                                                    char*& bodyData) {
    if (Joylog::IsCapturing()) {
        CaptureJoyOp(MJ_LoadReady);
    }
    Joylog::AddOrGetData(const_cast< char* >(entryName), JOYLOG_CHANNEL_MEMORY_CARD);
    unsigned int hSize = Joylog::AddOrGetData(headerSize, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    unsigned int bSize = Joylog::AddOrGetData(bodySize, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    if (hSize == 8 && bSize == static_cast< unsigned int >(GetMemcard()->m_DataSize)) {
        bodyData = GetMemcard()->m_pBuffer;
        headerData = GetMemcard()->GetHeader();
        return RealmcIface::TASK_CONTINUE;
    }
    return RealmcIface::TASK_CANCEL;
}

void IJoyHelper::EmulateMemoryCardLibrary(int aJoyOp) {
    char* buf = new char[0x400];
    const wchar_t* options[4];
    options[0] = reinterpret_cast< const wchar_t* >(buf + 0x338);
    options[1] = reinterpret_cast< const wchar_t* >(buf + 0x36a);
    options[2] = reinterpret_cast< const wchar_t* >(buf + 0x39c);
    options[3] = reinterpret_cast< const wchar_t* >(buf + 0x3ce);
    RealmcIface::CardInfo cardInfo;
    RealmcIface::EntryInfo entryInfo;
    entryInfo.mName = buf;
    if (aJoyOp == MJ_ClearEntries) {
        gMemcardCallbacks.ClearEntries();
    } else if (aJoyOp < MJ_FoundEntry) {
        if (aJoyOp == MJ_SaveCheckDone) {
            gMemcardCallbacks.SaveCheckDone(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK);
        } else if (aJoyOp < MJ_SaveDone) {
            if (aJoyOp == MJ_ShowMesssage) {
                gMemcardCallbacks.ShowMessage(reinterpret_cast< const wchar_t* >(buf), 0, options);
            } else if (aJoyOp > MJ_ShowMesssage) {
                if (aJoyOp == MJ_ClearMessage) {
                    gMemcardCallbacks.ClearMessage();
                } else if (aJoyOp == MJ_BootupCheckDone) {
                    RealmcIface::BootupCheckResults res;
                    res.mFirstGoodCard = static_cast< RealmcIface::CardId >(0);
                    res.mEntryFound = false;
                    res.mNumBlocksNeeded = 0;
                    gMemcardCallbacks.BootupCheckDone(RealmcIface::STATUS_OK, res);
                }
            }
        } else if (aJoyOp == MJ_CheckLoadedData) {
            gMemcardCallbacks.CheckLoadedData(buf);
        } else if (aJoyOp < MJ_CheckLoadedData) {
            gMemcardCallbacks.SaveDone(buf);
        } else if (aJoyOp == MJ_LoadDone) {
            gMemcardCallbacks.LoadDone(buf);
        } else if (aJoyOp == MJ_DeleteDone) {
            gMemcardCallbacks.DeleteDone(buf);
        }
    } else if (aJoyOp == MJ_CardChecked) {
        gMemcardCallbacks.CardChecked(&cardInfo);
    } else if (aJoyOp < MJ_CardRemoved) {
        if (aJoyOp == MJ_FindEntriesDone) {
            gMemcardCallbacks.FindEntriesDone(RealmcIface::STATUS_OK);
        } else if (aJoyOp < MJ_FindEntriesDone) {
            gMemcardCallbacks.FoundEntry(&entryInfo);
        } else if (aJoyOp == MJ_Retry) {
            gMemcardCallbacks.Retry(RealmcIface::STATUS_OK);
        } else if (aJoyOp == MJ_Failed) {
            gMemcardCallbacks.Failed(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK);
        }
    } else if (aJoyOp == MJ_SetAutosaveDone) {
        gMemcardCallbacks.SetAutosaveDone(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK,
                                          RealmcIface::AUTOSAVE_DISABLE);
    } else if (aJoyOp < MJ_SetAutosaveDone) {
        gMemcardCallbacks.CardRemoved();
    } else if (aJoyOp == MJ_LoadReady) {
        char* hdr = buf + 1;
        gMemcardCallbacks.LoadReady(buf, 0, 0, hdr, hdr);
    } else if (aJoyOp == MJ_SetMonitorDone) {
        gMemcardCallbacks.SetMonitorDone(RealmcIface::STATUS_OK, RealmcIface::MONITOR_ON);
    }
    if (buf != nullptr) {
        delete[] buf;
    }
}
