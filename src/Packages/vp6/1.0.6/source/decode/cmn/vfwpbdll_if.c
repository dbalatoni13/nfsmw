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

typedef enum {
    PBC_SET_POSTPROC = 0,
    PBC_SET_CPUFREE = 1,
    PBC_MAX_PARAM = 2,
    PBC_SET_TESTMODE = 3,
    PBC_SET_PBSTRUCT = 4,
    PBC_SET_BLACKCLAMP = 5,
    PBC_SET_WHITECLAMP = 6,
    PBC_SET_REFERENCEFRAME = 7,
    PBC_SET_DEINTERLACEMODE = 8,
    PBC_SET_ADDNOISE = 9
} VP6_PB_COMMAND;

extern UINT32 CPUFrequency;

extern PB_INSTANCE *VP6_CreatePBInstance(void);
extern void VP6_DeletePBInstance(PB_INSTANCE **pbi);
extern QUANTIZER_TYPE *VP6_CreateQuantizer(void);
extern void VP6_DeleteQuantizer(QUANTIZER_TYPE **quantizer);
extern struct POSTPROC_INSTANCE *CreatePostProcInstance(CONFIG_TYPE *cfg);
extern void DeletePostProcInstance(struct POSTPROC_INSTANCE **ppi);
extern void SetAddNoiseMode(struct POSTPROC_INSTANCE *ppi, int AddNoiseMode);
extern void CopyFrame(struct POSTPROC_INSTANCE *ppi, UINT8 *dest, UINT8 *src);
extern int VP6_InitFrameDetails(PB_INSTANCE *pbi);
extern void VP6_InitialiseConfiguration(PB_INSTANCE *pbi);
extern void VP6_DeleteFragmentInfo(PB_INSTANCE *pbi);
extern void VP6_DeleteFrameInfo(PB_INSTANCE *pbi);


typedef struct {
    int YWidth;
    int YHeight;
    int YStride;
    int UVWidth;
    int UVHeight;
    int UVStride;
    char *YBuffer;
    char *UBuffer;
    char *VBuffer;
    char *YBufferStart;
} YUV_BUFFER_CONFIG;

extern void *duck_malloc(unsigned long size, int type);
extern int AllocatePostProcBuffers(struct POSTPROC_INSTANCE *ppi);
extern void PostProcess(struct POSTPROC_INSTANCE *ppi, int Vp3VersionNo, int FrameType,
                        int PostProcessingLevel, int FrameQIndex, UINT8 *LastFrameRecon,
                        UINT8 *PostProcessBuffer, UINT8 *FragInfo, UINT32 FragInfoElementSize,
                        UINT32 FragInfoCodedMask);
extern void (*ClampLevels)(struct POSTPROC_INSTANCE *, int, int, UINT8 *, UINT8 *);
extern void ScaleOrCenter(struct POSTPROC_INSTANCE *ppi, UINT8 *src, YUV_BUFFER_CONFIG *dst);
extern void InitHeaderBuffer(FRAME_HEADER *Header, UINT8 *Buffer);
extern int VP6_LoadFrame(PB_INSTANCE *pbi);
extern void VP6_StartDecode(BR_TYPE *br, UINT8 *buffer);
extern void VP6_DecodeFrameMbs(PB_INSTANCE *pbi);
extern void UpdateUMVBorder(struct POSTPROC_INSTANCE *ppi, UINT8 *buffer);
extern struct __sFILE *fopen(const char *, const char *);
extern int fprintf(struct __sFILE *, const char *, ...);
extern int fclose(struct __sFILE *);
struct __sFILE;

int CurrentFrame = 0;

