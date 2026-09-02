#include <string.h>
#include <stdlib.h>

typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef signed char INT8;
typedef int INT32;

#define VAL_RANGE 0x100

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

extern unsigned char LimitVal_VP31[0x300];

unsigned int DeblockLimitValuesVp4[64] = {
    30, 25, 20, 20, 15, 15, 14, 14,
    13, 13, 12, 12, 11, 11, 10, 10,
    9, 9, 8, 8, 7, 7, 7, 7,
    6, 6, 6, 6, 5, 5, 5, 5,
    4, 4, 4, 4, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1
};

unsigned int DeblockLimitValuesVp5[64] = {
    15, 15, 15, 15, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 9, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 5, 5, 5, 5,
    5, 4, 4, 4, 4, 4, 4, 3,
    3, 3, 3, 3, 3, 2, 2, 2,
    2, 2, 1, 1, 1, 0, 0, 0
};

unsigned int DeblockLimitValuesVp6[64] = {
    15, 15, 15, 15, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 9, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 5, 5, 5, 5,
    5, 4, 4, 4, 4, 4, 4, 3,
    3, 3, 3, 3, 3, 2, 2, 2,
    2, 2, 1, 1, 1, 0, 0, 0
};

extern unsigned int *DeblockLimitValuesV2;
extern int *(*SetupDeblockValueArray)(POSTPROC_INSTANCE *pbi, int FLimit);
extern unsigned int *DCQuantScaleV2;
extern unsigned int *DCQuantScaleUV;
extern unsigned int *DCQuantScaleV1;

typedef POSTPROC_INSTANCE *xPB_INST;

extern void (*DeblockLoopFilteredBand)(void);
extern void (*DeblockNonFilteredBand)(void);
extern void (*DeblockNonFilteredBandNewFilter)(void);

int *SetupDeblockValueArray_Generic(POSTPROC_INSTANCE *pbi, int FLimit) {
    int i;
    int *DeblockValuePtr;

    DeblockValuePtr = &pbi->DeblockBoundingValue[256];
    memset(pbi->DeblockBoundingValue, 0,
           512 * sizeof(*pbi->DeblockBoundingValue));
    for (i = 0; i < FLimit; i++) {
        DeblockValuePtr[-i - FLimit] = -FLimit + i;
        DeblockValuePtr[-i] = -i;
        DeblockValuePtr[i] = i;
        DeblockValuePtr[i + FLimit] = FLimit - i;
    }
    return DeblockValuePtr;
}

void SetupDeblocker(POSTPROC_INSTANCE *pbi) {
    int FLimit;

    if (pbi->Vp3VersionNo >= 2) {
        FLimit = DeblockLimitValuesV2[pbi->FrameQIndex];
        pbi->DeblockValuePtr = SetupDeblockValueArray_Generic(pbi, FLimit);
    } else {
        FLimit = DeblockLimitValuesV2[pbi->FrameQIndex];
        pbi->DeblockValuePtr = SetupDeblockValueArray(pbi, FLimit);
    }
}

void DeblockVerticalEdgesInLoopFilteredBand
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    INT32 QStep;
    INT32 FLimit;
    INT32 p1, p2;
    INT32 psum;
    INT32 v[10];
    INT32 Sum1, Sum2;
    INT32 Variance1, Variance2;
    UINT8 *Src, *Des;
    UINT32 CurrentFrag = StartFrag;

    while (CurrentFrag < (StartFrag + FragsAcross - 1)) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag + 1);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag + 1);

        QStep = QuantScale[pbi->FragQIndex[CurrentFrag + 1]];
        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[1] = Src[-4];
            v[2] = Src[-3];
            v[3] = Src[-2];
            v[4] = Src[-1];
            v[5] = Src[0];
            v[6] = Src[+1];
            v[7] = Src[+2];
            v[8] = Src[+3];

            Variance1 = Variance2 = 0;
            Sum1 = Sum2 = 0;

            for (k = 1; k <= 4; k++) {
                Sum1 += v[k];
                Variance1 += v[k] * v[k];
            }

            for (k = 5; k <= 8; k++) {
                Sum2 += v[k];
                Variance2 += v[k] * v[k];
            }
            Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
            Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
            pbi->FragmentVariances[CurrentFrag] += Variance1;
            pbi->FragmentVariances[CurrentFrag + 1] += Variance2;

            if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = (abs(Src[-4] - Src[-5]) < QStep) ? Src[-5] : Src[-4];
                p2 = (abs(Src[+3] - Src[+4]) < QStep) ? Src[+4] : Src[+3];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                psum += v[5] - p1;
                Des[-3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                psum += v[6] - p1;
                Des[-2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                psum += v[7] - p1;
                Des[-1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                   (v[7] - v[8])) >> 4);
                psum += v[8] - v[1];
                Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                  v[8] + p2) >> 4);
                psum += p2 - v[2];
                Des[+1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                psum += p2 - v[3];
                Des[+2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                psum += p2 - v[4];
                Des[+3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
            }

            Src += PlaneLineStep;
            Des += PlaneLineStep;
        }

        CurrentFrag++;
    }
}

