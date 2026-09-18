#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardImp.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

const char *gComment1 = "NFS Most Wanted";

const char *MemoryCardImp::GetPrefix() {
    return "NFSMW";
}

RealmcIface::SaveInfo *MemoryCardImp::ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize) {
    static char sDisplayName[32];
    RealmcIface::SaveInfo *pInfo = new ("NFS Most Wanted", 0) RealmcIface::SaveInfo;

    if (type == MemoryCard::ST_PROFILE) {
        bStrCpy(sDisplayName, DisplayName);
    }

    pInfo->mGcInfo.mComment1 = gComment1;
    pInfo->mGcInfo.mComment1Size = bStrLen(gComment1);
    pInfo->mGcInfo.mComment2 = sDisplayName;
    pInfo->mGcInfo.mComment2Size = bStrLen(sDisplayName);
    pInfo->mGcInfo.mIconDataInfo = MemoryCard::GetInstance()->GetSaveIcon();
    pInfo->mGcInfo.mBannerDataInfo = MemoryCard::GetInstance()->GetSaveBanner();
    m_SaveReq.mSaveInfo = pInfo;
    pInfo->mTypeName = reinterpret_cast<const wchar_t *>(gEntryType[type]);
    pInfo->mContentName = reinterpret_cast<const wchar_t *>(gContentName);
    pInfo->mHeaderSize = 8;
    pInfo->mBodySize = aSize;
    return pInfo;
}

void MemoryCardImp::DestructSaveInfo() {
    RealmcIface::SaveInfo *pInfo = m_SaveReq.mSaveInfo;
    if (pInfo != 0) {
        delete pInfo;
        pInfo = 0;
        m_SaveReq.mSaveInfo = pInfo;
    }
}

MemoryCard *GetMemcard() {
    return MemoryCard::GetInstance();
}

void MemoryCardImp::BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam) {
    MemoryCard *memcard = GetMemcard();

    if (memcard->m_bRetryBootCheck == 0) {
        return;
    }

    unsigned int kind = static_cast<unsigned int>(status) - 4;
    if (kind <= 2 || status == RealmcIface::STATUS_NO_CARD) {
        cFEng::Get()->QueuePackageMessage(0x8867412D, GetMemcard()->GetScreen()->GetPackageName(), 0);
    } else {
        cFEng::Get()->QueuePackageMessage(0x3A2BE557, GetMemcard()->GetScreen()->GetPackageName(), 0);
    }
}
