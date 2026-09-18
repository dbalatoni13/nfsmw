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

typedef unsigned char UINT8;
typedef unsigned int UINT32;

extern UINT32 VP6_DecodeBool(BR_TYPE *br, int probability);
extern UINT32 VP6_DecodeBool128(BR_TYPE *br);
extern UINT32 VP6_bitread(BR_TYPE *br, int bits);
const UINT8 VP6_ModeVq[3][16][10][2] = {
    {
        {{9, 15}, {32, 25}, {7, 19}, {9, 21}, {1, 12}, {14, 12}, {3, 18}, {14, 23}, {3, 10}, {0, 4}},
        {{48, 39}, {1, 2}, {11, 27}, {29, 44}, {7, 27}, {1, 4}, {0, 3}, {1, 6}, {1, 2}, {0, 0}},
        {{21, 32}, {1, 2}, {4, 10}, {32, 43}, {6, 23}, {2, 3}, {1, 19}, {1, 6}, {12, 21}, {0, 7}},
        {{69, 83}, {0, 0}, {0, 2}, {10, 29}, {3, 12}, {0, 1}, {0, 3}, {0, 3}, {2, 2}, {0, 0}},
        {{11, 20}, {1, 4}, {18, 36}, {43, 48}, {13, 35}, {0, 2}, {0, 5}, {3, 12}, {1, 2}, {0, 0}},
        {{70, 44}, {0, 1}, {2, 10}, {37, 46}, {8, 26}, {0, 2}, {0, 2}, {0, 2}, {0, 1}, {0, 0}},
        {{8, 15}, {0, 1}, {8, 21}, {74, 53}, {22, 42}, {0, 1}, {0, 2}, {0, 3}, {1, 2}, {0, 0}},
        {{141, 42}, {0, 0}, {1, 4}, {11, 24}, {1, 11}, {0, 1}, {0, 1}, {0, 2}, {0, 0}, {0, 0}},
        {{8, 19}, {4, 10}, {24, 45}, {21, 37}, {9, 29}, {0, 3}, {1, 7}, {11, 25}, {0, 2}, {0, 1}},
        {{46, 42}, {0, 1}, {2, 10}, {54, 51}, {10, 30}, {0, 2}, {0, 2}, {0, 1}, {0, 1}, {0, 0}},
        {{28, 32}, {0, 0}, {3, 10}, {75, 51}, {14, 33}, {0, 1}, {0, 2}, {0, 1}, {1, 2}, {0, 0}},
        {{100, 46}, {0, 1}, {3, 9}, {21, 37}, {5, 20}, {0, 1}, {0, 2}, {1, 2}, {0, 1}, {0, 0}},
        {{27, 29}, {0, 1}, {9, 25}, {53, 51}, {12, 34}, {0, 1}, {0, 3}, {1, 5}, {0, 2}, {0, 0}},
        {{80, 38}, {0, 0}, {1, 4}, {69, 33}, {5, 16}, {0, 1}, {0, 1}, {0, 0}, {0, 1}, {0, 0}},
        {{16, 20}, {0, 0}, {2, 8}, {104, 49}, {15, 33}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {0, 0}},
        {{194, 16}, {0, 0}, {1, 1}, {1, 9}, {1, 3}, {0, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 0}},
    },
    {
        {{41, 22}, {1, 0}, {1, 31}, {0, 0}, {0, 0}, {0, 1}, {1, 7}, {0, 1}, {98, 25}, {4, 10}},
        {{123, 37}, {6, 4}, {1, 27}, {0, 0}, {0, 0}, {5, 8}, {1, 7}, {0, 1}, {12, 10}, {0, 2}},
        {{26, 14}, {14, 12}, {0, 24}, {0, 0}, {0, 0}, {55, 17}, {1, 9}, {0, 36}, {5, 7}, {1, 3}},
        {{209, 5}, {0, 0}, {0, 27}, {0, 0}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}},
        {{2, 5}, {4, 5}, {0, 121}, {0, 0}, {0, 0}, {0, 3}, {2, 4}, {1, 4}, {2, 2}, {0, 1}},
        {{175, 5}, {0, 1}, {0, 48}, {0, 0}, {0, 0}, {0, 2}, {0, 1}, {0, 2}, {0, 1}, {0, 0}},
        {{83, 5}, {2, 3}, {0, 102}, {0, 0}, {0, 0}, {1, 3}, {0, 2}, {0, 1}, {0, 0}, {0, 0}},
        {{233, 6}, {0, 0}, {0, 8}, {0, 0}, {0, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 1}, {0, 0}},
        {{34, 16}, {112, 21}, {1, 28}, {0, 0}, {0, 0}, {6, 8}, {1, 7}, {0, 3}, {2, 5}, {0, 2}},
        {{159, 35}, {2, 2}, {0, 25}, {0, 0}, {0, 0}, {3, 6}, {0, 5}, {0, 1}, {4, 4}, {0, 1}},
        {{75, 39}, {5, 7}, {2, 48}, {0, 0}, {0, 0}, {3, 11}, {2, 16}, {1, 4}, {7, 10}, {0, 2}},
        {{212, 21}, {0, 1}, {0, 9}, {0, 0}, {0, 0}, {1, 2}, {0, 2}, {0, 0}, {2, 2}, {0, 0}},
        {{4, 2}, {0, 0}, {0, 172}, {0, 0}, {0, 0}, {0, 1}, {0, 2}, {0, 0}, {2, 0}, {0, 0}},
        {{187, 22}, {1, 1}, {0, 17}, {0, 0}, {0, 0}, {3, 6}, {0, 4}, {0, 1}, {4, 4}, {0, 1}},
        {{133, 6}, {1, 2}, {1, 70}, {0, 0}, {0, 0}, {0, 2}, {0, 4}, {0, 3}, {1, 1}, {0, 0}},
        {{251, 1}, {0, 0}, {0, 2}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
    },
    {
        {{2, 3}, {2, 3}, {0, 2}, {0, 2}, {0, 0}, {11, 4}, {1, 4}, {0, 2}, {3, 2}, {0, 4}},
        {{49, 46}, {3, 4}, {7, 31}, {42, 41}, {0, 0}, {2, 6}, {1, 7}, {1, 4}, {2, 4}, {0, 1}},
        {{26, 25}, {1, 1}, {2, 10}, {67, 39}, {0, 0}, {1, 1}, {0, 14}, {0, 2}, {31, 26}, {1, 6}},
        {{103, 46}, {1, 2}, {2, 10}, {33, 42}, {0, 0}, {1, 4}, {0, 3}, {0, 1}, {1, 3}, {0, 0}},
        {{14, 31}, {9, 13}, {14, 54}, {22, 29}, {0, 0}, {2, 6}, {4, 18}, {6, 13}, {1, 5}, {0, 1}},
        {{85, 39}, {0, 0}, {1, 9}, {69, 40}, {0, 0}, {0, 1}, {0, 3}, {0, 1}, {2, 3}, {0, 0}},
        {{31, 28}, {0, 0}, {3, 14}, {130, 34}, {0, 0}, {0, 1}, {0, 3}, {0, 1}, {3, 3}, {0, 1}},
        {{171, 25}, {0, 0}, {1, 5}, {25, 21}, {0, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0}},
        {{17, 21}, {68, 29}, {6, 15}, {13, 22}, {0, 0}, {6, 12}, {3, 14}, {4, 10}, {1, 7}, {0, 3}},
        {{51, 39}, {0, 1}, {2, 12}, {91, 44}, {0, 0}, {0, 2}, {0, 3}, {0, 1}, {2, 3}, {0, 1}},
        {{81, 25}, {0, 0}, {2, 9}, {106, 26}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {0, 0}},
        {{140, 37}, {0, 1}, {1, 8}, {24, 33}, {0, 0}, {1, 2}, {0, 2}, {0, 1}, {1, 2}, {0, 0}},
        {{14, 23}, {1, 3}, {11, 53}, {90, 31}, {0, 0}, {0, 3}, {1, 5}, {2, 6}, {1, 2}, {0, 0}},
        {{123, 29}, {0, 0}, {1, 7}, {57, 30}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0}},
        {{13, 14}, {0, 0}, {4, 20}, {175, 20}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {0, 0}},
        {{202, 23}, {0, 0}, {1, 3}, {2, 9}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}},
    },
};

