#ifndef REALMEMCARD_IMPL_MEMCARD_INTERFACE_IMPL_H
#define REALMEMCARD_IMPL_MEMCARD_INTERFACE_IMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <string.h>

#include "../memcard_interface.h"

extern "C" char lbl_804147C8[];

namespace Realmc {

enum LibMessage {
    LMSG_NONE = 0,
    LMSG_TRC_STARTGAME_DONE = 1,
    LMSG_TRC_CARDEXISTS_DONE = 2,
    LMSG_TRC_GETCARDINFO_DONE = 3,
    LMSG_TRC_LOADFILE_DONE = 4,
    LMSG_TRC_SAVECHECK_DONE = 5,
    LMSG_TRC_SAVEFILE_DONE = 6,
    LMSG_TRC_DELETEFILE_DONE = 7,
    LMSG_TRC_LISTFILES_DONE = 8,
    LMSG_TRC_LIST_DONE = 9,
    LMSG_CARD_EXISTS_DONE = 10,
    LMSG_CARD_INFO_DONE = 11,
    LMSG_CREATE_DIRECTORY_DONE = 12,
    LMSG_DELETE_DIRECTORY_DONE = 13,
    LMSG_FIND_FILE_DONE = 14,
    LMSG_OPEN_FILE_DONE = 15,
    LMSG_CLOSE_FILE_DONE = 16,
    LMSG_DELETE_FILE_DONE = 17,
    LMSG_SET_ATTRIBUTE_DONE = 18,
    LMSG_SEEK_DONE = 19,
    LMSG_READ_DONE = 20,
    LMSG_WRITE_DONE = 21,
    LMSG_FLUSH_DONE = 22,
    LMSG_MOUNT_DONE = 23,
    LMSG_UNMOUNT_DONE = 24,
    LMSG_IS_IT_YOUR_FILE = 25,
    LMSG_TRC = 26,
    LMSG_FILE_INFO = 27,
    LMSG_READ_READY = 28,
    LMSG_WRITE_READY = 29,
    LMSG_CHECK_DATA_INTEGRITY = 30,
    LMSG_FORMAT_DONE = 31,
    LMSG_TASK_UNKNOWN = 32,
};

enum TaskResult {
    RESULT_SUCCESS = 0,
    RESULT_FAILED = 1,
    RESULT_CANCELLED = 2,
    RESULT_RETRY = 3,
    RESULT_UNKNOWN = 4,
};

enum CardStatus {
    STATUS_OK = 0,
    STATUS_NO_CARD = 1,
    STATUS_CARD_CHANGED = 2,
    STATUS_CARD_UNFORMATTED = 3,
    STATUS_CARD_DAMAGED = 4,
    STATUS_WRONG_DEVICE = 5,
    STATUS_CARD_FULL = 6,
    STATUS_ACCESS_DENIED = 7,
    STATUS_INSUFFICIENT_SPACE = 8,
    STATUS_FILE_NOT_FOUND = 9,
    STATUS_ENTRY_NOT_FOUND = 10,
    STATUS_ENTRY_ALREADY_EXISTS = 11,
    STATUS_FILE_NOT_OPENED = 12,
    STATUS_FILE_CORRUPTED = 13,
    STATUS_DIRECTORY_NOT_FOUND = 14,
    STATUS_DIRECTORY_NOT_EMPTY = 15,
    STATUS_TOO_MANY_OPENED_FILES = 16,
    STATUS_CANNOTMOUNT = 17,
    STATUS_FILE_DELETED = 18,
    STATUS_RANGE_ERROR = 19,
    STATUS_CARD_REMOVED = 20,
    STATUS_INACCESSIBLE_CARD = 21,
    STATUS_EXIT_TO_CARD_MANAGER = 22,
    STATUS_FAILED = 23,
    STATUS_UNKNOWN = -1,
};

enum SeekFrom {
    SF_SET = 0,
    SF_CUR = 1,
    SF_END = 2,
};

enum UserMessage {
    UMSG_NONE = 0,
    UMSG_WRITE_COMPLETE = 1,
    UMSG_READ_COMPLETE = 2,
    UMSG_YES = 3,
    UMSG_NO = 4,
    UMSG_OK = 5,
    UMSG_DATA_OK = 6,
    UMSG_DATA_CORRUPT = 7,
    UMSG_EXPIRE_DELAY = 8,
    UMSG_SHOW_CARD_CHECKING_MSG = 9,
    UMSG_OPTION1 = 10,
    UMSG_OPTION4 = 11,
    UMSG_OPTION2 = 12,
    UMSG_OPTION3 = 13,
    UMSG_TOTAL = 14,
};

struct CardID {
    CardID() {
        this->slot = 0;
        this->systemData = 0;
    }
    CardID(unsigned short s, unsigned short sysData) {
        this->slot = s;
        this->systemData = sysData;
    }

    unsigned short slot;
    unsigned short systemData;
};

enum GCImageFormat {
    GCIF_RGB5A3 = 0,
    GCIF_CI8 = 1,
};

enum GCAnimationImageLoop {
    GCIL_NONE = 0,
    GCIL_REPEAT = 1,
    GCIL_BACK_AND_FORTH = 2,
};

enum GCAnimationPause {
    GCIS_NONE = 0,
    GCIS_12FRAMES = 1,
    GCIS_8FRAMES = 2,
    GCIS_4FRAMES = 3,
};

struct GCIconDataInfo {
    GCIconDataInfo() {}

    int numIconFrames;
    char *imageData;
    GCAnimationPause animationPause[8];
    GCImageFormat imageFormat;
    GCAnimationImageLoop animationLoop;
};

struct GCBannerDataInfo {
    GCBannerDataInfo() {}

    GCImageFormat imageFormat;
    char *imageData;
};

struct FileInfo {
    FileInfo()
        : fileName(0)
        , fileByteSize(0)
        , gameTitle(0)
        , comment1(0)
        , sizeofcomment1(0)
        , comment2(0)
        , sizeofcomment2(0)
        , gcIconDataInfo(0)
        , gcBannerDataInfo(0)
        , fileTypeName(0)
        , fileContentName(0)
        , usingMultipleSaves(false) {}

    const char *fileName;
    int fileByteSize;
    wchar_t *gameTitle;
    char *comment1;
    int sizeofcomment1;
    char *comment2;
    int sizeofcomment2;
    GCIconDataInfo *gcIconDataInfo;
    GCBannerDataInfo *gcBannerDataInfo;
    wchar_t *fileTypeName;
    wchar_t *fileContentName;
    bool usingMultipleSaves;
};

struct StartGameInfo {
    StartGameInfo() {
        this->totalBlocksNeeded = 0;
        this->totalFilesNeeded = 0;
        this->checkAllSlots = false;
    }

    FileInfo fileInfo;
    unsigned int totalBlocksNeeded;
    CardID checkCardID;
    bool checkAllSlots;
    unsigned int totalFilesNeeded;
};

struct OpenFileDescriptor {
    OpenFileDescriptor() {
        this->fileNumber = 0;
    }

    int fileNumber;
};

struct Message {
    struct DetailInfo {
        struct Trc {
            struct Option {
                unsigned int mMsgId;
                const wchar_t *mMsg;
            };

            unsigned int mMsgId;
            const wchar_t *mMsg;
            unsigned int mNumOptions;
            Option mOptions[4];
        };

        struct CardInfo {
            CardInfo() {}

            CardID cardID;
            int freeSpace;
            int freeFiles;
        };

