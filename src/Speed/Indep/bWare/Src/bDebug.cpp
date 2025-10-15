#include "Speed/Indep/bWare/Inc/bDebug.hpp"

#ifdef TARGET_GC
#include <dolphin.h>
#endif

void bInitTicker(float min_wraparound_time);

BOOL EnableReleasePrintf = false;
bool (*UserPutStringFunction)(int, const char *) = nullptr;
bool InUserPutStringFunction = false;

void bWareInit(void) {
    bInitTicker(60000.0f);
}

void bSetUserPutStringFunction(bool (*function)(int, const char *)) {
    UserPutStringFunction = function;
}

bool HandleUserPutString(int terminal_channel, const char *s) {
    bool result;

    if (!UserPutStringFunction || InUserPutStringFunction) {
        result = 0;
    } else {
        InUserPutStringFunction = true;
        result = UserPutStringFunction(terminal_channel, s);
        InUserPutStringFunction = false;
    }
    return result;
}

void bReleasePutString(char terminal_channel, const char *s) {
    if (EnableReleasePrintf && !HandleUserPutString(terminal_channel, s)) {
#ifdef TARGET_GC
        OSReport(s);
#endif
    }
}

void bBreak() {
#ifdef TARGET_GC
    OSPanic("", 0, "");
#endif
}

int bIsValidPointer(void *p, int size) {
    return (reinterpret_cast<uintptr_t>(p) & size - 1) == 0;
}

float bGetTickerDifference(unsigned int start_ticks) {

    return bGetTickerDifference(start_ticks, bGetTicker());
}

int bGetFixTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
    return static_cast<int>(bGetTickerDifference(start_ticks, end_ticks) * 65536.0f);
}

void bInitTicker(float min_wraparound_time) {}

unsigned int bGetTicker() {
#ifdef TARGET_GC
    return OSGetTick();
#endif
}

// UNSOLVED
float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
    if (start_ticks < end_ticks) {
        start_ticks = end_ticks - start_ticks;
    } else {
        start_ticks = end_ticks - start_ticks;
    }
#ifdef TARGET_GC
    return OSTicksToMicroseconds(start_ticks) * 0.001f;
#endif
}

void bMutex::Create() {
    MUTEX_create(reinterpret_cast<MUTEX *>(this));
}

void bMutex::Destroy() {
    MUTEX_destroy(reinterpret_cast<MUTEX *>(this));
}

void bMutex::Lock() {
    MUTEX_lock(reinterpret_cast<MUTEX *>(this));
}

void bMutex::Unlock() {
    MUTEX_unlock(reinterpret_cast<MUTEX *>(this));
}
