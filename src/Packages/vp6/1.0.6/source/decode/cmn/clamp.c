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

void ClampLevels_C(POSTPROC_INSTANCE *pbi, int BlackClamp, int WhiteClamp, unsigned char *Src, unsigned char *Dst) {
    int i;
    int row;
    int col;
    unsigned char clamped[256];
    int width;
    int height;
    unsigned char *SrcPtr, *DestPtr;
    unsigned int LineLength;

    width = pbi->HFragments << 3;
    height = pbi->VFragments << 3;
    LineLength = pbi->YStride;
    SrcPtr = Src + pbi->ReconYDataOffset;
    DestPtr = Dst + pbi->ReconYDataOffset;

    for (i = 0; i < 256; i++) {
        clamped[i] = i;
        if (i < BlackClamp) {
            clamped[i] = BlackClamp;
        }
        if (i > 255 - WhiteClamp) {
            clamped[i] = ~WhiteClamp;
        }
    }

    row = 0;
    while (row < height) {
        for (col = 0; col < width; col++) {
            *(SrcPtr + col) = clamped[*(DestPtr + col)];
        }
        SrcPtr = &SrcPtr[LineLength];
        DestPtr = &DestPtr[LineLength];
        row++;
    }
}