        struct FileInfo {
            char *fileName;
            int fileSize;
            unsigned int fileTime;
            unsigned int userDataOffset;
            char companyCode[2];
            char gameCode[4];
        };

        struct OpenResult {
            struct OpenFileDescriptor *fileHandle;
            const char *fileName;
        };

        struct ReadResult {
            int bytesRead;
        };

        struct WriteResult {
            int bytesWritten;
        };

        struct SeekResult {
            unsigned int filePosition;
        };

            struct DirectoryInfo {
                const char *name;
            };

        DetailInfo() {}

        Trc trc;
        CardInfo cardInfo;
        FileInfo fileInfo;
        OpenResult openResult;
        ReadResult readResult;
        WriteResult writeResult;
        SeekResult seekResult;
        DirectoryInfo directoryInfo;
    };

    Message() {
        this->mMsg = LMSG_NONE;
        this->mTaskResult = RESULT_UNKNOWN;
        this->mCardStatus = STATUS_UNKNOWN;
    }

    LibMessage mMsg;
    TaskResult mTaskResult;
    CardStatus mCardStatus;
    DetailInfo info;
};

enum SaveTaskType {
    SAVETASK_CHECK = 0,
    SAVETASK_SAVE = 1,
};

enum ListTaskType {
    LISTTASK_ENTRIES = 0,
    LISTTASK_FILES = 1,
};

enum FileOpenMode {
    FOM_UNKNOWN = 0,
    FOM_READ = 1,
    FOM_WRITE = 2,
    FOM_READ_WRITE = 3,
    FOM_CREATE = 512,
};

enum FileAttribute {
    FA_NONE = 0,
    FA_PUBLIC = 4,
    FA_COPY_PROTECTED = 8,
    FA_NO_MOVE = 16,
    FA_COMPANY = 64,
};

struct BaseInterface {
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void TrcStartGame(const StartGameInfo &) = 0;
    virtual void TrcCardExists(const CardID &) = 0;
    virtual void TrcGetCardInfo(const CardID &) = 0;
    virtual void TrcLoadFile(const CardID &, const FileInfo &) = 0;
    virtual void TrcSaveFile(const CardID &, const FileInfo &, SaveTaskType, unsigned int, unsigned int) = 0;
    virtual void TrcDeleteFile(const CardID &, const FileInfo &) = 0;
    virtual void TrcListFiles(const CardID &, const FileInfo &, ListTaskType) = 0;
    virtual void CardExists(const CardID &) = 0;
    virtual void GetCardInfo(const CardID &) = 0;
    virtual void OpenFile(const CardID &, const FileInfo &, FileOpenMode) = 0;
    virtual void CloseFile(OpenFileDescriptor *) = 0;
    virtual void DeleteFile(const CardID &, const char *, const char *) = 0;
    virtual void Read(OpenFileDescriptor *, void *, int) = 0;
    virtual void Write(OpenFileDescriptor *, void *, int) = 0;
    virtual void Seek(OpenFileDescriptor *, int, SeekFrom) = 0;
    virtual void Flush(OpenFileDescriptor *) = 0;
    virtual void SetFileAttribute(const CardID &, const char *, const char *, FileAttribute) = 0;
    virtual void FindFile(const CardID &, const char *, const char *) = 0;
    virtual const Message *GetMessage(int) = 0;
    virtual void SendMessage(UserMessage, int) = 0;
    virtual bool IsBusy() = 0;
    virtual const unsigned short *GetCardName(const CardID &) = 0;
    virtual unsigned int GetBlockSize(const CardID &) = 0;
    virtual void *GetBlockCalculator() = 0;

  protected:
    virtual ~BaseInterface() {}
};

struct Interface : public BaseInterface {
    static Interface *CreateInstance(const SystemInterface &iSystem);

    virtual void Mount(const CardID &) = 0;
    virtual void Unmount(const CardID &) = 0;
    virtual bool CheckForAutosaveCardRemoval() = 0;
    virtual void ResetAutosaveCardDetection() = 0;

  protected:
    virtual ~Interface() {}
};

struct InterfaceImp : public Interface {
    static void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }

    InterfaceImp(const SystemInterface &iSystem);
    virtual ~InterfaceImp();
    virtual int AddRef();
    virtual int Release();
    virtual void ClearTask() = 0;

    void CheckMessageCompatibility();
    void LockInterfaceMutex() {
        this->mMutex->Lock();
    }
    void UnlockInterfaceMutex() {
        this->mMutex->Unlock();
    }

    SystemInterface mISystem;
    IMutex *mMutex;
    int mRefcount;
};

struct MessageTimer {
    MessageTimer()
        : mIsActive(false)
        , mNumTicksElapsed(0)
        , mNumTicksToExpire(0)
        , mNumSecondsDefaultDelay(0) {}

    ~MessageTimer() {}

    void Init(unsigned int nSecondsDefaultDelay) {
        this->mNumSecondsDefaultDelay = nSecondsDefaultDelay;
    }

    void Set(unsigned int nSeconds) {
        this->mIsActive = true;
        this->mNumTicksElapsed = 0;
        this->mNumTicksToExpire = nSeconds * 1000;
    }

    void Stop() {
        this->mIsActive = false;
    }

    void AddElapsedTime(unsigned int elapsedTime) {
        if (this->mIsActive) {
            this->mNumTicksElapsed += elapsedTime;
        }
    }

    bool IsExpired() {
        if (this->mIsActive) {
            if (this->mNumTicksToExpire < this->mNumTicksElapsed) {
                this->Stop();
                return true;
            }
            return false;
        }
        return true;
    }

    bool mIsActive;
    unsigned int mNumTicksElapsed;
    unsigned int mNumTicksToExpire;
    unsigned int mNumSecondsDefaultDelay;
};

enum TaskID {
    TID_NONE = 0,
    TID_TRC_STARTGAME = 1,
    TID_TRC_CARDEXISTS = 2,
    TID_TRC_GETCARDINFO = 3,
    TID_TRC_LOADFILE = 4,
    TID_TRC_SAVECHECK = 5,
    TID_TRC_SAVEFILE = 6,
    TID_TRC_DELETEFILE = 7,
    TID_TRC_LIST = 8,
    TID_TRC_LISTENTRIES = 9,
    TID_TRC_LISTFILES = 10,
    TID_TRC_MOUNT = 11,
    TID_TRC_FORMAT = 12,
    TID_STARTGAME_COMMON = 13,
    TID_CARD_EXISTS = 14,
    TID_GET_CARD_INFO = 15,
    TID_MOUNT = 16,
    TID_UNMOUNT = 17,
    TID_CREATE_DIRECTORY = 18,
    TID_DELETE_DIRECTORY = 19,
    TID_OPEN = 20,
    TID_CLOSE = 21,
    TID_DELETE = 22,
    TID_SEEK = 23,
    TID_READ = 24,
    TID_WRITE = 25,
    TID_FLUSH = 26,
    TID_SET_ATTRIBUTES = 27,
    TID_FORMAT = 28,
    TID_FIND = 29,
    TID_SAVE_FILE = 30,
    TID_CHECK_DIRECTORY = 31,
    TID_CHANGE_DIRECTORY = 32,
    TID_CHECK_SPACE = 33,
    TID_SHOW_CARD_STATUS_MSG = 34,
    TID_TOTAL = 35,
};

