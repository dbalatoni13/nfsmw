#include <string.h>

#include <dolphin/card.h>

#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#include "../../../include/common/realmemcard/gc_blockcalculator.h"
#include "../../../include/common/realmemcard/memcard_utilities.h"

namespace RealmcIface {

inline CardId _ChangeToRealmcIfaceCardId(Realmc::CardID cardId) {
    return static_cast<CardId>(1 << (cardId.slot * 4));
}

Realmc::CardID _ChangeToRealmcCardId(CardId cardId) {
    Realmc::CardID memcard;

    memcard.slot = cardId == PORT1_DEFAULT ? 0 : 1;
    return memcard;
}

void _SplitPath(const char *path, char *filename) {
    char tempPath[63];
    char *separator;
    unsigned int length;

    {
        unsigned int pos;

        strcpy(tempPath, path);
        separator = nullptr;
        pos = strlen(path);
        if (pos != 0) {
            do {
                if (tempPath[pos] == '\\' || tempPath[pos] == '/') {
                    tempPath[pos] = 0;
                    separator = tempPath + pos;
                }
                pos--;
            } while (separator == nullptr && pos != 0);
        }
    }
    strncpy(filename, separator + 1, 0x1f);
}

MemcardInterfaceImpl::MemcardInterfaceImpl(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo)
    : mISystem(*iSystem)
    , mIGame(iGame)
    , mGameInfo(*gameInfo)
    , mActiveCard()
    , mAutosaveCard()
    , mAutosaveEnabled(false)
    , mActiveTask(TASK_NONE)
    , mActiveSubtask(SUBTASK_NONE)
    , mUserChoice(CHOICE_NONE)
    , mMessageShowing(false)
    , mHiddenMessages(0)
    , mForceMessages(0)
    , mIsResettable(true)
    , mMonitorState(MONITOR_OFF)
    , mCardInfo()
    , mStartGameInfo()
    , mFileSize(0)
    , mEntryInfo()
    , mBlockSize(0)
    , mFileInfo()
    , mFilehandle(nullptr)
    , mFileHeader() {
    this->mFileHeader.Clear();
    this->mUserHeader = nullptr;
    this->mUserBody = nullptr;
    this->mEntryFound = false;
    this->mDataFormat = FORMAT_LAYER2;
    this->mTaskStatus = TASK_CONTINUE;
    this->mBlocksNeeded = 0;
    this->mEntryList = nullptr;
    this->mNumEntries = 0;
    this->mCurEntry = 0;
    this->mInsufficientSpaceMsg = nullptr;
    this->mFilesNeeded = 0;
    this->mTaskManager = new TaskManager(this, iGame);
    memset(this->mEntryName, 0, 0x20);
    memset(&this->mFileInfo, 0, 0x30);
    memset(&this->mStartGameInfo, 0, 0x40);
    this->mIMemcard = Realmc::Interface::CreateInstance(Realmc::SystemInterface(iSystem));
    this->SetActiveCard(PORT1_DEFAULT);
}

void MemcardInterfaceImpl::BootupCheck(const BootupCheckParams *params) {
    unsigned int nBlocksNeeded = 0;
    unsigned int nFilesNeeded = 0;

    {
        unsigned int iSaveType;

        for (iSaveType = 0; iSaveType < params->mNumSaveTypes; iSaveType++) {
            SaveInfo *saveInfo = params->mSaveReqs[iSaveType]->mSaveInfo;
            if (saveInfo->mHeaderSize + saveInfo->mBodySize != 0) {
                nBlocksNeeded += this->CalcSaveSize(saveInfo, FORMAT_LAYER2) * params->mSaveReqs[iSaveType]->mNumSaves;
                nFilesNeeded += params->mSaveReqs[iSaveType]->mNumSaves;
            }
        }
    }

    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, params->mEntryNamePattern, 0x1f);
    bool checkAllSlots = false;
    bool foundFirstCardId = false;
    Realmc::CardID checkCardId;
    if ((params->mValidCardIds & PORT1_DEFAULT) != 0) {
        checkCardId = _ChangeToRealmcCardId(PORT1_DEFAULT);
        foundFirstCardId = true;
    }
    if ((params->mValidCardIds & PORT2_DEFAULT) != 0) {
        if (foundFirstCardId) {
            checkAllSlots = true;
        } else {
            checkCardId = _ChangeToRealmcCardId(PORT2_DEFAULT);
        }
    }

    memset(&this->mStartGameInfo, 0, 0x40);
    this->mStartGameInfo.totalBlocksNeeded = nBlocksNeeded;
    this->mStartGameInfo.totalFilesNeeded = nFilesNeeded;
    this->mStartGameInfo.checkAllSlots = checkAllSlots;
    this->mStartGameInfo.checkCardID = checkCardId;
    this->mStartGameInfo.fileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->mStartGameInfo.fileInfo.fileName = Realmc::FILENAME_ALL_FILES;
    this->mStartGameInfo.fileInfo.usingMultipleSaves = this->mGameInfo.mMultipleSaveTypesUsed;
    this->mEntryFound = false;
    this->mActiveTask = TASK_BOOTUPCHECK;
    this->mIMemcard->TrcStartGame(this->mStartGameInfo);
    if (this->mGameInfo.mMultipleSaveTypesUsed) {
        this->_MakeInsufficientSpaceMessage(params->mNumSaveTypes, params->mSaveReqs);
    }
}

