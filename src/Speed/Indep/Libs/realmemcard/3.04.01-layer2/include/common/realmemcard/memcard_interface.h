#ifndef REALMEMCARD_MEMCARD_INTERFACE_H
#define REALMEMCARD_MEMCARD_INTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include <Allocator/iallocator.h>

namespace Realmc {

struct Interface;

struct IThread {
    virtual IThread *CreateInstance();
    virtual int AddRef();
    virtual int Release();
    virtual void SetStackSize(unsigned int size);
    virtual void Begin(int (*entry)(void *), void *arg);
    virtual void WaitForEnd();
    virtual void Sleep(unsigned int milliseconds);
    virtual void SetPriority(int priority);
};

struct IMutex {
    virtual IMutex *CreateInstance();
    virtual int AddRef();
    virtual int Release();
    virtual void Lock();
    virtual void Unlock();
};

struct SystemInterface {
    SystemInterface() {}
    SystemInterface(SystemInterface *iSystem) {
        this->mAllocator = iSystem->mAllocator;
        this->mThread = iSystem->mThread;
        this->mMutex = iSystem->mMutex;
        this->mGetStrCallback = iSystem->mGetStrCallback;
    }

    EA::Allocator::IAllocator *mAllocator;
    IThread *mThread;
    IMutex *mMutex;
    const char *(*mGetStrCallback)(int);
};

void SetMemAllocator(EA::Allocator::IAllocator *allocator);
void *AllocateMemSize(const char *pBlockName, int size, int align, int alignmentOffset, int flags);
void FreeMemSize(void *pBlock, int size);

namespace Locale {
void SetLocaleGetStrCallback(const char *(*cb)(int));
int GetWstrLength(const wchar_t *str);
const unsigned short *GetString(int strID, char *parameterTypes, ...);
}

} // namespace Realmc

