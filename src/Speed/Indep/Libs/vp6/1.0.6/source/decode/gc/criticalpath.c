#include "../../../include/vp6_pbdll.h"

extern int VP6_ModeUsesMC[10];
extern int VP6_Mode2Frame[16];
extern void (*ReconIntra)(short *, unsigned char *, unsigned short *, unsigned int);
extern void (*ReconInter)(short *, unsigned char *, unsigned char *, short *, unsigned int);
extern void (*ReconBlock)(short *, short *, unsigned char *, unsigned int);
extern void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *OutputPtr,
                                     BLOCK_POSITION bp);
extern void VP6_DecodeBlock(struct PB_INSTANCE *pbi, unsigned int MBrow,
                             unsigned int MBcol, BLOCK_POSITION bp);
extern void FilterBlock1d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                          unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                          unsigned int OutputHeight, unsigned int OutputWidth,
                          int *Filter);
extern void FilterBlock2d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                          unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter);
extern void FilterBlock1dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                                unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                                int *Filter);
extern void FilterBlock2dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                                unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter);


static int BicubicFilters[32] = {
    0, 0x80, 0, 0,
    -4, 0x76, 0x10, -2,
    -7, 0x6a, 0x22, -5,
    -8, 0x5a, 0x35, -7,
    -8, 0x48, 0x48, -8,
    -7, 0x35, 0x5a, -8,
    -5, 0x22, 0x6a, -7,
    -2, 0x10, 0x76, -4
};
static int BilinearFilters[16] = {
    0x80000000, 0, 0x70000000, 0x10000000,
    0x60000000, 0x20000000, 0x50000000, 0x30000000,
    0x40000000, 0x40000000, 0x30000000, 0x50000000,
    0x20000000, 0x60000000, 0x10000000, 0x70000000
};

int VP6_DecodeBool(BOOL_CODER *br, int probability) {
    unsigned int bit = 0;
    unsigned int split;
    unsigned int bigsplit;
    unsigned int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

    split = 1 + (((range - 1) * probability) >> 8);
    bigsplit = split << 24;

    range = split;
    if (value >= bigsplit) {
        range = br->range - split;
        value = value - bigsplit;
        bit = 1;
    }

    if (range >= 0x80) {
        br->value = value;
        br->range = range;
        return bit;
    }

    do {
        range += range;
        value += value;

        if (!--count) {
            count = 8;
            value |= br->buffer[br->pos];
            br->pos++;
        }
    } while (range < 0x80);

    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
}

int VP6_DecodeBool128(BOOL_CODER *br) {
    unsigned int bit;
    unsigned int split;
    unsigned int bigsplit;
    unsigned int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

    split = (range + 1) >> 1;
    bigsplit = split << 24;

    if (value >= bigsplit) {
        range = (range - split) << 1;
        value = (value - bigsplit) << 1;
        bit = 1;
    } else {
        range = split << 1;
        value = value << 1;
        bit = 0;
    }

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

int nDecodeBool(BOOL_CODER *br, int probability) {
    unsigned int bit = 0;
    unsigned int split;
    unsigned int bigsplit;
    int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

    split = 1 + (((range - 1) * probability) >> 8);
    bigsplit = split << 24;

    if (value >= bigsplit) {
        range = br->range - split;
        value = value - bigsplit;
        bit = 1;
    } else {
        range = split;
    }

    if (range < 0x80) {
        do {
            range += range;
            value += value;

            if (!--count) {
                count = 8;
                value |= br->buffer[br->pos];
                br->pos++;
            }
        } while (range < 0x80);
    }

    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
}

CODING_MODE VP6_DecodeBlockMode(struct PB_INSTANCE *pbi) {
    int choice;

    choice = VP6_DecodeBool128((BOOL_CODER *)&pbi->br) << 1;
    choice += VP6_DecodeBool128((BOOL_CODER *)&pbi->br);

    switch (choice) {
    case 0:
        return 0;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    default:
        return 0;
    }
}

CODING_MODE VP6_DecodeMode(struct PB_INSTANCE *pbi, CODING_MODE lastmode,
                           unsigned int type) {
    CODING_MODE mode;

    if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->probModeSame[type][lastmode])) {
        mode = lastmode;
    } else {
        unsigned char *Stats;
        Stats = pbi->probMode[type][lastmode];
        if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[0])) {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[2])) {
                if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[6])) {
                    mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[8]) + 8;
                } else {
                    mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[7]) + 5;
                }
            } else {
                mode = 1;
                if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[5])) {
                    mode = 7;
                }
            }
        } else {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[1])) {
                mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[4]) + 3;
            } else {
                mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[3]) << 1;
            }
        }
    }
    return mode;
}

