
typedef struct {
    unsigned char pad0[120];
    int MiddleStart;      /* +120 */
    unsigned char pad1[20];
    int RowLength;        /* +144, bound = *8 */
    int RowCount;         /* +148, bound = *8 */
    int PlaneLineStep;    /* +152 */
} PP_INST;

void ClampLevels_C(PP_INST *pbi, int BlackClamp, int WhiteClamp, unsigned char *Src,
                   unsigned char *Dst) {
    int i;
    int row;
    int col;
    unsigned char clamped[256];
    int width;
    int height;
    unsigned char *DestPtr;
    unsigned char *SrcPtr;
    unsigned int LineLength;

    width = pbi->RowLength * 8;
    height = pbi->RowCount * 8;
    LineLength = pbi->PlaneLineStep;

    SrcPtr = Src + pbi->MiddleStart;
    DestPtr = Dst + pbi->MiddleStart;

    for (i = 0; i < 256; i++) {
        clamped[i] = i;
        if (i < BlackClamp) {
            clamped[i] = BlackClamp;
        }
        if (i > 255 - WhiteClamp) {
            clamped[i] = ~WhiteClamp;
        }
    }

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            SrcPtr[col] = clamped[DestPtr[col]];
        }
        SrcPtr += LineLength;
        DestPtr += LineLength;
    }
}
