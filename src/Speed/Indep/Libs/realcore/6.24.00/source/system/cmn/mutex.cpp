#include "types.h"

#include "../../../include/common/realcore/std.h"
#include "../../../include/common/realcore/system.h"

#include <dolphin/os/OSMutex.h>

namespace RealSystem {

Mutex::Mutex() {
    MEM_clear(this, sizeof(Mutex));
}

void Mutex::Create() {
    OSInitMutex(reinterpret_cast<OSMutex *>(this->mBuf + 4));
}

void Mutex::Destroy() {}

void Mutex::Lock() {
    OSLockMutex(reinterpret_cast<OSMutex *>(this->mBuf + 4));
}

void Mutex::Unlock() {
    OSUnlockMutex(reinterpret_cast<OSMutex *>(this->mBuf + 4));
}

};
