#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>
#include <stdio.h>

// for UMath constants static_init
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin.h>
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include "Speed/PSX2/bWare/Src/ee/include/eekernel.h"
#elif defined(EA_PLATFORM_WIN32)
extern "C" __declspec(dllimport) int __stdcall IsBadReadPtr(const void *address, unsigned long size);
#endif

void bFigureOutPSX2Platform();

int EnableReleasePrintf = false;
bool (*UserPutStringFunction)(int, const char *) = nullptr;
bool InUserPutStringFunction = false;

#ifdef EA_PLATFORM_WIN32
struct bStartupInfo {
    unsigned int Size;
    char *Reserved;
    char *Desktop;
    char *Title;
    unsigned int X;
    unsigned int Y;
    unsigned int XSize;
    unsigned int YSize;
    unsigned int XCountChars;
    unsigned int YCountChars;
    unsigned int FillAttribute;
    unsigned int Flags;
    unsigned short ShowWindow;
    unsigned short Reserved2Size;
    unsigned char *Reserved2;
    void *StdInput;
    void *StdOutput;
    void *StdError;
};

struct bProcessInformation {
    void *Process;
    void *Thread;
    unsigned int ProcessId;
    unsigned int ThreadId;
};

extern "C" {
__declspec(dllimport) int __stdcall QueryPerformanceCounter(__int64 *counter);
__declspec(dllimport) int __stdcall QueryPerformanceFrequency(__int64 *frequency);
__declspec(dllimport) void __stdcall OutputDebugStringA(const char *text);
__declspec(dllimport) int __stdcall CreateProcessA(const char *application_name, char *command_line, void *process_attributes,
                                                   void *thread_attributes, int inherit_handles, unsigned int creation_flags,
                                                   void *environment, const char *current_directory, bStartupInfo *startup_info,
                                                   bProcessInformation *process_info);
__declspec(dllimport) unsigned int __stdcall GetLastError();
}

static float _ticker_msperfreq = 0.0f;
static int _ticker_cycpertick = 0;
#endif

#ifdef EA_PLATFORM_PLAYSTATION2
static int NextNewFileCheckerID = 1;
int NewFileCheckResult = 0;
// void (*SendPacketFunction)(/* parameters unknown */);
// void (*ServiceMonitorFunction)(/* parameters unknown */);
// bList bFunkServerList;
int bSuperBenderConnected = false;
#endif

#ifdef MILESTONE_OPT
float bCodeineVersion = 0.0f;
#endif

void bWareInit(void) {
    bInitTicker(60000.0f);
#if defined(EA_PLATFORM_PLAYSTATION2)
    bFigureOutPSX2Platform();
#endif
}

// STRIPPED
void bWareClose() {}

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
        int num_to_send;

        while (num_sent < len) {
            num_to_send = len - num_sent;
            if (num_to_send > 126) {
                num_to_send = 126;
            }
            packet_buffer[0] = (char)terminal_channel;
            bMemCpy(&packet_buffer[1], &s[num_sent], num_to_send);
            num_sent += num_to_send;
            packet_buffer[num_to_send + 1] = '\0';
            bFunkCallASync("CODEINE", 6, packet_buffer, num_to_send + 2);
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
#elif defined(EA_PLATFORM_WIN32)
        printf("%s", s);
        OutputDebugStringA(s);
#endif
    }
}

// STRIPPED
int bNewFileCheckerAdd(const char *filename) {
    return 0;
}

// STRIPPED
void bNewFileCheckerRemove(int id) {}

// STRIPPED
int bCheckForNewFiles() {
    return 0;
}

// STRIPPED
int bCheckForNewFiles(float seconds) {
    return 0;
}

bool bIsDebuggerConnected() {
    if (bIsCodeineConnected()) {
        // TODO: from sn debug.c
        // return snIsDebuggerRunning();
    }
    return true;
}

static int GetCodeineString(char *string, int max_chars, int bfunk_num) {
    int return_code = bFunkCallSync("CODEINE", bfunk_num, nullptr, 0, string, max_chars);

    if (return_code > 0) {
        return true;
    }

    *string = '\0';
    return false;
}

int bGetComputerName(char *computer_name, int max_chars) {
    return GetCodeineString(computer_name, max_chars, 0x5f);
}

