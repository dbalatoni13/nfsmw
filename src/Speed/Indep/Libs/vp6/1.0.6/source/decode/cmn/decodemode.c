#include "../../../include/vp6_pbdll.h"

#define MAX_MODES 10
#define MODETYPES 3
#define MODEVECTORS 16
#define PROBVECTORXMIT 174
#define PROBIDEALXMIT 254

extern int VP6_DecodeBool(BOOL_CODER *br, int probability);
extern int VP6_DecodeBool128(BOOL_CODER *br);
extern unsigned int VP6_bitread(BOOL_CODER *br, int bits);
const unsigned char VP6_ModeVq[MODETYPES][16][MAX_MODES * 2] = {
    9, 15, 32, 25, 7, 19, 9, 21, 1, 12, 14, 12, 3, 18, 14, 23, 3, 10, 0, 4,
    48, 39, 1, 2, 11, 27, 29, 44, 7, 27, 1, 4, 0, 3, 1, 6, 1, 2, 0, 0,
    21, 32, 1, 2, 4, 10, 32, 43, 6, 23, 2, 3, 1, 19, 1, 6, 12, 21, 0, 7,
    69, 83, 0, 0, 0, 2, 10, 29, 3, 12, 0, 1, 0, 3, 0, 3, 2, 2, 0, 0,
    11, 20, 1, 4, 18, 36, 43, 48, 13, 35, 0, 2, 0, 5, 3, 12, 1, 2, 0, 0,
    70, 44, 0, 1, 2, 10, 37, 46, 8, 26, 0, 2, 0, 2, 0, 2, 0, 1, 0, 0,
    8, 15, 0, 1, 8, 21, 74, 53, 22, 42, 0, 1, 0, 2, 0, 3, 1, 2, 0, 0,
    141, 42, 0, 0, 1, 4, 11, 24, 1, 11, 0, 1, 0, 1, 0, 2, 0, 0, 0, 0,
    8, 19, 4, 10, 24, 45, 21, 37, 9, 29, 0, 3, 1, 7, 11, 25, 0, 2, 0, 1,
    46, 42, 0, 1, 2, 10, 54, 51, 10, 30, 0, 2, 0, 2, 0, 1, 0, 1, 0, 0,
    28, 32, 0, 0, 3, 10, 75, 51, 14, 33, 0, 1, 0, 2, 0, 1, 1, 2, 0, 0,
    100, 46, 0, 1, 3, 9, 21, 37, 5, 20, 0, 1, 0, 2, 1, 2, 0, 1, 0, 0,
    27, 29, 0, 1, 9, 25, 53, 51, 12, 34, 0, 1, 0, 3, 1, 5, 0, 2, 0, 0,
    80, 38, 0, 0, 1, 4, 69, 33, 5, 16, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
    16, 20, 0, 0, 2, 8, 104, 49, 15, 33, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0,
    194, 16, 0, 0, 1, 1, 1, 9, 1, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,

    41, 22, 1, 0, 1, 31, 0, 0, 0, 0, 0, 1, 1, 7, 0, 1, 98, 25, 4, 10,
    123, 37, 6, 4, 1, 27, 0, 0, 0, 0, 5, 8, 1, 7, 0, 1, 12, 10, 0, 2,
    26, 14, 14, 12, 0, 24, 0, 0, 0, 0, 55, 17, 1, 9, 0, 36, 5, 7, 1, 3,
    209, 5, 0, 0, 0, 27, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0,
    2, 5, 4, 5, 0, 121, 0, 0, 0, 0, 0, 3, 2, 4, 1, 4, 2, 2, 0, 1,
    175, 5, 0, 1, 0, 48, 0, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 1, 0, 0,
    83, 5, 2, 3, 0, 102, 0, 0, 0, 0, 1, 3, 0, 2, 0, 1, 0, 0, 0, 0,
    233, 6, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
    34, 16, 112, 21, 1, 28, 0, 0, 0, 0, 6, 8, 1, 7, 0, 3, 2, 5, 0, 2,
    159, 35, 2, 2, 0, 25, 0, 0, 0, 0, 3, 6, 0, 5, 0, 1, 4, 4, 0, 1,
    75, 39, 5, 7, 2, 48, 0, 0, 0, 0, 3, 11, 2, 16, 1, 4, 7, 10, 0, 2,
    212, 21, 0, 1, 0, 9, 0, 0, 0, 0, 1, 2, 0, 2, 0, 0, 2, 2, 0, 0,
    4, 2, 0, 0, 0, 172, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 0, 0,
    187, 22, 1, 1, 0, 17, 0, 0, 0, 0, 3, 6, 0, 4, 0, 1, 4, 4, 0, 1,
    133, 6, 1, 2, 1, 70, 0, 0, 0, 0, 0, 2, 0, 4, 0, 3, 1, 1, 0, 0,
    251, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    2, 3, 2, 3, 0, 2, 0, 2, 0, 0, 11, 4, 1, 4, 0, 2, 3, 2, 0, 4,
    49, 46, 3, 4, 7, 31, 42, 41, 0, 0, 2, 6, 1, 7, 1, 4, 2, 4, 0, 1,
    26, 25, 1, 1, 2, 10, 67, 39, 0, 0, 1, 1, 0, 14, 0, 2, 31, 26, 1, 6,
    103, 46, 1, 2, 2, 10, 33, 42, 0, 0, 1, 4, 0, 3, 0, 1, 1, 3, 0, 0,
    14, 31, 9, 13, 14, 54, 22, 29, 0, 0, 2, 6, 4, 18, 6, 13, 1, 5, 0, 1,
    85, 39, 0, 0, 1, 9, 69, 40, 0, 0, 0, 1, 0, 3, 0, 1, 2, 3, 0, 0,
    31, 28, 0, 0, 3, 14, 130, 34, 0, 0, 0, 1, 0, 3, 0, 1, 3, 3, 0, 1,
    171, 25, 0, 0, 1, 5, 25, 21, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
    17, 21, 68, 29, 6, 15, 13, 22, 0, 0, 6, 12, 3, 14, 4, 10, 1, 7, 0, 3,
    51, 39, 0, 1, 2, 12, 91, 44, 0, 0, 0, 2, 0, 3, 0, 1, 2, 3, 0, 1,
    81, 25, 0, 0, 2, 9, 106, 26, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0,
    140, 37, 0, 1, 1, 8, 24, 33, 0, 0, 1, 2, 0, 2, 0, 1, 1, 2, 0, 0,
    14, 23, 1, 3, 11, 53, 90, 31, 0, 0, 0, 3, 1, 5, 2, 6, 1, 2, 0, 0,
    123, 29, 0, 0, 1, 7, 57, 30, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
    13, 14, 0, 0, 4, 20, 175, 20, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0,
    202, 23, 0, 0, 1, 3, 2, 9, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0
};

