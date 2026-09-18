#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os/OSMutex.h>
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include <eekernel.h>
#endif

void MEM_clear(void *ptr, int size);

// 0x1C en GameCube y 0x10 en PS2 (system.h).
RealSystem::Mutex::Mutex() {
    MEM_clear(this, sizeof(Mutex));
}

#ifdef EA_PLATFORM_PLAYSTATION2
// PlayStation 2 (DWARF y ELF de la alpha 124): semaforo del kernel mas una
// cuenta de bloqueos por hilo, con las interrupciones cortadas alrededor.

// total size: 0x10
struct MutexPrivate {
    int mSemaphore;  // offset 0x0, size 0x4
    int mnThreadId;  // offset 0x4, size 0x4
    int mnLockCount; // offset 0x8, size 0x4
    int mnWaitCount; // offset 0xC, size 0x4
};

// realcore/source/system/ps2 (inittmr.cpp): distinto de cero dentro del
// manejador del temporizador, donde solo valen las llamadas i*.
extern int bInTimerSubroutine;

void RealSystem::Mutex::Create() {
    SemaParam sp;

    reinterpret_cast<MutexPrivate *>(this)->mnLockCount = 0;
    reinterpret_cast<MutexPrivate *>(this)->mnWaitCount = 0;
    reinterpret_cast<MutexPrivate *>(this)->mnThreadId = 0;
    sp.maxCount = 0x101;
    sp.initCount = 0;
    reinterpret_cast<MutexPrivate *>(this)->mSemaphore = CreateSema(&sp);
}

void RealSystem::Mutex::Destroy() {
    DeleteSema(reinterpret_cast<MutexPrivate *>(this)->mSemaphore);
}

void RealSystem::Mutex::Lock() {
    MutexPrivate *m = reinterpret_cast<MutexPrivate *>(this);
    int nThreadId = GetThreadId();

    if (m->mnLockCount != 0 && m->mnThreadId == nThreadId) {
        m->mnLockCount++;
        return;
    }

    DI();
    while (m->mnLockCount != 0) {
        m->mnWaitCount++;
        EI();
        WaitSema(m->mSemaphore);
        DI();
    }
    m->mnThreadId = nThreadId;
    m->mnLockCount++;
    EI();
}

void RealSystem::Mutex::Unlock() {
    MutexPrivate *m = reinterpret_cast<MutexPrivate *>(this);

    DI();
    m->mnLockCount--;
    if (m->mnLockCount == 0 && m->mnWaitCount != 0) {
        m->mnWaitCount--;
        EI();
        if (bInTimerSubroutine == 0) {
            SignalSema(m->mSemaphore);
        } else {
            iSignalSema(m->mSemaphore);
        }
        return;
    }
    EI();
}

#elif defined(EA_PLATFORM_GAMECUBE)
void RealSystem::Mutex::Create() {
    OSInitMutex((OSMutex *)&mBuf[4]);
}

void RealSystem::Mutex::Destroy() {
}

void RealSystem::Mutex::Lock() {
    OSLockMutex((OSMutex *)&mBuf[4]);
}

void RealSystem::Mutex::Unlock() {
    OSUnlockMutex((OSMutex *)&mBuf[4]);
}
#endif
