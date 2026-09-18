typedef enum {
    TOP_LEFT_Y_BLOCK = 0,
    TOP_RIGHT_Y_BLOCK = 1,
    BOTTOM_LEFT_Y_BLOCK = 2,
    BOTTOM_RIGHT_Y_BLOCK = 3,
    U_BLOCK = 4,
    V_BLOCK = 5
} BLOCK_POSITION;

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
    DO_NOT_CODE = 16
} CODING_MODE;

typedef struct {
    unsigned char Token;
    CODING_MODE Mode;
    unsigned short Frame;
    short Dc;
    unsigned char unused[3];
} MB_INFO;

typedef struct {
    short x;
    short y;
} MOTION_VECTOR;

typedef struct {
    unsigned int lowvalue;
    unsigned int range;
    unsigned int value;
    int count;
    unsigned int pos;
    unsigned char *buffer;
    unsigned int MeasureCost;
    unsigned int BitCounter;
} BOOL_CODER;

struct _BITREADER {
    int bitsinremainder;
    unsigned int remainder;
    const unsigned char *position;
};

struct _tokenorptr {
    unsigned int value : 7;
    unsigned int selector : 1;
};

struct _huffnode {
    union {
        char l;
        struct _tokenorptr left;
    } leftunion;
    union {
        char r;
        struct _tokenorptr right;
    } rightunion;
    unsigned char freq;
};

struct HUFF_TABLE_NODE {
    unsigned short length : 4;
    unsigned short unused : 6;
    unsigned short value : 5;
    unsigned short flag : 1;
};

struct CONFIG_TYPE {
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
};

struct RAW_BUFFER {
    unsigned int pos;
    int byte_bit_offset;
    unsigned int DataBlock;
    unsigned char *Buffer;
};

struct FRAME_HEADER {
    unsigned char *buffer;
    unsigned int value;
    int bits_available;
    unsigned int pos;
};

struct PB_INSTANCE {
    struct {
        short (*CoeffsAlloc)[64];
        short (*Coeffs)[64];
        CODING_MODE Mode;
        CODING_MODE BlockMode[6];
        MOTION_VECTOR Mv[6];
        MOTION_VECTOR NearestInterMVect;
        MOTION_VECTOR NearInterMVect;
        int NearestMvIndex;
        MOTION_VECTOR NearestGoldMVect;
        MOTION_VECTOR NearGoldMVect;
        int NearestGMvIndex;
        unsigned int MBrow;
        unsigned int MBcol;
        BLOCK_POSITION bp;
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
        MB_INFO *Above;
        MB_INFO *Left;
        short *LastDc;
        int Interlaced;
    } mbi;
    struct {
        MB_INFO LeftY[2];
        MB_INFO LeftU;
        MB_INFO LeftV;
        MB_INFO *AboveY;
        MB_INFO *AboveU;
        MB_INFO *AboveV;
        MB_INFO *AboveYAlloc;
        MB_INFO *AboveUAlloc;
        MB_INFO *AboveVAlloc;
        short LastDcY[4];
        short LastDcU[4];
        short LastDcV[4];
    } fc;
    struct {
        unsigned int FrameQIndex;
        unsigned int ThisFrameQuantizerValue;
        short round[8];
        short mult[8];
        short zbin[8];
        unsigned int LastQuantizerValue;
        unsigned int QThreshTable[64];
        unsigned int *transIndex;
        unsigned char quant_index[64];
        short *dequant_coeffs[2];
        short *dequant_coeffsAlloc[2];
        int QuantCoeffs[2][64];
        int QuantRound[2][64];
        int ZeroBinSize[2][64];
        int ZlrZbinCorrections[2][64];
    } * quantizer;
    int CodedBlockIndex;
    unsigned char *DataOutputInPtr;
    struct FRAGMENT_INFO {
        unsigned int FragCodingMode : 4;
        int MVectorX : 8;
        int MVectorY : 8;
    } * FragInfo;
    struct FRAGMENT_INFO *FragInfoAlloc;
    BOOL_CODER br;
    BOOL_CODER br2;
    struct _BITREADER br3;
    unsigned char Vp3VersionNo;
    unsigned char VpProfile;
    unsigned int PostProcessingLevel;
    unsigned int ProcessorFrequency;
    unsigned int CPUFree;
    unsigned char FrameType;
    struct CONFIG_TYPE Configuration;
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
    unsigned char *ThisFrameRecon;
    unsigned char *ThisFrameReconAlloc;
    unsigned char *GoldenFrame;
    unsigned char *GoldenFrameAlloc;
    unsigned char *LastFrameRecon;
    unsigned char *LastFrameReconAlloc;
    unsigned char *PostProcessBuffer;
    unsigned char *PostProcessBufferAlloc;
    unsigned char *ScaleBuffer;
    unsigned char *ScaleBufferAlloc;
    unsigned char *OtherFrameRecon;
    short *quantized_list;
    short *ReconDataBuffer;
    short *ReconDataBufferAlloc;
    unsigned char FragCoefEOB;
    short *TmpReconBuffer;
    short *TmpReconBufferAlloc;
    short *TmpDataBuffer;
    short *TmpDataBufferAlloc;
    unsigned char *LoopFilteredBlockAlloc;
    unsigned char *LoopFilteredBlock;
    void (**idct)(short *, short *, short *);
    struct POSTPROC_INSTANCE *postproc;
    unsigned char LastToken[256];
    CODING_MODE LastMode;
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
    char *MBInterlaced;
    char *predictionMode;
    MOTION_VECTOR *MBMotionVector;
    char *MBInterlacedAlloc;
    char *predictionModeAlloc;
    MOTION_VECTOR *MBMotionVectorAlloc;
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
    struct _huffnode DcHuffTree[2][12];
    unsigned int AcHuffCode[3][2][6][12];
    unsigned char AcHuffLength[3][2][6][12];
    unsigned int AcHuffProbs[3][2][6][12];
    struct _huffnode AcHuffTree[3][2][6][12];
    unsigned int ZeroHuffCode[2][14];
    unsigned char ZeroHuffLength[2][14];
    unsigned int ZeroHuffProbs[2][14];
    struct _huffnode ZeroHuffTree[2][14];
    unsigned short DcHuffLUT[2][64];
    unsigned short AcHuffLUT[3][2][6][64];
    unsigned short ZeroHuffLUT[2][64];
    struct RAW_BUFFER HuffBuffer;
    struct FRAME_HEADER Header;
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
    char *ScratchpadStart;
    char *ScratchpadEnd;
};

int VP6_DecodeBool(BOOL_CODER *br, int probability);
int VP6_DecodeBool128(BOOL_CODER *br);
int nDecodeBool(BOOL_CODER *br, int probability);
void VP6_DecodeBlock(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol,
                     BLOCK_POSITION bp);
void VP6_DecodeMacroBlock(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol);
CODING_MODE VP6_DecodeBlockMode(struct PB_INSTANCE *pbi);
CODING_MODE VP6_DecodeMode(struct PB_INSTANCE *pbi, CODING_MODE lastmode, unsigned int type);
void VP6_decodeModeAndMotionVector(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol);
void VP6_decodeMotionVector(struct PB_INSTANCE *pbi, MOTION_VECTOR *mv, CODING_MODE Mode);
void VP6_FindNearestandNextNearest(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol,
                                   unsigned char Frame, int *type);
void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *OutputPtr, BLOCK_POSITION bp);
void VP6_ReconstructBlock(struct PB_INSTANCE *pbi, BLOCK_POSITION bp);
void ScalarReconIntra_GC(short *TmpDataBuffer, unsigned char *ReconPtr, unsigned short *ChangePtr,
                         unsigned int LineStep);
void ScalarReconInter_GC(short *TmpDataBuffer, unsigned char *ReconPtr, unsigned char *RefPtr,
                         short *ChangePtr, unsigned int LineStep);
void ReconBlock_GC(short *SrcBlock, short *ReconRefPtr, unsigned char *DestBlock,
                   unsigned int LineStep);
void IDct64_GC(short *InputData, short *QuantMatrix, short *OutputData);
void IDct1_GC(short *InputData, short *QuantMatrix, short *OutputData);
void UnpackBlock_GC(unsigned char *ReconPtr, short *ReconRefPtr, unsigned int ReconPixelsPerLine);
void FilterBlock1dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, unsigned int PixelStep, int *Filter);
void FilterBlock2dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter);
void FilterBlock_GC(unsigned char *ReconPtr1, unsigned char *ReconPtr2, unsigned short *ReconRefPtr,
                    unsigned int PixelsPerLine, int ModX, int ModY, int UseBicubic);

extern unsigned int VP6_ModeUsesMC[10];

extern void (*ReconIntra)(short *, unsigned char *, unsigned short *, unsigned int);
extern void (*ReconInter)(short *, unsigned char *, unsigned char *, short *, unsigned int);
extern void (*ReconBlock)(short *, short *, unsigned char *, unsigned int);

extern int Var16Point(unsigned char *Data, int Stride);
extern void VP6_PredictFiltered(struct PB_INSTANCE *pbi, unsigned char *SrcPtr, int MVX, int MVY);