const unsigned char VP6_BaselineXmittedProbs[4][2][MAX_MODES] = {
    42, 2, 7, 42, 22, 3, 2, 5, 1, 0, 69, 1, 1, 44, 6, 1, 0, 1, 0, 0,
    8, 1, 8, 0, 0, 2, 1, 0, 1, 0, 229, 1, 0, 0, 0, 1, 0, 0, 1, 0,
    35, 1, 6, 34, 0, 2, 1, 1, 1, 0, 122, 1, 1, 46, 0, 1, 0, 0, 1, 0,
    64, 0, 64, 64, 64, 0, 0, 0, 0, 0, 64, 0, 64, 64, 64, 0, 0, 0, 0, 0
};

void VP6_BuildModeTree(struct PB_INSTANCE *pbi) {
    int i, j, k;

    for (i = 0; i < MAX_MODES; i++) {
        unsigned int Counts[MAX_MODES], total;

        for (k = 0; k < MODETYPES; k++) {
            total = 0;
            for (j = 0; j < MAX_MODES; j++) {
                if (i == j) {
                    Counts[j] = 0;
                } else {
                    Counts[j] = 100 * pbi->probXmitted[k][0][j];
                }

                total += Counts[j];
            }

            pbi->probModeSame[k][i] =
                255 - 255 * pbi->probXmitted[k][1][i] /
                    (1 + pbi->probXmitted[k][1][i] + pbi->probXmitted[k][0][i]);

            pbi->probMode[k][i][0] =
                1 + 255 * (Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV] +
                           Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]) /
                        (1 + total);
            pbi->probMode[k][i][1] =
                1 + 255 * (Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV]) /
                        (1 + Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV] +
                         Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]);
            pbi->probMode[k][i][2] =
                1 + 255 * (Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV]) /
                        (1 + Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV] +
                         Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV] +
                         Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
            pbi->probMode[k][i][3] =
                1 + 255 * Counts[CODE_INTER_NO_MV] /
                        (1 + Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV]);
            pbi->probMode[k][i][4] =
                1 + 255 * Counts[CODE_INTER_NEAREST_MV] /
                        (1 + Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]);
            pbi->probMode[k][i][5] =
                1 + 255 * Counts[CODE_INTRA] /
                        (1 + Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV]);
            pbi->probMode[k][i][6] =
                1 + 255 * (Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV]) /
                        (1 + Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV] +
                         Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
            pbi->probMode[k][i][7] =
                1 + 255 * Counts[CODE_USING_GOLDEN] /
                        (1 + Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV]);
            pbi->probMode[k][i][8] =
                1 + 255 * Counts[CODE_GOLD_NEAREST_MV] /
                        (1 + Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
        }

    }
}

