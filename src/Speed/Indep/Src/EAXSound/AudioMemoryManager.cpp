#include "./AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp" // needed for symbol order
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int DEBUG_360MEM = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 15

AudioMemoryManager gAudioMemoryManager; // Decl: 128

int AudioMemoryPool = -1; // Decl: 147
int SND11MemoryPoolNum;   // size: 0x4, address: 0xFFFFFFFF, Decl: 148

AudioMemoryManager::AudioMemoryManager()
    : m_pMemoryPoolMem(nullptr), //
      m_pSnd11PoolMem(nullptr),  //
      m_memoryPoolSize(0),       //
      m_Snd11MemPoolSize(0),     //
      m_numMemoryAllocations(0) {}

void AudioMemoryManager::InitMemoryPool(eAUDMEMPOOLTYPE etype, int size) {
    if (etype == AUD_MAIN_MEM_POOL) {
        m_memoryPoolSize = size;
        m_pMemoryPoolMem = bMalloc(size, "Audio Memory Pool", 0, 0x2000);
        AudioMemoryPool = bGetFreeMemoryPoolNum();
        bInitMemoryPool(AudioMemoryPool, m_pMemoryPoolMem, m_memoryPoolSize, "Audio Memory Pool");
    }
}

void *AudioMemoryManager::AllocateMemory(int size, const char *debug_name, bool FromTop) {
    void *memptr;

    if (size <= bLargestMalloc(AudioMemoryPool)) {
        if (!FromTop) {
            memptr = bMalloc(size, debug_name, 0, AudioMemoryPool & 0xF | 0x1000);
        } else {
            memptr = bMalloc(size, debug_name, 0, AudioMemoryPool & 0xF | 0x1040);
        }
    } else {
        bMemoryPrintAllocationsByAddress(AudioMemoryPool, 0, 0x7FFFFFFF);
        if (!FromTop) {
            memptr = bMalloc(size, debug_name, 0, 0x1000);
        } else {
            memptr = bMalloc(size, debug_name, 0, 0x1040);
        }
    }

    return memptr;
}

void AudioMemoryManager::FreeMemory(void *mem) {
    if (mem != nullptr) {
        bFree(mem);
    }
}

char *AudioMemoryManager::AllocateMemoryChar(int size, const char *debug_name, bool FromTop) {
    return static_cast<char *>(AllocateMemory(size, debug_name, FromTop));
}
