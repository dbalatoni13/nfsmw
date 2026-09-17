#ifndef REALMEMCARD_MEMCARD_INTERFACE__H
#define REALMEMCARD_MEMCARD_INTERFACE__H

#include "./memcard_system.h"

#include <types.h>

namespace RealmcIface {

const uint16_t MAX_GAMETITLE_LENGTH = 32; // Decl: 19
const uint16_t TITLEID_LENGTH = 9;        // Decl: 20
const uint16_t MAX_FILENAME_LENGTH = 32;  // Decl: 21
const uint16_t MAX_CARDNAME_LENGTH = 64;  // Decl: 22
const uint16_t MAX_PATH_LENGTH = 65;      // Decl: 23
const uint16_t MAX_MESSAGE_OPTIONS = 4;   // Decl: 24
const uint16_t MAX_ENTRYNAME_LENGTH = 64; // Decl: 25
#ifdef EA_PLATFORM_GAMECUBE
typedef unsigned int TitleId; // Decl: 26
#else
typedef char TitleId[13]; // Decl: 26
#endif

// Decl: 38
enum AutosaveState {
    AUTOSAVE_DISABLE = 0,
    AUTOSAVE_ENABLE = 1,
};

// Decl: 59
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

const CardId XBOX_HARDDRIVE_PORT = CARD_UNKNOWN; // Decl: 100 TODO value

// Decl: 105
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

// Decl: 130
enum DataFormat {
    FORMAT_LAYER2 = 0,
    FORMAT_RAW = 1,
};

// Decl: 140
enum DataStatus {
    DATA_OK = 0,
    DATA_CORRUPT = 1,
};

// Decl: 152
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

// Decl: 174
enum MessageChoices {
    CHOICE_NONE = 0,
    CHOICE_OPTION1 = 1,
    CHOICE_OPTION2 = 2,
    CHOICE_OPTION3 = 3,
    CHOICE_OPTION4 = 4,
};

enum MessageIds {
    ID_CHECKINGFORCARD = 0x1,
    ID_NOCARD = 0x2,
    ID_BADCARD = 0x4,
    ID_FILENOTFOUND = 0x8,
    ID_SAVE_READY = 0x100,
    ID_SAVE_WARNING = 0x200,
    ID_SAVE_COMPLETE = 0x400,
    ID_SAVE_FAILED = 0x800,
    ID_LOAD_READY = 0x1000,
    ID_LOAD_WARNING = 0x2000,
    ID_LOAD_COMPLETE = 0x4000,
    ID_LOAD_FAILED = 0x8000,
    ID_DELETE_READY = 0x10000,
    ID_DELETE_WARNING = 0x20000,
    ID_DELETE_COMPLETE = 0x40000,
    ID_DELETE_FAILED = 0x80000,
    ID_ALLMESSAGES = 0xFFFFFFFF,
};

// Decl: 221
enum MessageState {
    MESSAGE_SHOW = 0,
    MESSAGE_HIDE = 1,
    MESSAGE_FORCE = 2,
};

// Decl: 234
enum MonitorState {
    MONITOR_OFF = 0,
    MONITOR_ON = 1,
    MONITOR_ON_USER_CANCELLED = 2,
};

// Decl: 245
enum NameType {
    NAME_ENTRY = 0,
    NAME_PATH = 1,
};

// Decl: 255
enum TaskResult {
    RESULT_SUCCESS = 0,
    RESULT_FAILED = 1,
    RESULT_CANCELLED = 2,
    RESULT_RETRY = 3,
    RESULT_UNKNOWN = 4,
};

// Decl: 269
enum TaskStatus {
    TASK_CONTINUE = 0,
    TASK_CANCEL = 1,
};

// Decl: 280
enum TitleType {
    TITLE_DEFAULT = 0,
    TITLE_ALTERNATE = 1,
};

// total size: 0xC
// Decl: 294
struct BootupCheckResults {
    void Clear();

    CardId mFirstGoodCard;     // offset 0x0, size 0x4
    bool mEntryFound;          // offset 0x4, size 0x1
    uint32_t mNumBlocksNeeded; // offset 0x8, size 0x4
};

// total size: 0x1C
// Decl: 309
struct CardInfo {
    CardInfo();
    void Clear();

