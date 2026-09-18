typedef struct {
    unsigned int lowvalue;
    unsigned int range;
    unsigned int value;
    int count;
    unsigned int pos;
    unsigned char *buffer;
    unsigned int MeasureCost;
    unsigned int BitCounter;
} BOOL_CODER;

typedef struct {
    unsigned int value : 7;
    unsigned int selector : 1;
} tokenorptr;

typedef struct {
    union {
        char l;
        tokenorptr left;
    } leftunion;
    union {
        char r;
        tokenorptr right;
    } rightunion;
    unsigned char freq;
} HUFF_NODE;

typedef struct {
    unsigned char Token;
    int Mode;
    unsigned short Frame;
    short Dc;
    unsigned char unused[3];
} BLOCK_CONTEXT;

typedef struct {
    unsigned char pad0[0x13C];
    unsigned int *transIndex;
} QUANTIZER;

typedef struct {
    unsigned short MinVal;
    short Length;
    unsigned char Probs[11];
} TOKENEXTRABITS;

typedef struct {
    unsigned char pad000[0x004];
    short (*Coeffs)[64];                        /* 0x004 */
    unsigned char pad008[0x0CC - 0x008];
    BLOCK_CONTEXT LeftY[2];                     /* 0x0CC */
    BLOCK_CONTEXT LeftU;                        /* 0x0EC */
    BLOCK_CONTEXT LeftV;                        /* 0x0FC */
    BLOCK_CONTEXT *AboveY;                      /* 0x10C */
    BLOCK_CONTEXT *AboveU;                      /* 0x110 */
    BLOCK_CONTEXT *AboveV;                      /* 0x114 */
    BLOCK_CONTEXT *AboveYAlloc;                 /* 0x118 */
    BLOCK_CONTEXT *AboveUAlloc;                 /* 0x11C */
    BLOCK_CONTEXT *AboveVAlloc;                 /* 0x120 */
    short LastDcY[4];                           /* 0x124 */
    short LastDcU[4];                           /* 0x12C */
    short LastDcV[4];                           /* 0x134 */
    QUANTIZER *quantizer;                       /* 0x13C */
    unsigned char pad140[0x150 - 0x140];
    BOOL_CODER br;                              /* 0x150 */
    BOOL_CODER br2;                             /* 0x170 */
    unsigned char pad190[0x19C - 0x190];
    unsigned char Vp3VersionNo;                 /* 0x19C */
    unsigned char VpProfile;                    /* 0x19D */
    unsigned char pad19E[0x1AC - 0x19E];
    unsigned char FrameType;                    /* 0x1AC */
    unsigned char pad1AD[0x1DC - 0x1AD];
    unsigned int Interlaced;                    /* 0x1DC */
    unsigned char pad1E0[0x1F8 - 0x1E0];
    unsigned int HFragments;                    /* 0x1F8 */
    unsigned char pad1FC[0x228 - 0x1FC];
    unsigned int MacroBlocks;                   /* 0x228 */
    unsigned int MBRows;                        /* 0x22C */
    unsigned int MBCols;                        /* 0x230 */
    unsigned char pad234[0x3A0 - 0x234];
    int LastMode;                               /* 0x3A0 */
    unsigned char DcProbs[2][11];               /* 0x3A4 */
    unsigned char AcProbs[396];                 /* 0x3BA */
    unsigned char DcNodeContexts[2][3][5];      /* 0x546 */
    unsigned char ZeroRunProbs[2][14];          /* 0x564 */
    unsigned char MergedScanOrder[64];          /* 0x580 */
    unsigned char ModifiedScanOrder[64];        /* 0x5C0 */
    unsigned char EobOffsetTable[64];           /* 0x600 */
    unsigned char ScanBands[64];                /* 0x640 */
    unsigned char MBModeProb[11];               /* 0x680 */
    unsigned char BModeProb[11];                /* 0x68B */
    unsigned char pad696[0x6EC - 0x696];
    int probInterlaced;                         /* 0x6EC */
    unsigned char pad6F0[0x6F4 - 0x6F0];
    char *predictionMode;                       /* 0x6F4 */
    unsigned char pad6F8[0x708 - 0x6F8];
    unsigned char MvSignProbs[2];               /* 0x708 */
    unsigned char IsMvShortProb[2];             /* 0x70A */
    unsigned char MvShortProbs[2][7];           /* 0x70C */
    unsigned char pad71A[0x720 - 0x71A];
    unsigned char MvSizeProbs[2][8];            /* 0x720 */
    unsigned char probXmitted[10][2][4];        /* 0x730 */
    unsigned char pad780[0x948 - 0x780];
    int MultiStream;                            /* 0x948 */
    unsigned int DcHuffCode[2][12];             /* 0x94C */
    unsigned char DcHuffLength[2][12];          /* 0x9AC */
    unsigned int DcHuffProbs[2][12];            /* 0x9C4 */
    HUFF_NODE DcHuffTree[2][12];                /* 0xA24 */
    unsigned int AcHuffCode[3][2][6][12];       /* 0xB44 */
    unsigned char AcHuffLength[3][2][6][12];    /* 0x1204 */
    unsigned int AcHuffProbs[3][2][6][12];      /* 0x13B4 */
    HUFF_NODE AcHuffTree[3][2][6][12];          /* 0x1A74 */
    unsigned int ZeroHuffCode[2][14];           /* 0x2EB4 */
    unsigned char ZeroHuffLength[2][14];        /* 0x2F24 */
    unsigned int ZeroHuffProbs[2][14];          /* 0x2F40 */
    HUFF_NODE ZeroHuffTree[2][14];              /* 0x2FB0 */
    unsigned short DcHuffLUT[2][64];            /* 0x3100 */
    unsigned short AcHuffLUT[3][2][6][64];      /* 0x3200 */
    unsigned short ZeroHuffLUT[2][64];          /* 0x4400 */
    unsigned char pad4500[0x4524 - 0x4500];
    int UseHuffman;                             /* 0x4524 */
    int CurrentDcRunLen[2];                     /* 0x4528 */
    int CurrentAc1RunLen[2];                    /* 0x4530 */
} PB_INSTANCE;