namespace RealmcIface {

struct AutoloadEntry {
    char *entryName;
    wchar_t *content;
};

enum MessageChoices {
    CHOICE_NONE = 0,
    CHOICE_OPTION1 = 1,
    CHOICE_OPTION2 = 2,
    CHOICE_OPTION3 = 3,
    CHOICE_OPTION4 = 4,
};

enum CardStatus {
    STATUS_OK = 0,
    STATUS_NO_CARD = 1,
    STATUS_CARD_CHANGED = 2,
    STATUS_CARD_REMOVED = 3,
    STATUS_CARD_UNFORMATTED = 4,
    STATUS_CARD_DAMAGED = 5,
    STATUS_WRONG_DEVICE = 6,
    STATUS_CARD_ERROR = 7,
    STATUS_ENTRY_CORRUPTED = 8,
    STATUS_ENTRY_NOT_FOUND = 9,
    STATUS_ENTRY_DELETED = 10,
    STATUS_INSUFFICIENT_SPACE = 11,
    STATUS_CANNOTMOUNT = 12,
    STATUS_RETRY_BOOT_FLOW = 13,
    STATUS_EXIT_TO_CARD_MANAGER = 14,
    STATUS_UNKNOWN = -1,
};

enum TaskResult {
    RESULT_SUCCESS = 0,
    RESULT_FAILED = 1,
    RESULT_CANCELLED = 2,
    RESULT_RETRY = 3,
    RESULT_UNKNOWN = 4,
};

enum CardId {
    PORT1_SLOT1 = 1,
    PORT1_SLOT2 = 2,
    PORT1_SLOT3 = 4,
    PORT1_SLOT4 = 8,
    PORT1_ALL = 15,
    PORT1_DEFAULT = 1,
    PORT2_SLOT1 = 16,
    PORT2_SLOT2 = 32,
    PORT2_SLOT3 = 64,
    PORT2_SLOT4 = 128,
    PORT2_ALL = 240,
    PORT2_DEFAULT = 16,
    PORT3_SLOT1 = 256,
    PORT3_SLOT2 = 512,
    PORT3_SLOT3 = 1024,
    PORT3_SLOT4 = 2048,
    PORT3_ALL = 3840,
    PORT3_DEFAULT = 256,
    PORT4_SLOT1 = 4096,
    PORT4_SLOT2 = 8192,
    PORT4_SLOT3 = 16384,
    PORT4_SLOT4 = 32768,
    PORT4_ALL = 61440,
    PORT4_DEFAULT = 4096,
    PORT5_SLOT1 = 65536,
    PORT5_DEFAULT = 65536,
    MAX_CARDID = 65536,
    CARD_UNKNOWN = -1,
};

enum DataStatus {
    DATA_OK = 0,
    DATA_CORRUPT = 1,
};

enum AutosaveState {
    AUTOSAVE_DISABLE = 0,
    AUTOSAVE_ENABLE = 1,
};

enum MonitorState {
    MONITOR_OFF = 0,
    MONITOR_ON = 1,
    MONITOR_ON_USER_CANCELLED = 2,
};

enum TaskStatus {
    TASK_CONTINUE = 0,
    TASK_CANCEL = 1,
};

enum MessageState {
    MESSAGE_SHOW = 0,
    MESSAGE_HIDE = 1,
    MESSAGE_FORCE = 2,
};

enum MemcardTask {
    TASK_NONE = 0,
    TASK_CHECKCARD = 1,
    TASK_BOOTUPCHECK = 2,
    TASK_SAVECHECK = 4,
    TASK_SAVE = 8,
    TASK_LOAD = 16,
    TASK_DELETE = 32,
    TASK_FINDENTRIES = 64,
    TASK_SETAUTOSAVE = 128,
    TASK_MONITOR = 256,
};

enum MemcardSubtask {
    SUBTASK_NONE = 0,
    SUBTASK_WRITE_FILE_HEADER = 1,
    SUBTASK_WRITE_USER_HEADER = 2,
    SUBTASK_WRITE_USER_BODY = 3,
    SUBTASK_READ_FILE_HEADER = 4,
    SUBTASK_READ_USER_HEADER = 5,
    SUBTASK_READ_USER_BODY = 6,
    SUBTASK_FIND_DIRECTORIES = 7,
    SUBTASK_FIND_FILES = 8,
    SUBTASK_SAVE_OVERWRITE_CONFIRM = 9,
    SUBTASK_SAVE_WAIT_FOR_OVERWRITE_CONFIRMATION = 10,
    SUBTASK_SAVE_SAVE = 11,
    SUBTASK_SAVE_SHOW_TASK_RESULT = 12,
    SUBTASK_SAVE_WAIT_FOR_RESULT_DISMISSAL = 13,
    SUBTASK_SAVE_DONE = 14,
    SUBTASK_LOAD_LOAD = 15,
    SUBTASK_LOAD_SHOW_TASK_RESULT = 16,
    SUBTASK_LOAD_WAIT_FOR_RESULT_DISMISSAL = 17,
    SUBTASK_LOAD_DONE = 18,
    SUBTASK_DELETE_CONFIRM = 19,
    SUBTASK_DELETE_WAIT_FOR_DELETE_CONFIRMATION = 20,
    SUBTASK_DELETE_DELETE = 21,
    SUBTASK_DELETE_SHOW_TASK_RESULT = 22,
    SUBTASK_DELETE_WAIT_FOR_RESULT_DISMISSAL = 23,
    SUBTASK_DELETE_DONE = 24,
    SUBTASK_DELETE_SINGLE = 25,
    SUBTASK_DELETE_MULTIPLE = 26,
};

enum TitleType {
    TITLE_DEFAULT = 0,
    TITLE_ALTERNATE = 1,
};

enum NameType {
    NAME_ENTRY = 0,
    NAME_PATH = 1,
};

enum DataFormat {
    FORMAT_LAYER2 = 0,
    FORMAT_RAW = 1,
};

struct TimeInfo {
    TimeInfo();
    void Clear();

    unsigned int mCreated;
    unsigned int mLastModified;
    unsigned int mLastAccessed;
};

struct BootupCheckResults {
    void Clear();

    CardId mFirstGoodCard;
    bool mEntryFound;
    unsigned int mNumBlocksNeeded;
};

struct CardInfo {
    CardInfo();
    void Clear();

    CardId mCardId;
    CardStatus mStatus;
    unsigned int mFreeSpace;
    unsigned int mFreeFiles;
    unsigned int mTotalSpace;
    bool mFreeSpaceOverLimit;
    bool mTotalSpaceOverLimit;
};

struct EntryInfo {
    EntryInfo();
    void Clear();

    char *mName;
    CardStatus mStatus;
    unsigned int mEntryBlocks;
    unsigned int mUserDataSize;
    TimeInfo mTime;
    char mCompanyCode[2];
    char mGameCode[4];
};

struct BootupCheckParams {
    void Clear();

    char *mEntryNamePattern;
    unsigned int mNumSaveTypes;
    struct SaveReq **mSaveReqs;
    unsigned int mValidCardIds;
};

struct GameInfo {
    GameInfo(const wchar_t *gameTitle, const unsigned int titleId, bool multipleSaveTypesUsed, bool multitapSupported);
    void Clear();

    wchar_t mGameTitle[33];
    unsigned int mTitleId;
    bool mMultipleSaveTypesUsed;
    bool mMultitapSupported;
};

struct GcSaveInfo {
    GcSaveInfo();
    void Clear();

