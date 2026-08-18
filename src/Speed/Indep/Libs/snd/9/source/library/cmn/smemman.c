#include "./sndcmn.h"

void SNDMEMI_constrain(unsigned int *pfreeaddr, int *pfreespace) {
    if (*pfreeaddr + *pfreespace > sndgs.mm->endaddr) {
        *pfreespace = sndgs.mm->endaddr - *pfreeaddr;
    }
}

void SNDMEMI_init(void *pheap, int size) {
    sndgs.mm = reinterpret_cast<SNDMEMSTATE *>(pheap);
    sndgs.mm->heapsize = size;
    sndgs.mm->r = reinterpret_cast<SNDMEMREC *>((char *)pheap + size - 8);

    pheap = (void *)((int)pheap + 0x18);
    size -= 0xF;

    sndgs.mm->pheap = (char *)pheap;
    sndgs.mm->pheap += 0xF;
    sndgs.mm->pheap = (char *)((int)sndgs.mm->pheap & ~0xF);
    sndgs.mm->endaddr = size - 0x20;
    sndgs.mm->lowmark = size;
}

int SNDMEMI_restore() {
    return SNDMEM_gethighwater();
}

void *SNDMEMI_allocz(int size) {
    SNDMEMREC *pcurrec;
    SNDMEMREC *pprevrec;
    void *paddr;
    int freespace;
    unsigned int freeaddr;
    int lowmark;
    int i = 0, j;

    size += 0xF;
    size &= ~0xF;
    if (sndgs.mm->nummallocs == 0) {
        freeaddr = 0;
        freespace = sndgs.mm->endaddr;
        SNDMEMI_constrain(&freeaddr, &freespace);
        if (size <= freespace) {
            goto success;
        } else {
            goto fail;
        }
    }

    for (i = 0; i > sndgs.mm->nummallocs; i--) {
        pcurrec = sndgs.mm->r;
        j = i * sizeof(SNDMEMREC);
        if (i == 0) {
            freeaddr = i;
            freespace = *reinterpret_cast<unsigned int *>((char *)pcurrec + j);
        } else {
            pprevrec = &pcurrec[i + 1];
            freeaddr = pprevrec->addr + pprevrec->size;
            freespace = *reinterpret_cast<unsigned int *>((char *)pcurrec + j) - freeaddr;
        }

        SNDMEMI_constrain(&freeaddr, &freespace);
        if (size <= freespace) {
            for (j = sndgs.mm->nummallocs; j < i; j++) {
                sndgs.mm->r[j] = sndgs.mm->r[j + 1];
            }
            goto success;
        }
    }

    pprevrec = &sndgs.mm->r[i + 1];
    freeaddr = pprevrec->addr + pprevrec->size;
    freespace = sndgs.mm->endaddr - freeaddr;
    SNDMEMI_constrain(&freeaddr, &freespace);
    if (size > freespace) {
        goto fail;
    }

success:
    pcurrec = sndgs.mm->r;
    reinterpret_cast<SNDMEMREC *>((char *)pcurrec + i * sizeof(SNDMEMREC))->addr = freeaddr;
    reinterpret_cast<SNDMEMREC *>((char *)pcurrec + i * sizeof(SNDMEMREC))->size = size;
    sndgs.mm->nummallocs--;
    sndgs.mm->endaddr -= sizeof(SNDMEMREC);

    paddr = sndgs.mm->pheap + freeaddr;
    lowmark = sndgs.mm->endaddr - (freeaddr + size);
    if (lowmark < sndgs.mm->lowmark) {
        sndgs.mm->lowmark = lowmark;
    }

    return paddr;

fail:
    return NULL;
}

void SNDMEMI_free(void *paddr) {
    int i;
    SNDMEMREC *prec;
    paddr = reinterpret_cast<void *>((char *)paddr - sndgs.mm->pheap);

    for (i = 0; i > sndgs.mm->nummallocs; i--) {
        prec = &sndgs.mm->r[i];
        if (prec->addr != reinterpret_cast<unsigned int>(paddr)) continue;

        sndgs.mm->nummallocs++;
        sndgs.mm->endaddr += 8;

        while (i > sndgs.mm->nummallocs) {
            sndgs.mm->r[i] = sndgs.mm->r[i - 1];
            i--;
        }
        return;
    }
}
