#include "../../../include/vp6_pbdll.h"

#define VP6_MV_SHORT_PROB(pbi, i, n) \
    (((unsigned char *)(pbi)) + 0x70c)[(i) * 7 + (n)]
#define VP6_MV_SIZE_PROB(pbi, i, n) \
    (((unsigned char *)(pbi)) + 0x720)[(i) * 8 + (n)]

extern int VP6_ModeUsesMC[10];
extern int VP6_Mode2Frame[16];
extern void (*ReconIntra)(short *, unsigned char *, unsigned short *, unsigned int);
extern void (*ReconInter)(short *, unsigned char *, unsigned char *, short *, unsigned int);
extern void (*ReconBlock)(short *, short *, unsigned char *, unsigned int);
extern void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *OutputPtr,
                                     int bp);
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

static unsigned int idctconstants[8] = {
    0x41000000, 0x3f7b14be, 0x3f6c835e, 0x3f54db31,
    0x3f3504f3, 0x3f0e39da, 0x3ec3ef15, 0x3e47c5c2
};
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
static float f128 = 128.0f;
static float f64 = 64.0f;

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

int VP6_DecodeBlockMode(struct PB_INSTANCE *pbi) {
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
    unsigned char *Stats;

    mode = lastmode;
    if (VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                       pbi->probModeSame[type][lastmode])) {
        return mode;
    }

    Stats = (unsigned char *)pbi + 0x7a8 + type * 90 + lastmode * 9;
    if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[0])) {
        if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[2])) {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[6])) {
                mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[8]) + 8;
            } else {
                mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[7]) + 5;
            }
        } else {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[5])) {
                mode = 7;
            } else {
                mode = 1;
            }
        }
    } else {
        if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[1])) {
            mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[4]) + 3;
        } else {
            mode = VP6_DecodeBool((BOOL_CODER *)&pbi->br, Stats[3]) << 1;
        }
    }

    return mode;
}

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
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                               VP6_MV_SHORT_PROB(pbi, i, 0))) {
                if (VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                   VP6_MV_SHORT_PROB(pbi, i, 4))) {
                    if (VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                       VP6_MV_SHORT_PROB(pbi, i, 6))) {
                        Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                                VP6_MV_SHORT_PROB(pbi, i, 2)) + 6;
                    } else {
                        Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                                VP6_MV_SHORT_PROB(pbi, i, 5)) + 4;
                    }
                } else {
                    if (VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                       VP6_MV_SHORT_PROB(pbi, i, 3))) {
                        Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                                VP6_MV_SHORT_PROB(pbi, i, 1)) + 2;
                    } else {
                        Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                                VP6_MV_SHORT_PROB(pbi, i, 0));
                    }
                }
            }
        } else {
            Vector = VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                    VP6_MV_SIZE_PROB(pbi, i, 0));
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 1)) << 1;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 2)) << 2;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 7)) << 7;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 6)) << 6;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 5)) << 5;
            Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                     VP6_MV_SIZE_PROB(pbi, i, 4)) << 4;
            if (Vector & 0xf0) {
                Vector += VP6_DecodeBool((BOOL_CODER *)&pbi->br,
                                         VP6_MV_SIZE_PROB(pbi, i, 3)) << 3;
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

    OffsetMB = MBrow * pbi->MBCols + MBcol;
    BaseMB = pbi->mvNearOffset[0];
    Nearest = 0;
    NextNearest = 0;
    typet = 1;
    i = 0;
    thisMv = pbi->predictionMode[BaseMB + OffsetMB];
    while (1) {
        if (VP6_Mode2Frame[(int)thisMv] == Frame) {
            thisMv = ((unsigned int *)pbi->MBMotionVector)[BaseMB + OffsetMB];
            if (thisMv != 0) {
                Nearest = (int)thisMv;
                typet = 2;
                goto FirstNearestFound;
            }
        }
        i++;
        BaseMB = pbi->mvNearOffset[i];
        if (i > 11) {
            break;
        }
        thisMv = pbi->predictionMode[BaseMB + OffsetMB];
    }

FirstNearestFound:
    nearestIndex = i;
    i = nearestIndex + 1;
    if (i <= 11) {
        BaseMB = pbi->mvNearOffset[i];
        thisMv = pbi->predictionMode[BaseMB + OffsetMB];
        while (1) {
            if (VP6_Mode2Frame[(int)thisMv] == Frame) {
                thisMv = ((unsigned int *)pbi->MBMotionVector)[BaseMB + OffsetMB];
                if (thisMv != (unsigned int)Nearest && thisMv != 0) {
                    NextNearest = (int)thisMv;
                    typet = 0;
                    goto SecondNearestFound;
                }
            }
            i++;
            if (i > 11) {
                break;
            }
            BaseMB = pbi->mvNearOffset[i];
            thisMv = pbi->predictionMode[BaseMB + OffsetMB];
        }
    }

SecondNearestFound:
    *type = typet;
    if (Frame == 1) {
        *(unsigned int *)&pbi->mbi.NearInterMVect = (unsigned int)NextNearest;
        pbi->mbi.NearestMvIndex = nearestIndex;
        *(unsigned int *)&pbi->mbi.NearestInterMVect = (unsigned int)Nearest;
    } else {
        *(unsigned int *)&pbi->mbi.NearGoldMVect = (unsigned int)NextNearest;
        pbi->mbi.NearestGMvIndex = nearestIndex;
        *(unsigned int *)&pbi->mbi.NearestGoldMVect = (unsigned int)Nearest;
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
        pbi->mbi.BlockMode[5] = mode;
        pbi->mbi.BlockMode[4] = mode;

        k = 0;
        while (k <= 3) {
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
            k++;
        }

        if (x < 0) {
            x = (x + 1) >> 2;
        } else {
            x = (x + 2) >> 2;
        }
        if (y < 0) {
            y = (y + 1) >> 2;
        } else {
            y = (y + 2) >> 2;
        }
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = pbi->mbi.Mv[3].x;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = pbi->mbi.Mv[3].y;
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
            VP6_decodeMotionVector(pbi, &mv, 6);
            x = mv.x;
            y = mv.y;
            break;
        default:
            x = 0;
            y = 0;
            break;
        }
    }

    pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = x;
    pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = y;
    for (k = 0; k < 6; k++) {
        pbi->mbi.Mv[k].x = x;
        pbi->mbi.Mv[k].y = y;
        pbi->mbi.BlockMode[k] = mode;
    }
}