const unsigned char VP6_BaselineXmittedProbs[10][2][4] = {
    {{42, 2, 7, 42}, {22, 3, 2, 5}},
    {{1, 0, 69, 1}, {1, 44, 6, 1}},
    {{0, 1, 0, 0}, {8, 1, 8, 0}},
    {{0, 2, 1, 0}, {1, 0, 229, 1}},
    {{0, 0, 0, 1}, {0, 0, 1, 0}},
    {{35, 1, 6, 34}, {0, 2, 1, 1}},
    {{1, 0, 122, 1}, {1, 46, 0, 1}},
    {{0, 0, 1, 0}, {64, 0, 64, 64}},
    {{64, 0, 0, 0}, {0, 0, 64, 0}},
    {{64, 64, 64, 0}, {0, 0, 0, 0}},
};

void VP6_BuildModeTree(PB_INSTANCE *pbi) {
    int i, j, k;

    for (i = 0; i < 10; i++) {
        unsigned int Counts[10];
        unsigned int total;

        for (k = 0; k < 3; k++) {
            total = 0;

            for (j = 0; j < 10; j++) {
                if (i == j)
                    Counts[j] = 0;
                else
                    Counts[j] = 100 * pbi->probXmitted[k][0][j];

                total += Counts[j];
            }

            pbi->probModeSame[k][i] =
                255 - 255 * pbi->probXmitted[k][1][i] /
                          (1 + pbi->probXmitted[k][1][i] + pbi->probXmitted[k][0][i]);

            pbi->probMode[k][i][0] =
                1 + 255 * (Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV] +
                           Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]) /
                        (1 + total);
            pbi->probMode[k][i][1] =
                1 + 255 * (Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV]) /
                        (1 + Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV] +
                         Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]);
            pbi->probMode[k][i][2] =
                1 + 255 * (Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV]) /
                        (1 + Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV] +
                         Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV] +
                         Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
            pbi->probMode[k][i][3] =
                1 + 255 * Counts[CODE_INTER_NO_MV] /
                        (1 + Counts[CODE_INTER_NO_MV] + Counts[CODE_INTER_PLUS_MV]);
            pbi->probMode[k][i][4] =
                1 + 255 * Counts[CODE_INTER_NEAREST_MV] /
                        (1 + Counts[CODE_INTER_NEAREST_MV] + Counts[CODE_INTER_NEAR_MV]);
            pbi->probMode[k][i][5] =
                1 + 255 * Counts[CODE_INTRA] /
                        (1 + Counts[CODE_INTRA] + Counts[CODE_INTER_FOURMV]);
            pbi->probMode[k][i][6] =
                1 + 255 * (Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV]) /
                        (1 + Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV] +
                         Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
            pbi->probMode[k][i][7] =
                1 + 255 * Counts[CODE_USING_GOLDEN] /
                        (1 + Counts[CODE_USING_GOLDEN] + Counts[CODE_GOLDEN_MV]);
            pbi->probMode[k][i][8] =
                1 + 255 * Counts[CODE_GOLD_NEAREST_MV] /
                        (1 + Counts[CODE_GOLD_NEAREST_MV] + Counts[CODE_GOLD_NEAR_MV]);
        }
    }
}

