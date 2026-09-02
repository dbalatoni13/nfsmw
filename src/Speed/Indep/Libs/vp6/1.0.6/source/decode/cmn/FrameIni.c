#include "../../../include/vp6_pbdll.h"

extern void *duck_malloc(unsigned int blocksize, int type);
extern void duck_free(void *block);
extern void *memset(void *dest, int value, unsigned int size);
extern void VP6_SetPbParam(struct PB_INSTANCE *pbi, int Command,
                           unsigned int Parameter);
extern void ChangePostProcConfiguration(struct POSTPROC_INSTANCE *ppi,
                                        struct CONFIG_TYPE *ConfigurationInit);

static const int NearMacroBlocks[12][2] = {
    {-1, 0}, {0, -1}, {-1, -1}, {-1, 1},
    {-2, 0}, {0, -2}, {-1, -2}, {-2, -1},
    {-2, 1}, {-1, 2}, {-2, -2}, {-2, 2}
};

void VP6_DeleteFragmentInfo(struct PB_INSTANCE *pbi) {
    if (pbi->mbi.CoeffsAlloc != 0) {
        duck_free(pbi->mbi.CoeffsAlloc);
    }
    pbi->mbi.CoeffsAlloc = 0;
    pbi->mbi.Coeffs = 0;
    if (pbi->FragInfoAlloc != 0) {
        duck_free(pbi->FragInfoAlloc);
    }
    pbi->FragInfoAlloc = 0;
    pbi->FragInfo = 0;
    if (pbi->fc.AboveYAlloc != 0) {
        duck_free(pbi->fc.AboveYAlloc);
    }
    pbi->fc.AboveYAlloc = 0;
    pbi->fc.AboveY = 0;
    if (pbi->fc.AboveUAlloc != 0) {
        duck_free(pbi->fc.AboveUAlloc);
    }
    pbi->fc.AboveUAlloc = 0;
    pbi->fc.AboveU = 0;
    if (pbi->fc.AboveVAlloc != 0) {
        duck_free(pbi->fc.AboveVAlloc);
    }
    pbi->fc.AboveVAlloc = 0;
    pbi->fc.AboveV = 0;
    if (pbi->MBInterlacedAlloc != 0) {
        duck_free(pbi->MBInterlacedAlloc);
    }
    pbi->MBInterlacedAlloc = 0;
    pbi->MBInterlaced = 0;
    if (pbi->MBMotionVectorAlloc != 0) {
        duck_free(pbi->MBMotionVectorAlloc);
    }
    pbi->MBMotionVectorAlloc = 0;
    pbi->MBMotionVector = 0;
    if (pbi->predictionModeAlloc != 0) {
        duck_free(pbi->predictionModeAlloc);
    }
    pbi->predictionModeAlloc = 0;
    pbi->predictionMode = 0;
}

