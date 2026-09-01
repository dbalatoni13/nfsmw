void fillidctconstants(void) {}

extern void (*idct[65])(short *, short *, short *);
extern void (*idctc[65])(short *, short *, short *);

extern void (*ClearSysState)(void);
extern void (*ReconIntra)(void);
extern void (*ReconInter)(void);
extern void (*ReconInterHalfPixel2)(void);
extern void (*AverageBlock)(void);
extern void (*UnpackBlock)(void);
extern void (*ReconBlock)(void);
extern void (*SubtractBlock)(void);
extern void (*CopyBlock)(void);
extern void (*Copy12x12)(void);
extern void (*FilterBlockBil_8)(void);
extern void (*FilterBlock)(void);

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

void UtilMachineSpecificConfig(void) {
    int i;

    for (i = 0; i < 65; i++) {
        if (i < 2) {
            idct[i] = IDct1_GC;
        } else if (i < 11) {
            idct[i] = IDct10;
        } else {
            idct[i] = IDct64_GC;
        }
    }

    for (i = 0; i < 65; i++) {
        if (i < 2) {
            idctc[i] = IDct1_GC;
        } else if (i < 11) {
            idct[i] = IDct10;
        } else {
            idctc[i] = IDct64_GC;
        }
    }

    ClearSysState = ClearSysState_C;
    ReconIntra = ScalarReconIntra_GC;
    ReconInter = ScalarReconInter_GC;
    ReconInterHalfPixel2 = ScalarReconInterHalfPixel2;
    AverageBlock = AverageBlock_C;
    UnpackBlock = UnpackBlock_GC;
    ReconBlock = ReconBlock_GC;
    SubtractBlock = SubtractBlock_C;
    CopyBlock = CopyBlock_C;
    Copy12x12 = Copy12x12_C;
    FilterBlockBil_8 = FilterBlockBil_8_C;
    FilterBlock = FilterBlock_GC;
}
