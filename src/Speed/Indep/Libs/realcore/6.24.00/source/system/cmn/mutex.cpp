#include "types.h"

#include "../../../include/common/realcore/std.h"
#include "../../../include/common/realcore/system.h"

#include <dolphin/os/OSMutex.h>

namespace RealSystem {

struct MutexPrivate {
    unsigned int signature;
    OSMutex mutex;
};

Mutex::Mutex() {
    MEM_clear(this, sizeof(Mutex));
}

void Mutex::Create() {
    MutexPrivate *m = reinterpret_cast<MutexPrivate *>(this->mBuf);
    OSInitMutex(&m->mutex);
}

void Mutex::Destroy() {}

void Mutex::Lock() {
    MutexPrivate *m = reinterpret_cast<MutexPrivate *>(this->mBuf);
    OSLockMutex(&m->mutex);
}

void Mutex::Unlock() {
    MutexPrivate *m = reinterpret_cast<MutexPrivate *>(this->mBuf);
    OSUnlockMutex(&m->mutex);
}

};