extern int nDecodeBool(BOOL_CODER *br, int probability);
extern int VP6_bitread(BOOL_CODER *br, int bits);
extern void VP6_BuildHuffTree(HUFF_NODE *tree, unsigned int *probs, int entries);
extern void VP6_BuildHuffLookupTable(HUFF_NODE *tree, unsigned short *lut);
extern void VP6_CreateCodeArray(HUFF_NODE *tree, int node, unsigned int *codes,
                                unsigned char *lengths, int code, int length);
extern void VP6_ConfigureContexts(PB_INSTANCE *pbi);
extern void VP6_DecodeModeProbs(PB_INSTANCE *pbi);
extern void VP6_ConfigureMvEntropyDecoder(PB_INSTANCE *pbi, unsigned char FrameType);
extern void VP6_DecodeMacroBlock(PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol);

extern const unsigned char VP6_DcUpdateProbs[2][11];
extern const unsigned char VP6_AcUpdateProbs[3][2][6][11];
extern const unsigned char ScanBandUpdateProbs[64];
extern const unsigned char ZrlUpdateProbs[2][14];
extern const unsigned char ZeroRunProbDefaults[2][14];
extern const unsigned char VP6_BaselineXmittedProbs[10][2][4];
extern const unsigned char DefaultMvShortProbs[2][7];
extern const unsigned char DefaultMvLongProbs[2][8];
extern const unsigned char DefaultIsShortProbs[2];
extern const unsigned char DefaultSignProbs[2];
extern const unsigned char DefaultNonInterlacedScanBands[64];
extern const unsigned char DefaultInterlacedScanBands[64];

