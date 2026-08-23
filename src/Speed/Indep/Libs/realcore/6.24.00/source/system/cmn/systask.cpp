#include "types.h"

#include "../../../include/common/realcore/system/systask.h"
#include "../../../include/common/realcore/system/threads.h"

struct SYSTEMTASKSUB {
    void (*func)(void *, int);
    void *param;
    int rate;
    int next;
    int exec;
};

static SYSTEMTASKSUB systemtasksubs[16];
extern volatile int libticks;

void SYNCTASK_add(void (*taskfunc)(void *, int), int rate, int delay, void *param) {
    static int reentry;
    int skipcount = reentry++;
    int index;
    int entry = -1;

    if (rate == -1) {
        rate = 0;
    } else if (rate == 0) {
        rate = 1;
    }

    for (index = 0; index < 16; index++) {
        if (systemtasksubs[index].func == taskfunc) {
            entry = index;
        } else if (systemtasksubs[index].func == nullptr && entry == -1) {
            if (skipcount == 0) {
                entry = index;
            } else {
                skipcount--;
            }
        }
    }

    systemtasksubs[entry].func = taskfunc;
    systemtasksubs[entry].param = param;
    systemtasksubs[entry].rate = rate;
    systemtasksubs[entry].next = libticks + delay;
    systemtasksubs[entry].exec = 0;
    reentry--;
}

void SYNCTASK_del(void (*taskfunc)(void *, int)) {
    int j = 0;
    while (systemtasksubs[j].func != taskfunc) {
        j++;
        if (j > 15) {
            return;
        }
    }
    if (j > 15) {
        return;
    }
    if (systemtasksubs[j].func != taskfunc) {
        return;
    }
    systemtasksubs[j].func = nullptr;
}

void SYNCTASK_run() {
    static int lastsystemtask;
    int i;

    if (THREAD_iscurrent(nullptr)) {
        lastsystemtask = libticks;
        for (i = 0; i < 16; i++) {
            void (*func)(void *, int) = systemtasksubs[i].func;
            if (func != nullptr && systemtasksubs[i].next <= libticks && systemtasksubs[i].exec == 0) {
                systemtasksubs[i].exec = 1;
                func(systemtasksubs[i].param, libticks - systemtasksubs[i].next);
                systemtasksubs[i].exec = 0;
                systemtasksubs[i].next = libticks + systemtasksubs[i].rate;
            }
        }
    }
}
