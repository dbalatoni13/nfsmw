#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif

extern volatile unsigned int ticks;
extern volatile unsigned int libticks;
typedef void (*TIMERSUBFUNC)();
extern TIMERSUBFUNC tmrsub[8];

enum TIMERMSG {
    TIMERMSG_DOTIMER = 0,
    TIMERMSG_DOVBLTIMER = 1,
    TIMERMSG_QUIT = 2,
};

void ttInit();
void ttKill();
void ttMsg(TIMERMSG msg);

void SYSTEM_addexit(void (*func)(void));

void TIMER_restore();
static void AlarmHandler(OSAlarm *alarm, OSContext *context);

static OSAlarm Alarm;

static unsigned char bIsTimerInited = 0;
/* Quien DEFINE `TIMERhz` es `systemvars.cpp` (`.sdata` 0x804FF644); aqui el
   objeto extraido lo trae como indefinido.  Definirlo tambien aqui nos metia
   4 B de `.sbss` que el original no tiene, y en el enlace son +8.  */
extern unsigned int TIMERhz;

int TIMER_init(int hz) {
    OSTime now;
    OSTime Period;
    int tmp;

    if (bIsTimerInited) {
        return TIMERhz;
    }
    if (hz == 0) {
        hz = 100;
    }
    TIMERhz = hz;

    now = OSGetTime();
    tmp = (int)((float)OS_TIMER_CLOCK * (1.0f / (float)hz));
    Period = tmp;

    ttInit();
    OSSetPeriodicAlarm(&Alarm, now, Period, AlarmHandler);
    bIsTimerInited = 1;
    SYSTEM_addexit(TIMER_restore);

    return TIMERhz;
}

void TIMER_restore() {
    if (bIsTimerInited) {
        bIsTimerInited = 0;
        OSCancelAlarm(&Alarm);
        ttKill();
    }
}

void ttDoTimerMsg() {
    ticks = ticks + 1;
    libticks = libticks + 1;

    for (int i = 0; i < 8; i++) {
        if (tmrsub[i] != 0) {
            tmrsub[i]();
        }
    }
}

static void AlarmHandler(OSAlarm *alarm, OSContext *context) {
    ttMsg(TIMERMSG_DOTIMER);
}
