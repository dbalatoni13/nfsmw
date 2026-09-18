struct POSTPROC_INSTANCE;

typedef enum {
    CODE_INTER_NO_MV = 0,
    CODE_INTRA = 1,
    CODE_INTER_PLUS_MV = 2,
    CODE_INTER_NEAREST_MV = 3,
    CODE_INTER_NEAR_MV = 4,
    CODE_USING_GOLDEN = 5,
    CODE_GOLDEN_MV = 6,
    CODE_INTER_FOURMV = 7,
    CODE_GOLD_NEAREST_MV = 8,
    CODE_GOLD_NEAR_MV = 9,
    DO_NOT_CODE = 16,
} MODE_TYPE;

typedef struct {
    short x;
    short y;
} MV_TYPE;

typedef enum {
    TOP_LEFT_Y_BLOCK = 0,
    TOP_RIGHT_Y_BLOCK = 1,
    BOTTOM_LEFT_Y_BLOCK = 2,
    BOTTOM_RIGHT_Y_BLOCK = 3,
    U_BLOCK = 4,
    V_BLOCK = 5,
} BP_TYPE;

typedef struct {
    unsigned char Token;
    MODE_TYPE Mode;
    unsigned short Frame;
    short Dc;
    unsigned char unused[3];
} ABOVE_TYPE;

typedef struct {
    short (* CoeffsAlloc)[64];
    short (* Coeffs)[64];
    MODE_TYPE Mode;
    MODE_TYPE BlockMode[6];
    MV_TYPE Mv[6];
    MV_TYPE NearestInterMVect;
    MV_TYPE NearInterMVect;
    int NearestMvIndex;
    MV_TYPE NearestGoldMVect;
    MV_TYPE NearGoldMVect;
    int NearestGMvIndex;
    unsigned int MBrow;
    unsigned int MBcol;
    BP_TYPE bp;
    unsigned int Source;
    unsigned int SourceY;
    unsigned int SourceX;
    int CurrentSourceStride;
    unsigned int Recon;
    int CurrentReconStride;
    unsigned int Plane;
    int MvShift;
    int MvModMask;
    int FrameSourceStride;
    int FrameReconStride;
    unsigned int SourcePtr[6];
    unsigned int ReconPtr[6];
    ABOVE_TYPE * Above;
    ABOVE_TYPE * Left;
    short * LastDc;
    int Interlaced;
} MBI_TYPE;

typedef struct {
    ABOVE_TYPE LeftY[2];
    ABOVE_TYPE LeftU;
    ABOVE_TYPE LeftV;
    ABOVE_TYPE * AboveY;
    ABOVE_TYPE * AboveU;
    ABOVE_TYPE * AboveV;
    ABOVE_TYPE * AboveYAlloc;
    ABOVE_TYPE * AboveUAlloc;
    ABOVE_TYPE * AboveVAlloc;
    short LastDcY[4];
    short LastDcU[4];
    short LastDcV[4];
} FC_TYPE;

typedef struct {
    unsigned int FrameQIndex;
    unsigned int ThisFrameQuantizerValue;
    short round[8];
    short mult[8];
    short zbin[8];
    unsigned int LastQuantizerValue;
    unsigned int QThreshTable[64];
    unsigned int * transIndex;
    unsigned char quant_index[64];
    short * dequant_coeffs[2];
    short * dequant_coeffsAlloc[2];
    int QuantCoeffs[2][64];
    int QuantRound[2][64];
    int ZeroBinSize[2][64];
    int ZlrZbinCorrections[2][64];
} QUANTIZER_TYPE;

typedef struct {
    unsigned int FragCodingMode : 4;
    int MVectorX : 8;
    int MVectorY : 8;
} FRAGINFO_TYPE;

typedef struct {
    unsigned int lowvalue;
    unsigned int range;
    unsigned int value;
    int count;
    unsigned int pos;
    unsigned char * buffer;
    unsigned int MeasureCost;
    unsigned int BitCounter;
} BR_TYPE;

typedef struct {
    unsigned int value : 7;
    unsigned int selector : 1;
} _tokenorptr;

typedef union {
    char l;
    _tokenorptr left;
} LEFTUNION_TYPE;