// NON_MATCHING: probability-address hoisting changes ASM and pbi's DWARF home.
void VP6_decodeMotionVector(
    struct PB_INSTANCE *pbi,
    MOTION_VECTOR *mv,
    CODING_MODE Mode) {
    unsigned int i;
    int Vector;
    int SignBit;
    int MvOffsetX;
    int MvOffsetY;

    MvOffsetX = 0;
    MvOffsetY = 0;
    if (Mode == CODE_INTER_PLUS_MV) {
        if (pbi->mbi.NearestMvIndex <= 1) {
            MvOffsetX = pbi->mbi.NearestInterMVect.x;
            MvOffsetY = pbi->mbi.NearestInterMVect.y;
        }
    } else {
        if (pbi->mbi.NearestGMvIndex <= 1) {
            MvOffsetX = pbi->mbi.NearestGoldMVect.x;
            MvOffsetY = pbi->mbi.NearestGoldMVect.y;
        }
    }

    for (i = 0; i < 2; i++) {
        if (!VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->IsMvShortProb[i])) {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][0])) {
                if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][4])) {
                    Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][6]) + 6;
                } else {
                    Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][5]) + 4;
                }
            } else {
                if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][1])) {
                    Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][3]) + 2;
                } else {
                    Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvShortProbs[i][2]);
                }
            }
        } else {
            Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                    pbi->MvSizeProbs[i][0]);
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][1]) << 1;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][2]) << 2;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][7]) << 7;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][6]) << 6;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][5]) << 5;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     pbi->MvSizeProbs[i][4]) << 4;
            if (Vector & 0xf0) {
                Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                         pbi->MvSizeProbs[i][3]) << 3;
            } else {
                Vector += 8;
            }
        }

        if (Vector != 0) {
            SignBit = VP6_DecodeBool((BOOL_CODER *)&pbi->br, pbi->MvSignProbs[i]);
            if (SignBit) {
                Vector = -Vector;
            }
        }
        if (i != 0) {
            mv->y = Vector + MvOffsetY;
        } else {
            mv->x = Vector + MvOffsetX;
        }
    }
}

