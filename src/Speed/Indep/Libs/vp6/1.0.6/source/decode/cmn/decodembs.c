#include "../../../include/vp6_pbdll.h"
#include <string.h>
#include <stddef.h>

#define VP6_PROB_MUL(a, b) ((unsigned int)(a) * (unsigned int)(b) >> 8)

extern void VP6_BuildHuffTree(struct _huffnode *hn, unsigned int *counts,
                              int values);
extern void VP6_BuildHuffLookupTable(struct _huffnode *HuffTreeRoot,
                                      unsigned short *HuffTable);
extern void VP6_CreateCodeArray(struct _huffnode *hn, int node,
                                unsigned int *codearray,
                                unsigned char *lengtharray, int codevalue,
                                int codelength);
extern int nDecodeBool(BOOL_CODER *br, int probability);
extern unsigned int VP6_bitread(BOOL_CODER *br, int bits);
extern void VP6_ConfigureContexts(struct PB_INSTANCE *pbi);
extern void VP6_DecodeModeProbs(struct PB_INSTANCE *pbi);
extern void VP6_ConfigureMvEntropyDecoder(struct PB_INSTANCE *pbi,
                                           unsigned char FrameType);
extern void VP6_DecodeMacroBlock(struct PB_INSTANCE *pbi, unsigned int MBrow,
                                  unsigned int MBcol);
extern const unsigned char VP6_BaselineXmittedProbs[4][2][10];
extern const unsigned char DefaultMvShortProbs[2][7];
extern const unsigned char DefaultMvLongProbs[2][8];
extern const unsigned char DefaultIsShortProbs[2];
extern const unsigned char DefaultSignProbs[2];
extern const unsigned char DefaultInterlacedScanBands[64];
extern const unsigned char DefaultNonInterlacedScanBands[64];
extern unsigned char VP6_DcUpdateProbs[2][11];
extern unsigned char ScanBandUpdateProbs[64];
extern unsigned char ZrlUpdateProbs[2][14];
extern unsigned char ZeroRunProbDefaults[2][14];
extern unsigned char VP6_AcUpdateProbs[3][2][6][11];

typedef struct {
    unsigned short MinVal;
    short Length;
    unsigned char Probs[11];
} TOKENEXTRABITS;

static const TOKENEXTRABITS VP6_TokenExtraBits2[12] = {
    { 0, -1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 2, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 5, 0, { 0x9f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 7, 1, { 0x91, 0xa5, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 11, 2, { 0x8c, 0x94, 0xad, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 19, 3, { 0x87, 0x8c, 0x9b, 0xb0, 0, 0, 0, 0, 0, 0, 0 } },
    { 35, 4, { 0x82, 0x86, 0x8d, 0x9d, 0xb4, 0, 0, 0, 0, 0, 0 } },
    { 67, 10, { 0x81, 0x82, 0x85, 0x8c, 0x99, 0xb1, 0xc4, 0xe6, 0xf3, 0xfe, 0xfe } },
    { 0, -1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};

extern const int VP6_CoeffToBand[];

static inline int VP6_DecodeBool128(BOOL_CODER *br) {
    int bit;
    unsigned int split;
    unsigned int bigsplit;
    unsigned int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

    split = (range + 1) >> 1;
    bigsplit = split << 24;

    bit = value >= bigsplit;
    range = bit ? range - split : split;
    if (bit != 0) {
        value -= bigsplit;
    }
    value += value;
    range += range;

    if (!--count) {
        count = 8;
        value |= br->buffer[br->pos];
        br->pos++;
    }
    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
}

void BuildScanOrder(struct PB_INSTANCE *pbi, unsigned char *ScanBands) {
    unsigned int i;
    unsigned int j;
    unsigned int ScanOrderIndex;
    unsigned int MaxOffset;

    pbi->ModifiedScanOrder[0] = 0;
    ScanOrderIndex = 1;
    for (i = 0; i < 16; i++) {
        for (j = 1; j < 64; j++) {
            if (ScanBands[j] == i) {
                pbi->ModifiedScanOrder[ScanOrderIndex] = j;
                ScanOrderIndex++;
            }
        }
    }

    MaxOffset = 0;
    for (i = 0; i < 64; i++) {
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

    HuffProbs[11] = VP6_PROB_MUL(BoolTreeProbs[0], BoolTreeProbs[1]);
    HuffProbs[0] = VP6_PROB_MUL(BoolTreeProbs[0], 255 - BoolTreeProbs[1]);
    Prob = 255 - BoolTreeProbs[0];
    HuffProbs[1] = VP6_PROB_MUL(Prob, BoolTreeProbs[2]);

    Prob = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[2]);
    Prob1 = VP6_PROB_MUL(Prob, BoolTreeProbs[3]);
    HuffProbs[2] = VP6_PROB_MUL(Prob1, BoolTreeProbs[4]);
    Prob1 = VP6_PROB_MUL(Prob1, 255 - BoolTreeProbs[4]);
    HuffProbs[3] = VP6_PROB_MUL(Prob1, BoolTreeProbs[5]);
    HuffProbs[4] = VP6_PROB_MUL(Prob1, 255 - BoolTreeProbs[5]);

    Prob = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[3]);
    Prob1 = VP6_PROB_MUL(Prob, BoolTreeProbs[6]);
    HuffProbs[5] = VP6_PROB_MUL(Prob1, BoolTreeProbs[7]);
    HuffProbs[6] = VP6_PROB_MUL(Prob1, 255 - BoolTreeProbs[7]);

    Prob = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[6]);
    Prob1 = VP6_PROB_MUL(Prob, BoolTreeProbs[8]);
    HuffProbs[7] = VP6_PROB_MUL(Prob1, BoolTreeProbs[9]);
    HuffProbs[8] = VP6_PROB_MUL(Prob1, 255 - BoolTreeProbs[9]);

    Prob = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[8]);
    HuffProbs[9] = VP6_PROB_MUL(Prob, BoolTreeProbs[10]);
    HuffProbs[10] = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[10]);
}

