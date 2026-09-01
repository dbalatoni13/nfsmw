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
    void (**idctp)(short *, short *, short *);
    void (**idctcp)(short *, short *, short *);

    idctp = idct;
    for (i = 0; i < 65; i++) {
        if (idctp <= idct + 1) {
            *idctp = IDct1_GC;
        } else if (idctp <= idct + 10) {
            *idctp = IDct10;
        } else {
            *idctp = IDct64_GC;
        }
        idctp++;
    }

    idctcp = idctc;
    for (i = 0; i < 65; i++) {
        if (idctcp <= idctc + 1) {
            *idctcp = IDct1_GC;
        } else if (idctcp <= idctc + 10) {
            idct[i] = IDct10;
        } else {
            *idctcp = IDct64_GC;
        }
        idctcp++;
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
