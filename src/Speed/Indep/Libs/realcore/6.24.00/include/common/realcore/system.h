#ifndef REALCORE_COMMON_SYSTEM_H
#define REALCORE_COMMON_SYSTEM_H

#include <stdarg.h>

// TODO move away
namespace RealSystem {

struct Mutex {
#ifdef EA_PLATFORM_PLAYSTATION2
    // total size: 0x10 (DWARF de la alpha 124)
    char mBuf[16]; // offset 0x0, size 0x10
#else
    // total size: 0x1C
    char mBuf[28]; // offset 0x0, size 0x1C
#endif

    Mutex();
    void Create();
    void Destroy();
    void Lock();
    void Unlock();
};

}; // namespace RealSystem

struct MUTEX {
#ifdef EA_PLATFORM_PLAYSTATION2
    // total size: 0x10 (DWARF de la alpha 124)
    int reserved[4]; // offset 0x0, size 0x10
#else
    // total size: 0x1C
    int reserved[7]; // offset 0x0, size 0x1C
#endif
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

// La API de sistema que el juego ve a traves de este fichero (MoviePlayer.cpp
// ya solo incluye realcore/system.h). THREAD_iscurrent NO se declara aqui:
// filesys.cpp la declara por su cuenta devolviendo int, y con la de bool su
// AddToQueue deja de casar; asi que el original no la veia en este fichero.
void SYNCTASK_run();

void THREAD_yield(int dur);

// Salida de depuracion (source/debug). Tipos y nombres del DWARF del original.
enum PRINTCHANNEL {
    PRINT_ALL = 0,
    PRINT_REAL = 1,
    PRINT_ALERT = 2,
    PRINT_DEBUG = 3,
    PRINT_STD = 4,
    PRINT_SYSTEM = 5,
    PRINT_CODEC = 6,
    PRINT_MATH = 7,
    PRINT_MEM = 8,
    PRINT_FILE = 9,
    PRINT_STREAM = 10,
    PRINT_INPUT = 11,
    PRINT_EAGLCORE = 12,
    PRINT_EAGLANIM = 13,
    PRINT_EAGLREND = 14,
    PRINT_USER_CHANNEL = 32,
};

enum PRINTDEVICE {
    PRINT_CONSOLE = 0,
    PRINT_DEBUGGER = 1,
    PRINT_LOGFILE = 2,
    PRINT_SURREAL = 3,
    PRINT_USER_DEVICE = 6,
};

typedef void PRINTFUNCTION(PRINTCHANNEL, const char *);

// total size: 0xC
struct PRINTDEVICEINFO {
    const char *name;     // offset 0x0, size 0x4
    PRINTFUNCTION *func;  // offset 0x4, size 0x4
    bool enabled;         // offset 0x8, size 0x4
};

// total size: 0xC
struct PRINTCHANNELINFO {
    const char *name;     // offset 0x0, size 0x4
    bool enabled;         // offset 0x4, size 0x4
    bool library_channel; // offset 0x8, size 0x4
};

void PRINT_string(PRINTCHANNEL channel, const char *format, ...);
void PRINT_vstring(PRINTCHANNEL channel, const char *format, va_list args);
void REAL_abortmessage(const char *format, ...);
void DEBUG_break();

#endif