void VP6_ReconstructBlock(struct PB_INSTANCE *pbi, BLOCK_POSITION bp) {
    if (pbi->mbi.Mode == 0) {
        ReconInter(pbi->TmpDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->LastFrameRecon + pbi->mbi.Recon,
                   pbi->ReconDataBuffer, pbi->mbi.CurrentReconStride);
    } else if (*((int *)VP6_ModeUsesMC + (int)pbi->mbi.Mode)) {
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
        } else if (diff == (int)PixelsPerLine + 1) {
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
        } else if (diff == (int)PixelsPerLine + 1) {
            FilterBlock2d(ReconPtr1, ReconRefPtr, PixelsPerLine,
                          &BicubicFilters[ModX * 4],
                          &BicubicFilters[ModY * 4]);
        }
    }
}

void FilterBlock1dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                         int *Filter) {
    __asm__ volatile(
        "li 9, f64\n"
        "li 0, 8\n"
        "lq 8, -24576(7)\n"
        "psq_l f9, 7(7), 1, 2\n"
        "lq 10, -32768(9)\n"
        "mtctr 0\n"
        "add 6, 3, 6\n"
        "vmhaddshs v10, v10, v10, v16\n"
        "1:\n"
        "lq 0, 8192(3)\n"
        "psq_l f1, 2(3), 0, 2\n"
        "lq 2, 8192(3)\n"
        "psq_l f3, 6(3), 0, 2\n"
        "add 3, 3, 5\n"
        "lq 4, 8192(6)\n"
        "psq_l f5, 2(6), 0, 2\n"
        "psq_l f6, 4(6), 0, 2\n"
        "psq_l f7, 6(6), 0, 2\n"
        "add 6, 6, 5\n"
        "vextduwvlx v0, v0, v10, 8\n"
        "vextduwvlx v1, v1, v10, 8\n"
        "vextduwvlx v2, v2, v10, 8\n"
        "vextduwvlx v3, v3, v10, 8\n"
        "vextduwvlx v0, v4, v0, 9\n"
        "vextduwvlx v1, v5, v1, 9\n"
        "vextduwvlx v2, v6, v2, 9\n"
        "vextduwvlx v3, v7, v3, 9\n"
        "xsaddsp vs0, vs4, vs12\n"
        "xsaddsp vs1, vs36, vs12\n"
        "xsmaddasp vs2, vs4, vs12\n"
        "xsmaddasp vs3, vs36, vs12\n"
        "addi 4, 4, 16\n"
        "bdnz 1b\n"
        : : : "memory");
}