void ZerosBoolTreeToHuffCodes(unsigned char *BoolTreeProbs,
                              unsigned int *HuffProbs) {
    unsigned int Prob;

    Prob = VP6_PROB_MUL(BoolTreeProbs[0], BoolTreeProbs[1]);
    HuffProbs[0] = VP6_PROB_MUL(Prob, BoolTreeProbs[2]);
    HuffProbs[1] = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[2]);

    Prob = VP6_PROB_MUL(BoolTreeProbs[0], 255 - BoolTreeProbs[1]);
    HuffProbs[2] = VP6_PROB_MUL(Prob, BoolTreeProbs[3]);
    HuffProbs[3] = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[3]);

    Prob = VP6_PROB_MUL(255 - BoolTreeProbs[0], BoolTreeProbs[4]);
    Prob = VP6_PROB_MUL(Prob, BoolTreeProbs[5]);
    HuffProbs[4] = VP6_PROB_MUL(Prob, BoolTreeProbs[6]);
    HuffProbs[5] = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[6]);

    Prob = VP6_PROB_MUL(255 - BoolTreeProbs[0], BoolTreeProbs[4]);
    Prob = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[5]);
    HuffProbs[6] = VP6_PROB_MUL(Prob, BoolTreeProbs[7]);
    HuffProbs[7] = VP6_PROB_MUL(Prob, 255 - BoolTreeProbs[7]);

    Prob = VP6_PROB_MUL(255 - BoolTreeProbs[0],
                        255 - BoolTreeProbs[4]);
    HuffProbs[8] = Prob;
}

void ConvertBoolTrees(struct PB_INSTANCE *pbi) {
    unsigned int i;
    unsigned int Plane;
    unsigned int Band;
    int Prec;

    for (Plane = 0; Plane < 2; Plane++) {
        BoolTreeToHuffCodes(pbi->DcProbs + Plane * 11,
                            pbi->DcHuffProbs[Plane]);
        VP6_BuildHuffTree(pbi->DcHuffTree[Plane],
                          pbi->DcHuffProbs[Plane], 12);
        VP6_BuildHuffLookupTable(
            pbi->DcHuffTree[Plane],
            pbi->DcHuffLUT[Plane]);
        VP6_CreateCodeArray(
            pbi->DcHuffTree[Plane], 0,
            pbi->DcHuffCode[Plane],
            pbi->DcHuffLength[Plane], 0, 0);
    }

    for (i = 0; i < 2; i++) {
        ZerosBoolTreeToHuffCodes(
            pbi->ZeroRunProbs[i],
            pbi->ZeroHuffProbs[i]);
        VP6_BuildHuffTree(pbi->ZeroHuffTree[i],
                          pbi->ZeroHuffProbs[i], 9);
        VP6_BuildHuffLookupTable(
            pbi->ZeroHuffTree[i],
            pbi->ZeroHuffLUT[i]);
        VP6_CreateCodeArray(
            pbi->ZeroHuffTree[i], 0,
            pbi->ZeroHuffCode[i],
            pbi->ZeroHuffLength[i], 0, 0);
    }

    for (Prec = 0; Prec < 3; Prec++) {
        for (Plane = 0; Plane < 2; Plane++) {
            for (Band = 0; Band < 6; Band++) {
                BoolTreeToHuffCodes(
                    pbi->AcProbs + (Prec * 66 + Plane * 198 + Band * 11),
                    pbi->AcHuffProbs[Prec][Plane][Band]);
                VP6_BuildHuffTree(
                    pbi->AcHuffTree[Prec][Plane][Band],
                    pbi->AcHuffProbs[Prec][Plane][Band], 12);
                VP6_BuildHuffLookupTable(
                    pbi->AcHuffTree[Prec][Plane][Band],
                    pbi->AcHuffLUT[Prec][Plane][Band]);
                VP6_CreateCodeArray(
                    pbi->AcHuffTree[Prec][Plane][Band],
                    0,
                    pbi->AcHuffCode[Prec][Plane][Band],
                    pbi->AcHuffLength[Prec][Plane][Band], 0, 0);
            }
        }
    }
}

