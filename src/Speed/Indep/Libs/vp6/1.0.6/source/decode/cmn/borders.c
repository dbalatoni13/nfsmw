#include "../../../include/vp6_pbdll.h"

extern void *memcpy(void *dest, const void *source, unsigned int size);
extern void *memset(void *dest, int value, unsigned int size);

typedef struct {
    int Vp3VersionNo;
    int FrameType;
    int PostProcessingLevel;
    int FrameQIndex;
    unsigned char *LastFrameRecon;
    unsigned char *PostProcessBuffer;
    unsigned char *FragInfo;
    unsigned int FragInfoElementSize;
    unsigned int FragInfoCodedMask;
    int *FragQIndex;
    int *FragmentVariances;
    unsigned char *FragDeblockingFlag;
    int *BoundingValuePtr;
    int *FiltBoundingValue;
    int *DeblockValuePtr;
    int *DeblockBoundingValue;
    struct CONFIG_TYPE Configuration;
    unsigned int ReconYDataOffset;
    unsigned int ReconUDataOffset;
    unsigned int ReconVDataOffset;
    unsigned int YPlaneFragments;
    unsigned int UVPlaneFragments;
    unsigned int UnitFragments;
    unsigned int HFragments;
    unsigned int VFragments;
    int YStride;
    int UVStride;
    int *FiltBoundingValueAlloc;
    int *DeblockBoundingValueAlloc;
    int *FragQIndexAlloc;
    int *FragmentVariancesAlloc;
    unsigned char *FragDeblockingFlagAlloc;
    unsigned int MVBorder;
    unsigned char *IntermediateBufferAlloc;
    unsigned char *IntermediateBuffer;
    unsigned int DeInterlaceMode;
    unsigned int AddNoiseMode;
} POSTPROC_INSTANCE;

void UpdateUMVBorder(POSTPROC_INSTANCE *pbi, unsigned char *DestReconPtr) {
    int i;
    int PlaneHeight;
    unsigned char *SrcPtr1;
    unsigned char *SrcPtr2;
    unsigned char *DestPtr1;
    unsigned char *DestPtr2;
    unsigned int Border;
    int PlaneStride;

    Border = pbi->MVBorder;
    PlaneStride = pbi->YStride;

    PlaneStride = pbi->YStride;
    PlaneHeight = pbi->VFragments * 8;

    SrcPtr1 = DestReconPtr + pbi->ReconYDataOffset;
    SrcPtr2 = SrcPtr1 + 8 * pbi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border;
    DestPtr2 = SrcPtr2 + 1;
    for (i = 0; i < PlaneHeight; i++) {
            memset(DestPtr1, *SrcPtr1, Border);
            memset(DestPtr2, *SrcPtr2, Border);
            SrcPtr1 += PlaneStride;
            SrcPtr2 += PlaneStride;
            DestPtr1 += PlaneStride;
            DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + Border * PlaneStride;
    SrcPtr2 = SrcPtr1 + (pbi->VFragments * 8 * PlaneStride) - PlaneStride;
    DestPtr1 = DestReconPtr;
    DestPtr2 = SrcPtr2 + PlaneStride;
    for (i = 0; i < (int)Border; i++) {
            memcpy(DestPtr1, SrcPtr1, PlaneStride);
            memcpy(DestPtr2, SrcPtr2, PlaneStride);
            DestPtr1 += PlaneStride;
            DestPtr2 += PlaneStride;
    }

    PlaneStride = pbi->UVStride;
    PlaneHeight = pbi->VFragments * 4;

    SrcPtr1 = DestReconPtr + pbi->ReconUDataOffset;
    SrcPtr2 = SrcPtr1 + 4 * pbi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border / 2;
    DestPtr2 = SrcPtr2 + 1;
    for (i = 0; i < PlaneHeight; i++) {
            memset(DestPtr1, *SrcPtr1, Border / 2);
            memset(DestPtr2, *SrcPtr2, Border / 2);
            SrcPtr1 += PlaneStride;
            DestPtr1 += PlaneStride;
            SrcPtr2 += PlaneStride;
            DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + pbi->ReconUDataOffset - Border / 2;
    SrcPtr2 = SrcPtr1 + (pbi->VFragments * 4 * PlaneStride) - PlaneStride;
    DestPtr1 = SrcPtr1 - Border / 2 * PlaneStride;
    DestPtr2 = SrcPtr2 + PlaneStride;
    for (i = 0; i < (int)(Border / 2); i++) {
            memcpy(DestPtr1, SrcPtr1, PlaneStride);
            memcpy(DestPtr2, SrcPtr2, PlaneStride);
            DestPtr1 += PlaneStride;
            DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + pbi->ReconVDataOffset;
    SrcPtr2 = SrcPtr1 + 4 * pbi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border / 2;
    DestPtr2 = SrcPtr2 + 1;
    for (i = 0; i < PlaneHeight; i++) {
            memset(DestPtr1, *SrcPtr1, Border / 2);
            memset(DestPtr2, *SrcPtr2, Border / 2);
            SrcPtr1 += PlaneStride;
            DestPtr1 += PlaneStride;
            SrcPtr2 += PlaneStride;
            DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + pbi->ReconVDataOffset - Border / 2;
    SrcPtr2 = SrcPtr1 + (pbi->VFragments * 4 * PlaneStride) - PlaneStride;
    DestPtr1 = SrcPtr1 - Border / 2 * PlaneStride;
    DestPtr2 = SrcPtr2 + PlaneStride;
    for (i = 0; i < (int)(Border / 2); i++) {
            memcpy(DestPtr1, SrcPtr1, PlaneStride);
            memcpy(DestPtr2, SrcPtr2, PlaneStride);
            DestPtr1 += PlaneStride;
            DestPtr2 += PlaneStride;
    }
}

typedef struct {
    int YWidth;
    int YHeight;
    int YStride;
    int UVWidth;
    int UVHeight;
    int UVStride;
    unsigned char *YBuffer;
    unsigned char *UBuffer;
    unsigned char *VBuffer;
} YUV_BUFFER_CONFIG;

void CopyFrame(POSTPROC_INSTANCE *pbi, YUV_BUFFER_CONFIG *src, unsigned char *DestReconPtr) {
    int i;
    unsigned char *SrcPtr;
    unsigned char *DestPtr;

    SrcPtr = src->YBuffer;
    DestPtr = DestReconPtr + pbi->ReconYDataOffset;
    i = 0;
    while (i < src->YHeight) {
        memcpy(DestPtr, SrcPtr, src->YWidth);
        i++;
        SrcPtr += src->YStride;
        DestPtr += pbi->YStride;
    }

    SrcPtr = src->UBuffer;
    DestPtr = DestReconPtr + pbi->ReconUDataOffset;
    i = 0;
    while (i < src->UVHeight) {
        memcpy(DestPtr, SrcPtr, src->UVWidth);
        i++;
        SrcPtr += src->UVStride;
        DestPtr += pbi->UVStride;
    }

    SrcPtr = src->VBuffer;
    DestPtr = DestReconPtr + pbi->ReconVDataOffset;
    i = 0;
    while (i < src->UVHeight) {
        memcpy(DestPtr, SrcPtr, src->UVWidth);
        i++;
        SrcPtr += src->UVStride;
        DestPtr += pbi->UVStride;
    }

    UpdateUMVBorder(pbi, DestReconPtr);
}