extern void (*UnpackBlock)(unsigned char *, short *, unsigned int);
extern void (*FilterBlock)(unsigned char *, unsigned char *, unsigned short *, unsigned int, int,
                           int, int);

extern void FilterBlock1d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                          unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                          unsigned int OutputHeight, unsigned int OutputWidth, int *Filter);
extern void FilterBlock2d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                          unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter);

extern unsigned char VP6_ReadTokensPredictA(struct PB_INSTANCE *pbi, short *CoeffData,
                                           unsigned int Plane, MB_INFO *Above, MB_INFO *Left);
extern void (*idct[65])(short *, short *, short *);
extern const unsigned char VP6_QTableSelect[6];

static float f128 __attribute__((aligned(8))) = 128.0f;
static float f64 = 64.0f;

static const int VP6_HuffTokenMinVal[12] = {0, 1, 2, 3, 4, 5, 7, 11, 19, 35, 67, 0};

static const int VP6_CoeffToHuffBand[65] = {
    -1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

const unsigned char DefaultNonInterlacedScanBands[64] = {
    0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6,
    7, 7, 7, 7, 7, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 11, 11, 11, 11, 11,
    11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
};

const unsigned char DefaultInterlacedScanBands[64] = {
    0, 1, 0, 1, 1, 2, 5, 3, 2, 2, 2, 2, 4, 7, 8, 10, 9, 7, 5, 4, 2, 3,
    5, 6, 8, 9, 11, 12, 13, 12, 11, 10, 9, 7, 5, 4, 6, 7, 9, 11, 12, 12,
    13, 13, 14, 12, 11, 9, 7, 9, 11, 12, 14, 14, 14, 15, 13, 11, 13, 15, 15, 15, 15, 15,
};

const unsigned int VP6_Mode2Frame[16] = {1, 0, 1, 1, 1, 2, 2, 1, 2, 2, 0, 0, 0, 0, 0, 0};

static const unsigned int loMaskTbl_VP60[33] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F, 0x0000003F,
    0x0000007F, 0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF, 0x00001FFF,
    0x00003FFF, 0x00007FFF, 0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
    0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
    0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

static float idctconstants[8] = {
    8.0f,          0.980785251f, 0.923879504f, 0.831469595f,
    0.707106769f,  0.555570245f, 0.382683426f, 0.195090324f,
};

static int BicubicFilters[8][4] = {
    {0, 128, 0, 0},   {-4, 118, 16, -2}, {-7, 106, 34, -5}, {-8, 90, 53, -7},
    {-8, 72, 72, -8}, {-7, 53, 90, -8},  {-5, 34, 106, -7}, {-2, 16, 118, -4},
};

static int BilinearFilters[8][2] = {
    {128, 0}, {112, 16}, {96, 32}, {80, 48}, {64, 64}, {48, 80}, {32, 96}, {16, 112},
};

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

    if (range >= 128) {
        br->value = value;
        br->range = range;
        return bit;
    }

    do {
        range += range;
        value += value;
        if (--count == 0) {
            count = 8;
            value |= br->buffer[br->pos];
            br->pos++;
        }
    } while (range < 128);

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

    if (--count == 0) {
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
        range = range - split;
        value = value - bigsplit;
        bit = 1;
    } else {
        range = split;
    }

    while (range < 128) {
        range += range;
        value += value;
        if (--count == 0) {
            count = 8;
            value |= br->buffer[br->pos];
            br->pos++;
        }
    }

    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
}

#define VP6_LOADREMAINDER(br)                                                                      \
    br->remainder = (br->position[0] << 24) + (br->position[1] << 16) + (br->position[2] << 8) +   \
                    br->position[3];                                                               \
    br->position += 4

static inline unsigned int bitread(struct _BITREADER *br, int bits) {
    unsigned int ret = 0;

    br->remainder &= loMaskTbl_VP60[br->bitsinremainder];

    if ((bits -= br->bitsinremainder) > 0) {
        ret |= br->remainder << bits;
        VP6_LOADREMAINDER(br);
        bits -= 32;
    }
    return ret | (br->remainder >> (br->bitsinremainder = -bits));
}

static inline unsigned int bitreadonly(struct _BITREADER *br, unsigned int bits) {
    unsigned int bitsinremainder = br->bitsinremainder;
    unsigned int ret = (1 << bitsinremainder) - 1;

    ret &= br->remainder;
    if (bitsinremainder >= bits) {
        return ret >> (bitsinremainder - bits);
    }
    ret <<= 8;
    ret |= br->position[0];
    return ret >> (8 + bitsinremainder - bits);
}

static inline void bitShift(struct _BITREADER *br, int bits) {
    br->bitsinremainder -= bits;
    if (br->bitsinremainder < 0) {
        VP6_LOADREMAINDER(br);
        br->bitsinremainder += 32;
    }
}

static inline unsigned int bitread1(struct _BITREADER *br) {
    if (br->bitsinremainder) {
        return (br->remainder >> --br->bitsinremainder) & 1;
    }
    VP6_LOADREMAINDER(br);
    return br->remainder >> (br->bitsinremainder = 31);
}

static inline void VP6_PredictDC(struct PB_INSTANCE *pbi, BLOCK_POSITION bp, short *LastDC,
                                 MB_INFO *Above, MB_INFO *Left) {
    unsigned char Frame;
    unsigned char Count;
    int Avg;

    Frame = VP6_Mode2Frame[pbi->mbi.Mode];
    Count = 0;
    Avg = 0;

    if (Frame == Left->Frame) {
        Avg = Left->Dc;
        Count = 1;
    }

    if (Frame == Above->Frame) {
        Avg += Above->Dc;
        Count++;
    }

    if (Count == 0) {
        Avg = LastDC[Frame];
    }

    if (Count == 2) {
        Avg += (((signed short)Avg >> 15) & 1);
        Avg >>= 1;
    }

    pbi->mbi.Coeffs[bp][0] += Avg;
    LastDC[Frame] = pbi->mbi.Coeffs[bp][0];
}

static inline int VP6_ExtractTokenN(struct _BITREADER *br, struct _huffnode *hn,
                                    unsigned short *hlt) {
    struct _tokenorptr torp;
    struct HUFF_TABLE_NODE *htptr = (struct HUFF_TABLE_NODE *)hlt;
    unsigned int x = bitreadonly(br, 6);

    bitShift(br, htptr[x].length);

    if (htptr[x].flag) {
        return htptr[x].value;
    }

    torp.value = htptr[x].value;

    do {
        if (bitread1(br)) {
            torp = hn[torp.value].rightunion.right;
        } else {
            torp = hn[torp.value].leftunion.left;
        }
    } while (!torp.selector);

    return torp.value;
}

static inline unsigned char VP6_ReadTokensPredictB(struct PB_INSTANCE *pbi, BLOCK_POSITION bp,
                                                   unsigned int Plane) {
    short *CoeffData;
    struct _BITREADER *br;
    unsigned int PrecTokenIndex;
    int token;
    int value;
    int SignBit;
    unsigned int ZeroRunLength;
    int i;
    int band;
    unsigned int ZeroToken;

    CoeffData = (short *)((bp << 7) + (unsigned int)pbi->mbi.Coeffs);
    br = &pbi->br3;

    i = 1;

    if (pbi->CurrentDcRunLen[Plane] > 0) {
        pbi->CurrentDcRunLen[Plane]--;
        PrecTokenIndex = 0;
    } else {
        token = VP6_ExtractTokenN(br, pbi->DcHuffTree[Plane], pbi->DcHuffLUT[Plane]);
        value = VP6_HuffTokenMinVal[token];

        if (token == 11) {
            goto Done;
        }

        if (token == 0) {
            ZeroRunLength = bitread(br, 2) + 1;

            if (ZeroRunLength == 3) {
                ZeroRunLength = bitread(br, 2) + 3;
            } else if (ZeroRunLength == 4) {
                if (bitread1(br)) {
                    ZeroRunLength = bitread(br, 6) + 11;
                } else {
                    ZeroRunLength = bitread(br, 2) + 7;
                }
            }
            pbi->CurrentDcRunLen[Plane] = ZeroRunLength - 1;

            PrecTokenIndex = 0;
        } else {
            if (token <= 4) {
                SignBit = bitread1(br);
            } else if (token <= 9) {
                value += bitread(br, token - 4);
                SignBit = bitread1(br);
            } else {
                value += bitread(br, 11);
                SignBit = bitread1(br);
            }
            CoeffData[0] = (value ^ -SignBit) + SignBit;
            PrecTokenIndex = (value > 1) ? 2 : 1;
        }
    }

    if (pbi->CurrentAc1RunLen[Plane] > 0) {
        pbi->CurrentAc1RunLen[Plane]--;
        goto Done;
    }

    do {
        band = VP6_CoeffToHuffBand[i];
        token = VP6_ExtractTokenN(br, pbi->AcHuffTree[PrecTokenIndex][Plane][band],
                                  pbi->AcHuffLUT[PrecTokenIndex][Plane][band]);
        value = VP6_HuffTokenMinVal[token];

        if (token == 0) {
            unsigned int ZrlBand = i > 5;

            ZeroToken = VP6_ExtractTokenN(br, pbi->ZeroHuffTree[ZrlBand],
                                          pbi->ZeroHuffLUT[ZrlBand]);

            if (ZeroToken <= 7) {
                i += ZeroToken;
            } else {
                i += bitread(br, 6) + 8;
            }

            PrecTokenIndex = 0;

            continue;
        }

        if (token == 11) {
            if (i != 1) {
                break;
            }

            ZeroRunLength = bitread(br, 2) + 1;

            if (ZeroRunLength == 3) {
                ZeroRunLength = bitread(br, 2) + 3;
            } else if (ZeroRunLength == 4) {
                if (bitread1(br)) {
                    ZeroRunLength = bitread(br, 6) + 11;
                } else {
                    ZeroRunLength = bitread(br, 2) + 7;
                }
            }
            pbi->CurrentAc1RunLen[Plane] = ZeroRunLength - 1;

            break;
        }

        if (token <= 4) {
            SignBit = bitread1(br);
        } else if (token <= 9) {
            value += bitread(br, token - 4);
            SignBit = bitread1(br);
        } else {
            value += bitread(br, 11);
            SignBit = bitread1(br);
        }
        CoeffData[pbi->MergedScanOrder[i]] = (value ^ -SignBit) + SignBit;
        PrecTokenIndex = (value > 1) ? 2 : 1;
    } while (++i <= 63);

Done:
    return pbi->EobOffsetTable[i - 1];
}

