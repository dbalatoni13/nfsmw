#include "types.h"

#include <dolphin/os/OSAlarm.h>

#include "../../../include/common/realcore/system.h"

enum TIMERMSG {
    TIMER_CLICK = 0,
    VTIMER_CLICK = 1,
    TIMER_END = 2,
};

static OSAlarm Alarm;
static char bIsTimerInited = 0;

extern int TIMERhz;
extern volatile int ticks;
extern volatile int libticks;
extern void (*tmrsub[8])();

extern void ttInit();
extern void ttKill();
extern void ttMsg(TIMERMSG msg);

static void AlarmHandler(OSAlarm *, OSContext *);
void TIMER_restore();

int TIMER_init(int hz) {
    long long now;
    long long Period;
    int tmp;

    if (bIsTimerInited == 0) {
        if (hz == 0) {
            hz = 100;
        }
        TIMERhz = hz;
        now = OSGetTime();
        tmp = OS_TIMER_CLOCK;
        Period = static_cast<int>(static_cast<float>(tmp) * (1.0f / hz));
        ttInit();
        OSSetPeriodicAlarm(&Alarm, now, Period, AlarmHandler);
        bIsTimerInited = 1;
        SYSTEM_addexit(TIMER_restore);
    }
    return TIMERhz;
}

void TIMER_restore() {
    if (bIsTimerInited != 0) {
        bIsTimerInited = 0;
        OSCancelAlarm(&Alarm);
        ttKill();
    }
}

void ttDoTimerMsg() {
    int i;

    ticks++;
    libticks++;
    for (i = 0; i < 8; i++) {
        if (tmrsub[i] != nullptr) {
            tmrsub[i]();
        }
    }
}

static void AlarmHandler(OSAlarm *, OSContext *) {
    ttMsg(TIMER_CLICK);
}