typedef union {
    char r;
    _tokenorptr right;
} RIGHTUNION_TYPE;

typedef struct {
    unsigned char * buffer;
    unsigned int value;
    int bits_available;
    unsigned int pos;
} FRAME_HEADER;

typedef struct {
    unsigned int pos;
    int byte_bit_offset;
    unsigned int DataBlock;
    unsigned char * Buffer;
} RAW_BUFFER;

typedef struct {
    LEFTUNION_TYPE leftunion;
    RIGHTUNION_TYPE rightunion;
    unsigned char freq;
} _huffnode;

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
    int bitsinremainder;
    unsigned int remainder;
    const unsigned char * position;
} _BITREADER;

typedef struct {
    MBI_TYPE mbi;
    FC_TYPE fc;
    QUANTIZER_TYPE * quantizer;
    int CodedBlockIndex;
    unsigned char * DataOutputInPtr;
    FRAGINFO_TYPE * FragInfo;
    FRAGINFO_TYPE * FragInfoAlloc;
    BR_TYPE br;
    BR_TYPE br2;
    _BITREADER br3;
    unsigned char Vp3VersionNo;
    unsigned char VpProfile;
    unsigned int PostProcessingLevel;
    unsigned int ProcessorFrequency;
    unsigned int CPUFree;
    unsigned char FrameType;
    CONFIG_TYPE Configuration;
    unsigned int CurrentFrameSize;
    unsigned int YPlaneSize;
    unsigned int UVPlaneSize;
    unsigned int VFragments;
    unsigned int HFragments;
    unsigned int UnitFragments;
    unsigned int YPlaneFragments;
    unsigned int UVPlaneFragments;
    unsigned int ReconYPlaneSize;
    unsigned int ReconUVPlaneSize;
    unsigned int YDataOffset;
    unsigned int UDataOffset;
    unsigned int VDataOffset;
    unsigned int ReconYDataOffset;
    unsigned int ReconUDataOffset;
    unsigned int ReconVDataOffset;
    unsigned int MacroBlocks;
    unsigned int MBRows;
    unsigned int MBCols;
    unsigned int ScaleWidth;
    unsigned int ScaleHeight;
    unsigned int OutputWidth;
    unsigned int OutputHeight;
    unsigned char * ThisFrameRecon;
    unsigned char * ThisFrameReconAlloc;
    unsigned char * GoldenFrame;
    unsigned char * GoldenFrameAlloc;
    unsigned char * LastFrameRecon;
    unsigned char * LastFrameReconAlloc;
    unsigned char * PostProcessBuffer;
    unsigned char * PostProcessBufferAlloc;
    unsigned char * ScaleBuffer;
    unsigned char * ScaleBufferAlloc;
    unsigned char * OtherFrameRecon;
    short * quantized_list;
    short * ReconDataBuffer;
    short * ReconDataBufferAlloc;
    unsigned char FragCoefEOB;
    short * TmpReconBuffer;
    short * TmpReconBufferAlloc;
    short * TmpDataBuffer;
    short * TmpDataBufferAlloc;
    unsigned char * LoopFilteredBlockAlloc;
    unsigned char * LoopFilteredBlock;
    void (* * idct)(short *, short *, short *);
    struct POSTPROC_INSTANCE * postproc;
    unsigned char LastToken[256];
    MODE_TYPE LastMode;
    unsigned char DcProbs[22];
    unsigned char AcProbs[396];
    unsigned char DcNodeContexts[2][3][5];
    unsigned char ZeroRunProbs[2][14];
    unsigned char MergedScanOrder[64];
    unsigned char ModifiedScanOrder[64];
    unsigned char EobOffsetTable[64];
    unsigned char ScanBands[64];
    unsigned char MBModeProb[11];
    unsigned char BModeProb[11];
    unsigned char PredictionFilterMode;
    unsigned char PredictionFilterMvSizeThresh;
    unsigned int PredictionFilterVarThresh;
    int RefreshGoldenFrame;
    unsigned char Inter00Prob;
    unsigned int AvgFrameQIndex;
    int testMode;
    unsigned int mvNearOffset[16];
    int probInterlaced;
    char * MBInterlaced;
    char * predictionMode;
    MV_TYPE * MBMotionVector;
    char * MBInterlacedAlloc;
    char * predictionModeAlloc;
    MV_TYPE * MBMotionVectorAlloc;
    unsigned char MvSignProbs[2];
    unsigned char IsMvShortProb[2];
    unsigned char MvShortProbs[2][7];
    unsigned char MvQPelProbs[2];
    unsigned char MvHalfPixelProbs[2];
    unsigned char MvLowBitProbs[2];
    unsigned char MvSizeProbs[2][8];
    unsigned char probXmitted[4][2][10];
    unsigned char probModeSame[4][10];
    unsigned char probMode[4][10][9];
    unsigned int maxTimePerFrame;
    unsigned int thisDecodeTime;
    unsigned int avgDecodeTime;
    unsigned int avgPPTime[10];
    unsigned int avgBlitTime;
    int MultiStream;
    unsigned int DcHuffCode[2][12];
    unsigned char DcHuffLength[2][12];
    unsigned int DcHuffProbs[2][12];
    _huffnode DcHuffTree[2][12];
    unsigned int AcHuffCode[3][2][6][12];
    unsigned char AcHuffLength[3][2][6][12];
    unsigned int AcHuffProbs[3][2][6][12];
    _huffnode AcHuffTree[3][2][6][12];
    unsigned int ZeroHuffCode[2][14];
    unsigned char ZeroHuffLength[2][14];
    unsigned int ZeroHuffProbs[2][14];
    _huffnode ZeroHuffTree[2][14];
    unsigned short DcHuffLUT[2][64];
    unsigned short AcHuffLUT[3][2][6][64];
    unsigned short ZeroHuffLUT[2][64];
    RAW_BUFFER HuffBuffer;
    FRAME_HEADER Header;
    unsigned int Buff2Offset;
    int UseHuffman;
    int CurrentDcRunLen[2];
    int CurrentAc1RunLen[2];
    unsigned char UseLoopFilter;
    unsigned int DrCutOff;
    unsigned int DrThresh[256];
    unsigned int BlackClamp;
    unsigned int WhiteClamp;
    unsigned int DeInterlaceMode;
    unsigned int AddNoiseMode;
    char * ScratchpadStart;
    char * ScratchpadEnd;
} PB_INSTANCE;

