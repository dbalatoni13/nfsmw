#ifndef EAGL4ANIM_MEMORYPOOLMANAGER_H
#define EAGL4ANIM_MEMORYPOOLMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimTypeId.h"
#include "FnAnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x4
class MemoryPoolManager {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    MemoryPoolManager() {}

    virtual ~MemoryPoolManager() {}

    static void Startup() {}

    static void Shutdown() {}

    static void Init(unsigned int poolSize) {}

    static void Cleanup() {}

    // static unsigned int GetMemoryPoolUsage() {}

    // static unsigned int GetFreePoolSize() {}

    // static void *NewBlock(unsigned int size) {}

    static void DeleteBlock(void *p) {}

    // static void *NewBlockByIdx(unsigned short idx) {}

    static void DeleteBlockByIdx(unsigned short idx, void *ptr) {}

    // static FnAnim *NewFnAnim(AnimTypeId::Type animTypeId) {}

    // static FnAnimMemoryMap *NewFnAnim(AnimMemoryMap *anim) {}

    static void DeleteFnAnim(FnAnim *fnAnim) {}

    static void InitAnimMemoryMap(AnimMemoryMap *anim) {
        gMemoryManager->InitAnimMemoryMapAux(anim);
    }

    static void ResetPool() {}

    static void SetMemoryPoolManager(MemoryPoolManager &manager) {}

    static void AllocateIdxBlock(unsigned short idx);

    // static MemoryPoolManager *GetMemoryPoolManager() {}

    virtual void InitAux(unsigned int poolSize);
    virtual void CleanupAux();
    virtual unsigned int GetMemoryPoolUsageAux();
    virtual unsigned int GetFreePoolSizeAux();
    virtual void *NewBlockAux(unsigned int size);
    virtual void DeleteBlockAux(void *ptr);
    virtual void *NewBlockByIdxAux(unsigned short idx);
    virtual void DeleteBlockByIdxAux(unsigned short idx, void *ptr);
    virtual FnAnim *NewFnAnimAux(AnimTypeId::Type type);
    virtual FnAnimMemoryMap *NewFnAnimAux(AnimMemoryMap *anim);
    virtual void DeleteFnAnimAux(FnAnim *fnAnim);
    virtual void InitAnimMemoryMapAux(AnimMemoryMap *memMap);
    virtual void ResetPoolAux();

  protected:
    static MemoryPoolManager *gDefaultMemoryManager; // size: 0x4, address: 0x80417124
    static MemoryPoolManager *gMemoryManager;        // size: 0x4, address: 0x80417128
    static char *gMemoryPool;                        // size: 0x4, address: 0x80417168
    static char *gMemoryPoolFree;                    // size: 0x4, address: 0x8041716C
    static unsigned int gMemoryPoolSize;             // size: 0x4, address: 0x80417170
    static unsigned short gMaxIdx;                   // size: 0x2, address: 0x80417174
    static unsigned short gFreeListSize[];           // size: 0x0, address: 0x8041712C
    static char *gFreeList[26];                      // size: 0x68, address: 0x80417178
    static char *gSizeFreeList[256];                 // size: 0x400, address: 0x804171E0
    static int gNumUsedBlocks;                       // size: 0x4, address: 0xFFFFFFFF
};

}; // namespace EAGL4Anim

#endif