static const TOKENEXTRABITS VP6_TokenExtraBits2[12] = {
    {0, -1, {0}},
    {1, 0, {0}},
    {2, 0, {0}},
    {3, 0, {0}},
    {4, 0, {0}},
    {5, 0, {0x9F}},
    {7, 1, {0x91, 0xA5}},
    {11, 2, {0x8C, 0x94, 0xAD}},
    {19, 3, {0x87, 0x8C, 0x9B, 0xB0}},
    {35, 4, {0x82, 0x86, 0x8D, 0x9D, 0xB4}},
    {67, 10, {0x81, 0x82, 0x85, 0x8C, 0x99, 0xB1, 0xC4, 0xE6, 0xF3, 0xFE, 0xFE}},
    {0, -1, {0}},
};

const int VP6_CoeffToBand[65] = {
    -1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    7,
};

static inline int nDecodeBool128(BOOL_CODER *br) {
    unsigned int bit;
    unsigned int split;
    unsigned int bigsplit;
    unsigned int count;
    unsigned int range;
    unsigned int value;

    range = br->range;
    value = br->value;
    count = br->count;
    split = (range + 1) >> 1;
    bigsplit = split << 24;

    bit = (value >= bigsplit);
    range = bit ? range - split : split;
    if (bit) {
        value = value - bigsplit;
    }

    value = value + value;
    range = range + range;
    count--;
    if (!count) {
        count = 8;
        value = value | br->buffer[br->pos];
        br->pos++;
    }

    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
}

void BuildScanOrder(PB_INSTANCE *pbi, unsigned char *ScanBands) {
    unsigned int i;
    unsigned int j;
    unsigned int ScanOrderIndex;
    unsigned int MaxOffset;

    pbi->ModifiedScanOrder[0] = 0;
    ScanOrderIndex = 1;

    for (i = 0; i <= 15; i++) {
        for (j = 1; j <= 63; j++) {
            if (ScanBands[j] == i) {
                pbi->ModifiedScanOrder[ScanOrderIndex] = j;
                ScanOrderIndex++;
            }
        }
    }

    MaxOffset = 0;
    for (i = 0; i <= 63; i++) {
        if (pbi->ModifiedScanOrder[i] > MaxOffset) {
            MaxOffset = pbi->ModifiedScanOrder[i];
        }
        pbi->EobOffsetTable[i] = MaxOffset;
        if (pbi->Vp3VersionNo > 6) {
            pbi->EobOffsetTable[i] = MaxOffset + 1;
        }
    }
}

void BoolTreeToHuffCodes(unsigned char *BoolTreeProbs, unsigned int *HuffProbs) {
    unsigned int Prob;
    unsigned int Prob1;

    HuffProbs[11] = ((unsigned int)BoolTreeProbs[0] * BoolTreeProbs[1]) >> 8;
    HuffProbs[0] = ((unsigned int)BoolTreeProbs[0] * (255 - BoolTreeProbs[1])) >> 8;

    Prob = 255 - BoolTreeProbs[0];
    HuffProbs[1] = (Prob * BoolTreeProbs[2]) >> 8;

    Prob = (Prob * (255 - BoolTreeProbs[2])) >> 8;
    Prob1 = (Prob * BoolTreeProbs[3]) >> 8;
    HuffProbs[2] = (Prob1 * BoolTreeProbs[4]) >> 8;
    Prob1 = (Prob1 * (255 - BoolTreeProbs[4])) >> 8;
    HuffProbs[3] = (Prob1 * BoolTreeProbs[5]) >> 8;
    HuffProbs[4] = (Prob1 * (255 - BoolTreeProbs[5])) >> 8;

    Prob = (Prob * (255 - BoolTreeProbs[3])) >> 8;
    Prob1 = (Prob * BoolTreeProbs[6]) >> 8;
    HuffProbs[5] = (Prob1 * BoolTreeProbs[7]) >> 8;
    HuffProbs[6] = (Prob1 * (255 - BoolTreeProbs[7])) >> 8;

    Prob = (Prob * (255 - BoolTreeProbs[6])) >> 8;
    Prob1 = (Prob * BoolTreeProbs[8]) >> 8;
    HuffProbs[7] = (Prob1 * BoolTreeProbs[9]) >> 8;
    HuffProbs[8] = (Prob1 * (255 - BoolTreeProbs[9])) >> 8;

    Prob = (Prob * (255 - BoolTreeProbs[8])) >> 8;
    HuffProbs[9] = (Prob * BoolTreeProbs[10]) >> 8;
    HuffProbs[10] = (Prob * (255 - BoolTreeProbs[10])) >> 8;
}