void MemcardInterfaceImpl::SaveCheck(const char *entryName, unsigned int nSaveReqs, SaveReq **saveReqs) {
    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, entryName, 0x1f);
    this->mBlocksNeeded = 0;
    this->mFilesNeeded = 0;
    {
        unsigned int iSaveReq;

        for (iSaveReq = 0; iSaveReq < nSaveReqs; iSaveReq++) {
            this->mBlocksNeeded += this->CalcSaveSize(saveReqs[iSaveReq]->mSaveInfo, FORMAT_LAYER2) * saveReqs[iSaveReq]->mNumSaves;
            this->mFilesNeeded += saveReqs[iSaveReq]->mNumSaves;
        }
    }
    SaveInfo *saveInfo;

    saveInfo = saveReqs[0]->mSaveInfo;
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.comment1 = const_cast<char *>(saveInfo->mGcInfo.mComment1);
    this->mFileInfo.sizeofcomment1 = saveInfo->mGcInfo.mComment1Size;
    this->mFileInfo.comment2 = const_cast<char *>(saveInfo->mGcInfo.mComment2);
    this->mFileInfo.sizeofcomment2 = saveInfo->mGcInfo.mComment2Size;
    this->mFileInfo.gcIconDataInfo = static_cast<Realmc::GCIconDataInfo *>(saveInfo->mGcInfo.mIconDataInfo);
    this->mFileInfo.gcBannerDataInfo = static_cast<Realmc::GCBannerDataInfo *>(saveInfo->mGcInfo.mBannerDataInfo);
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
        this->mFileInfo.fileTypeName = const_cast<wchar_t *>(saveInfo->mTypeName);
        this->mFileInfo.fileContentName = const_cast<wchar_t *>(saveInfo->mContentName);
    this->mFileInfo.usingMultipleSaves = nSaveReqs != 1;
    this->mFileInfo.fileByteSize = (saveInfo->mHeaderSize + sizeof(FileHeader)) + saveInfo->mBodySize;
    this->mIMemcard->TrcSaveFile(this->mActiveCard, this->mFileInfo, 0, this->mBlocksNeeded, this->mFilesNeeded);
    this->mActiveTask = TASK_SAVECHECK;
    if (nSaveReqs > 1) {
        this->_MakeInsufficientSpaceMessage(nSaveReqs, saveReqs);
    }
}

void MemcardInterfaceImpl::Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo) {
    Realmc::BlockCalculator *calculator;
    unsigned int filesize;

    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, entryName, 0x1f);
    this->mUserHeader = const_cast<char *>(header);
    this->mUserBody = const_cast<char *>(body);
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.comment1 = const_cast<char *>(saveInfo->mGcInfo.mComment1);
    this->mFileInfo.sizeofcomment1 = saveInfo->mGcInfo.mComment1Size;
    this->mFileInfo.comment2 = const_cast<char *>(saveInfo->mGcInfo.mComment2);
    this->mFileInfo.sizeofcomment2 = saveInfo->mGcInfo.mComment2Size;
    this->mFileInfo.gcIconDataInfo = static_cast<Realmc::GCIconDataInfo *>(saveInfo->mGcInfo.mIconDataInfo);
    this->mFileInfo.gcBannerDataInfo = static_cast<Realmc::GCBannerDataInfo *>(saveInfo->mGcInfo.mBannerDataInfo);
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->mFileInfo.fileTypeName = const_cast<wchar_t *>(saveInfo->mTypeName);
    this->mFileInfo.fileContentName = const_cast<wchar_t *>(saveInfo->mContentName);
    this->mFileInfo.usingMultipleSaves = this->mGameInfo.mMultipleSaveTypesUsed;
    this->mFileInfo.fileByteSize = (saveInfo->mHeaderSize + sizeof(FileHeader)) + saveInfo->mBodySize;
    calculator = static_cast<Realmc::BlockCalculator *>(this->mIMemcard->GetBlockCalculator());
    calculator->Clear();
    calculator->SetFileInfo(this->mActiveCard, this->mFileInfo);
    filesize = this->mIMemcard->GetBlockSize(this->mActiveCard) * calculator->GetResult();
    this->mFileHeader.Init(saveInfo->mHeaderSize, saveInfo->mBodySize, filesize,
                           this->_CalcSignature(header, saveInfo->mHeaderSize),
                           this->_CalcSignature(body, saveInfo->mBodySize));
    memset(&this->mFileHeader.mFileHeaderSignature, 0, sizeof(this->mFileHeader.mFileHeaderSignature));
    this->mFileHeader.mFileHeaderSignature = this->_CalcSignature(&this->mFileHeader, 0x18);
    this->mIMemcard->TrcSaveFile(this->mActiveCard, this->mFileInfo, 1, 0, 0);
    this->mActiveTask = TASK_SAVE;
}

unsigned int MemcardInterfaceImpl::_CalcSignature(const void *data, unsigned int size) {
    return RealmcUtils::Crc32(data, size);
}

inline void FileHeader::Clear() {
    this->mFileHeaderVersion = 0;
    this->mUserHeaderSize = 0;
    this->mUserBodySize = 0;
    this->mFileSize = 0;
    memset(&this->mUserHeaderSignature, 0, sizeof(this->mUserHeaderSignature));
    memset(&this->mUserBodySignature, 0, sizeof(this->mUserBodySignature));
    memset(&this->mFileHeaderSignature, 0, sizeof(this->mFileHeaderSignature));
}

