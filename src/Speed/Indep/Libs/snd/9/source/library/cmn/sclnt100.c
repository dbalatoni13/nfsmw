#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"

void SNDSYS_add100hzclient(void (*client)(void)) {
    if (sndgs.installed == 0) {
        return;
    }

    sndgs.serverclient100hz[sndgs.numserverclients100hz] = client;
    sndgs.numserverclients100hz++;
}

void SNDSYS_remove100hzclient(void (*client)()) {
    if (sndgs.installed == 0) {
        return;
    }

    SNDSYS_entercritical();

    int i;
    for (i = 0; i < sndgs.numserverclients100hz; i++) {
        if (sndgs.serverclient100hz[i] == client) {
            sndgs.numserverclients100hz--;
            while (i < sndgs.numserverclients100hz) {
                sndgs.serverclient100hz[i] = sndgs.serverclient100hz[i + 1];
                i++;
            }
            break;
        }
    }

abort:
    SNDSYS_leavecritical();
}
