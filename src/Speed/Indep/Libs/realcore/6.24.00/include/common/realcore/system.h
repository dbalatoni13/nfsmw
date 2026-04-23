#ifndef REALCORE_COMMON_SYSTEM_H
#define REALCORE_COMMON_SYSTEM_H

extern "C" void bMemSet(void *dest, unsigned char pattern, unsigned int size);

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
    bMemSet(m, sizeof(MUTEX), 0);
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