void DeblockLoopFilteredBand_C
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    UINT32 CurrentFrag = StartFrag;
    INT32 QStep;
    INT32 FLimit;
    UINT8 *Src, *Des;
    INT32 psum;
    INT32 v[10];
    INT32 p1, p2;
    INT32 w1, w2, w3, w4, w5;
    INT32 Variance1, Variance2;
    INT32 Sum1, Sum2;

    w1 = PlaneLineStep;
    w2 = PlaneLineStep * 2;
    w3 = PlaneLineStep * 3;
    w4 = PlaneLineStep * 4;
    w5 = PlaneLineStep * 5;

    while (CurrentFrag < StartFrag + FragsAcross) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag);

        QStep = QuantScale[pbi->FragQIndex[CurrentFrag + FragsAcross]];
        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[1] = Src[-w4];
            v[2] = Src[-w3];
            v[3] = Src[-w2];
            v[4] = Src[-w1];
            v[5] = Src[0];
            v[6] = Src[+w1];
            v[7] = Src[+w2];
            v[8] = Src[+w3];

            Variance1 = Variance2 = 0;
            Sum1 = Sum2 = 0;

            for (k = 1; k <= 4; k++) {
                Sum1 += v[k];
                Variance1 += v[k] * v[k];
            }
            for (k = 5; k <= 8; k++) {
                Sum2 += v[k];
                Variance2 += v[k] * v[k];
            }
            Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
            Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
            pbi->FragmentVariances[CurrentFrag] += Variance1;
            pbi->FragmentVariances[CurrentFrag + FragsAcross] += Variance2;

            if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = (abs(Src[-w4] - Src[-w5]) < QStep) ? Src[-w5] : Src[-w4];
                p2 = (abs(Src[+w3] - Src[+w4]) < QStep) ? Src[+w4] : Src[+w3];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-w4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                psum += v[5] - p1;
                Des[-w3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                psum += v[6] - p1;
                Des[-w2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                psum += v[7] - p1;
                Des[-w1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                   (v[7] - v[8])) >> 4);
                psum += v[8] - v[1];
                Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                  v[8] + p2) >> 4);
                psum += p2 - v[2];
                Des[+w1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                psum += p2 - v[3];
                Des[+w2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                psum += p2 - v[4];
                Des[+w3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
            } else {
                Des[-w4] = Src[-w4];
                Des[-w3] = Src[-w3];
                Des[-w2] = Src[-w2];
                Des[-w1] = Src[-w1];
                Des[0] = Src[0];
                Des[+w1] = Src[+w1];
                Des[+w2] = Src[+w2];
                Des[+w3] = Src[+w3];
            }
            Src++;
            Des++;
        }
        CurrentFrag++;
    }

    CurrentFrag = StartFrag;

    while (CurrentFrag < (StartFrag + FragsAcross - 1)) {
        Des = DesPtr - 8 * PlaneLineStep + 8 * (CurrentFrag - StartFrag + 1);
        Src = Des;

        QStep = QuantScale[pbi->FragQIndex[CurrentFrag + 1]];
        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[1] = Src[-4];
            v[2] = Src[-3];
            v[3] = Src[-2];
            v[4] = Src[-1];
            v[5] = Src[0];
            v[6] = Src[+1];
            v[7] = Src[+2];
            v[8] = Src[+3];

            Variance1 = Variance2 = 0;
            Sum1 = Sum2 = 0;

            for (k = 1; k <= 4; k++) {
                Sum1 += v[k];
                Variance1 += v[k] * v[k];
            }
            for (k = 5; k <= 8; k++) {
                Sum2 += v[k];
                Variance2 += v[k] * v[k];
            }
            Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
            Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
            pbi->FragmentVariances[CurrentFrag] += Variance1;
            pbi->FragmentVariances[CurrentFrag + 1] += Variance2;

            if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = (abs(Src[-4] - Src[-5]) < QStep) ? Src[-5] : Src[-4];
                p2 = (abs(Src[+3] - Src[+4]) < QStep) ? Src[+4] : Src[+3];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                psum += v[5] - p1;
                Des[-3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                psum += v[6] - p1;
                Des[-2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                psum += v[7] - p1;
                Des[-1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                   (v[7] - v[8])) >> 4);
                psum += v[8] - v[1];
                Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                  v[8] + p2) >> 4);
                psum += p2 - v[2];
                Des[+1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                psum += p2 - v[3];
                Des[+2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                psum += p2 - v[4];
                Des[+3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
            }

            Src += PlaneLineStep;
            Des += PlaneLineStep;
        }

        CurrentFrag++;
    }
}

