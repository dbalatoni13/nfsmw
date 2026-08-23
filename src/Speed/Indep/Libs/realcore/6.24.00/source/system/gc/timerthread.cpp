#include "types.h"

#include <dolphin/os/OSMessage.h>

enum TIMERMSG {
    TIMER_CLICK = 0,
    VTIMER_CLICK = 1,
    TIMER_END = 2,
};

OSMessageQueue TimerThreadMsgQ;
void *TimerThreadMsgData[32];
OSThread TimerThread;
unsigned char TimerThreadStack[4096];
int TimesInited = 0;

extern void ttDoTimerMsg();
extern void ttDoVTimerMsg();

void ttMsg(TIMERMSG msg);
static void *TimerThreadFunc(void *);

void ttInit() {
    if (TimesInited == 0) {
        OSCreateThread(&TimerThread, TimerThreadFunc, nullptr, TimerThreadStack + sizeof(TimerThreadStack), sizeof(TimerThreadStack), 4, 1);
        OSInitMessageQueue(&TimerThreadMsgQ, TimerThreadMsgData, 32);
        OSResumeThread(&TimerThread);
    }
    TimesInited++;
}

void ttKill() {
    TimesInited--;
    if (TimesInited == 0) {
        ttMsg(TIMER_END);
    }
}

void ttMsg(TIMERMSG msg) {
    OSSendMessage(&TimerThreadMsgQ, reinterpret_cast<void *>(msg), 0);
}

static void *TimerThreadFunc(void *) {
    void *msg = nullptr;
    while (msg != reinterpret_cast<void *>(TIMER_END)) {
        OSReceiveMessage(&TimerThreadMsgQ, &msg, 1);
        if (msg == reinterpret_cast<void *>(TIMER_CLICK)) {
            ttDoTimerMsg();
        } else if (msg == reinterpret_cast<void *>(VTIMER_CLICK)) {
            ttDoVTimerMsg();
        }
    }
    return nullptr;
}