unsigned int MemcardInterfaceImpl::CalcSaveSize(const SaveInfo *saveInfo, const DataFormat) {
    const char dummy = 0x21;
    Realmc::FileInfo fileInfo;
    Realmc::BlockCalculator *calculator;

    memset(&fileInfo, 0, 0x30);
    fileInfo.fileName = &dummy;
    fileInfo.fileByteSize = (saveInfo->mHeaderSize + sizeof(FileHeader)) + saveInfo->mBodySize;
    fileInfo.comment1 = const_cast<char *>(saveInfo->mGcInfo.mComment1);
    fileInfo.sizeofcomment1 = saveInfo->mGcInfo.mComment1Size;
    fileInfo.comment2 = const_cast<char *>(saveInfo->mGcInfo.mComment2);
    fileInfo.sizeofcomment2 = saveInfo->mGcInfo.mComment2Size;
    fileInfo.gcIconDataInfo = static_cast<Realmc::GCIconDataInfo *>(saveInfo->mGcInfo.mIconDataInfo);
    fileInfo.gcBannerDataInfo = static_cast<Realmc::GCBannerDataInfo *>(saveInfo->mGcInfo.mBannerDataInfo);
    fileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    fileInfo.fileTypeName = const_cast<wchar_t *>(saveInfo->mTypeName);
    fileInfo.fileContentName = const_cast<wchar_t *>(saveInfo->mContentName);
    fileInfo.usingMultipleSaves = this->mGameInfo.mMultipleSaveTypesUsed;
    calculator = static_cast<Realmc::BlockCalculator *>(this->mIMemcard->GetBlockCalculator());
    calculator->Clear();
    calculator->SetFileInfo(this->mActiveCard, fileInfo);
    return calculator->GetResult();
}

void MemcardInterfaceImpl::CheckCard(CardId cardId) {
    this->mCardInfo.Clear();
    if (cardId == CARD_UNKNOWN) {
        this->mIMemcard->TrcGetCardInfo(this->mActiveCard);
    } else {
        this->mIMemcard->TrcGetCardInfo(_ChangeToRealmcCardId(cardId));
    }
    this->mActiveTask = TASK_CHECKCARD;
}

void MemcardInterfaceImpl::SetActiveCard(CardId cardId) {
    this->mActiveCard = _ChangeToRealmcCardId(cardId);
    this->mBlockSize = this->mIMemcard->GetBlockSize(this->mActiveCard);
}

void MemcardInterfaceImpl::SetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId) {
    if (cardId == CARD_UNKNOWN) {
        this->mAutosaveCard = this->mActiveCard;
    } else {
        this->mAutosaveCard = _ChangeToRealmcCardId(cardId);
    }
    if (state == AUTOSAVE_ENABLE) {
        this->mAutosaveEnabled = state;
        if (nSaveReqs == 0) {
            this->mIMemcard->TrcCardExists(this->mAutosaveCard);
        } else {
            Realmc::CardID prevCardId = this->mActiveCard;
            this->mActiveCard = this->mAutosaveCard;
            if (entryName != nullptr) {
                this->SaveCheck(entryName, nSaveReqs, saveReqs);
            } else {
                this->SaveCheck("", nSaveReqs, saveReqs);
            }
            this->mActiveCard = prevCardId;
        }
        this->mActiveTask = TASK_SETAUTOSAVE;
    } else {
        this->mAutosaveEnabled = false;
        this->mTaskManager->CompleteTask(RESULT_SUCCESS, STATUS_OK, &state);
    }
}

void MemcardInterfaceImpl::_ProcessGuidelinesMessage(const Realmc::Message *message) {
    bool showMessage;
    Realmc::UserMessage defaultResponse;
    unsigned int hiddenMessages;

    defaultResponse = Realmc::UMSG_NONE;
    hiddenMessages = this->mHiddenMessages;
    if (this->mAutosaveEnabled && this->mActiveCard.slot == this->mAutosaveCard.slot) {
        hiddenMessages |= 0x100;
    }
    hiddenMessages |= this->mTaskManager->FilterGuidelinesMessage(message);
    if (this->mForceMessages != 0) {
        hiddenMessages &= ~this->mForceMessages;
    }

    const Realmc::Message::DetailInfo::Trc &trcMessage = message->info.trc;

    switch (trcMessage.mMsgId) {
    case 14:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x2) == 0;
        break;
    case 15:
    case 17:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x4) == 0;
        break;
    case 16:
        defaultResponse = Realmc::UMSG_OPTION3;
        showMessage = (hiddenMessages & 0x4) == 0;
        break;
    case 24:
    case 25:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x100) == 0;
        break;
    case 26:
        defaultResponse = Realmc::UMSG_OPTION1;
        showMessage = (hiddenMessages & 0x100) == 0;
        break;
    case 27:
        showMessage = (hiddenMessages & 0x200) == 0;
        break;
    case 28:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x800) == 0;
        break;
    case 30:
        defaultResponse = Realmc::UMSG_OPTION1;
        showMessage = (hiddenMessages & 0x1000) == 0;
        break;
    case 31:
        showMessage = (hiddenMessages & 0x2000) == 0;
        break;
    case 29:
    case 32:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x8000) == 0;
        break;
    case 37:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x10000) == 0;
        break;
    case 38:
        showMessage = (hiddenMessages & 0x20000) == 0;
        break;
    case 39:
        defaultResponse = Realmc::UMSG_OPTION2;
        showMessage = (hiddenMessages & 0x80000) == 0;
        break;
    default:
        showMessage = true;
        break;
    }

    if (showMessage) {
        if (trcMessage.mMsgId == Realmc::LMSG_SEEK_DONE) {
            int lenText;
            Realmc::Message::DetailInfo::Trc *msg;
            const wchar_t *prevMsgText;

            lenText = Realmc::Locale::GetWstrLength(trcMessage.mMsg);
            memcpy(this->mInsufficientSpaceMsg, trcMessage.mMsg, lenText * 2);
            msg = const_cast<Realmc::Message::DetailInfo::Trc *>(&trcMessage);
            prevMsgText = msg->mMsg;
            msg->mMsg = this->mInsufficientSpaceMsg;
            this->_ShowGuidelinesMessage(msg);
            msg->mMsg = prevMsgText;
        } else {
            this->_ShowGuidelinesMessage(&trcMessage);
        }
    } else if (defaultResponse != Realmc::UMSG_NONE) {
        this->mIMemcard->SendMessage(defaultResponse, 0);
    } else {
        this->mIMemcard->SendMessage(Realmc::UMSG_EXPIRE_DELAY, 0);
    }
}

