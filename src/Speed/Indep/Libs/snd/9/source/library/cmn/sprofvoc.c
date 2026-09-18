#include "./sndcmn.h"

// total size: 0xC
typedef struct SNDPROFVOICES {
    unsigned short currentmaincpu; // offset 0x0
    unsigned char currentiopcpu;   // offset 0x2
    unsigned char currentspu;      // offset 0x3
    unsigned short currentdsp;     // offset 0x4
    unsigned short currentds2dhw;  // offset 0x6
    unsigned short currentds3dhw;  // offset 0x8
    char pad[2];                   // offset 0xA
} SNDPROFVOICES;

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
}

int SNDPROFILEI_voicesinrange(int playloc) {
    int minvoicerange;
    int maxvoicerange;
    int voices;
    int i;

    SNDPLATFORM_getvoicerange(playloc, &minvoicerange, &maxvoicerange);
    voices = 0;
    for (i = minvoicerange; i < maxvoicerange; i++) {
        if (sndgs.chan[i].status) {
            voices++;
        }
    }
    return voices;
}

extern "C" int SNDPROFILE_voices(SNDPROFVOICES *pspv) {
    int voices;

    memset(pspv, 0, sizeof(SNDPROFVOICES));
    pspv->currentmaincpu = SNDPROFILEI_voicesinrange(4);
    voices = pspv->currentmaincpu;
    pspv->currentdsp = SNDPROFILEI_voicesinrange(0x200);
    voices += pspv->currentdsp;
    return voices;
}
