typedef void (*VP6_FUNC)(void);

extern VP6_FUNC idct[65];
extern VP6_FUNC idctc[65];

extern VP6_FUNC ClearSysState;
extern VP6_FUNC ReconIntra;
extern VP6_FUNC ReconInter;
extern VP6_FUNC ReconInterHalfPixel2;
extern VP6_FUNC AverageBlock;
extern VP6_FUNC UnpackBlock;
extern VP6_FUNC ReconBlock;
extern VP6_FUNC SubtractBlock;
extern VP6_FUNC CopyBlock;
extern VP6_FUNC Copy12x12;
extern VP6_FUNC FilterBlockBil_8;
extern VP6_FUNC FilterBlock;

extern void idct_func(void);
extern void IDct1_GC(void);
extern void IDct10(void);
extern void IDct64_GC(void);
extern void ClearSysState_C(void);
extern void ScalarReconIntra_GC(void);
extern void ScalarReconInter_GC(void);
extern void ScalarReconInterHalfPixel2(void);
extern void AverageBlock_C(void);
extern void UnpackBlock_GC(void);
extern void ReconBlock_GC(void);
extern void SubtractBlock_C(void);
extern void CopyBlock_C(void);
extern void Copy12x12_C(void);
extern void FilterBlockBil_8_C(void);
extern void FilterBlock_GC(void);

void fillidctconstants(void) {
}

void UtilMachineSpecificConfig(void) {
    int i;
    for (i = 0; i <= 64; i++) {
        if (i <= 1) {
            idct[i] = (VP6_FUNC)IDct1_GC;
        } else if (i <= 10) {
            idct[i] = (VP6_FUNC)IDct10;
        } else {
            idct[i] = (VP6_FUNC)IDct64_GC;
        }
    }
    for (i = 0; i <= 64; i++) {
        if (i <= 1) {
            idctc[i] = (VP6_FUNC)IDct1_GC;
        } else if (i <= 10) {
            idct[i] = (VP6_FUNC)IDct10;
        } else {
            idctc[i] = (VP6_FUNC)IDct64_GC;
        }
    }
    ClearSysState = (VP6_FUNC)ClearSysState_C;
    ReconIntra = (VP6_FUNC)ScalarReconIntra_GC;
    ReconInter = (VP6_FUNC)ScalarReconInter_GC;
    ReconInterHalfPixel2 = (VP6_FUNC)ScalarReconInterHalfPixel2;
    AverageBlock = (VP6_FUNC)AverageBlock_C;
    UnpackBlock = (VP6_FUNC)UnpackBlock_GC;
    ReconBlock = (VP6_FUNC)ReconBlock_GC;
    SubtractBlock = (VP6_FUNC)SubtractBlock_C;
    CopyBlock = (VP6_FUNC)CopyBlock_C;
    Copy12x12 = (VP6_FUNC)Copy12x12_C;
    FilterBlockBil_8 = (VP6_FUNC)FilterBlockBil_8_C;
    FilterBlock = (VP6_FUNC)FilterBlock_GC;
}
