typedef struct CONFIG_TYPE {
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

extern unsigned char LimitVal_VP31[0x300];
extern void *memset(void *dest, int value, unsigned int size);

int *SetupBoundingValueArray_Generic(POSTPROC_INSTANCE *ppi, int FLimit) {
    int i;
    int *BoundingValuePtr;

    BoundingValuePtr = ppi->FiltBoundingValue;
    BoundingValuePtr += 256;
    memset(BoundingValuePtr, 0, 0x800);

    for (i = 0; i < FLimit; i++) {
        BoundingValuePtr[-FLimit + i] = -FLimit + i;
        BoundingValuePtr[-i] = 0;
        BoundingValuePtr[i] = i;
        BoundingValuePtr[FLimit - i] = FLimit - i;
    }

    return BoundingValuePtr;
}

void FilterHoriz_Generic(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr, int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable;

    LimitTable = LimitVal_VP31 + 0x100;

    j = 8;
    do {
        FiltVal = PixelPtr[0] - 3 * PixelPtr[1] + 3 * PixelPtr[2] - PixelPtr[3];
        FiltVal = (FiltVal + 4) >> 3;
        FiltVal = BoundingValuePtr[FiltVal];
        PixelPtr[1] = LimitTable[PixelPtr[1] + FiltVal];
        PixelPtr[2] = LimitTable[PixelPtr[2] - FiltVal];
        PixelPtr += LineLength;
    } while (--j);
}

void FilterVert_Generic(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr, int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable;

    LimitTable = LimitVal_VP31 + 0x100;

    j = 8;
    do {
        FiltVal = *(PixelPtr - 2 * LineLength) - 3 * *(PixelPtr - LineLength) +
                  3 * PixelPtr[0] - PixelPtr[LineLength];
        FiltVal = (FiltVal + 4) >> 3;
        FiltVal = BoundingValuePtr[FiltVal];
        PixelPtr[-LineLength] = LimitTable[PixelPtr[-LineLength] + FiltVal];
        PixelPtr[0] = LimitTable[PixelPtr[0] - FiltVal];
        PixelPtr++;
    } while (--j);
}
