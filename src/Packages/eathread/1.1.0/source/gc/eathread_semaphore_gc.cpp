#include "eathread/eathread_semaphore.h"
#include "dolphin/os.h"
#include "dolphin/os/OSSemaphore.h"
#include "dolphin/os/OSThread.h"
#include <cstddef>
// #include <cstring>

extern "C" void *memset(void *, int, size_t, ...);

EASemaphoreData::EASemaphoreData() {
    memset(this, 0, sizeof(int) * 3);
    this->mnCount = AtomicInt<int>(0);
}

namespace EA {
namespace Thread {

SemaphoreParameters::SemaphoreParameters(int initialCount, bool, const char *) {
    this->mInitialCount = initialCount;
}

Semaphore::Semaphore(int initialCount) : mSemaphoreData() {
    SemaphoreParameters parameters(initialCount, true, NULL);
    this->Init(&parameters);
}

Semaphore::~Semaphore() {}

bool Semaphore::Init(const SemaphoreParameters *parameters) {
    if (parameters != NULL) {
        OSInitSemaphore((OSSemaphore *)this, parameters->mInitialCount);
        return true;
    } else {
        return false;
    }
}

int Semaphore::Wait(const ThreadTime &timeoutAbsolute) {
    int nLastCount;
    if (timeoutAbsolute == -1) {
        nLastCount = OSWaitSemaphore((OSSemaphore *)this);
    } else if (timeoutAbsolute == 0) {
        nLastCount = OSTryWaitSemaphore((OSSemaphore *)this);
        if (nLastCount < 0) return -2;
    } else {
        const unsigned int nTimeout = GetThreadTime() + timeoutAbsolute;
        while ((nLastCount = OSTryWaitSemaphore((OSSemaphore *)this)) < 0 && GetThreadTime() < nTimeout) {
            ThreadSleep(1);
        }
        if (nLastCount < 0) return -2;
    }
    return nLastCount - 1;
}

int Semaphore::Post(int count) {
    int nLastCount = this->mSemaphoreData.mSemaphore.count;
    while (count-- > 0) {
        nLastCount = OSSignalSemaphore((OSSemaphore *)this);
    }

    return ++nLastCount;
}

void ThreadSleep(const ThreadTime &timeout) {
    OSYieldThread();
}

ThreadTime GetThreadTime() {
    return OSTicksToMilliseconds(OSGetTime());
}

}
}
