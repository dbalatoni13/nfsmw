#include "../../../include/vp6_pbdll.h"

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
    struct CONFIG_TYPE Configuration;
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
extern unsigned int *DeblockLimitValuesV2;
extern unsigned int *DeblockLimitValuesV1;
extern unsigned int *SetupDeblockValueArray(POSTPROC_INSTANCE *ppi, int FLimit);
extern void FilterHoriz_Simple(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                               int LineLength, int *BoundingValuePtr);
extern void FilterVert_Simple(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                              int LineLength, int *BoundingValuePtr);
extern void *memcpy(void *dest, const void *source, unsigned int size);
extern int abs(int value);

void FilterHoriz_Simple_C(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                          int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable;
    int UseHighVariance;

    LimitTable = LimitVal_VP31 + 0x100;
    j = 8;
    do {
        UseHighVariance = 0;
        if (abs(PixelPtr[0] - PixelPtr[1]) > 1) {
            UseHighVariance = 1;
        }
        if (UseHighVariance == 0 &&
            abs(PixelPtr[2] - PixelPtr[3]) > 1) {
            UseHighVariance = 1;
        }
        FiltVal = 3 * (PixelPtr[2] - PixelPtr[1]);
        if (UseHighVariance != 0) {
            FiltVal += PixelPtr[3] - PixelPtr[0];
        }
        FiltVal = (FiltVal + 4) >> 3;
        FiltVal = BoundingValuePtr[FiltVal];
        PixelPtr[1] = LimitTable[PixelPtr[1] + FiltVal];
        PixelPtr[2] = LimitTable[PixelPtr[2] - FiltVal];
        if (UseHighVariance == 0) {
            FiltVal >>= 1;
            PixelPtr[0] = LimitTable[PixelPtr[0] + FiltVal];
            PixelPtr[3] = LimitTable[PixelPtr[3] - FiltVal];
        }
        PixelPtr += LineLength;
    } while (--j);
}

void FilterVert_Simple_C(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                         int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable;
    int UseHighVariance;

    LimitTable = LimitVal_VP31 + 0x100;
    j = 8;
    do {
        UseHighVariance = 0;
        if (abs(*(PixelPtr - 2 * LineLength) - *(PixelPtr - LineLength)) > 1) {
            UseHighVariance = 1;
        }
        if (UseHighVariance == 0 &&
            abs(*(PixelPtr + LineLength) - PixelPtr[0]) > 1) {
            UseHighVariance = 1;
        }
        FiltVal = 3 * (PixelPtr[0] - *(PixelPtr - LineLength));
        if (UseHighVariance != 0) {
            FiltVal += *(PixelPtr + LineLength) - *(PixelPtr - 2 * LineLength);
        }
        FiltVal = (FiltVal + 4) >> 3;
        FiltVal = BoundingValuePtr[FiltVal];
        *(PixelPtr - LineLength) = LimitTable[*(PixelPtr - LineLength) + FiltVal];
        PixelPtr[0] = LimitTable[PixelPtr[0] - FiltVal];
        if (UseHighVariance == 0) {
            FiltVal >>= 1;
            *(PixelPtr - 2 * LineLength) =
                LimitTable[*(PixelPtr - 2 * LineLength) + FiltVal];
            *(PixelPtr + LineLength) =
                LimitTable[*(PixelPtr + LineLength) - FiltVal];
        }
        PixelPtr++;
    } while (--j);
}

void SimpleDeblockFrame(POSTPROC_INSTANCE *ppi, unsigned char *SrcBuffer,
                        unsigned char *DestBuffer) {
    int j;
    int m;
    int n;
    int RowStart;
    int NextRow;
    int FLimit;
    int QIndex;
    int *BoundingValuePtr;
    int LineLength;
    int FragsAcross;
    int FragsDown;

    QIndex = ppi->FrameQIndex;
    if (ppi->Vp3VersionNo > 1) {
        FLimit = DeblockLimitValuesV2[QIndex];
    } else {
        FLimit = DeblockLimitValuesV1[QIndex];
    }
    BoundingValuePtr = (int *)SetupDeblockValueArray(ppi, FLimit);
    LineLength = ppi->YStride;
    FragsAcross = ppi->HFragments;
    FragsDown = ppi->VFragments;
    RowStart = ppi->ReconYDataOffset;
    j = 0;
    while (j <= 2) {
        if (j == 0) {
            LineLength = ppi->YStride;
            FragsAcross = ppi->HFragments;
            FragsDown = ppi->VFragments;
            RowStart = ppi->ReconYDataOffset;
        } else if (j == 1) {
            LineLength = ppi->UVStride;
            FragsAcross = ppi->HFragments >> 1;
            FragsDown = ppi->VFragments >> 1;
            RowStart = ppi->ReconUDataOffset;
        } else if (j == 2) {
            LineLength = ppi->UVStride;
            FragsAcross = ppi->HFragments >> 1;
            FragsDown = ppi->VFragments >> 1;
            RowStart = ppi->ReconVDataOffset;
        }
        if (FragsAcross != 0) {
            memcpy(DestBuffer + RowStart, SrcBuffer + RowStart,
                   FragsAcross * 8);
            for (m = 1; m < FragsAcross; m++) {
                FilterHoriz_Simple(ppi, DestBuffer + RowStart + m * 8 - 2,
                                   LineLength, BoundingValuePtr);
            }
            for (n = 1; n < FragsDown; n++) {
                NextRow = RowStart + n * LineLength * 8;
                memcpy(DestBuffer + NextRow, SrcBuffer + NextRow, FragsAcross * 8);
                FilterVert_Simple(ppi, DestBuffer + NextRow - 2 * LineLength,
                                  LineLength, BoundingValuePtr);
                for (m = 1; m < FragsAcross; m++) {
                    FilterHoriz_Simple(ppi, DestBuffer + NextRow + m * 8 - 2,
                                       LineLength, BoundingValuePtr);
                    FilterVert_Simple(ppi, DestBuffer + NextRow + m * 8 - 2,
                                      LineLength, BoundingValuePtr);
                }
            }
        }
        j++;
    }
}
