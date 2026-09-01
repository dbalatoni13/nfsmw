unsigned int Var16Point(unsigned char *DataPtr, int SourceStride) {
    unsigned int i;
    unsigned int XSum = 0;
    unsigned int XXSum = 0;
    unsigned char *DiffPtr;

    DiffPtr = DataPtr;

    for (i = 4; i; i--) {
        XSum += DiffPtr[0];
        XXSum += DiffPtr[0] * DiffPtr[0];
        XSum += DiffPtr[2];
        XXSum += DiffPtr[2] * DiffPtr[2];
        XSum += DiffPtr[4];
        XXSum += DiffPtr[4] * DiffPtr[4];
        XSum += DiffPtr[6];
        XXSum += DiffPtr[6] * DiffPtr[6];

        DiffPtr += SourceStride * 2;
    }

    return ((XXSum << 4) - XSum * XSum) >> 8;
}
