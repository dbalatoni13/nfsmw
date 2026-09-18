#include "./sndcmn.h"

namespace Snd {

void CsisMutexLock() {
    Csis::System::Lock();
}

void CsisMutexUnlock() {
    Csis::System::Unlock();
}

Result System::VectorToCsisMutex() {
    if (gMutexLockFn == CsisMutexLock) {
        return RESULT_OK;
    }

    gMutexLockFn = CsisMutexLock;
    gMutexUnlockFn = CsisMutexUnlock;

    return RESULT_OK;
}

} // namespace Snd
