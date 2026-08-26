#ifndef REALMEMCARD_IMPL_MEMCARD_INTERFACE_IMPL_H
#define REALMEMCARD_IMPL_MEMCARD_INTERFACE_IMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../memcard_interface.h"

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
    UMSG_OPTION2 = 11,
    UMSG_OPTION3 = 12,
    UMSG_OPTION4 = 13,
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

struct FileInfo {
    const char *fileName;
    int fileByteSize;
    wchar_t *gameTitle;
    char *comment1;
    int sizeofcomment1;
    char *comment2;
    int sizeofcomment2;
    void *gcIconDataInfo;
    void *gcBannerDataInfo;
    wchar_t *fileTypeName;
    wchar_t *fileContentName;
    bool usingMultipleSaves;
};

struct StartGameInfo {
    FileInfo fileInfo;
    unsigned int totalBlocksNeeded;
    CardID checkCardID;
    bool checkAllSlots;
    unsigned int totalFilesNeeded;
};

struct OpenFileDescriptor {
    int fileNumber;
};

struct Message {
    struct DetailInfo {
        struct Trc {
            struct Option {
                unsigned int mMsgId;
                const unsigned short *mMsg;
            };

            unsigned int mMsgId;
            const unsigned short *mMsg;
            unsigned int mNumOptions;
            Option mOptions[4];
        };

        struct CardInfo {
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

        Trc trc;
        CardInfo cardInfo;
        FileInfo fileInfo;
        OpenResult openResult;
        ReadResult readResult;
        WriteResult writeResult;
        SeekResult seekResult;
        DirectoryInfo directoryInfo;
    };

    LibMessage mMsg;
    TaskResult mTaskResult;
    CardStatus mCardStatus;
    DetailInfo info;
};

struct BaseInterface {
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void TrcStartGame(const StartGameInfo &) = 0;
    virtual void TrcCardExists(const CardID &) = 0;
    virtual void TrcGetCardInfo(const CardID &) = 0;
    virtual void TrcLoadFile(const CardID &, const FileInfo &) = 0;
    virtual void TrcSaveFile(const CardID &, const FileInfo &, int, unsigned int, unsigned int) = 0;
    virtual void TrcDeleteFile(const CardID &, const FileInfo &) = 0;
    virtual void TrcListFiles(const CardID &, const FileInfo &, int) = 0;
    virtual void CardExists(const CardID &) = 0;
    virtual void GetCardInfo(const CardID &) = 0;
    virtual void OpenFile(const CardID &, const FileInfo &, int) = 0;
    virtual void CloseFile(int) = 0;
    virtual void DeleteFile(const CardID &, const char *, const char *) = 0;
    virtual void Read(int, void *, int) = 0;
    virtual void Write(int, void *, int) = 0;
    virtual void Seek(int, int, int) = 0;
    virtual void Flush(int) = 0;
    virtual void SetFileAttribute(const CardID &, const char *, const char *, int) = 0;
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
    virtual void CreateDirectory(const CardID &, const char *) = 0;
    virtual void DeleteDirectory(const CardID &, const char *) = 0;
    virtual void *GetSjisInterface() = 0;
    virtual void Format(const CardID &) = 0;

  protected:
    virtual ~Interface() {}
};

struct GCMessage : public Message {
    virtual ~GCMessage();
    void Init();
    void _SetMsgOptions(int options);
    short *_LcGetSlotString(int slotnum);
    void Clear();
};

} // namespace Realmc

namespace RealmcIface {

struct FileHeader {
    unsigned int mFileHeaderVersion;
    unsigned int mFileSize;
    unsigned int mUserHeaderSize;
    unsigned int mUserBodySize;
    unsigned int mUserHeaderSignature;
    unsigned int mUserBodySignature;
    unsigned int mFileHeaderSignature;

    void Clear();
};

struct MemcardInterfaceImpl;

struct FindEntriesInfo {
    char mEntryNamePattern[64];
    TitleInfo mTitleInfo;
};

struct LoadInfo {
    char mEntryName[64];
    const wchar_t *mContentName;
    const wchar_t *mTypeName;
    char *mHeader;
    char *mBody;
    TitleInfo mTitleInfo;
    bool mTryLoad;
};

struct McTask {
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
    void BootupCheck(const BootupCheckParams *, unsigned int, const char **, wchar_t *);
    void FindEntries(const char *, const TitleInfo *);
    void Load(const char *, char *, char *, const wchar_t *, const wchar_t *, const TitleInfo *);
    void Save(const char *, const char *, const char *, const SaveInfo *, const TitleInfo *);
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
    void TaskManagerSetMonitor(MonitorState state);
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
    void _ShowGuidelinesMessage(const Realmc::Message::DetailInfo::Trc *message);
    void _ClearMessage();
    RealmcIface::TaskResult _TranslateTaskResult(Realmc::TaskResult result);
    RealmcIface::CardStatus _TranslateCardStatus(Realmc::CardStatus status);
    void ClearTask();
    bool IsResettable() {
        return this->mIsResettable;
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
    unsigned short *mInsufficientSpaceMsg;
    unsigned int mFilesNeeded;
};

} // namespace RealmcIface

#endif