int VP6_StartDecoder(PB_INSTANCE **pbi, UINT32 ImageWidth, UINT32 ImageHeight) {
    *pbi = VP6_CreatePBInstance();

    (*pbi)->ScaleWidth = ImageWidth;
    (*pbi)->ScaleHeight = ImageHeight;
    (*pbi)->OutputWidth = ImageWidth;
    (*pbi)->OutputHeight = ImageHeight;
    (*pbi)->Configuration.VideoFrameWidth = ImageWidth;
    (*pbi)->Configuration.VideoFrameHeight = ImageHeight;

    (*pbi)->postproc = CreatePostProcInstance(&(*pbi)->Configuration);
    (*pbi)->quantizer = VP6_CreateQuantizer();
    (*pbi)->ProcessorFrequency = CPUFrequency;

    if (!VP6_InitFrameDetails(*pbi)) {
        VP6_DeletePBInstance(pbi);
        return 0;
    }

    (*pbi)->quantizer->LastQuantizerValue = 0;
    VP6_InitialiseConfiguration(*pbi);
    return 1;
}

void VP6_SetPbParam(PB_INSTANCE *pbi, VP6_PB_COMMAND Command, UINT32 Parameter) {
    switch (Command) {
        case PBC_SET_CPUFREE:
            pbi->CPUFree = Parameter;
            pbi->PostProcessingLevel = 0;
            break;

        case PBC_SET_ADDNOISE:
            pbi->AddNoiseMode = Parameter;
            SetAddNoiseMode(pbi->postproc, Parameter);
            break;

        case PBC_SET_REFERENCEFRAME:
            CopyFrame(pbi->postproc, (UINT8 *)Parameter, pbi->LastFrameRecon);
            CopyFrame(pbi->postproc, (UINT8 *)Parameter, pbi->GoldenFrame);
            break;

        case PBC_SET_POSTPROC:
            if (Parameter == 9) {
                VP6_SetPbParam(pbi, PBC_SET_CPUFREE, 70);
            } else {
                pbi->PostProcessingLevel = Parameter;
                pbi->CPUFree = 0;
            }
            break;

        case PBC_SET_DEINTERLACEMODE:
            pbi->DeInterlaceMode = Parameter;
            break;

        case PBC_SET_BLACKCLAMP:
            pbi->BlackClamp = Parameter;
            break;

        case PBC_SET_WHITECLAMP:
            pbi->WhiteClamp = Parameter;
            break;

        default:
            break;
    }
}