enum TaskState {
    TS_START = 0,
    TS_RESULT = 1,
    TS_CHECK_RESULT = 2,
    TS_FAILED = 3,
    TS_SUCCESS = 4,
    TS_WAIT_FOR_USER_REPLY = 5,
    TS_FIND_DIRECTORY = 6,
    TS_CHANGE_DIRECTORY = 7,
    TS_CHANGE_DIRECTORY_RESULT = 8,
    TS_SEARCH = 9,
    TS_FIND_FIRST = 10,
    TS_FIND_NEXT = 11,
    TS_FIND_FILE = 12,
    TS_FIND_FILE_RESULT = 13,
    TS_FILE_NOT_FOUND = 14,
    TS_FIND_ICONSYS_RESULT = 15,
    TS_FIND_DUMMYFILE_RESULT = 16,
    TS_ENTRY_NOT_FOUND = 17,
    TS_CHECK_CARD = 18,
    TS_CHECK_CARD_RESULT = 19,
    TS_CHECK_NEXT_CARD = 20,
    TS_GET_CARD_INFO = 21,
    TS_NO_CARD_ASK_USER = 22,
    TS_NO_CARD_WAIT_FOR_USER_REPLY = 23,
    TS_CHECK_SPACE = 24,
    TS_CHECK_SPACE_RESULT = 25,
    TS_CHECK_MAX_FILES = 26,
    TS_CHECK_MAX_FILES_RESULT = 27,
    TS_INSUFFICIENT_SPACE_ASK_USER = 28,
    TS_INSUFFICIENT_SPACE_ASK_USER_RESULT = 29,
    TS_INSUFFICIENT_SPACE_INFORM_USER = 30,
    TS_NO_AUTOSAVE_CONFIRM = 31,
    TS_NO_AUTOSAVE_CONFIRM_WAIT_FOR_REPLY = 32,
    TS_CHECK_FOR_PREVIOUS_SAVE = 33,
    TS_AUTOSAVE_ENABLE_WAIT_FOR_REPLY = 34,
    TS_CONTINUE_NO_SAVE = 35,
    TS_CHECK_DIRECTORY = 36,
    TS_CHECK_DIRECTORY_RESULT = 37,
    TS_CREATE_DIRECTORY = 38,
    TS_CREATE_DIRECTORY_RESULT = 39,
    TS_CHECK_ICON_SYS = 40,
    TS_CHECK_ICON_SYS_RESULT = 41,
    TS_CHECK_STATIC_ICON = 42,
    TS_CHECK_STATIC_ICON_RESULT = 43,
    TS_CHECK_COPY_ICON = 44,
    TS_CHECK_COPY_ICON_RESULT = 45,
    TS_CHECK_DELETE_ICON = 46,
    TS_CHECK_DELETE_ICON_RESULT = 47,
    TS_CHECK_DUMMY_FILE = 48,
    TS_CHECK_DUMMY_FILE_RESULT = 49,
    TS_SAVE_ICON_SYS = 50,
    TS_SAVE_ICON_SYS_RESULT = 51,
    TS_SAVE_STATIC_ICON = 52,
    TS_SAVE_STATIC_ICON_RESULT = 53,
    TS_SAVE_COPY_ICON = 54,
    TS_SAVE_COPY_ICON_RESULT = 55,
    TS_SAVE_DELETE_ICON = 56,
    TS_SAVE_DELETE_ICON_RESULT = 57,
    TS_SAVE_FAILED_ASK_USER = 58,
    TS_CHECK_FORMAT_RESULT = 59,
    TS_OVERWRITE_ASK_USER = 60,
    TS_OVERWRITE_CHECK_USER_REPLY = 61,
    TS_SHOW_SAVE_WARNING = 62,
    TS_FORMAT_WAIT_FOR_USER_CONFIRMATION = 63,
    TS_FORMAT = 64,
    TS_FORMAT_WAIT_FOR_RESULT = 65,
    TS_WRITE_READY = 66,
    TS_WRITE_WAIT = 67,
    TS_SAVE_FAILED = 68,
    TS_SAVE_FAILED_INFORM_USER = 69,
    TS_SAVE_FAILED_WAIT_FOR_USER_REPLY = 70,
    TS_WRITE = 71,
    TS_WRITE_RESULT = 72,
    TS_SHOW_LOAD_WARNING_MSG = 73,
    TS_READ_READY = 74,
    TS_READ_WAIT = 75,
    TS_SHOW_RESULT = 76,
    TS_USER_CHECK_DATA = 77,
    TS_USER_CHECK_DATA_RESULT = 78,
    TS_OPEN_FILE = 79,
    TS_OPEN_FILE_RESULT = 80,
    TS_CLOSE_FILE = 81,
    TS_CLOSE_FILE_RESULT = 82,
    TS_FIND_FILE_SIZE = 83,
    TS_FIND_FILE_SIZE_RESULT = 84,
    TS_DELETE_FILE_ASK_USER = 85,
    TS_DELETE_WAIT_FOR_USER_REPLY = 86,
    TS_DELETE_FILE = 87,
    TS_DELETE_FILE_RESULT = 88,
    TS_DELETE_ALL_FILES = 89,
    TS_DELETE_DIRECTORY = 90,
    TS_DELETE_DIRECTORY_RESULT = 91,
    TS_DISPLAY_RESULT = 92,
    TS_GET_FREE_SPACE = 93,
    TS_WAIT_FOR_RESULT = 94,
    TS_DELAY = 95,
    TS_UNFORMAT_RESULT = 96,
    TS_LOAD_READY = 97,
    TS_MOUNT = 98,
    TS_MOUNT_RESULT = 99,
    TS_CHECK_REMOUNT = 100,
    TS_SET_ATTRIBUTE = 101,
    TS_CHECK_MOUNT_RESULT = 102,
    TS_BAD_CARD_ASK_USER = 103,
    TS_ERROR_ASK_USER = 104,
    TS_MOUNT_RESULT_WAIT_USER_REPLY = 105,
    TS_RETRY = 106,
    TS_SAVE_READY = 107,
    TS_UPDATE_SPACE_USED = 108,
    TS_FOUND_FILE_ASK_USER = 109,
    TS_FILE_LIMIT_WAIT_FOR_REPLY = 110,
    TS_LOAD_FAILED_INFORM_USER = 111,
    TS_LOAD_FAILED_WAIT_FOR_USER_REPLY = 112,
    TS_DELETE_FAILED_INFORM_USER = 113,
    TS_DELETE_FAILED_WAIT_FOR_USER_REPLY = 114,
    TS_FAILED_CARD_REMOVED_INFORM_USER = 115,
    TS_FAILED_CARD_REMOVED_WAIT_FOR_USER_REPLY = 116,
    TS_SAVE_DIRECTORY_FILES = 117,
    TS_CREATE_DIRECTORY_TABLE = 118,
    TS_CONFIRM_CHECKING_DIRECTORY = 119,
    TS_SHOW_CARD_STATUS_MSG = 120,
    TS_LIST = 121,
    TS_DONE = 122,
    TS_END = 123,
};

enum InputOptions {
    IO_NONE = 0,
    IO_RETRY = 1,
    IO_FORMAT = 2,
    IO_CONTINUE = 4,
    IO_OVERWRITE = 8,
    IO_MANAGE = 16,
    IO_DELETE = 32,
    IO_NO = 64,
    IO_YES = 128,
    IO_A_BUTTON = 256,
};

struct BlockCalculator;
struct BlockCalculatorImp;
struct GCDriver;
struct TaskManager;

struct Task {
    Task()
        : mParent(nullptr)
        , mID(TID_NONE)
        , mState(TS_START)
        , mSubstate(TS_START) {}

    Task(TaskID id, TaskManager *parent)
        : mParent(parent)
        , mID(id)
        , mState(TS_START)
        , mSubstate(TS_START) {}

