#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>
#include "smixi.h"

extern "C" {
void SFILTER_add(SFILTERNODE **pphead, SFILTERNODE *pnew);
}

extern "C" void MIX_filteradd(int voice, int channel, SNDFILTERDEF *psfd) {
    MIXVOICE *psmv;
    SFILTERNODE *psfn;

    psmv = &sndmix.voices[voice];
    psfn = (SFILTERNODE *)SNDMEMI_allocz(psfd->statesize);
    psfd->filterinit(psfn, psfd->id, channel);
    psfn->priority = psfd->priority;
    psfn->filterfn = psfd->filter;
    psfn->restorefn = psfd->filterrestore;
    psfn->requester = 0;
    SFILTER_add(&psmv->pfilterhead, psfn);
}