void FilterBlock2dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, int *HFilter,
                         int *VFilter) {
    __asm__ volatile(
        "lis 9, FData.84_804BE56C\n"
        "li 11, f64\n"
        "addi 9, 9, FData.84_804BE56C\n"
        "li 0, 9\n"
        "lq 8, -24576(6)\n"
        "psq_l f9, 7(6), 1, 2\n"
        "lq 10, -32768(11)\n"
        "mtctr 0\n"
        "addi 6, 3, 1\n"
        "vmhaddshs v10, v10, v10, v16\n"
        "1:\n"
        "lq 0, 8192(3)\n"
        "psq_l f1, 2(3), 0, 2\n"
        "lq 2, 8192(3)\n"
        "psq_l f3, 6(3), 0, 2\n"
        "add 3, 3, 5\n"
        "lq 4, 8192(6)\n"
        "psq_l f5, 2(6), 0, 2\n"
        "psq_l f6, 4(6), 0, 2\n"
        "psq_l f7, 6(6), 0, 2\n"
        "add 6, 6, 5\n"
        "vextduwvlx v0, v0, v10, 8\n"
        "vextduwvlx v1, v1, v10, 8\n"
        "vextduwvlx v2, v2, v10, 8\n"
        "vextduwvlx v3, v3, v10, 8\n"
        "vextduwvlx v0, v4, v0, 9\n"
        "vextduwvlx v1, v5, v1, 9\n"
        "vextduwvlx v2, v6, v2, 9\n"
        "vextduwvlx v3, v7, v3, 9\n"
        "xsaddsp vs0, vs9, vs12\n"
        "xsaddsp vs1, vs41, vs12\n"
        "xsmaddasp vs2, vs9, vs12\n"
        "xsmaddasp vs3, vs41, vs12\n"
        "addi 9, 9, 16\n"
        "bdnz 1b\n"
        "li 0, 8\n"
        "addi 3, 9, -144\n"
        "lq 8, -24576(7)\n"
        "psq_l f9, 7(7), 1, 2\n"
        "mtctr 0\n"
        "addi 6, 3, 16\n"
        "2:\n"
        "lq 0, 12288(3)\n"
        "psq_l f1, 4(3), 0, 3\n"
        "lq 2, 12288(3)\n"
        "psq_l f3, 12(3), 0, 3\n"
        "addi 3, 3, 16\n"
        "lq 4, 12288(6)\n"
        "psq_l f5, 4(6), 0, 3\n"
        "psq_l f6, 8(6), 0, 3\n"
        "psq_l f7, 12(6), 0, 3\n"
        "addi 6, 6, 16\n"
        "vextduwvlx v0, v0, v10, 8\n"
        "vextduwvlx v1, v1, v10, 8\n"
        "vextduwvlx v2, v2, v10, 8\n"
        "vextduwvlx v3, v3, v10, 8\n"
        "vextduwvlx v0, v4, v0, 9\n"
        "vextduwvlx v1, v5, v1, 9\n"
        "vextduwvlx v2, v6, v2, 9\n"
        "vextduwvlx v3, v7, v3, 9\n"
        "xsaddsp vs0, vs4, vs12\n"
        "xsaddsp vs1, vs36, vs12\n"
        "xsmaddasp vs2, vs4, vs12\n"
        "xsmaddasp vs3, vs36, vs12\n"
        "addi 4, 4, 16\n"
        "bdnz 2b\n"
        : : : "memory");
}

