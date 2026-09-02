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
extern int *(*SetupDeblockValueArray)(POSTPROC_INSTANCE *ppi, int FLimit);
extern void (*FilterHoriz_Simple)(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                                  int LineLength, int *BoundingValuePtr);
extern void (*FilterVert_Simple)(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                                 int LineLength, int *BoundingValuePtr);
extern void *memcpy(void *dest, const void *source, unsigned int size);
extern int abs(int value);

static const unsigned int DeblockLimitValuesV1[64] = {
    30, 25, 20, 20, 15, 15, 14, 14,
    13, 13, 12, 12, 11, 11, 10, 10,
    9, 9, 8, 8, 7, 7, 7, 7,
    6, 6, 6, 6, 5, 5, 5, 5,
    4, 4, 4, 4, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void FilterHoriz_Simple_C(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                          int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable = &LimitVal_VP31[0x100];

    (void)ppi;

    for (j = 0; j < 8; j++) {
        int UseHighVariance;

        FiltVal = (PixelPtr[2] * 3) - (PixelPtr[1] * 3);

        UseHighVariance = abs(PixelPtr[0] - PixelPtr[1]) > 1 ||
                          abs(PixelPtr[2] - PixelPtr[3]) > 1;

        if (UseHighVariance) {
            FiltVal += PixelPtr[0] - PixelPtr[3];
        }

        FiltVal = BoundingValuePtr[(FiltVal + 4) >> 3];

        PixelPtr[1] = LimitTable[(int)PixelPtr[1] + FiltVal];
        PixelPtr[2] = LimitTable[(int)PixelPtr[2] - FiltVal];

        if (!UseHighVariance) {
            FiltVal >>= 1;
            PixelPtr[0] = LimitTable[(int)PixelPtr[0] + FiltVal];
            PixelPtr[3] = LimitTable[(int)PixelPtr[3] - FiltVal];
        }

        PixelPtr += LineLength;
    }
}

void FilterVert_Simple_C(POSTPROC_INSTANCE *ppi, unsigned char *PixelPtr,
                         int LineLength, int *BoundingValuePtr) {
    int j;
    int FiltVal;
    unsigned char *LimitTable = &LimitVal_VP31[0x100];

    (void)ppi;

    for (j = 0; j < 8; j++) {
        int UseHighVariance;

        FiltVal = (((int)PixelPtr[0] * 3) -
                    ((int)PixelPtr[-LineLength] * 3));

        UseHighVariance =
            abs(PixelPtr[-(2 * LineLength)] - PixelPtr[-LineLength]) > 1 ||
            abs(PixelPtr[0] - PixelPtr[LineLength]) > 1;

        if (UseHighVariance) {
            FiltVal += ((int)PixelPtr[-(2 * LineLength)]) -
                       ((int)PixelPtr[LineLength]);
        }

        FiltVal = BoundingValuePtr[(FiltVal + 4) >> 3];

        PixelPtr[-LineLength] =
            LimitTable[(int)PixelPtr[-LineLength] + FiltVal];
        PixelPtr[0] = LimitTable[(int)PixelPtr[0] - FiltVal];

        if (!UseHighVariance) {
            FiltVal >>= 1;
            PixelPtr[-2 * LineLength] =
                LimitTable[(int)PixelPtr[-2 * LineLength] + FiltVal];
            PixelPtr[LineLength] =
                LimitTable[(int)PixelPtr[LineLength] - FiltVal];
        }

        PixelPtr++;
    }
}

void SimpleDeblockFrame(POSTPROC_INSTANCE *ppi, unsigned char *SrcBuffer,
                        unsigned char *DestBuffer) {
    int j, m, n;
    int RowStart = 0;
    int NextRow;
    int FLimit;
    int QIndex;
    int *BoundingValuePtr;
    int LineLength = 0;
    int FragsAcross = ppi->HFragments;
    int FragsDown = ppi->VFragments;

    QIndex = ppi->FrameQIndex;
    if (ppi->Vp3VersionNo >= 2) {
        FLimit = DeblockLimitValuesV2[QIndex];
    }
    else {
        FLimit = DeblockLimitValuesV1[QIndex];
    }
    BoundingValuePtr = SetupDeblockValueArray(ppi, FLimit);

    for (j = 0; j < 3; j++) {
        switch (j) {
        case 0:
            FragsAcross = ppi->HFragments;
            FragsDown = ppi->VFragments;
            LineLength = ppi->YStride;
            RowStart = ppi->ReconYDataOffset;
            break;
        case 1:
            FragsAcross = ppi->HFragments >> 1;
            FragsDown = ppi->VFragments >> 1;
            LineLength = ppi->UVStride;
            RowStart = ppi->ReconUDataOffset;
            break;
        case 2:
            FragsAcross = ppi->HFragments >> 1;
            FragsDown = ppi->VFragments >> 1;
            LineLength = ppi->UVStride;
            RowStart = ppi->ReconVDataOffset;
            break;
        }

        NextRow = LineLength * 8;
        memcpy(&DestBuffer[RowStart], &SrcBuffer[RowStart], 8 * LineLength);

        for (n = 1; n < FragsAcross; n++) {
            FilterHoriz_Simple(ppi, &DestBuffer[RowStart + n * 8 - 2],
                               LineLength, BoundingValuePtr);
        }

        RowStart += NextRow;

        for (m = 1; m < FragsDown; m++) {
            n = 0;
            memcpy(&DestBuffer[RowStart], &SrcBuffer[RowStart], 8 * LineLength);
            FilterVert_Simple(ppi, &DestBuffer[RowStart + n * 8],
                              LineLength, BoundingValuePtr);

            for (n = 1; n < FragsAcross; n++) {
                FilterHoriz_Simple(ppi, &DestBuffer[RowStart + n * 8 - 2],
                                   LineLength, BoundingValuePtr);
                FilterVert_Simple(ppi, &DestBuffer[RowStart + n * 8],
                                  LineLength, BoundingValuePtr);
            }

            RowStart += NextRow;
        }
    }
}
