#include "types.h"

#include "../../../include/common/realcore/system.h"
#include "../../../include/common/realcore/system/threads.h"

#include <dolphin/os/OSAlarm.h>

struct OSAlarmWithSignal {
    OSAlarm Alarm;
    SIGNAL Signal;
};

OSThread *g_thMain;

static int ThreadRealPriority2GCPriority(int realpriority) {
    switch (realpriority) {
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
        default:
            return 16;
    }
}

void THREAD_init() {
    g_thMain = OSGetCurrentThread();
}

bool THREAD_create(THREAD *hThread, int (*entryproc)(void *), void *param, void *stack_addr, int stack_size, int prio) {
    bool ret = true;
    OSThread *MyThread = reinterpret_cast<OSThread *>(hThread);
    if (OSCreateThread(MyThread, reinterpret_cast<void *(*)(void *)>(entryproc), param,
                       static_cast<unsigned char *>(stack_addr) + stack_size, stack_size,
                       ThreadRealPriority2GCPriority(prio), 1) == 0) {
        ret = false;
    }
    OSResumeThread(MyThread);
    return ret;
}

bool THREAD_testexit(THREAD *thread);

void THREAD_destroy(THREAD *thread) {
    OSThread *MyThread;
    if (!THREAD_testexit(thread)) {
        if (thread == reinterpret_cast<THREAD *>(-1)) {
            MyThread = OSGetCurrentThread();
        } else {
            MyThread = reinterpret_cast<OSThread *>(thread);
        }
        if (MyThread != nullptr) {
            OSCancelThread(MyThread);
        }
    }
}

static void AlarmHandler(OSAlarm *alarm, OSContext *) {
    OSAlarmWithSignal *asp = reinterpret_cast<OSAlarmWithSignal *>(alarm);
    SIGNAL_set(&asp->Signal);
}

static void THREAD_yield64(long long durtick) {
    OSAlarmWithSignal as;
    if (durtick == 0) {
        OSYieldThread();
    } else {
        SIGNAL_create(&as.Signal);
        OSCreateAlarm(&as.Alarm);
        OSSetAlarm(&as.Alarm, durtick, AlarmHandler);
        SIGNAL_wait(&as.Signal);
        OSCancelAlarm(&as.Alarm);
        SIGNAL_destroy(&as.Signal);
    }
}

void THREAD_yield(int dur) {
    THREAD_yield64(OSMillisecondsToTicks(static_cast<long long>(dur)));
}

bool THREAD_iscurrent(THREAD *thread) {
    OSThread *thisthread = OSGetCurrentThread();
    OSThread *th;
    if (thread == nullptr) {
        th = g_thMain;
    } else if (thread == reinterpret_cast<THREAD *>(-1)) {
        th = OSGetCurrentThread();
    } else {
        th = reinterpret_cast<OSThread *>(thread);
    }
    return thisthread == th;
}

bool THREAD_setpriority(THREAD *thread, int priority) {
    OSThread *th;
    if (thread == nullptr) {
        th = g_thMain;
    } else if (thread == reinterpret_cast<THREAD *>(-1)) {
        th = OSGetCurrentThread();
    } else {
        th = reinterpret_cast<OSThread *>(thread);
    }
    OSSetThreadPriority(th, ThreadRealPriority2GCPriority(priority));
    return true;
}

bool THREAD_testexit(THREAD *thread) {
    OSThread *th;
    if (thread == nullptr) {
        th = g_thMain;
    } else if (thread == reinterpret_cast<THREAD *>(-1)) {
        th = OSGetCurrentThread();
    } else {
        th = reinterpret_cast<OSThread *>(thread);
    }
    return OSIsThreadTerminated(th) != 0;
}

bool THREAD_waitexit(THREAD *thread, int) {
    while (!THREAD_testexit(thread)) {
        THREAD_yield(10);
    }
    return true;
}
