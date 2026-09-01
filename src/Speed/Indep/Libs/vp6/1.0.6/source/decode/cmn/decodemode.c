#include "../../../include/vp6_pbdll.h"

extern int VP6_DecodeBool(void *br, int probability);
extern int VP6_DecodeBool128(void *br);
extern unsigned int VP6_bitread(void *br, int bits);
extern unsigned char VP6_ModeVq[3][16][20];

int VP6_decodeModeDiff(struct PB_INSTANCE *pbi) {
    int sign;

    sign = VP6_DecodeBool(&pbi->br, 205);
    if (!sign) {
        return 0;
    }

    {
        int diff;

        diff = VP6_DecodeBool128(&pbi->br) * -2 + 1;
        if (!VP6_DecodeBool(&pbi->br, 171)) {
            return diff << (3 - VP6_DecodeBool(&pbi->br, 83));
        }
        if (!VP6_DecodeBool(&pbi->br, 199)) {
            if (VP6_DecodeBool(&pbi->br, 140)) {
                return diff * 12;
            }
            if (VP6_DecodeBool(&pbi->br, 125)) {
                return diff * 16;
            }
            if (VP6_DecodeBool(&pbi->br, 104)) {
                return diff * 20;
            }
            return diff * 24;
        }
        return diff * VP6_bitread(&pbi->br, 7) << 2;
    }

    return 0;
}

void VP6_DecodeModeProbs(struct PB_INSTANCE *pbi) {
    int i;
    int j;

    j = 0;
    while (j <= 2) {
        if (VP6_DecodeBool(&pbi->br, 174)) {
            int whichVector;

            whichVector = VP6_bitread(&pbi->br, 4);
            for (i = 0; i < 10; i++) {
                unsigned char *prob0;
                unsigned char *prob1;
                unsigned char *mode;

                mode = &VP6_ModeVq[j][whichVector][i * 2];
                prob0 = (unsigned char *)pbi + 0x730 + j * 20 + i;
                prob1 = (unsigned char *)pbi + 0x73a + j * 20 + i;
                *prob1 = mode[0];
                *prob0 = mode[1];
            }
        }
        if (VP6_DecodeBool(&pbi->br, 254)) {
            for (i = 0; i < 10; i++) {
                int diff;
                int value;
                unsigned char *prob0;
                unsigned char *prob1;

                prob0 = (unsigned char *)pbi + 0x73a + j * 20 + i;
                prob1 = (unsigned char *)pbi + 0x730 + j * 20 + i;
                diff = VP6_decodeModeDiff(pbi);
                value = diff + *prob0;
                if (value < 0) {
                    value = 0;
                } else if (value > 255) {
                    value = 255;
                }
                *prob0 = value;
                diff = VP6_decodeModeDiff(pbi);
                value = diff + *prob1;
                if (value < 0) {
                    value = 0;
                } else if (value > 255) {
                    value = 255;
                }
                *prob1 = value;
            }
        }
        j++;
    }
}

void VP6_BuildModeTree(struct PB_INSTANCE *pbi) {
    int i;
    int j;
    int k;

    unsigned int Counts[10];
    unsigned int total;

    for (k = 0; k < 10; k++) {
        Counts[k] = 0;
        for (i = 0; i < 10; i++) {
            Counts[k] += pbi->probModeSame[k][i] * 100;
        }
    }
    total = 0;
    for (j = 0; j < 10; j++) {
        total += Counts[j];
    }
    if (total != 0) {
        for (i = 0; i < 10; i++) {
            pbi->MBModeProb[i] = (unsigned char)((Counts[i] * 256) / total);
        }
    }
}