    const char *mComment1;
    unsigned int mComment1Size;
    const char *mComment2;
    unsigned int mComment2Size;
    void *mIconDataInfo;
    void *mBannerDataInfo;
};

struct Ps2SaveInfo {
    Ps2SaveInfo();
    void Clear();

    const char *mIconSysData;
    unsigned int mIconSysDataSize;
    const char *mStaticIconData;
    unsigned int mStaticIconDataSize;
    const char *mStaticIconFilename;
    const char *mCopyIconData;
    unsigned int mCopyIconDataSize;
    const char *mCopyIconFilename;
    const char *mDeleteIconData;
    unsigned int mDeleteIconDataSize;
    const char *mDeleteIconFilename;
};

struct XboxSaveInfo {
    XboxSaveInfo();
    void Clear();

    const char *mImageData;
    unsigned int mImageDataSize;
};

struct SaveInfo {
    SaveInfo();
    void Clear();

    Ps2SaveInfo mPs2Info;
    XboxSaveInfo mXboxInfo;
    GcSaveInfo mGcInfo;
    unsigned int mHeaderSize;
    unsigned int mBodySize;
    const wchar_t *mTypeName;
    const wchar_t *mContentName;
};

struct SaveReq {
    SaveReq();
    void Clear();

    unsigned int mNumSaves;
    SaveInfo *mSaveInfo;
};

struct TitleInfo {
    void Clear();
    void Init(TitleType titleType, const unsigned int titleId, NameType nameType, DataFormat dataFormat);

    TitleType mTitleType;
    unsigned int mTitleId;
    NameType mNameType;
    DataFormat mDataFormat;
};

struct DirectoryList;

struct IGameInterface {
    virtual void ShowMessage(const unsigned short *, unsigned int, const unsigned short **) = 0;
    virtual void ClearMessage() = 0;
    virtual void BootupCheckDone(CardStatus, BootupCheckResults) = 0;
    virtual void SaveCheckDone(TaskResult, CardStatus) = 0;
    virtual void SaveDone(const char *) = 0;
    virtual DataStatus CheckLoadedData(const char *) = 0;
    virtual void LoadDone(const char *) = 0;
    virtual void DeleteDone(const char *) = 0;
    virtual void ClearEntries() = 0;
    virtual void FoundEntry(const EntryInfo *) = 0;
    virtual void FindEntriesDone(CardStatus) = 0;
    virtual void Retry(CardStatus) = 0;
    virtual void Failed(TaskResult, CardStatus) = 0;
    virtual void CardChanged(TaskResult, CardStatus) = 0;
    virtual void CardChecked(const CardInfo *) = 0;
    virtual void CardRemoved() = 0;
    virtual void SetAutosaveDone(TaskResult, CardStatus, AutosaveState) = 0;
    virtual void SetMonitorDone(CardStatus, MonitorState) = 0;
    virtual TaskStatus LoadReady(const char *, unsigned int, unsigned int, char *&, char *&) = 0;

  protected:
    virtual ~IGameInterface() {}
};

struct MemcardInterfaceImpl;

class MemcardInterface {
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

    static MemcardInterface *CreateInstance(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo);
    static const char *GetFilterForAllEntries();

    void Release();
    MemcardInterface(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo);
    ~MemcardInterface();
    void BootupCheck(const BootupCheckParams *params, unsigned int nEntries, const char **entryNames, wchar_t *content);
    void BootupCheck(const BootupCheckParams *params, unsigned int nEntries, const AutoloadEntry *autoloadEntries);
    void SaveCheck(const char *entryName, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const TitleInfo *titleInfo, const wchar_t *typeName);
    void Delete(const char *entryName, const wchar_t *contentName);
    void DeleteMultiple(unsigned int nEntryNames, const char **entryNames, const unsigned short *contentName);
    void FindEntries(const char *entryNamePattern, const TitleInfo *titleInfo);
    void MessageDone(MessageChoices choice);
    void CheckCard(CardId cardId);
    void SetActiveCard(CardId cardId);
    void SetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId);
    void SetMonitor(MonitorState state);
    void SetMessage(MessageState state, unsigned int message);
    const wchar_t *GetCardName();
    const wchar_t *GetCardName(CardId cardId);
    MemcardTask Update(unsigned int elapsedTime);
    unsigned int CalcSaveSize(const SaveInfo *saveInfo, DataFormat dataFormat);
    void SetMaxCardNameLength(unsigned int maxLength);
    void ConvertAsciiToSjisString(const char *asciiStr, wchar_t *sjisStr);
    Realmc::Interface *GetLowlevelInterface();
    bool IsResettable();
    void SetRootPath(const char *path);

  private:
    MemcardInterfaceImpl *mImpl;
};

} // namespace RealmcIface

#endif
