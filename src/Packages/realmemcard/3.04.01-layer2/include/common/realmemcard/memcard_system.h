//
#ifndef REALMC_MEMCARD_SYSTEM__H
#define REALMC_MEMCARD_SYSTEM__H // Decl: 4

#include "Allocator/iallocator.h"

namespace Realmc {

typedef int (*ThreadEntryFunc)(void *); // Decl: 10

// total size: 0x4
// Decl: 19
class IThread {
  public:
    enum Priority {
        IDLE_PRIORITY = -3,
        LOW_PRIORITY = -2,
        BELOW_PRIORITY = -1,
        NORM_PRIORITY = 0,
        ABOVE_PRIORITY = 1,
        HIGH_PRIORITY = 2,
        CRIT_PRIORITY = 3,
    };

    virtual IThread *CreateInstance() = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void SetStackSize(unsigned int stacksize) = 0;
    virtual void Begin(ThreadEntryFunc func) = 0;
    virtual void WaitForEnd(int) = 0;
    virtual void Sleep(int ticks) = 0;
    virtual void SetPriority(int priority) = 0;
};

// total size: 0x4
// Decl: 99
class IMutex {
  public:
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual IMutex *CreateInstance() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

// total size: 0x10
// Decl: 144
struct SystemInterface {
    EA::Allocator::IAllocator *mAllocator; // offset 0x0, size 0x4
    IThread *mThread;                      // offset 0x4, size 0x4
    IMutex *mMutex;                        // offset 0x8, size 0x4
    const char *(*mGetStrCallback)(int);   // offset 0xC, size 0x4

    void Clear();

    SystemInterface() {
        Clear();
    }
};

}; // namespace Realmc

#endif
