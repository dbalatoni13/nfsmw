typedef int INT32;
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

extern unsigned char LimitVal_VP31[768];

UINT32 LoopFilterLimitValuesVp4[64] = {
    30, 25, 20, 20, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,
    9,  9,  8,  8,  7,  7,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,
    4,  4,  4,  4,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,  1,
};

UINT32 LoopFilterLimitValuesVp5[64] = {
    14, 14, 13, 13, 12, 12, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,
    5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  2,
};

UINT32 LoopFilterLimitValuesVp6[64] = {
    14, 14, 13, 13, 12, 12, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,
    5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  2,
};

UINT32 *LoopFilterLimitValuesV2;

INT32 *SetupBoundingValueArray_Generic(POSTPROC_INSTANCE *ppi, INT32 FLimit) {
    INT32 i;
    INT32 *BoundingValuePtr = ppi->FiltBoundingValue + 256;

    memset(ppi->FiltBoundingValue, 0, 256 * 2 * sizeof(*ppi->FiltBoundingValue));

    for (i = 0; i < FLimit; i++) {
        BoundingValuePtr[-i - FLimit] = (-FLimit + i);
        BoundingValuePtr[-i] = -i;
        BoundingValuePtr[i] = i;
        BoundingValuePtr[i + FLimit] = FLimit - i;
    }

    return BoundingValuePtr;
}

void FilterHoriz_Generic(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr, INT32 LineLength,
                         INT32 *BoundingValuePtr) {
    INT32 j;
    INT32 FiltVal;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    for (j = 0; j < 8; j++) {
        FiltVal = (PixelPtr[0]) - (PixelPtr[1] * 3) + (PixelPtr[2] * 3) - (PixelPtr[3]);
        FiltVal = BoundingValuePtr[(FiltVal + 4) >> 3];
        PixelPtr[1] = LimitTable[PixelPtr[1] + FiltVal];
        PixelPtr[2] = LimitTable[PixelPtr[2] - FiltVal];
        PixelPtr += LineLength;
    }
}

void FilterVert_Generic(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr, INT32 LineLength,
                        INT32 *BoundingValuePtr) {
    INT32 j;
    INT32 FiltVal;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    for (j = 0; j < 8; j++) {
        FiltVal = (PixelPtr[-(2 * LineLength)]) - (PixelPtr[-LineLength] * 3) +
                  (PixelPtr[0] * 3) - (PixelPtr[LineLength]);
        FiltVal = BoundingValuePtr[(FiltVal + 4) >> 3];
        PixelPtr[-LineLength] = LimitTable[PixelPtr[-LineLength] + FiltVal];
        PixelPtr[0] = LimitTable[PixelPtr[0] - FiltVal];
        PixelPtr++;
    }
}

static __inline INT32 Bound(UINT32 FLimit, INT32 FiltVal) {
    INT32 Clamp;
    INT32 FiltSign;
    INT32 NewSign;

    Clamp = 2 * FLimit;

    FiltSign = FiltVal >> 31;
    FiltVal ^= FiltSign;
    FiltVal -= FiltSign;

    FiltVal *= (FiltVal < Clamp);

    FiltVal -= FLimit;

    NewSign = FiltVal >> 31;
    FiltVal ^= NewSign;
    FiltVal -= NewSign;

    FiltVal = FLimit - FiltVal;

    FiltVal += FiltSign;
    FiltVal ^= FiltSign;

    return FiltVal;
}

void FilteringHoriz_8_C(UINT32 QValue, unsigned char *Src, INT32 Pitch) {
    INT32 j;
    INT32 FiltVal;
    UINT32 FLimit;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    FLimit = LoopFilterLimitValuesV2[QValue];

    for (j = 0; j < 8; j++) {
        FiltVal = (Src[-2] - (Src[-1] * 3) + (Src[0] * 3) - Src[1] + 4) >> 3;
        FiltVal = Bound(FLimit, FiltVal);
        Src[-1] = LimitTable[Src[-1] + FiltVal];
        Src[0] = LimitTable[Src[0] - FiltVal];
        Src += Pitch;
    }
}

void FilteringVert_8_C(UINT32 QValue, unsigned char *Src, INT32 Pitch) {
    INT32 j;
    INT32 FiltVal;
    UINT32 FLimit;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    FLimit = LoopFilterLimitValuesV2[QValue];

    for (j = 0; j < 8; j++) {
        FiltVal = (Src[-(2 * Pitch)] - (Src[-Pitch] * 3) + (Src[0] * 3) -
                   Src[Pitch] + 4) >> 3;
        FiltVal = Bound(FLimit, FiltVal);
        Src[-Pitch] = LimitTable[Src[-Pitch] + FiltVal];
        Src[0] = LimitTable[Src[0] - FiltVal];
        Src++;
    }
}

void FilteringHoriz_12_C(UINT32 QValue, unsigned char *Src, INT32 Pitch) {
    INT32 j;
    INT32 FiltVal;
    UINT32 FLimit;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    FLimit = LoopFilterLimitValuesV2[QValue];

    for (j = 0; j < 12; j++) {
        FiltVal = (Src[-2] - (Src[-1] * 3) + (Src[0] * 3) - Src[1] + 4) >> 3;
        FiltVal = Bound(FLimit, FiltVal);
        Src[-1] = LimitTable[Src[-1] + FiltVal];
        Src[0] = LimitTable[Src[0] - FiltVal];
        Src += Pitch;
    }
}

void FilteringVert_12_C(UINT32 QValue, unsigned char *Src, INT32 Pitch) {
    INT32 j;
    INT32 FiltVal;
    UINT32 FLimit;
    unsigned char *LimitTable = &LimitVal_VP31[256];

    FLimit = LoopFilterLimitValuesV2[QValue];

    for (j = 0; j < 12; j++) {
        FiltVal = (Src[-(2 * Pitch)] - (Src[-Pitch] * 3) + (Src[0] * 3) -
                   Src[Pitch] + 4) >> 3;
        FiltVal = Bound(FLimit, FiltVal);
        Src[-Pitch] = LimitTable[Src[-Pitch] + FiltVal];
        Src[0] = LimitTable[Src[0] - FiltVal];
        Src++;
    }
}
