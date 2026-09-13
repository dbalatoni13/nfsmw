#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

#ifdef EA_PLATFORM_WIN32
#include <stdio.h>

// The PC build only carries the MSVC CRT headers. Keep the small Win32 ABI
// declarations local instead of pulling in a host SDK's incompatible headers.
struct bWin32LargeInteger {
    union {
        struct {
            unsigned long LowPart;
            long HighPart;
        };
        __int64 QuadPart;
    };
};

struct bWin32StartupInfo {
    unsigned long cb;
    char *reserved;
    char *desktop;
    char *title;
    unsigned long x;
    unsigned long y;
    unsigned long x_size;
    unsigned long y_size;
    unsigned long x_count_chars;
    unsigned long y_count_chars;
    unsigned long fill_attribute;
    unsigned long flags;
    unsigned short show_window;
    unsigned short reserved2;
    unsigned char *reserved2_data;
    void *std_input;
    void *std_output;
    void *std_error;
};

struct bWin32ProcessInformation {
    void *process;
    void *thread;
    unsigned long process_id;
    unsigned long thread_id;
};

extern "C" __declspec(dllimport) int __stdcall QueryPerformanceCounter(bWin32LargeInteger *counter);
extern "C" __declspec(dllimport) int __stdcall QueryPerformanceFrequency(bWin32LargeInteger *frequency);
extern "C" __declspec(dllimport) int __stdcall IsBadReadPtr(const void *address, unsigned long size);
extern "C" __declspec(dllimport) int __stdcall CreateProcessA(
    const char *application_name, char *command_line, void *process_attributes, void *thread_attributes,
    int inherit_handles, unsigned long creation_flags, void *environment, const char *current_directory,
    bWin32StartupInfo *startup_info, bWin32ProcessInformation *process_information);
extern "C" __declspec(dllimport) unsigned long __stdcall GetLastError();
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char *output);

float __ticker_msperfreq = 0.0f;
int __ticker_cycpertick = 0;
#endif

// for UMath constants static_init
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

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
#ifdef EA_PLATFORM_WIN32
    return false;
#else
    if (bIsCodeineConnected()) {
        // TODO: from sn debug.c
        // return snIsDebuggerRunning();
    }
    return true;
#endif
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
    return 0;
}

// STRIPPED
int bGetTimeString(char *time_string, int max_chars) {
    return 0;
}

void bBreak() {
#ifdef EA_PLATFORM_GAMECUBE
    OSPanic("", 0, "");
#elif defined(EA_PLATFORM_PLAYSTATION2)
    asm("break 0, 1");
#elif defined(EA_PLATFORM_WIN32)
    __asm { int 3 }
#endif
}

int bIsValidPointer(void *p, int size) {
#ifdef EA_PLATFORM_WIN32
    if ((reinterpret_cast<uintptr_t>(p) & (size - 1)) != 0) {
        return false;
    }
    return !IsBadReadPtr(p, 1);
#else
    return (reinterpret_cast<uintptr_t>(p) & size - 1) == 0;
#endif
}

// The PC build does not expose the DOS launcher.
int bLaunch(const char *command_line, int dos_command) {
#ifdef EA_PLATFORM_WIN32
    return -1;
#else
    return 0;
#endif
}

// The PC build delegates process creation to Win32.
int bLaunchWindows(const char *command_line) {
#ifdef EA_PLATFORM_WIN32
    bWin32StartupInfo startup_info = {};
    bWin32ProcessInformation process_info;
    startup_info.cb = sizeof(startup_info);
    if (!CreateProcessA(nullptr, const_cast<char *>(command_line), nullptr, nullptr, false, 0, nullptr, nullptr,
                        &startup_info, &process_info)) {
        return static_cast<int>(GetLastError());
    }
    return 0;
#else
    return 0;
#endif
}

float bGetTickerDifference(unsigned int start_ticks) {
    return bGetTickerDifference(start_ticks, bGetTicker());
}

int bGetFixTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
#ifdef EA_BUILD_A124
    unsigned int ticks = end_ticks - start_ticks;
    return ticks * 0x40 / 0x125;
#else
    return static_cast<int>(bGetTickerDifference(start_ticks, end_ticks) * 65536.0f);
#endif
}

void bInitTicker(float min_wraparound_time) {
#ifdef EA_PLATFORM_WIN32
    bWin32LargeInteger frequency;
    QueryPerformanceFrequency(&frequency);
    __ticker_cycpertick = 0;
    __ticker_msperfreq = 1000.0f / static_cast<float>(frequency.QuadPart);
    float wraparound_time = __ticker_msperfreq * 2147483648.0f;
    while (wraparound_time < min_wraparound_time) {
        ++__ticker_cycpertick;
        wraparound_time += wraparound_time;
        __ticker_msperfreq += __ticker_msperfreq;
    }
#endif
}

unsigned int bGetTicker() {
#ifdef EA_PLATFORM_GAMECUBE
    return OSGetTick();
#elif defined(EA_PLATFORM_WIN32)
    bWin32LargeInteger counter;
    QueryPerformanceCounter(&counter);
    const __int64 ticks = counter.QuadPart;
    return static_cast<unsigned int>(ticks >> __ticker_cycpertick);
#else
    return 0;
#endif
}

float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks) {
#ifdef EA_PLATFORM_GAMECUBE
    unsigned int ticks;
    if (start_ticks < end_ticks) {
        ticks = end_ticks - start_ticks;
    } else {
        ticks = end_ticks - start_ticks;
    }
    return OSTicksToMicroseconds(ticks) * 0.001f;
#elif defined(EA_PLATFORM_WIN32)
    if (__ticker_msperfreq == 0.0f) {
        bInitTicker(60000.0f);
        return 0.0f;
    }
    const int ticks = static_cast<int>(end_ticks - start_ticks);
    float elapsed = static_cast<float>(ticks);
    if (ticks < 0) {
        elapsed += 4294967296.0f;
    }
    return elapsed * __ticker_msperfreq;
#else
    return 0;
#endif
}

// Recovered from the PC ticker comparison and wraparound path.
bool bHasTickerExpired(unsigned int start_ticks, float ms) {
#ifdef EA_PLATFORM_WIN32
    const unsigned int end_ticks = bGetTicker();
    if (__ticker_msperfreq == 0.0f) {
        bInitTicker(60000.0f);
        return false;
    }
    const int ticks = static_cast<int>(end_ticks - start_ticks);
    float elapsed = static_cast<float>(ticks);
    if (ticks < 0) {
        elapsed += 4294967296.0f;
    }
    return (elapsed * __ticker_msperfreq) > ms;
#else
    return false;
#endif
}

int bDisableInterrupts() {
    return 0;
}

void bRestoreInterrupts(int previous_state) {}

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