void VP6_DecodeBlock(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol,
                     BLOCK_POSITION bp) {
    int FragCoefEOB;

    if (pbi->UseHuffman) {
        FragCoefEOB = VP6_ReadTokensPredictB(pbi, bp, pbi->mbi.Plane != 0);
    } else {
        FragCoefEOB = VP6_ReadTokensPredictA(pbi, pbi->mbi.Coeffs[bp], pbi->mbi.Plane != 0,
                                             pbi->mbi.Above, pbi->mbi.Left);
    }

    VP6_PredictDC(pbi, bp, pbi->mbi.LastDc, pbi->mbi.Above, pbi->mbi.Left);

    pbi->mbi.Above->Mode = pbi->mbi.Left->Mode = pbi->mbi.BlockMode[bp];
    pbi->mbi.Above->Dc = pbi->mbi.Left->Dc = pbi->mbi.Coeffs[bp][0];
    pbi->mbi.Above->Frame = pbi->mbi.Left->Frame = VP6_Mode2Frame[pbi->mbi.Mode];

    if (FragCoefEOB <= 1) {
        idct[1](pbi->mbi.Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]],
                pbi->ReconDataBuffer);

        pbi->mbi.Coeffs[bp][0] = 0;
    } else if (FragCoefEOB <= 10) {
        idct[9](pbi->mbi.Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]],
                pbi->ReconDataBuffer);

        memset(&pbi->mbi.Coeffs[bp][0], 0, 16);
        memset(&pbi->mbi.Coeffs[bp][8], 0, 8);
        memset(&pbi->mbi.Coeffs[bp][16], 0, 8);
        memset(&pbi->mbi.Coeffs[bp][24], 0, 8);
        pbi->mbi.Coeffs[bp][32] = 0;
    } else {
        idct[63](pbi->mbi.Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]],
                 pbi->ReconDataBuffer);

        memset(pbi->mbi.Coeffs[bp], 0, 128);
    }

    VP6_ReconstructBlock(pbi, bp);
}

void VP6_DecodeMacroBlock(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol) {
    unsigned int MBPointer;
    int NextBlock;

    if (pbi->Configuration.Interlaced) {
        unsigned char prob = pbi->probInterlaced;

        if (MBcol > 3) {
            if (pbi->mbi.Interlaced) {
                prob = prob - (prob >> 1);
            } else {
                prob = prob + ((256 - prob) >> 1);
            }
        }

        pbi->mbi.Interlaced = nDecodeBool(&pbi->br, prob);
    } else {
        pbi->mbi.Interlaced = 0;
    }

    if (pbi->FrameType == 0) {
        pbi->mbi.Mode = CODE_INTRA;
    } else {
        VP6_decodeModeAndMotionVector(pbi, MBrow, MBcol);
    }

    if (pbi->mbi.Interlaced == 0) {
        NextBlock = 8;
        pbi->mbi.CurrentReconStride = pbi->Configuration.YStride;
    } else {
        NextBlock = 1;
        pbi->mbi.CurrentReconStride = pbi->Configuration.YStride * 2;
    }

    pbi->mbi.MvShift = 2;
    pbi->mbi.FrameReconStride = pbi->Configuration.YStride;
    pbi->mbi.LastDc = pbi->fc.LastDcY;
    pbi->mbi.MvModMask = 3;
    pbi->mbi.Plane = 0;
    pbi->mbi.SourceY = MBrow * 16;
    pbi->mbi.SourceX = MBcol * 16;
    MBPointer = pbi->ReconYDataOffset + pbi->mbi.SourceY * pbi->Configuration.YStride +
                pbi->mbi.SourceX;
    pbi->mbi.Recon = MBPointer;
    pbi->mbi.Above = &pbi->fc.AboveY[MBcol * 2];
    pbi->mbi.Left = &pbi->fc.LeftY[0];
    VP6_DecodeBlock(pbi, MBrow, MBcol, TOP_LEFT_Y_BLOCK);

    pbi->mbi.Recon += 8;
    pbi->mbi.Above = &pbi->fc.AboveY[MBcol * 2 + 1];
    pbi->mbi.Left = &pbi->fc.LeftY[0];
    pbi->mbi.SourceX += 8;
    VP6_DecodeBlock(pbi, MBrow, MBcol, TOP_RIGHT_Y_BLOCK);

    MBPointer += NextBlock * pbi->Configuration.YStride; pbi->mbi.Recon = MBPointer;
    pbi->mbi.Above = &pbi->fc.AboveY[MBcol * 2];
    pbi->mbi.Left = &pbi->fc.LeftY[1];
    pbi->mbi.SourceX -= 8;
    pbi->mbi.SourceY += NextBlock;
    VP6_DecodeBlock(pbi, MBrow, MBcol, BOTTOM_LEFT_Y_BLOCK);

    pbi->mbi.Recon += 8;
    pbi->mbi.Above = &pbi->fc.AboveY[MBcol * 2 + 1];
    pbi->mbi.Left = &pbi->fc.LeftY[1];
    pbi->mbi.SourceX += 8;
    VP6_DecodeBlock(pbi, MBrow, MBcol, BOTTOM_RIGHT_Y_BLOCK);

    pbi->mbi.FrameReconStride = pbi->Configuration.UVStride;
    pbi->mbi.CurrentReconStride = pbi->Configuration.UVStride;
    pbi->mbi.SourceY = MBrow * 8;
    pbi->mbi.SourceX = MBcol * 8;
    pbi->mbi.MvShift = 3;
    pbi->mbi.MvModMask = 7;

    pbi->mbi.Recon = pbi->ReconUDataOffset + pbi->mbi.SourceY * pbi->Configuration.UVStride +
                     pbi->mbi.SourceX;
    pbi->mbi.Above = &pbi->fc.AboveU[MBcol];
    pbi->mbi.Left = &pbi->fc.LeftU;
    pbi->mbi.LastDc = pbi->fc.LastDcU;
    pbi->mbi.Plane = 1;
    VP6_DecodeBlock(pbi, MBrow, MBcol, U_BLOCK);

    pbi->mbi.Above = &pbi->fc.AboveV[MBcol];
    pbi->mbi.Left = &pbi->fc.LeftV;
    pbi->mbi.Recon = pbi->ReconVDataOffset + pbi->mbi.SourceY * pbi->mbi.CurrentReconStride +
                     pbi->mbi.SourceX;
    pbi->mbi.LastDc = pbi->fc.LastDcV;
    pbi->mbi.Plane = 2;
    VP6_DecodeBlock(pbi, MBrow, MBcol, V_BLOCK);
}

CODING_MODE VP6_DecodeBlockMode(struct PB_INSTANCE *pbi) {
    int choice;

    choice = VP6_DecodeBool128(&pbi->br) * 2;
    choice = choice + VP6_DecodeBool128(&pbi->br);

    switch (choice) {
    case 1:
        return CODE_INTER_PLUS_MV;
    case 2:
        return CODE_INTER_NEAREST_MV;
    case 3:
        return CODE_INTER_NEAR_MV;
    case 0:
        return CODE_INTER_NO_MV;
    default:
        return CODE_INTER_NO_MV;
    }
}

CODING_MODE VP6_DecodeMode(struct PB_INSTANCE *pbi, CODING_MODE lastmode, unsigned int type) {
    CODING_MODE mode;

    if (VP6_DecodeBool(&pbi->br, pbi->probModeSame[type][lastmode])) {
        mode = lastmode;
    } else {
        unsigned char *Stats = pbi->probMode[type][lastmode];

        if (VP6_DecodeBool(&pbi->br, Stats[0])) {
            if (VP6_DecodeBool(&pbi->br, Stats[2])) {
                if (VP6_DecodeBool(&pbi->br, Stats[6])) {
                    mode = (CODING_MODE)(8 + VP6_DecodeBool(&pbi->br, Stats[8]));
                } else {
                    mode = (CODING_MODE)(5 + VP6_DecodeBool(&pbi->br, Stats[7]));
                }
            } else {
                mode = CODE_INTRA;
                if (VP6_DecodeBool(&pbi->br, Stats[5])) {
                    mode = CODE_INTER_FOURMV;
                }
            }
        } else {
            if (VP6_DecodeBool(&pbi->br, Stats[1])) {
                mode = (CODING_MODE)(3 + VP6_DecodeBool(&pbi->br, Stats[4]));
            } else {
                mode = (CODING_MODE)(2 * VP6_DecodeBool(&pbi->br, Stats[3]));
            }
        }
    }

    return mode;
}