void ZerosBoolTreeToHuffCodes(unsigned char *BoolTreeProbs, unsigned int *HuffProbs) {
    unsigned int Prob;

    Prob = ((unsigned int)BoolTreeProbs[0] * BoolTreeProbs[1]) >> 8;
    HuffProbs[0] = (Prob * BoolTreeProbs[2]) >> 8;
    HuffProbs[1] = (Prob * (255 - BoolTreeProbs[2])) >> 8;

    Prob = ((unsigned int)BoolTreeProbs[0] * (255 - BoolTreeProbs[1])) >> 8;
    HuffProbs[2] = (Prob * BoolTreeProbs[3]) >> 8;
    HuffProbs[3] = (Prob * (255 - BoolTreeProbs[3])) >> 8;

    Prob = ((255u - BoolTreeProbs[0]) * BoolTreeProbs[4]) >> 8;
    Prob = (Prob * BoolTreeProbs[5]) >> 8;
    HuffProbs[4] = (Prob * BoolTreeProbs[6]) >> 8;
    HuffProbs[5] = (Prob * (255 - BoolTreeProbs[6])) >> 8;

    Prob = ((255u - BoolTreeProbs[0]) * BoolTreeProbs[4]) >> 8;
    Prob = (Prob * (255 - BoolTreeProbs[5])) >> 8;
    HuffProbs[6] = (Prob * BoolTreeProbs[7]) >> 8;
    HuffProbs[7] = (Prob * (255 - BoolTreeProbs[7])) >> 8;

    Prob = ((255u - BoolTreeProbs[0]) * (255 - BoolTreeProbs[4])) >> 8;
    HuffProbs[8] = Prob;
}

void ConvertBoolTrees(PB_INSTANCE *pbi) {
    unsigned int i;
    unsigned int Plane;
    unsigned int Band;
    int Prec;

    for (Plane = 0; Plane <= 1; Plane++) {
        BoolTreeToHuffCodes(pbi->DcProbs[Plane], pbi->DcHuffProbs[Plane]);
        VP6_BuildHuffTree(pbi->DcHuffTree[Plane], pbi->DcHuffProbs[Plane], 12);
        VP6_BuildHuffLookupTable(pbi->DcHuffTree[Plane], pbi->DcHuffLUT[Plane]);
        VP6_CreateCodeArray(pbi->DcHuffTree[Plane], 0, pbi->DcHuffCode[Plane],
                            pbi->DcHuffLength[Plane], 0, 0);
    }

    for (i = 0; i <= 1; i++) {
        ZerosBoolTreeToHuffCodes(pbi->ZeroRunProbs[i], pbi->ZeroHuffProbs[i]);
        VP6_BuildHuffTree(pbi->ZeroHuffTree[i], pbi->ZeroHuffProbs[i], 9);
        VP6_BuildHuffLookupTable(pbi->ZeroHuffTree[i], pbi->ZeroHuffLUT[i]);
        VP6_CreateCodeArray(pbi->ZeroHuffTree[i], 0, pbi->ZeroHuffCode[i], pbi->ZeroHuffLength[i],
                            0, 0);
    }

    for (Prec = 0; Prec <= 2; Prec++) {
        for (Plane = 0; Plane <= 1; Plane++) {
            for (Band = 0; Band <= 5; Band++) {
                BoolTreeToHuffCodes(&pbi->AcProbs[Plane * 198 + Prec * 66 + Band * 11],
                                    pbi->AcHuffProbs[Prec][Plane][Band]);
                VP6_BuildHuffTree(pbi->AcHuffTree[Prec][Plane][Band],
                                  pbi->AcHuffProbs[Prec][Plane][Band], 12);
                VP6_BuildHuffLookupTable(pbi->AcHuffTree[Prec][Plane][Band],
                                         pbi->AcHuffLUT[Prec][Plane][Band]);
                VP6_CreateCodeArray(pbi->AcHuffTree[Prec][Plane][Band], 0,
                                    pbi->AcHuffCode[Prec][Plane][Band],
                                    pbi->AcHuffLength[Prec][Plane][Band], 0, 0);
            }
        }
    }
}

