#define SAT(j)                                                        \
    {                                                                 \
        int value = NextSrc[j];                                       \
        if (value >= 0) {                                             \
            if (value > 255) {                                        \
                value = 255;                                          \
            } else {                                                  \
                value = ((unsigned char *)NextSrc)[2 * (j) + 1];      \
            }                                                         \
        } else {                                                      \
            value = 0;                                                \
        }                                                             \
        NextDest[j] = value;                                          \
    }

void SatUnsigned8(unsigned char *DestPtr, short *SourcePtr, int DestStep, int SourceStep) {
    unsigned int i;
    unsigned char *NextDest = DestPtr;
    short *NextSrc = SourcePtr;

    for (i = 0; i < 8; i++) {
        SAT(0)
        SAT(1)
        SAT(2)
        SAT(3)
        SAT(4)
        SAT(5)
        SAT(6)
        SAT(7)
        NextDest += DestStep;
        NextSrc += SourceStep;
    }
}

void ScalarReconInterHalfPixel2(short *TmpBuffer, unsigned char *Output,
                                unsigned char *Src1, unsigned char *Src2,
                                unsigned short *ChangePtr, int Stride) {
    unsigned int i;
    short *tmp = TmpBuffer;

    for (i = 0; i < 8; i++) {
        tmp[0] = ChangePtr[0] + ((Src1[0] + Src2[0]) >> 1);
        tmp[1] = ChangePtr[1] + ((Src1[1] + Src2[1]) >> 1);
        tmp[2] = ChangePtr[2] + ((Src1[2] + Src2[2]) >> 1);
        tmp[3] = ChangePtr[3] + ((Src1[3] + Src2[3]) >> 1);
        tmp[4] = ChangePtr[4] + ((Src1[4] + Src2[4]) >> 1);
        tmp[5] = ChangePtr[5] + ((Src1[5] + Src2[5]) >> 1);
        tmp[6] = ChangePtr[6] + ((Src1[6] + Src2[6]) >> 1);
        tmp[7] = ChangePtr[7] + ((Src1[7] + Src2[7]) >> 1);
        Src1 += Stride;
        Src2 += Stride;
        ChangePtr += 8;
        tmp += 8;
    }

    SatUnsigned8(Output, TmpBuffer, Stride, 8);
}