void MemcardInterfaceImpl::_ProcessCheckCard(const Realmc::Message *message) {
    if (message->mMsg != Realmc::LMSG_TRC_GETCARDINFO_DONE) {
        if (message->mMsg == Realmc::LMSG_TRC) {
            if (message->info.trc.mMsgId == 0x23) {
                this->mIsResettable = false;
            } else {
                this->mIsResettable = true;
            }
            this->_ProcessGuidelinesMessage(message);
        }
    } else {
        this->mIsResettable = true;
        this->mCardInfo.Clear();
        this->mCardInfo.mStatus = this->_TranslateCardStatus(message->mCardStatus);
        this->mCardInfo.mCardId = _ChangeToRealmcIfaceCardId(message->info.cardInfo.cardID);
        this->mCardInfo.mFreeSpace = message->info.cardInfo.freeSpace;
        this->mCardInfo.mFreeFiles = message->info.cardInfo.freeFiles;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult), this->mCardInfo.mStatus, &this->mCardInfo);
        if (this->mAutosaveEnabled &&
            (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
             message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
            this->_DisableAutosave();
        }
    }
}

void MemcardInterfaceImpl::_ProcessSetAutosave(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
        }
        this->_ProcessGuidelinesMessage(message);
        break;
    case Realmc::LMSG_TRC_CARDEXISTS_DONE:
        this->mIsResettable = true;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        if (message->mCardStatus == Realmc::STATUS_OK) {
            {
                AutosaveState state;

                state = AUTOSAVE_ENABLE;
                this->mTaskManager->CompleteTask(RESULT_SUCCESS, STATUS_OK, &state);
            }
        } else {
            this->mAutosaveEnabled = false;
            {
                AutosaveState state;

                state = AUTOSAVE_DISABLE;
                this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                                 this->_TranslateCardStatus(message->mCardStatus), &state);
            }
        }
        break;
    case Realmc::LMSG_TRC_SAVECHECK_DONE:
        this->mIsResettable = true;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        this->_ReleaseInsufficientSpaceMessage();
        if (message->mCardStatus == Realmc::STATUS_OK) {
            {
                AutosaveState state;

                state = AUTOSAVE_ENABLE;
                this->mTaskManager->CompleteTask(RESULT_SUCCESS, STATUS_OK, &state);
            }
        } else {
            this->mAutosaveEnabled = false;
            {
                AutosaveState state;

                state = AUTOSAVE_DISABLE;
                this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                                 this->_TranslateCardStatus(message->mCardStatus), &state);
            }
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::_ProcessBootupCheck(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
        }
        this->_ProcessGuidelinesMessage(message);
        break;
    case Realmc::LMSG_IS_IT_YOUR_FILE:
        if (RealmcUtils::Wildcard(message->info.fileInfo.fileName, this->mEntryName)) {
            this->mEntryFound = true;
            this->mIMemcard->SendMessage(Realmc::UMSG_YES, 0);
        } else {
            this->mIMemcard->SendMessage(Realmc::UMSG_NO, 0);
        }
        break;
    case Realmc::LMSG_TRC_STARTGAME_DONE:
        {
            BootupCheckResults results;

            this->mIsResettable = true;
            this->_ReleaseInsufficientSpaceMessage();
            results.mEntryFound = this->mEntryFound;
            this->mActiveTask = TASK_NONE;
            results.mFirstGoodCard = _ChangeToRealmcIfaceCardId(message->info.cardInfo.cardID);
            results.mNumBlocksNeeded =
                message->mCardStatus == Realmc::STATUS_EXIT_TO_CARD_MANAGER ? this->mStartGameInfo.totalBlocksNeeded : 0;
            this->_ClearMessage();
            this->mTaskManager->CompleteTask(RESULT_SUCCESS, this->_TranslateCardStatus(message->mCardStatus), &results);
            if (this->mAutosaveEnabled &&
                (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
                 message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
                this->_DisableAutosave();
            }
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::_ProcessSave(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x1b || message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
            this->_ClearMessage();
        }
        this->_ProcessGuidelinesMessage(message);
        break;
    case Realmc::LMSG_OPEN_FILE_DONE:
        this->mFilehandle = message->info.openResult.fileHandle;
        break;
    case Realmc::LMSG_WRITE_READY:
        this->mActiveSubtask = SUBTASK_WRITE_FILE_HEADER;
        this->mIMemcard->Write(this->mFilehandle, &this->mFileHeader, sizeof(this->mFileHeader));
        break;
    case Realmc::LMSG_WRITE_DONE:
        switch (this->mActiveSubtask) {
        case SUBTASK_WRITE_FILE_HEADER:
            if (this->mFileHeader.mUserHeaderSize != 0 && this->mUserHeader != nullptr) {
                this->mActiveSubtask = SUBTASK_WRITE_USER_HEADER;
                this->mIMemcard->Write(this->mFilehandle, this->mUserHeader, this->mFileHeader.mUserHeaderSize);
            } else if (this->mFileHeader.mUserBodySize != 0 && this->mUserBody != nullptr) {
                this->mActiveSubtask = SUBTASK_WRITE_USER_BODY;
                this->mIMemcard->Write(this->mFilehandle, this->mUserBody, this->mFileHeader.mUserBodySize);
            } else {
                this->mActiveSubtask = SUBTASK_NONE;
                this->mIMemcard->SendMessage(Realmc::UMSG_WRITE_COMPLETE, 0);
                this->mFilehandle = nullptr;
            }
            break;
        case SUBTASK_WRITE_USER_HEADER:
            if (this->mFileHeader.mUserBodySize != 0 && this->mUserBody != nullptr) {
                this->mActiveSubtask = SUBTASK_WRITE_USER_BODY;
                this->mIMemcard->Write(this->mFilehandle, this->mUserBody, this->mFileHeader.mUserBodySize);
            } else {
                this->mActiveSubtask = SUBTASK_NONE;
                this->mIMemcard->SendMessage(Realmc::UMSG_WRITE_COMPLETE, 0);
                this->mFilehandle = nullptr;
            }
            break;
        case SUBTASK_WRITE_USER_BODY:
            this->mActiveSubtask = SUBTASK_NONE;
            this->mIMemcard->SendMessage(Realmc::UMSG_WRITE_COMPLETE, 0);
            this->mFilehandle = nullptr;
            break;
        default:
            break;
        }
        break;
    case Realmc::LMSG_TRC_SAVECHECK_DONE:
    case Realmc::LMSG_TRC_SAVEFILE_DONE:
        this->mIsResettable = true;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        this->_ReleaseInsufficientSpaceMessage();
        this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                         this->_TranslateCardStatus(message->mCardStatus), this->mEntryName);
        if (this->mAutosaveEnabled &&
            (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
             message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
            this->_DisableAutosave();
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::_ProcessLoad(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x26 || message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
        }
        this->_ProcessGuidelinesMessage(message);
        break;
    case Realmc::LMSG_MOUNT_DONE:
        if (message->mTaskResult == Realmc::RESULT_SUCCESS) {
            this->mIMemcard->FindFile(this->mActiveCard, 0, this->mEntryName);
        } else {
            this->mFileSize = 0;
            this->mIMemcard->Unmount(this->mActiveCard);
        }
        break;
    case Realmc::LMSG_FILE_INFO:
        this->mFileSize = message->info.fileInfo.fileSize;
        break;
    case Realmc::LMSG_FIND_FILE_DONE:
        this->mIMemcard->Unmount(this->mActiveCard);
        break;
    case Realmc::LMSG_UNMOUNT_DONE:
        this->mIMemcard->TrcLoadFile(this->mActiveCard, this->mFileInfo);
        break;
    case Realmc::LMSG_OPEN_FILE_DONE:
        this->mFilehandle = message->info.openResult.fileHandle;
        break;
    case Realmc::LMSG_SEEK_DONE:
        this->mIMemcard->Read(this->mFilehandle, this->mUserBody, this->mFileHeader.mUserBodySize);
        break;
    case Realmc::LMSG_READ_READY:
        if (this->mDataFormat == FORMAT_LAYER2) {
            this->mActiveSubtask = SUBTASK_READ_FILE_HEADER;
            this->mIMemcard->Read(this->mFilehandle, &this->mFileHeader, 0x1c);
        } else {
            if (this->mUserBody == nullptr) {
                this->mTaskStatus = this->mIGame->LoadReady(this->mEntryName, 0, this->mFileSize, this->mUserHeader, this->mUserBody);
                if (this->mTaskStatus == TASK_CONTINUE) {
                    this->mActiveSubtask = SUBTASK_READ_USER_BODY;
                    this->mIMemcard->Read(this->mFilehandle, this->mUserBody, this->mFileSize);
                } else {
                    this->mActiveSubtask = SUBTASK_NONE;
                    this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
                }
            } else {
                this->mActiveSubtask = SUBTASK_READ_USER_BODY;
                this->mIMemcard->Read(this->mFilehandle, this->mUserBody, this->mFileSize);
            }
        }
        break;
    case Realmc::LMSG_READ_DONE:
        switch (this->mActiveSubtask) {
        case SUBTASK_READ_FILE_HEADER:
            if (this->mFileHeader.mFileSize != this->mFileSize ||
                this->mFileHeader.mFileHeaderVersion != 0x4d433032 ||
                this->mFileHeader.mFileHeaderSignature != this->_CalcSignature(&this->mFileHeader, 0x18)) {
                this->mActiveSubtask = SUBTASK_NONE;
                this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
                this->mFilehandle = nullptr;
            } else {
                if (this->mUserHeader == nullptr && this->mUserBody == nullptr) {
                    this->mTaskStatus = this->mIGame->LoadReady(this->mEntryName, this->mFileHeader.mUserHeaderSize,
                                                                this->mFileHeader.mUserBodySize, this->mUserHeader,
                                                                this->mUserBody);
                }
                if (this->mTaskStatus == TASK_CANCEL) {
                    this->mActiveSubtask = SUBTASK_NONE;
                    this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
                    this->mFilehandle = nullptr;
                } else if (this->mFileHeader.mUserHeaderSize == 0 || this->mUserHeader == nullptr) {
                    if (this->mFileHeader.mUserBodySize != 0 && this->mUserBody != nullptr) {
                        this->mActiveSubtask = SUBTASK_READ_USER_BODY;
                        this->mIMemcard->Seek(this->mFilehandle, this->mFileHeader.mUserHeaderSize, Realmc::SF_CUR);
                    } else {
                        this->mActiveSubtask = SUBTASK_NONE;
                        this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
                        this->mFilehandle = nullptr;
                    }
                } else {
                    this->mActiveSubtask = SUBTASK_READ_USER_HEADER;
                    this->mIMemcard->Read(this->mFilehandle, this->mUserHeader, this->mFileHeader.mUserHeaderSize);
                }
            }
            break;
        case SUBTASK_READ_USER_HEADER:
            if (this->mFileHeader.mUserBodySize == 0 || this->mUserBody == nullptr) {
                this->mActiveSubtask = SUBTASK_NONE;
                this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
                this->mFilehandle = nullptr;
            } else {
                this->mActiveSubtask = SUBTASK_READ_USER_BODY;
                this->mIMemcard->Seek(this->mFilehandle, this->mFileHeader.mUserHeaderSize, Realmc::SF_CUR);
            }
            break;
        case SUBTASK_READ_USER_BODY:
            this->mActiveSubtask = SUBTASK_NONE;
            this->mIMemcard->SendMessage(Realmc::UMSG_READ_COMPLETE, 0);
            this->mFilehandle = nullptr;
            break;
        default:
            break;
        }
        break;
    case Realmc::LMSG_CHECK_DATA_INTEGRITY:
        if (this->mTaskStatus != TASK_CONTINUE) {
            this->mIMemcard->SendMessage(Realmc::UMSG_DATA_CORRUPT, 0);
        } else if (this->mDataFormat == FORMAT_LAYER2) {
            if (this->mFileHeader.mFileSize != this->mFileSize ||
                this->mFileHeader.mFileHeaderVersion != 0x4d433032 ||
                this->mFileHeader.mFileHeaderSignature != this->_CalcSignature(&this->mFileHeader, 0x18)) {
                this->mIMemcard->SendMessage(Realmc::UMSG_DATA_CORRUPT, 0);
            } else {
            bool isUserDataOk;

            isUserDataOk = true;
                if (this->mUserHeader != nullptr) {
                    isUserDataOk = this->mFileHeader.mUserHeaderSignature ==
                                    this->_CalcSignature(this->mUserHeader, this->mFileHeader.mUserHeaderSize);
            }
            if (this->mUserBody != nullptr &&
                this->mFileHeader.mUserBodySignature !=
                    this->_CalcSignature(this->mUserBody, this->mFileHeader.mUserBodySize)) {
                isUserDataOk = false;
            }
            if (isUserDataOk) {
                this->mIMemcard->SendMessage(Realmc::UMSG_DATA_OK, 0);
            } else {
                this->mIMemcard->SendMessage(Realmc::UMSG_DATA_CORRUPT, 0);
            }
            }
        } else if (this->mIGame->CheckLoadedData(this->mUserBody) == DATA_OK) {
            this->mIMemcard->SendMessage(Realmc::UMSG_DATA_OK, 0);
        } else {
            this->mIMemcard->SendMessage(Realmc::UMSG_DATA_CORRUPT, 0);
        }
        break;
    case Realmc::LMSG_TRC_LOADFILE_DONE:
        this->mIsResettable = true;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                         this->_TranslateCardStatus(message->mCardStatus), this->mEntryName);
        if (this->mAutosaveEnabled &&
            (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
             message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
            this->_DisableAutosave();
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::_ProcessFindEntries(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
        }
        this->_ProcessGuidelinesMessage(message);
        break;
    case Realmc::LMSG_FILE_INFO:
        if (RealmcUtils::Wildcard(message->info.fileInfo.fileName, this->mEntryName)) {
            int userDataSize;

            this->mEntryFound = true;
            this->mEntryInfo.mName = message->info.fileInfo.fileName;
            this->mEntryInfo.mEntryBlocks = message->info.fileInfo.fileSize / this->mBlockSize;
            userDataSize = message->info.fileInfo.fileSize - message->info.fileInfo.userDataOffset - 0x1c;
            this->mEntryInfo.mUserDataSize = userDataSize < 0 ? 0 : userDataSize;
            this->mEntryInfo.mStatus = message->info.fileInfo.fileSize <= 0 && userDataSize < 0 ? STATUS_ENTRY_CORRUPTED : STATUS_OK;
            this->mEntryInfo.mTime.mCreated = message->info.fileInfo.fileTime;
            this->mEntryInfo.mTime.mLastModified = 0;
            this->mEntryInfo.mTime.mLastAccessed = 0;
            strncpy(&this->mEntryInfo.mCompanyCode[0], message->info.fileInfo.companyCode, 2);
            strncpy(&this->mEntryInfo.mGameCode[0], message->info.fileInfo.gameCode, 4);
            this->mTaskManager->FoundEntry(&this->mEntryInfo);
        }
        break;
    case Realmc::LMSG_TRC_LISTFILES_DONE:
        this->mIsResettable = true;
        this->mActiveTask = TASK_NONE;
        this->_ClearMessage();
        {
            CardStatus cardStatus;

            if (message->mTaskResult == Realmc::RESULT_SUCCESS) {
                cardStatus = this->mEntryFound ? STATUS_OK : STATUS_ENTRY_NOT_FOUND;
            } else {
                cardStatus = this->_TranslateCardStatus(message->mCardStatus);
            }
            this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult), cardStatus, nullptr);
            if (this->mAutosaveEnabled &&
                (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
                 message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
                this->_DisableAutosave();
            }
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::_ProcessDelete(const Realmc::Message *message) {
    switch (message->mMsg) {
    case Realmc::LMSG_TRC:
        if (message->info.trc.mMsgId == 0x26 || message->info.trc.mMsgId == 0x23) {
            this->mIsResettable = false;
        } else {
            this->mIsResettable = true;
        }
        if (message->info.trc.mMsgId == 0x27 && this->mActiveSubtask == SUBTASK_DELETE_MULTIPLE &&
            message->mCardStatus == Realmc::STATUS_FILE_NOT_FOUND) {
            this->mIMemcard->SendMessage(Realmc::UMSG_OPTION1, 0);
        } else {
            this->_ProcessGuidelinesMessage(message);
        }
        break;
    case Realmc::LMSG_TRC_DELETEFILE_DONE:
        this->mIsResettable = true;
        if (this->mActiveSubtask == SUBTASK_DELETE_SINGLE) {
            this->mActiveTask = TASK_NONE;
            this->_ClearMessage();
            this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                             this->_TranslateCardStatus(message->mCardStatus), this->mEntryName);
        } else if (message->mTaskResult == Realmc::RESULT_SUCCESS ||
                   message->mCardStatus == Realmc::STATUS_FILE_NOT_FOUND) {
            if (message->mTaskResult == Realmc::RESULT_SUCCESS) {
                this->mEntryFound = true;
            }
            if (this->mCurEntry < this->mNumEntries) {
                this->SetMessage(MESSAGE_HIDE, 0x70001);
                memset(this->mEntryName, 0, 0x20);
                strncpy(this->mEntryName, this->mEntryList[this->mCurEntry++], 0x1f);
                this->mFileInfo.fileName = this->mEntryName;
                this->mIMemcard->TrcDeleteFile(this->mActiveCard, this->mFileInfo);
            } else {
                this->SetMessage(MESSAGE_SHOW, 0x70001);
                if (this->mEntryFound) {
                    this->mUserChoice = CHOICE_OPTION3;
                } else {
                    Realmc::GCMessage message;
                    message.LC_msg(0x27,
                                   Realmc::GCMessage::PackMsgOptions(Realmc::UMSG_YES, Realmc::UMSG_OK, 0, 0),
                                   this->mActiveCard.slot);
                    this->mUserChoice = CHOICE_NONE;
                    this->_ProcessGuidelinesMessage(&message);
                }
                static_cast<Realmc::GCInterface *>(this->mIMemcard)->SetDummyMessage();
            }
        } else {
            this->mActiveTask = TASK_NONE;
            this->mActiveSubtask = SUBTASK_NONE;
            this->SetMessage(MESSAGE_SHOW, 0x70001);
            this->_ClearMessage();
            this->mTaskManager->CompleteTask(this->_TranslateTaskResult(message->mTaskResult),
                                             this->_TranslateCardStatus(message->mCardStatus), this->mEntryName);
        }
        if (this->mAutosaveEnabled &&
            (message->mCardStatus == Realmc::STATUS_NO_CARD || message->mCardStatus == Realmc::STATUS_CARD_CHANGED ||
             message->mCardStatus == Realmc::STATUS_CARD_REMOVED)) {
            this->_DisableAutosave();
        }
        break;
    case Realmc::LMSG_NONE:
        if (this->mUserChoice == CHOICE_NONE) {
            static_cast<Realmc::GCInterface *>(this->mIMemcard)->SetDummyMessage();
        } else {
            this->mActiveTask = TASK_NONE;
            this->mActiveSubtask = SUBTASK_NONE;
            this->_ClearMessage();
            this->SetMessage(MESSAGE_SHOW, 0x30001);
            if (this->mUserChoice == CHOICE_OPTION1) {
                this->mTaskManager->CompleteTask(RESULT_RETRY, STATUS_OK, this->mEntryName);
            } else if (this->mUserChoice == CHOICE_OPTION2) {
                this->mTaskManager->CompleteTask(RESULT_CANCELLED, STATUS_ENTRY_NOT_FOUND, this->mEntryName);
            } else {
                this->mTaskManager->CompleteTask(RESULT_SUCCESS, STATUS_OK, this->mEntryName);
            }
        }
        break;
    default:
        break;
    }
}

void MemcardInterfaceImpl::Delete(const char *entryName, const wchar_t *contentName) {
    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, entryName, 0x1f);
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->mIMemcard->TrcDeleteFile(this->mActiveCard, this->mFileInfo);
    this->mActiveTask = TASK_DELETE;
    this->mActiveSubtask = SUBTASK_DELETE_SINGLE;
}

void MemcardInterfaceImpl::DeleteMultiple(unsigned int nEntryNames, const char **entryNames, const wchar_t *contentName) {
    this->mNumEntries = nEntryNames;
    this->mEntryList = entryNames;
    this->mCurEntry = 0;
    this->mEntryFound = false;
    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, this->mEntryList[this->mCurEntry++], 0x1f);
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->SetMessage(MESSAGE_HIDE, 0x40000);
    this->mIMemcard->TrcDeleteFile(this->mActiveCard, this->mFileInfo);
    this->mActiveTask = TASK_DELETE;
    this->mActiveSubtask = SUBTASK_DELETE_MULTIPLE;
}

void MemcardInterfaceImpl::FindEntries(const char *entryNamePattern) {
    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, entryNamePattern, 0x1f);
    memset(&this->mFileInfo, 0, 0x30);
    this->mEntryFound = false;
    this->mFileInfo.fileName = Realmc::FILENAME_ALL_FILES;
    this->mEntryInfo.Clear();
    this->mTaskManager->ClearEntries();
    this->mIMemcard->TrcListFiles(this->mActiveCard, this->mFileInfo, 0);
    this->mActiveTask = TASK_FINDENTRIES;
}

