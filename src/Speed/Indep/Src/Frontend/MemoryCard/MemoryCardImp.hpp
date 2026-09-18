#ifndef FRONTEND_MEMORYCARD_MEMORYCARDIMP_H
#define FRONTEND_MEMORYCARD_MEMORYCARDIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

struct GCIconDataInfo;
struct GCBannerDataInfo;

// The parts of the RealMemCard library that the frontend (and MemoryCardImp) use.
namespace Realmc {

// total size: 0x4
struct IThread {
    enum Priority {
        IDLE_PRIORITY = -3,
        LOW_PRIORITY = -2,
        BELOW_PRIORITY = -1,
        NORM_PRIORITY = 0,
        ABOVE_PRIORITY = 1,
        HIGH_PRIORITY = 2,
        CRIT_PRIORITY = 3,
    };

    virtual IThread *CreateInstance() = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void SetStackSize(unsigned int stacksize) = 0;
    virtual void Begin(int (*func)(void *)) = 0;
    virtual void WaitForEnd(int) = 0;
    virtual void Sleep(int ticks) = 0;
    virtual void SetPriority(int priority) = 0;
};

// total size: 0x4
struct IMutex {
    virtual IMutex *CreateInstance() = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

// total size: 0x10
struct SystemInterface {
    SystemInterface() {
        Clear();
    }

    // r56-fe: cuerpo EN CLASE.  El objetivo emite Clear en la cola de
    // finish_file (zFe.o 0x027550), no en el punto del parseo de MemoryCard.cpp.
    void Clear() {
        mAllocator = nullptr;
        mThread = nullptr;
        mMutex = nullptr;
        mLocaleGetter = nullptr;
    }

    void *mAllocator;      // offset 0x0
    IThread *mThread;      // offset 0x4
    IMutex *mMutex;        // offset 0x8
    const char *(*mLocaleGetter)(int); // offset 0xC
};

} // namespace Realmc

namespace RealmcIface {

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
    STATUS_UNKNOWN = 0xFFFFFFFF,
};

enum MessageState {
    MESSAGE_SHOW = 0,
    MESSAGE_HIDE = 1,
    MESSAGE_FORCE = 2,
};

enum MonitorState {
    MONITOR_OFF = 0,
    MONITOR_ON = 1,
    MONITOR_ON_USER_CANCELLED = 2,
};

enum AutosaveState {
    AUTOSAVE_DISABLE = 0,
    AUTOSAVE_ENABLE = 1,
};

enum TaskResult {
    RESULT_SUCCESS = 0,
    RESULT_FAILED = 1,
    RESULT_CANCELLED = 2,
    RESULT_RETRY = 3,
    RESULT_UNKNOWN = 4,
};

enum DataStatus {
    DATA_OK = 0,
    DATA_CORRUPT = 1,
};

enum TaskStatus {
    TASK_CONTINUE = 0,
    TASK_CANCEL = 1,
};

struct MemcardInterfaceImpl;

// total size: 0x2C
struct Ps2SaveInfo {
    const char *mIconSysData;         // offset 0x0
    unsigned int mIconSysDataSize;    // offset 0x4
    const char *mStaticIconData;      // offset 0x8
    unsigned int mStaticIconDataSize; // offset 0xC
    const char *mStaticIconFilename;  // offset 0x10
    const char *mCopyIconData;        // offset 0x14
    unsigned int mCopyIconDataSize;   // offset 0x18
    const char *mCopyIconFilename;    // offset 0x1C
    const char *mDeleteIconData;      // offset 0x20
    unsigned int mDeleteIconDataSize; // offset 0x24
    const char *mDeleteIconFilename;  // offset 0x28
};

// total size: 0x8
struct XboxSaveInfo {
    const char *mImageData;      // offset 0x0
    unsigned int mImageDataSize; // offset 0x4
};

// total size: 0x18
struct GcSaveInfo {
    const char *mComment1;                  // offset 0x0
    unsigned int mComment1Size;             // offset 0x4
    const char *mComment2;                  // offset 0x8
    unsigned int mComment2Size;             // offset 0xC
    GCIconDataInfo *mIconDataInfo;          // offset 0x10
    GCBannerDataInfo *mBannerDataInfo;      // offset 0x14
};

// total size: 0x5C
struct SaveInfo {
    SaveInfo();

    Ps2SaveInfo mPs2Info;         // offset 0x0
    XboxSaveInfo mXboxInfo;       // offset 0x2C
    GcSaveInfo mGcInfo;           // offset 0x34
    unsigned int mHeaderSize;     // offset 0x4C
    unsigned int mBodySize;       // offset 0x50
    const wchar_t *mTypeName;     // offset 0x54
    const wchar_t *mContentName;  // offset 0x58
};

// total size: 0xC
struct TimeInfo {
    unsigned int mCreated;      // offset 0x0
    unsigned int mLastModified; // offset 0x4
    unsigned int mLastAccessed; // offset 0x8
};

// total size: 0x24
struct EntryInfo {
    EntryInfo();

    char *mName;                 // offset 0x0
    enum CardStatus mStatus;     // offset 0x4
    unsigned int mEntryBlocks;   // offset 0x8
    unsigned int mUserDataSize;  // offset 0xC
    struct TimeInfo mTime;       // offset 0x10
    char mCompanyCode[2];        // offset 0x1C
    char mGameCode[4];           // offset 0x1E
};

// total size: 0x1C
struct CardInfo {
    CardInfo();

