#include "./AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

AudioMemoryManager gAudioMemoryManager;
int AudioMemoryPool = -1;

AudioMemoryManager::AudioMemoryManager()
    : m_pMemoryPoolMem(nullptr), m_pSnd11PoolMem(nullptr), m_memoryPoolSize(0), m_Snd11MemPoolSize(0), m_numMemoryAllocations(0) {}

void AudioMemoryManager::InitMemoryPool(eAUDMEMPOOLTYPE etype, int size) {
    if (etype == AUD_MAIN_MEM_POOL) {
        m_memoryPoolSize = size;
        m_pMemoryPoolMem = bMalloc(size, "TODO", __LINE__, 0x2000);
        AudioMemoryPool = bGetFreeMemoryPoolNum();
        bInitMemoryPool(AudioMemoryPool, m_pMemoryPoolMem, m_memoryPoolSize, "Audio Memory Pool");
    }
}

void *AudioMemoryManager::AllocateMemory(int size, const char *debug_name, bool FromTop) {
    if (size <= bLargestMalloc(AudioMemoryPool)) {
        if (!FromTop) {
            return bMalloc(size, "TODO", __LINE__, AudioMemoryPool & 0xF | 0x1000);
        } else {
            return bMalloc(size, "TODO", __LINE__, AudioMemoryPool & 0xF | 0x1040);
        }
    } else {
        bMemoryPrintAllocationsByAddress(AudioMemoryPool, 0, 2147483647);
        if (!FromTop) {
            return bMalloc(size, "TODO", __LINE__, 0x1000);
        } else {
            return bMalloc(size, "TODO", __LINE__, 0x1040);
        }
    }
}

void AudioMemoryManager::FreeMemory(void *mem) {
    if (mem)
        bFree(mem);
}

char *AudioMemoryManager::AllocateMemoryChar(int size, const char *debug_name, bool FromTop) {
    return (char *)AllocateMemory(size, debug_name, FromTop);
}
