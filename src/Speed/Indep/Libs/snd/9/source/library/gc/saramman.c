#include "../cmn/sndcmn.h"

typedef struct SNDARAMREC {
    unsigned int addr;
    int size;
} SNDARAMREC;

typedef struct SNDARAMSTATE {
    unsigned short numallocs;
    unsigned short maxallocs;
    unsigned int startaddr;
    unsigned int endaddr;
    SNDARAMREC *parec;
} SNDARAMSTATE;

SNDARAMSTATE sndaram;

void SNDARAM_init(int maxallocs) {
    SNDSYS_entercritical();
    sndaram.parec = (SNDARAMREC *)SNDMEMI_allocz(maxallocs * sizeof(SNDARAMREC));
    SNDSYS_leavecritical();
    sndaram.maxallocs = maxallocs;
    sndaram.numallocs = 0;
}

void SNDARAM_setpool(unsigned int pooladdr, int poolsize) {
    sndaram.endaddr = (pooladdr + poolsize) & ~4;
    if (pooladdr < 32) {
        pooladdr = 32;
    }
    sndaram.startaddr = pooladdr;
    if (sndaram.startaddr & 31) {
        sndaram.startaddr = (sndaram.startaddr + 31) & ~31;
    }
    sndaram.startaddr >>= 5;
    sndaram.endaddr >>= 5;
}

void SNDARAM_restore() {
    if (sndaram.parec) {
        SNDSYS_entercritical();
        SNDMEMI_free(sndaram.parec);
        SNDSYS_leavecritical();
        sndaram.parec = 0;
    }
}

void SNDARAM_constrain(unsigned int *pfreeaddr, int *pfreespace) {
    unsigned int offset;

    if (*pfreeaddr < sndaram.startaddr) {
        offset = sndaram.startaddr - *pfreeaddr;
        *pfreeaddr = sndaram.startaddr;
        *pfreespace -= offset;
    }
    if (*pfreeaddr + *pfreespace > sndaram.endaddr) {
        *pfreespace = sndaram.endaddr - *pfreeaddr;
    }
}

int SNDARAM_largestfree(unsigned int *paddr) {
    int i;
    int largest = 0;
    int freespace;
    unsigned int freeaddr;
    SNDARAMREC *pcurrec;
    SNDARAMREC *pprevrec;

    if (sndaram.numallocs == 0) {
        freeaddr = sndaram.startaddr;
        freespace = sndaram.endaddr - sndaram.startaddr;
        SNDARAM_constrain(&freeaddr, &freespace);
        largest = freespace;
        *paddr = freeaddr;
        goto success;
    }
    for (i = 0; i < sndaram.numallocs; i++) {
        pcurrec = &sndaram.parec[i];
        if (i == 0) {
            freeaddr = pcurrec->addr;
            freespace = pcurrec->addr - sndaram.startaddr;
            SNDARAM_constrain(&freeaddr, &freespace);
            if (freespace > largest) {
                largest = freespace;
                *paddr = freeaddr;
            }
        } else {
            pprevrec = &sndaram.parec[i - 1];
            freeaddr = pprevrec->addr + pprevrec->size;
            freespace = pcurrec->addr - freeaddr;
            SNDARAM_constrain(&freeaddr, &freespace);
            if (freespace > largest) {
                largest = freespace;
                *paddr = freeaddr;
            }
        }
    }
    pprevrec = &sndaram.parec[i - 1];
    freeaddr = pprevrec->addr + pprevrec->size;
    freespace = sndaram.endaddr - freeaddr;
    SNDARAM_constrain(&freeaddr, &freespace);
    if (freespace > largest) {
        largest = freespace;
        *paddr = freeaddr;
    }
success:
    *paddr *= 32;
    return largest * 32;
}

unsigned int SNDARAM_alloc(int size) {
    int i = 0;
    int j;
    int freespace;
    unsigned int freeaddr;
    SNDARAMREC *pcurrec;
    SNDARAMREC *pprevrec;

    if (sndaram.numallocs >= sndaram.maxallocs) {
        return 0;
    }
    size += 31;
    size >>= 5;
    if (sndaram.numallocs == 0) {
        freeaddr = sndaram.startaddr;
        freespace = sndaram.endaddr - sndaram.startaddr;
        SNDARAM_constrain(&freeaddr, &freespace);
        if (size > freespace) {
            return 0;
        }
        goto success;
    }
    for (i = 0; i < sndaram.numallocs; i++) {
        pcurrec = &sndaram.parec[i];
        if (i == 0) {
            freeaddr = sndaram.startaddr;
            freespace = pcurrec->addr - sndaram.startaddr;
        } else {
            pprevrec = &sndaram.parec[i - 1];
            freeaddr = pprevrec->addr + pprevrec->size;
            freespace = pcurrec->addr - freeaddr;
        }
        SNDARAM_constrain(&freeaddr, &freespace);
        if (size <= freespace) {
            for (j = sndaram.numallocs; j > i; j--) {
                sndaram.parec[j] = sndaram.parec[j - 1];
            }
            goto success;
        }
    }
    pprevrec = &sndaram.parec[i - 1];
    freeaddr = pprevrec->addr + pprevrec->size;
    freespace = sndaram.endaddr - freeaddr;
    SNDARAM_constrain(&freeaddr, &freespace);
    if (size > freespace) {
        return 0;
    }
success:
    pcurrec = &sndaram.parec[i];
    pcurrec->addr = freeaddr;
    pcurrec->size = size;
    sndaram.numallocs++;
    return freeaddr * 32;
}

void SNDARAM_free(unsigned int address) {
    int i;
    SNDARAMREC *prec;

    address >>= 5;
    for (i = 0; i < sndaram.numallocs; i++) {
        prec = &sndaram.parec[i];
        if (prec->addr == address) {
            sndaram.numallocs--;
            for (; i < sndaram.numallocs; i++) {
                sndaram.parec[i] = sndaram.parec[i + 1];
            }
            return;
        }
    }
}
