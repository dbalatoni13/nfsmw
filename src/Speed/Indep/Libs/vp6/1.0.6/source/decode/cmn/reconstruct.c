#define SAT_UNSIGNED8(offset) \
    do { \
        if (DataBlock[offset] < 0) { \
            ResultPtr[offset] = 0; \
        } else if (DataBlock[offset] > 255) { \
            ResultPtr[offset] = 255; \
        } else { \
            ResultPtr[offset] = (unsigned char)DataBlock[offset]; \
        } \
    } while (0)

void SatUnsigned8(unsigned char *ResultPtr, short *DataBlock,
                  unsigned int ResultLineStep, unsigned int DataLineStep) {
    int i;

    for (i = 0; i < 8; i++) {
        SAT_UNSIGNED8(0);
        SAT_UNSIGNED8(1);
        SAT_UNSIGNED8(2);
        SAT_UNSIGNED8(3);
        SAT_UNSIGNED8(4);
        SAT_UNSIGNED8(5);
        SAT_UNSIGNED8(6);
        SAT_UNSIGNED8(7);
        DataBlock += DataLineStep;
        ResultPtr += ResultLineStep;
    }
}

#undef SAT_UNSIGNED8

void ScalarReconInterHalfPixel2(short *TmpDataBuffer, unsigned char *ReconPtr,
                                unsigned char *RefPtr1, unsigned char *RefPtr2,
                                short *ChangePtr, unsigned int LineStep) {
    unsigned int i;
    short *TmpDataPtr;

    TmpDataPtr = TmpDataBuffer;
    i = 8;
    do {
        TmpDataPtr[0] = ChangePtr[0] + ((RefPtr1[0] + RefPtr2[0]) >> 1);
        TmpDataPtr[1] = ChangePtr[1] + ((RefPtr1[1] + RefPtr2[1]) >> 1);
        TmpDataPtr[2] = ChangePtr[2] + ((RefPtr1[2] + RefPtr2[2]) >> 1);
        TmpDataPtr[3] = ChangePtr[3] + ((RefPtr1[3] + RefPtr2[3]) >> 1);
        TmpDataPtr[4] = ChangePtr[4] + ((RefPtr1[4] + RefPtr2[4]) >> 1);
        TmpDataPtr[5] = ChangePtr[5] + ((RefPtr1[5] + RefPtr2[5]) >> 1);
        TmpDataPtr[6] = ChangePtr[6] + ((RefPtr1[6] + RefPtr2[6]) >> 1);
        TmpDataPtr[7] = ChangePtr[7] + ((RefPtr1[7] + RefPtr2[7]) >> 1);
        RefPtr1 += LineStep;
        RefPtr2 += LineStep;
        ChangePtr += 8;
        TmpDataPtr += 8;
    } while (--i);
    SatUnsigned8(ReconPtr, TmpDataBuffer, LineStep, 8);
}
