
extern void *memcpy();

void CFastDeInterlace(unsigned char *SrcBuffer, unsigned char *DesBuffer,
                      int Width, int Height, int Stride) {
    unsigned char *PrevRow;
    unsigned char *CurRow;
    unsigned char *NextRow;
    unsigned char *DesPtr;
    int i, j;

    CurRow = SrcBuffer;
    DesPtr = DesBuffer;

    memcpy(DesBuffer, SrcBuffer, Width);

    for (i = 1; i < Height - 1; i++) {
        PrevRow = CurRow;
        CurRow += Stride;
        NextRow = CurRow + Stride;
        DesPtr += Stride;
        for (j = 0; j < Width; j++) {
            DesPtr[j] = ((unsigned int)(PrevRow[j] + (CurRow[j] << 1) + NextRow[j] + 2)) >> 2;
        }
    }

    memcpy(DesPtr + Stride, CurRow + Stride, Width);
}