void VP6_ConfigureEntropyDecoder(PB_INSTANCE *pbi, unsigned char FrameType) {
    unsigned int i;
    unsigned int j;
    unsigned int Plane;
    unsigned int Band;
    int Prec;
    unsigned char LastProb[11];

    memset(LastProb, 128, 11);

    for (Plane = 0; Plane <= 1; Plane++) {
        for (i = 0; i <= 10; i++) {
            if (nDecodeBool(&pbi->br, VP6_DcUpdateProbs[Plane][i])) {
                unsigned char Prob = VP6_bitread(&pbi->br, 7) << 1;
                pbi->DcProbs[Plane][i] = LastProb[i] = Prob + (Prob == 0);
            } else if (FrameType == 0) {
                pbi->DcProbs[Plane][i] = LastProb[i];
            }
        }
    }

    if (FrameType == 0) {
        memcpy(pbi->ZeroRunProbs, ZeroRunProbDefaults, sizeof(pbi->ZeroRunProbs));
    }

    if (nDecodeBool(&pbi->br, 128)) {
        for (i = 1; i <= 63; i++) {
            if (nDecodeBool(&pbi->br, ScanBandUpdateProbs[i])) {
                pbi->ScanBands[i] = VP6_bitread(&pbi->br, 4);
            }
        }
        BuildScanOrder(pbi, pbi->ScanBands);
    }

    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 13; j++) {
            if (nDecodeBool(&pbi->br, ZrlUpdateProbs[i][j])) {
                unsigned char Prob = VP6_bitread(&pbi->br, 7) << 1;
                pbi->ZeroRunProbs[i][j] = Prob + (Prob == 0);
            }
        }
    }

    for (Prec = 0; Prec <= 2; Prec++) {
        for (Plane = 0; Plane <= 1; Plane++) {
            for (Band = 0; Band <= 5; Band++) {
                for (i = 0; i <= 10; i++) {
                    if (nDecodeBool(&pbi->br, VP6_AcUpdateProbs[Prec][Plane][Band][i])) {
                        unsigned char Prob = VP6_bitread(&pbi->br, 7) << 1;
                        pbi->AcProbs[Plane * 198 + Prec * 66 + Band * 11 + i] = LastProb[i] =
                            Prob + (Prob == 0);
                    } else if (FrameType == 0) {
                        pbi->AcProbs[Plane * 198 + Prec * 66 + Band * 11 + i] = LastProb[i];
                    }
                }
            }
        }
    }

    VP6_ConfigureContexts(pbi);
}

void VP6_ResetLeftContext(PB_INSTANCE *pbi) {
    memset(&pbi->LeftY[0], 0, sizeof(BLOCK_CONTEXT));
    memset(&pbi->LeftY[1], 0, sizeof(BLOCK_CONTEXT));
    memset(&pbi->LeftU, 0, sizeof(BLOCK_CONTEXT));
    memset(&pbi->LeftV, 0, sizeof(BLOCK_CONTEXT));

    pbi->LeftY[0].Mode = -1;
    pbi->LeftY[1].Mode = -1;
    pbi->LeftU.Mode = -1;
    pbi->LeftV.Mode = -1;
    pbi->LeftY[0].Frame = 4;
    pbi->LeftY[1].Frame = 4;
    pbi->LeftU.Frame = 4;
    pbi->LeftV.Frame = 4;
}