// STRIPPED
int bGetHostName(char *host_name, int max_chars) {
    return GetCodeineString(host_name, max_chars, 0x60);
}

// STRIPPED
int bGetTimeString(char *time_string, int max_chars) {
    return GetCodeineString(time_string, max_chars, 0x61);
}

void bBreak() {
#ifdef EA_PLATFORM_GAMECUBE
    OSPanic("", 0, "");
#elif defined(EA_PLATFORM_PLAYSTATION2)
    asm("break 0, 1");
#elif defined(EA_PLATFORM_WIN32)
    __debugbreak();
#endif
}

int bIsValidPointer(void *p, int size) {
    if ((reinterpret_cast<uintptr_t>(p) & size - 1) != 0) {
        return 0;
    }
#ifdef EA_PLATFORM_WIN32
    return !IsBadReadPtr(p, 1);
#else
    return 1;
#endif
}

// STRIPPED
int bLaunch(const char *command_line, int dos_command) {
    return 0;
}

int bLaunchWindows(const char *command_line) {
#ifdef EA_PLATFORM_WIN32
    bStartupInfo startup_info = {};
    bProcessInformation process_info;
    startup_info.Size = sizeof(startup_info);

    if (!CreateProcessA(nullptr, const_cast<char *>(command_line), nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info)) {
        return GetLastError();
    }
#endif
    return 0;
}

float bGetTickerDifference(unsigned int start_ticks) {
    return bGetTickerDifference(start_ticks, bGetTicker());
}

int bGetFixTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
#ifdef EA_PLATFORM_PLAYSTATION2
    unsigned int ticks = end_ticks - start_ticks;
    return ticks * 0x40 / 0x125;
#else
    return static_cast<int>(bGetTickerDifference(start_ticks, end_ticks) * 65536.0f);
#endif
}

void bInitTicker(float min_wraparound_time) {
#ifdef EA_PLATFORM_WIN32
    __int64 frequency;
    QueryPerformanceFrequency(&frequency);

    _ticker_cycpertick = 0;
    _ticker_msperfreq = 1000.0f / static_cast<float>(frequency);
    float wraparound_time = 4294967296.0f * _ticker_msperfreq;
    while (wraparound_time < min_wraparound_time) {
        _ticker_cycpertick++;
        wraparound_time += wraparound_time;
        _ticker_msperfreq += _ticker_msperfreq;
    }
#endif
}

unsigned int bGetTicker() {
#ifdef EA_PLATFORM_GAMECUBE
    return OSGetTick();
#elif defined(EA_PLATFORM_WIN32)
    __int64 counter;
    QueryPerformanceCounter(&counter);
    return static_cast<unsigned int>(counter >> _ticker_cycpertick);
#elif defined(EA_PLATFORM_PLAYSTATION2)
    unsigned int ticks;
    asm volatile("mfc0 %0, $9" : "=r"(ticks));
    return ticks;
#else
    return 0;
#endif
}

float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
#ifdef EA_PLATFORM_GAMECUBE
    unsigned int ticks = end_ticks - start_ticks;
    return OSTicksToMicroseconds(ticks) * 0.001f;
#elif defined(EA_PLATFORM_WIN32)
    if (_ticker_msperfreq == 0.0f) {
        bInitTicker(60000.0f);
        return 0.0f;
    }
    unsigned int ticks = end_ticks - start_ticks;
    return static_cast<float>(ticks) * _ticker_msperfreq;
#elif defined(EA_PLATFORM_PLAYSTATION2)
    unsigned int ticks = end_ticks - start_ticks;
    return static_cast<float>(ticks) * 3.3333333249174757e-6f * 1.016700029373169f;
#else
    return 0;
#endif
}

bool bHasTickerExpired(unsigned int start_ticks, float ms) {
    return bGetTickerDifference(start_ticks, bGetTicker()) > ms;
}

int bDisableInterrupts() {
#ifdef EA_PLATFORM_PLAYSTATION2
    return DIntr() != 0;
#else
    return 0;
#endif
}

void bRestoreInterrupts(int previous_state) {
#ifdef EA_PLATFORM_PLAYSTATION2
    if (previous_state) {
        EIntr();
    }
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