void VP6_GetYUVConfig(PB_INSTANCE *pbi, YUV_BUFFER_CONFIG *YuvConfig) {
    if (pbi->PostProcessingLevel ||
        (pbi->Configuration.Interlaced && pbi->DeInterlaceMode)) {
        if (pbi->PostProcessBuffer == 0) {
            UINT32 size = pbi->ReconYPlaneSize + pbi->ReconUVPlaneSize * 2 + 32;

            pbi->PostProcessBufferAlloc = duck_malloc(pbi->Configuration.YStride + size, 0);

            pbi->PostProcessBuffer = (UINT8 *)(((UINT32)pbi->PostProcessBufferAlloc + 31) & ~31);
            AllocatePostProcBuffers(pbi->postproc);
        }

        PostProcess(pbi->postproc, pbi->Vp3VersionNo, pbi->FrameType, pbi->PostProcessingLevel,
                    pbi->AvgFrameQIndex, pbi->LastFrameRecon, pbi->PostProcessBuffer,
                    (UINT8 *)pbi->FragInfo, 4, 1);

        if (pbi->BlackClamp) {
            ClampLevels(pbi->postproc, pbi->BlackClamp, pbi->WhiteClamp, pbi->PostProcessBuffer,
                        pbi->PostProcessBuffer);
        }
    }

    if (pbi->Configuration.VideoFrameWidth < pbi->OutputWidth ||
        pbi->Configuration.VideoFrameHeight < pbi->OutputHeight) {
        YuvConfig->YWidth = pbi->OutputWidth + 32;
        YuvConfig->YHeight = pbi->OutputHeight + 32;
        YuvConfig->YStride = YuvConfig->YWidth;

        YuvConfig->UVWidth = YuvConfig->YWidth / 2;
        YuvConfig->UVHeight = YuvConfig->YHeight / 2;
        YuvConfig->UVStride = YuvConfig->UVWidth;

        YuvConfig->YBuffer = (char *)pbi->ScaleBuffer;
        YuvConfig->UBuffer = (char *)pbi->ScaleBuffer + YuvConfig->YWidth * YuvConfig->YHeight;
        YuvConfig->VBuffer = (char *)pbi->ScaleBuffer + YuvConfig->YWidth * YuvConfig->YHeight +
                             YuvConfig->UVWidth * YuvConfig->UVHeight;

        YuvConfig->YBufferStart = (char *)pbi->ScaleBuffer;

        if (pbi->PostProcessingLevel) {
            ScaleOrCenter(pbi->postproc, pbi->PostProcessBuffer, YuvConfig);
        } else {
            ScaleOrCenter(pbi->postproc, pbi->LastFrameRecon, YuvConfig);
        }

        YuvConfig->YBuffer += ((YuvConfig->YHeight - pbi->OutputHeight) / 2) * YuvConfig->YStride +
                              (YuvConfig->YWidth - pbi->OutputWidth) / 2;

        YuvConfig->YWidth = pbi->OutputWidth;
        YuvConfig->YHeight = pbi->OutputHeight;

        YuvConfig->UBuffer += ((YuvConfig->UVHeight - pbi->OutputHeight / 2) / 2) *
                                  YuvConfig->UVStride +
                              (YuvConfig->UVWidth - pbi->OutputWidth / 2) / 2;

        YuvConfig->VBuffer += ((YuvConfig->UVHeight - pbi->OutputHeight / 2) / 2) *
                                  YuvConfig->UVStride +
                              (YuvConfig->UVWidth - pbi->OutputWidth / 2) / 2;

        YuvConfig->UVWidth = pbi->OutputWidth / 2;
        YuvConfig->UVHeight = pbi->OutputHeight / 2;
    } else {
        YuvConfig->YWidth = pbi->Configuration.VideoFrameWidth;
        YuvConfig->YHeight = pbi->Configuration.VideoFrameHeight;
        YuvConfig->YStride = pbi->Configuration.YStride;

        YuvConfig->UVWidth = pbi->Configuration.VideoFrameWidth / 2;
        YuvConfig->UVHeight = pbi->Configuration.VideoFrameHeight / 2;
        YuvConfig->UVStride = pbi->Configuration.UVStride;

        if (pbi->PostProcessingLevel ||
            (pbi->Configuration.Interlaced && pbi->DeInterlaceMode)) {
            YuvConfig->YBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconYDataOffset + pbi->Configuration.YStride * 48 + 48);
            YuvConfig->UBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconUDataOffset + pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->VBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconVDataOffset + pbi->Configuration.UVStride * 24 + 24);

            YuvConfig->YBufferStart = (char *)pbi->PostProcessBuffer + pbi->ReconYDataOffset;
        } else {
            YuvConfig->YBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconYDataOffset + pbi->Configuration.YStride * 48 + 48);
            YuvConfig->UBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconUDataOffset + pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->VBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconVDataOffset + pbi->Configuration.UVStride * 24 + 24);

            YuvConfig->YBufferStart = (char *)pbi->LastFrameRecon + pbi->ReconYDataOffset;
        }
    }
}