void VP6_ResetAboveContext(PB_INSTANCE *pbi) {
    unsigned int i;

    for (i = 0; i < pbi->HFragments + 8; i++) {
        pbi->AboveY[i].Mode = -1;
        pbi->AboveY[i].Frame = 4;
        pbi->AboveY[i].Dc = 0;
        pbi->AboveY[i].Token = 0;
    }

    for (i = 0; i < pbi->HFragments / 2 + 8; i++) {
        pbi->AboveU[i].Mode = -1;
        pbi->AboveU[i].Frame = 4;
        pbi->AboveU[i].Token = 0;
        pbi->AboveU[i].Dc = 0;
        pbi->AboveV[i].Mode = -1;
        pbi->AboveV[i].Frame = 4;
        pbi->AboveV[i].Token = 0;
        pbi->AboveV[i].Dc = 0;
    }

    if (pbi->Vp3VersionNo <= 5) {
        pbi->AboveU[1].Mode = 0;
        pbi->AboveU[1].Frame = 0;
        pbi->AboveV[1].Mode = 0;
        pbi->AboveV[1].Frame = 0;
    }

    pbi->LastDcY[0] = 0;
    pbi->LastDcU[0] = 128;
    pbi->LastDcV[0] = 128;

    for (i = 1; i <= 2; i++) {
        pbi->LastDcY[i] = 0;
        pbi->LastDcU[i] = 0;
        pbi->LastDcV[i] = 0;
    }
}

