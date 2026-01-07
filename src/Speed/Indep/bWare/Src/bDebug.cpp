#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <types.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin.h>
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include "Speed/PSX2/bWare/Src/ee/include/eekernel.h"
#endif

void bFigureOutPSX2Platform();

int EnableReleasePrintf = false;
bool (*UserPutStringFunction)(int, const char *) = nullptr;
bool InUserPutStringFunction = false;

#ifdef EA_PLATFORM_PLAYSTATION2
static int NextNewFileCheckerID = 1;
int NewFileCheckResult = 0;
// void (*SendPacketFunction)(/* parameters unknown */);
// void (*ServiceMonitorFunction)(/* parameters unknown */);
// bList bFunkServerList;
int bSonyToolConnected = false;
int bSuperBenderConnected = false;
#endif

#ifdef DEBUG
float bCodeineVersion = 0.0f;
#endif

void bWareInit(void) {
    bInitTicker(60000.0f);
#if defined(EA_PLATFORM_PLAYSTATION2)
    bFigureOutPSX2Platform();
#endif
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

void bSendStringToCodeine(int terminal_channel, const char *s) {
    int len = bStrLen(s);
    int num_sent = 0;
    if (len > 0) {
        char packet_buffer[128];
        int num_to_send = len;

        while (num_sent < len) {
            if (num_to_send > 126) {
                num_to_send = 126;
            }
            packet_buffer[0] = (char)terminal_channel;
            bMemCpy(&packet_buffer[1], &s[num_sent], num_to_send);
            num_sent += num_to_send;
            packet_buffer[num_to_send + 1] = '\0';
            bFunkCallASync("CODEINE", 6, packet_buffer, num_to_send + 2);
            num_to_send = len - num_sent;
        }
    }
}

void bReleasePutString(char terminal_channel, const char *s) {
    if (EnableReleasePrintf && !HandleUserPutString(terminal_channel, s)) {
#ifdef EA_PLATFORM_GAMECUBE
        OSReport(s);
#elif defined(EA_PLATFORM_PLAYSTATION2)
        if (bIsCodeineConnected()) {
            bSendStringToCodeine(terminal_channel, s);
        } else {
            int state = bDisableInterrupts();
            scePrintf("%s", s);
            bRestoreInterrupts(state);
        }
#endif
    }
}

void bBreak() {
#ifdef EA_PLATFORM_GAMECUBE
    OSPanic("", 0, "");
#elif defined(EA_PLATFORM_PLAYSTATION2)
    asm("break 0, 1");
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

#if defined(EA_PLATFORM_PLAYSTATION2)
void bFigureOutPSX2Platform() {
    bSonyToolConnected = false;
    if (GetMemorySize() > 0x4000000) {
        bSonyToolConnected = true;
    }
}
#endif

int bDisableInterrupts() {}

void bRestoreInterrupts(int previous_state) {}

unsigned int bGetTicker() {
#ifdef EA_PLATFORM_GAMECUBE
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
#ifdef EA_PLATFORM_GAMECUBE
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