void DeblockVerticalEdgesInNonFilteredBand
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    INT32 QStep;
    INT32 FLimit;
    INT32 psum;
    INT32 v[10];
    INT32 p1, p2;
    INT32 Sum1, Sum2;
    INT32 Variance1, Variance2;
    UINT8 *Src, *Des;
    UINT32 CurrentFrag = StartFrag;

    while (CurrentFrag < (StartFrag + FragsAcross - 1)) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag + 1);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag + 1);

        QStep = QuantScale[pbi->FragQIndex[CurrentFrag + 1]];
        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[1] = Src[-4];
            v[2] = Src[-3];
            v[3] = Src[-2];
            v[4] = Src[-1];
            v[5] = Src[0];
            v[6] = Src[+1];
            v[7] = Src[+2];
            v[8] = Src[+3];

            Variance1 = Variance2 = 0;
            Sum1 = Sum2 = 0;

            for (k = 1; k <= 4; k++) {
                Sum1 += v[k];
                Variance1 += v[k] * v[k];
            }
            for (k = 5; k <= 8; k++) {
                Sum2 += v[k];
                Variance2 += v[k] * v[k];
            }
            Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
            Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
            pbi->FragmentVariances[CurrentFrag] += Variance1;
            pbi->FragmentVariances[CurrentFrag + 1] += Variance2;

            if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = (abs(Src[-4] - Src[-5]) < QStep) ? Src[-5] : Src[-4];
                p2 = (abs(Src[+3] - Src[+4]) < QStep) ? Src[+4] : Src[+3];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                psum += v[5] - p1;
                Des[-3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                psum += v[6] - p1;
                Des[-2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                psum += v[7] - p1;
                Des[-1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                  (v[7] - v[8])) >> 4);
                psum += v[8] - v[1];
                Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                 v[8] + p2) >> 4);
                psum += p2 - v[2];
                Des[+1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                psum += p2 - v[3];
                Des[+2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                psum += p2 - v[4];
                Des[+3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
            } else {
                INT32 FiltVal;
                UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                Des[-1] = LimitTable[(INT32)v[4] + FiltVal];
                Des[0] = LimitTable[(INT32)v[5] - FiltVal];
            }

            Src += PlaneLineStep;
            Des += PlaneLineStep;
        }

        CurrentFrag++;
    }
}

void DeblockVerticalEdgesInNonFilteredBandNewFilter
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    INT32 QStep;
    INT32 FLimit;
    INT32 psum;
    INT32 v[10];
    INT32 p1, p2;
    INT32 Sum1, Sum2;
    UINT8 *Src, *Des;
    UINT32 CurrentFrag = StartFrag;

    QStep = QuantScale[pbi->FrameQIndex];

    for (CurrentFrag = StartFrag; CurrentFrag < (StartFrag + FragsAcross);
         CurrentFrag++) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag + 1);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag + 1);

        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[0] = Src[-5];
            v[1] = Src[-4];
            v[2] = Src[-3];
            v[3] = Src[-2];
            v[4] = Src[-1];
            v[5] = Src[0];
            v[6] = Src[+1];
            v[7] = Src[+2];
            v[8] = Src[+3];
            v[9] = Src[+4];

            Sum1 = Sum2 = 0;
            for (k = 1; k <= 4; k++)
                Sum1 += abs(v[k] - v[k - 1]);
            for (k = 5; k <= 8; k++)
                Sum2 += abs(v[k] - v[k + 1]);

            if ((Sum1 < FLimit) && (Sum2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = v[0];
                p2 = v[9];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-4] = (INT8)((psum + v[1]) >> 3);
                psum += v[5] - p1;
                Des[-3] = (INT8)((psum + v[2]) >> 3);
                psum += v[6] - p1;
                Des[-2] = (INT8)((psum + v[3]) >> 3);
                psum += v[7] - p1;
                Des[-1] = (INT8)((psum + v[4]) >> 3);
                psum += v[8] - v[1];
                Des[0] = (INT8)((psum + v[5]) >> 3);
                psum += p2 - v[2];
                Des[+1] = (INT8)((psum + v[6]) >> 3);
                psum += p2 - v[3];
                Des[+2] = (INT8)((psum + v[7]) >> 3);
                psum += p2 - v[4];
                Des[+3] = (INT8)((psum + v[8]) >> 3);
            } else {
                INT32 FiltVal;
                UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                Des[-1] = LimitTable[(INT32)v[4] + FiltVal];
                Des[0] = LimitTable[(INT32)v[5] - FiltVal];
            }

            Src += PlaneLineStep;
            Des += PlaneLineStep;
        }
    }
}