void VP6_decodeModeAndMotionVector(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol) {
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

    if (mode == CODE_INTER_FOURMV) {
        pbi->mbi.BlockMode[0] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[1] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[2] = VP6_DecodeBlockMode(pbi);
        pbi->mbi.BlockMode[3] = VP6_DecodeBlockMode(pbi);

        pbi->mbi.BlockMode[4] = mode;
        pbi->mbi.BlockMode[5] = mode;
        x = 0;
        y = 0;

        for (k = 0; k < 4; k++) {
            if (pbi->mbi.BlockMode[k] == CODE_INTER_NO_MV) {
                pbi->mbi.Mv[k].x = 0;
                pbi->mbi.Mv[k].y = 0;
            } else if (pbi->mbi.BlockMode[k] == CODE_INTER_NEAREST_MV) {
                pbi->mbi.Mv[k].x = pbi->mbi.NearestInterMVect.x;
                pbi->mbi.Mv[k].y = pbi->mbi.NearestInterMVect.y;
                x += pbi->mbi.NearestInterMVect.x;
                y += pbi->mbi.NearestInterMVect.y;
            } else if (pbi->mbi.BlockMode[k] == CODE_INTER_NEAR_MV) {
                pbi->mbi.Mv[k].x = pbi->mbi.NearInterMVect.x;
                pbi->mbi.Mv[k].y = pbi->mbi.NearInterMVect.y;
                x += pbi->mbi.NearInterMVect.x;
                y += pbi->mbi.NearInterMVect.y;
            } else if (pbi->mbi.BlockMode[k] == CODE_INTER_PLUS_MV) {
                VP6_decodeMotionVector(pbi, &mv, CODE_INTER_PLUS_MV);
                pbi->mbi.Mv[k].x = mv.x;
                pbi->mbi.Mv[k].y = mv.y;
                x += mv.x;
                y += mv.y;
            }
        }

        { int t = x + 1; int u; if (x >= 0) { u = (t + 1) >> 2; } else { u = t >> 2; } x = u; }
        { int t = y + 1; int u; if (y >= 0) { u = (t + 1) >> 2; } else { u = t >> 2; } y = u; }

        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = pbi->mbi.Mv[3].x;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = pbi->mbi.Mv[3].y;

        pbi->mbi.Mv[4].x = x;
        pbi->mbi.Mv[4].y = y;

        pbi->mbi.Mv[5].x = x;
        pbi->mbi.Mv[5].y = y;
    } else {
        switch (mode) {
        case CODE_INTER_NEAREST_MV:
            x = pbi->mbi.NearestInterMVect.x;
            y = pbi->mbi.NearestInterMVect.y;
            break;
        case CODE_INTER_NEAR_MV:
            x = pbi->mbi.NearInterMVect.x;
            y = pbi->mbi.NearInterMVect.y;
            break;
        case CODE_GOLD_NEAREST_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearestGoldMVect.x;
            y = pbi->mbi.NearestGoldMVect.y;
            break;
        case CODE_GOLD_NEAR_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearGoldMVect.x;
            y = pbi->mbi.NearGoldMVect.y;
            break;
        case CODE_INTER_PLUS_MV:
            VP6_decodeMotionVector(pbi, &mv, CODE_INTER_PLUS_MV);
            x = mv.x;
            y = mv.y;
            break;
        case CODE_GOLDEN_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            VP6_decodeMotionVector(pbi, &mv, CODE_GOLDEN_MV);
            x = mv.x;
            y = mv.y;
            break;
        default:
            x = 0;
            y = 0;
            break;
        }

        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].x = x;
        pbi->MBMotionVector[MBrow * pbi->MBCols + MBcol].y = y;

        for (k = 0; k < 6; k++) {
            pbi->mbi.Mv[k].x = x;
            pbi->mbi.Mv[k].y = y;
            pbi->mbi.BlockMode[k] = mode;
        }
    }
}