    virtual void Init(bool) {}

    TaskID GetID() {
        return this->mID;
    }

    TaskState GetState() {
        return this->mState;
    }

    TaskState SetState(TaskState newState, TaskState subState) {
        TaskState prevState = this->mState;
        this->mState = newState;
        this->mSubstate = subState;
        return prevState;
    }

    int GetSubstate() {
        return this->mSubstate;
    }

    void SetSubstate(TaskState sst) {
        this->mSubstate = sst;
    }

    TaskManager *mParent;
    TaskID mID;
    TaskState mState;
    TaskState mSubstate;
};

struct TaskStack {
    TaskStack()
        : mNumTasks(0) {}

    void Push(Task *task) {
        if (this->mNumTasks <= 15) {
            this->mBuf[this->mNumTasks++] = task;
        }
    }

    void Pop(Task **task) {
        if (this->mNumTasks > 0) {
            *task = this->mBuf[--this->mNumTasks];
        } else {
            *task = nullptr;
        }
    }

    Task *mBuf[16];
    int mNumTasks;
};

struct GcTask : public Task {
    GcTask(TaskID id, TaskManager *parent)
        : Task(id, parent)
        , mCardID() {
        this->Init(false);
    }

    virtual void Init(bool notifyUser) override {
        this->mCardID.slot = 0;
        this->mNotifyUser = notifyUser;
        this->mState = TS_START;
        this->mTaskResult = RESULT_UNKNOWN;
        this->mCardStatus = STATUS_UNKNOWN;
    }

    void End();

    CardID mCardID;
    bool mNotifyUser;
    TaskResult mTaskResult;
    CardStatus mCardStatus;
};

struct TaskTrc : public GcTask {
    TaskTrc(TaskID id, TaskManager *parent)
        : GcTask(id, parent)
        {
        this->Clear();
    }

    virtual void Clear() {
        this->mFileHandle = nullptr;
        this->mMounted = false;
        this->mFileOpened = false;
    }

    virtual void Init(bool nofifyUser) override {
        this->Clear();
        this->GcTask::Init(nofifyUser);
    }

    OpenFileDescriptor *mFileHandle;
    bool mMounted;
    bool mFileOpened;
};

struct TaskTrcStartGame : public TaskTrc {
    TaskTrcStartGame(TaskManager *parent)
        : TaskTrc(TID_TRC_STARTGAME, parent)
        , mInfo()
        , mFirstCardChecked() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mInfo, 0, sizeof(this->mInfo));
        this->mSlotStatus[0] = STATUS_UNKNOWN;
        this->mSlotStatus[1] = STATUS_UNKNOWN;
    }

    void Start(const StartGameInfo *info) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mInfo = *info;
        this->mFirstCardChecked = info->checkCardID;
        this->mSlotStatus[0] = STATUS_UNKNOWN;
        this->mSlotStatus[1] = STATUS_UNKNOWN;
    }

    StartGameInfo mInfo;
    CardID mFirstCardChecked;
    CardStatus mSlotStatus[2];
};

struct TaskTrcCardExists : public TaskTrc {
    TaskTrcCardExists(TaskManager *parent)
        : TaskTrc(TID_TRC_CARDEXISTS, parent) {
        this->Clear();
    }

    void Start(const CardID &cID) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mCardID = cID;
    }
};

struct TaskTrcGetCardInfo : public TaskTrc {
    TaskTrcGetCardInfo(TaskManager *parent)
        : TaskTrc(TID_TRC_GETCARDINFO, parent)
        , mCardInfo() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mCardInfo, 0, sizeof(this->mCardInfo));
    }

    void Start(const CardID &cID) {
        this->Clear();
        this->TaskTrc::Init(true);
        memset(&this->mCardInfo, 0, sizeof(this->mCardInfo));
        this->mCardID = cID;
        this->mCardInfo.cardID = cID;
    }

    Message::DetailInfo::CardInfo mCardInfo;
};

struct TaskTrcSaveFile : public TaskTrc {
    TaskTrcSaveFile(TaskManager *parent)
        : TaskTrc(TID_TRC_SAVEFILE, parent)
        , mFileInfo() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mFileInfo, 0, sizeof(this->mFileInfo));
        this->mFileHandle = nullptr;
        this->mFileFound = false;
        this->mFileName = nullptr;
        this->mBlocksNeeded = 0;
        this->mFilesNeeded = 0;
    }

    void Start(CardID cID, const FileInfo *finfo, SaveTaskType saveTaskType, unsigned int nBlocksNeeded, unsigned int nFilesNeeded) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mCardID = cID;
        this->mFileFound = false;
        this->mFileInfo = *finfo;
        this->mFileName = finfo->fileName;
        this->mBlocksNeeded = nBlocksNeeded;
        this->mFilesNeeded = nFilesNeeded;
        this->mState = saveTaskType == SAVETASK_CHECK ? TS_START : TS_SAVE_READY;
    }

    FileInfo mFileInfo;
    OpenFileDescriptor *mFileHandle;
    bool mFileFound;
    const char *mFileName;
    unsigned int mBlocksNeeded;
    unsigned int mFilesNeeded;
};

struct TaskTrcListFiles : public TaskTrc {
    TaskTrcListFiles(TaskManager *parent)
        : TaskTrc(TID_TRC_LISTFILES, parent)
        , mFileInfo() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mFileInfo, 0, sizeof(this->mFileInfo));
        this->mMounted = false;
        this->mNumFilesFound = 0;
    }

    void Start(CardID cID, const FileInfo *finfo) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mCardID = cID;
        this->mFileInfo = *finfo;
        this->mListingStarted = false;
    }

    FileInfo mFileInfo;
    unsigned int mNumFilesFound;
    bool mListingStarted;
};

struct TaskTrcLoadFile : public TaskTrc {
    TaskTrcLoadFile(TaskManager *parent)
        : TaskTrc(TID_TRC_LOADFILE, parent)
        , mFileInfo() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mFileInfo, 0, sizeof(this->mFileInfo));
        this->mFileHandle = nullptr;
        this->mFileFound = false;
    }

    void Start(CardID cID, const FileInfo *finfo) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mCardID = cID;
        this->mFileInfo = *finfo;
        this->mFileFound = false;
    }

    FileInfo mFileInfo;
    bool mFileFound;
};

struct TaskTrcDeleteFile : public TaskTrc {
    TaskTrcDeleteFile(TaskManager *parent)
        : TaskTrc(TID_TRC_DELETEFILE, parent)
        , mFileInfo() {
        this->Clear();
    }

    virtual void Clear() override {
        this->TaskTrc::Clear();
        memset(&this->mFileInfo, 0, sizeof(this->mFileInfo));
        this->mFileFound = false;
    }

    void Start(CardID cID, const FileInfo *finfo) {
        this->Clear();
        this->TaskTrc::Init(true);
        this->mCardID = cID;
        this->mFileInfo = *finfo;
        this->mFileFound = false;
    }

    FileInfo mFileInfo;
    bool mFileFound;
};

struct TaskTrcMount : public TaskTrc {
    TaskTrcMount(TaskManager *parent)
        : TaskTrc(TID_TRC_MOUNT, parent)
        , mTrcTaskID(TID_NONE) {}

    void Start(CardID cardID, TaskID trcTaskID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mTrcTaskID = trcTaskID;
    }

    TaskID mTrcTaskID;
};

struct TaskTrcFormat : public TaskTrc {
    TaskTrcFormat(TaskManager *parent)
        : TaskTrc(TID_TRC_FORMAT, parent)
        , mTrcTaskID(TID_NONE) {}

