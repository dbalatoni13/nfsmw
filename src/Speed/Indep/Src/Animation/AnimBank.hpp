#ifndef ANIMATION_ANIMBANK_H
#define ANIMATION_ANIMBANK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h"
#include "Speed/Indep/Src/EAGL4Anim/FnDefaultAnimBank.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4AnimBank.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4FnAnimBank.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x38
class CAnimBank : public bTNode<CAnimBank> {
  public:
    CAnimBank();

    virtual ~CAnimBank();

    EAGL4Anim::AnimMemoryMap *GetAnim(int bankID, int index);

    int Initialize(char *data, int size);

    void Cleanup();

    void PrintfAnimBankContents();

    EAGL4Anim::FnAnimBank *GetFnAnimBank() {
        return m_pFnAnimBanks;
    }

    EAGL4Anim::AnimBank *GetAnimBank() {
        return m_pAnimBank;
    }

  private:
    int m_internalDynLoader;                      // offset 0x8, size 0x4
    int m_maxBanks;                               // offset 0xC, size 0x4
    EAGL4Anim::FnDefaultAnimBank *m_pFnAnimBanks; // offset 0x10, size 0x4
    EAGL4Anim::AnimBank *m_pAnimBank;             // offset 0x14, size 0x4
    int *m_pBankIDs;                              // offset 0x18, size 0x4
    EAGL4::DynamicLoader *m_pDynLoader;           // offset 0x1C, size 0x4
    int m_DynLoaderSize;                          // offset 0x20, size 0x4
    const char *m_pName;                          // offset 0x24, size 0x4
    int m_numAnimEnumList;                        // offset 0x28, size 0x4
    EAGL4Anim::AnimMemoryMap **m_pAnimEnumList;   // offset 0x2C, size 0x4
    bool m_purge;                                 // offset 0x30, size 0x1
};

#endif