int VP6_decodeModeDiff(struct PB_INSTANCE *pbi) {
    int sign;

    if (VP6_DecodeBool(&pbi->br, 205) == 0) {
        return 0;
    }

    sign = 1 + -2 * VP6_DecodeBool128(&pbi->br);

    if (!VP6_DecodeBool(&pbi->br, 171)) {
        return sign << (3 - VP6_DecodeBool(&pbi->br, 83));
    } else {
        if (!VP6_DecodeBool(&pbi->br, 199)) {
            if (VP6_DecodeBool(&pbi->br, 140)) {
                return sign * 12;
            }
            if (VP6_DecodeBool(&pbi->br, 125)) {
                return sign * 16;
            }
            if (VP6_DecodeBool(&pbi->br, 104)) {
                return sign * 20;
            }
            return sign * 24;
        } else {
            int diff = VP6_bitread(&pbi->br, 7);

            return sign * diff * 4;
        }
    }
}

void VP6_DecodeModeProbs(struct PB_INSTANCE *pbi) {
    int i, j;

    for (j = 0; j < MODETYPES; j++) {
        if (VP6_DecodeBool(&pbi->br, PROBVECTORXMIT)) {
            int whichVector = VP6_bitread(&pbi->br, 4);

            for (i = 0; i < MAX_MODES; i++) {
                pbi->probXmitted[j][1][i] = VP6_ModeVq[j][whichVector][i * 2];
                pbi->probXmitted[j][0][i] = VP6_ModeVq[j][whichVector][i * 2 + 1];
            }
        }
        if (VP6_DecodeBool(&pbi->br, PROBIDEALXMIT)) {
            for (i = 0; i < MAX_MODES; i++) {
                int diff;

                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][1][i];
                pbi->probXmitted[j][1][i] = (diff < 0 ? 0 : (diff > 255 ? 255 : diff));

                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][0][i];
                pbi->probXmitted[j][0][i] = (diff < 0 ? 0 : (diff > 255 ? 255 : diff));
            }
        }
    }

    VP6_BuildModeTree(pbi);
}