    void Start(CardID cardID, TaskID trcTaskID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mTrcTaskID = trcTaskID;
    }

    TaskID mTrcTaskID;
};

struct TaskShowCardStatusMsg : public TaskTrc {
    TaskShowCardStatusMsg(TaskManager *parent)
        : TaskTrc(TID_SHOW_CARD_STATUS_MSG, parent)
        , mTrcTaskID(TID_NONE) {}

    void Start(CardID cardID, TaskResult result, CardStatus status, TaskID trcTaskID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mTrcTaskID = trcTaskID;
        this->mTaskResult = result;
        this->mCardStatus = status;
    }

    TaskID mTrcTaskID;
};

struct TaskTrcCheckSpace : public TaskTrc {
    TaskTrcCheckSpace(TaskManager *parent)
        : TaskTrc(TID_CHECK_SPACE, parent)
        , mFileInfo()
        , mTaskID(TID_NONE)
        , mFindResult(nullptr)
        , mSectorSize(0)
        , mBlocksFree(0)
        , mBlocksUsed(0)
        , mBlocksNeeded(0)
        , mFilesFree(0)
        , mFilesUsed(0)
        , mFilesNeeded(0)
        , mFileExists(false) {}

    void Start(CardID cardID, FileInfo *fileInfo, TaskID taskID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mFileInfo = *fileInfo;
        this->mTaskID = taskID;
        this->mFindResult = nullptr;
        this->mSectorSize = 0;
        this->mBlocksFree = 0;
        this->mBlocksUsed = 0;
        this->mBlocksNeeded = 0;
        this->mFilesFree = 0;
        this->mFilesUsed = 0;
        this->mFilesNeeded = 0;
        this->mFileExists = false;
    }

    void Start(CardID cardID, FileInfo *fileInfo, unsigned int nBlocksNeeded, unsigned int nFilesNeeded, TaskID taskID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mFileInfo = *fileInfo;
        this->mTaskID = taskID;
        this->mFindResult = nullptr;
        this->mSectorSize = 0;
        this->mBlocksFree = 0;
        this->mBlocksUsed = 0;
        this->mBlocksNeeded = nBlocksNeeded;
        this->mFilesFree = 0;
        this->mFilesUsed = 0;
        this->mFilesNeeded = nFilesNeeded;
        this->mFileExists = false;
    }

    FileInfo mFileInfo;
    TaskID mTaskID;
    struct FindResult *mFindResult;
    unsigned int mSectorSize;
    unsigned int mBlocksFree;
    unsigned int mBlocksUsed;
    unsigned int mBlocksNeeded;
    unsigned int mFilesFree;
    unsigned int mFilesUsed;
    unsigned int mFilesNeeded;
    bool mFileExists;
};

struct TaskMount : public GcTask {
    TaskMount(TaskManager *parent)
        : GcTask(TID_MOUNT, parent) {}
    void Start(CardID cardID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
    }
};

struct TaskUnmount : public GcTask {
    TaskUnmount(TaskManager *parent)
        : GcTask(TID_UNMOUNT, parent) {}
    void Start(CardID cardID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
    }
};

struct TaskCardExists : public GcTask {
    TaskCardExists(TaskManager *parent)
        : GcTask(TID_CARD_EXISTS, parent)
        , mCardFound(false) {}
    void Start(CardID cardID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mCardFound = false;
    }
    bool mCardFound;
};

struct TaskGetCardInfo : public GcTask {
    TaskGetCardInfo(TaskManager *parent)
        : GcTask(TID_GET_CARD_INFO, parent) {}
    void Start(CardID cardID, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
    }
};

struct TaskOpen : public GcTask {
    TaskOpen(TaskManager *parent)
        : GcTask(TID_OPEN, parent)
        , mFileInfo(nullptr)
        , mMode(FOM_UNKNOWN) {}
    void Start(CardID cardID, const FileInfo *fileInfo, FileOpenMode mode, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        this->mFileInfo = const_cast<FileInfo *>(fileInfo);
        this->mMode = mode;
    }
    FileInfo *mFileInfo;
    FileOpenMode mMode;
};

struct TaskClose : public GcTask {
    TaskClose(TaskManager *parent)
        : GcTask(TID_CLOSE, parent)
        , mFileHandle(nullptr) {}
    void Start(OpenFileDescriptor *fh, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mFileHandle = fh;
    }
    OpenFileDescriptor *mFileHandle;
};

struct TaskRead : public GcTask {
    TaskRead(TaskManager *parent)
        : GcTask(TID_READ, parent)
        , mFileHandle(nullptr)
        , mBuffer(nullptr)
        , mBufferSize(0)
        , mBytesRead(0) {}
    void Start(OpenFileDescriptor *fh, void *buffer, int bufsize, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mFileHandle = fh;
        this->mBuffer = buffer;
        this->mBufferSize = bufsize;
        this->mBytesRead = 0;
    }
    OpenFileDescriptor *mFileHandle;
    void *mBuffer;
    int mBufferSize;
    int mBytesRead;
};

struct TaskWrite : public GcTask {
    TaskWrite(TaskManager *parent)
        : GcTask(TID_WRITE, parent)
        , mFileHandle(nullptr)
        , mBuffer(nullptr)
        , mBufferSize(0)
        , mBytesWritten(0) {}
    void Start(OpenFileDescriptor *fh, void *buffer, int bufsize, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mFileHandle = fh;
        this->mBuffer = buffer;
        this->mBufferSize = bufsize;
        this->mBytesWritten = 0;
    }
    OpenFileDescriptor *mFileHandle;
    void *mBuffer;
    int mBufferSize;
    int mBytesWritten;
};

struct TaskSeek : public GcTask {
    TaskSeek(TaskManager *parent)
        : GcTask(TID_SEEK, parent)
        , mFileHandle(nullptr)
        , mOffset(0)
        , mSeekFrom(SF_SET) {}
    void Start(OpenFileDescriptor *fh, int offset, SeekFrom from, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mFileHandle = fh;
        this->mOffset = offset;
        this->mSeekFrom = from;
    }
    OpenFileDescriptor *mFileHandle;
    int mOffset;
    SeekFrom mSeekFrom;
};

struct TaskFlush : public GcTask {
    TaskFlush(TaskManager *parent)
        : GcTask(TID_FLUSH, parent)
        , mFileHandle(nullptr) {}
    void Start(OpenFileDescriptor *fh, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mFileHandle = fh;
    }
    OpenFileDescriptor *mFileHandle;
};

struct TaskDelete : public GcTask {
    TaskDelete(TaskManager *parent)
        : GcTask(TID_DELETE, parent) {
        memset(this->mFileName, 0, sizeof(this->mFileName));
    }
    void Start(CardID cardID, const char *fileName, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        strncpy(this->mFileName, fileName, sizeof(this->mFileName));
    }
    char mFileName[36];
};

struct TaskSetAttribute : public GcTask {
    TaskSetAttribute(TaskManager *parent)
        : GcTask(TID_SET_ATTRIBUTES, parent)
        , mAttribute(FA_NONE) {
        memset(this->mFileName, 0, sizeof(this->mFileName));
    }
    void Start(CardID cardID, const char *fileName, FileAttribute attr, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        strncpy(this->mFileName, fileName, sizeof(this->mFileName));
        this->mAttribute = attr;
    }
    char mFileName[36];
    FileAttribute mAttribute;
};

