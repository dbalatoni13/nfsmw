typedef int INT32;
typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef short INT16;

#define VP6_FILTER_WEIGHT 128
#define VP6_FILTER_SHIFT 7

extern void fillidctconstants(void);
extern void UtilMachineSpecificConfig(void);

static int BilinearFilters[8][2] = {
    { 128, 0 },
    { 112, 16 },
    { 96, 32 },
    { 80, 48 },
    { 64, 64 },
    { 48, 80 },
    { 32, 96 },
    { 16, 112 },
};

static int FData[88];

void (*ReconIntra)(INT16 *, UINT8 *, UINT16 *, UINT32);
void (*ReconInter)(INT16 *, UINT8 *, UINT8 *, INT16 *, UINT32);
void (*ReconInterHalfPixel2)(INT16 *, UINT8 *, UINT8 *, UINT8 *, INT16 *, UINT32);
void (*idct[65])(INT16 *, INT16 *, INT16 *);
void (*ClearSysState)(void);
void (*ReconBlock)(INT16 *, INT16 *, UINT8 *, UINT32);
void (*SubtractBlock)(UINT8 *, INT16 *, UINT32);
void (*UnpackBlock)(UINT8 *, INT16 *, UINT32);
void (*AverageBlock)(UINT8 *, UINT8 *, UINT16 *, UINT32);
void (*CopyBlock)(UINT8 *, UINT8 *, UINT32);
void (*Copy12x12)(const UINT8 *, UINT8 *, UINT32, UINT32);
void (*idctc[65])(INT16 *, INT16 *, INT16 *);
void (*FilterBlockBil_8)(UINT8 *, UINT8 *, UINT8 *, UINT32, INT32, INT32);
void (*FilterBlock)(UINT8 *, UINT8 *, UINT16 *, UINT32, INT32, INT32, INT32);


void ClearSysState_C(void) {
}

void AverageBlock_C(UINT8 *ReconPtr1, UINT8 *ReconPtr2, UINT16 *ReconRefPtr,
                    UINT32 ReconPixelsPerLine) {
    UINT32 i;

    for (i = 0; i < 8; i++) {
        ReconRefPtr[0] = (ReconPtr1[0] + ReconPtr2[0]) >> 1;
        ReconRefPtr[1] = (ReconPtr1[1] + ReconPtr2[1]) >> 1;
        ReconRefPtr[2] = (ReconPtr1[2] + ReconPtr2[2]) >> 1;
        ReconRefPtr[3] = (ReconPtr1[3] + ReconPtr2[3]) >> 1;
        ReconRefPtr[4] = (ReconPtr1[4] + ReconPtr2[4]) >> 1;
        ReconRefPtr[5] = (ReconPtr1[5] + ReconPtr2[5]) >> 1;
        ReconRefPtr[6] = (ReconPtr1[6] + ReconPtr2[6]) >> 1;
        ReconRefPtr[7] = (ReconPtr1[7] + ReconPtr2[7]) >> 1;

        ReconRefPtr += 8;
        ReconPtr1 += ReconPixelsPerLine;
        ReconPtr2 += ReconPixelsPerLine;
    }
}

void SubtractBlock_C(UINT8 *SrcBlock, INT16 *DestPtr, UINT32 LineStep) {
    UINT32 i;

    for (i = 0; i < 8; i++) {
        DestPtr[0] = SrcBlock[0] - DestPtr[0];
        DestPtr[1] = SrcBlock[1] - DestPtr[1];
        DestPtr[2] = SrcBlock[2] - DestPtr[2];
        DestPtr[3] = SrcBlock[3] - DestPtr[3];
        DestPtr[4] = SrcBlock[4] - DestPtr[4];
        DestPtr[5] = SrcBlock[5] - DestPtr[5];
        DestPtr[6] = SrcBlock[6] - DestPtr[6];
        DestPtr[7] = SrcBlock[7] - DestPtr[7];

        SrcBlock += LineStep;
        DestPtr += 8;
    }
}

void CopyBlock_C(UINT8 *src, UINT8 *dest, UINT32 srcstride) {
    int j;
    UINT8 *s = src;
    UINT8 *d = dest;
    UINT32 stride = srcstride;

    for (j = 0; j < 8; j++) {
        ((UINT32 *)d)[0] = ((UINT32 *)s)[0];
        ((UINT32 *)d)[1] = ((UINT32 *)s)[1];
        s += stride;
        d += stride;
    }
}

void Copy12x12_C(const UINT8 *src, UINT8 *dest, UINT32 srcstride, UINT32 deststride) {
    int j;
    const UINT8 *s = src;
    UINT8 *d = dest;

    for (j = 0; j < 12; j++) {
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        d[4] = s[4];
        d[5] = s[5];
        d[6] = s[6];
        d[7] = s[7];
        d[8] = s[8];
        d[9] = s[9];
        d[10] = s[10];
        d[11] = s[11];
        s += srcstride;
        d += deststride;
    }
}

void InitVPUtil(void) {
    fillidctconstants();
    UtilMachineSpecificConfig();
}