typedef int INT32;
typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef short INT16;

extern void *duck_malloc(unsigned long size, int type);
extern void duck_free(void *mem);
extern void VP6_SetPbParam(PB_INSTANCE *pbi, int Param, int Value);
extern void ChangePostProcConfiguration(struct POSTPROC_INSTANCE *ppi, CONFIG_TYPE *cfg);

static const int NearMacroBlocks[12][2] = {
    { -1, 0 },  { 0, -1 },  { -1, -1 }, { -1, 1 },  { -2, 0 },  { 0, -2 },
    { -1, -2 }, { -2, -1 }, { -2, 1 },  { -1, 2 },  { -2, -2 }, { -2, 2 },
};

void VP6_DeleteFragmentInfo(PB_INSTANCE *pbi) {
    if (pbi->mbi.CoeffsAlloc) {
        duck_free(pbi->mbi.CoeffsAlloc);
    }
    pbi->mbi.CoeffsAlloc = 0;
    pbi->mbi.Coeffs = 0;

    if (pbi->FragInfoAlloc) {
        duck_free(pbi->FragInfoAlloc);
    }
    pbi->FragInfoAlloc = 0;
    pbi->FragInfo = 0;

    if (pbi->fc.AboveYAlloc) {
        duck_free(pbi->fc.AboveYAlloc);
    }
    pbi->fc.AboveYAlloc = 0;
    pbi->fc.AboveY = 0;

    if (pbi->fc.AboveUAlloc) {
        duck_free(pbi->fc.AboveUAlloc);
    }
    pbi->fc.AboveUAlloc = 0;
    pbi->fc.AboveU = 0;

    if (pbi->fc.AboveVAlloc) {
        duck_free(pbi->fc.AboveVAlloc);
    }
    pbi->fc.AboveVAlloc = 0;
    pbi->fc.AboveV = 0;

    if (pbi->MBInterlacedAlloc) {
        duck_free(pbi->MBInterlacedAlloc);
    }
    pbi->MBInterlacedAlloc = 0;
    pbi->MBInterlaced = 0;

    if (pbi->MBMotionVectorAlloc) {
        duck_free(pbi->MBMotionVectorAlloc);
    }
    pbi->MBMotionVectorAlloc = 0;
    pbi->MBMotionVector = 0;

    if (pbi->predictionModeAlloc) {
        duck_free(pbi->predictionModeAlloc);
    }
    pbi->predictionModeAlloc = 0;
    pbi->predictionMode = 0;
}

