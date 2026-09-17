#include "../../../include/vp6_pbdll.h"

extern int VP6_DecodeBool(void *br, int probability);
extern unsigned int VP6_bitread(void *br, int bits);
extern unsigned char VP6_MvUpdateProbs[2][17];

void VP6_ConfigureMvEntropyDecoder(struct PB_INSTANCE *pbi, unsigned char FrameType) {
    int i;

    for (i = 0; i < 2; i++) {
        if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][0])) {
            pbi->IsMvShortProb[i] = VP6_bitread(&pbi->br, 7) << 1;
            if (pbi->IsMvShortProb[i] == 0) {
                pbi->IsMvShortProb[i] = 1;
            }
        }
        if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][1])) {
            pbi->MvSignProbs[i] = VP6_bitread(&pbi->br, 7) << 1;
            if (pbi->MvSignProbs[i] == 0) {
                pbi->MvSignProbs[i] = 1;
            }
        }
    }
    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned int MvUpdateProbsOffset;

        MvUpdateProbsOffset = 2;
        for (j = 0; j < 7; j++) {
            if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset++])) {
                pbi->MvShortProbs[i][j] = VP6_bitread(&pbi->br, 7) << 1;
                if (pbi->MvShortProbs[i][j] == 0) {
                    pbi->MvShortProbs[i][j] = 1;
                }
            }
        }
    }
    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned int MvUpdateProbsOffset;

        MvUpdateProbsOffset = 9;
        for (j = 0; j < 8; j++) {
            if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset++])) {
                pbi->MvSizeProbs[i][j] = VP6_bitread(&pbi->br, 7) << 1;
                if (pbi->MvSizeProbs[i][j] == 0) {
                    pbi->MvSizeProbs[i][j] = 1;
                }
            }
        }
    }
}