int VP6_decodeModeDiff(PB_INSTANCE *pbi) {
    int sign;

    if (!VP6_DecodeBool(&pbi->br, 205)) {
        return 0;
    }

    {
        sign = -2 * VP6_DecodeBool128(&pbi->br) + 1;

        if (!VP6_DecodeBool(&pbi->br, 171)) {
            return sign << (3 - VP6_DecodeBool(&pbi->br, 83));
        }

        if (!VP6_DecodeBool(&pbi->br, 199)) {
            if (VP6_DecodeBool(&pbi->br, 140)) {
                return sign * 12;
            }
            if (VP6_DecodeBool(&pbi->br, 125)) {
                return sign * 16;
            }
            if (VP6_DecodeBool(&pbi->br, 104)) {
                return sign * 20;
            }
            return sign * 24;
        }

        {
            int diff = VP6_bitread(&pbi->br, 7);
            return sign * diff * 4;
        }
    }
}

void VP6_DecodeModeProbs(PB_INSTANCE *pbi) {
    int i;
    int j;

    for (j = 0; j < 3; j++) {
        if (VP6_DecodeBool(&pbi->br, 174)) {
            int whichVector = VP6_bitread(&pbi->br, 4);

            for (i = 0; i < 10; i++) {
                pbi->probXmitted[j][1][i] = VP6_ModeVq[j][whichVector][i][0];
                pbi->probXmitted[j][0][i] = VP6_ModeVq[j][whichVector][i][1];
            }
        }

        if (VP6_DecodeBool(&pbi->br, 254)) {
            for (i = 0; i < 10; i++) {
                int diff;

                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][1][i];

                pbi->probXmitted[j][1][i] =
                    (diff < 0 ? 0 : (diff > 255 ? 255 : diff));

                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][0][i];

                pbi->probXmitted[j][0][i] =
                    (diff < 0 ? 0 : (diff > 255 ? 255 : diff));
            }
        }
    }

    VP6_BuildModeTree(pbi);
}