void FilterBlock1d(UINT8 *SrcPtr, UINT16 *OutputPtr, UINT32 SrcPixelsPerLine, UINT32 PixelStep,
                   UINT32 OutputHeight, UINT32 OutputWidth, INT32 *Filter) {
    UINT32 i;
    UINT32 j;
    INT32 Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((int)SrcPtr[-(int)PixelStep] * Filter[0]) + ((int)SrcPtr[0] * Filter[1]) +
                   ((int)SrcPtr[PixelStep] * Filter[2]) +
                   ((int)SrcPtr[2 * PixelStep] * Filter[3]) + (VP6_FILTER_WEIGHT >> 1);

            Temp = Temp >> VP6_FILTER_SHIFT;

            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 255) {
                Temp = 255;
            }

            OutputPtr[j] = Temp;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dFirstPass(UINT8 *SrcPtr, INT32 *OutputPtr, UINT32 SrcPixelsPerLine,
                            UINT32 PixelStep, UINT32 OutputHeight, UINT32 OutputWidth,
                            INT32 *Filter) {
    UINT32 i;
    UINT32 j;
    INT32 Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((int)SrcPtr[-(int)PixelStep] * Filter[0]) + ((int)SrcPtr[0] * Filter[1]) +
                   ((int)SrcPtr[PixelStep] * Filter[2]) +
                   ((int)SrcPtr[2 * PixelStep] * Filter[3]) + (VP6_FILTER_WEIGHT >> 1);

            Temp = Temp >> VP6_FILTER_SHIFT;

            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 255) {
                Temp = 255;
            }

            OutputPtr[j] = Temp;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dSecondPass(INT32 *SrcPtr, UINT16 *OutputPtr, UINT32 SrcPixelsPerLine,
                             UINT32 PixelStep, UINT32 OutputHeight, UINT32 OutputWidth,
                             INT32 *Filter) {
    UINT32 i;
    UINT32 j;
    INT32 Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = (SrcPtr[-(int)PixelStep] * Filter[0]) + (SrcPtr[0] * Filter[1]) +
                   (SrcPtr[PixelStep] * Filter[2]) + (SrcPtr[2 * PixelStep] * Filter[3]) +
                   (VP6_FILTER_WEIGHT >> 1);

            Temp = Temp >> VP6_FILTER_SHIFT;

            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 255) {
                Temp = 255;
            }

            OutputPtr[j] = Temp;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2d(UINT8 *SrcPtr, UINT16 *OutputPtr, UINT32 SrcPixelsPerLine, INT32 *HFilter,
                   INT32 *VFilter) {
    FilterBlock2dFirstPass(SrcPtr - SrcPixelsPerLine, FData, SrcPixelsPerLine, 1, 11, 8, HFilter);
    FilterBlock2dSecondPass(FData + 8, OutputPtr, 8, 8, 8, 8, VFilter);
}

void FilterBlock2dBil_FirstPass(UINT8 *SrcPtr, INT32 *OutputPtr, UINT32 SrcPixelsPerLine,
                                UINT32 PixelStep, UINT32 OutputHeight, UINT32 OutputWidth,
                                INT32 *Filter) {
    UINT32 i;
    UINT32 j;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            OutputPtr[j] = (((int)SrcPtr[0] * Filter[0]) + ((int)SrcPtr[PixelStep] * Filter[1]) +
                            (VP6_FILTER_WEIGHT >> 1)) >>
                           VP6_FILTER_SHIFT;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock1dBil_8(UINT8 *SrcPtr, UINT8 *OutputPtr, UINT32 SrcPixelsPerLine, UINT32 PixelStep,
                        UINT32 OutputHeight, UINT32 OutputWidth, INT32 *Filter) {
    UINT32 i;
    UINT32 j;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            OutputPtr[j] = (((int)SrcPtr[0] * Filter[0]) + ((int)SrcPtr[PixelStep] * Filter[1]) +
                            (VP6_FILTER_WEIGHT >> 1)) >>
                           VP6_FILTER_SHIFT;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dBil_SecondPass_8(INT32 *SrcPtr, UINT8 *OutputPtr, UINT32 SrcPixelsPerLine,
                                   UINT32 PixelStep, UINT32 OutputHeight, UINT32 OutputWidth,
                                   INT32 *Filter) {
    UINT32 i;
    UINT32 j;
    INT32 Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((SrcPtr[0] * Filter[0]) + (SrcPtr[PixelStep] * Filter[1]) +
                    (VP6_FILTER_WEIGHT >> 1)) >>
                   VP6_FILTER_SHIFT;
            OutputPtr[j] = Temp;
            SrcPtr++;
        }

        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dBil_8(UINT8 *SrcPtr, UINT8 *OutputPtr, UINT32 SrcPixelsPerLine, INT32 *HFilter,
                        INT32 *VFilter) {
    FilterBlock2dBil_FirstPass(SrcPtr, FData, SrcPixelsPerLine, 1, 9, 8, HFilter);
    FilterBlock2dBil_SecondPass_8(FData, OutputPtr, 8, 8, 8, 8, VFilter);
}

void FilterBlockBil_8_C(UINT8 *ReconPtr1, UINT8 *ReconPtr2, UINT8 *ReconRefPtr,
                        UINT32 PixelsPerLine, INT32 ModX, INT32 ModY) {
    INT32 diff = (INT32)(ReconPtr2 - ReconPtr1);

    if (diff < 0) {
        UINT8 *temp = ReconPtr1;
        ReconPtr1 = ReconPtr2;
        diff = (INT32)(temp - ReconPtr1);
    }

    if (diff == 1) {
        FilterBlock1dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8, BilinearFilters[ModX]);
    } else if (diff == (INT32)PixelsPerLine) {
        FilterBlock1dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine, 8, 8,
                           BilinearFilters[ModY]);
    } else if (diff == (INT32)PixelsPerLine - 1) {
        FilterBlock2dBil_8(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX],
                           BilinearFilters[ModY]);
    } else if (diff == (INT32)PixelsPerLine + 1) {
        FilterBlock2dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX],
                           BilinearFilters[ModY]);
    }
}
