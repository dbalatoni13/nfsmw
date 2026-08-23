#include "../../../include/common/realcore/std.h"
#include "../../../include/common/realcore/system.h"

#include <dolphin/os/OSMessage.h>

struct GCSIGNAL {
    unsigned int signature;
    OSMessageQueue MsgQ;
    void *Msg[1];
};

bool SIGNAL_create(SIGNAL *hSignal) {
    GCSIGNAL *Signal = reinterpret_cast<GCSIGNAL *>(hSignal);
    OSInitMessageQueue(&Signal->MsgQ, Signal->Msg, 1);
    return true;
}

void SIGNAL_set(SIGNAL *hSignal) {
    GCSIGNAL *Signal = reinterpret_cast<GCSIGNAL *>(hSignal);
    void *dummy;
    OSSendMessage(&Signal->MsgQ, &dummy, 0);
}

void SIGNAL_wait(SIGNAL *hSignal) {
    GCSIGNAL *Signal = reinterpret_cast<GCSIGNAL *>(hSignal);
    void *dummy;
    OSReceiveMessage(&Signal->MsgQ, &dummy, 1);
}

void SIGNAL_destroy(SIGNAL *hSignal) {
    GCSIGNAL *Signal = reinterpret_cast<GCSIGNAL *>(hSignal);
    MEM_fill(Signal, 0xDEADBEEF, sizeof(GCSIGNAL));
}
