#include "../../../include/vp6_pbdll.h"

extern int VP6_DecodeBool(void *br, int probability);
extern unsigned int VP6_bitread(void *br, int bits);
extern unsigned char VP6_MvUpdateProbs[2][17];

void VP6_ConfigureMvEntropyDecoder(struct PB_INSTANCE *pbi, unsigned char FrameType) {
    int i;
    void *br;
    unsigned char *MvUpdateProbs;
    unsigned char *Output;

    br = &pbi->br;
    MvUpdateProbs = (unsigned char *)VP6_MvUpdateProbs;
    for (i = 0; i < 2; i++) {
        if (VP6_DecodeBool(br, MvUpdateProbs[0])) {
            pbi->IsMvShortProb[i] = VP6_bitread(br, 7) << 1;
            if ((pbi->IsMvShortProb[i] & 0xfe) == 0) {
                pbi->IsMvShortProb[i] = 1;
            }
        }
        if (VP6_DecodeBool(br, MvUpdateProbs[1])) {
            pbi->MvSignProbs[i] = VP6_bitread(br, 7) << 1;
            if ((pbi->MvSignProbs[i] & 0xfe) == 0) {
                pbi->MvSignProbs[i] = 1;
            }
        }
        MvUpdateProbs += 17;
    }

    MvUpdateProbs = (unsigned char *)VP6_MvUpdateProbs;
    Output = (unsigned char *)pbi + 0x70c;
    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned char *MvUpdateProbsPtr;

        MvUpdateProbsPtr = MvUpdateProbs + 2;
        for (j = 0; j < 7; j++) {
            if (VP6_DecodeBool(br, *MvUpdateProbsPtr)) {
                *Output = VP6_bitread(br, 7) << 1;
                if ((*Output & 0xfe) == 0) {
                    *Output = 1;
                }
            }
            MvUpdateProbsPtr++;
            Output++;
        }
        MvUpdateProbs += 17;
    }

    MvUpdateProbs = (unsigned char *)VP6_MvUpdateProbs;
    Output = (unsigned char *)pbi + 0x720;
    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned char *MvUpdateProbsPtr;

        MvUpdateProbsPtr = MvUpdateProbs + 9;
        for (j = 0; j < 8; j++) {
            if (VP6_DecodeBool(br, *MvUpdateProbsPtr)) {
                *Output = VP6_bitread(br, 7) << 1;
                if ((*Output & 0xfe) == 0) {
                    *Output = 1;
                }
            }
            MvUpdateProbsPtr++;
            Output++;
        }
        MvUpdateProbs += 17;
        Output += 1;
    }
}
