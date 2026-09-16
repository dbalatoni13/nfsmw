#ifndef REALCORE_COMMON_SYSTEM_H
#define REALCORE_COMMON_SYSTEM_H

// TODO move away
namespace RealSystem {

// total size: 0x1C
struct Mutex {
    char mBuf[28]; // offset 0x0, size 0x1C

    Mutex();
    void Create();
    void Destroy();
    void Lock();
    void Unlock();
};

}; // namespace RealSystem

// total size: 0x318
struct THREAD {
    int reserved[198]; // offset 0x0, size 0x318
};

// total size: 0x28
struct SIGNAL {
    int reserved[10]; // offset 0x0, size 0x28
};

struct MUTEX {
    // total size: 0x1C
    int reserved[7]; // offset 0x0, size 0x1C
};

inline bool MUTEX_create(MUTEX *m) {
    reinterpret_cast<RealSystem::Mutex *>(m)->Create();
    return true;
}

inline void MUTEX_destroy(MUTEX *m) {
    reinterpret_cast<RealSystem::Mutex *>(m)->Destroy();
}

inline void MUTEX_lock(MUTEX *m) {
    reinterpret_cast<RealSystem::Mutex *>(m)->Lock();
}

inline void MUTEX_unlock(MUTEX *m) {
    reinterpret_cast<RealSystem::Mutex *>(m)->Unlock();
}

inline int CPU_getcycle() {}

void SYNCTASK_run();

void THREAD_yield(int dur);
bool THREAD_iscurrent(THREAD *thread);
void THREAD_sleep(int ticks);
void THREAD_create(THREAD *thread, int (*func)(void *), void *param, void *stack, int stackSize, int priority);
void THREAD_waitexit(THREAD *thread, int status);
void THREAD_setpriority(THREAD *thread, int priority);
void THREAD_destroy(THREAD *thread);

#endif
