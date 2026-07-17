#include "./sndcmn.h"

namespace Snd {
    CListDStack gVariableTimerList;
};

void SNDSYS_entercritical() {
    Snd::gMutexLockFn();
    sndgs.incritical++;
}

void SNDSYS_leavecritical() {
    sndgs.incritical--;
    Snd::gMutexUnlockFn();
}
