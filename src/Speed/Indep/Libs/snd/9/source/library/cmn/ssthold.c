#include "./sndcmn.h"

Snd::SNDSTREAMREQUEST *SNDSTRMI_getrequestptr(int sndrequesthandle);

int SNDSTRM_modifyhold(int sndrequesthandle, int holdtime) {
    Snd::SNDSTREAMREQUEST *psr;
    int rc = -8;

    SNDSYS_entercritical();
    psr = SNDSTRMI_getrequestptr(sndrequesthandle);

    if (psr != NULL) {
        psr->holdtime = (short)holdtime;
        rc = 0;
    }

    SNDSYS_leavecritical();

    return rc;
}
