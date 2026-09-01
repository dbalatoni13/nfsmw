#ifndef VP6_PBDLL_H
#define VP6_PBDLL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

typedef struct {
    short x;
    short y;
} MOTION_VECTOR;

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

typedef enum {
    TOP_LEFT_Y_BLOCK = 0,
    TOP_RIGHT_Y_BLOCK = 1,
    BOTTOM_LEFT_Y_BLOCK = 2,
    BOTTOM_RIGHT_Y_BLOCK = 3,
    U_BLOCK = 4,
    V_BLOCK = 5
} BLOCK_POSITION;

struct POSTPROC_INSTANCE;

struct _BITREADER {
    int bitsinremainder;
    unsigned int remainder;
    const unsigned char *position;
};

struct PB_INSTANCE {
    // Members
    // total size: 0xCC
    struct {
        // Members
        short (* CoeffsAlloc)[64]; // offset 0x0, size 0x4
        short (* Coeffs)[64]; // offset 0x4, size 0x4
        CODING_MODE Mode; // offset 0x8, size 0x4
        CODING_MODE BlockMode[6]; // offset 0xC, size 0x18
        MOTION_VECTOR Mv[6]; // offset 0x24, size 0x18
        MOTION_VECTOR NearestInterMVect; // offset 0x3C, size 0x4
        MOTION_VECTOR NearInterMVect; // offset 0x40, size 0x4
        int NearestMvIndex; // offset 0x44, size 0x4
        MOTION_VECTOR NearestGoldMVect; // offset 0x48, size 0x4
        MOTION_VECTOR NearGoldMVect; // offset 0x4C, size 0x4
        int NearestGMvIndex; // offset 0x50, size 0x4
        unsigned int MBrow; // offset 0x54, size 0x4
        unsigned int MBcol; // offset 0x58, size 0x4
        int bp; // offset 0x5C, size 0x4
        unsigned int Source; // offset 0x60, size 0x4
        unsigned int SourceY; // offset 0x64, size 0x4
        unsigned int SourceX; // offset 0x68, size 0x4
        int CurrentSourceStride; // offset 0x6C, size 0x4
        unsigned int Recon; // offset 0x70, size 0x4
        int CurrentReconStride; // offset 0x74, size 0x4
        unsigned int Plane; // offset 0x78, size 0x4
        int MvShift; // offset 0x7C, size 0x4
        int MvModMask; // offset 0x80, size 0x4
        int FrameSourceStride; // offset 0x84, size 0x4
        int FrameReconStride; // offset 0x88, size 0x4
        unsigned int SourcePtr[6]; // offset 0x8C, size 0x18
        unsigned int ReconPtr[6]; // offset 0xA4, size 0x18
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * Above; // offset 0xBC, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * Left; // offset 0xC0, size 0x4
        short * LastDc; // offset 0xC4, size 0x4
        int Interlaced; // offset 0xC8, size 0x4
    } mbi; // offset 0x0, size 0xCC
    // total size: 0x70
    struct {
        // Members
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } LeftY[2]; // offset 0x0, size 0x20
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } LeftU; // offset 0x20, size 0x10
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } LeftV; // offset 0x30, size 0x10
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveY; // offset 0x40, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveU; // offset 0x44, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveV; // offset 0x48, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveYAlloc; // offset 0x4C, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveUAlloc; // offset 0x50, size 0x4
        // total size: 0x10
        struct {
            // Members
            unsigned char Token; // offset 0x0, size 0x1
            int Mode; // offset 0x4, size 0x4
            unsigned short Frame; // offset 0x8, size 0x2
            short Dc; // offset 0xA, size 0x2
            unsigned char unused[3]; // offset 0xC, size 0x3
        } * AboveVAlloc; // offset 0x54, size 0x4
        short LastDcY[4]; // offset 0x58, size 0x8
        short LastDcU[4]; // offset 0x60, size 0x8
        short LastDcV[4]; // offset 0x68, size 0x8
    } fc; // offset 0xCC, size 0x70
    // total size: 0x990
    struct {
        // Members
        unsigned int FrameQIndex; // offset 0x0, size 0x4
        unsigned int ThisFrameQuantizerValue; // offset 0x4, size 0x4
        short round[8]; // offset 0x8, size 0x10
        short mult[8]; // offset 0x18, size 0x10
        short zbin[8]; // offset 0x28, size 0x10
        unsigned int LastQuantizerValue; // offset 0x38, size 0x4
        unsigned int QThreshTable[64]; // offset 0x3C, size 0x100
        unsigned int * transIndex; // offset 0x13C, size 0x4
        unsigned char quant_index[64]; // offset 0x140, size 0x40
        short * dequant_coeffs[2]; // offset 0x180, size 0x8
        short * dequant_coeffsAlloc[2]; // offset 0x188, size 0x8
        int QuantCoeffs[64][2]; // offset 0x190, size 0x200
        int QuantRound[64][2]; // offset 0x390, size 0x200
        int ZeroBinSize[64][2]; // offset 0x590, size 0x200
        int ZlrZbinCorrections[64][2]; // offset 0x790, size 0x200
    } * quantizer; // offset 0x13C, size 0x4
    int CodedBlockIndex; // offset 0x140, size 0x4
    unsigned char * DataOutputInPtr; // offset 0x144, size 0x4
    // total size: 0x4
    struct {
        // Members
        unsigned int FragCodingMode : 4; // offset 0x0, size 0x4
        int MVectorX : 8; // offset 0x0, size 0x4
        int MVectorY : 8; // offset 0x0, size 0x4
    } * FragInfo; // offset 0x148, size 0x4
    // total size: 0x4
    struct {
        // Members
        unsigned int FragCodingMode : 4; // offset 0x0, size 0x4
        int MVectorX : 8; // offset 0x0, size 0x4
        int MVectorY : 8; // offset 0x0, size 0x4
    } * FragInfoAlloc; // offset 0x14C, size 0x4
    // total size: 0x20
    struct {
        // Members
        unsigned int lowvalue; // offset 0x0, size 0x4
        unsigned int range; // offset 0x4, size 0x4
        unsigned int value; // offset 0x8, size 0x4
        int count; // offset 0xC, size 0x4
        unsigned int pos; // offset 0x10, size 0x4
        unsigned char * buffer; // offset 0x14, size 0x4
        unsigned int MeasureCost; // offset 0x18, size 0x4
        unsigned int BitCounter; // offset 0x1C, size 0x4
    } br; // offset 0x150, size 0x20
    // total size: 0x20
    struct {
        // Members
        unsigned int lowvalue; // offset 0x0, size 0x4
        unsigned int range; // offset 0x4, size 0x4
        unsigned int value; // offset 0x8, size 0x4
        int count; // offset 0xC, size 0x4
        unsigned int pos; // offset 0x10, size 0x4
        unsigned char * buffer; // offset 0x14, size 0x4
        unsigned int MeasureCost; // offset 0x18, size 0x4
        unsigned int BitCounter; // offset 0x1C, size 0x4
    } br2; // offset 0x170, size 0x20
    struct _BITREADER br3; // offset 0x190, size 0xC
    unsigned char Vp3VersionNo; // offset 0x19C, size 0x1
    unsigned char VpProfile; // offset 0x19D, size 0x1
    unsigned int PostProcessingLevel; // offset 0x1A0, size 0x4
    unsigned int ProcessorFrequency; // offset 0x1A4, size 0x4
    unsigned int CPUFree; // offset 0x1A8, size 0x4
    unsigned char FrameType; // offset 0x1AC, size 0x1
    struct CONFIG_TYPE Configuration; // offset 0x1B0, size 0x38
    unsigned int CurrentFrameSize; // offset 0x1E8, size 0x4
    unsigned int YPlaneSize; // offset 0x1EC, size 0x4
    unsigned int UVPlaneSize; // offset 0x1F0, size 0x4
    unsigned int VFragments; // offset 0x1F4, size 0x4
    unsigned int HFragments; // offset 0x1F8, size 0x4
    unsigned int UnitFragments; // offset 0x1FC, size 0x4
    unsigned int YPlaneFragments; // offset 0x200, size 0x4
    unsigned int UVPlaneFragments; // offset 0x204, size 0x4
    unsigned int ReconYPlaneSize; // offset 0x208, size 0x4
    unsigned int ReconUVPlaneSize; // offset 0x20C, size 0x4
    unsigned int YDataOffset; // offset 0x210, size 0x4
    unsigned int UDataOffset; // offset 0x214, size 0x4
    unsigned int VDataOffset; // offset 0x218, size 0x4
    unsigned int ReconYDataOffset; // offset 0x21C, size 0x4
    unsigned int ReconUDataOffset; // offset 0x220, size 0x4
    unsigned int ReconVDataOffset; // offset 0x224, size 0x4
    unsigned int MacroBlocks; // offset 0x228, size 0x4
    unsigned int MBRows; // offset 0x22C, size 0x4
    unsigned int MBCols; // offset 0x230, size 0x4
    unsigned int ScaleWidth; // offset 0x234, size 0x4
    unsigned int ScaleHeight; // offset 0x238, size 0x4
    unsigned int OutputWidth; // offset 0x23C, size 0x4
    unsigned int OutputHeight; // offset 0x240, size 0x4
    unsigned char * ThisFrameRecon; // offset 0x244, size 0x4
    unsigned char * ThisFrameReconAlloc; // offset 0x248, size 0x4
    unsigned char * GoldenFrame; // offset 0x24C, size 0x4
    unsigned char * GoldenFrameAlloc; // offset 0x250, size 0x4
    unsigned char * LastFrameRecon; // offset 0x254, size 0x4
    unsigned char * LastFrameReconAlloc; // offset 0x258, size 0x4
    unsigned char * PostProcessBuffer; // offset 0x25C, size 0x4
    unsigned char * PostProcessBufferAlloc; // offset 0x260, size 0x4
    unsigned char * ScaleBuffer; // offset 0x264, size 0x4
    unsigned char * ScaleBufferAlloc; // offset 0x268, size 0x4
    unsigned char * OtherFrameRecon; // offset 0x26C, size 0x4
    short * quantized_list; // offset 0x270, size 0x4
    short * ReconDataBuffer; // offset 0x274, size 0x4
    short * ReconDataBufferAlloc; // offset 0x278, size 0x4
    unsigned char FragCoefEOB; // offset 0x27C, size 0x1
    short * TmpReconBuffer; // offset 0x280, size 0x4
    short * TmpReconBufferAlloc; // offset 0x284, size 0x4
    short * TmpDataBuffer; // offset 0x288, size 0x4
    short * TmpDataBufferAlloc; // offset 0x28C, size 0x4
    unsigned char * LoopFilteredBlockAlloc; // offset 0x290, size 0x4
    unsigned char * LoopFilteredBlock; // offset 0x294, size 0x4
    void (* * idct)(short *, short *, short *); // offset 0x298, size 0x4
    struct POSTPROC_INSTANCE * postproc; // offset 0x29C, size 0x4
    unsigned char LastToken[256]; // offset 0x2A0, size 0x100
    int LastMode; // offset 0x3A0, size 0x4
    unsigned char DcProbs[22]; // offset 0x3A4, size 0x16
    unsigned char AcProbs[396]; // offset 0x3BA, size 0x18C
    unsigned char DcNodeContexts[5][3][2]; // offset 0x546, size 0x1E
    unsigned char ZeroRunProbs[2][14]; // offset 0x564, size 0x1C
    unsigned char MergedScanOrder[64]; // offset 0x580, size 0x40
    unsigned char ModifiedScanOrder[64]; // offset 0x5C0, size 0x40
    unsigned char EobOffsetTable[64]; // offset 0x600, size 0x40
    unsigned char ScanBands[64]; // offset 0x640, size 0x40
    unsigned char MBModeProb[11]; // offset 0x680, size 0xB
    unsigned char BModeProb[11]; // offset 0x68B, size 0xB
    unsigned char PredictionFilterMode; // offset 0x696, size 0x1
    unsigned char PredictionFilterMvSizeThresh; // offset 0x697, size 0x1
    unsigned int PredictionFilterVarThresh; // offset 0x698, size 0x4
    int RefreshGoldenFrame; // offset 0x69C, size 0x4
    unsigned char Inter00Prob; // offset 0x6A0, size 0x1
    unsigned int AvgFrameQIndex; // offset 0x6A4, size 0x4
    int testMode; // offset 0x6A8, size 0x4
    unsigned int mvNearOffset[16]; // offset 0x6AC, size 0x40
    int probInterlaced; // offset 0x6EC, size 0x4
    char * MBInterlaced; // offset 0x6F0, size 0x4
    char * predictionMode; // offset 0x6F4, size 0x4
    MOTION_VECTOR * MBMotionVector; // offset 0x6F8, size 0x4
    char * MBInterlacedAlloc; // offset 0x6FC, size 0x4
    char * predictionModeAlloc; // offset 0x700, size 0x4
    MOTION_VECTOR * MBMotionVectorAlloc; // offset 0x704, size 0x4
    unsigned char MvSignProbs[2]; // offset 0x708, size 0x2
    unsigned char IsMvShortProb[2]; // offset 0x70A, size 0x2
    unsigned char MvShortProbs[2][7]; // offset 0x70C, size 0xE
    unsigned char MvQPelProbs[2]; // offset 0x71A, size 0x2
    unsigned char MvHalfPixelProbs[2]; // offset 0x71C, size 0x2
    unsigned char MvLowBitProbs[2]; // offset 0x71E, size 0x2
    unsigned char MvSizeProbs[2][8]; // offset 0x720, size 0x10
    unsigned char probXmitted[10][2][4]; // offset 0x730, size 0x50
    unsigned char probModeSame[4][10]; // offset 0x780, size 0x28
    unsigned char probMode[4][10][9]; // offset 0x7A8, size 0x168
    unsigned int maxTimePerFrame; // offset 0x910, size 0x4
    unsigned int thisDecodeTime; // offset 0x914, size 0x4
    unsigned int avgDecodeTime; // offset 0x918, size 0x4
    unsigned int avgPPTime[10]; // offset 0x91C, size 0x28
    unsigned int avgBlitTime; // offset 0x944, size 0x4
    int MultiStream; // offset 0x948, size 0x4
    unsigned int DcHuffCode[12][2]; // offset 0x94C, size 0x60
    unsigned char DcHuffLength[12][2]; // offset 0x9AC, size 0x18
    unsigned int DcHuffProbs[12][2]; // offset 0x9C4, size 0x60
    struct _huffnode DcHuffTree[12][2]; // offset 0xA24, size 0x120
    unsigned int AcHuffCode[12][6][2][3]; // offset 0xB44, size 0x6C0
    unsigned char AcHuffLength[12][6][2][3]; // offset 0x1204, size 0x1B0
    unsigned int AcHuffProbs[12][6][2][3]; // offset 0x13B4, size 0x6C0
    struct _huffnode AcHuffTree[12][6][2][3]; // offset 0x1A74, size 0x1440
    unsigned int ZeroHuffCode[14][2]; // offset 0x2EB4, size 0x70
    unsigned char ZeroHuffLength[14][2]; // offset 0x2F24, size 0x1C
    unsigned int ZeroHuffProbs[14][2]; // offset 0x2F40, size 0x70
    struct _huffnode ZeroHuffTree[14][2]; // offset 0x2FB0, size 0x150
    unsigned short DcHuffLUT[64][2]; // offset 0x3100, size 0x100
    unsigned short AcHuffLUT[64][6][2][3]; // offset 0x3200, size 0x1200
    unsigned short ZeroHuffLUT[64][2]; // offset 0x4400, size 0x100
    struct RAW_BUFFER HuffBuffer; // offset 0x4500, size 0x10
    struct FRAME_HEADER Header; // offset 0x4510, size 0x10
    unsigned int Buff2Offset; // offset 0x4520, size 0x4
    int UseHuffman; // offset 0x4524, size 0x4
    int CurrentDcRunLen[2]; // offset 0x4528, size 0x8
    int CurrentAc1RunLen[2]; // offset 0x4530, size 0x8
    unsigned char UseLoopFilter; // offset 0x4538, size 0x1
    unsigned int DrCutOff; // offset 0x453C, size 0x4
    unsigned int DrThresh[256]; // offset 0x4540, size 0x400
    unsigned int BlackClamp; // offset 0x4940, size 0x4
    unsigned int WhiteClamp; // offset 0x4944, size 0x4
    unsigned int DeInterlaceMode; // offset 0x4948, size 0x4
    unsigned int AddNoiseMode; // offset 0x494C, size 0x4
    char * ScratchpadStart; // offset 0x4950, size 0x4
    char * ScratchpadEnd; // offset 0x4954, size 0x4
};



#endif
