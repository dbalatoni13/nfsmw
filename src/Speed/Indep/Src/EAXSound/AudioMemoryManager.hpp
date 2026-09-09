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

#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern char *g_pcsCSISAllocString; // Decl: 18
// Decl: 19
enum eAUDMEMPOOLTYPE {
    AUD_MAIN_MEM_POOL = 0,
    AUD_SND11_MEM_POOL = 1,
};

extern SlotPool *pCsisSlotPools[1];
extern char *g_pcsCSISAllocString;

// total size: 0x4
// Decl: 44
class CSISCoreAllocator : public EA::Allocator::ICoreAllocator {
  public:
    // Overrides: ICoreAllocator
    void *Alloc(size_t size, const char *name, unsigned int flags) override {
        return bOMalloc(pCsisSlotPools[0]);
    }

    void *Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset) override {
        return this->Alloc(size, g_pcsCSISAllocString, 0);
    }

    void Free(void *block, size_t size) override {
        bFree(pCsisSlotPools[0], block);
    }
};

extern CSISCoreAllocator g_CSISCoreAllocator; // size: 0x4, address: 0x8045C8E0, Decl: 70
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

    // Decl: 113
    int GetMemoryPoolSize() {
        return this->m_memoryPoolSize;
    }

    // Decl: 118
    void *GetMemoryPoolStart() {
        return this->m_pMemoryPoolMem;
    }

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