void DeblockNonFilteredBand_C
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    INT32 QStep;
    INT32 FLimit;
    INT32 psum;
    INT32 v[10];
    INT32 p1, p2;
    INT32 w1, w2, w3, w4, w5;
    INT32 Variance1, Variance2;
    INT32 Sum1, Sum2;
    UINT8 *Src, *Des;
    UINT32 CurrentFrag = StartFrag;

    w1 = PlaneLineStep;
    w2 = PlaneLineStep * 2;
    w3 = PlaneLineStep * 3;
    w4 = PlaneLineStep * 4;
    w5 = PlaneLineStep * 5;

    while (CurrentFrag < StartFrag + FragsAcross) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag);
        QStep = QuantScale[pbi->FragQIndex[CurrentFrag + FragsAcross]];
        FLimit = (QStep * QStep * 3) >> 5;

        for (j = 0; j < 8; j++) {
            v[1] = Src[-w4];
            v[2] = Src[-w3];
            v[3] = Src[-w2];
            v[4] = Src[-w1];
            v[5] = Src[0];
            v[6] = Src[+w1];
            v[7] = Src[+w2];
            v[8] = Src[+w3];

            Variance1 = Variance2 = 0;
            Sum1 = Sum2 = 0;
            for (k = 1; k <= 4; k++) {
                Sum1 += v[k];
                Variance1 += v[k] * v[k];
            }
            for (k = 5; k <= 8; k++) {
                Sum2 += v[k];
                Variance2 += v[k] * v[k];
            }
            Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
            Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
            pbi->FragmentVariances[CurrentFrag] += Variance1;
            pbi->FragmentVariances[CurrentFrag + FragsAcross] += Variance2;

            if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = (abs(Src[-w4] - Src[-w5]) < QStep) ? Src[-w5] : Src[-w4];
                p2 = (abs(Src[+w3] - Src[+w4]) < QStep) ? Src[+w4] : Src[+w3];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-w4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                psum += v[5] - p1;
                Des[-w3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                psum += v[6] - p1;
                Des[-w2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                psum += v[7] - p1;
                Des[-w1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                   (v[7] - v[8])) >> 4);
                psum += v[8] - v[1];
                Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                  v[8] + p2) >> 4);
                psum += p2 - v[2];
                Des[+w1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                psum += p2 - v[3];
                Des[+w2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                psum += p2 - v[4];
                Des[+w3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
            } else {
                INT32 FiltVal;
                UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                Des[-w1] = LimitTable[(INT32)v[4] + FiltVal];
                Des[0] = LimitTable[(INT32)v[5] - FiltVal];
                Des[-w4] = Src[-w4];
                Des[-w3] = Src[-w3];
                Des[-w2] = Src[-w2];
                Des[+w1] = Src[+w1];
                Des[+w2] = Src[+w2];
                Des[+w3] = Src[+w3];
            }

            Src++;
            Des++;
        }

        if (CurrentFrag == StartFrag) {
            CurrentFrag++;
        } else {
            Des = DesPtr - 8 * PlaneLineStep + 8 * (CurrentFrag - StartFrag);
            Src = Des;
            QStep = QuantScale[pbi->FragQIndex[CurrentFrag]];
            FLimit = (QStep * QStep * 3) >> 5;

            for (j = 0; j < 8; j++) {
                v[1] = Src[-4];
                v[2] = Src[-3];
                v[3] = Src[-2];
                v[4] = Src[-1];
                v[5] = Src[0];
                v[6] = Src[+1];
                v[7] = Src[+2];
                v[8] = Src[+3];

                Variance1 = Variance2 = 0;
                Sum1 = Sum2 = 0;
                for (k = 1; k <= 4; k++) {
                    Sum1 += v[k];
                    Variance1 += v[k] * v[k];
                }
                for (k = 5; k <= 8; k++) {
                    Sum2 += v[k];
                    Variance2 += v[k] * v[k];
                }
                Variance1 -= ((Sum1 >> 1) * ((Sum1 + 1) >> 1));
                Variance2 -= ((Sum2 >> 1) * ((Sum2 + 1) >> 1));
                pbi->FragmentVariances[CurrentFrag - 1] += Variance1;
                pbi->FragmentVariances[CurrentFrag] += Variance2;

                if ((Variance1 < FLimit) && (Variance2 < FLimit) &&
                    ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                    p1 = (abs(Src[-4] - Src[-5]) < QStep) ? Src[-5] : Src[-4];
                    p2 = (abs(Src[+3] - Src[+4]) < QStep) ? Src[+4] : Src[+3];

                    psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                    Des[-4] = (INT8)((((psum + v[1]) << 1) - (v[4] - v[5])) >> 4);
                    psum += v[5] - p1;
                    Des[-3] = (INT8)((((psum + v[2]) << 1) - (v[5] - v[6])) >> 4);
                    psum += v[6] - p1;
                    Des[-2] = (INT8)((((psum + v[3]) << 1) - (v[6] - v[7])) >> 4);
                    psum += v[7] - p1;
                    Des[-1] = (INT8)((((psum + v[4]) << 1) + p1 - v[1] -
                                      (v[7] - v[8])) >> 4);
                    psum += v[8] - v[1];
                    Des[0] = (INT8)((((psum + v[5]) << 1) + (v[1] - v[2]) -
                                     v[8] + p2) >> 4);
                    psum += p2 - v[2];
                    Des[+1] = (INT8)((((psum + v[6]) << 1) + (v[2] - v[3])) >> 4);
                    psum += p2 - v[3];
                    Des[+2] = (INT8)((((psum + v[7]) << 1) + (v[3] - v[4])) >> 4);
                    psum += p2 - v[4];
                    Des[+3] = (INT8)((((psum + v[8]) << 1) + (v[4] - v[5])) >> 4);
                } else {
                    INT32 FiltVal;
                    UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                    FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                    FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                    Des[-1] = LimitTable[(INT32)v[4] + FiltVal];
                    Des[0] = LimitTable[(INT32)v[5] - FiltVal];
                }

                Src += PlaneLineStep;
                Des += PlaneLineStep;
            }
        }

        CurrentFrag++;
    }
}