void MemcardInterfaceImpl::FindEntriesAlternate(const char *entryNamePattern, const TitleInfo *titleInfo) {
    memset(this->mEntryName, 0, 0x20);
    if (titleInfo->mNameType == NAME_ENTRY) {
        strncpy(this->mEntryName, entryNamePattern, 0x1f);
    } else {
        _SplitPath(entryNamePattern, this->mEntryName);
    }
    memset(&this->mFileInfo, 0, 0x30);
    this->mEntryFound = false;
    this->mFileInfo.fileName = Realmc::FILENAME_ALL_FILES;
    this->mEntryInfo.Clear();
    this->mTaskManager->ClearEntries();
    this->mIMemcard->TrcListFiles(this->mActiveCard, this->mFileInfo, 0);
    this->mActiveTask = TASK_FINDENTRIES;
}

void MemcardInterfaceImpl::Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName) {
    this->mDataFormat = FORMAT_LAYER2;
    memset(this->mEntryName, 0, 0x20);
    strncpy(this->mEntryName, entryName, 0x1f);
    this->mUserHeader = header;
    this->mUserBody = body;
    this->mFileHeader.Clear();
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->mFileInfo.fileContentName = const_cast<wchar_t *>(contentName);
    this->mFileInfo.fileTypeName = const_cast<wchar_t *>(typeName);
    this->mIMemcard->Mount(this->mActiveCard);
    this->mActiveTask = TASK_LOAD;
}

