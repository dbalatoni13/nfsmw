#ifndef FRONTEND_MEMORYCARD_REALMCIFACE_H
#define FRONTEND_MEMORYCARD_REALMCIFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Realmc {
struct SystemInterface;
} // namespace Realmc
using Realmc::SystemInterface;

struct IGameInterface;

namespace RealmcIface {
struct GameInfo;

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

// total size: 0xC
struct TimeInfo {
    unsigned int mCreated;      // offset 0x0, size 0x4
    unsigned int mLastModified; // offset 0x4, size 0x4
    unsigned int mLastAccessed; // offset 0x8, size 0x4
};

// total size: 0xC
struct BootupCheckResults {
    void Clear();

    CardId mFirstGoodCard;        // offset 0x0, size 0x4
    bool mEntryFound;             // offset 0x4, size 0x1
    unsigned int mNumBlocksNeeded; // offset 0x8, size 0x4
};

// total size: 0x1C
struct CardInfo {
    CardInfo();
    void Clear();

    CardId mCardId;             // offset 0x0, size 0x4
    CardStatus mStatus;         // offset 0x4, size 0x4
    unsigned int mFreeSpace;    // offset 0x8, size 0x4
    unsigned int mFreeFiles;    // offset 0xC, size 0x4
    unsigned int mTotalSpace;   // offset 0x10, size 0x4
    bool mFreeSpaceOverLimit;   // offset 0x14, size 0x1
    bool mTotalSpaceOverLimit;  // offset 0x18, size 0x1
};

// total size: 0x24
struct EntryInfo {
    EntryInfo();
    void Clear();

    char *mName;                // offset 0x0, size 0x4
    CardStatus mStatus;         // offset 0x4, size 0x4
    unsigned int mEntryBlocks;  // offset 0x8, size 0x4
    unsigned int mUserDataSize; // offset 0xC, size 0x4
    TimeInfo mTime;             // offset 0x10, size 0xC
    char mCompanyCode[2];       // offset 0x1C, size 0x2
    char mGameCode[4];          // offset 0x1E, size 0x4
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

struct SaveInfo;
struct AutoloadEntry;
struct SaveReq {
    unsigned int mNumSaves; // offset 0x0, size 0x4
    SaveInfo *mSaveInfo;    // offset 0x4, size 0x4

    SaveReq();
    void Clear();
};

// total size: 0x10
struct BootupCheckParams {
    char *mEntryNamePattern;    // offset 0x0, size 0x4
    unsigned int mNumSaveTypes; // offset 0x4, size 0x4
    SaveReq **mSaveReqs;        // offset 0x8, size 0x4
    unsigned int mValidCardIds; // offset 0xC, size 0x4

    void Clear();
};

// total size: 0x10
struct TitleInfo {
    TitleType mTitleType;   // offset 0x0, size 0x4
    unsigned int mTitleId;  // offset 0x4, size 0x4
    NameType mNameType;     // offset 0x8, size 0x4
    DataFormat mDataFormat; // offset 0xC, size 0x4

    void Clear();
    void Init(TitleType titleType, unsigned int titleId, NameType nameType, DataFormat dataFormat);
};

struct MemcardInterfaceImpl;

struct GameInfo {
    wchar_t mGameTitle[33];          // offset 0x0, size 0x84
    unsigned int mTitleId;           // offset 0x84, size 0x4
    bool mMultipleSaveTypesUsed;     // offset 0x88, size 0x1
    bool mMultitapSupported;         // offset 0x8C, size 0x1

    GameInfo(const unsigned short *gameTitle, unsigned int titleId,
             bool multipleSaveTypesUsed, bool multitapSupported);
#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
    GameInfo(const wchar_t *gameTitle, unsigned int titleId,
             bool multipleSaveTypesUsed, bool multitapSupported);
#endif
    void Clear();
};

struct MemcardInterface {
    MemcardInterfaceImpl *mImpl;

    static MemcardInterface *CreateInstance(struct SystemInterface *iSystem,
                                            struct IGameInterface *iGame,
                                            struct GameInfo *gameInfo);
    static const char *GetFilterForAllEntries();
    void Release();
    MemcardInterface(struct SystemInterface *iSystem, struct IGameInterface *iGame,
                     struct GameInfo *gameInfo);
    ~MemcardInterface();
    void BootupCheck(const BootupCheckParams *params, unsigned int nEntries,
                     const char **entryNames, wchar_t *content);
    void BootupCheck(const BootupCheckParams *params, unsigned int nEntries,
                     const AutoloadEntry *autoloadEntries);
    void SaveCheck(const char *entryName, const SaveInfo *saveInfo,
                   const TitleInfo *titleInfo);
    void Save(const char *entryName, const char *header, const char *body,
              const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void Save(const char *entryName, const char *header, const char *body,
              const SaveInfo *saveInfo);
    void Load(const char *entryName, char *header, char *body,
              const unsigned short *contentName, const TitleInfo *titleInfo,
              const unsigned short *typeName);
    void Load(const char *entryName, char *header, char *body,
              const wchar_t *contentName, const TitleInfo *titleInfo);
    void Delete(const char *entryName, const unsigned short *contentName);
#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
    void Delete(const char *entryName, const wchar_t *contentName);
#endif
    void DeleteMultiple(unsigned int nEntryNames, const char **entryNames,
                        const unsigned short *contentName);
    void FindEntries(const char *entryNamePattern, const TitleInfo *titleInfo);
    void MessageDone(MessageChoices choice);
    void CheckCard(CardId cardId);
    void SetActiveCard(CardId cardId);
    void SetAutosave(AutosaveState state, unsigned int nSaveReqs,
                     SaveReq **saveReqs, const char *entryName, CardId cardId);
    void SetMonitor(MonitorState state);
    void SetMessage(MessageState state, unsigned int message);
    const unsigned short *GetCardName();
    const unsigned short *GetCardName(CardId cardId);
    MemcardTask Update(unsigned int elapsedTime);
    unsigned int CalcSaveSize(const SaveInfo *saveInfo);
    void SetMaxCardNameLength(unsigned int maxLength);
    bool IsResettable();
    void SetRootPath(const char *path);
};

} // namespace RealmcIface

using RealmcIface::GameInfo;

#endif
