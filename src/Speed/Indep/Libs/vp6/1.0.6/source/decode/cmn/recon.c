#include "../../../include/vp6_pbdll.h"

extern void (*Copy12x12)(const unsigned char *, unsigned char *, unsigned int, unsigned int);
extern void (*FilteringHoriz_12)(unsigned int, unsigned char *, int);
extern void (*FilteringVert_12)(unsigned int, unsigned char *, int);

unsigned int Var16Point(unsigned char *DataPtr, int SourceStride) {
    unsigned int i;
    unsigned int XSum = 0;
    unsigned int XXSum = 0;
    unsigned char *DiffPtr;

    DiffPtr = DataPtr;

    for (i = 4; i; i--) {
        XSum += DiffPtr[0];
        XXSum += DiffPtr[0] * DiffPtr[0];
        XSum += DiffPtr[2];
        XXSum += DiffPtr[2] * DiffPtr[2];
        XSum += DiffPtr[4];
        XXSum += DiffPtr[4] * DiffPtr[4];
        XSum += DiffPtr[6];
        XXSum += DiffPtr[6] * DiffPtr[6];

        DiffPtr += SourceStride * 2;
    }

    return ((XXSum << 4) - XSum * XSum) >> 8;
}

void VP6_PredictFiltered(struct PB_INSTANCE *pbi, unsigned char *SrcPtr, int mx, int my) {
    int mVx;
    int mVy;
    int ReconIndex;
    /* Preserve the anonymous macroblock-state type from PB_INSTANCE. */
    __typeof__(pbi->mbi) *mbi;
    unsigned char *TempBuffer;
    int BoundaryX;
    int BoundaryY;

    mbi = &pbi->mbi;
    TempBuffer = pbi->LoopFilteredBlock;
    mVx = mx > 0 ? mx >> mbi->MvShift : -((-mx) >> mbi->MvShift);
    mVy = my > 0 ? my >> mbi->MvShift : -((-my) >> mbi->MvShift);
    ReconIndex = mbi->FrameReconStride * mVy + mVx - 2 * mbi->CurrentReconStride - 2;
    Copy12x12(SrcPtr + ReconIndex, TempBuffer, mbi->CurrentReconStride, 16);
    BoundaryX = (8 - (mVx & 7)) & 7;
    BoundaryY = (8 - (mVy & 7)) & 7;
    if (BoundaryX != 0) {
        FilteringHoriz_12(pbi->quantizer->FrameQIndex, TempBuffer + (BoundaryX + 2), 16);
    }
    if (BoundaryY != 0) {
        FilteringVert_12(pbi->quantizer->FrameQIndex, TempBuffer + (BoundaryY * 16 + 32), 16);
    }
}
