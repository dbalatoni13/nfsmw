#ifndef REALCORE_COMMON_SYSTEM_H
#define REALCORE_COMMON_SYSTEM_H

// TODO move away
namespace RealSystem {

struct Mutex {
    // total size: 0x1C
    char mBuf[28]; // offset 0x0, size 0x1C

    Mutex();
    void Create();
    void Destroy();
    void Lock();
    void Unlock();
};

}; // namespace RealSystem

struct MUTEX {
    // total size: 0x1C
    int reserved[7]; // offset 0x0, size 0x1C
};

struct SIGNAL {
    int reserved[10];
};

bool SIGNAL_create(SIGNAL *hSignal);
void SIGNAL_set(SIGNAL *hSignal);
void SIGNAL_wait(SIGNAL *hSignal);
void SIGNAL_destroy(SIGNAL *hSignal);

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

#endif