void MemcardInterfaceImpl::LoadAlternate(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName, const TitleInfo *titleInfo) {
    this->mDataFormat = titleInfo->mDataFormat;
    memset(this->mEntryName, 0, 0x20);
    if (titleInfo->mNameType == NAME_ENTRY) {
        strncpy(this->mEntryName, entryName, 0x1f);
    } else {
        _SplitPath(entryName, this->mEntryName);
    }
    this->mUserHeader = header;
    this->mUserBody = body;
    this->mFileHeader.Clear();
    memset(&this->mFileInfo, 0, 0x30);
    this->mFileInfo.fileName = this->mEntryName;
    this->mFileInfo.gameTitle = &this->mGameInfo.mGameTitle[0];
    this->mFileInfo.fileContentName = const_cast<wchar_t *>(contentName);
    this->mFileInfo.fileTypeName = const_cast<wchar_t *>(typeName);
    this->mIMemcard->Mount(this->mActiveCard);
    this->mActiveTask = TASK_LOAD;
}

void MemcardInterfaceImpl::_CheckForCardRemoval() {
    if (static_cast<unsigned int>(CARDProbeEx(this->mAutosaveCard.slot, nullptr, nullptr) + 1) > 1) {
        if (this->mIMemcard->CheckForAutosaveCardRemoval()) {
            this->_DisableAutosave();
        }
    }
}