void VP6_FindNearestandNextNearest(struct PB_INSTANCE *pbi,
                                    unsigned int MBrow, unsigned int MBcol,
                                    unsigned char Frame, int *type) {
    int i;
    unsigned int OffsetMB;
    unsigned int BaseMB;
    int Nearest;
    int NextNearest;
    int nearestIndex;
    unsigned int thisMv;
    int typet;

    BaseMB = MBrow * pbi->MBCols + MBcol;
    Nearest = 0;
    NextNearest = 0;
    typet = 1;
    for (i = 0; i < 12; i++) {
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;
        if (VP6_Mode2Frame[pbi->predictionMode[OffsetMB]] == Frame) {
            thisMv = ((unsigned int *)pbi->MBMotionVector)[OffsetMB];
            if (thisMv != 0) {
                Nearest = thisMv;
                typet = 2;
                break;
            }
        }
    }
    nearestIndex = i;
    i = nearestIndex + 1;
    if (i < 12) {
        do {
            OffsetMB = pbi->mvNearOffset[i] + BaseMB;
            if (VP6_Mode2Frame[pbi->predictionMode[OffsetMB]] == Frame) {
                thisMv = ((unsigned int *)pbi->MBMotionVector)[OffsetMB];
                if (thisMv != (unsigned int)Nearest && thisMv != 0) {
                    NextNearest = thisMv;
                    typet = 0;
                    break;
                }
            }
            i++;
        } while (i < 12);
    }
    if (Frame == 1) {
        *type = typet;
        pbi->mbi.NearestMvIndex = nearestIndex;
        pbi->mbi.NearestInterMVect = *(MOTION_VECTOR *)&Nearest;
        pbi->mbi.NearInterMVect = *(MOTION_VECTOR *)&NextNearest;
    } else {
        pbi->mbi.NearestGMvIndex = nearestIndex;
        pbi->mbi.NearestGoldMVect = *(MOTION_VECTOR *)&Nearest;
        pbi->mbi.NearGoldMVect = *(MOTION_VECTOR *)&NextNearest;
    }
}

void VP6_decodeModeAndMotionVector(struct PB_INSTANCE *pbi,
                                   unsigned int MBrow, unsigned int MBcol) {
    int type;
    int x;
    int y;
    unsigned int k;
    CODING_MODE mode;
    MOTION_VECTOR mv;

    VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 1, &type);
    mode = VP6_DecodeMode(pbi, pbi->LastMode, type);
    pbi->LastMode = mode;
    pbi->predictionMode[MBrow * pbi->MBCols + MBcol] = mode;
    pbi->mbi.Mode = mode;

    if (mode == 7) {
        x = 0;
        y = 0;
        pbi->mbi.BlockMode[0] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[1] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[2] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[3] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[4] = mode;
        pbi->mbi.BlockMode[5] = mode;

        for (k = 0; k < 4; k++) {
            if (pbi->mbi.BlockMode[k] == 0) {
                pbi->mbi.Mv[k].x = 0;
                pbi->mbi.Mv[k].y = 0;
            } else if (pbi->mbi.BlockMode[k] == 3) {
                pbi->mbi.Mv[k].x = pbi->mbi.NearestInterMVect.x;
                pbi->mbi.Mv[k].y = pbi->mbi.NearestInterMVect.y;
                x += pbi->mbi.NearestInterMVect.x;
                y += pbi->mbi.NearestInterMVect.y;
            } else if (pbi->mbi.BlockMode[k] == 4) {
                pbi->mbi.Mv[k].x = pbi->mbi.NearInterMVect.x;
                pbi->mbi.Mv[k].y = pbi->mbi.NearInterMVect.y;
                x += pbi->mbi.NearInterMVect.x;
                y += pbi->mbi.NearInterMVect.y;
            } else if (pbi->mbi.BlockMode[k] == 2) {
                VP6_decodeMotionVector(pbi, &mv, 2);
                pbi->mbi.Mv[k].x = mv.x;
                pbi->mbi.Mv[k].y = mv.y;
                x += mv.x;
                y += mv.y;
            }
        }

        x = (x + 1 + (x >= 0)) >> 2;
        y = (y + 1 + (y >= 0)) >> 2;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = pbi->mbi.Mv[3].x;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = pbi->mbi.Mv[3].y;
        pbi->mbi.Mv[4].x = x;
        pbi->mbi.Mv[4].y = y;
        pbi->mbi.Mv[5].x = x;
        pbi->mbi.Mv[5].y = y;
    } else {
        switch (mode) {
        case 3:
            x = pbi->mbi.NearestInterMVect.x;
            y = pbi->mbi.NearestInterMVect.y;
            break;
        case 4:
            x = pbi->mbi.NearInterMVect.x;
            y = pbi->mbi.NearInterMVect.y;
            break;
        case 8:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearestGoldMVect.x;
            y = pbi->mbi.NearestGoldMVect.y;
            break;
        case 9:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearGoldMVect.x;
            y = pbi->mbi.NearGoldMVect.y;
            break;
        case 2:
            VP6_decodeMotionVector(pbi, &mv, 2);
            x = mv.x;
            y = mv.y;
            break;
        case 6:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            VP6_decodeMotionVector(pbi, &mv, 6);
            x = mv.x;
            y = mv.y;
            break;
        default:
            x = 0;
            y = 0;
            break;
        }

        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = x;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = y;
        for (k = 0; k < 6; k++) {
            pbi->mbi.Mv[k].x = x;
            pbi->mbi.Mv[k].y = y;
            pbi->mbi.BlockMode[k] = mode;
        }
    }
}

