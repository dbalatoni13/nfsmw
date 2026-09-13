#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

namespace RealmcIface {

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
    return MemoryCard::GetInstance();
}

const char *MemoryCardImp::GetPrefix() {
    return "NFSMW";
}

RealmcIface::SaveInfo *MemoryCardImp::ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize) {
    RealmcIface::SaveInfo *pInfo;
    static char sDisplayName[32];

    pInfo = ::new (__FILE__, __LINE__) RealmcIface::SaveInfo;

    if (type == MemoryCard::ST_PROFILE) {
        bStrCpy(sDisplayName, DisplayName);
    }

    {
        int len;

        pInfo->mGcInfo.mComment1 = gComment1;
        len = bStrLen(gComment1);
        pInfo->mGcInfo.mComment1Size = len;
        pInfo->mGcInfo.mComment2 = sDisplayName;
        len = bStrLen(sDisplayName);
        pInfo->mGcInfo.mComment2Size = len;
    }

    pInfo->mGcInfo.mIconDataInfo = MemoryCard::GetInstance()->GetSaveIcon();
    pInfo->mGcInfo.mBannerDataInfo = MemoryCard::GetInstance()->GetSaveBanner();
    this->m_SaveReq.mSaveInfo = pInfo;
    pInfo->mTypeName = MemoryCardImp::gEntryType[type];
    pInfo->mContentName = MemoryCardImp::gContentName;
    pInfo->mHeaderSize = 8;
    pInfo->mBodySize = aSize;
    return pInfo;
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