    enum CardId mCardId;        // offset 0x0
    enum CardStatus mStatus;    // offset 0x4
    unsigned int mFreeSpace;    // offset 0x8
    unsigned int mFreeFiles;    // offset 0xC
    unsigned int mTotalSpace;   // offset 0x10
    bool mFreeSpaceOverLimit;   // offset 0x14
    bool mTotalSpaceOverLimit;  // offset 0x18
};

// total size: 0xC
struct BootupCheckResults {
    void Clear();

    enum CardId mFirstGoodCard;    // offset 0x0
    bool mEntryFound;              // offset 0x4
    unsigned int mNumBlocksNeeded; // offset 0x8
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

// total size: 0x10
struct TitleInfo {
    void Init(TitleType type, unsigned int id, NameType nameType, DataFormat format);

    int mPad[4];
};

// total size: 0x8
struct SaveReq {
    SaveReq();

    unsigned int mNumSaves; // offset 0x0
    SaveInfo *mSaveInfo;    // offset 0x4
};

// total size: 0x90
struct GameInfo {
    GameInfo(const wchar_t *name, unsigned int titleId, bool b1, bool b2);

    int mGameTitle[33];            // offset 0x0
    unsigned int mTitleId;         // offset 0x84
    bool mMultipleSaveTypesUsed;   // offset 0x88
    bool mMultitapSupported;       // offset 0x8C
};

// total size: 0x4
struct IGameInterface {
    // El ORDEN de estos virtuales es el del vtable.  Es el de RealmcIface::IGameInterface
    // de realmemcard/include/common/realmemcard/memcard_interface.h, no otro:
    // con LoadReady al final, _vt.16MemcardCallbacks salia con doce entradas
    // corridas y el 100% de zFe era falso (barrido de reubicaciones, r24).
    virtual void ShowMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options) = 0;
    virtual void ClearMessage() = 0;
    virtual void BootupCheckDone(CardStatus status, BootupCheckResults res) = 0;
    virtual void SaveCheckDone(TaskResult result, CardStatus status) = 0;
    virtual void SaveDone(const char *filename) = 0;
    virtual TaskStatus LoadReady(const char *entryName, unsigned int headerSize, unsigned int bodySize, char *&headerData, char *&bodyData) = 0;
    virtual DataStatus CheckLoadedData(const char *data) = 0;
    virtual void LoadDone(const char *filename) = 0;
    virtual void DeleteDone(const char *filename) = 0;
    virtual void ClearEntries() = 0;
    virtual void FoundEntry(const EntryInfo *info) = 0;
    virtual void FindEntriesDone(CardStatus status) = 0;
    virtual void Retry(CardStatus status) = 0;
    virtual void Failed(TaskResult result, CardStatus status) = 0;
    virtual void CardChecked(const CardInfo *info) = 0;
    virtual void SetAutosaveDone(TaskResult res, CardStatus status, AutosaveState flag) = 0;
    virtual void CardRemoved() = 0;
    virtual void SetMonitorDone(CardStatus status, MonitorState state) = 0;
    virtual void CardChanged(TaskResult result, CardStatus status) = 0;
};

// total size: 0x4
struct MemcardInterface {
    static MemcardInterface *CreateInstance(Realmc::SystemInterface *system, IGameInterface *game, GameInfo *info);

    bool IsResettable();
    void Update(unsigned int ticks);
    void SetMessage(MessageState state, unsigned int flags);
    void MessageDone(MessageChoices choice);
    void BootupCheck(const BootupCheckParams *params, unsigned int nEntries, const char **entries, wchar_t *displayName);
    void CheckCard(CardId cardId);
    void SetMonitor(MonitorState state);
    void SetAutosave(AutosaveState state, unsigned int nReqs, SaveReq **reqs, const char *entryName, CardId cardId);
    void Save(const char *entryName, const char *headerData, const char *bodyData, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void FindEntries(const char *filter, const TitleInfo *titleInfo);
    void Load(const char *entryName, char *headerData, char *bodyData, const wchar_t *displayName, const TitleInfo *titleInfo);
    void Delete(const char *entryName, const wchar_t *displayName);

    MemcardInterfaceImpl *mImpl; // offset 0x0
};

} // namespace RealmcIface

// total size: 0x30
struct GCIconDataInfo {
    GCIconDataInfo() {
        mNumIcons = 0;
        mIconData = 0;
    }

    int mNumIcons;   // offset 0x0
    void *mIconData; // offset 0x4
    int mPad[8];     // offset 0x8
    int mSpeed;      // offset 0x28
    int mPad2;       // offset 0x2C
};

// total size: 0x8
struct GCBannerDataInfo {
    GCBannerDataInfo() {
        mBannerData = 0;
    }

    int mFormat;       // offset 0x0
    void *mBannerData; // offset 0x4
};

// total size: 0xC
class MemoryCardImp {
  public:
    MemoryCardImp() {
        m_pSaveReq = &m_SaveReq;
        m_SaveReq.mNumSaves = 1;
        m_SaveReq.mSaveInfo = 0;
    }

    static unsigned short *gEntryType[3];
    static unsigned short gContentName[];

    const char *GetPrefix();
    RealmcIface::SaveInfo *ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize);
    void DestructSaveInfo();
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *res);
    RealmcIface::SaveInfo *GetSaveInfo() {
        return m_SaveReq.mSaveInfo;
    }
    RealmcIface::SaveReq **GetSaveReqArray() {
        return &m_pSaveReq;
    }

    RealmcIface::SaveReq *m_pSaveReq; // offset 0x0
    RealmcIface::SaveReq m_SaveReq;   // offset 0x4
};

#endif
