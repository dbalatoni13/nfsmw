typedef int INT32;
typedef unsigned char UINT8;
typedef unsigned int UINT32;

typedef struct {
    unsigned int VideoFrameWidth;
    unsigned int VideoFrameHeight;
    int YStride;
    int UVStride;
    unsigned int HFragPixels;
    unsigned int VFragPixels;
    unsigned int HScale;
    unsigned int HRatio;
    unsigned int VScale;
    unsigned int VRatio;
    unsigned int ScalingMode;
    unsigned int Interlaced;
    unsigned int ExpandedFrameWidth;
    unsigned int ExpandedFrameHeight;
} CONFIG_TYPE;

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
    CONFIG_TYPE Configuration;
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

extern void *memcpy();
extern void *memset();

void UpdateUMVBorder(POSTPROC_INSTANCE *ppi, UINT8 *DestReconPtr) {
    INT32 i;
    INT32 PlaneHeight;
    UINT8 *SrcPtr1, *SrcPtr2;
    UINT8 *DestPtr1, *DestPtr2;
    UINT32 Border = ppi->MVBorder;
    INT32 PlaneStride = ppi->YStride;

    PlaneStride = ppi->YStride;
    PlaneHeight = ppi->VFragments * 8;

    SrcPtr1 = DestReconPtr + ppi->ReconYDataOffset;
    SrcPtr2 = SrcPtr1 + 8 * ppi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border;
    DestPtr2 = SrcPtr2 + 1;

    for (i = 0; i < PlaneHeight; i++) {
        memset(DestPtr1, SrcPtr1[0], Border);
        memset(DestPtr2, SrcPtr2[0], Border);
        SrcPtr1 += PlaneStride;
        SrcPtr2 += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + Border * PlaneStride;
    SrcPtr2 = SrcPtr1 + (ppi->VFragments * 8 * PlaneStride) - PlaneStride;
    DestPtr1 = DestReconPtr;
    DestPtr2 = SrcPtr2 + PlaneStride;

    for (i = 0; i < (INT32)Border; i++) {
        memcpy(DestPtr1, SrcPtr1, PlaneStride);
        memcpy(DestPtr2, SrcPtr2, PlaneStride);
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    PlaneStride = ppi->UVStride;
    PlaneHeight = ppi->VFragments * 4;

    SrcPtr1 = DestReconPtr + ppi->ReconUDataOffset;
    SrcPtr2 = SrcPtr1 + 4 * ppi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border / 2;
    DestPtr2 = SrcPtr2 + 1;

    for (i = 0; i < PlaneHeight; i++) {
        memset(DestPtr1, SrcPtr1[0], Border / 2);
        memset(DestPtr2, SrcPtr2[0], Border / 2);
        SrcPtr1 += PlaneStride;
        SrcPtr2 += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + ppi->ReconUDataOffset - Border / 2;
    SrcPtr2 = SrcPtr1 + (ppi->VFragments * 4 * PlaneStride) - PlaneStride;
    DestPtr1 = SrcPtr1 - Border / 2 * PlaneStride;
    DestPtr2 = SrcPtr2 + PlaneStride;

    for (i = 0; i < (INT32)(Border / 2); i++) {
        memcpy(DestPtr1, SrcPtr1, PlaneStride);
        memcpy(DestPtr2, SrcPtr2, PlaneStride);
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + ppi->ReconVDataOffset;
    SrcPtr2 = SrcPtr1 + 4 * ppi->HFragments - 1;
    DestPtr1 = SrcPtr1 - Border / 2;
    DestPtr2 = SrcPtr2 + 1;

    for (i = 0; i < PlaneHeight; i++) {
        memset(DestPtr1, SrcPtr1[0], Border / 2);
        memset(DestPtr2, SrcPtr2[0], Border / 2);
        SrcPtr1 += PlaneStride;
        SrcPtr2 += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    SrcPtr1 = DestReconPtr + ppi->ReconVDataOffset - Border / 2;
    SrcPtr2 = SrcPtr1 + (ppi->VFragments * 4 * PlaneStride) - PlaneStride;
    DestPtr1 = SrcPtr1 - Border / 2 * PlaneStride;
    DestPtr2 = SrcPtr2 + PlaneStride;

    for (i = 0; i < (INT32)(Border / 2); i++) {
        memcpy(DestPtr1, SrcPtr1, PlaneStride);
        memcpy(DestPtr2, SrcPtr2, PlaneStride);
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }
}

void CopyFrame(POSTPROC_INSTANCE *ppi, YUV_BUFFER_CONFIG *YuvBuffer, UINT8 *DestBuffer) {
    INT32 i;
    UINT8 *SrcPtr;
    UINT8 *DesPtr;

    DesPtr = DestBuffer + ppi->ReconYDataOffset;
    SrcPtr = YuvBuffer->YBuffer;

    for (i = 0; i < YuvBuffer->YHeight; i++) {
        memcpy(DesPtr, SrcPtr, YuvBuffer->YWidth);
        SrcPtr += YuvBuffer->YStride;
        DesPtr += ppi->YStride;
    }

    DesPtr = DestBuffer + ppi->ReconUDataOffset;
    SrcPtr = YuvBuffer->UBuffer;

    for (i = 0; i < YuvBuffer->UVHeight; i++) {
        memcpy(DesPtr, SrcPtr, YuvBuffer->UVWidth);
        SrcPtr += YuvBuffer->UVStride;
        DesPtr += ppi->UVStride;
    }

    DesPtr = DestBuffer + ppi->ReconVDataOffset;
    SrcPtr = YuvBuffer->VBuffer;

    for (i = 0; i < YuvBuffer->UVHeight; i++) {
        memcpy(DesPtr, SrcPtr, YuvBuffer->UVWidth);
        SrcPtr += YuvBuffer->UVStride;
        DesPtr += ppi->UVStride;
    }

    UpdateUMVBorder(ppi, DestBuffer);
}