#include "../cmn/decodembs.inl"

#include "../cmn/recon.inl"

void VP6_ReconstructBlock(struct PB_INSTANCE *pbi, BLOCK_POSITION bp) {
    if (pbi->mbi.Mode == 0) {
        ReconInter(pbi->TmpDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->LastFrameRecon + pbi->mbi.Recon,
                   pbi->ReconDataBuffer, pbi->mbi.CurrentReconStride);
    } else if (VP6_ModeUsesMC[(int)pbi->mbi.Mode]) {
        VP6_PredictFilteredBlock(pbi, pbi->TmpDataBuffer, bp);
        ReconBlock(pbi->TmpDataBuffer, pbi->ReconDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->mbi.CurrentReconStride);
    } else if (pbi->mbi.Mode == 5) {
        ReconInter(pbi->TmpDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->GoldenFrame + pbi->mbi.Recon,
                   pbi->ReconDataBuffer, pbi->mbi.CurrentReconStride);
    } else {
        ReconIntra(pbi->TmpDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon,
                   (unsigned short *)pbi->ReconDataBuffer,
                   pbi->mbi.CurrentReconStride);
    }
}

void FilterBlock_GC(unsigned char *ReconPtr1, unsigned char *ReconPtr2,
                    unsigned short *ReconRefPtr, unsigned int PixelsPerLine,
                    int ModX, int ModY, int UseBicubic) {
    int diff;

    diff = ReconPtr2 - ReconPtr1;
    if (diff < 0) {
        unsigned char *temp = ReconPtr1;
        ReconPtr1 = ReconPtr2;
        ReconPtr2 = temp;
        diff = ReconPtr2 - ReconPtr1;
    }

    if (!UseBicubic) {
        if (diff == 1) {
            FilterBlock1dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine, 1,
                                 &BilinearFilters[ModX * 2]);
        } else if (diff == (int)PixelsPerLine) {
            FilterBlock1dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine,
                                PixelsPerLine, &BilinearFilters[ModY * 2]);
        } else if (diff == (int)PixelsPerLine - 1) {
            FilterBlock2dBil_GC(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine,
                                &BilinearFilters[ModX * 2],
                                &BilinearFilters[ModY * 2]);
        } else {
            FilterBlock2dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine,
                                &BilinearFilters[ModX * 2],
                                &BilinearFilters[ModY * 2]);
        }
    } else {
        if (diff == 1) {
            FilterBlock1d(ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8,
                          &BicubicFilters[ModX * 4]);
        } else if (diff == (int)PixelsPerLine) {
            FilterBlock1d(ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine,
                          8, 8, &BicubicFilters[ModY * 4]);
        } else if (diff == (int)PixelsPerLine - 1) {
            FilterBlock2d(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine,
                          &BicubicFilters[ModX * 4],
                          &BicubicFilters[ModY * 4]);
        } else {
            FilterBlock2d(ReconPtr1, ReconRefPtr, PixelsPerLine,
                          &BicubicFilters[ModX * 4],
                          &BicubicFilters[ModY * 4]);
        }
    }
}

#include "gcreconstruct.inl"
#include "gcidct.inl"
#include "gcfilterblock.inl"