void DeblockNonFilteredBandNewFilter_C
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 PlaneLineStep,
    UINT32 FragsAcross,
    UINT32 StartFrag,
    UINT32 *QuantScale
)
{
    UINT32 j, k;
    INT32 QStep;
    INT32 FLimit;
    INT32 psum;
    INT32 v[10];
    INT32 p1, p2;
    INT32 w1, w2, w3, w4, w5;
    INT32 Sum1, Sum2;
    UINT8 *Src, *Des;
    UINT32 CurrentFrag = StartFrag;

    w1 = PlaneLineStep;
    w2 = PlaneLineStep * 2;
    w3 = PlaneLineStep * 3;
    w4 = PlaneLineStep * 4;
    w5 = PlaneLineStep * 5;
    QStep = QuantScale[pbi->FrameQIndex];

    while (CurrentFrag < StartFrag + FragsAcross) {
        Src = SrcPtr + 8 * (CurrentFrag - StartFrag);
        Des = DesPtr + 8 * (CurrentFrag - StartFrag);
        FLimit = (QStep * 3) >> 2;

        for (j = 0; j < 8; j++) {
            v[0] = Src[-w5];
            v[1] = Src[-w4];
            v[2] = Src[-w3];
            v[3] = Src[-w2];
            v[4] = Src[-w1];
            v[5] = Src[0];
            v[6] = Src[+w1];
            v[7] = Src[+w2];
            v[8] = Src[+w3];
            v[9] = Src[+w4];

            Sum1 = Sum2 = 0;
            for (k = 1; k <= 4; k++)
                Sum1 += abs(v[k] - v[k - 1]);
            for (k = 5; k <= 8; k++)
                Sum2 += abs(v[k] - v[k + 1]);

            pbi->FragmentVariances[CurrentFrag] += (Sum1 > 255) ? 255 : Sum1;
            pbi->FragmentVariances[CurrentFrag + FragsAcross] +=
                (Sum2 > 255) ? 255 : Sum2;

            if ((Sum1 < FLimit) && (Sum2 < FLimit) &&
                ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                p1 = v[0];
                p2 = v[9];

                psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                Des[-w4] = (INT8)((psum + v[1]) >> 3);
                psum += v[5] - p1;
                Des[-w3] = (INT8)((psum + v[2]) >> 3);
                psum += v[6] - p1;
                Des[-w2] = (INT8)((psum + v[3]) >> 3);
                psum += v[7] - p1;
                Des[-w1] = (INT8)((psum + v[4]) >> 3);
                psum += v[8] - v[1];
                Des[0] = (INT8)((psum + v[5]) >> 3);
                psum += p2 - v[2];
                Des[+w1] = (INT8)((psum + v[6]) >> 3);
                psum += p2 - v[3];
                Des[+w2] = (INT8)((psum + v[7]) >> 3);
                psum += p2 - v[4];
                Des[+w3] = (INT8)((psum + v[8]) >> 3);
            } else {
                INT32 FiltVal;
                UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                Des[-w1] = LimitTable[(INT32)v[4] + FiltVal];
                Des[0] = LimitTable[(INT32)v[5] - FiltVal];
                Des[-w4] = Src[-w4];
                Des[-w3] = Src[-w3];
                Des[-w2] = Src[-w2];
                Des[+w1] = Src[+w1];
                Des[+w2] = Src[+w2];
                Des[+w3] = Src[+w3];
            }

            Src++;
            Des++;
        }

        if (CurrentFrag == StartFrag) {
            CurrentFrag++;
        } else {
            Des = DesPtr - 8 * PlaneLineStep + 8 * (CurrentFrag - StartFrag);
            Src = Des;
            FLimit = (QStep * 3) >> 2;

            for (j = 0; j < 8; j++) {
                v[0] = Src[-5];
                v[1] = Src[-4];
                v[2] = Src[-3];
                v[3] = Src[-2];
                v[4] = Src[-1];
                v[5] = Src[0];
                v[6] = Src[+1];
                v[7] = Src[+2];
                v[8] = Src[+3];
                v[9] = Src[+4];

                Sum1 = Sum2 = 0;
                for (k = 1; k <= 4; k++)
                    Sum1 += abs(v[k] - v[k - 1]);
                for (k = 5; k <= 8; k++)
                    Sum2 += abs(v[k] - v[k + 1]);

                pbi->FragmentVariances[CurrentFrag - 1] +=
                    (Sum1 > 255) ? 255 : Sum1;
                pbi->FragmentVariances[CurrentFrag] +=
                    (Sum2 > 255) ? 255 : Sum2;

                if ((Sum1 < FLimit) && (Sum2 < FLimit) &&
                    ((v[5] - v[4]) < QStep) && ((v[4] - v[5]) < QStep)) {
                    p1 = v[0];
                    p2 = v[9];

                    psum = p1 + p1 + p1 + v[1] + v[2] + v[3] + v[4] + 4;
                    Des[-4] = (INT8)((psum + v[1]) >> 3);
                    psum += v[5] - p1;
                    Des[-3] = (INT8)((psum + v[2]) >> 3);
                    psum += v[6] - p1;
                    Des[-2] = (INT8)((psum + v[3]) >> 3);
                    psum += v[7] - p1;
                    Des[-1] = (INT8)((psum + v[4]) >> 3);
                    psum += v[8] - v[1];
                    Des[0] = (INT8)((psum + v[5]) >> 3);
                    psum += p2 - v[2];
                    Des[+1] = (INT8)((psum + v[6]) >> 3);
                    psum += p2 - v[3];
                    Des[+2] = (INT8)((psum + v[7]) >> 3);
                    psum += p2 - v[4];
                    Des[+3] = (INT8)((psum + v[8]) >> 3);
                } else {
                    INT32 FiltVal;
                    UINT8 *LimitTable = &LimitVal_VP31[VAL_RANGE];

                    FiltVal = v[3] - v[4] * 3 + v[5] * 3 - v[6];
                    FiltVal = pbi->DeblockValuePtr[(FiltVal + 4) >> 3];
                    Des[-1] = LimitTable[(INT32)v[4] + FiltVal];
                    Des[0] = LimitTable[(INT32)v[5] - FiltVal];
                }

                Src += PlaneLineStep;
                Des += PlaneLineStep;
            }
            CurrentFrag++;
        }
    }
}