void VP6_decodeMotionVector(struct PB_INSTANCE *pbi, MOTION_VECTOR *mv, CODING_MODE Mode) {
    unsigned int i;
    int Vector;
    int SignBit;
    int MvOffsetX = 0;
    int MvOffsetY = 0;

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
        if (VP6_DecodeBool(&pbi->br, pbi->IsMvShortProb[i]) == 0) {
            if (VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][0])) {
                if (VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][4])) {
                    Vector = 6 + VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][6]);
                } else {
                    Vector = 4 + VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][5]);
                }
            } else {
                if (VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][1])) {
                    Vector = 2 + VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][3]);
                } else {
                    Vector = VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][2]);
                }
            }
        } else {
            Vector = VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][0]);
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][1]) << 1;
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][2]) << 2;
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][7]) << 7;
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][6]) << 6;
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][5]) << 5;
            Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][4]) << 4;

            if (Vector & 0xF0) {
                Vector += VP6_DecodeBool(&pbi->br, pbi->MvSizeProbs[i][3]) << 3;
            } else {
                Vector += 8;
            }
        }

        if (Vector != 0) {
            SignBit = VP6_DecodeBool(&pbi->br, pbi->MvSignProbs[i]);
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

void VP6_FindNearestandNextNearest(struct PB_INSTANCE *pbi, unsigned int MBrow, unsigned int MBcol,
                                   unsigned char Frame, int *type) {
    int i;
    unsigned int OffsetMB;
    unsigned int BaseMB;
    int Nearest = 0;
    int NextNearest = 0;
    int nearestIndex;
    unsigned int thisMv;
    int typet;

    typet = 1;

    BaseMB = MBrow * pbi->MBCols + MBcol;

    for (i = 0; i < 12; i++) {
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;

        if (VP6_Mode2Frame[pbi->predictionMode[OffsetMB]] != Frame) {
            continue;
        }

        thisMv = *((unsigned int *)&pbi->MBMotionVector[OffsetMB]);

        if (thisMv) {
            *((unsigned int *)&Nearest) = thisMv;
            typet = 2;
            break;
        }
    }

    nearestIndex = i;

    for (i = i + 1; i < 12; i++) {
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;

        if (VP6_Mode2Frame[pbi->predictionMode[OffsetMB]] != Frame) {
            continue;
        }

        thisMv = *((unsigned int *)&pbi->MBMotionVector[OffsetMB]);

        if (thisMv == *((unsigned int *)&Nearest)) {
            continue;
        }

        if (thisMv) {
            *((unsigned int *)&NextNearest) = thisMv;
            typet = 0;
            break;
        }
    }

    if (Frame == 1) {
        *type = typet;
        pbi->mbi.NearestMvIndex = nearestIndex;
        *(unsigned int *)&pbi->mbi.NearestInterMVect = *(unsigned int *)&Nearest;
        *(unsigned int *)&pbi->mbi.NearInterMVect = *(unsigned int *)&NextNearest;
    } else {
        pbi->mbi.NearestGMvIndex = nearestIndex;
        *(unsigned int *)&pbi->mbi.NearestGoldMVect = *(unsigned int *)&Nearest;
        *(unsigned int *)&pbi->mbi.NearGoldMVect = *(unsigned int *)&NextNearest;
    }
}

/* UNSOLVED r36f, 740 B al 94,3946 % (40 filas, tamano YA exacto). El DWARF del
   original da el reparto entero: SrcPtr r27, TempBuffer r29, TempPtr1 r8,
   TempPtr2 r27, ModX r28, ModY r25, BicMvSizeLimit r6, Stride r26, MvShift r30,
   MvModMask r3, y en el bloque interior mVx r10, mVy r0, mx r10, my r0.
   Los cinco pines que ya estaban puestos (r24/r23/r27/r25/r26) son correctos.

   MEDIDO en la r36f y NO aplicado (cero bytes con asm es deuda):
     - `__asm__("" : "+r"(mVx))` detras de las declaraciones del bloque interior
       baja de 40 a 29 filas y DEJA LA COLA ENTERA CASADA: arregla el registro
       de TempPtr1 (r8 como dice el DWARF, no r10) y con el el orden de
       preparacion de los argumentos de las cuatro llamadas a FilterBlock
       (`add r3,r29,r8` antes de `add r4,r29,r27`). El fuzzy baja a 93,5622 %
       porque las filas que quedan se concentran en el primer bloque.
     - encima de esa, `"+r"(ModY)` deja 28 filas / 94,1297 %; `"+r"(ModX)` 32;
       las dos juntas, el eje "a quien" y la barrera entre las dos asignaciones,
       31 (94,4811 %).
     - `"+r"(mVy)` solo: 55 filas y 736 B. Las dos a la vez: 46.
   NEGATIVOS de fuente: las CUATRO reordenaciones del bloque ModX/ModY/mx/my
   (y/x, por parejas en los dos sentidos, my antes de mx) dan EL MISMO OBJETO:
   GCC canonicaliza el orden de esas cuatro sentencias.
   NEGATIVOS de pin (todos peores, sobre la base limpia): TempPtr1 r8 -> 57
   filas y 736 B, mVx r10 -> 56, mVy r0 -> 53, ModX r28 -> 61 y 736 B,
   TempBuffer r29 -> 51. Es el sintoma que describe el brief: la primera
   diferencia que no es de registro (el adelanto de `lwz 0x88(r31)` por delante
   del `lhax` de mVx) va antes que cualquier pin.

   r46. Atacado ese adelanto DESDE LA FUENTE, reasociando la sentencia de
   TempBuffer --que es de donde sale el `lwz 0x88(r31)`--, sin un solo asm:
     base .................................................. 94,39459 %
     `my * pbi->mbi.FrameReconStride` (factores al reves) ... 94,556755 %
     `... + mx + FrameReconStride * my` .................... 90,98919 %
     partida en dos sentencias ............................. 91,0973 %
     el producto delante del todo .......................... 92,23243 %
   O sea que el orden de los factores mueve 0,16 pt y nada mas: el adelanto no
   se corrige reasociando. Sigue siendo scheduling. Los 740 B son exactos en
   todos los casos.

   DEUDA DE DATOS de esta unidad, aparte de la funcion: `.rodata` mide 632 B en
   nuestro objeto y 636 en el objetivo. Los 4 B son una palabra a CERO al final
   de la seccion, DETRAS de `loMaskTbl_VP60` --el simbolo mide 132 B en los dos,
   comprobado declarandolo [34]: eso lo sube a 136 y lo rompe--. Falta un objeto
   const de 4 B a cero al final de .rodata, no una entrada de la tabla.

   r47. Dos ejes barridos ENTEROS y los dos negativos; el frente sigue siendo
   el adelanto del `lwz 0x88(r31)`, que es scheduling puro.
     LOS CINCO PINES QUE HAY PUESTOS SON UN CONJUNTO, no cinco decisiones
     sueltas: quitados de uno en uno, OutputPtr(r24), TempPtr2(r27) y
     Stride(r26) son NEUTROS (94,39459 % los tres) y solo bp (94,28648 %) y
     ModY (94,01621 %) bajan; pero quitados por parejas caen a 93,50 %-94,29 %
     y quitados los cinco a 93,23243 %. Barridas las 5 supresiones sueltas y
     las 10 parejas: ninguna mejora. No tocar el conjunto.
     PINES NUEVOS guiados por el DWARF, uno a uno sobre la base de los cinco:
     TempPtr1->r8 736 B / 92,52973 %, ModX->r28 736 B / 92,03243 %,
     MvModMask->r3 92,65946 %, SrcPtr->r27 93,69190 %, TempBuffer->r29
     93,80000 %, BicMvSizeLimit->r6 94,28648 %, MvShift->r30 94,39459 %
     (exactamente neutro). Ninguno mejora: el eje del pin esta agotado.
     ORDEN DE SENTENCIAS del bloque interior, ocho formas (ModY antes de ModX,
     my antes de mx, las dos, cadena y entera y luego cadena x, cadena x y
     luego y): las seis primeras dan EL MISMO OBJETO (94,39459 %) y las dos que
     funden el desplazamiento en la asignacion bajan a 93,80000 %.
     COLA del bloque, ocho formas: `Stride` delante 94,39459 %, `TempPtr1 =
     TempPtr2 = 0` delante 94,39459 %, `TempPtr2 = 0; TempPtr1 = 0;` separadas
     94,39459 %, al reves 94,17838 %, `my * FrameReconStride` 94,556755 %
     (el mejor, como en r46, y sigue siendo 0,16 pt sin un byte),
     parentizar `(SrcPtr + Recon) + (...)` 89,75676 %, `mx` antes del producto
     90,98919 %.

   r48. LA DEUDA DE DATOS ESTA CERRADA (ver el `asm` de fichero detras de
   `loMaskTbl_VP60`): .rodata pasa de 632 a 636 B y sale BYTE-IDENTICA al objeto
   extraido (sha256 11fa9fa36e095736 los dos), y `python scripts/promote.py
   Packages/vp6/1.0.6/source/decode/gc/criticalpath` pasa de
   ".rodata mide 632 B y el extraido 636 B" a "LIMPIA: se puede marcar
   Matching". .data y .sdata ya eran identicas. Lo UNICO que separa a esta
   unidad de promocionar --y son 12.040 B de .text de `linked`-- es esta
   funcion.

   r48, la funcion. Frente sin cerrar, pero el suelo baja de 40 filas a 25 con
   DOS palancas, y conviene medir el trabajo en FILAS y no en fuzzy (el fuzzy
   baja cuando las filas se concentran):
     1) `__asm__("" : "+r"(mVx));` detras de las declaraciones del bloque
        interior --la de r36f-- deja 29 filas (93,562164 %) y CASA LA COLA
        ENTERA: TempPtr1 cae en r8 como dice el DWARF y con el se arregla el
        orden de `add r3,r29,r8` / `add r4,r29,r27` en las cuatro llamadas a
        FilterBlock.
     2) NUEVA en r48: sacar `Recon` a una local con barrera,
          { int rec = pbi->mbi.Recon;
            __asm__("" : "+r"(rec));
            TempBuffer = SrcPtr + rec + pbi->mbi.FrameReconStride * my + mx; }
        baja a 25 filas / 95,0 %, tamano exacto. Es la misma idea que cerro
        `dvd_device` en r46: la palanca esta en el operando que NO sale en el
        diff.
   Lo que queda en esas 25 filas son DOS cosas y las dos son sched1:
     - nuestro `lwz r8,0x88(r31)` (FrameReconStride) ocupa la ranura 28, que en
       el objetivo es el `lhax` de mVx; el objetivo lo emite en la 33.
     - nuestros `and` de ModX y ModY se HUNDEN al final del bloque (indices
       51/52) y en el objetivo estan en 32 y 40, pegados a cada `lhax`. Por eso
       el objetivo reutiliza el registro (mVy y my comparten r0, mVx y mx
       comparten r10, como dice el DWARF) y nosotros no.
     - mas 5 filas en la rama `else`, que son solo el registro de MvModMask
       (r9 en el objetivo, r0 en el nuestro) y caeran solas cuando caiga el
       bloque de arriba.
   Medido y NEGATIVO en r48 sobre esas dos palancas (todo 740/740 salvo nota):
     asociatividad de TempBuffer: `+ (stride*my + mx)` 53 filas, `(my*stride +
     mx)` 54, `SrcPtr + (Recon + (...))` 59, `(mx + stride*my)` 51, `my*stride`
     sin parentesis 30; sobre la base limpia daban 48/48/60/43/34.
     orden de sentencias del bloque interior (ModY antes, cadena y entera,
     my antes de mx): LAS TRES EL MISMO OBJETO, 29 filas.
     `asm` de SOLO ENTRADA en este bloque NO es de cero bytes: `: : "r"(mVx)`
     744 B / 55 filas, sobre mx/my/SrcPtr/ModX+ModY 744 B / 64 filas, sobre
     ModY al final 744 B / 62. Aqui la forma que vale es el inout.
     barrera de ranura `__volatile__("")` delante de ModX 736 B / 57 filas y al
     final del bloque 736 B / 58.
     inout sobre otras variables, sobre la base de 25: ModY tras ModY 27,
     ModX tras ModX 31, los dos 30, mVx+mVy juntos 33, solo mVy 736 B / 55,
     `rec`+`my` 32, `rec`+`mx` 53, `rec`+`ModY` 26, `rec`+`pbi` 744 B / 63,
     `rec` con `pbi` de entrada 25 (identico), `rec` volatil 736 B / 52,
     `rec` de solo entrada 744 B / 64, local `bse = SrcPtr + Recon` con barrera
     30, `frs` local con barrera 44, `frs` local sin barrera 29,
     `TempBuffer = ... + mx; TempBuffer += stride*my;` 40 filas.
     inout sobre `pbi` en cualquier sitio: 744 B y de 55 a 63 filas.
   Las dos palancas de r48 son CERO BYTES con el tamano ya exacto, asi que por
   la regla 9 del brief NO se dejan puestas. Se reaplican en un minuto y el
   siguiente empieza en 25 filas, no en 40.  */

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

    if (pbi->VpProfile == 0 || pbi->UseLoopFilter == 0) {
        int mVx = pbi->mbi.Mv[bp].x;
        int mVy = pbi->mbi.Mv[bp].y;
        int mx;
        int my;

        ModX = mVx & MvModMask;
        ModY = mVy & MvModMask;

        mVx += MvModMask & (mVx >> 31);
        mVy += MvModMask & (mVy >> 31);

        mx = mVx >> MvShift;
        my = mVy >> MvShift;

        TempBuffer = SrcPtr + pbi->mbi.Recon + (pbi->mbi.FrameReconStride * my + mx);
        Stride = pbi->mbi.CurrentReconStride;
        TempPtr1 = TempPtr2 = 0;
    } else {
        VP6_PredictFiltered(pbi, SrcPtr + pbi->mbi.Recon, pbi->mbi.Mv[bp].x, pbi->mbi.Mv[bp].y);
        TempBuffer = pbi->LoopFilteredBlock;
        Stride = 16;
        TempPtr1 = 34;
        TempPtr2 = 34;

        ModX = pbi->mbi.Mv[bp].x & pbi->mbi.MvModMask;
        ModY = pbi->mbi.Mv[bp].y & pbi->mbi.MvModMask;
    }

    if (ModX) {
        if (pbi->mbi.Mv[bp].x > 0) {
            TempPtr2++;
        } else {
            TempPtr2--;
        }
    }

    if (ModY) {
        if (pbi->mbi.Mv[bp].y <= 0) {
            TempPtr2 -= Stride;
        } else {
            TempPtr2 += Stride;
        }
    }

    if (TempPtr1 != TempPtr2) {
        if (bp <= 3) {
            ModX = ModX << 1;
            ModY = ModY << 1;

            if (pbi->VpProfile) {
                if (pbi->PredictionFilterMode == 2) {
                    if (pbi->PredictionFilterMvSizeThresh) {
                        BicMvSizeLimit = 4 << (pbi->PredictionFilterMvSizeThresh - 1);
                    } else {
                        BicMvSizeLimit = 128;
                    }

                    if (pbi->PredictionFilterMvSizeThresh == 0 ||
                        (abs(pbi->mbi.Mv[bp].x) <= BicMvSizeLimit &&
                         abs(pbi->mbi.Mv[bp].y) <= BicMvSizeLimit)) {
                        if (pbi->PredictionFilterVarThresh) {
                            IVar = Var16Point(TempBuffer + TempPtr1, Stride);
                            FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                        (unsigned short *)OutputPtr, Stride, ModX, ModY,
                                        IVar >= pbi->PredictionFilterVarThresh);
                        } else {
                            FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                        (unsigned short *)OutputPtr, Stride, ModX, ModY, 1);
                        }
                        return;
                    }
                } else {
                    FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2,
                                (unsigned short *)OutputPtr, Stride, ModX, ModY,
                                pbi->PredictionFilterMode == 1);
                    return;
                }
            }
        }

        FilterBlock(TempBuffer + TempPtr1, TempBuffer + TempPtr2, (unsigned short *)OutputPtr,
                    Stride, ModX, ModY, 0);
    } else {
        UnpackBlock(TempBuffer + TempPtr1, OutputPtr, Stride);
    }
}

