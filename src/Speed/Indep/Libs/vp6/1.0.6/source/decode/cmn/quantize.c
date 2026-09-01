#include "../../../include/vp6_pbdll.h"

typedef struct {
    unsigned int FrameQIndex;
    unsigned int ThisFrameQuantizerValue;
    short round[8];
    short mult[8];
    short zbin[8];
    unsigned int LastQuantizerValue;
    unsigned int QThreshTable[64];
    unsigned int *transIndex;
    unsigned char quant_index[64];
    short *dequant_coeffs[2];
    short *dequant_coeffsAlloc[2];
    int QuantCoeffs[64][2];
    int QuantRound[64][2];
    int ZeroBinSize[64][2];
    int ZlrZbinCorrections[64][2];
} QUANTIZER;

extern unsigned int VP6_QThreshTable[64];
extern unsigned int VP6_UvQThreshTable[64];
extern short VP6_DcQuant[64];
extern short VP6_UvDcQuant[64];
extern unsigned int dequant_index[64];
extern unsigned int transIndexC[64];
extern void (*VP6_BuildQuantIndex)(QUANTIZER *qi);
extern void VP6_init_dequantizer(QUANTIZER *qi, unsigned char Vp3VersionNo);
extern void *duck_malloc(unsigned int blocksize, int type);
extern void duck_free(void *block);
extern void *memset(void *dest, int value, unsigned int size);

void VP6_InitQTables(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    int i;

    for (i = 0; i < 64; i++) {
        qi->QThreshTable[i] = VP6_QThreshTable[i];
    }
}

void VP6_BuildQuantIndex_Generic(QUANTIZER *qi) {
    int i;
    int j;

    qi->transIndex = transIndexC;
    for (i = 0; i < 64; i++) {
        j = dequant_index[i];
        qi->quant_index[j] = i;
    }
}

void VP6_init_dequantizer(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    int i;
    int j;

    for (i = 1; i < 64; i++) {
        j = qi->quant_index[i];
        qi->dequant_coeffs[0][j] = VP6_QThreshTable[qi->FrameQIndex] * 4;
    }
    qi->dequant_coeffs[0][0] = VP6_DcQuant[qi->FrameQIndex] * 4;

    for (i = 1; i < 64; i++) {
        j = qi->quant_index[i];
        qi->dequant_coeffs[1][j] = VP6_UvQThreshTable[qi->FrameQIndex] * 4;
    }
    qi->dequant_coeffs[1][0] = VP6_UvDcQuant[qi->FrameQIndex] * 4;
}

void VP6_UpdateQ(QUANTIZER *qi, unsigned char Vp3VersionNo) {
    if (qi->QThreshTable[qi->FrameQIndex] != qi->LastQuantizerValue) {
        qi->LastQuantizerValue = qi->ThisFrameQuantizerValue;
        VP6_BuildQuantIndex(qi);
        VP6_init_dequantizer(qi, Vp3VersionNo);
    }
}

static void DeleteQuantizerBuffers(QUANTIZER *qi) {
    if (qi->dequant_coeffsAlloc[0] != 0) {
        duck_free(qi->dequant_coeffsAlloc[0]);
    }
    if (qi->dequant_coeffsAlloc[1] != 0) {
        duck_free(qi->dequant_coeffsAlloc[1]);
    }

    qi->dequant_coeffsAlloc[0] = 0;
    qi->dequant_coeffs[0] = 0;
    qi->dequant_coeffs[1] = 0;
    qi->dequant_coeffsAlloc[1] = 0;
}

static int AllocateQuantizerBuffers(QUANTIZER *qi) {
    DeleteQuantizerBuffers(qi);

    qi->dequant_coeffsAlloc[0] = (short *)duck_malloc(0xa0, 0);
    if (qi->dequant_coeffsAlloc[0] == 0) {
        DeleteQuantizerBuffers(qi);
        return 0;
    }
    qi->dequant_coeffs[0] = (short *)(((unsigned int)qi->dequant_coeffsAlloc[0] + 0x1f) & ~0x1f);

    qi->dequant_coeffsAlloc[1] = (short *)duck_malloc(0xa0, 0);
    if (qi->dequant_coeffsAlloc[1] == 0) {
        DeleteQuantizerBuffers(qi);
        return 0;
    }
    qi->dequant_coeffs[1] = (short *)(((unsigned int)qi->dequant_coeffsAlloc[1] + 0x1f) & ~0x1f);
    return 1;
}

void VP6_DeleteQuantizer(QUANTIZER **qi) {
    if (*qi != 0) {
        DeleteQuantizerBuffers(*qi);
        duck_free(*qi);
        *qi = 0;
    }
}

QUANTIZER *VP6_CreateQuantizer(void) {
    QUANTIZER *qi;
    int quantizer_size;

    quantizer_size = sizeof(QUANTIZER);
    qi = (QUANTIZER *)duck_malloc(quantizer_size, 0);
    if (qi != 0) {
        memset(qi, 0, quantizer_size);
        if (AllocateQuantizerBuffers(qi) == 0) {
            VP6_DeleteQuantizer(&qi);
        }
    }
    return qi;
}
