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

extern UINT32 VP6_DecodeBool128(BR_TYPE *br);
extern UINT32 VP6_DecodeBool(BR_TYPE *br, int probability);
extern void VP6_StartDecode(BR_TYPE *br, UINT8 *source);
extern void VP6_InitQTables(QUANTIZER_TYPE *quantizer, UINT32 Vp3VersionNo);
extern void VP6_UpdateQ(QUANTIZER_TYPE *quantizer, UINT32 Vp3VersionNo);
extern void VP6_InitFrameDetails(PB_INSTANCE *pbi);
extern void SetPPInterlacedMode(struct POSTPROC_INSTANCE *ppi, int Interlaced);
extern void SetDeInterlaceMode(struct POSTPROC_INSTANCE *ppi, int DeInterlaceMode);
extern void *memset(void *, int, unsigned long);

void InitHeaderBuffer(FRAME_HEADER *Header, UINT8 *Buffer) {
    Header->buffer = Buffer;
    Header->value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
    Header->bits_available = 32;
    Header->pos = 4;
}

UINT32 ReadHeaderBits(FRAME_HEADER *Header, UINT32 BitsRequired) {
    UINT32 pos = Header->pos;
    UINT32 available = Header->bits_available;
    UINT32 value = Header->value;
    UINT8 *Buffer = Header->buffer + pos;
    UINT32 RetVal = 0;

    if (available < BitsRequired) {
        RetVal = value >> (32 - available);
        BitsRequired -= available;
        value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
        RetVal <<= BitsRequired;
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32 - BitsRequired);
    Header->value = value << BitsRequired;
    Header->bits_available = available - BitsRequired;
    Header->pos = pos;

    return RetVal;
}

inline UINT32 VP6_bitread(BR_TYPE *br, int bits) {
    UINT32 z = 0;
    int bit;

    for (bit = bits - 1; bit >= 0; bit--) {
        z |= (VP6_DecodeBool128(br) << bit);
    }

    return z;
}