void VP6_ConfigureEntropyDecoder(struct PB_INSTANCE *pbi,
                                  unsigned char FrameType) {
    unsigned int i;
    unsigned int j;
    unsigned int Plane;
    unsigned int Band;
    int Prec;
    unsigned char PrecNonZero;
    unsigned char LastProb[11];

    memset(LastProb, 128, 11);

    for (Plane = 0; Plane < 2; Plane++) {
        for (i = 0; i < 11; i++) {
            if (nDecodeBool((void *)&pbi->br, VP6_DcUpdateProbs[Plane][i])) {
                LastProb[i] = VP6_bitread((void *)&pbi->br, 7) << 1;
                LastProb[i] += (LastProb[i] == 0);
                pbi->DcProbs[Plane * 11 + i] = LastProb[i];
            } else if (FrameType == 0) {
                pbi->DcProbs[Plane * 11 + i] = LastProb[i];
            }
        }
    }

    if (FrameType == 0) {
        memcpy(pbi->ZeroRunProbs, ZeroRunProbDefaults, 28);
    }

    if (nDecodeBool((void *)&pbi->br, 128)) {
        for (i = 1; i < 64; i++) {
            if (nDecodeBool((void *)&pbi->br, ScanBandUpdateProbs[i])) {
                pbi->ScanBands[i] = VP6_bitread((void *)&pbi->br, 4);
            }
        }
        BuildScanOrder(pbi, pbi->ScanBands);
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 14; j++) {
            if (nDecodeBool((void *)&pbi->br, ZrlUpdateProbs[i][j])) {
                pbi->ZeroRunProbs[i][j] = VP6_bitread((void *)&pbi->br, 7) << 1;
                pbi->ZeroRunProbs[i][j] += (pbi->ZeroRunProbs[i][j] == 0);
            }
        }
    }

    for (Prec = 0; Prec < 3; Prec++) {
        for (Plane = 0; Plane < 2; Plane++) {
            for (Band = 0; Band < 6; Band++) {
                for (i = 0; i < 11; i++) {
                    if (nDecodeBool(
                            (void *)&pbi->br,
                            VP6_AcUpdateProbs[Prec][Plane][Band][i])) {
                        LastProb[i] = VP6_bitread((void *)&pbi->br, 7) << 1;
                        LastProb[i] += (LastProb[i] == 0);
                        pbi->AcProbs[Plane * 198 + Prec * 66 + Band * 11 + i] = LastProb[i];
                    } else if (FrameType == 0) {
                        pbi->AcProbs[Plane * 198 + Prec * 66 + Band * 11 + i] =
                            LastProb[i];
                    }
                }
            }
        }
    }

    VP6_ConfigureContexts(pbi);
}

void VP6_ResetLeftContext(struct PB_INSTANCE *pbi) {
    memset(&pbi->fc.LeftY[0], 0, sizeof(pbi->fc.LeftY[0]));
    memset(&pbi->fc.LeftY[1], 0, sizeof(pbi->fc.LeftY[1]));
    memset(&pbi->fc.LeftU, 0, sizeof(pbi->fc.LeftU));
    memset(&pbi->fc.LeftV, 0, sizeof(pbi->fc.LeftV));

    pbi->fc.LeftY[0].Mode = -1;
    pbi->fc.LeftY[1].Mode = -1;
    pbi->fc.LeftU.Mode = -1;
    pbi->fc.LeftV.Mode = -1;
    pbi->fc.LeftY[0].Frame = 4;
    pbi->fc.LeftY[1].Frame = 4;
    pbi->fc.LeftU.Frame = 4;
    pbi->fc.LeftV.Frame = 4;
}

