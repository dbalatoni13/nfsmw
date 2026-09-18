#include "./sndcmn.h"

int SNDPKTPLAYI_voicetopackethandle(int voice) {
    int i;

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {
        if (sndpps.ppc[i] != NULL) {
            if (SNDVOICEI_get(sndpps.ppc[i]->shandle) == voice) {
                return i;
            }
        }
    }

    return -1;
}
