#ifndef EAXSOUND_AUDIOMEMORYMANAGER_H
#define EAXSOUND_AUDIOMEMORYMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

enum eAUDMEMPOOLTYPE { AUD_MAIN_MEM_POOL = 0, AUD_SND11_MEM_POOL = 1 };

// total size: 0x14
class AudioMemoryManager {
  public:
    AudioMemoryManager(void);

    void InitMemoryPool(eAUDMEMPOOLTYPE, int);
    void *AllocateMemory(int, const char *, bool);
    char *AllocateMemoryChar(int, const char *, bool);
    void FreeMemory(void *);

    void *GetMemPoolMem() const { return m_pMemoryPoolMem; }
    void *GetMemoryPoolStart() { return m_pMemoryPoolMem; }
    int GetMemoryPoolSize() { return m_memoryPoolSize; }
    int GetMemoryPoolSize() const { return m_memoryPoolSize; }

  private:
    void *m_pMemoryPoolMem;     // offset 0x0, size 0x4
    void *m_pSnd11PoolMem;      // offset 0x4, size 0x4
    int m_memoryPoolSize;       // offset 0x8, size 0x4
    int m_Snd11MemPoolSize;     // offset 0xC, size 0x4
    int m_numMemoryAllocations; // offset 0x10, size 0x4
};

extern AudioMemoryManager gAudioMemoryManager;
extern int AudioMemoryPool;

#endif
