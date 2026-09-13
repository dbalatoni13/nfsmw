#ifndef MEMORYCARDIMP_H
#define MEMORYCARDIMP_H

#include <types.h>

#include "Packages/realmemcard/3.04.01-layer2/include/common/realmemcard/memcard_interface.h"
#include "MemoryCard.hpp"

// total size: 0x8
class MemoryCardImp {
  public:
    inline MemoryCardImp() {
        m_pSaveReq = &m_SaveReq;
        m_SaveReq.mNumSaves = 1;
        m_SaveReq.mSaveInfo = nullptr;
    }
    inline RealmcIface::SaveInfo *GetSaveInfo() {
        return m_SaveReq.mSaveInfo;
    }
    inline RealmcIface::SaveReq **GetSaveReqArray() {
        return &m_pSaveReq;
    }
    RealmcIface::SaveInfo *ConstructSaveInfo(MemoryCard::SaveType type, const char *DisplayName, int aSize);
    void DestructSaveInfo();
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults *pParam);
    const char *GetPrefix();
    const char *GetTitleId();

    static uint16 *gEntryType[3];
    static uint16 gContentName[];

  private:
    RealmcIface::SaveReq *m_pSaveReq; // offset 0x0, size 0x4
    RealmcIface::SaveReq m_SaveReq;   // offset 0x4, size 0x8
};

// TODO not sure where these go
uint16 gSaveType0[32];
uint16 gSaveType1[32];
uint16 gSaveType2[16];

uint16 *MemoryCardImp::gEntryType[3] = {gSaveType0, gSaveType1, gSaveType2};

#endif
