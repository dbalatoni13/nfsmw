#include "./sndcmn.h"

short sndvoicereserved[6];

int SNDVOICEI_isreserved(int voice, int voicesallocated) {
    for (int i = 0; i < voicesallocated; i++) {
        if (sndvoicereserved[i] == voice) {
            return 1;
        }
    }
    return 0;
}

int SNDVOICEI_alloc(int voicesneeded, int priority, int *phandle, int minvoicerange, int maxvoicerange) {
    int voicesallocated = 0;
    int rc = -9;
    int i = 0;
    int lowestpriority;
    int lowestvoice;
    unsigned int timestamp;
    int voice;
    static int lasthandle = 0;
    struct CHANPUB * pv;
    int temphandle;
    int sorted;

    if (voicesallocated < voicesneeded) {
        for (; i < voicesneeded; i++) {
            sndvoicereserved[i] = -1;
        }
    }

    lasthandle += 0x100;
    if (lasthandle < 0) {
        lasthandle = 0;
    }

    i = 0;
    if (voicesallocated < voicesneeded) {
        do {
            i++;
            lowestvoice = -1;
            timestamp = -1;
            for (voice = minvoicerange; voice < maxvoicerange; voice++) {
                pv = &sndgs.chan[voice];
                if (pv->status == 0 && SNDVOICEI_isreserved(voice, voicesallocated) == 0 && pv->timestamp < timestamp) {
                    timestamp = pv->timestamp;
                    lowestvoice = voice;
                }
            }

            if (lowestvoice >= 0) {
                sndvoicereserved[voicesallocated++] = lowestvoice;
            }
        } while (i < voicesneeded);
    }

    i = voicesallocated;
    if (voicesallocated < voicesneeded) {
        for (; i < voicesneeded; i++) {
            lowestpriority = priority;
            if (sndgs.sso.set.stealequalpriorityvoices == 0) {
                lowestpriority = priority - 1;
            }
            lowestvoice = -1;
            timestamp = -1;
            for (voice = minvoicerange; voice < maxvoicerange; voice++) {
                if (SNDVOICEI_isreserved(voice, voicesallocated) != 0) continue;

                pv = &sndgs.chan[voice];
                if (pv->priority > 100) continue;

                if (pv->priority < lowestpriority) {
                    lowestpriority = pv->priority;
                    timestamp = pv->timestamp;
                    lowestvoice = voice;
                } else if (pv->priority == lowestpriority && pv->timestamp < timestamp) {
                    timestamp = pv->timestamp;
                    lowestvoice = voice;
                }
            }

            if (lowestvoice >= 0) {
                sndvoicereserved[voicesallocated++] = lowestvoice;
                if (voicesallocated >= voicesneeded) break;
            }
        }
    }

    if (voicesallocated == voicesneeded) {

    do {
        sorted = 1;
        for (i = 0; i < voicesallocated - 1; i++) {
            if (sndvoicereserved[i] > sndvoicereserved[i + 1]) {
                temphandle = sndvoicereserved[i];
                sndvoicereserved[i] = sndvoicereserved[i + 1];
                sndvoicereserved[i + 1] = temphandle;
                sorted = 0;
            }
        }
    } while (sorted == 0);

    *phandle = lasthandle | sndvoicereserved[0];
    rc = sndvoicereserved[0];

    for (i = 0; i < voicesallocated; i++) {
        pv = &sndgs.chan[sndvoicereserved[i]];
        temphandle = pv->handle;
        if (pv->status != 0) {
            if (temphandle < 0) {
                temphandle = sndgs.chan[pv->masterchan].handle;
            }
            SNDstop(temphandle);
        }

        pv->status = 1;
        pv->timestamp = sndgs.audiotick;
        pv->priority = priority;
    }

    sndgs.chan[sndvoicereserved[0]].handle = *phandle;
    sndgs.chan[sndvoicereserved[0]].voices[0] = sndvoicereserved[0];
    sndgs.chan[sndvoicereserved[0]].masterchan = -1;

    for (i = 1; i < voicesallocated; i++) {
        sndgs.chan[sndvoicereserved[0]].voices[i] = sndvoicereserved[i];
        sndgs.chan[sndvoicereserved[i]].handle = -1;
        sndgs.chan[sndvoicereserved[i]].masterchan = sndvoicereserved[0];
    }

    }

    return rc;
}

void SNDVOICEI_free(int voice) {
    CHANPUB *pv = &sndgs.chan[voice];
    int numtimbres = 0;
    int patchkey = pv->patchkey;
    int i;
    int mastervoice = -1;

    if (patchkey != 0) {
        for (i = 0; i < sndgs.voicestotal; i++) {
            pv = &sndgs.chan[i];
            if (pv->patchkey == patchkey && pv->handle >= 0 && pv->status != 0) {
                numtimbres++;
                if (pv->ismaster != 0) {
                    mastervoice = i;
                }
            }
        }

        pv = &sndgs.chan[voice];
        if (numtimbres == 1) {
            pv->status = 0;
            pv->patchkey = 0;
            pv->ismaster = 0;
            pv->timestamp = sndgs.audiotick;
            return;
        }

        if (sndgs.chan[mastervoice].status == 2 && voice != mastervoice && numtimbres == 2) {
            pv->status = 0;
            pv->patchkey = 0;
            pv->ismaster = 0;
            pv->timestamp = sndgs.audiotick;
            sndgs.chan[mastervoice].status = 0;
            sndgs.chan[mastervoice].patchkey = 0;
            sndgs.chan[mastervoice].ismaster = 0;
            sndgs.chan[mastervoice].timestamp = sndgs.audiotick;
            return;
        }

        if (sndgs.chan[mastervoice].status == 1 && voice == mastervoice) {
            sndgs.chan[mastervoice].status = 2;
            return;
        }

        pv->status = 0;
        pv->patchkey = 0;
        pv->ismaster = 0;
        pv->timestamp = sndgs.audiotick;
        return;
    }

    pv->status = patchkey;
    pv->patchkey = patchkey;
    pv->ismaster = patchkey;
    pv->timestamp = sndgs.audiotick;
}


int SNDVOICEI_get(int handle) {
    if (handle < 0) return -8;

    int voice = handle & 0xFF;
    if (voice >= sndgs.voicestotal) return -8;

    CHANPUB *pv = &sndgs.chan[voice];
    if (pv->status != 0 && pv->handle == handle) {
        return voice;
    }

    return -8;
}
