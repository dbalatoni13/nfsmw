#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif

enum TIMERMSG {
    TIMERMSG_DOTIMER = 0,
    TIMERMSG_DOVBLTIMER = 1,
    TIMERMSG_QUIT = 2,
};

extern unsigned int ticks;
extern unsigned int libticks;
typedef void (*TIMERSUBFUNC)();
extern TIMERSUBFUNC tmrsub[8];
typedef void (*VBLTIMERSUBFUNC)();
extern VBLTIMERSUBFUNC vbltmrsub[8];

void ttDoTimerMsg();
void ttDoVTimerMsg();

OSMessageQueue TimerThreadMsgQ;
/* El objeto original exporta los tres: con enlace interno el simbolo sale
   local y la unidad no puede promocionarse. */
void *TimerThreadMsgData[32];
OSThread TimerThread;
unsigned char TimerThreadStack[0x1000];
int TimesInited = 0;

static void *TimerThreadFunc(void *param);

void ttMsg(TIMERMSG msg);

void ttInit() {
    if (TimesInited == 0) {
        OSCreateThread(&TimerThread, TimerThreadFunc, 0,
                       TimerThreadStack + sizeof(TimerThreadStack), sizeof(TimerThreadStack), 4, 1);
        OSInitMessageQueue(&TimerThreadMsgQ, TimerThreadMsgData, 0x20);
        OSResumeThread(&TimerThread);
    }
    TimesInited = TimesInited + 1;
}

void ttKill() {
    TimesInited = TimesInited - 1;
    if (TimesInited == 0) {
        ttMsg(TIMERMSG_QUIT);
    }
}

void ttMsg(TIMERMSG msg) {
    OSSendMessage(&TimerThreadMsgQ, (void *)msg, 0);
}

static void *TimerThreadFunc(void *param) {
    TIMERMSG msg = (TIMERMSG)0;

    while (msg != TIMERMSG_QUIT) {
        OSReceiveMessage(&TimerThreadMsgQ, (void **)&msg, 1);
        if (msg == TIMERMSG_DOTIMER) {
            ttDoTimerMsg();
        } else if (msg == TIMERMSG_DOVBLTIMER) {
            ttDoVTimerMsg();
        }
    }

    return 0;
}