static int LoadFrameHeader(PB_INSTANCE *pbi) {
    UINT8 DctQMask;
    FRAME_HEADER *Header = &pbi->Header;
    int RetVal = 1;

    pbi->FrameType = ReadHeaderBits(Header, 1);
    DctQMask = ReadHeaderBits(Header, 6);
    pbi->MultiStream = (UINT8)ReadHeaderBits(Header, 1);

    if (pbi->FrameType == 0) {
        pbi->Vp3VersionNo = ReadHeaderBits(Header, 5);
        pbi->VpProfile = ReadHeaderBits(Header, 2);

        if (pbi->Vp3VersionNo > 7) {
            return 0;
        }

        VP6_InitQTables(pbi->quantizer, pbi->Vp3VersionNo);

        pbi->Configuration.Interlaced = (UINT8)ReadHeaderBits(Header, 1);

        if (pbi->MultiStream || pbi->VpProfile == 0) {
            VP6_StartDecode(&pbi->br, pbi->Header.buffer + 4);
            pbi->Buff2Offset = ReadHeaderBits(Header, 16);
        } else {
            VP6_StartDecode(&pbi->br, pbi->Header.buffer + 2);
        }

        SetPPInterlacedMode(pbi->postproc, pbi->Configuration.Interlaced);

        if (pbi->Configuration.Interlaced) {
            SetDeInterlaceMode(pbi->postproc, pbi->DeInterlaceMode);
        }

        {
            UINT32 HFragments;
            UINT32 VFragments;
            UINT32 HOldScaled;
            UINT32 VOldScaled;
            UINT32 HNewScaled;
            UINT32 VNewScaled;
            UINT32 OutputHFragments;
            UINT32 OutputVFragments;

            VFragments = (UINT8)VP6_bitread(&pbi->br, 8) * 2;
            HFragments = (UINT8)VP6_bitread(&pbi->br, 8) * 2;
            OutputVFragments = (UINT8)VP6_bitread(&pbi->br, 8) * 2;
            OutputHFragments = (UINT8)VP6_bitread(&pbi->br, 8) * 2;

            if (pbi->Configuration.HRatio == 0) {
                pbi->Configuration.HRatio = 1;
            }

            if (pbi->Configuration.VRatio == 0) {
                pbi->Configuration.VRatio = 1;
            }

            pbi->Configuration.ExpandedFrameWidth = OutputHFragments * 8;
            pbi->Configuration.ExpandedFrameHeight = OutputVFragments * 8;

            HOldScaled =
                pbi->Configuration.HScale * pbi->HFragments * 8 / pbi->Configuration.HRatio;
            VOldScaled =
                pbi->Configuration.VScale * pbi->VFragments * 8 / pbi->Configuration.VRatio;

            if (VFragments >= OutputVFragments) {
                pbi->Configuration.VRatio = pbi->Configuration.VScale = 1;
            } else if (VFragments * 5 >= OutputVFragments * 4) {
                pbi->Configuration.VScale = 5;
                pbi->Configuration.VRatio = 4;
            } else if (VFragments * 5 >= OutputVFragments * 3) {
                pbi->Configuration.VScale = 5;
                pbi->Configuration.VRatio = 3;
            } else {
                pbi->Configuration.VScale = 2;
                pbi->Configuration.VRatio = 1;
            }

            if (HFragments >= OutputHFragments) {
                pbi->Configuration.HRatio = pbi->Configuration.HScale = 1;
            } else if (HFragments * 5 >= OutputHFragments * 4) {
                pbi->Configuration.HScale = 5;
                pbi->Configuration.HRatio = 4;
            } else if (HFragments * 5 >= OutputHFragments * 3) {
                pbi->Configuration.HScale = 5;
                pbi->Configuration.HRatio = 3;
            } else {
                pbi->Configuration.HScale = 2;
                pbi->Configuration.HRatio = 1;
            }

            HNewScaled = pbi->Configuration.HScale * HFragments * 8 / pbi->Configuration.HRatio;
            VNewScaled = pbi->Configuration.VScale * VFragments * 8 / pbi->Configuration.VRatio;

            pbi->ScaleWidth = HNewScaled;
            pbi->ScaleHeight = VNewScaled;

            pbi->Configuration.ScalingMode = VP6_bitread(&pbi->br, 2);

            if (VFragments != pbi->VFragments || HFragments != pbi->HFragments) {
                pbi->Configuration.VideoFrameWidth = HFragments * 8;
                pbi->Configuration.VideoFrameHeight = VFragments * 8;
                VP6_InitFrameDetails(pbi);
            }

            if (pbi->ScaleBuffer && (HOldScaled != HNewScaled || VOldScaled != VNewScaled)) {
                memset(pbi->ScaleBuffer, 0, (pbi->OutputWidth + 32) * (pbi->OutputHeight + 32));
                memset(pbi->ScaleBuffer + (pbi->OutputWidth + 32) * (pbi->OutputHeight + 32), 128,
                       (pbi->OutputWidth + 32) * (pbi->OutputHeight + 32) / 2);
            }
        }

        if (pbi->VpProfile) {
            if (VP6_DecodeBool(&pbi->br, 128)) {
                pbi->PredictionFilterMode = 2;
                pbi->PredictionFilterVarThresh = VP6_bitread(&pbi->br, 5) * 32;
                pbi->PredictionFilterMvSizeThresh = VP6_bitread(&pbi->br, 3);
            } else if (VP6_DecodeBool(&pbi->br, 128)) {
                pbi->PredictionFilterMode = 1;
            } else {
                pbi->PredictionFilterMode = 0;
            }
        }
    } else {
        if (pbi->MultiStream || pbi->VpProfile == 0) {
            VP6_StartDecode(&pbi->br, pbi->Header.buffer + 3);
            pbi->Buff2Offset = ReadHeaderBits(Header, 16);
        } else {
            VP6_StartDecode(&pbi->br, pbi->Header.buffer + 1);
        }

        pbi->RefreshGoldenFrame = VP6_DecodeBool(&pbi->br, 128);

        if (pbi->VpProfile) {
            pbi->UseLoopFilter = VP6_DecodeBool(&pbi->br, 128);

            if (pbi->UseLoopFilter) {
                pbi->UseLoopFilter = (pbi->UseLoopFilter << 1) | VP6_DecodeBool(&pbi->br, 128);
            }
        }
    }

    pbi->UseHuffman = VP6_DecodeBool(&pbi->br, 128);
    pbi->quantizer->FrameQIndex = DctQMask;
    pbi->quantizer->ThisFrameQuantizerValue = pbi->quantizer->QThreshTable[DctQMask];
    VP6_UpdateQ(pbi->quantizer, pbi->Vp3VersionNo);

    return RetVal;
}

int VP6_LoadFrame(PB_INSTANCE *pbi) {
    int ret = 1;

    if (LoadFrameHeader(pbi) == 0) {
        ret = 0;
    }

    return ret;
}
