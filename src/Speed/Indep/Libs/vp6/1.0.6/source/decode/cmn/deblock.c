#include <string.h>

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
} POSTPROC_INSTANCE;

extern unsigned int *DeblockLimitValuesV2;
extern int *(*SetupDeblockValueArray)(POSTPROC_INSTANCE *pbi, int FLimit);

int *SetupDeblockValueArray_Generic(POSTPROC_INSTANCE *pbi, int FLimit) {
    int *DeblockValuePtr;
    int i;

    DeblockValuePtr = pbi->DeblockBoundingValue + 0x100;
    memset(DeblockValuePtr, 0, 2048);
    for (i = 0; i < FLimit; i++) {
        *((volatile int *)&DeblockValuePtr[i - FLimit]) = i - FLimit;
        *((volatile int *)&DeblockValuePtr[-i]) = -i;
        *((volatile int *)&DeblockValuePtr[i]) = i;
        *((volatile int *)&DeblockValuePtr[FLimit + i]) = FLimit - i;
    }
    return DeblockValuePtr;
}

void SetupDeblocker(POSTPROC_INSTANCE *pbi) {
    if (pbi->Vp3VersionNo > 1) {
        pbi->DeblockValuePtr = SetupDeblockValueArray_Generic(
            pbi, DeblockLimitValuesV2[pbi->FrameQIndex]);
    } else {
        pbi->DeblockValuePtr = SetupDeblockValueArray(
            pbi, DeblockLimitValuesV2[pbi->FrameQIndex]);
    }
}
