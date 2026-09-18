typedef int INT32;
typedef unsigned int UINT32;
typedef short Q_LIST_ENTRY;

typedef struct {
    UINT32 FrameQIndex;
    UINT32 ThisFrameQuantizerValue;
    Q_LIST_ENTRY round[8];
    Q_LIST_ENTRY mult[8];
    Q_LIST_ENTRY zbin[8];
    UINT32 LastQuantizerValue;
    UINT32 QThreshTable[64];
    UINT32 *transIndex;
    unsigned char quant_index[64];
    Q_LIST_ENTRY *dequant_coeffs[2];
    Q_LIST_ENTRY *dequant_coeffsAlloc[2];
    INT32 QuantCoeffs[64][2];
    INT32 QuantRound[64][2];
    INT32 ZeroBinSize[64][2];
    INT32 ZlrZbinCorrections[64][2];
} QUANTIZER;

extern void *memset(void *, int, unsigned int);
extern void *memcpy(void *, const void *, unsigned int);
extern void *duck_malloc(unsigned long size, int type);
extern void duck_free(void *mem);

static const UINT32 VP6_QThreshTable[64] = {
    94, 92, 90, 88, 86, 82, 78, 74, 70, 66, 62, 58, 54, 53, 52, 51,
    50, 49, 48, 47, 46, 45, 44, 43, 42, 40, 39, 37, 36, 35, 34, 33,
    32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
    16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,
};

static const UINT32 VP6_UvQThreshTable[64] = {
    94, 92, 90, 88, 86, 82, 78, 74, 70, 66, 62, 58, 54, 53, 52, 51,
    50, 49, 48, 47, 46, 45, 44, 43, 42, 40, 39, 37, 36, 35, 34, 33,
    32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
    16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,
};

const Q_LIST_ENTRY VP6_DcQuant[64] = {
    47, 47, 47, 47, 45, 43, 43, 43, 43, 43, 42, 41, 41, 40, 40, 40,
    40, 35, 35, 35, 35, 33, 33, 33, 33, 32, 32, 32, 27, 27, 26, 26,
    25, 25, 24, 24, 23, 23, 19, 19, 19, 19, 18, 18, 17, 16, 16, 16,
    16, 16, 15, 11, 11, 11, 10, 10, 9,  8,  7,  5,  3,  3,  2,  2,
};

static const Q_LIST_ENTRY VP6_UvDcQuant[64] = {
    47, 47, 47, 47, 45, 43, 43, 43, 43, 43, 42, 41, 41, 40, 40, 40,
    40, 35, 35, 35, 35, 33, 33, 33, 33, 32, 32, 32, 27, 27, 26, 26,
    25, 25, 24, 24, 23, 23, 19, 19, 19, 19, 18, 18, 17, 16, 16, 16,
    16, 16, 15, 11, 11, 11, 10, 10, 9,  8,  7,  5,  3,  3,  2,  2,
};

static const UINT32 dequant_index[64] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
};

static const UINT32 transIndexC[64] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
};

void (*VP6_BuildQuantIndex)(QUANTIZER *);

void VP6_InitQTables(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    memcpy(qi->QThreshTable, VP6_QThreshTable, sizeof(qi->QThreshTable));
}

void VP6_BuildQuantIndex_Generic(QUANTIZER *qi) {
    INT32 i, j;

    qi->transIndex = (UINT32 *)transIndexC;

    for (i = 0; i < 64; i++) {
        j = dequant_index[i];
        qi->quant_index[j] = i;
    }
}

void VP6_init_dequantizer(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    INT32 i, j;

    for (i = 1; i < 64; i++) {
        j = qi->quant_index[i];
        qi->dequant_coeffs[0][j] = (unsigned short)VP6_QThreshTable[qi->FrameQIndex] * 4;
    }
    qi->dequant_coeffs[0][0] = (unsigned short)VP6_DcQuant[qi->FrameQIndex] * 4;

    for (i = 1; i < 64; i++) {
        j = qi->quant_index[i];
        qi->dequant_coeffs[1][j] = (unsigned short)VP6_UvQThreshTable[qi->FrameQIndex] * 4;
    }
    qi->dequant_coeffs[1][0] = (unsigned short)VP6_UvDcQuant[qi->FrameQIndex] * 4;
}

void VP6_UpdateQ(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    if (qi->QThreshTable[qi->FrameQIndex] != qi->LastQuantizerValue) {
        qi->LastQuantizerValue = qi->ThisFrameQuantizerValue;
        VP6_BuildQuantIndex(qi);
        VP6_init_dequantizer(qi, Vp3VersionNo);
    }
}

static void DeleteQuantizerBuffers(QUANTIZER *qi) {
    if (qi->dequant_coeffsAlloc[0])
        duck_free(qi->dequant_coeffsAlloc[0]);
    qi->dequant_coeffsAlloc[0] = 0;
    qi->dequant_coeffs[0] = 0;

    if (qi->dequant_coeffsAlloc[1])
        duck_free(qi->dequant_coeffsAlloc[1]);
    qi->dequant_coeffsAlloc[1] = 0;
    qi->dequant_coeffs[1] = 0;
}

static INT32 AllocateQuantizerBuffers(QUANTIZER *qi) {
    DeleteQuantizerBuffers(qi);

    qi->dequant_coeffsAlloc[0] = duck_malloc(64 * sizeof(Q_LIST_ENTRY) + 32, 0);
    if (qi->dequant_coeffsAlloc[0] == 0)
        goto fail;
    qi->dequant_coeffs[0] = (Q_LIST_ENTRY *)(((UINT32)qi->dequant_coeffsAlloc[0] + 31) & ~31);

    qi->dequant_coeffsAlloc[1] = duck_malloc(64 * sizeof(Q_LIST_ENTRY) + 32, 0);
    if (qi->dequant_coeffsAlloc[1] == 0)
        goto fail;
    qi->dequant_coeffs[1] = (Q_LIST_ENTRY *)(((UINT32)qi->dequant_coeffsAlloc[1] + 31) & ~31);

    return 1;

fail:
    DeleteQuantizerBuffers(qi);
    return 0;
}

void VP6_DeleteQuantizer(QUANTIZER **qi) {
    if (*qi) {
        DeleteQuantizerBuffers(*qi);
        duck_free(*qi);
        *qi = 0;
    }
}

QUANTIZER *VP6_CreateQuantizer(void) {
    QUANTIZER *qi = 0;
    INT32 quantizer_size = sizeof(QUANTIZER);

    qi = duck_malloc(quantizer_size, 0);
    if (!qi)
        return 0;

    memset(qi, 0, quantizer_size);

    if (!AllocateQuantizerBuffers(qi))
        VP6_DeleteQuantizer(&qi);

    return qi;
}
