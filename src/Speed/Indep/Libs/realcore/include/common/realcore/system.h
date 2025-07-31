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

inline bool MUTEX_create(MUTEX *m) {
    reinterpret_cast<RealSystem::Mutex *>(m)->Create();
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
