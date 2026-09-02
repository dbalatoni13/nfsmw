#include "../../../include/vp6_pbdll.h"
#include <string.h>

#define VP6_PROB_MUL(a, b) ((unsigned int)(a) * (unsigned int)(b) >> 8)

extern void VP6_BuildHuffTree(struct _huffnode *hn, unsigned int *counts,
                              int values);
extern void VP6_BuildHuffLookupTable(struct _huffnode *HuffTreeRoot,
                                      unsigned short *HuffTable);
extern void VP6_CreateCodeArray(struct _huffnode *hn, int node,
                                unsigned int *codearray,
                                unsigned char *lengtharray, int codevalue,
                                int codelength);
extern int nDecodeBool(void *br, int probability);
extern unsigned int VP6_bitread(void *br, int bits);
extern void VP6_ConfigureContexts(struct PB_INSTANCE *pbi);
extern void VP6_DecodeModeProbs(struct PB_INSTANCE *pbi);
extern void VP6_ConfigureMvEntropyDecoder(struct PB_INSTANCE *pbi,
                                           unsigned char FrameType);
extern void VP6_DecodeMacroBlock(struct PB_INSTANCE *pbi, unsigned int MBrow,
                                  unsigned int MBcol);
extern const unsigned char VP6_BaselineXmittedProbs[4][2][10];
extern const unsigned char DefaultMvShortProbs[14];
extern const unsigned char DefaultMvLongProbs[16];
extern const unsigned short DefaultIsShortProbs;
extern const unsigned short DefaultSignProbs;
extern const unsigned char DefaultInterlacedScanBands[64];
extern const unsigned char DefaultNonInterlacedScanBands[64];
extern unsigned char VP6_DcUpdateProbs[2][11];
extern unsigned char ScanBandUpdateProbs[64];
extern unsigned char ZrlUpdateProbs[2][14];
extern unsigned char ZeroRunProbDefaults[2][14];
extern unsigned char VP6_AcUpdateProbs[3][2][6][11];

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
        BoolTreeToHuffCodes(pbi->DcProbs,
                            (unsigned int *)pbi->DcHuffProbs + Plane * 12);
        VP6_BuildHuffTree((struct _huffnode *)pbi->DcHuffTree + Plane * 12,
                          (unsigned int *)pbi->DcHuffProbs + Plane * 12, 12);
        VP6_BuildHuffLookupTable(
            (struct _huffnode *)pbi->DcHuffTree + Plane * 12,
            (unsigned short *)pbi->DcHuffLUT + Plane * 64);
        VP6_CreateCodeArray(
            (struct _huffnode *)pbi->DcHuffTree + Plane * 12, 0,
            (unsigned int *)pbi->DcHuffCode + Plane * 12,
            (unsigned char *)pbi->DcHuffLength + Plane * 12, 0, 0);
    }

    for (i = 0; i < 2; i++) {
        ZerosBoolTreeToHuffCodes(
            (unsigned char *)pbi->ZeroRunProbs + i * 14,
            (unsigned int *)pbi->ZeroHuffProbs + i * 14);
        VP6_BuildHuffTree((struct _huffnode *)pbi->ZeroHuffTree + i * 14,
                          (unsigned int *)pbi->ZeroHuffProbs + i * 14, 9);
        VP6_BuildHuffLookupTable(
            (struct _huffnode *)pbi->ZeroHuffTree + i * 14,
            (unsigned short *)pbi->ZeroHuffLUT + i * 64);
        VP6_CreateCodeArray(
            (struct _huffnode *)pbi->ZeroHuffTree + i * 14, 0,
            (unsigned int *)pbi->ZeroHuffCode + i * 14,
            (unsigned char *)pbi->ZeroHuffLength + i * 14, 0, 0);
    }

    for (Prec = 0; Prec < 3; Prec++) {
        for (Plane = 0; Plane < 2; Plane++) {
            for (Band = 0; Band < 6; Band++) {
                BoolTreeToHuffCodes(
                    pbi->AcProbs + Prec * 66 + Plane * 198 + Band * 11,
                    (unsigned int *)pbi->AcHuffProbs +
                        Prec * 144 + Plane * 72 + Band * 12);
                VP6_BuildHuffTree(
                    (struct _huffnode *)pbi->AcHuffTree +
                        Prec * 144 + Plane * 72 + Band * 12,
                    (unsigned int *)pbi->AcHuffProbs +
                        Prec * 144 + Plane * 72 + Band * 12, 12);
                VP6_BuildHuffLookupTable(
                    (struct _huffnode *)pbi->AcHuffTree +
                        Prec * 144 + Plane * 72 + Band * 12,
                    (unsigned short *)pbi->AcHuffLUT +
                        Prec * 768 + Plane * 384 + Band * 64);
                VP6_CreateCodeArray(
                    (struct _huffnode *)pbi->AcHuffTree +
                        Prec * 144 + Plane * 72 + Band * 12,
                    0,
                    (unsigned int *)pbi->AcHuffCode +
                        Prec * 144 + Plane * 72 + Band * 12,
                    (unsigned char *)pbi->AcHuffLength +
                        Prec * 144 + Plane * 72 + Band * 12, 0, 0);
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
    unsigned int PrecNonZero;
    unsigned char LastProb[11];

    memset(LastProb, 128, 11);

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 11; i++) {
            if (nDecodeBool((void *)&pbi->br, VP6_DcUpdateProbs[j][i])) {
                PrecNonZero = VP6_bitread((void *)&pbi->br, 7) << 1;
                PrecNonZero += PrecNonZero == 0;
                pbi->DcProbs[j * 11 + i] = PrecNonZero;
                LastProb[i] = PrecNonZero;
            } else if (FrameType == 0) {
                pbi->DcProbs[j * 11 + i] = LastProb[i];
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

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 14; i++) {
            if (nDecodeBool((void *)&pbi->br, ZrlUpdateProbs[j][i])) {
                PrecNonZero = VP6_bitread((void *)&pbi->br, 7) << 1;
                PrecNonZero += PrecNonZero == 0;
                pbi->ZeroRunProbs[j][i] = PrecNonZero;
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
                        PrecNonZero = VP6_bitread((void *)&pbi->br, 7) << 1;
                        PrecNonZero += PrecNonZero == 0;
                        pbi->AcProbs[Prec * 66 + Plane * 198 + Band * 11 + i] =
                            PrecNonZero;
                        LastProb[i] = PrecNonZero;
                    } else if (FrameType == 0) {
                        pbi->AcProbs[Prec * 66 + Plane * 198 + Band * 11 + i] =
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
        memcpy(pbi->probXmitted, VP6_BaselineXmittedProbs,
               sizeof(pbi->probXmitted));
        memcpy(pbi->MvSignProbs, &DefaultSignProbs,
               sizeof(pbi->MvSignProbs));
        memcpy(pbi->IsMvShortProb, &DefaultIsShortProbs,
               sizeof(pbi->IsMvShortProb));
        memcpy(pbi->MvShortProbs, DefaultMvShortProbs,
               sizeof(pbi->MvShortProbs));
        memcpy(pbi->MvSizeProbs, DefaultMvLongProbs,
               sizeof(pbi->MvSizeProbs));
        memset(pbi->MBModeProb, 128, sizeof(pbi->MBModeProb));
        memset(pbi->BModeProb, 128, sizeof(pbi->BModeProb));
        memset(pbi->predictionMode, 1, pbi->HFragments);

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

    for (MB = 0; MB < 64; MB++) {
        pbi->MergedScanOrder[MB] =
            pbi->quantizer->transIndex[pbi->ModifiedScanOrder[MB]];
    }

    if (pbi->UseHuffman != 0) {
        ConvertBoolTrees(pbi);
    }

    if (pbi->Configuration.Interlaced == 1) {
        pbi->probInterlaced = VP6_bitread(&pbi->br, 8);
    }

    VP6_ResetAboveContext(pbi);
    memset(pbi->mbi.Coeffs, 0, 768);

    pbi->CurrentDcRunLen[0] = 0;
    pbi->CurrentDcRunLen[1] = 0;
    pbi->CurrentAc1RunLen[0] = 0;
    pbi->CurrentAc1RunLen[1] = 0;

    MBRows -= 3;
    MBCols -= 3;
    MBrow = 3;
    if (MBrow < MBRows) {
        do {
            MBcol = 3;
            VP6_ResetLeftContext(pbi);
            do {
                VP6_DecodeMacroBlock(pbi, MBrow, MBcol);
                MBcol++;
            } while (MBcol < MBCols);
            MBrow++;
        } while (MBrow < MBRows);
    }
}