void VP6_ResetAboveContext(struct PB_INSTANCE *pbi) {
    unsigned int i;

    i = 0;
    if (pbi->HFragments != (unsigned int)-8) {
        do {
            pbi->fc.AboveY[i].Mode = -1;
            pbi->fc.AboveY[i].Frame = 4;
            pbi->fc.AboveY[i].Dc = 0;
            pbi->fc.AboveY[i].Token = 0;
            i++;
        } while (i < pbi->HFragments + 8);
    }

    i = 0;
    if (pbi->HFragments / 2 != (unsigned int)-8) {
        do {
            pbi->fc.AboveU[i].Mode = -1;
            pbi->fc.AboveU[i].Frame = 4;
            pbi->fc.AboveU[i].Token = 0;
            pbi->fc.AboveU[i].Dc = 0;
            pbi->fc.AboveV[i].Mode = -1;
            pbi->fc.AboveV[i].Frame = 4;
            pbi->fc.AboveV[i].Token = 0;
            pbi->fc.AboveV[i].Dc = 0;
            i++;
        } while (i < pbi->HFragments / 2 + 8);
    }

    if (pbi->Vp3VersionNo <= 5) {
        pbi->fc.AboveU[1].Mode = 0;
        pbi->fc.AboveU[1].Frame = 0;
        pbi->fc.AboveV[1].Mode = 0;
        pbi->fc.AboveV[1].Frame = 0;
    }

    pbi->fc.LastDcY[0] = 0;
    pbi->fc.LastDcU[0] = 128;
    pbi->fc.LastDcV[0] = 128;
    for (i = 1; i < 3; i++) {
        pbi->fc.LastDcY[i] = 0;
        pbi->fc.LastDcU[i] = 0;
        pbi->fc.LastDcV[i] = 0;
    }
}

// NON_MATCHING: record-relative probability cursor ownership restored; other address lifetimes still differ.
unsigned char VP6_ReadTokensPredictA(struct PB_INSTANCE *pbi,
                                     short *CoeffData, unsigned int Plane,
                                     BLOCK_CONTEXT *Above,
                                     BLOCK_CONTEXT *Left) {
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
    int ThisTokenNonZero;

    AcProbsPtr = pbi->AcProbs + Plane * 198;
    TransIndex = pbi->quantizer->transIndex;

    if (pbi->MultiStream || !pbi->VpProfile) {
        br = &pbi->br2;
    } else {
        br = &pbi->br;
    }

    BaselineProbsPtr = pbi->DcProbs + Plane * 11;
    ContextProbsPtr = pbi->DcNodeContexts[Plane][Left->Token + Above->Token];
    if (!nDecodeBool(br, ContextProbsPtr[0])) {
        PrecTokenIndex = 0;
        Left->Token = 0;
        Above->Token = 0;
        goto ac_tokens;
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
                ContextProbsPtr = (unsigned char *)&VP6_TokenExtraBits2[token];
                ContextProbsPtr += offsetof(TOKENEXTRABITS, Probs) + BitsCount;
                do {
                    value += nDecodeBool(br, *ContextProbsPtr) << BitsCount;
                    ContextProbsPtr--;
                } while (--BitsCount >= 0);
                SignBit = VP6_DecodeBool128(br);
                CoeffData[0] = (short)((value ^ -SignBit) + SignBit);
            } else {
                if (nDecodeBool(br, ContextProbsPtr[4])) {
                    token = nDecodeBool(br, BaselineProbsPtr[5]) + 3;
                } else {
                    token = 2;
                }
                SignBit = VP6_DecodeBool128(br);
                CoeffData[0] = (short)((token ^ -SignBit) + SignBit);
            }
        } else {
            PrecTokenIndex = 1;
            value = 1;
            SignBit = VP6_DecodeBool128(br);
            CoeffData[0] = (short)((value ^ -SignBit) + SignBit);
            goto ac_tokens;
        }
    }

