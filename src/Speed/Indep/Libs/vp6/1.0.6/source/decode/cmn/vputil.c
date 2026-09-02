extern void fillidctconstants(void);
extern void UtilMachineSpecificConfig(void);

static int BilinearFilters[16] = {
    0x80000000, 0, 0x70000000, 0x10000000,
    0x60000000, 0x20000000, 0x50000000, 0x30000000,
    0x40000000, 0x40000000, 0x30000000, 0x50000000,
    0x20000000, 0x60000000, 0x10000000, 0x70000000
};
static int FData[0x160 / 4];
void (*idct[65])(short *, short *, short *);
void (*idctc[65])(short *, short *, short *);

void InitVPUtil(void) {
    fillidctconstants();
    UtilMachineSpecificConfig();
}

void CopyBlock_C(unsigned char *src, unsigned char *dest, unsigned int srcstride) {
    int j;
    unsigned char *s = src;
    unsigned char *d = dest;
    unsigned int stride = srcstride;

    for (j = 8; j; --j) {
        *(unsigned int *)d = *(unsigned int *)s;
        *(unsigned int *)(d + 4) = *(unsigned int *)(s + 4);
        s += stride;
        d += stride;
    }
}

void Copy12x12_C(const unsigned char *src, unsigned char *dest, unsigned int srcstride,
                 unsigned int deststride) {
    int j;
    const unsigned char *s = src;
    unsigned char *d = dest;

    for (j = 12; j; --j) {
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

void AverageBlock_C(unsigned char *ReconPtr1, unsigned char *ReconPtr2,
                    unsigned short *ReconRefPtr, unsigned int ReconPixelsPerLine) {
    unsigned int i;

    for (i = 8; i; --i) {
        ReconRefPtr[0] = (ReconPtr1[0] + ReconPtr2[0]) >> 1;
        ReconRefPtr[1] = (ReconPtr1[1] + ReconPtr2[1]) >> 1;
        ReconRefPtr[2] = (ReconPtr1[2] + ReconPtr2[2]) >> 1;
        ReconRefPtr[3] = (ReconPtr1[3] + ReconPtr2[3]) >> 1;
        ReconRefPtr[4] = (ReconPtr1[4] + ReconPtr2[4]) >> 1;
        ReconRefPtr[5] = (ReconPtr1[5] + ReconPtr2[5]) >> 1;
        ReconRefPtr[6] = (ReconPtr1[6] + ReconPtr2[6]) >> 1;
        ReconRefPtr[7] = (ReconPtr1[7] + ReconPtr2[7]) >> 1;
        ReconPtr1 += ReconPixelsPerLine;
        ReconPtr2 += ReconPixelsPerLine;
        ReconRefPtr += 8;
    }
}

void SubtractBlock_C(unsigned char *SrcBlock, short *DestPtr, unsigned int LineStep) {
    unsigned int i;

    for (i = 8; i; --i) {
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

void FilterBlock1d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                   unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                   unsigned int OutputHeight, unsigned int OutputWidth, int *Filter) {
    unsigned int i;
    unsigned int j;
    int Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((int)SrcPtr[-(int)PixelStep] * Filter[0]) +
                   ((int)SrcPtr[0] * Filter[1]) +
                   ((int)SrcPtr[PixelStep] * Filter[2]) +
                   ((int)SrcPtr[2 * PixelStep] * Filter[3]) +
                   (128 >> 1);
            Temp = Temp >> 7;
            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 0xff) {
                Temp = 0xff;
            }
            OutputPtr[j] = (unsigned short)Temp;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dFirstPass(unsigned char *SrcPtr, int *OutputPtr,
                            unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                            unsigned int OutputHeight, unsigned int OutputWidth, int *Filter) {
    unsigned int i;
    unsigned int j;
    int Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((int)SrcPtr[-(int)PixelStep] * Filter[0]) +
                   ((int)SrcPtr[0] * Filter[1]) +
                   ((int)SrcPtr[PixelStep] * Filter[2]) +
                   ((int)SrcPtr[2 * PixelStep] * Filter[3]) +
                   (128 >> 1);
            Temp = Temp >> 7;
            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 0xff) {
                Temp = 0xff;
            }
            OutputPtr[j] = Temp;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dSecondPass(int *SrcPtr, unsigned short *OutputPtr,
                             unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                             unsigned int OutputHeight, unsigned int OutputWidth, int *Filter) {
    unsigned int i;
    unsigned int j;
    int Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = ((int)SrcPtr[-(int)PixelStep] * Filter[0]) +
                   ((int)SrcPtr[0] * Filter[1]) +
                   ((int)SrcPtr[PixelStep] * Filter[2]) +
                   ((int)SrcPtr[2 * PixelStep] * Filter[3]) +
                   (128 >> 1);
            Temp = Temp >> 7;
            if (Temp < 0) {
                Temp = 0;
            } else if (Temp > 0xff) {
                Temp = 0xff;
            }
            OutputPtr[j] = (unsigned short)Temp;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2d(unsigned char *SrcPtr, unsigned short *OutputPtr,
                   unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter) {
    FilterBlock2dFirstPass(SrcPtr - SrcPixelsPerLine, FData, SrcPixelsPerLine,
                           1, 11, 8, HFilter);
    FilterBlock2dSecondPass(FData + 8, OutputPtr, 8, 8, 8, 8, VFilter);
}

void FilterBlock2dBil_FirstPass(unsigned char *SrcPtr, int *OutputPtr,
                                unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                                unsigned int OutputHeight, unsigned int OutputWidth,
                                int *Filter) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            OutputPtr[j] = (((int)SrcPtr[0] * Filter[0]) +
                            ((int)SrcPtr[PixelStep] * Filter[1]) +
                            (128 >> 1)) >> 7;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock1dBil_8(unsigned char *SrcPtr, unsigned char *OutputPtr,
                        unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                        unsigned int OutputHeight, unsigned int OutputWidth, int *Filter) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            OutputPtr[j] = (SrcPtr[0] * Filter[0] + SrcPtr[PixelStep] * Filter[1] + 0x40) >> 7;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dBil_SecondPass_8(int *SrcPtr, unsigned char *OutputPtr,
                                   unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                                   unsigned int OutputHeight, unsigned int OutputWidth,
                                   int *Filter) {
    unsigned int i;
    unsigned int j;
    int Temp;

    for (i = 0; i < OutputHeight; i++) {
        for (j = 0; j < OutputWidth; j++) {
            Temp = (SrcPtr[0] * Filter[0] + SrcPtr[PixelStep] * Filter[1] + 0x40) >> 7;
            OutputPtr[j] = Temp;
            SrcPtr++;
        }
        SrcPtr += SrcPixelsPerLine - OutputWidth;
        OutputPtr += OutputWidth;
    }
}

void FilterBlock2dBil_8(unsigned char *SrcPtr, unsigned char *OutputPtr,
                        unsigned int SrcPixelsPerLine, int *HFilter, int *VFilter) {
    FilterBlock2dBil_FirstPass(SrcPtr, FData, SrcPixelsPerLine, 1, 9, 8, HFilter);
    FilterBlock2dBil_SecondPass_8(FData, OutputPtr, 8, 8, 8, 8, VFilter);
}

void FilterBlockBil_8_C(unsigned char *ReconPtr1, unsigned char *ReconPtr2,
                        unsigned char *ReconRefPtr, unsigned int PixelsPerLine,
                        int ModX, int ModY) {
    int diff;

    diff = ReconPtr2 - ReconPtr1;
    if (diff < 0) {
        unsigned char *temp = ReconPtr1;
        ReconPtr1 = ReconPtr2;
        ReconPtr2 = temp;
        diff = ReconPtr2 - ReconPtr1;
    }

    if (diff == 1) {
        FilterBlock1dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8,
                           &BilinearFilters[ModX * 2]);
    } else if (diff == (int)PixelsPerLine) {
        FilterBlock1dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine, 8, 8,
                           &BilinearFilters[ModY * 2]);
    } else if (diff == (int)PixelsPerLine - 1) {
        FilterBlock2dBil_8(ReconPtr1 - 1, ReconRefPtr, PixelsPerLine,
                           &BilinearFilters[ModX * 2], &BilinearFilters[ModY * 2]);
    } else if (diff == (int)PixelsPerLine + 1) {
        FilterBlock2dBil_8(ReconPtr1, ReconRefPtr, PixelsPerLine,
                           &BilinearFilters[ModX * 2], &BilinearFilters[ModY * 2]);
    }
}

void ClearSysState_C(void) {}
