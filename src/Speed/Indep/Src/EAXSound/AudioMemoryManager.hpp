//
//
//
//
//
//
//
//
//
//
//
#ifndef AUDIOMEMORYMANAGER_HPP
#define AUDIOMEMORYMANAGER_HPP

#include "types.h"

extern char *g_pcsCSISAllocString;
enum eAUDMEMPOOLTYPE {
    AUD_MAIN_MEM_POOL = 0,
    AUD_SND11_MEM_POOL = 1,
};

// extern bCsisCoreAllocator g_CSISCoreAllocator; // size: 0x4, address: 0x8045C8E0, Decl: 70
static void *CSISAllocatorMemAlloc(unsigned int numBytes);
static void CSISAllocatorMemFree(void *memPtr);

// total size: 0x14
// Decl: 106
class AudioMemoryManager {
  public:
    AudioMemoryManager(void);

    void InitMemoryPool(eAUDMEMPOOLTYPE etype, int size);
    void *AllocateMemory(int size, const char *debug_name, bool FromTop);
    char *AllocateMemoryChar(int size, const char *debug_name, bool FromTop);
    void FreeMemory(void *mem);

  private:
    void *m_pMemoryPoolMem;     // offset 0x0, size 0x4
    void *m_pSnd11PoolMem;      // offset 0x4, size 0x4
    int m_memoryPoolSize;       // offset 0x8, size 0x4
    int m_Snd11MemPoolSize;     // offset 0xC, size 0x4
    int m_numMemoryAllocations; // offset 0x10, size 0x4
};

extern AudioMemoryManager gAudioMemoryManager; //  Decl: 128
extern int AudioMemoryPool;

#endif
