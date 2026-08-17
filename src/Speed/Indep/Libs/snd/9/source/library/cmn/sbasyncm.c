#include "Speed/Indep/Libs/snd/9/source/library/cmn/sbanki.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"
#include <cstddef>

SNDBANKASYNCMEMSTATE sndbams;

void SNDBANKI_asyncloadmemresolve() {
    BANKLIST *pbl = &sndgs.banklist[sndbams.bhandle];
    BANKVER5 *pb = pbl->phdr;

    int firstoffset;
    int i;
    if (pb->spusize != 0) {
        firstoffset = 0;

        for (i = 0; i < pb->numpatches; i++) {
            if (pb->patch[i] != NULL) {
                SNDBANKI_asyncresolvepatch(0x200, reinterpret_cast<TAGGEDPATCH *>((int)&pb->patch[i] + (int)pb->patch[i]), (char *)pbl->pspuram,
                                           &firstoffset);
            }
        }
    }

    pbl->locked = 0;
    sndbams.loading = 0;
}

void SNDBANKI_asyncloadmem100hz() {
    if (SNDPLATFORM_downloadcomplete(sndbams.dlhandle) == 0) {
        if (sndbams.dlhandle > 0) {
            return;
        }
    }

    BANKLIST *pbl = &sndgs.banklist[sndbams.bhandle];
    BANKVER5 *pb = pbl->phdr;
    int xfersize;
    if (pbl->pspuram != NULL) {
        xfersize = pb->spusize - sndbams.spudownloaded;
        if (xfersize != 0) {
            if (xfersize > 0x10000) {
                xfersize = 0x10000;
            }

            sndbams.dlhandle = SNDPLATFORM_download(0x200, (char *)pb + pb->hdrsize + pb->iopcpusize + sndbams.spudownloaded,
                                                    (char *)pbl->pspuram + sndbams.spudownloaded, xfersize);
            sndbams.spudownloaded += xfersize;

            return;
        }
    }

    SNDBANKI_asyncloadmemresolve();
    SNDSYS_remove100hzclient(SNDBANKI_asyncloadmem100hz);
}

int SNDBANK_asyncloadmem(int *pbhandle, void *pbank) {
    BANKLIST *pbl;
    BANKVER5 *pb;
    int rc = 0;

    sndgs.bankremove = SNDbankremove;

    SNDSYS_entercritical();

    *pbhandle = SNDBANKI_alloc();
    sndbams.bhandle = *pbhandle;

    pbl = &sndgs.banklist[*pbhandle];
    pbl->phdr = reinterpret_cast<BANKVER5 *>(pbank);

    pb = pbl->phdr;

    pbl->pspuram = NULL;
    if (pb->spusize != 0) {
        pbl->pspuram = (char *)SNDPLATFORM_memalloc(0x200, pb->spusize + 0x30);
        if (pbl->pspuram == NULL) {
            pbl->phdr = NULL;
            rc = -6;
            goto abort;
        }
    }
    if (pb->spusize != 0 || pb->iopcpusize != 0) {
        pbl->locked = 1;
        sndbams.loading = 1;
        sndbams.spudownloaded = 0;
        rc = 7;
        SNDSYS_add100hzclient(SNDBANKI_asyncloadmem100hz);
        goto abort;
    }

    SNDBANKI_asyncloadmemresolve();
    rc = 8;

abort:
    SNDSYS_leavecritical();
    return rc;
}

int SNDBANK_asyncloadmemdone() {
    return sndbams.loading == 0;
}