unsigned char VP6_ReadTokensPredictA(PB_INSTANCE *pbi, short *CoeffData, unsigned int Plane,
                                     BLOCK_CONTEXT *Above, BLOCK_CONTEXT *Left) {
    int token;
    BOOL_CODER *br;
    unsigned char EncodedCoeffs;
    unsigned char *BaselineProbsPtr;
    unsigned char *ContextProbsPtr;
    unsigned char PrecTokenIndex;
    int SignBit;
    int BitsCount;
    unsigned char *AcProbsPtr;
    unsigned int *TransIndex;
    int value;
    unsigned int ZeroRunCount;
    unsigned char *ZeroRunProbPtr;
    int ThisTokeNonZero;

    TransIndex = pbi->quantizer->transIndex;
    AcProbsPtr = &pbi->AcProbs[Plane * 198];

    if (pbi->MultiStream || pbi->VpProfile == 0) {
        br = &pbi->br2;
    } else {
        br = &pbi->br;
    }

    BaselineProbsPtr = pbi->DcProbs[Plane];
    ContextProbsPtr = pbi->DcNodeContexts[Plane][Left->Token + Above->Token];

    if (!nDecodeBool(br, ContextProbsPtr[0])) {
        PrecTokenIndex = 0;
        Left->Token = 0;
        Above->Token = 0;
    } else {
        Left->Token = 1;
        Above->Token = 1;
        if (nDecodeBool(br, ContextProbsPtr[2])) {
            PrecTokenIndex = 2;
            if (nDecodeBool(br, ContextProbsPtr[3])) {
                if (nDecodeBool(br, BaselineProbsPtr[6])) {
                    if (nDecodeBool(br, BaselineProbsPtr[8])) {
                        token = nDecodeBool(br, BaselineProbsPtr[10]) + 9;
                    } else {
                        token = nDecodeBool(br, BaselineProbsPtr[9]) + 7;
                    }
                } else {
                    token = nDecodeBool(br, BaselineProbsPtr[7]) + 5;
                }
                value = VP6_TokenExtraBits2[token].MinVal;
                BitsCount = VP6_TokenExtraBits2[token].Length;
                do {
                    value += nDecodeBool(br, VP6_TokenExtraBits2[token].Probs[BitsCount])
                             << BitsCount;
                    BitsCount--;
                } while (BitsCount >= 0);
                SignBit = nDecodeBool128(br);
                CoeffData[0] = (value ^ -SignBit) + SignBit;
            } else {
                if (nDecodeBool(br, ContextProbsPtr[4])) {
                    token = nDecodeBool(br, BaselineProbsPtr[5]) + 3;
                } else {
                    token = 2;
                }
                SignBit = nDecodeBool128(br);
                CoeffData[0] = (token ^ -SignBit) + SignBit;
            }
        } else {
            PrecTokenIndex = 1;
            SignBit = nDecodeBool128(br);
            CoeffData[0] = (1 ^ -SignBit) + SignBit;
        }
    }

    for (EncodedCoeffs = 1; EncodedCoeffs <= 63;) {
        BaselineProbsPtr = &AcProbsPtr[PrecTokenIndex * 66 + VP6_CoeffToBand[EncodedCoeffs] * 11];

        if (EncodedCoeffs > 1 && PrecTokenIndex == 0) {
            ThisTokeNonZero = 1;
        } else {
            ThisTokeNonZero = nDecodeBool(br, BaselineProbsPtr[0]);
        }

        if (!ThisTokeNonZero) {
            if (!nDecodeBool(br, BaselineProbsPtr[1])) {
                EncodedCoeffs++;
                break;
            }

            PrecTokenIndex = 0;
            ZeroRunProbPtr = pbi->ZeroRunProbs[0];
            if (EncodedCoeffs > 5) {
                ZeroRunProbPtr = pbi->ZeroRunProbs[1];
            }

            if (!nDecodeBool(br, ZeroRunProbPtr[0])) {
                if (!nDecodeBool(br, ZeroRunProbPtr[1])) {
                    ZeroRunCount = nDecodeBool(br, ZeroRunProbPtr[2]) + 1;
                } else {
                    ZeroRunCount = nDecodeBool(br, ZeroRunProbPtr[3]) + 3;
                }
            } else if (!nDecodeBool(br, ZeroRunProbPtr[4])) {
                if (!nDecodeBool(br, ZeroRunProbPtr[5])) {
                    ZeroRunCount = nDecodeBool(br, ZeroRunProbPtr[6]) + 5;
                } else {
                    ZeroRunCount = nDecodeBool(br, ZeroRunProbPtr[7]) + 7;
                }
            } else {
                ZeroRunCount = nDecodeBool(br, ZeroRunProbPtr[8]);
                ZeroRunCount += nDecodeBool(br, ZeroRunProbPtr[9]) << 1;
                ZeroRunCount += nDecodeBool(br, ZeroRunProbPtr[10]) << 2;
                ZeroRunCount += nDecodeBool(br, ZeroRunProbPtr[11]) << 3;
                ZeroRunCount += nDecodeBool(br, ZeroRunProbPtr[12]) << 4;
                ZeroRunCount += nDecodeBool(br, ZeroRunProbPtr[13]) << 5;
                ZeroRunCount += 9;
            }

            EncodedCoeffs = EncodedCoeffs + ZeroRunCount;
        } else {
            if (nDecodeBool(br, BaselineProbsPtr[2])) {
                PrecTokenIndex = 2;
                if (nDecodeBool(br, BaselineProbsPtr[3])) {
                    if (nDecodeBool(br, BaselineProbsPtr[6])) {
                        if (nDecodeBool(br, BaselineProbsPtr[8])) {
                            token = nDecodeBool(br, BaselineProbsPtr[10]) + 9;
                        } else {
                            token = nDecodeBool(br, BaselineProbsPtr[9]) + 7;
                        }
                    } else {
                        token = nDecodeBool(br, BaselineProbsPtr[7]) + 5;
                    }
                    value = VP6_TokenExtraBits2[token].MinVal;
                    BitsCount = VP6_TokenExtraBits2[token].Length;
                    do {
                        value += nDecodeBool(br, VP6_TokenExtraBits2[token].Probs[BitsCount])
                                 << BitsCount;
                        BitsCount--;
                    } while (BitsCount >= 0);
                    SignBit = nDecodeBool128(br);
                    CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] =
                        (value ^ -SignBit) + SignBit;
                } else {
                    if (nDecodeBool(br, BaselineProbsPtr[4])) {
                        token = nDecodeBool(br, BaselineProbsPtr[5]) + 3;
                    } else {
                        token = 2;
                    }
                    SignBit = nDecodeBool128(br);
                    CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] =
                        (token ^ -SignBit) + SignBit;
                }
            } else {
                PrecTokenIndex = 1;
                SignBit = nDecodeBool128(br);
                CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] =
                    (1 ^ -SignBit) + SignBit;
            }
            EncodedCoeffs++;
        }
    }

    EncodedCoeffs--;
    return pbi->EobOffsetTable[EncodedCoeffs];
}