void MemcardInterfaceImpl::_DisableAutosave() {
    this->mAutosaveEnabled = false;
    this->mIMemcard->ResetAutosaveCardDetection();
    this->SetMessage(MESSAGE_SHOW, 0x100);
    this->mIGame->CardRemoved();
}

void MemcardInterfaceImpl::_ReleaseInsufficientSpaceMessage() {
    if (this->mInsufficientSpaceMsg != nullptr) {
        this->mISystem.mAllocator->Free(this->mInsufficientSpaceMsg, 0);
        this->mInsufficientSpaceMsg = nullptr;
    }
}

void MemcardInterfaceImpl::_MakeInsufficientSpaceMessage(unsigned int nSaveReqs, SaveReq **saveReqs) {
    int lenText;
    const wchar_t *endMsg;
    int lenEndText;

    {
        this->mInsufficientSpaceMsg = static_cast<wchar_t *>(this->mISystem.mAllocator->Alloc(
            0x800,
            EA::TagValuePair(EA::Allocator::ATT_NAME, "InsufficientSpaceMessage") +
                EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, 4)));
        memset(this->mInsufficientSpaceMsg, 0, 0x800);
    }

    Realmc::GCMessage message;
    message.LC_msg(0x13, 0, this->mActiveCard.slot, this->mGameInfo.mGameTitle);
    lenText = Realmc::Locale::GetWstrLength(message.info.trc.mMsg);
    memcpy(this->mInsufficientSpaceMsg, message.info.trc.mMsg, lenText * 2);
    this->mInsufficientSpaceMsg[lenText++] = 10;
    this->mInsufficientSpaceMsg[lenText++] = 10;

    {
        unsigned int iSaveReq;

        for (iSaveReq = 0; iSaveReq < nSaveReqs; iSaveReq++) {
            if (saveReqs[iSaveReq]->mSaveInfo->mHeaderSize + saveReqs[iSaveReq]->mSaveInfo->mBodySize != 0) {
                const wchar_t *reqMsg;
                int lenReqText;

                reqMsg = Realmc::Locale::GetString(
                    0x14, "sdd", saveReqs[iSaveReq]->mSaveInfo->mContentName, saveReqs[iSaveReq]->mNumSaves,
                    this->CalcSaveSize(saveReqs[iSaveReq]->mSaveInfo, FORMAT_LAYER2) * saveReqs[iSaveReq]->mNumSaves);
                lenReqText = Realmc::Locale::GetWstrLength(reqMsg);
                memcpy(&this->mInsufficientSpaceMsg[lenText], reqMsg, lenReqText * 2);
                lenText += lenReqText;
                this->mInsufficientSpaceMsg[lenText++] = 10;
            }
        }
    }

    this->mInsufficientSpaceMsg[lenText++] = 10;
    endMsg = Realmc::Locale::GetString(0x15, nullptr);
    lenEndText = Realmc::Locale::GetWstrLength(endMsg);
    memcpy(&this->mInsufficientSpaceMsg[lenText], endMsg, lenEndText * 2);
}

} // namespace RealmcIface

namespace Realmc {

void GCMessage::_SetMsgOptions(int options) {
    int curOption;
    int iOption;

    this->info.trc.mNumOptions = 0;
    if (options != 0) {
        while (options != 0) {
            curOption = options & 0xff;
            iOption = this->info.trc.mNumOptions++;
            this->info.trc.mOptions[iOption].mMsgId = curOption;
            this->info.trc.mOptions[iOption].mMsg = Locale::GetString(curOption, nullptr);
            options >>= 8;
        }
    }
}

short *GCMessage::_LcGetSlotString(int slotnum) {
    static short slotA[2] = {'A', 0};
    static short slotB[2] = {'B', 0};

    if (slotnum == 1) {
        return slotB;
    }
    return slotA;
}

} // namespace Realmc
