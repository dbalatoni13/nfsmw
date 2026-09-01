extern void *memcpy(void *dest, const void *src, unsigned int num);

void CFastDeInterlace(unsigned char *SrcPtr, unsigned char *DstPtr, int Width, int Height, int Stride) {
    int i;
    int j;
    unsigned int x0;
    unsigned int x1;
    unsigned int x2;
    unsigned char *PrevSrcPtr;
    unsigned char *NextSrcPtr;
    unsigned char *CurrentSrcPtr;
    unsigned char *CurrentDstPtr;

    CurrentSrcPtr = SrcPtr;
    CurrentDstPtr = DstPtr;
    memcpy(CurrentDstPtr, CurrentSrcPtr, Width);

    for (i = 1; i < Height - 1; i++) {
        PrevSrcPtr = CurrentSrcPtr;
        CurrentSrcPtr += Stride;
        NextSrcPtr = CurrentSrcPtr + Stride;
        CurrentDstPtr += Stride;

        for (j = 0; j < Width; j++) {
            x1 = CurrentSrcPtr[j];
            x0 = PrevSrcPtr[j];
            x2 = NextSrcPtr[j];
            x1 <<= 1;
            x0 += x1;
            x0 += x2;
            x0 += 2;
            x0 >>= 2;
            CurrentDstPtr[j] = x0;
        }
    }

    memcpy(CurrentDstPtr + Stride, CurrentSrcPtr + Stride, Width);
}
