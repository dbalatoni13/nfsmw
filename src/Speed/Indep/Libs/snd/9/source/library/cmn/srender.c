#include "./sndcmn.h"

int SNDI_validrendermode(int *prenderindex, SNDIPATCHHEADER *pph) {
    int validrendermode = 0;
    int globalplayloc;
    int globalspatialization;

    for (; *prenderindex < sndgs.sso.set.numrendermodes; (*prenderindex)++) {
        globalplayloc = sndgs.sso.set.rendermode[*prenderindex] & 0x70C;

        if (pph->rendermode & 0x70C) {
            validrendermode = pph->rendermode & globalplayloc;
            if (validrendermode == 0) {
                validrendermode = 0;
                continue;
            }
        } else {
            validrendermode = globalplayloc;
        }

        globalspatialization = sndgs.sso.set.rendermode[*prenderindex] & 0x60;
        if (pph->rendermode & 0x60) {
            validrendermode |= pph->rendermode & globalspatialization;
            if ((pph->rendermode & globalspatialization) == 0) {
                validrendermode = 0;
                continue;
            }
        } else {
            validrendermode |= globalspatialization;
        }

        if (validrendermode != 0) {
            (*prenderindex)++;
            return validrendermode;
        }
    }

    return validrendermode;
}
