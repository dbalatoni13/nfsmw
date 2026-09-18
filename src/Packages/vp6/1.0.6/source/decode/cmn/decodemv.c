typedef struct {
    unsigned char pad0[0x150];
    unsigned char br[24];
    unsigned char pad1[0x708 - 0x150 - 24];
    unsigned char MvSignProbs[2];     /* 0x708 */
    unsigned char MvIsShort[2];       /* 0x70a */
    unsigned char MvShortProbs[2][7]; /* 0x70c */
    unsigned char pad2[0x720 - 0x70c - 14];
    unsigned char MvLongProbs[2][8];  /* 0x720 */
} PB_INSTANCE;

extern int VP6_DecodeBool(void *br, int probability);
extern int VP6_bitread(void *br, int bits);

const unsigned char VP6_MvUpdateProbs[2][17] = {
    {0xED, 0xF6, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFA, 0xFA, 0xFC},
    {0xE7, 0xF3, 0xF5, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFB, 0xFB, 0xFE},
};

const unsigned char DefaultMvShortProbs[2][7] = {
    {0xE1, 0x92, 0xAC, 0x93, 0xD6, 0x27, 0x9C},
    {0xCC, 0xAA, 0x77, 0xEB, 0x8C, 0xE6, 0xE4},
};

const unsigned char DefaultMvLongProbs[2][8] = {
    {0xF7, 0xD2, 0x87, 0x44, 0x8A, 0xDC, 0xEF, 0xF6},
    {0xF4, 0xB8, 0xC9, 0x2C, 0xAD, 0xDD, 0xEF, 0xFD},
};

void VP6_ConfigureMvEntropyDecoder(PB_INSTANCE *pbi, unsigned char FrameType) {
    int i;

    for (i = 0; i < 2; i++) {
        if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][0])) {
            pbi->MvIsShort[i] = (VP6_bitread(&pbi->br, 7) << 1);
            if (pbi->MvIsShort[i] == 0) {
                pbi->MvIsShort[i] = 1;
            }
        }
        if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][1])) {
            pbi->MvSignProbs[i] = (VP6_bitread(&pbi->br, 7) << 1);
            if (pbi->MvSignProbs[i] == 0) {
                pbi->MvSignProbs[i] = 1;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned int MvUpdateProbsOffset = 2;

        for (j = 0; j < 7; j++) {
            if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset])) {
                pbi->MvShortProbs[i][j] = (VP6_bitread(&pbi->br, 7) << 1);
                if (pbi->MvShortProbs[i][j] == 0) {
                    pbi->MvShortProbs[i][j] = 1;
                }
            }
            MvUpdateProbsOffset++;
        }
    }

    for (i = 0; i < 2; i++) {
        unsigned int j;
        unsigned int MvUpdateProbsOffset = 2 + 7;

        for (j = 0; j < 8; j++) {
            if (VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset])) {
                pbi->MvLongProbs[i][j] = (VP6_bitread(&pbi->br, 7) << 1);
                if (pbi->MvLongProbs[i][j] == 0) {
                    pbi->MvLongProbs[i][j] = 1;
                }
            }
            MvUpdateProbsOffset++;
        }
    }
}