struct TaskFind : public GcTask {
    TaskFind(TaskManager *parent)
        : GcTask(TID_FIND, parent)
        , mNumFilesFound(0) {
        memset(this->mFileName, 0, sizeof(this->mFileName));
    }
    void Start(CardID cardID, const char *fileName, bool notifyUser) {
        this->GcTask::Init(notifyUser);
        this->mCardID = cardID;
        strncpy(this->mFileName, fileName, sizeof(this->mFileName));
        this->mNumFilesFound = 0;
    }
    char mFileName[36];
    int mNumFilesFound;
};

struct FindResult;

struct TaskManager {
    TaskManager()
        : mCurrentTask(nullptr)
        , mTaskStack()
        , mPublicTrcFunctionRefcount(0) {}

    virtual void StartTask(Task *newTask);
    virtual void EndTask(Task *task);
    Task *GetCurrentTask() {
        return this->mCurrentTask;
    }
    virtual void UpdateCurrentTask() {}
    void ClearTask() {
        while (this->mCurrentTask != nullptr) {
            this->EndTask(this->mCurrentTask);
        }
    }
    bool IsPublicTrcTask(Task *task);
    void TrcSingletonAssert(Task *newTask);

    Task *mCurrentTask;
    TaskStack mTaskStack;
    int mPublicTrcFunctionRefcount;
};

inline void GcTask::End() {
    this->mParent->EndTask(this);
}

struct GCMessage : public Message {
    GCMessage() {
        this->Init();
    }
    virtual ~GCMessage() {}

#ifdef REALMC_GC_MESSAGE_INIT_INLINE
    inline void Init() {
        this->Clear();
    }
#else
    void Init();
#endif

    void _SetMsgOptions(int options);
    short *_LcGetSlotString(int slotnum);

#ifdef REALMC_GC_MESSAGE_INLINE
    inline void Clear() {
        memset(this, 0, 0x78);
    }
#else
    void Clear();
#endif

    void Set(LibMessage msg) {
        this->Clear();
        this->mMsg = msg;
    }

    void Set(LibMessage msg, TaskResult result, CardStatus status) {
        this->Clear();
        this->mMsg = msg;
        this->mTaskResult = result;
        this->mCardStatus = status;
    }

    void Set(Message *msg) {
        memcpy(this, msg, sizeof(*this));
    }

    void LC_msg(int msgId, int options) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, nullptr);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, lbl_804147C8, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    static int PackMsgOptions(int option1, int option2, int option3, int option4) {
        return option1 | option2 << 8 | option3 << 16 | option4 << 24;
    }

    void LC_msg(int msgId, int options, int nSlot, wchar_t *name) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, "ss", this->_LcGetSlotString(nSlot), name);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *type, wchar_t *content) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, "ss", type, content);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *name, int nSlot) {
        this->LC_msg(msgId, options, nSlot, name);
    }

    void LC_msg(int msgId, int options, wchar_t *gameTitle, wchar_t *fileName, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, "sss", gameTitle, fileName, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *gameTitle, wchar_t *saveGameContent, wchar_t *fileName, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, "ssss", gameTitle, saveGameContent, fileName, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, int nSlot, wchar_t *name, unsigned int space, unsigned int files) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, "ssii", this->_LcGetSlotString(nSlot), name, space, files);
        this->_SetMsgOptions(options);
    }
};

struct FindResult {
    FindResult() {}
    ~FindResult() {}

    GCMessage msg;
    char fileName[36];
};

struct GCInterface : public InterfaceImp {
    static inline void *operator new(unsigned int size) {
        return AllocateMemSize(0, size, 0, 0, 0);
    }
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }
    static inline void *operator new(unsigned int, void *ptr) {
        return ptr;
    }
    static inline void operator delete(void *, void *) {}

    static GCMessage mTaskMsg;
    static volatile GCMessage *mpNewTaskMsg;

    static TaskManager mTaskManager;
    static TaskTrcStartGame mTaskTrcStartGame;
    static TaskTrcCardExists mTaskTrcCardExists;
    static TaskTrcGetCardInfo mTaskTrcGetCardInfo;
    static TaskTrcSaveFile mTaskTrcSaveFile;
    static TaskTrcLoadFile mTaskTrcLoadFile;
    static TaskTrcDeleteFile mTaskTrcDeleteFile;
    static TaskTrcListFiles mTaskTrcListFiles;
    static TaskTrcMount mTaskTrcMount;
    static TaskTrcFormat mTaskTrcFormat;
    static TaskShowCardStatusMsg mTaskShowCardStatusMsg;
    static TaskTrcCheckSpace mTaskTrcCheckSpace;
    static TaskCardExists mTaskCardExists;
    static TaskGetCardInfo mTaskGetCardInfo;
    static TaskMount mTaskMount;
    static TaskUnmount mTaskUnmount;
    static TaskOpen mTaskOpen;
    static TaskClose mTaskClose;
    static TaskRead mTaskRead;
    static TaskWrite mTaskWrite;
    static TaskSeek mTaskSeek;
    static TaskFlush mTaskFlush;
    static TaskDelete mTaskDelete;
    static TaskSetAttribute mTaskSetAttribute;
    static TaskFind mTaskFind;
    static FindResult mFindResult;
    static GCDriver *mpDriver;
    static UserMessage mUserMsg;
    static volatile bool mExitThread;
    static MessageTimer mMsgTimer;
    static BlockCalculatorImp mBlockCalculator;
    static unsigned short mCardName[48];

    GCInterface(const SystemInterface &iSystem);
    virtual ~GCInterface();

    static int TaskThread(void *);
    static InputOptions ConvertUmsgToOption(UserMessage, int, TaskID);
    static CardStatus CheckCard(const CardID &);
    static const unsigned short *GetCardName(const CardID &, wchar_t *, int);

    const Message *GetMessage(int) override;
    void SendMessage(UserMessage, int) override;
    bool IsBusy() override;
    unsigned int GetBlockSize(const CardID &) override;
    BlockCalculator *GetBlockCalculator() override;
    bool CheckForAutosaveCardRemoval() override;
    void ResetAutosaveCardDetection() override;

    void TrcStartGame(const StartGameInfo &) override;
    void TrcCardExists(const CardID &) override;
    void TrcGetCardInfo(const CardID &) override;
    void TrcLoadFile(const CardID &, const FileInfo &) override;
    void TrcSaveFile(const CardID &, const FileInfo &, SaveTaskType, unsigned int, unsigned int) override;
    void TrcListFiles(const CardID &, const FileInfo &, ListTaskType) override;
    void TrcDeleteFile(const CardID &, const FileInfo &) override;
    const unsigned short *GetCardName(const CardID &) override;
    void CardExists(const CardID &) override;
    void GetCardInfo(const CardID &) override;
    void Mount(const CardID &) override;
    void Unmount(const CardID &) override;
    void OpenFile(const CardID &, const FileInfo &, FileOpenMode) override;
    void CloseFile(OpenFileDescriptor *) override;
    void Read(OpenFileDescriptor *, void *, int) override;
    void Write(OpenFileDescriptor *, void *, int) override;
    void Seek(OpenFileDescriptor *, int, SeekFrom) override;
    void Flush(OpenFileDescriptor *) override;
    void DeleteFile(const CardID &, const char *, const char *) override;
    void SetFileAttribute(const CardID &, const char *, const char *, FileAttribute) override;
    void FindFile(const CardID &, const char *, const char *) override;
    void ClearTask() override;

    static void UpdateTaskCardExists();
    static void UpdateTaskGetCardInfo();
    static void UpdateTaskMount();
    static void UpdateTaskUnmount();
    static void UpdateTaskOpen();
    static void UpdateTaskClose();
    static void UpdateTaskRead();
    static void UpdateTaskWrite();
    static void UpdateTaskSeek();
    static void UpdateTaskFlush();
    static void UpdateTaskDelete();
    static void UpdateTaskSetAttribute();
    static void UpdateTaskFindFile();
    static FindResult *Find(const CardID &, const char *, bool);
    static void UpdateTaskTrcStartGame();
    static void UpdateTaskTrcCardExists();
    static void UpdateTaskTrcGetCardInfo();
    static void UpdateTaskTrcLoadFile();
    static void UpdateTaskTrcSaveCheck();
    static void UpdateTaskTrcSaveFile();
    static void UpdateTaskTrcDeleteFile();
    static void UpdateTaskTrcListFiles();
    static void UpdateTaskTrcMount();
    static void UpdateTaskTrcFormat();
    static void UpdateTaskShowCardStatusMessage();
    static void UpdateTaskTrcCheckSpace();

    inline void SetDummyMessage() {
        mTaskMsg.mMsg = LMSG_NONE;
        mpNewTaskMsg = &mTaskMsg;
    }
};

} // namespace Realmc