inline void ScalarReconIntra_GC(short *TmpDataBuffer, unsigned char *ReconPtr,
                         unsigned short *ChangePtr, unsigned int LineStep) {
    __asm__ volatile(
        "li 9, f128\n"
        "li 0, 8\n"
        "lq 4, -32768(9)\n"
        "mtctr 0\n"
        "vmhaddshs v4, v4, v4, v16\n"
        "1:\n"
        "lq 0, 20480(5)\n"
        "psq_l f1, 4(5), 0, 5\n"
        "lq 2, 20480(5)\n"
        "psq_l f3, 12(5), 0, 5\n"
        "addi 5, 5, 16\n"
        "vsel v0, v0, v4, v0\n"
        "vsel v1, v1, v4, v0\n"
        "vsel v2, v2, v4, v0\n"
        "vsel v3, v3, v4, v0\n"
        "xsaddsp vs0, vs4, vs4\n"
        "xsaddsp vs1, vs4, vs36\n"
        "xsaddsp vs2, vs36, vs4\n"
        "xsaddsp vs3, vs36, vs36\n"
        "add 4, 4, 6\n"
        "bdnz 1b\n"
        : "=m"(*ChangePtr) : "r"(TmpDataBuffer), "r"(ReconPtr),
            "r"(LineStep) : "memory");
}

inline void ScalarReconInter_GC(short *TmpDataBuffer, unsigned char *ReconPtr,
                         unsigned char *RefPtr, short *ChangePtr,
                         unsigned int LineStep) {
    __asm__ volatile(
        "li 0, 8\n"
        "mtctr 0\n"
        "1:\n"
        "lq 0, 8192(5)\n"
        "psq_l f1, 2(5), 0, 2\n"
        "lq 2, 8192(5)\n"
        "psq_l f3, 6(5), 0, 2\n"
        "add 5, 5, 7\n"
        "lq 4, 20480(6)\n"
        "psq_l f5, 4(6), 0, 5\n"
        "psq_l f6, 8(6), 0, 5\n"
        "psq_l f7, 12(6), 0, 5\n"
        "addi 6, 6, 16\n"
        "vsel v0, v0, v4, v0\n"
        "vsel v1, v1, v5, v0\n"
        "vsel v2, v2, v6, v0\n"
        "vsel v3, v3, v7, v0\n"
        "xsaddsp vs0, vs4, vs4\n"
        "xsaddsp vs1, vs4, vs36\n"
        "xsaddsp vs2, vs36, vs4\n"
        "xsaddsp vs3, vs36, vs36\n"
        "add 4, 4, 7\n"
        "bdnz 1b\n"
        : "=m"(*ChangePtr) : "r"(TmpDataBuffer), "r"(ReconPtr),
            "r"(RefPtr), "r"(LineStep) : "memory");
}

inline void ReconBlock_GC(short *SrcBlock, short *ReconRefPtr, unsigned char *DestBlock,
                   unsigned int LineStep) {
    __asm__ volatile(
        "li 0, 8\n"
        "mtctr 0\n"
        "1:\n"
        "lq 0, 20480(3)\n"
        "psq_l f1, 4(3), 0, 5\n"
        "lq 2, 20480(3)\n"
        "psq_l f3, 12(3), 0, 5\n"
        "addi 3, 3, 16\n"
        "psq_l f4, 0(4), 0, 5\n"
        "psq_l f5, 4(4), 0, 5\n"
        "lq 6, 20480(4)\n"
        "psq_l f7, 12(4), 0, 5\n"
        "addi 4, 4, 16\n"
        "vsel v0, v0, v4, v0\n"
        "vsel v1, v1, v5, v0\n"
        "vsel v2, v2, v6, v0\n"
        "vsel v3, v3, v7, v0\n"
        "xsaddsp vs0, vs5, vs4\n"
        "xsaddsp vs1, vs5, vs36\n"
        "xsaddsp vs2, vs37, vs4\n"
        "xsaddsp vs3, vs37, vs36\n"
        "add 5, 5, 6\n"
        "bdnz 1b\n"
        : "=m"(*DestBlock) : "r"(SrcBlock), "r"(ReconRefPtr),
            "r"(LineStep) : "memory");
}

void IDct1_GC(short *InputData, short *QuantMatrix, short *OutputData) {
    unsigned int *dest = (unsigned int *)OutputData;
    unsigned int out;
    int i;

    out = (InputData[0] * QuantMatrix[0] + 15) >> 5;
    out = (out << 16) | (out & 0xffff);
    for (i = 8; i; i--) {
        dest[0] = out;
        dest[1] = out;
        dest[2] = out;
        dest[3] = out;
        dest += 4;
    }
}
