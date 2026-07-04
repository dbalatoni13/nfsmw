#ifndef EATHREAD_EATHREAD_SEMAPHORE_H
#define EATHREAD_EATHREAD_SEMAPHORE_H

#include "dolphin/os/OSSemaphore.h"
#include "eathread/eathread_atomic.h"

struct EASemaphoreData {
    OSSemaphore mSemaphore; // offset 0x0, size 0xC
    AtomicInt<int> mnCount; // offset 0xC, size 0x4

    EASemaphoreData();
};

namespace EA {
namespace Thread {

struct SemaphoreParameters {
    SemaphoreParameters(int initialCount, bool, const char *);

    // Members
    int mInitialCount; // offset 0x0, size 0x4
    bool mbIntraProcess; // offset 0x4, size 0x1
    char mName[16]; // offset 0x8, size 0x10
};

typedef unsigned int ThreadTime;

class Semaphore {
public:
    enum Result {
        kResultError = -1,
        kResultTimeout = -2,
    };

public:
    Semaphore() {}

    Semaphore(const struct SemaphoreParameters * pSemaphoreParameters, bool bDefaultParameters);

    ~Semaphore();

    bool Init(const SemaphoreParameters *pSemaphoreParameters) ;

    int Wait(const ThreadTime &timeoutAbsolute) ;

    int Post(int count) ;

    int GetCount() const;

    void * GetPlatformData() {}

private:
    Semaphore(int initialCount);

    // Semaphore &operator=() {}

    EASemaphoreData mSemaphoreData; // offset 0x0, size 0x10
};

void ThreadSleep(const ThreadTime &timeout);
unsigned int GetThreadTime();

}
}

#endif