void DeblockPlane
(
    POSTPROC_INSTANCE *pbi,
    UINT8 *SourceBuffer,
    UINT8 *DestinationBuffer,
    UINT32 Channel
)
{
    UINT32 i, j, k;
    UINT32 PixelIndex;
    UINT32 FragsDown = 0;
    UINT32 FragsAcross = 0;
    UINT32 StartFrag = 0;
    UINT32 PlaneLineStep = 0;
    UINT8 *SrcPtr = 0, *DesPtr = 0;
    UINT32 *QuantScale = 0;
    typedef void (*ApplyFilterToBand)(xPB_INST, UINT8 *, UINT8 *, UINT32,
                                      UINT32, UINT32, UINT32 *);
    ApplyFilterToBand DeblockBand;
    ApplyFilterToBand DeblockVerticalEdgesInBand;

    if (pbi->Vp3VersionNo >= 2) {
        DeblockBand = DeblockNonFilteredBand;
        DeblockVerticalEdgesInBand = DeblockVerticalEdgesInNonFilteredBand;
    } else {
        DeblockBand = DeblockLoopFilteredBand;
        DeblockVerticalEdgesInBand = DeblockVerticalEdgesInLoopFilteredBand;
    }

    switch (Channel) {
    case 0:
        PlaneLineStep = pbi->YStride;
        FragsAcross = pbi->HFragments;
        FragsDown = pbi->VFragments;
        StartFrag = 0;
        PixelIndex = pbi->ReconYDataOffset;
        SrcPtr = &SourceBuffer[PixelIndex];
        DesPtr = &DestinationBuffer[PixelIndex];
        break;
    case 1:
        PlaneLineStep = pbi->UVStride;
        FragsAcross = pbi->HFragments / 2;
        FragsDown = pbi->VFragments / 2;
        StartFrag = pbi->YPlaneFragments;
        PixelIndex = pbi->ReconUDataOffset;
        SrcPtr = &SourceBuffer[PixelIndex];
        DesPtr = &DestinationBuffer[PixelIndex];
        break;
    default:
        PlaneLineStep = pbi->UVStride;
        FragsAcross = pbi->HFragments / 2;
        FragsDown = pbi->VFragments / 2;
        StartFrag = pbi->YPlaneFragments + pbi->UVPlaneFragments;
        PixelIndex = pbi->ReconVDataOffset;
        SrcPtr = &SourceBuffer[PixelIndex];
        DesPtr = &DestinationBuffer[PixelIndex];
        break;
    }

    if (pbi->Vp3VersionNo >= 2) {
        switch (Channel) {
        case 0:
            QuantScale = DCQuantScaleV2;
            break;
        case 1:
        case 2:
            QuantScale = DCQuantScaleUV;
            break;
        }
    } else {
        QuantScale = DCQuantScaleV1;
    }

    for (i = 0; i < 4; i++)
        for (j = 0; j < PlaneLineStep; j++)
            DesPtr[i * PlaneLineStep + j] = SrcPtr[i * PlaneLineStep + j];

    k = 1;
    while (k < FragsDown) {
        SrcPtr += 8 * PlaneLineStep;
        DesPtr += 8 * PlaneLineStep;
        DeblockBand(pbi, SrcPtr, DesPtr, PlaneLineStep, FragsAcross,
                    StartFrag, QuantScale);
        StartFrag += FragsAcross;
        k++;
    }

    for (i = 0; i < 4; i++)
        for (j = 0; j < PlaneLineStep; j++)
            DesPtr[(i + 4) * PlaneLineStep + j] =
                SrcPtr[(i + 4) * PlaneLineStep + j];

    DeblockVerticalEdgesInBand(pbi, SrcPtr, DesPtr, PlaneLineStep,
                               FragsAcross, StartFrag, QuantScale);
}

