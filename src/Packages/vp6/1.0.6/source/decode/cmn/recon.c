typedef struct {
    unsigned char pad0[0x74];
    int CurrentReconStride;    /* 0x74 */
    unsigned char pad1[4];
    int MvShift;               /* 0x7c */
    unsigned char pad2[0x88 - 0x80];
    int FrameReconStride;      /* 0x88 */
} MBI_TYPE;

typedef struct {
    MBI_TYPE mbi;
    unsigned char pad3[0x13c - 0x8c];
    void **field_13c;
    unsigned char pad4[0x294 - 0x140];
    unsigned char *field_294;
} PB_INSTANCE;

typedef void (*Copy12x12_FUNC)(unsigned char *src, unsigned char *dest, unsigned int srcStride,
                               unsigned int destStep);
typedef void (*FilterFunc)(void *info, unsigned char *dest, int step);

extern Copy12x12_FUNC Copy12x12;
extern FilterFunc FilteringHoriz_12;
extern FilterFunc FilteringVert_12;

int Var16Point(unsigned char *Data, int Stride) {
    int i;
    int Sum = 0;
    int SumSq = 0;

    for (i = 0; i < 4; i++) {
        int a = Data[0];
        int b = Data[2];
        int c = Data[4];
        int d = Data[6];
        Sum += a;
        Sum += b;
        Sum += c;
        Sum += d;
        SumSq += a * a;
        SumSq += b * b;
        SumSq += c * c;
        SumSq += d * d;
        Data += Stride * 2;
    }

    return ((unsigned int)(16 * SumSq - Sum * Sum)) >> 8;
}

void VP6_PredictFiltered(PB_INSTANCE *pbi, unsigned char *SrcPtr, int mx, int my) {
    int mVx;
    int mVy;
    int ReconIndex;
    MBI_TYPE *mbi;
    unsigned char *TempBuffer;
    int BoundaryX;
    int BoundaryY;

    mbi = &pbi->mbi;
    TempBuffer = pbi->field_294;

    if (mx > 0) {
        mVx = mx >> mbi->MvShift;
    } else {
        mVx = -(-mx >> mbi->MvShift);
    }

    if (my > 0) {
        mVy = my >> mbi->MvShift;
    } else {
        mVy = -(-my >> mbi->MvShift);
    }

    ReconIndex = mbi->FrameReconStride * mVy + mVx - 2 * mbi->CurrentReconStride - 2;

    Copy12x12(SrcPtr + ReconIndex, TempBuffer, mbi->CurrentReconStride, 16);

    BoundaryX = (8 - (mVx & 7)) & 7;
    BoundaryY = (8 - (mVy & 7)) & 7;

    if (BoundaryX != 0) {
        FilteringHoriz_12(*pbi->field_13c, TempBuffer + (BoundaryX + 2), 16);
    }

    if (BoundaryY != 0) {
        FilteringVert_12(*pbi->field_13c, TempBuffer + (BoundaryY * 16 + 32), 16);
    }
}