namespace RealmcIface {

extern const char *ALL_ENTRIES;

struct FileHeader {
    FileHeader() {}

    unsigned int mFileHeaderVersion;
    unsigned int mFileSize;
    unsigned int mUserHeaderSize;
    unsigned int mUserBodySize;
    unsigned int mUserHeaderSignature;
    unsigned int mUserBodySignature;
    unsigned int mFileHeaderSignature;

    void Init(unsigned int userHeaderSize, unsigned int userBodySize, unsigned int fileSize, unsigned int userHeaderSignature, unsigned int userBodySignature) {
        this->mFileHeaderVersion = 0x4d433032;
        this->mUserHeaderSize = userHeaderSize;
        this->mUserBodySize = userBodySize;
        this->mFileSize = fileSize;
        this->mUserHeaderSignature = userHeaderSignature;
        this->mUserBodySignature = userBodySignature;
    }

    void Clear();
};

struct MemcardInterfaceImpl;

struct FindEntriesInfo {
    void Clear() {
        memset(this->mEntryNamePattern, 0, sizeof(this->mEntryNamePattern));
        this->mTitleInfo.Clear();
    }

    void Init(const char *entryNamePattern, const TitleInfo *titleInfo) {
        this->Clear();
        strncpy(this->mEntryNamePattern, entryNamePattern, sizeof(this->mEntryNamePattern));
        if (titleInfo != nullptr) {
            this->mTitleInfo = *titleInfo;
        }
    }

    char mEntryNamePattern[64];
    TitleInfo mTitleInfo;
};

struct LoadInfo {
    static void *operator new[](unsigned int size) {
        return Realmc::AllocateMemSize(0, size, 0, 4, 0);
    }

    static void operator delete[](void *ptr, unsigned int size) {
        Realmc::FreeMemSize(ptr, size);
    }

    void Clear() {
        memset(this->mEntryName, 0, sizeof(this->mEntryName));
        this->mContentName = nullptr;
        this->mTypeName = nullptr;
        this->mHeader = nullptr;
        this->mBody = nullptr;
        this->mTitleInfo.Clear();
        this->mTryLoad = false;
    }

    void Init(const char *entryName, const wchar_t *content, const wchar_t *type,
              char *header, char *body, const TitleInfo *titleInfo) {
        this->Clear();
        strncpy(this->mEntryName, entryName, sizeof(this->mEntryName));
        this->mContentName = content;
        this->mTypeName = type;
        this->mHeader = header;
        this->mBody = body;
        if (titleInfo != nullptr) {
            this->mTitleInfo = *titleInfo;
        }
        this->mTryLoad = false;
    }

    char mEntryName[64];
    const wchar_t *mContentName;
    const wchar_t *mTypeName;
    char *mHeader;
    char *mBody;
    TitleInfo mTitleInfo;
    bool mTryLoad;
};

struct McTask {
    void Clear() {
        this->mTask = TASK_NONE;
    }

    void InitBootupCheck(const BootupCheckParams *params) {
        this->mDetails.mBootupCheck.mBootupParams = params;
        this->mDetails.mBootupCheck.mCardStatus = STATUS_UNKNOWN;
        this->mDetails.mBootupCheck.mBootupResults.Clear();
    }

    void InitLoad(unsigned int nEntries, const char **entryNames,
                  const wchar_t *content, const wchar_t *type, const TitleInfo *titleInfo) {
        this->mDetails.mLoad.mNumEntries = nEntries;
        this->mDetails.mLoad.mCurEntry = 0;
        this->mDetails.mLoad.mLastEntryFound = 0;
        this->mDetails.mLoad.mLoadInfos = new LoadInfo[nEntries];
        for (unsigned int iEntry = 0; iEntry < nEntries; iEntry++) {
            this->mDetails.mLoad.mLoadInfos[iEntry].Init(entryNames[iEntry], content, type, nullptr, nullptr, titleInfo);
        }
    }

    void InitFindEntries(const char *entryNamePattern, const TitleInfo *titleInfo) {
        this->mDetails.mFindEntries.Init(entryNamePattern, titleInfo);
    }

    void InitSaveCheck(const char *entryName, const SaveInfo *saveInfo, const TitleInfo *titleInfo) {
        this->mDetails.mSaveCheck.mEntryName = entryName;
        this->mDetails.mSaveCheck.mSaveInfo = saveInfo;
        this->mDetails.mSaveCheck.mTitleInfo = titleInfo;
    }

    void InitSave(const char *entryName, const char *header, const char *body,
                  const SaveInfo *saveInfo, const TitleInfo *titleInfo) {
        this->mDetails.mSave.mEntryName = entryName;
        this->mDetails.mSave.mHeader = header;
        this->mDetails.mSave.mBody = body;
        this->mDetails.mSave.mSaveInfo = saveInfo;
        this->mDetails.mSave.mTitleInfo = titleInfo;
    }

    void InitDelete(unsigned int nEntries, const char **entryNames, const wchar_t *contentName) {
        this->mDetails.mDelete.mNumEntries = nEntries;
        this->mDetails.mDelete.mContentName = contentName;
        if (nEntries == 1) {
            this->mDetails.mDelete.mEntryName = entryNames[0];
        } else {
            this->mDetails.mDelete.mEntryNames = entryNames;
        }
    }

    void InitCheckCard(CardId cardId) {
        this->mDetails.mCheckCardId = cardId;
    }

    void InitSetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs,
                         const char *entryName, CardId cardId) {
        this->mDetails.mSetAutosave.mState = state;
        this->mDetails.mSetAutosave.mNumSaveReqs = nSaveReqs;
        this->mDetails.mSetAutosave.mSaveReqs = saveReqs;
        this->mDetails.mSetAutosave.mEntryName = entryName;
        this->mDetails.mSetAutosave.mCardId = cardId;
    }

    void InitMonitor() {
        this->mDetails.mMonitorStatus = STATUS_UNKNOWN;
    }

