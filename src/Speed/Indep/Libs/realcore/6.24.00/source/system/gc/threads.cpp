#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system/threads.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif

struct SIGNAL {
    /* 0x00 */ int field_0;
    /* 0x04 */ unsigned char field_4[0x20];
    /* 0x24 */ void *messages[1];
};

bool SIGNAL_create(SIGNAL *signal);
void SIGNAL_set(SIGNAL *signal);
void SIGNAL_wait(SIGNAL *signal);
void SIGNAL_destroy(SIGNAL *signal);

static int ThreadRealPriority2GCPriority(int priority) {
    switch (priority) {
    case 3:
        return 0;
    case 2:
        return 5;
    case 1:
        return 10;
    case 0:
        return 16;
    case -1:
        return 20;
    case -2:
        return 25;
    case -3:
        return 31;
    }
    return 16;
}

bool THREAD_testexit(THREAD *thread);

THREAD *g_thMain;

void THREAD_init() {
    g_thMain = (THREAD *)OSGetCurrentThread();
}

bool THREAD_create(THREAD *thread, int (*func)(void *), void *param, void *stack, int stacksize, int priority) {
    bool ok = true;

    if (OSCreateThread((OSThread *)thread, (void *(*)(void *))func, param, (char *)stack + stacksize,
                       stacksize, ThreadRealPriority2GCPriority(priority), 1) == 0) {
        ok = false;
    }
    OSResumeThread((OSThread *)thread);
    return ok;
}

void THREAD_destroy(THREAD *thread) {
    if (!THREAD_testexit(thread)) {
        THREAD *th;

        if ((int)thread == -1) {
            th = (THREAD *)OSGetCurrentThread();
        } else {
            th = thread;
        }
        if (th != 0) {
            OSCancelThread((OSThread *)th);
        }
    }
}

static void AlarmHandler(OSAlarm *alarm, OSContext *context) {
    SIGNAL_set((SIGNAL *)((char *)alarm + 0x28));
}

static void THREAD_yield64(long long ticks) {
    if (ticks == 0) {
        OSYieldThread();
    } else {
        OSAlarm alarm;
        SIGNAL signal;

        SIGNAL_create(&signal);
        OSCreateAlarm(&alarm);
        OSSetAlarm(&alarm, ticks, AlarmHandler);
        SIGNAL_wait(&signal);
        OSCancelAlarm(&alarm);
        SIGNAL_destroy(&signal);
    }
}

void THREAD_yield(int millis) {
    THREAD_yield64((long long)millis * (OS_TIMER_CLOCK / 1000));
}

bool THREAD_iscurrent(THREAD *thread) {
    THREAD *current = (THREAD *)OSGetCurrentThread();
    THREAD *th;

    if (thread == 0) {
        th = g_thMain;
    } else if ((int)thread == -1) {
        th = (THREAD *)OSGetCurrentThread();
    } else {
        th = thread;
    }
    return current == th;
}

bool THREAD_setpriority(THREAD *thread, int priority) {
    THREAD *th;

    if (thread == 0) {
        th = g_thMain;
    } else if ((int)thread == -1) {
        th = (THREAD *)OSGetCurrentThread();
    } else {
        th = thread;
    }
    OSSetThreadPriority((OSThread *)th, ThreadRealPriority2GCPriority(priority));
    return true;
}

bool THREAD_testexit(THREAD *thread) {
    if (thread == 0) {
        thread = g_thMain;
    } else if ((int)thread == -1) {
        thread = (THREAD *)OSGetCurrentThread();
    }
    return OSIsThreadTerminated((OSThread *)thread) != 0;
}

bool THREAD_waitexit(THREAD *thread, int timeout) {
    while (!THREAD_testexit(thread)) {
        THREAD_yield(10);
    }
    return true;
}
