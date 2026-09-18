#include "./sndcmn.h"

int SNDI_gettag(SNDTAGINFO *pti) {
    while (*pti->p == 0xFC) {
        pti->p = pti->p + 1;
    }

    unsigned char *p = pti->p;

    pti->tag = *p;
    if (pti->tag == 0xFF) {
        return 0;
    }

    pti->p = p + 1;
    if (pti->tag == 0xFD) {
        return 1;
    }
    if (pti->tag == 0xFE) {
        return 1;
    }

    pti->datasize = p[1];
    if (pti->datasize == 0xFF) {
        pti->datasize = SNDI_getb(pti->p + 1, 4);
        pti->p = pti->p + 4;
    }

    pti->p = pti->p + 1;
    pti->pdata = pti->p;
    if (pti->datasize <= 4) {
        pti->size = SNDI_getb(pti->pdata, pti->datasize);
    }

    pti->p = pti->p + pti->datasize;

    return 1;
}