void VP6_ReconstructBlock(struct PB_INSTANCE *pbi, BLOCK_POSITION bp) {
    if (pbi->mbi.Mode == CODE_INTER_NO_MV) {
        ReconInter(pbi->TmpDataBuffer, pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->LastFrameRecon + pbi->mbi.Recon, pbi->ReconDataBuffer,
                   pbi->mbi.CurrentReconStride);
    } else if (VP6_ModeUsesMC[pbi->mbi.Mode]) {
        VP6_PredictFilteredBlock(pbi, pbi->TmpDataBuffer, bp);
        ReconBlock(pbi->TmpDataBuffer, pbi->ReconDataBuffer,
                   pbi->ThisFrameRecon + pbi->mbi.Recon, pbi->mbi.CurrentReconStride);
    } else if (pbi->mbi.Mode == CODE_USING_GOLDEN) {
        ReconInter(pbi->TmpDataBuffer, pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->GoldenFrame + pbi->mbi.Recon, pbi->ReconDataBuffer,
                   pbi->mbi.CurrentReconStride);
    } else {
        ReconIntra(pbi->TmpDataBuffer, pbi->ThisFrameRecon + pbi->mbi.Recon,
                   pbi->ReconDataBuffer, pbi->mbi.CurrentReconStride);
    }
}

