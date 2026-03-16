#ifndef FRONTEND_MEMORYCARD_MEMORYCARDIMP_H
#define FRONTEND_MEMORYCARD_MEMORYCARDIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "RealmcIface.hpp"

struct MemoryCard;

// total size: 0x8
struct MemoryCardImp {
    RealmcIface::SaveReq *m_pSaveReq; // offset 0x0, size 0x4
    RealmcIface::SaveReq m_SaveReq;   // offset 0x4, size 0x8

    inline MemoryCardImp() {
        m_pSaveReq = &m_SaveReq;
        m_SaveReq.mSaveInfo = nullptr;
        m_SaveReq.mNumSaves = 1;
    }
    inline RealmcIface::SaveInfo *GetSaveInfo() { return m_SaveReq.mSaveInfo; }
    inline RealmcIface::SaveReq **GetSaveReqArray() { return &m_pSaveReq; }
    const char *GetPrefix();
    const char *GetTitleId();
    RealmcIface::SaveInfo *ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize);
    void DestructSaveInfo();
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam);

    static unsigned short *gEntryType[3];
    static unsigned short gContentName[];
};

#endif
