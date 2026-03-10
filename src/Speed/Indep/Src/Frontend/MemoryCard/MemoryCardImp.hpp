#ifndef FRONTEND_MEMORYCARD_MEMORYCARDIMP_H
#define FRONTEND_MEMORYCARD_MEMORYCARDIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "RealmcIface.hpp"

struct SaveInfo;
struct MemoryCard;

// total size: 0x8
struct SaveReq {
    unsigned int mNumSaves; // offset 0x0, size 0x4
    SaveInfo *mSaveInfo;    // offset 0x4, size 0x4
};

// total size: 0xC
struct MemoryCardImp {
    SaveReq *m_pSaveReq; // offset 0x0, size 0x4
    SaveReq m_SaveReq;   // offset 0x4, size 0x8

    inline MemoryCardImp() : m_pSaveReq(nullptr) {}
    inline SaveInfo *GetSaveInfo() { return m_SaveReq.mSaveInfo; }
    inline SaveReq **GetSaveReqArray() { return &m_pSaveReq; }
    const char *GetPrefix();
    const char *GetTitleId();
    SaveInfo *ConstructSaveInfo(int type, const char *DisplayName, int aSize);
    void DestructSaveInfo();
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam);

    static unsigned short *gEntryType[3];
    static unsigned short gContentName[];
};

#endif