static int VP6_DecodeFrameToYUV_internal(PB_INSTANCE *pbi, char *VideoBufferPtr, UINT32 ByteCount,
                                         UINT32 ImageWidth, UINT32 ImageHeight) {
    struct __sFILE *f;

    pbi->CurrentFrameSize = ByteCount;

    InitHeaderBuffer(&pbi->Header, (UINT8 *)VideoBufferPtr);

    if (!VP6_LoadFrame(pbi)) {
        return -1;
    }

    if (pbi->MultiStream || !pbi->VpProfile) {
        if (pbi->UseHuffman) {
            pbi->br3.bitsinremainder = 0;
            pbi->br3.remainder = 0;
            pbi->br3.position = (UINT8 *)VideoBufferPtr + pbi->Buff2Offset;
        } else {
            VP6_StartDecode(&pbi->br2, (UINT8 *)VideoBufferPtr + pbi->Buff2Offset);
        }
    }

    VP6_DecodeFrameMbs(pbi);

    {
        UINT8 *tmp = pbi->LastFrameRecon;

        pbi->LastFrameRecon = pbi->ThisFrameRecon;

        pbi->ThisFrameRecon = pbi->OtherFrameRecon ? pbi->OtherFrameRecon : tmp;
        pbi->OtherFrameRecon = 0;
    }

    UpdateUMVBorder(pbi->postproc, pbi->LastFrameRecon);

    if (pbi->FrameType == 0 || pbi->RefreshGoldenFrame) {
        pbi->OtherFrameRecon = pbi->GoldenFrame;
        pbi->GoldenFrame = pbi->LastFrameRecon;
    }

    if (pbi->FrameType == 0) {
        pbi->AvgFrameQIndex = pbi->quantizer->FrameQIndex;
    } else {
        {
            UINT32 q = pbi->quantizer->FrameQIndex + 2;

            pbi->AvgFrameQIndex = (pbi->AvgFrameQIndex * 3 + q) / 4;
        }
    }

    if (pbi->br.pos > pbi->CurrentFrameSize) {
        f = fopen("badframes.stt", "a");
        fprintf(f, "%8d %8d %8d \n", CurrentFrame, pbi->br.pos, pbi->CurrentFrameSize);
        fclose(f);
    }

    CurrentFrame = CurrentFrame + 1;

    return 0;
}

#define MFGQR(n) ({ UINT32 v_; asm("mfspr %0,GQR" #n : "=b"(v_)); v_; })
#define MTGQR(n, v) asm volatile("mtspr GQR" #n ",%0" : : "b"(v))

static __inline void OSInitFastCast(void) {
    asm volatile("li 3,4
	oris 3,3,4
	mtspr GQR2,3
	"
                 "li 3,5
	oris 3,3,5
	mtspr GQR3,3
	"
                 "li 3,6
	oris 3,3,6
	mtspr GQR4,3
	"
                 "li 3,7
	oris 3,3,7
	mtspr GQR5,3" : : : "r3");
}

int VP6_DecodeFrameToYUV(PB_INSTANCE *pbi, char *VideoBufferPtr, UINT32 ByteCount,
                         UINT32 ImageWidth, UINT32 ImageHeight) {
    int result;
    UINT32 _gqrStates[5];

    _gqrStates[0] = MFGQR(2);
    _gqrStates[1] = MFGQR(3);
    _gqrStates[2] = MFGQR(4);
    _gqrStates[3] = MFGQR(5);
    _gqrStates[4] = MFGQR(6);

    OSInitFastCast();
    MTGQR(6, 0x39053905);

    result = VP6_DecodeFrameToYUV_internal(pbi, VideoBufferPtr, ByteCount, ImageWidth, ImageHeight);

    MTGQR(6, _gqrStates[4]);
    MTGQR(5, _gqrStates[3]);
    MTGQR(4, _gqrStates[2]);
    MTGQR(3, _gqrStates[1]);
    MTGQR(2, _gqrStates[0]);

    return result;
}

int VP6_StopDecoder(PB_INSTANCE **pbi) {
    if (*pbi) {
        VP6_DeleteQuantizer(&(*pbi)->quantizer);
        DeletePostProcInstance(&(*pbi)->postproc);
        VP6_DeleteFragmentInfo(*pbi);
        VP6_DeleteFrameInfo(*pbi);
        VP6_DeletePBInstance(pbi);
    }

    return 1;
}