    MemcardTask mTask;
    union {
        FindEntriesInfo mFindEntries;
        CardId mCheckCardId;
        CardStatus mMonitorStatus;
        struct {
            const BootupCheckParams *mBootupParams;
            CardStatus mCardStatus;
            BootupCheckResults mBootupResults;
        } mBootupCheck;
        struct {
            unsigned int mNumEntries;
            unsigned int mCurEntry;
            unsigned int mLastEntryFound;
            LoadInfo *mLoadInfos;
        } mLoad;
        struct {
            const char *mEntryName;
            const SaveInfo *mSaveInfo;
            const TitleInfo *mTitleInfo;
        } mSaveCheck;
        struct {
            const char *mEntryName;
            const char *mHeader;
            const char *mBody;
            const SaveInfo *mSaveInfo;
            const TitleInfo *mTitleInfo;
        } mSave;
        struct {
            unsigned int mNumEntries;
            const char *mEntryName;
            const char **mEntryNames;
            const wchar_t *mContentName;
        } mDelete;
        struct {
            AutosaveState mState;
            unsigned int mNumSaveReqs;
            SaveReq **mSaveReqs;
            const char *mEntryName;
            CardId mCardId;
        } mSetAutosave;
    } mDetails;
};

struct TaskManager {
    static void *operator new(unsigned int size) {
        return Realmc::AllocateMemSize(0, size, 0, 0, 0);
    }

    TaskManager(MemcardInterfaceImpl *, IGameInterface *);

    void _ClearMainTask() {
        this->mMainTask = TASK_NONE;
    }

    void BootupCheck(const BootupCheckParams *, unsigned int, const char **, wchar_t *);
    void FindEntries(const char *, const TitleInfo *);
    void Load(const char *, char *, char *, const wchar_t *, const wchar_t *, const TitleInfo *);
    void Save(const char *, const char *, const char *, const SaveInfo *, const TitleInfo *);
    void Delete(const char *, const wchar_t *);
    void Delete(unsigned int, const char **, const wchar_t *);
    void CheckCard(CardId);
    void SetAutosave(AutosaveState, unsigned int, SaveReq **, const char *, CardId);
    void SetMonitor(MonitorState);
    void FoundEntry(EntryInfo *);
    void ClearEntries();
    void _StartTask();
    void CompleteTask(TaskResult, CardStatus, void *);
    void _ClearOldMsgs();
    void _ClearTaskList();
    void _InitTaskList();
    bool _HasStatusChanged(CardStatus);
    unsigned int FilterGuidelinesMessage(const Realmc::Message *message);

    RealmcIface::MemcardInterfaceImpl *mMemcardImpl;
    IGameInterface *mIGame;
    McTask mTaskList[3];
    unsigned int mCurTask;
    MemcardTask mMainTask;
    MonitorState mMonitorState;
    bool mCheckingMsgShown;
    bool mWarningMsgShown;
    unsigned int mLastMessageId;
    bool mSetMonitorDoneCalled;
    CardInfo mRecentCardInfo;
    bool mCancelledCardChangedCalled;
};

struct MemcardInterfaceImpl {
  public:
    static void *operator new(unsigned int size) {
        return Realmc::AllocateMemSize(0, size, 0, 0, 0);
    }

    static void operator delete(void *ptr, unsigned int size) {
        Realmc::FreeMemSize(ptr, size);
    }

    static void *operator new[](unsigned int size) {
        return Realmc::AllocateMemSize(0, size, 0, 0, 0);
    }

    static void operator delete[](void *ptr, unsigned int size) {
        Realmc::FreeMemSize(ptr, size);
    }

    static void *operator new(unsigned int, void *ptr) {
        return ptr;
    }

    static void operator delete(void *) {}

    static void *operator new[](unsigned int, void *ptr) {
        return ptr;
    }

    static void operator delete[](void *) {}

    MemcardInterfaceImpl(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo);
    ~MemcardInterfaceImpl();

    void MessageDone(MessageChoices choice);
    MemcardTask Update(unsigned int elapsedTime);
    void SetMessage(MessageState state, unsigned int message);
    void TaskManagerBootupCheck(const BootupCheckParams *params, unsigned int nEntries, const char **entryNames, wchar_t *content);
    void TaskManagerFindEntries(const char *entryNamePattern, const TitleInfo *titleInfo);
    void TaskManagerLoad(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName, const TitleInfo *titleInfo);
    void TaskManagerSave(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void TaskManagerDelete(const char *entryName, const wchar_t *contentName);
    void TaskManagerCheckCard(CardId cardId);
    void TaskManagerSetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId);
    void SaveCheck(const char *entryName, unsigned int nSaveReqs, SaveReq **saveReqs);
    void Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo);
    unsigned int CalcSaveSize(const SaveInfo *saveInfo, const DataFormat dataFormat);
    void SetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId);
    void Delete(const char *entryName, const wchar_t *contentName);
    void FindEntries(const char *entryNamePattern);
    void DeleteMultiple(unsigned int nEntryNames, const char **entryNames, const wchar_t *contentName);
    void FindEntriesAlternate(const char *entryNamePattern, const TitleInfo *titleInfo);
    void Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName);
    void LoadAlternate(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName, const TitleInfo *titleInfo);
    void TaskManagerSetMonitor(MonitorState state);
    void BootupCheck(const BootupCheckParams *params);
    void _ProcessBootupCheck(const Realmc::Message *message);
    void _ProcessCheckCard(const Realmc::Message *message);
    void _ProcessSave(const Realmc::Message *message);
    void _ProcessLoad(const Realmc::Message *message);
    void _ProcessDelete(const Realmc::Message *message);
    void _ProcessFindEntries(const Realmc::Message *message);
    void _ProcessSetAutosave(const Realmc::Message *message);
    void _ProcessGuidelinesMessage(const Realmc::Message *message);
    void _CheckForCardRemoval();
    void _DisableAutosave();
    unsigned int _CalcSignature(const void *data, unsigned int size);
    void CheckCard(CardId cardId);
    void SetActiveCard(CardId cardId);
    void _ReleaseInsufficientSpaceMessage();
    void _MakeInsufficientSpaceMessage(unsigned int nSaveReqs, SaveReq **saveReqs);
    void _ShowGuidelinesMessage(const Realmc::Message::DetailInfo::Trc *message);
    void _ClearMessage();
    RealmcIface::TaskResult _TranslateTaskResult(Realmc::TaskResult result);
    RealmcIface::CardStatus _TranslateCardStatus(Realmc::CardStatus status);
    void ClearTask();
    bool IsResettable() {
        return this->mIsResettable;
    }

    bool IsActiveTaskNone() {
        return this->mActiveTask == TASK_NONE;
    }

  private:
    Realmc::SystemInterface mISystem;
    IGameInterface *mIGame;
    Realmc::Interface *mIMemcard;
    TaskManager *mTaskManager;
    GameInfo mGameInfo;
    Realmc::CardID mActiveCard;
    Realmc::CardID mAutosaveCard;
    bool mAutosaveEnabled;
    MemcardTask mActiveTask;
    MemcardSubtask mActiveSubtask;
    MessageChoices mUserChoice;
    bool mMessageShowing;
    unsigned int mHiddenMessages;
    unsigned int mForceMessages;
    bool mIsResettable;
    MonitorState mMonitorState;
    CardInfo mCardInfo;
    Realmc::StartGameInfo mStartGameInfo;
    char mEntryName[32];
    unsigned int mFileSize;
    EntryInfo mEntryInfo;
    unsigned int mBlockSize;
    Realmc::FileInfo mFileInfo;
    Realmc::OpenFileDescriptor *mFilehandle;
    FileHeader mFileHeader;
    char *mUserHeader;
    char *mUserBody;
    bool mEntryFound;
    DataFormat mDataFormat;
    TaskStatus mTaskStatus;
    unsigned int mBlocksNeeded;
    const char **mEntryList;
    unsigned int mNumEntries;
    unsigned int mCurEntry;
    wchar_t *mInsufficientSpaceMsg;
    unsigned int mFilesNeeded;
};

} // namespace RealmcIface

#endif