    CardId mCardId;                  // offset 0x0, size 0x4
    RealmcIface::CardStatus mStatus; // offset 0x4, size 0x4
    uint32_t mFreeSpace;             // offset 0x8, size 0x4
    uint32_t mFreeFiles;             // offset 0xC, size 0x4
    uint32_t mTotalSpace;            // offset 0x10, size 0x4
    bool mFreeSpaceOverLimit;        // offset 0x14, size 0x1
    bool mTotalSpaceOverLimit;       // offset 0x18, size 0x1
};

// total size: 0xC
// Decl: 332
struct TimeInfo {
    uint32_t mCreated;      // offset 0x0, size 0x4
    uint32_t mLastModified; // offset 0x4, size 0x4
    uint32_t mLastAccessed; // offset 0x8, size 0x4
};

// total size: 0x24
// Decl: 351
struct EntryInfo {
    EntryInfo();
    void Clear();

    char *mName;            // offset 0x0, size 0x4
    CardStatus mStatus;     // offset 0x4, size 0x4
    uint32_t mEntryBlocks;  // offset 0x8, size 0x4
    uint32_t mUserDataSize; // offset 0xC, size 0x4
    TimeInfo mTime;         // offset 0x10, size 0xC
    char mCompanyCode[2];   // offset 0x1C, size 0x2
    char mGameCode[4];      // offset 0x1E, size 0x4
};

// total size: 0x8
// Decl: 371
struct AutoloadEntry {
    char *entryName;  // offset 0x0, size 0x4
    wchar_t *content; // offset 0x4, size 0x4
};

// total size: 0x50
// Decl: 383
class GameInfo {
  public:
    GameInfo(const wchar_t *gameTitle, unsigned int titleId, bool multipleSaveTypesUsed, bool multitapSupported);
    void Clear();

    wchar_t mGameTitle[33];      // offset 0x0, size 0x84
    unsigned int mTitleId;       // offset 0x84, size 0x4
    bool mMultipleSaveTypesUsed; // offset 0x88, size 0x1
    bool mMultitapSupported;     // offset 0x8C, size 0x1
};

// total size: 0x18
struct GcSaveInfo {
    GcSaveInfo();
    void Clear();

    char *mComment1;                          // offset 0x0, size 0x4
    uint32_t mComment1Size;                   // offset 0x4, size 0x4
    char *mComment2;                          // offset 0x8, size 0x4
    uint32_t mComment2Size;                   // offset 0xC, size 0x4
    struct GCIconDataInfo *mIconDataInfo;     // offset 0x10, size 0x4
    struct GCBannerDataInfo *mBannerDataInfo; // offset 0x14, size 0x4
};

// total size: 0x2C
struct Ps2SaveInfo {
    Ps2SaveInfo();
    void Clear();

    char *mIconSysData;           // offset 0x0, size 0x4
    uint32_t mIconSysDataSize;    // offset 0x4, size 0x4
    char *mStaticIconData;        // offset 0x8, size 0x4
    uint32_t mStaticIconDataSize; // offset 0xC, size 0x4
    char *mStaticIconFilename;    // offset 0x10, size 0x4
    char *mCopyIconData;          // offset 0x14, size 0x4
    uint32_t mCopyIconDataSize;   // offset 0x18, size 0x4
    char *mCopyIconFilename;      // offset 0x1C, size 0x4
    char *mDeleteIconData;        // offset 0x20, size 0x4
    uint32_t mDeleteIconDataSize; // offset 0x24, size 0x4
    char *mDeleteIconFilename;    // offset 0x28, size 0x4
};

// total size: 0x8
struct XboxSaveInfo {
    XboxSaveInfo();
    void Clear();

    char *mImageData;        // offset 0x0, size 0x4
    uint32_t mImageDataSize; // offset 0x4, size 0x4
};

// total size: 0x5C
// Decl: 459
class SaveInfo {
  public:
    SaveInfo();   // Decl: 460
    void Clear(); // Decl: 461

    Ps2SaveInfo mPs2Info;        // offset 0x0, size 0x2C
    XboxSaveInfo mXboxInfo;      // offset 0x2C, size 0x8
    GcSaveInfo mGcInfo;          // offset 0x34, size 0x18
    uint32_t mHeaderSize;        // offset 0x4C, size 0x4, Decl: 464
    uint32_t mBodySize;          // offset 0x50, size 0x4, Decl: 465
    const wchar_t *mTypeName;    // offset 0x54, size 0x4, Decl: 466
    const wchar_t *mContentName; // offset 0x58, size 0x4, Decl: 467
};

// total size: 0x8
// Decl: 479
class SaveReq {
  public:
    uint32_t mNumSaves;  // offset 0x0, size 0x4
    SaveInfo *mSaveInfo; // offset 0x4, size 0x4

