#ifndef FRONTEND_MEMORYCARD_REALMCIFACE_H
#define FRONTEND_MEMORYCARD_REALMCIFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace RealmcIface {

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
    CardId mFirstGoodCard;        // offset 0x0, size 0x4
    bool mEntryFound;             // offset 0x4, size 0x1
    unsigned int mNumBlocksNeeded; // offset 0x8, size 0x4
};

// total size: 0x1C
struct CardInfo {
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
    char *mName;                // offset 0x0, size 0x4
    CardStatus mStatus;         // offset 0x4, size 0x4
    unsigned int mEntryBlocks;  // offset 0x8, size 0x4
    unsigned int mUserDataSize; // offset 0xC, size 0x4
    TimeInfo mTime;             // offset 0x10, size 0xC
    char mCompanyCode[2];       // offset 0x1C, size 0x2
    char mGameCode[4];          // offset 0x1E, size 0x4
};

} // namespace RealmcIface

struct MemcardInterface;

#endif