void DeblockPlaneNew
(
    POSTPROC_INSTANCE *pbi,
    UINT32 PlaneLineStep,
    UINT32 StartFrag,
    UINT32 FragsAcross,
    UINT32 FragsDown,
    UINT8 *SrcPtr,
    UINT8 *DesPtr,
    UINT32 *QuantScale
)
{
    UINT32 i, k;
    typedef void (*ApplyFilterToBand)(xPB_INST, UINT8 *, UINT8 *, UINT32,
                                      UINT32, UINT32, UINT32 *);
    ApplyFilterToBand DeblockBand;
    ApplyFilterToBand DeblockVerticalEdgesInBand;

    DeblockBand = DeblockNonFilteredBandNewFilter;
    DeblockVerticalEdgesInBand = DeblockVerticalEdgesInNonFilteredBandNewFilter;

    for (i = 0; i < 4; i++)
        memcpy(DesPtr + i * PlaneLineStep, SrcPtr + i * PlaneLineStep,
               PlaneLineStep);

    k = 1;
    while (k < FragsDown) {
        SrcPtr += 8 * PlaneLineStep;
        DesPtr += 8 * PlaneLineStep;
        DeblockBand(pbi, SrcPtr, DesPtr, PlaneLineStep, FragsAcross,
                    StartFrag, QuantScale);
        StartFrag += FragsAcross;
        k++;
    }

    for (i = 0; i < 4; i++)
        memcpy(DesPtr + (i + 4) * PlaneLineStep,
               SrcPtr + (i + 4) * PlaneLineStep, PlaneLineStep);

    DeblockVerticalEdgesInBand(pbi, SrcPtr, DesPtr, PlaneLineStep,
                               FragsAcross, StartFrag, QuantScale);
}