    SaveReq();
    void Clear();
};

// total size: 0x10
// Decl: 495
class BootupCheckParams {
  public:
    void Clear();

    char *mEntryNamePattern; // offset 0x0, size 0x4
    uint32_t mNumSaveTypes;  // offset 0x4, size 0x4
    SaveReq **mSaveReqs;     // offset 0x8, size 0x4
    uint32_t mValidCardIds;  // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 514
class TitleInfo {
  public:
    void Clear();
    void Init(TitleType titleType, TitleId titleId, NameType nameType, DataFormat dataFormat);

    TitleType mTitleType;   // offset 0x0, size 0x4
    TitleId mTitleId;       // offset 0x4, size 0x4
    NameType mNameType;     // offset 0x8, size 0x4
    DataFormat mDataFormat; // offset 0xC, size 0x4
};

// Decl: 547
class IGameInterface {
  public:
    virtual void ShowMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t **options) = 0;
    virtual void ClearMessage() = 0;
    virtual void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults res) = 0;
    virtual void SaveCheckDone(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void SaveDone(const char *filename) = 0;
    virtual RealmcIface::DataStatus CheckLoadedData(const char *data) = 0;
    virtual void LoadDone(const char *filename) = 0;
    virtual void DeleteDone(const char *filename) = 0;
    virtual void ClearEntries() = 0;
    virtual void FoundEntry(const RealmcIface::EntryInfo *info) = 0;
    virtual void FindEntriesDone(RealmcIface::CardStatus status) = 0;
    virtual void Retry(RealmcIface::CardStatus status) = 0;
    virtual void Failed(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void CardChanged(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void CardChecked(const RealmcIface::CardInfo *info) = 0;
    virtual void CardRemoved() = 0;
    virtual void SetAutosaveDone(RealmcIface::TaskResult res, RealmcIface::CardStatus status, RealmcIface::AutosaveState flag) = 0;
    virtual void SetMonitorDone(RealmcIface::CardStatus status, RealmcIface::MonitorState state) = 0;
    virtual RealmcIface::TaskStatus LoadReady(const char *entryName, uint32_t headerSize, uint32_t bodySize, char *&headerData, char *&bodyData) = 0;
};

class MemcardInterfaceImpl;

// total size: 0x4
// Decl: 880
class MemcardInterface {
  public:
    static MemcardInterface *CreateInstance(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo);
    static const char *GetFilterForAllEntries();
    void Release();
    MemcardInterface(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo);
    ~MemcardInterface();
    void BootupCheck(const BootupCheckParams *params, uint32_t nEntries, const char **entryNames, wchar_t *content);
    void BootupCheck(const BootupCheckParams *params, uint32_t nEntries, const AutoloadEntry *autoloadEntries);
    void SaveCheck(const char *entryName, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo, const TitleInfo *titleInfo);
    void Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo);
    void Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const TitleInfo *titleInfo, const wchar_t *typeName);
    void Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const TitleInfo *titleInfo);
    void Delete(const char *entryName, const unsigned short *contentName);
#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
    void Delete(const char *entryName, const wchar_t *contentName);
#endif
    void DeleteMultiple(uint32_t nEntryNames, const char **entryNames, const wchar_t *contentName);
    void FindEntries(const char *entryNamePattern, const TitleInfo *titleInfo);
    void MessageDone(MessageChoices choice);
    void CheckCard(CardId cardId);
    void SetActiveCard(CardId cardId);
    void SetAutosave(AutosaveState state, uint32_t nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId);
    void SetMonitor(MonitorState state);
    void SetMessage(MessageState state, uint32_t message);
    const wchar_t *GetCardName();
    const wchar_t *GetCardName(CardId cardId);
    MemcardTask Update(uint32_t elapsedTime);
    uint32_t CalcSaveSize(const SaveInfo *saveInfo);
    void SetMaxCardNameLength(uint32_t maxLength);
    bool IsResettable();
    void SetRootPath(const char *path);

  private:
    MemcardInterfaceImpl *mImpl; // Decl: 1179
};

} // namespace RealmcIface

#endif
