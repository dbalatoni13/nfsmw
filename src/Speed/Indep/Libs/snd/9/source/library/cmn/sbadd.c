#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"
#include <cstddef>

void SNDBANKI_userdatacallback(SNDIPATCHHEADER *pph, int shandle, int type) {
    SNDUSERDATACBINFO sudcbi;
    int i;

    while (pph->numuserdata > 0) {
        pph->numuserdata--;
        sudcbi.type = type;
        sudcbi.pdata = pph->puserdata[pph->numuserdata];
        sudcbi.size = pph->userdatasize[pph->numuserdata];
        sudcbi.shandle = shandle;

        for (i = 0; i < sndgs.numuserdataclients; i++) {
            sndgs.userdataclient[i](&sudcbi);
        }
    }
}

int SNDbankadd(int *pbhandle, void *pbank) {
    int firstoffset;
    int xfersize;
    TAGGEDPATCH *ptp;
    SNDIPATCHHEADER ph;
    int moretags;
    int i;
    BANKVER5 *pb = reinterpret_cast<BANKVER5 *>(pbank);

    sndgs.bankremove = SNDbankremove;

    *pbhandle = SNDBANKI_alloc();
    BANKLIST *pbl = &sndgs.banklist[*pbhandle];

    pbl->phdr = pb;
    pbl->locked = 1;
    pbl->pspuram = NULL;

    if (pb->spusize != 0) {
        pbl->pspuram = reinterpret_cast<void *>(SNDPLATFORM_memalloc(0x200, pb->spusize + 0x30));
        if (pbl->pspuram == NULL) {
            pbl->phdr = NULL;
            pbl->locked = 0;
            return -6;
        }

        for (i = 0; i < pb->spusize; i += 0x1000) {
            xfersize = pb->spusize - i;
            if (xfersize > 0x1000) {
                xfersize = 0x1000;
            }

            int dlhandle = SNDPLATFORM_download(0x200, &((char *)pb + pb->hdrsize + pb->iopcpusize)[i], ((char *)pbl->pspuram + i), xfersize);
            while (SNDPLATFORM_downloadcomplete(dlhandle) == 0) {}
        }

        firstoffset = 0;
        for (i = 0; i < pb->numpatches; i++) {
            if (pb->patch[i] != NULL) {
                SNDBANKI_asyncresolvepatch(0x200, reinterpret_cast<TAGGEDPATCH *>(((int)&pb->patch[i] + (int)pb->patch[i])), (char *)pbl->pspuram, &firstoffset);
            }
        }
    }

    if (sndgs.numuserdataclients > 0) {
        for (i = 0; i < pb->numpatches; i++) {
            ptp = SNDBANKI_getppatch(pb, i);
            if (ptp != NULL) {
                pbhandle = reinterpret_cast<int *>(ptp);
                ptp = reinterpret_cast<TAGGEDPATCH *>(&pbhandle[1]);
                do {
                    moretags = SNDI_parsetimbre((void **)&ptp, &ph);
                    SNDBANKI_userdatacallback(&ph, -1, 0);
                } while (moretags != 0);
            }
        }
    }

    pbl->locked = 0;
    if (pb->spusize != 0 || pb->iopcpusize != 0) {
        return 7;
    } else {
        return 8;
    }
}
