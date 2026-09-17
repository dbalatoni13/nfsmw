#ifndef VP6_RECON_INL
#define VP6_RECON_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern void VP6_PredictFiltered(struct PB_INSTANCE *, unsigned char *, int, int);
extern unsigned int Var16Point(unsigned char *, int);
extern void (*FilterBlock)(unsigned char *, unsigned char *, unsigned short *, unsigned int, int, int, int);
extern void (*UnpackBlock)(unsigned char *, short *, unsigned int);
extern int abs(int);

void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *OutputPtr, BLOCK_POSITION bp) {
    unsigned char *SrcPtr;
    unsigned char *TempBuffer;
    unsigned int TempPtr1;
    unsigned int TempPtr2;
    int ModX;
    int ModY;
    unsigned int IVar;
    unsigned int BicMvSizeLimit;
    unsigned int Stride;
    unsigned int MvShift;
    unsigned int MvModMask;

    MvShift = pbi->mbi.MvShift;
    MvModMask = pbi->mbi.MvModMask;
    SrcPtr = pbi->LastFrameRecon;
    if (VP6_Mode2Frame[pbi->mbi.Mode] == 2) {
        SrcPtr = pbi->GoldenFrame;
    }
    if (!pbi->VpProfile || !pbi->UseLoopFilter) {
        int mVx;
        int mVy;
        int mx;
        int my;

        mx = pbi->mbi.Mv[bp].x;
        my = pbi->mbi.Mv[bp].y;
        ModX = pbi->mbi.Mv[bp].x & MvModMask;
        ModY = pbi->mbi.Mv[bp].y & MvModMask;
        mx += (int)MvModMask & (mx >> 31);
        mVx = mx >> MvShift;
        my += (int)MvModMask & (my >> 31);
        mVy = my >> MvShift;
        TempPtr1 = pbi->mbi.FrameReconStride * mVy + mVx;
        TempBuffer = SrcPtr + pbi->mbi.Recon + TempPtr1;
        Stride = pbi->mbi.CurrentReconStride;
        TempPtr2 = 0;
        TempPtr1 = 0;
    } else {
        VP6_PredictFiltered(pbi, SrcPtr + pbi->mbi.Recon, pbi->mbi.Mv[bp].x, pbi->mbi.Mv[bp].y);
        TempBuffer = pbi->LoopFilteredBlock;
        Stride = 16;
        TempPtr2 = 34;
        TempPtr1 = 34;
        ModX = pbi->mbi.Mv[bp].x & pbi->mbi.MvModMask;
        ModY = pbi->mbi.Mv[bp].y & pbi->mbi.MvModMask;
    }
    if (ModX != 0) {
        if (pbi->mbi.Mv[bp].x > 0) {
            TempPtr2++;
        } else {
            TempPtr2--;
        }
    }
    if (ModY != 0) {
        if (pbi->mbi.Mv[bp].y <= 0) {
            TempPtr2 -= Stride;
        } else {
            TempPtr2 += Stride;
        }
    }
    if (TempPtr1 != TempPtr2) {
        if (bp < U_BLOCK) {
            ModX <<= 1;
            ModY <<= 1;
        }
        if (bp < U_BLOCK && pbi->VpProfile != 0) {
            if (pbi->PredictionFilterMode == 2) {
                BicMvSizeLimit = pbi->PredictionFilterMvSizeThresh != 0
                    ? 4 << (pbi->PredictionFilterMvSizeThresh - 1) : 128;
                if (pbi->PredictionFilterMvSizeThresh == 0 ||
                    ((unsigned int)abs(pbi->mbi.Mv[bp].x) <= BicMvSizeLimit &&
                     (unsigned int)abs(pbi->mbi.Mv[bp].y) <= BicMvSizeLimit)) {
                    if (pbi->PredictionFilterVarThresh != 0) {
                        IVar = Var16Point(TempBuffer + TempPtr1, Stride);
                        FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                    (unsigned short *)OutputPtr, Stride, ModX, ModY,
                                    IVar >= pbi->PredictionFilterVarThresh);
                    } else {
                        FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                    (unsigned short *)OutputPtr, Stride, ModX, ModY, 1);
                    }
                } else {
                    FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                (unsigned short *)OutputPtr, Stride, ModX, ModY, 0);
                }
            } else {
                FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                            (unsigned short *)OutputPtr, Stride, ModX, ModY,
                            pbi->PredictionFilterMode == 1);
            }
        } else {
            FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                        (unsigned short *)OutputPtr, Stride, ModX, ModY, 0);
        }
    } else {
        UnpackBlock(TempBuffer + TempPtr1, OutputPtr, Stride);
    }
}

#endif
