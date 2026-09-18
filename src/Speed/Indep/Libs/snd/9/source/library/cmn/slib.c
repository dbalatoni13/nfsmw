#include "./sndcmn.h"

void iSNDcalcvol(int chan) {
    CHANPUB *pc = &sndgs.chan[chan];
    float v;

    v = (float)pc->builtinvol * pc->programmedVol * (float)*(short *)&pc->envvol *
        (1.0f / 127.0f / 127.0f);
    pc->finalvol = v;
    if (pc->pvollfo != 0) {
        pc->finalvol = v * (float)pc->pvollfo->xlate[pc->curvollfoentry] * (1.0f / 127.0f);
    }
    if (pc->pvoltable != 0) {
        float t = pc->finalvol * 127.0f;
        int idx;
        if (t >= 0.0f) {
            idx = (int)(t + 0.5f);
        } else {
            idx = (int)(t - 0.5f);
        }
        pc->finalvol = (float)pc->pvoltable->xlate[idx];
    }
    pc->finalvol = pc->finalvol * Snd::gMasterVol;
}