void ScalarReconIntra_GC(short *TmpDataBuffer, unsigned char *ReconPtr, unsigned short *ChangePtr,
                         unsigned int LineStep) {
    asm("	li 9,f128@sda21
	li 0,0x8
	psq_l 4,0x0(9),1,0
	mtctr 0
	ps_merge00 4,4,4
intra_loop:
	psq_l 0,0x0(5),0,5
	psq_l 1,0x4(5),0,5
	psq_l 2,0x8(5),0,5
	psq_l 3,0xc(5),0,5
	addi 5,5,0x10
	ps_add 0,0,4
	ps_add 1,1,4
	ps_add 2,2,4
	ps_add 3,3,4
	psq_st 0,0x0(4),0,2
	psq_st 1,0x2(4),0,2
	psq_st 2,0x4(4),0,2
	psq_st 3,0x6(4),0,2
	add 4,4,6
	bdnz intra_loop
");
}

void ScalarReconInter_GC(short *TmpDataBuffer, unsigned char *ReconPtr, unsigned char *RefPtr,
                         short *ChangePtr, unsigned int LineStep) {
    asm("	li 0,0x8
	mtctr 0
inter_loop:
	psq_l 0,0x0(5),0,2
	psq_l 1,0x2(5),0,2
	psq_l 2,0x4(5),0,2
	psq_l 3,0x6(5),0,2
	add 5,5,7
	psq_l 4,0x0(6),0,5
	psq_l 5,0x4(6),0,5
	psq_l 6,0x8(6),0,5
	psq_l 7,0xc(6),0,5
	addi 6,6,0x10
	ps_add 0,0,4
	ps_add 1,1,5
	ps_add 2,2,6
	ps_add 3,3,7
	psq_st 0,0x0(4),0,2
	psq_st 1,0x2(4),0,2
	psq_st 2,0x4(4),0,2
	psq_st 3,0x6(4),0,2
	add 4,4,7
	bdnz inter_loop
");
}

void ReconBlock_GC(short *SrcBlock, short *ReconRefPtr, unsigned char *DestBlock,
                   unsigned int LineStep) {
    asm("	li 0,0x8
	mtctr 0
block_loop:
	psq_l 0,0x0(3),0,5
	psq_l 1,0x4(3),0,5
	psq_l 2,0x8(3),0,5
	psq_l 3,0xc(3),0,5
	addi 3,3,0x10
	psq_l 4,0x0(4),0,5
	psq_l 5,0x4(4),0,5
	psq_l 6,0x8(4),0,5
	psq_l 7,0xc(4),0,5
	addi 4,4,0x10
	ps_add 0,0,4
	ps_add 1,1,5
	ps_add 2,2,6
	ps_add 3,3,7
	psq_st 0,0x0(5),0,2
	psq_st 1,0x2(5),0,2
	psq_st 2,0x4(5),0,2
	psq_st 3,0x6(5),0,2
	add 5,5,6
	bdnz block_loop
");
}

void IDct64_GC(short *InputData, short *QuantMatrix, short *OutputData) {
    asm("	stwu 1,-0x98(1)
	stfd 14,0x8(1)
	stfd 15,0x10(1)
	stfd 16,0x18(1)
	stfd 17,0x20(1)
	stfd 18,0x28(1)
	stfd 19,0x30(1)
	stfd 20,0x38(1)
	stfd 21,0x40(1)
	stfd 22,0x48(1)
	stfd 23,0x50(1)
	stfd 24,0x58(1)
	stfd 25,0x60(1)
	stfd 26,0x68(1)
	stfd 27,0x70(1)
	stfd 28,0x78(1)
	stfd 29,0x80(1)
	stfd 30,0x88(1)
	stfd 31,0x90(1)
	lis 11,idctconstants@ha
	addi 11,11,idctconstants@l
	mflr 6
	psq_l 0,0x0(11),1,0
	psq_l 1,0x4(11),1,0
	psq_l 2,0x8(11),1,0
	psq_l 3,0xc(11),1,0
	psq_l 4,0x10(11),1,0
	psq_l 5,0x14(11),1,0
	psq_l 6,0x18(11),1,0
	psq_l 7,0x1c(11),1,0
	ps_merge00 0,0,0
	ps_merge00 1,1,1
	ps_merge00 2,2,2
	ps_merge00 3,3,3
	ps_merge00 4,4,4
	ps_merge00 5,5,5
	ps_merge00 6,6,6
	ps_merge00 7,7,7
	psq_l 16,0x0(3),1,5
	psq_l 17,0x2(3),1,5
	psq_l 18,0x4(3),1,5
	psq_l 19,0x8(3),1,5
	psq_l 20,0x0(4),1,5
	psq_l 21,0x2(4),1,5
	psq_l 22,0x4(4),1,5
	psq_l 23,0x8(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 8,16,20
	ps_mul 9,17,21
	psq_l 16,0xa(3),1,5
	psq_l 17,0xc(3),1,5
	psq_l 18,0xe(3),1,5
	psq_l 19,0x1a(3),1,5
	psq_l 20,0xa(4),1,5
	psq_l 21,0xc(4),1,5
	psq_l 22,0xe(4),1,5
	psq_l 23,0x1a(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 10,16,20
	ps_mul 11,17,21
	psq_l 16,0x1c(3),1,5
	psq_l 17,0x1e(3),1,5
	psq_l 18,0x20(3),1,5
	psq_l 19,0x34(3),1,5
	psq_l 20,0x1c(4),1,5
	psq_l 21,0x1e(4),1,5
	psq_l 22,0x20(4),1,5
	psq_l 23,0x34(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 12,16,20
	ps_mul 13,17,21
	psq_l 16,0x36(3),1,5
	psq_l 17,0x38(3),1,5
	psq_l 18,0x3a(3),1,5
	psq_l 19,0x54(3),1,5
	psq_l 20,0x36(4),1,5
	psq_l 21,0x38(4),1,5
	psq_l 22,0x3a(4),1,5
	psq_l 23,0x54(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 14,16,20
	ps_mul 15,17,21
	bl idctcolumn64
	b idct64_resume
idctcolumn64:
	ps_mul 16,1,9
	ps_mul 17,3,11
	ps_mul 18,7,9
	ps_mul 19,3,13
	ps_madd 16,7,15,16
	ps_madd 17,5,13,17
	ps_nmsub 18,1,15,18
	ps_nmsub 19,5,11,19
	ps_sub 20,16,17
	ps_add 17,16,17
	ps_sub 21,18,19
	ps_add 19,19,19
	ps_mul 16,4,20
	ps_mul 18,4,21
	ps_add 20,8,12
	ps_mul 21,2,10
	ps_sub 22,8,12
	ps_mul 23,6,10
	ps_mul 20,4,20
	ps_madd 21,6,14,21
	ps_mul 22,4,22
	ps_nmsub 23,2,14,23
	ps_sub 24,20,21
	ps_add 21,20,21
	ps_add 25,22,16
	ps_sub 26,18,23
	ps_sub 22,22,16
	ps_add 23,18,23
	ps_add 8,21,17
	ps_sub 15,21,17
	ps_add 9,25,23
	ps_sub 10,25,23
	ps_add 11,24,19
	ps_sub 12,24,19
	ps_add 13,22,26
	ps_sub 14,22,26
	blr
idct64_resume:
	ps_merge00 16,8,9
	ps_merge00 17,10,11
	ps_merge00 18,12,13
	ps_merge00 19,14,15
	ps_merge11 20,8,9
	ps_merge11 21,10,11
	ps_merge11 22,12,13
	ps_merge11 23,14,15
	psq_st 16,0x0(5),0,5
	psq_st 17,0x4(5),0,5
	psq_st 18,0x8(5),0,5
	psq_st 19,0xc(5),0,5
	psq_st 20,0x10(5),0,5
	psq_st 21,0x14(5),0,5
	psq_st 22,0x18(5),0,5
	psq_st 23,0x1c(5),0,5
	psq_l 16,0x6(3),1,5
	psq_l 17,0x10(3),1,5
	psq_l 18,0x12(3),1,5
	psq_l 19,0x16(3),1,5
	psq_l 20,0x6(4),1,5
	psq_l 21,0x10(4),1,5
	psq_l 22,0x12(4),1,5
	psq_l 23,0x16(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 8,16,20
	ps_mul 9,17,21
	psq_l 16,0x18(3),1,5
	psq_l 17,0x22(3),1,5
	psq_l 18,0x24(3),1,5
	psq_l 19,0x30(3),1,5
	psq_l 20,0x18(4),1,5
	psq_l 21,0x22(4),1,5
	psq_l 22,0x24(4),1,5
	psq_l 23,0x30(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 10,16,20
	ps_mul 11,17,21
	psq_l 16,0x32(3),1,5
	psq_l 17,0x3c(3),1,5
	psq_l 18,0x3e(3),1,5
	psq_l 19,0x50(3),1,5
	psq_l 20,0x32(4),1,5
	psq_l 21,0x3c(4),1,5
	psq_l 22,0x3e(4),1,5
	psq_l 23,0x50(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 12,16,20
	ps_mul 13,17,21
	psq_l 16,0x52(3),1,5
	psq_l 17,0x56(3),1,5
	psq_l 18,0x58(3),1,5
	psq_l 19,0x6a(3),1,5
	psq_l 20,0x52(4),1,5
	psq_l 21,0x56(4),1,5
	psq_l 22,0x58(4),1,5
	psq_l 23,0x6a(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 14,16,20
	ps_mul 15,17,21
	bl idctcolumn64
	ps_merge00 16,8,9
	ps_merge00 17,10,11
	ps_merge00 18,12,13
	ps_merge00 19,14,15
	ps_merge11 20,8,9
	ps_merge11 21,10,11
	ps_merge11 22,12,13
	ps_merge11 23,14,15
	psq_st 16,0x20(5),0,5
	psq_st 17,0x24(5),0,5
	psq_st 18,0x28(5),0,5
	psq_st 19,0x2c(5),0,5
	psq_st 20,0x30(5),0,5
	psq_st 21,0x34(5),0,5
	psq_st 22,0x38(5),0,5
	psq_st 23,0x3c(5),0,5
	psq_l 16,0x14(3),1,5
	psq_l 17,0x26(3),1,5
	psq_l 18,0x28(3),1,5
	psq_l 19,0x2c(3),1,5
	psq_l 20,0x14(4),1,5
	psq_l 21,0x26(4),1,5
	psq_l 22,0x28(4),1,5
	psq_l 23,0x2c(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 8,16,20
	ps_mul 9,17,21
	psq_l 16,0x2e(3),1,5
	psq_l 17,0x40(3),1,5
	psq_l 18,0x42(3),1,5
	psq_l 19,0x4c(3),1,5
	psq_l 20,0x2e(4),1,5
	psq_l 21,0x40(4),1,5
	psq_l 22,0x42(4),1,5
	psq_l 23,0x4c(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 10,16,20
	ps_mul 11,17,21
	psq_l 16,0x4e(3),1,5
	psq_l 17,0x5a(3),1,5
	psq_l 18,0x5c(3),1,5
	psq_l 19,0x66(3),1,5
	psq_l 20,0x4e(4),1,5
	psq_l 21,0x5a(4),1,5
	psq_l 22,0x5c(4),1,5
	psq_l 23,0x66(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 12,16,20
	ps_mul 13,17,21
	psq_l 16,0x68(3),1,5
	psq_l 17,0x6c(3),1,5
	psq_l 18,0x6e(3),1,5
	psq_l 19,0x78(3),1,5
	psq_l 20,0x68(4),1,5
	psq_l 21,0x6c(4),1,5
	psq_l 22,0x6e(4),1,5
	psq_l 23,0x78(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 14,16,20
	ps_mul 15,17,21
	bl idctcolumn64
	ps_merge00 16,8,9
	ps_merge00 17,10,11
	ps_merge00 18,12,13
	ps_merge00 19,14,15
	ps_merge11 20,8,9
	ps_merge11 21,10,11
	ps_merge11 22,12,13
	ps_merge11 23,14,15
	psq_st 16,0x40(5),0,5
	psq_st 17,0x44(5),0,5
	psq_st 18,0x48(5),0,5
	psq_st 19,0x4c(5),0,5
	psq_st 20,0x50(5),0,5
	psq_st 21,0x54(5),0,5
	psq_st 22,0x58(5),0,5
	psq_st 23,0x5c(5),0,5
	psq_l 16,0x2a(3),1,5
	psq_l 17,0x44(3),1,5
	psq_l 18,0x46(3),1,5
	psq_l 19,0x48(3),1,5
	psq_l 20,0x2a(4),1,5
	psq_l 21,0x44(4),1,5
	psq_l 22,0x46(4),1,5
	psq_l 23,0x48(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 8,16,20
	ps_mul 9,17,21
	psq_l 16,0x4a(3),1,5
	psq_l 17,0x5e(3),1,5
	psq_l 18,0x60(3),1,5
	psq_l 19,0x62(3),1,5
	psq_l 20,0x4a(4),1,5
	psq_l 21,0x5e(4),1,5
	psq_l 22,0x60(4),1,5
	psq_l 23,0x62(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 10,16,20
	ps_mul 11,17,21
	psq_l 16,0x64(3),1,5
	psq_l 17,0x70(3),1,5
	psq_l 18,0x72(3),1,5
	psq_l 19,0x74(3),1,5
	psq_l 20,0x64(4),1,5
	psq_l 21,0x70(4),1,5
	psq_l 22,0x72(4),1,5
	psq_l 23,0x74(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 12,16,20
	ps_mul 13,17,21
	psq_l 16,0x76(3),1,5
	psq_l 17,0x7a(3),1,5
	psq_l 18,0x7c(3),1,5
	psq_l 19,0x7e(3),1,5
	psq_l 20,0x76(4),1,5
	psq_l 21,0x7a(4),1,5
	psq_l 22,0x7c(4),1,5
	psq_l 23,0x7e(4),1,5
	ps_merge00 16,16,18
	ps_merge00 17,17,19
	ps_merge00 20,20,22
	ps_merge00 21,21,23
	ps_mul 14,16,20
	ps_mul 15,17,21
	bl idctcolumn64
	ps_merge00 16,8,9
	ps_merge00 17,10,11
	ps_merge00 18,12,13
	ps_merge00 19,14,15
	ps_merge11 20,8,9
	ps_merge11 21,10,11
	ps_merge11 22,12,13
	ps_merge11 23,14,15
	psq_st 16,0x60(5),0,5
	psq_st 17,0x64(5),0,5
	psq_st 18,0x68(5),0,5
	psq_st 19,0x6c(5),0,5
	psq_st 20,0x70(5),0,5
	psq_st 21,0x74(5),0,5
	psq_st 22,0x78(5),0,5
	psq_st 23,0x7c(5),0,5
	li 0,0x4
	mtctr 0
idct64_loop:
	psq_l 8,0x0(5),0,5
	psq_l 9,0x10(5),0,5
	psq_l 10,0x20(5),0,5
	psq_l 11,0x30(5),0,5
	psq_l 12,0x40(5),0,5
	psq_l 13,0x50(5),0,5
	psq_l 14,0x60(5),0,5
	psq_l 15,0x70(5),0,5
	bl idctcolumn64
	ps_add 8,8,0
	ps_add 9,9,0
	ps_add 10,10,0
	ps_add 11,11,0
	ps_add 12,12,0
	ps_add 13,13,0
	ps_add 14,14,0
	ps_add 15,15,0
	psq_st 8,0x0(5),0,5
	psq_st 9,0x10(5),0,5
	psq_st 10,0x20(5),0,5
	psq_st 11,0x30(5),0,5
	psq_st 12,0x40(5),0,5
	psq_st 13,0x50(5),0,5
	psq_st 14,0x60(5),0,5
	psq_st 15,0x70(5),0,5
	addi 5,5,0x4
	bdnz idct64_loop
	li 0,0x10
	subi 5,5,0x10
	mtctr 0
idct64_scale:
	lha 7,0x0(5)
	lha 8,0x2(5)
	lha 9,0x4(5)
	lha 10,0x6(5)
	srawi 7,7,4
	srawi 8,8,4
	srawi 9,9,4
	srawi 10,10,4
	sth 7,0x0(5)
	sth 8,0x2(5)
	sth 9,0x4(5)
	sth 10,0x6(5)
	addi 5,5,0x8
	bdnz idct64_scale
	mtlr 6
	lfd 14,0x8(1)
	lfd 15,0x10(1)
	lfd 16,0x18(1)
	lfd 17,0x20(1)
	lfd 18,0x28(1)
	lfd 19,0x30(1)
	lfd 20,0x38(1)
	lfd 21,0x40(1)
	lfd 22,0x48(1)
	lfd 23,0x50(1)
	lfd 24,0x58(1)
	lfd 25,0x60(1)
	lfd 26,0x68(1)
	lfd 27,0x70(1)
	lfd 28,0x78(1)
	lfd 29,0x80(1)
	lfd 30,0x88(1)
	lfd 31,0x90(1)
	addi 1,1,0x98
");
}

void IDct1_GC(short *InputData, short *QuantMatrix, short *OutputData) {
    unsigned int *dest = (unsigned int *)OutputData;
    unsigned int out;
    int i;

    out = (InputData[0] * QuantMatrix[0] + 15) >> 5;
    out = (out << 16) | (out & 0xFFFF);

    for (i = 0; i < 8; i++) {
        dest[0] = out;
        dest[1] = out;
        dest[2] = out;
        dest[3] = out;
        dest += 4;
    }
}

void UnpackBlock_GC(unsigned char *ReconPtr, short *ReconRefPtr, unsigned int ReconPixelsPerLine) {
    asm("	li 0,0x8
	mtctr 0
unpack_loop:
	psq_l 0,0x0(3),0,2
	psq_l 1,0x2(3),0,2
	psq_l 2,0x4(3),0,2
	psq_l 3,0x6(3),0,2
	add 3,3,5
	psq_st 0,0x0(4),0,5
	psq_st 1,0x4(4),0,5
	psq_st 2,0x8(4),0,5
	psq_st 3,0xc(4),0,5
	addi 4,4,0x10
	bdnz unpack_loop
");
}

void FilterBlock1dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, unsigned int PixelStep, int *Filter) {
    asm("	li 9,f64@sda21
	li 0,0x8
	psq_l 8,0x3(7),1,2
	psq_l 9,0x7(7),1,2
	psq_l 10,0x0(9),1,0
	mtctr 0
	add 6,3,6
	ps_merge00 10,10,10
filt1d_loop:
	psq_l 0,0x0(3),0,2
	psq_l 1,0x2(3),0,2
	psq_l 2,0x4(3),0,2
	psq_l 3,0x6(3),0,2
	add 3,3,5
	psq_l 4,0x0(6),0,2
	psq_l 5,0x2(6),0,2
	psq_l 6,0x4(6),0,2
	psq_l 7,0x6(6),0,2
	add 6,6,5
	ps_madds0 0,0,8,10
	ps_madds0 1,1,8,10
	ps_madds0 2,2,8,10
	ps_madds0 3,3,8,10
	ps_madds0 0,4,9,0
	ps_madds0 1,5,9,1
	ps_madds0 2,6,9,2
	ps_madds0 3,7,9,3
	psq_st 0,0x0(4),0,6
	psq_st 1,0x4(4),0,6
	psq_st 2,0x8(4),0,6
	psq_st 3,0xc(4),0,6
	addi 4,4,0x10
	bdnz filt1d_loop
");
}

void FilterBlock2dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                         unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter) {
    static unsigned short FData[72];
    asm("	lis 9,%0@ha
	li 11,f64@sda21
	addi 9,9,%0@l
	li 0,0x9
	psq_l 8,0x3(6),1,2
	psq_l 9,0x7(6),1,2
	psq_l 10,0x0(11),1,0
	mtctr 0
	addi 6,3,0x1
	ps_merge00 10,10,10
filt2d_loop1:
	psq_l 0,0x0(3),0,2
	psq_l 1,0x2(3),0,2
	psq_l 2,0x4(3),0,2
	psq_l 3,0x6(3),0,2
	add 3,3,5
	psq_l 4,0x0(6),0,2
	psq_l 5,0x2(6),0,2
	psq_l 6,0x4(6),0,2
	psq_l 7,0x6(6),0,2
	add 6,6,5
	ps_madds0 0,0,8,10
	ps_madds0 1,1,8,10
	ps_madds0 2,2,8,10
	ps_madds0 3,3,8,10
	ps_madds0 0,4,9,0
	ps_madds0 1,5,9,1
	ps_madds0 2,6,9,2
	ps_madds0 3,7,9,3
	psq_st 0,0x0(9),0,6
	psq_st 1,0x4(9),0,6
	psq_st 2,0x8(9),0,6
	psq_st 3,0xc(9),0,6
	addi 9,9,0x10
	bdnz filt2d_loop1
	li 0,0x8
	subi 3,9,0x90
	psq_l 8,0x3(7),1,2
	psq_l 9,0x7(7),1,2
	mtctr 0
	addi 6,3,0x10
filt2d_loop2:
	psq_l 0,0x0(3),0,3
	psq_l 1,0x4(3),0,3
	psq_l 2,0x8(3),0,3
	psq_l 3,0xc(3),0,3
	addi 3,3,0x10
	psq_l 4,0x0(6),0,3
	psq_l 5,0x4(6),0,3
	psq_l 6,0x8(6),0,3
	psq_l 7,0xc(6),0,3
	addi 6,6,0x10
	ps_madds0 0,0,8,10
	ps_madds0 1,1,8,10
	ps_madds0 2,2,8,10
	ps_madds0 3,3,8,10
	ps_madds0 0,4,9,0
	ps_madds0 1,5,9,1
	ps_madds0 2,6,9,2
	ps_madds0 3,7,9,3
	psq_st 0,0x0(4),0,6
	psq_st 1,0x4(4),0,6
	psq_st 2,0x8(4),0,6
	psq_st 3,0xc(4),0,6
	addi 4,4,0x10
	bdnz filt2d_loop2
" : : "s"(FData));
}

void FilterBlock_GC(unsigned char *ReconPtr1, unsigned char *ReconPtr2, unsigned short *ReconRefPtr,
                    unsigned int PixelsPerLine, int ModX, int ModY, int UseBicubic) {
    int diff = ReconPtr2 - ReconPtr1;

    if (diff < 0) {
        unsigned char *temp = ReconPtr1;
        ReconPtr1 = ReconPtr2;
        ReconPtr2 = temp;
        diff = ReconPtr2 - ReconPtr1;
    }

    if (!UseBicubic) {
        if (diff == 1) {
            FilterBlock1dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine, 1, BilinearFilters[ModX]);
        } else if (diff == PixelsPerLine) {
            FilterBlock1dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine,
                                BilinearFilters[ModY]);
        } else if (diff == PixelsPerLine - 1) {
            FilterBlock2dBil_GC(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX],
                                BilinearFilters[ModY]);
        } else {
            FilterBlock2dBil_GC(ReconPtr1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX],
                                BilinearFilters[ModY]);
        }
    } else {
        if (diff == 1) {
            FilterBlock1d(ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8, BicubicFilters[ModX]);
        } else if (diff == PixelsPerLine) {
            FilterBlock1d(ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine, 8, 8,
                          BicubicFilters[ModY]);
        } else if (diff == PixelsPerLine - 1) {
            FilterBlock2d(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine, BicubicFilters[ModX],
                          BicubicFilters[ModY]);
        } else {
            FilterBlock2d(ReconPtr1, ReconRefPtr, PixelsPerLine, BicubicFilters[ModX],
                          BicubicFilters[ModY]);
        }
    }
}