ac_tokens:
    EncodedCoeffs = 1;
    do {
        BaselineProbsPtr = AcProbsPtr + (PrecTokenIndex * 66 + VP6_CoeffToBand[EncodedCoeffs] * 11);
        if (EncodedCoeffs > 1 && PrecTokenIndex == 0) {
            ThisTokenNonZero = 1;
        } else {
            ThisTokenNonZero = nDecodeBool(br, BaselineProbsPtr[0]);
        }

        if (!ThisTokenNonZero) {
            if (!nDecodeBool(
                    br, BaselineProbsPtr[1])) {
                EncodedCoeffs++;
                goto token_return;
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
            EncodedCoeffs += ZeroRunCount;
            continue;
        }

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
                ContextProbsPtr = (unsigned char *)&VP6_TokenExtraBits2[token];
                ContextProbsPtr += offsetof(TOKENEXTRABITS, Probs) + BitsCount;
                do {
                    value += nDecodeBool(br, *ContextProbsPtr) << BitsCount;
                    ContextProbsPtr--;
                } while (--BitsCount >= 0);
                SignBit = VP6_DecodeBool128(br);
                CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] = (short)((value ^ -SignBit) + SignBit);
            } else {
                if (nDecodeBool(br, BaselineProbsPtr[4])) {
                    token = nDecodeBool(br, BaselineProbsPtr[5]) + 3;
                } else {
                    token = 2;
                }
                SignBit = VP6_DecodeBool128(br);
                CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] = (short)((token ^ -SignBit) + SignBit);
            }
        } else {
            PrecTokenIndex = 1;
            value = 1;
            SignBit = VP6_DecodeBool128(br);
            CoeffData[TransIndex[pbi->ModifiedScanOrder[EncodedCoeffs]]] = (short)((value ^ -SignBit) + SignBit);
        }
        EncodedCoeffs++;
    } while (EncodedCoeffs <= 63);

token_return:
    EncodedCoeffs--;
    return pbi->EobOffsetTable[EncodedCoeffs];
}

void VP6_DecodeFrameMbs(struct PB_INSTANCE *pbi) {
    unsigned int MBrow;
    unsigned int MBcol;
    unsigned int MBRows;
    unsigned int MBCols;
    unsigned int MB;
    int i;

    MBRows = pbi->MBRows;
    MBCols = pbi->MBCols;

    if (pbi->FrameType != 0) {
        VP6_DecodeModeProbs(pbi);
        VP6_ConfigureMvEntropyDecoder(pbi, pbi->FrameType);
        pbi->LastMode = 0;
    } else {
        memcpy(&pbi->probXmitted[0][0][0],
               &VP6_BaselineXmittedProbs[0][0][0],
               sizeof(VP6_BaselineXmittedProbs));
        memcpy(pbi->IsMvShortProb, &DefaultIsShortProbs,
               sizeof(pbi->IsMvShortProb));
        memcpy(pbi->MvShortProbs, DefaultMvShortProbs,
               sizeof(pbi->MvShortProbs));
        memcpy(pbi->MvSignProbs, &DefaultSignProbs,
               sizeof(pbi->MvSignProbs));
        memcpy(pbi->MvSizeProbs, DefaultMvLongProbs,
               sizeof(pbi->MvSizeProbs));
        memset(pbi->MBModeProb, 128, sizeof(pbi->MBModeProb));
        memset(pbi->BModeProb, 128, sizeof(pbi->BModeProb));
        memset(pbi->predictionMode, 1, pbi->MacroBlocks);

        if (pbi->Configuration.Interlaced == 1) {
            memcpy(pbi->ScanBands, DefaultInterlacedScanBands,
                   sizeof(pbi->ScanBands));
        } else {
            memcpy(pbi->ScanBands, DefaultNonInterlacedScanBands,
                   sizeof(pbi->ScanBands));
        }
        BuildScanOrder(pbi, pbi->ScanBands);
    }

    VP6_ConfigureEntropyDecoder(pbi, pbi->FrameType);

    for (i = 0; i < 64; i++) {
        pbi->MergedScanOrder[i] =
            pbi->quantizer->transIndex[pbi->ModifiedScanOrder[i]];
    }

    if (pbi->UseHuffman != 0) {
        ConvertBoolTrees(pbi);
    }

    if (pbi->Configuration.Interlaced == 1) {
        pbi->probInterlaced = VP6_bitread(&pbi->br, 8) & 0xff;
    }

    VP6_ResetAboveContext(pbi);
    memset(pbi->mbi.Coeffs, 0, 768);

    MB = 0;
    pbi->CurrentDcRunLen[0] = MB;
    pbi->CurrentDcRunLen[1] = MB;
    pbi->CurrentAc1RunLen[0] = MB;
    pbi->CurrentAc1RunLen[1] = MB;

    MBrow = 3;
    if (MBrow < MBRows - 3) {
        do {
            VP6_ResetLeftContext(pbi);
            for (MBcol = 3; MBcol < MBCols - 3; MBcol++) {
                VP6_DecodeMacroBlock(pbi, MBrow, MBcol);
            }
            MBrow++;
        } while (MBrow < MBRows - 3);
    }
}

const int VP6_CoeffToBand[65] = {
    -1, 0,
    1, 1, 1,
    2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7
};
