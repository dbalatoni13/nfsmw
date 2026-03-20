#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

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

struct BootupCheckResults;
struct GCIconDataInfo;
struct GCBannerDataInfo;

struct Ps2SaveInfo {
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
    const char *mImageData;
    unsigned int mImageDataSize;
};

struct GcSaveInfo {
    const char *mComment1;
    unsigned int mComment1Size;
    const char *mComment2;
    unsigned int mComment2Size;
    GCIconDataInfo *mIconDataInfo;
    GCBannerDataInfo *mBannerDataInfo;
};

struct SaveInfo {
    SaveInfo();

    Ps2SaveInfo mPs2Info;
    XboxSaveInfo mXboxInfo;
    GcSaveInfo mGcInfo;
    unsigned int mHeaderSize;
    unsigned int mBodySize;
    const unsigned short *mTypeName;
    const unsigned short *mContentName;
};

struct SaveReq {
    unsigned int mNumSaves;
    SaveInfo *mSaveInfo;
};
} // namespace RealmcIface

struct MemoryCardImp {
    static unsigned short *gEntryType[MemoryCard::ST_MAX];
    static unsigned short gContentName[];

    RealmcIface::SaveReq *m_pSaveReq;
    RealmcIface::SaveReq m_SaveReq;

    const char *GetPrefix();
    RealmcIface::SaveInfo *ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize);
    void DestructSaveInfo();
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam);
};

char *bStrCpy(char *dst, const char *src);
int bStrLen(const char *str);
MemoryCard *GetMemcard();

extern const char *gComment1;

MemoryCard *GetMemcard() {
    return MemoryCard::s_pThis;
}

const char *MemoryCardImp::GetPrefix() {
    return "NFSMW";
}

RealmcIface::SaveInfo *MemoryCardImp::ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize) {
    static char sDisplayName[32];
    RealmcIface::SaveInfo *save_info = new RealmcIface::SaveInfo;

    if (type == MemoryCard::ST_PROFILE) {
        bStrCpy(sDisplayName, DisplayName);
    }

    save_info->mGcInfo.mComment1 = gComment1;
    save_info->mGcInfo.mComment1Size = bStrLen(gComment1);
    save_info->mGcInfo.mComment2 = sDisplayName;
    save_info->mGcInfo.mComment2Size = bStrLen(sDisplayName);
    save_info->mGcInfo.mIconDataInfo =
        *reinterpret_cast<RealmcIface::GCIconDataInfo **>(reinterpret_cast<char *>(MemoryCard::s_pThis) + 0x10);
    save_info->mGcInfo.mBannerDataInfo =
        *reinterpret_cast<RealmcIface::GCBannerDataInfo **>(reinterpret_cast<char *>(MemoryCard::s_pThis) + 0x14);
    this->m_SaveReq.mSaveInfo = save_info;
    save_info->mTypeName = MemoryCardImp::gEntryType[type];
    save_info->mContentName = MemoryCardImp::gContentName;
    save_info->mHeaderSize = 8;
    save_info->mBodySize = aSize;
    return save_info;
}

void MemoryCardImp::DestructSaveInfo() {
    RealmcIface::SaveInfo *pInfo;

    pInfo = this->m_SaveReq.mSaveInfo;
    if (pInfo) {
        delete pInfo;
        pInfo = 0;
        this->m_SaveReq.mSaveInfo = pInfo;
    }
}

void MemoryCardImp::BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam) {
    MemoryCard *memcard = GetMemcard();

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(memcard) + 0x30)) {
        if (status == RealmcIface::STATUS_CARD_DAMAGED || status == RealmcIface::STATUS_WRONG_DEVICE ||
            status == RealmcIface::STATUS_CARD_FULL || status == RealmcIface::STATUS_NO_CARD) {
            cFEng *fe = cFEng::mInstance;
            void *screen = *reinterpret_cast<void **>(reinterpret_cast<char *>(GetMemcard()) + 0x190);
            fe->QueuePackageMessage(0x8867412D, *reinterpret_cast<const char *const *>(reinterpret_cast<char *>(screen) + 0xC), 0);
        } else {
            cFEng *fe = cFEng::mInstance;
            void *screen = *reinterpret_cast<void **>(reinterpret_cast<char *>(GetMemcard()) + 0x190);
            fe->QueuePackageMessage(0x3A2BE557, *reinterpret_cast<const char *const *>(reinterpret_cast<char *>(screen) + 0xC), 0);
        }
    }
}