int VP6_AllocateFragmentInfo(struct PB_INSTANCE *pbi) {
    VP6_DeleteFragmentInfo(pbi);

    pbi->mbi.CoeffsAlloc = (short (*)[64])duck_malloc(0x320, 0);
    if (pbi->mbi.CoeffsAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->mbi.Coeffs = (short (*)[64])(((unsigned int)pbi->mbi.CoeffsAlloc + 0x1f) & ~0x1f);

    pbi->fc.AboveYAlloc = (void *)duck_malloc(pbi->HFragments * 0x10 + 0xa0, 0);
    if (pbi->fc.AboveYAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveY = (void *)(((unsigned int)pbi->fc.AboveYAlloc + 0x1f) & ~0x1f);

    pbi->fc.AboveUAlloc = (void *)duck_malloc((pbi->HFragments >> 1) * 0x10 + 0xa0, 0);
    if (pbi->fc.AboveUAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveU = (void *)(((unsigned int)pbi->fc.AboveUAlloc + 0x1f) & ~0x1f);

    pbi->fc.AboveVAlloc = (void *)duck_malloc((pbi->HFragments >> 1) * 0x10 + 0xa0, 0);
    if (pbi->fc.AboveVAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveV = (void *)(((unsigned int)pbi->fc.AboveVAlloc + 0x1f) & ~0x1f);

    pbi->MBInterlacedAlloc = (char *)duck_malloc(pbi->MacroBlocks + 0x20, 0);
    if (pbi->MBInterlacedAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->MBInterlaced = (char *)(((unsigned int)pbi->MBInterlacedAlloc + 0x1f) & ~0x1f);

    pbi->predictionModeAlloc = (char *)duck_malloc(pbi->MacroBlocks + 0x20, 0);
    if (pbi->predictionModeAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->predictionMode = (char *)(((unsigned int)pbi->predictionModeAlloc + 0x1f) & ~0x1f);

    pbi->MBMotionVectorAlloc = (void *)duck_malloc(pbi->MacroBlocks * 4 + 0x20, 0);
    if (pbi->MBMotionVectorAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->MBMotionVector = (void *)(((unsigned int)pbi->MBMotionVectorAlloc + 0x1f) & ~0x1f);

    pbi->FragInfoAlloc = (void *)duck_malloc(pbi->UnitFragments * 4 + 0x20, 0);
    if (pbi->FragInfoAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->FragInfo = (void *)(((unsigned int)pbi->FragInfoAlloc + 0x1f) & ~0x1f);
    return 1;
}

void VP6_DeleteFrameInfo(struct PB_INSTANCE *pbi) {
    if (pbi->ThisFrameReconAlloc != 0) {
        duck_free(pbi->ThisFrameReconAlloc);
    }
    if (pbi->GoldenFrameAlloc != 0) {
        duck_free(pbi->GoldenFrameAlloc);
    }
    if (pbi->LastFrameReconAlloc != 0) {
        duck_free(pbi->LastFrameReconAlloc);
    }
    if (pbi->PostProcessBufferAlloc != 0) {
        duck_free(pbi->PostProcessBufferAlloc);
    }

    pbi->ThisFrameReconAlloc = 0;
    pbi->GoldenFrameAlloc = 0;
    pbi->LastFrameReconAlloc = 0;
    pbi->ThisFrameRecon = 0;
    pbi->GoldenFrame = 0;
    pbi->LastFrameRecon = 0;
    pbi->PostProcessBufferAlloc = 0;
}

int VP6_AllocateFrameInfo(struct PB_INSTANCE *pbi, unsigned int FrameSize) {
    unsigned int size = FrameSize + 0x20;

    VP6_DeleteFrameInfo(pbi);

    pbi->ThisFrameReconAlloc = (unsigned char *)duck_malloc(pbi->Configuration.YStride + size, 0);
    if (pbi->ThisFrameReconAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->ThisFrameRecon = (unsigned char *)(((unsigned int)pbi->ThisFrameReconAlloc + 0x1f) & ~0x1f);

    pbi->GoldenFrameAlloc = (unsigned char *)duck_malloc(pbi->Configuration.YStride + size, 0);
    if (pbi->GoldenFrameAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->GoldenFrame = (unsigned char *)(((unsigned int)pbi->GoldenFrameAlloc + 0x1f) & ~0x1f);

    pbi->LastFrameReconAlloc = (unsigned char *)duck_malloc(pbi->Configuration.YStride + size, 0);
    if (pbi->LastFrameReconAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->LastFrameRecon = (unsigned char *)(((unsigned int)pbi->LastFrameReconAlloc + 0x1f) & ~0x1f);
    return 1;
}

int VP6_InitFrameDetails(struct PB_INSTANCE *pbi) {
    unsigned int i;
    int FrameSize;

    if (pbi->CPUFree != 0) {
        VP6_SetPbParam(pbi, 1, pbi->CPUFree);
    }

    pbi->Configuration.YStride = pbi->Configuration.VideoFrameWidth + 0x60;
    pbi->Configuration.UVStride = pbi->Configuration.YStride / 2;
    pbi->HFragments = pbi->Configuration.VideoFrameWidth / pbi->Configuration.HFragPixels;
    pbi->VFragments = pbi->Configuration.VideoFrameHeight / pbi->Configuration.VFragPixels;
    pbi->YPlaneFragments = pbi->HFragments * pbi->VFragments;
    pbi->UnitFragments = (pbi->YPlaneFragments * 3) / 2;
    pbi->UVPlaneFragments = pbi->YPlaneFragments / 4;
    pbi->YPlaneSize = pbi->Configuration.VideoFrameWidth * pbi->Configuration.VideoFrameHeight;
    pbi->UVPlaneSize = pbi->YPlaneSize / 4;
    pbi->ReconYPlaneSize = pbi->Configuration.YStride *
                           (pbi->Configuration.VideoFrameHeight + 0x60);
    pbi->ReconUVPlaneSize = pbi->ReconYPlaneSize / 4;
    pbi->UDataOffset = pbi->YPlaneSize;
    pbi->VDataOffset = pbi->YPlaneSize + pbi->UVPlaneSize;
    pbi->ReconUDataOffset = pbi->ReconYPlaneSize;
    pbi->ReconVDataOffset = pbi->ReconYPlaneSize + pbi->ReconUVPlaneSize;
    pbi->YDataOffset = 0;
    pbi->ReconYDataOffset = 0;

    pbi->MBRows = (pbi->Configuration.VideoFrameHeight >> 4) + 6 +
                  ((pbi->Configuration.VideoFrameHeight & 15) ? 1 : 0);
    pbi->MBCols = (pbi->Configuration.VideoFrameWidth >> 4) + 6 +
                  ((pbi->Configuration.VideoFrameWidth & 15) ? 1 : 0);
    pbi->MacroBlocks = pbi->MBRows * pbi->MBCols;

    FrameSize = pbi->ReconYPlaneSize + 2 * pbi->ReconUVPlaneSize;

    for (i = 0; i < 12; i++) {
        pbi->mvNearOffset[i] = NearMacroBlocks[i][0] * pbi->MBCols + NearMacroBlocks[i][1];
    }

    ChangePostProcConfiguration(pbi->postproc, &pbi->Configuration);
    if (VP6_AllocateFragmentInfo(pbi) == 0) {
        return 0;
    }
    if (VP6_AllocateFrameInfo(pbi, FrameSize) == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    if (pbi->ScaleBuffer == 0 && pbi->OutputWidth != 0 &&
        (pbi->Configuration.VideoFrameWidth != pbi->OutputWidth ||
         pbi->Configuration.VideoFrameHeight != pbi->OutputHeight)) {
        pbi->ScaleBufferAlloc = (unsigned char *)duck_malloc(
            ((pbi->OutputWidth + 32) * 3 * (pbi->OutputHeight + 32)) / 2 + 32, 0);
        pbi->ScaleBuffer = (unsigned char *)(((unsigned int)pbi->ScaleBufferAlloc + 0x1f) & ~0x1f);
    }
    return 1;
}

void VP6_InitialiseConfiguration(struct PB_INSTANCE *pbi) {
    pbi->Configuration.HFragPixels = 8;
    pbi->Configuration.VFragPixels = 8;
}