void DeblockFrame(POSTPROC_INSTANCE *pbi, UINT8 *SourceBuffer,
                  UINT8 *DestinationBuffer)
{
    memset(pbi->FragmentVariances, 0,
           pbi->UnitFragments * sizeof(INT32));

    if (pbi->Vp3VersionNo > 1)
        SetupDeblocker(pbi);

    if (pbi->Vp3VersionNo > 4) {
        DeblockPlaneNew(pbi, pbi->YStride, 0, pbi->HFragments,
                        pbi->VFragments,
                        &SourceBuffer[pbi->ReconYDataOffset],
                        &DestinationBuffer[pbi->ReconYDataOffset],
                        DCQuantScaleV2);
        DeblockPlaneNew(pbi, pbi->UVStride, 0, pbi->HFragments / 2,
                        pbi->VFragments / 2,
                        &SourceBuffer[pbi->ReconUDataOffset],
                        &DestinationBuffer[pbi->ReconUDataOffset],
                        DCQuantScaleUV);
        DeblockPlaneNew(pbi, pbi->UVStride, 0, pbi->HFragments / 2,
                        pbi->VFragments / 2,
                        &SourceBuffer[pbi->ReconVDataOffset],
                        &DestinationBuffer[pbi->ReconVDataOffset],
                        DCQuantScaleUV);
    } else {
        DeblockPlane(pbi, SourceBuffer, DestinationBuffer, 0);
        DeblockPlane(pbi, SourceBuffer, DestinationBuffer, 1);
        DeblockPlane(pbi, SourceBuffer, DestinationBuffer, 2);
    }
}

void DeblockFrameInterlaced(POSTPROC_INSTANCE *pbi, UINT8 *SourceBuffer,
                            UINT8 *DestinationBuffer)
{
    INT32 *FragVarPtr;

    SetupDeblocker(pbi);
    FragVarPtr = pbi->FragmentVariances;
    memset(FragVarPtr, 0, pbi->UnitFragments * sizeof(INT32));

    DeblockPlaneNew(pbi, pbi->YStride * 2, 0, pbi->HFragments,
                    pbi->VFragments / 2,
                    &SourceBuffer[pbi->ReconYDataOffset],
                    &DestinationBuffer[pbi->ReconYDataOffset],
                    DCQuantScaleV2);
    pbi->FragmentVariances = pbi->FragmentVariances +
                             pbi->HFragments * pbi->VFragments / 2;
    DeblockPlaneNew(pbi, pbi->YStride * 2, 0, pbi->HFragments,
                    pbi->VFragments / 2,
                    &SourceBuffer[pbi->ReconYDataOffset + pbi->YStride],
                    &DestinationBuffer[pbi->ReconYDataOffset + pbi->YStride],
                    DCQuantScaleV2);
    pbi->FragmentVariances = FragVarPtr;

    DeblockPlaneNew(pbi, pbi->UVStride, pbi->YPlaneFragments,
                    pbi->HFragments / 2, pbi->VFragments / 2,
                    &SourceBuffer[pbi->ReconUDataOffset],
                    &DestinationBuffer[pbi->ReconUDataOffset],
                    DCQuantScaleUV);
    DeblockPlaneNew(pbi, pbi->UVStride,
                    pbi->YPlaneFragments + pbi->UVPlaneFragments,
                    pbi->HFragments / 2, pbi->VFragments / 2,
                    &SourceBuffer[pbi->ReconVDataOffset],
                    &DestinationBuffer[pbi->ReconVDataOffset],
                    DCQuantScaleUV);
    return;
}
