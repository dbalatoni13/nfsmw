#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os/OSMessage.h>
#endif

struct SIGNAL {
    /* 0x00 */ int field_0;
    /* 0x04 */ unsigned char field_4[0x20];
    /* 0x24 */ void *messages[1];
};

void MEM_fill(void *vdst, unsigned int fill, int size);

int SIGNAL_create(SIGNAL *signal) {
    OSInitMessageQueue((OSMessageQueue *)&signal->field_4, signal->messages, 1);
    return 1;
}

void SIGNAL_set(SIGNAL *signal) {
    void *msg;

    OSSendMessage((OSMessageQueue *)&signal->field_4, &msg, 0);
}

void SIGNAL_wait(SIGNAL *signal) {
    void *msg;

    OSReceiveMessage((OSMessageQueue *)&signal->field_4, &msg, 1);
}

void SIGNAL_destroy(SIGNAL *signal) {
    MEM_fill(signal, 0xdeadbeef, 0x28);
}
