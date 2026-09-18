#include "./sndcmn.h"

void iSNDserveraddclient(void (*pfunc)(void)) {
    sndgs.serverclient[sndgs.numserverclients] = pfunc;
    sndgs.numserverclients++;
}

void iSNDserverremoveclient(void (*pfunc)(void)) {
    int i;

    for (i = 0; i < sndgs.numserverclients; i++) {
        if (sndgs.serverclient[i] == pfunc) {
            sndgs.numserverclients--;
            while (i < sndgs.numserverclients) {
                sndgs.serverclient[i] = sndgs.serverclient[i + 1];
                i++;
            }
            return;
        }
    }
}

void SNDSYS_service() {
    int i;

    if (sndgs.installed == 0) {
        return;
    }

    for (i = 0; i < sndgs.numserverclients; i++) {
        sndgs.serverclient[i]();
    }
}