void VP6_DecodeFrameMbs(PB_INSTANCE *pbi) {
    unsigned int MBrow;
    unsigned int MBcol;
    unsigned int MBRows;
    unsigned int MBCols;
    int i;

    MBRows = pbi->MBRows;
    MBCols = pbi->MBCols;

    if (pbi->FrameType) {
        VP6_DecodeModeProbs(pbi);
        VP6_ConfigureMvEntropyDecoder(pbi, pbi->FrameType);
        pbi->LastMode = 0;
    } else {
        memcpy(pbi->probXmitted, VP6_BaselineXmittedProbs, sizeof(pbi->probXmitted));
        memcpy(pbi->IsMvShortProb, DefaultIsShortProbs, sizeof(pbi->IsMvShortProb));
        memcpy(pbi->MvShortProbs, DefaultMvShortProbs, sizeof(pbi->MvShortProbs));
        memcpy(pbi->MvSignProbs, DefaultSignProbs, sizeof(pbi->MvSignProbs));
        memcpy(pbi->MvSizeProbs, DefaultMvLongProbs, sizeof(pbi->MvSizeProbs));

        memset(pbi->MBModeProb, 128, 11);
        memset(pbi->BModeProb, 128, 11);
        memset(pbi->predictionMode, 1, pbi->MacroBlocks);

        if (pbi->Interlaced == 1) {
            memcpy(pbi->ScanBands, DefaultInterlacedScanBands, sizeof(pbi->ScanBands));
        } else {
            memcpy(pbi->ScanBands, DefaultNonInterlacedScanBands, sizeof(pbi->ScanBands));
        }

        BuildScanOrder(pbi, pbi->ScanBands);
    }

    VP6_ConfigureEntropyDecoder(pbi, pbi->FrameType);

    for (i = 0; i <= 63; i++) {
        pbi->MergedScanOrder[i] = pbi->quantizer->transIndex[pbi->ModifiedScanOrder[i]];
    }

    if (pbi->UseHuffman) {
        ConvertBoolTrees(pbi);
    }

    if (pbi->Interlaced == 1) {
        pbi->probInterlaced = (unsigned char)VP6_bitread(&pbi->br, 8);
    }

    VP6_ResetAboveContext(pbi);

    memset(pbi->Coeffs, 0, 768);

    pbi->CurrentDcRunLen[0] = 0;
    pbi->CurrentDcRunLen[1] = 0;
    pbi->CurrentAc1RunLen[0] = 0;
    pbi->CurrentAc1RunLen[1] = 0;

    for (MBrow = 3; MBrow < MBRows - 3; MBrow++) {
        VP6_ResetLeftContext(pbi);
        for (MBcol = 3; MBcol < MBCols - 3; MBcol++) {
            VP6_DecodeMacroBlock(pbi, MBrow, MBcol);
        }
    }
}