int VP6_AllocateFragmentInfo(PB_INSTANCE *pbi) {
    VP6_DeleteFragmentInfo(pbi);

    pbi->mbi.CoeffsAlloc = duck_malloc(6 * 64 * sizeof(INT16) + 32, 0);
    if (pbi->mbi.CoeffsAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->mbi.Coeffs = (INT16(*)[64])(((UINT32)pbi->mbi.CoeffsAlloc + 31) & ~31);

    pbi->fc.AboveYAlloc = duck_malloc(pbi->HFragments * sizeof(ABOVE_TYPE) + 160, 0);
    if (pbi->fc.AboveYAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveY = (ABOVE_TYPE *)(((UINT32)pbi->fc.AboveYAlloc + 31) & ~31);

    pbi->fc.AboveUAlloc = duck_malloc((pbi->HFragments / 2) * sizeof(ABOVE_TYPE) + 160, 0);
    if (pbi->fc.AboveUAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveU = (ABOVE_TYPE *)(((UINT32)pbi->fc.AboveUAlloc + 31) & ~31);

    pbi->fc.AboveVAlloc = duck_malloc((pbi->HFragments / 2) * sizeof(ABOVE_TYPE) + 160, 0);
    if (pbi->fc.AboveVAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->fc.AboveV = (ABOVE_TYPE *)(((UINT32)pbi->fc.AboveVAlloc + 31) & ~31);

    pbi->MBInterlacedAlloc = duck_malloc(pbi->MacroBlocks + 32, 0);
    if (pbi->MBInterlacedAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->MBInterlaced = (char *)(((UINT32)pbi->MBInterlacedAlloc + 31) & ~31);

    pbi->predictionModeAlloc = duck_malloc(pbi->MacroBlocks + 32, 0);
    if (pbi->predictionModeAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->predictionMode = (char *)(((UINT32)pbi->predictionModeAlloc + 31) & ~31);

    pbi->MBMotionVectorAlloc = duck_malloc(pbi->MacroBlocks * sizeof(MV_TYPE) + 32, 0);
    if (pbi->MBMotionVectorAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->MBMotionVector = (MV_TYPE *)(((UINT32)pbi->MBMotionVectorAlloc + 31) & ~31);

    pbi->FragInfoAlloc = duck_malloc(pbi->UnitFragments * sizeof(FRAGINFO_TYPE) + 32, 0);
    if (pbi->FragInfoAlloc == 0) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }
    pbi->FragInfo = (FRAGINFO_TYPE *)(((UINT32)pbi->FragInfoAlloc + 31) & ~31);

    return 1;
}

void VP6_DeleteFrameInfo(PB_INSTANCE *pbi) {
    if (pbi->ThisFrameReconAlloc) {
        duck_free(pbi->ThisFrameReconAlloc);
    }
    if (pbi->GoldenFrameAlloc) {
        duck_free(pbi->GoldenFrameAlloc);
    }
    if (pbi->LastFrameReconAlloc) {
        duck_free(pbi->LastFrameReconAlloc);
    }
    if (pbi->PostProcessBufferAlloc) {
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

int VP6_AllocateFrameInfo(PB_INSTANCE *pbi, UINT32 FrameSize) {
    FrameSize += 32;

    VP6_DeleteFrameInfo(pbi);

    pbi->ThisFrameReconAlloc = duck_malloc(pbi->Configuration.YStride + FrameSize, 0);
    if (pbi->ThisFrameReconAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->ThisFrameRecon = (UINT8 *)(((UINT32)pbi->ThisFrameReconAlloc + 31) & ~31);

    pbi->GoldenFrameAlloc = duck_malloc(pbi->Configuration.YStride + FrameSize, 0);
    if (pbi->GoldenFrameAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->GoldenFrame = (UINT8 *)(((UINT32)pbi->GoldenFrameAlloc + 31) & ~31);

    pbi->LastFrameReconAlloc = duck_malloc(pbi->Configuration.YStride + FrameSize, 0);
    if (pbi->LastFrameReconAlloc == 0) {
        VP6_DeleteFrameInfo(pbi);
        return 0;
    }
    pbi->LastFrameRecon = (UINT8 *)(((UINT32)pbi->LastFrameReconAlloc + 31) & ~31);

    return 1;
}

int VP6_InitFrameDetails(PB_INSTANCE *pbi) {
    UINT32 i;
    int FrameSize;

    if (pbi->CPUFree > 0) {
        VP6_SetPbParam(pbi, 1, pbi->CPUFree);
    }

    pbi->YPlaneSize =
        pbi->Configuration.VideoFrameWidth * pbi->Configuration.VideoFrameHeight;
    pbi->UVPlaneSize = pbi->YPlaneSize / 4;
    pbi->HFragments = pbi->Configuration.VideoFrameWidth / pbi->Configuration.HFragPixels;
    pbi->VFragments = pbi->Configuration.VideoFrameHeight / pbi->Configuration.VFragPixels;
    pbi->YPlaneFragments = pbi->VFragments * pbi->HFragments;
    pbi->UnitFragments = pbi->YPlaneFragments * 3;
    pbi->UnitFragments /= 2;
    pbi->UVPlaneFragments = pbi->YPlaneFragments / 4;

    pbi->Configuration.YStride = pbi->Configuration.VideoFrameWidth + 96;
    pbi->Configuration.UVStride = pbi->Configuration.YStride / 2;

    pbi->ReconYPlaneSize =
        pbi->Configuration.YStride * (pbi->Configuration.VideoFrameHeight + 96);
    pbi->ReconUVPlaneSize = pbi->ReconYPlaneSize / 4;

    FrameSize = pbi->ReconYPlaneSize + 2 * pbi->ReconUVPlaneSize;

    pbi->YDataOffset = 0;
    pbi->UDataOffset = pbi->YPlaneSize;
    pbi->VDataOffset = pbi->YPlaneSize + pbi->UVPlaneSize;
    pbi->ReconYDataOffset = 0;
    pbi->ReconUDataOffset = pbi->ReconYPlaneSize;
    pbi->ReconVDataOffset = pbi->ReconYPlaneSize + pbi->ReconUVPlaneSize;

    pbi->MBRows = 6 + (pbi->Configuration.VideoFrameHeight / 16) +
                  ((pbi->Configuration.VideoFrameHeight % 16) ? 1 : 0);
    pbi->MBCols = 6 + (pbi->Configuration.VideoFrameWidth / 16) +
                  ((pbi->Configuration.VideoFrameWidth % 16) ? 1 : 0);
    pbi->MacroBlocks = pbi->MBRows * pbi->MBCols;

    for (i = 0; i < 12; i++) {
        pbi->mvNearOffset[i] = NearMacroBlocks[i][0] * pbi->MBCols + NearMacroBlocks[i][1];
    }

    ChangePostProcConfiguration(pbi->postproc, &pbi->Configuration);

    if (!VP6_AllocateFragmentInfo(pbi)) {
        return 0;
    }

    if (!VP6_AllocateFrameInfo(pbi, FrameSize)) {
        VP6_DeleteFragmentInfo(pbi);
        return 0;
    }

    if (pbi->ScaleBuffer == 0 && pbi->OutputWidth &&
        (pbi->Configuration.VideoFrameWidth != pbi->OutputWidth ||
         pbi->Configuration.VideoFrameHeight != pbi->OutputHeight)) {
        pbi->ScaleBufferAlloc =
            duck_malloc(32 + 3 * (pbi->OutputWidth + 32) * (pbi->OutputHeight + 32) / 2, 0);
        pbi->ScaleBuffer = (UINT8 *)(((UINT32)pbi->ScaleBufferAlloc + 31) & ~31);
    }

    return 1;
}

void VP6_InitialiseConfiguration(PB_INSTANCE *pbi) {
    pbi->Configuration.HFragPixels = 8;
    pbi->Configuration.VFragPixels = 8;
}
