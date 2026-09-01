struct CONFIG_TYPE {
    unsigned int VideoFrameWidth;
    unsigned int VideoFrameHeight;
    int YStride;
    int UVStride;
    unsigned int HFragPixels;
    unsigned int VFragPixels;
    unsigned int HScale;
    unsigned int HRatio;
    unsigned int VScale;
    unsigned int VRatio;
    unsigned int ScalingMode;
    unsigned int Interlaced;
    unsigned int ExpandedFrameWidth;
    unsigned int ExpandedFrameHeight;
};

typedef struct {
    int Vp3VersionNo;
    int FrameType;
    int PostProcessingLevel;
    int FrameQIndex;
    unsigned char *LastFrameRecon;
    unsigned char *PostProcessBuffer;
    unsigned char *FragInfo;
    unsigned int FragInfoElementSize;
    unsigned int FragInfoCodedMask;
    int *FragQIndex;
    int *FragmentVariances;
    unsigned char *FragDeblockingFlag;
    int *BoundingValuePtr;
    int *FiltBoundingValue;
    int *DeblockValuePtr;
    int *DeblockBoundingValue;
    struct CONFIG_TYPE Configuration;
    unsigned int ReconYDataOffset;
    unsigned int ReconUDataOffset;
    unsigned int ReconVDataOffset;
    unsigned int YPlaneFragments;
    unsigned int UVPlaneFragments;
    unsigned int UnitFragments;
    unsigned int HFragments;
    unsigned int VFragments;
    int YStride;
    int UVStride;
    int *FiltBoundingValueAlloc;
    int *DeblockBoundingValueAlloc;
    int *FragQIndexAlloc;
    int *FragmentVariancesAlloc;
    unsigned char *FragDeblockingFlagAlloc;
    unsigned int MVBorder;
    unsigned char *IntermediateBufferAlloc;
    unsigned char *IntermediateBuffer;
    unsigned int DeInterlaceMode;
    unsigned int AddNoiseMode;
} POSTPROC_INSTANCE;

unsigned short FData[72] __asm__("FData.84_804BE56C");
unsigned int DeringModifierV1[64];
int SharpenModifier[64] = {
    -12, -11, -10, -10,
    -9, -9, -9, -9,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
    -4, -4, -4, -4,
    -4, -4, -4, -4,
    -2, -2, -2, -2,
    -2, -2, -2, -2,
    -2, -2, -2, -2,
    -2, -2, -2, -2,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

unsigned int DeringModifierV3[64] = {
    9, 9, 9, 9,
    8, 8, 8, 8,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    7, 7, 7, 7,
    6, 6, 5, 5,
    5, 5, 5, 5,
    4, 4, 4, 4,
    3, 3, 3, 3,
    2, 2, 2, 0,
    0, 0, 0, 0
};
unsigned int DeringModifierV2[64] = {
    9, 9, 8, 8,
    7, 7, 7, 7,
    6, 6, 6, 6,
    6, 6, 6, 6,
    6, 6, 6, 6,
    6, 6, 6, 6,
    5, 5, 5, 5,
    5, 5, 5, 5,
    5, 5, 5, 5,
    5, 5, 5, 5,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    3, 3, 3, 3,
    2, 2, 2, 2
};
#define DERING_STRONG_PIXEL(index)                                             \
    do {                                                                        \
        al = LRMod[k * 9 + index];                                              \
        ar = LRMod[k * 9 + index + 1];                                          \
        au = UDMod[k * 8 + index];                                              \
        ad = UDMod[(k + 1) * 8 + index];                                        \
        pl = Src[index - 1];                                                    \
        pr = Src[index + 1];                                                    \
        pu = lastRow[rowOffset + index];                                        \
        pd = nextRow[rowOffset + index];                                        \
        p = Src[index];                                                          \
        atot = 128 - al - ar - au - ad;                                         \
        newVal = (atot * p + al * pl + ar * pr + au * pu + ad * pd + round) >> 7; \
        if (newVal < Low) {                                                      \
            newPixel[index] = Low;                                              \
        } else {                                                                 \
            if (newVal > High) {                                                 \
                newVal = High;                                                   \
            }                                                                    \
            newPixel[index] = newVal;                                           \
        }                                                                        \
    } while (0)

void DeringBlockStrong_C(POSTPROC_INSTANCE *pbi, const unsigned char *SrcPtr,
                         unsigned char *DstPtr, const int Pitch,
                         unsigned int FragQIndex, unsigned int *QuantScale) {
    int B;
    int al;
    int ar;
    int au;
    int ad;
    int atot;
    int High;
    int Low;
    int TmpMod;
    int newVal;
    short UDMod[72];
    short LRMod[72];
    unsigned int j;
    unsigned int k;
    unsigned char p;
    unsigned char pl;
    unsigned char pr;
    unsigned char pu;
    unsigned char pd;
    unsigned int rowOffset;
    unsigned int round;
    unsigned int QValue;
    int Sharpen;
    const unsigned char *Src;
    const unsigned char *curRow;
    const unsigned char *lastRow;
    const unsigned char *nextRow;
    unsigned char *dstRow;

    QValue = QuantScale[FragQIndex];
    Sharpen = SharpenModifier[FragQIndex];
    High = 255;
    Low = 0;
    round = 64;
    B = QValue + QValue + QValue;
    if (B > 32) {
        B = 32;
    }

    Src = SrcPtr;
    for (k = 0; k <= 8; k++) {
        curRow = Src;
        for (j = 0; j < 8; j++) {
            TmpMod = curRow[j] - curRow[j - Pitch];
            if (TmpMod < 0) {
                TmpMod = -TmpMod;
            }
            TmpMod -= 32;
            TmpMod = QValue - TmpMod;
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < 0) {
                TmpMod = 0;
            } else if (TmpMod > B) {
                TmpMod = B;
            }
            UDMod[k * 8 + j] = TmpMod;
        }
        Src += Pitch;
    }

    Src = SrcPtr;
    for (k = 0; k <= 7; k++) {
        curRow = Src;
        for (j = 0; j <= 8; j++) {
            TmpMod = curRow[j] - curRow[j - 1];
            if (TmpMod < 0) {
                TmpMod = -TmpMod;
            }
            TmpMod -= 32;
            TmpMod = QValue - TmpMod;
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < 0) {
                TmpMod = 0;
            } else if (TmpMod > B) {
                TmpMod = B;
            }
            LRMod[k * 9 + j] = TmpMod;
        }
        Src += Pitch;
    }

    {
        int newPixel[8];

        rowOffset = 0;
        lastRow = SrcPtr - Pitch;
        nextRow = SrcPtr + Pitch;
        for (k = 0; k < 8; k++) {
            Src = SrcPtr + rowOffset;
            dstRow = DstPtr + rowOffset;
            DERING_STRONG_PIXEL(0);
            DERING_STRONG_PIXEL(1);
            DERING_STRONG_PIXEL(2);
            DERING_STRONG_PIXEL(3);
            DERING_STRONG_PIXEL(4);
            DERING_STRONG_PIXEL(5);
            DERING_STRONG_PIXEL(6);
            DERING_STRONG_PIXEL(7);
            dstRow[0] = newPixel[0];
            dstRow[1] = newPixel[1];
            dstRow[2] = newPixel[2];
            dstRow[3] = newPixel[3];
            dstRow[4] = newPixel[4];
            dstRow[5] = newPixel[5];
            dstRow[6] = newPixel[6];
            dstRow[7] = newPixel[7];
            rowOffset += Pitch;
        }
    }
}

#undef DERING_STRONG_PIXEL
void DeringBlockWeak_C(POSTPROC_INSTANCE *pbi, const unsigned char *SrcPtr,
                       unsigned char *DstPtr, const int Pitch,
                       unsigned int FragQIndex, unsigned int *QuantScale) {
    int B;
    int al;
    int ar;
    int au;
    int ad;
    int atot;
    int High;
    int Low;
    int newVal;
    int TmpMod;
    short UDMod[72];
    short LRMod[72];
    unsigned int j;
    unsigned int k;
    unsigned char p;
    unsigned char pl;
    unsigned char pr;
    unsigned char pu;
    unsigned char pd;
    unsigned int rowOffset;
    unsigned int round;
    unsigned int QValue;
    int Sharpen;
    const unsigned char *Src;
    const unsigned char *curRow;
    const unsigned char *lastRow;
    const unsigned char *nextRow;
    unsigned char *dstRow;

    QValue = QuantScale[FragQIndex];
    Sharpen = SharpenModifier[FragQIndex];
    High = 255;
    Low = 0;
    round = 64;
    B = QValue + QValue + QValue;
    if (B > 24) {
        B = 24;
    }

    Src = SrcPtr;
    for (k = 0; k <= 8; k++) {
        curRow = Src;
        for (j = 0; j < 8; j++) {
            TmpMod = curRow[j] - curRow[j - Pitch];
            if (TmpMod < 0) {
                TmpMod = -TmpMod;
            }
            TmpMod = (TmpMod << 1) - 32;
            TmpMod = QValue - TmpMod;
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < 0) {
                TmpMod = 0;
            } else if (TmpMod > B) {
                TmpMod = B;
            }
            UDMod[k * 8 + j] = TmpMod;
        }
        Src += Pitch;
    }

    Src = SrcPtr;
    for (k = 0; k <= 7; k++) {
        curRow = Src;
        for (j = 0; j <= 8; j++) {
            TmpMod = curRow[j] - curRow[j - 1];
            if (TmpMod < 0) {
                TmpMod = -TmpMod;
            }
            TmpMod = (TmpMod << 1) - 32;
            TmpMod = QValue - TmpMod;
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < 0) {
                TmpMod = 0;
            } else if (TmpMod > B) {
                TmpMod = B;
            }
            LRMod[k * 9 + j] = TmpMod;
        }
        Src += Pitch;
    }

    rowOffset = 0;
    lastRow = SrcPtr - Pitch;
    nextRow = SrcPtr + Pitch;
    for (k = 0; k < 8; k++) {
        Src = SrcPtr + rowOffset;
        dstRow = DstPtr + rowOffset;
        for (j = 0; j < 8; j++) {
            al = LRMod[k * 9 + j];
            ar = LRMod[k * 9 + j + 1];
            au = UDMod[k * 8 + j];
            ad = UDMod[(k + 1) * 8 + j];
            pl = Src[j - 1];
            pr = Src[j + 1];
            pu = lastRow[rowOffset + j];
            pd = nextRow[rowOffset + j];
            p = Src[j];
            atot = 128 - al - ar - au - ad;
            newVal = (atot * p + al * pl + ar * pr + au * pu + ad * pd + round) >> 7;
            if (newVal < Low) {
                dstRow[j] = Low;
            } else {
                if (newVal > High) {
                    newVal = High;
                }
                dstRow[j] = newVal;
            }
        }
        rowOffset += Pitch;
    }
}
typedef void (*DERING_BLOCK_FUNCTION)(POSTPROC_INSTANCE *, const unsigned char *,
                                      unsigned char *, const int, unsigned int,
                                      unsigned int *);
typedef void (*COPY_BLOCK_FUNCTION)(unsigned char *, unsigned char *, unsigned int);

extern DERING_BLOCK_FUNCTION DeringBlockStrong;
extern DERING_BLOCK_FUNCTION DeringBlockWeak;
extern COPY_BLOCK_FUNCTION CopyBlock;

void DeringFrame(POSTPROC_INSTANCE *pbi, unsigned char *Src, unsigned char *Dst) {
    unsigned int Block;
    unsigned int col;
    unsigned int row;
    unsigned int BlocksAcross;
    unsigned int BlocksDown;
    unsigned int *QuantScale;
    unsigned int LineLength;
    int Thresh1;
    int Thresh2;
    int Thresh3;
    int Thresh4;
    unsigned char *SrcPtr;
    unsigned char *DestPtr;
    int Quality;

    Quality = pbi->FrameQIndex;
    if (pbi->Vp3VersionNo > 4) {
        Thresh1 = 0x180;
        Thresh2 = 0x900;
        Thresh3 = 0xB40;
        Thresh4 = 0x1680;
    } else {
        Thresh1 = 0x800;
        Thresh2 = 0x7800;
        Thresh3 = 0x16800;
        Thresh4 = 0x1E000;
    }

    if (pbi->Vp3VersionNo > 4) {
        QuantScale = DeringModifierV3;
    } else if (pbi->Vp3VersionNo > 1) {
        QuantScale = DeringModifierV2;
    } else {
        QuantScale = DeringModifierV1;
    }

    BlocksAcross = pbi->HFragments;
    BlocksDown = pbi->VFragments;
    LineLength = pbi->YStride;
    SrcPtr = Src + pbi->ReconYDataOffset;
    DestPtr = Dst + pbi->ReconYDataOffset;
    Block = 0;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                Variance = pbi->FragmentVariances[Block];
                if (pbi->PostProcessingLevel > 5 && Variance > Thresh3) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                    if ((col != 0 && pbi->FragmentVariances[Block - 1] > Thresh4) ||
                        (col + 1 < BlocksAcross &&
                         pbi->FragmentVariances[Block + 1] > Thresh4) ||
                        (row + 1 < BlocksDown &&
                         pbi->FragmentVariances[Block + BlocksAcross] > Thresh4) ||
                        (row != 0 &&
                         pbi->FragmentVariances[Block - BlocksAcross] > Thresh4)) {
                        DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                          DestPtr + (col << 3), LineLength, Quality,
                                          QuantScale);
                        DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                          DestPtr + (col << 3), LineLength, Quality,
                                          QuantScale);
                    }
                } else if (Variance > Thresh2) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                } else if (Variance > Thresh1) {
                    DeringBlockWeak(pbi, SrcPtr + (col << 3),
                                    DestPtr + (col << 3), LineLength, Quality,
                                    QuantScale);
                } else {
                    CopyBlock(SrcPtr + (col << 3), DestPtr + (col << 3), LineLength);
                }
                Block++;
                col++;
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }

    BlocksAcross >>= 1;
    BlocksDown >>= 1;
    LineLength = pbi->UVStride;
    SrcPtr = Src + pbi->ReconUDataOffset;
    DestPtr = Dst + pbi->ReconUDataOffset;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                Variance = pbi->FragmentVariances[Block];
                if (pbi->Vp3VersionNo <= 4) {
                    Quality = pbi->FragQIndex[Block];
                }
                if (pbi->PostProcessingLevel > 5 && Variance > Thresh4) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                } else if (Variance > Thresh2) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                } else if (Variance > Thresh1) {
                    DeringBlockWeak(pbi, SrcPtr + (col << 3),
                                    DestPtr + (col << 3), LineLength, Quality,
                                    QuantScale);
                } else {
                    CopyBlock(SrcPtr + (col << 3), DestPtr + (col << 3), LineLength);
                }
                Block++;
                col++;
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }

    SrcPtr = Src + pbi->ReconVDataOffset;
    DestPtr = Dst + pbi->ReconVDataOffset;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                Variance = pbi->FragmentVariances[Block];
                if (pbi->Vp3VersionNo <= 4) {
                    Quality = pbi->FragQIndex[Block];
                }
                if (pbi->PostProcessingLevel > 5 && Variance > Thresh4) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                } else if (Variance > Thresh2) {
                    DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                      DestPtr + (col << 3), LineLength, Quality,
                                      QuantScale);
                } else if (Variance > Thresh1) {
                    DeringBlockWeak(pbi, SrcPtr + (col << 3),
                                    DestPtr + (col << 3), LineLength, Quality,
                                    QuantScale);
                } else {
                    CopyBlock(SrcPtr + (col << 3), DestPtr + (col << 3), LineLength);
                }
                Block++;
                col++;
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }
}

#define DERING_PROCESS_Y_BLOCK()                                               \
    do {                                                                       \
        Variance = pbi->FragmentVariances[Block];                              \
        if (pbi->PostProcessingLevel > 5 && Variance > Thresh3) {              \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
            if ((col != 0 &&                                                   \
                 pbi->FragmentVariances[Block - 1] > Thresh4) ||              \
                (col + 1 < BlocksAcross &&                                    \
                 pbi->FragmentVariances[Block + 1] > Thresh4) ||               \
                (row + 1 < BlocksDown &&                                       \
                 pbi->FragmentVariances[Block + BlocksAcross] > Thresh4) ||    \
                (row != 0 &&                                                    \
                 pbi->FragmentVariances[Block - BlocksAcross] > Thresh4)) {    \
                DeringBlockStrong(pbi, SrcPtr + (col << 3),                    \
                                  DestPtr + (col << 3), LineLength, Quality,   \
                                  QuantScale);                                  \
                DeringBlockStrong(pbi, SrcPtr + (col << 3),                    \
                                  DestPtr + (col << 3), LineLength, Quality,   \
                                  QuantScale);                                  \
            }                                                                   \
        } else if (Variance > Thresh2) {                                        \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
        } else if (Variance > Thresh1) {                                        \
            DeringBlockWeak(pbi, SrcPtr + (col << 3),                          \
                            DestPtr + (col << 3), LineLength, Quality,         \
                            QuantScale);                                       \
        } else {                                                                \
            CopyBlock(SrcPtr + (col << 3), DestPtr + (col << 3), LineLength);  \
        }                                                                       \
        Block++;                                                                \
        col++;                                                                  \
    } while (0)

#define DERING_PROCESS_CHROMA_BLOCK()                                          \
    do {                                                                       \
        Variance = pbi->FragmentVariances[Block];                              \
        if (pbi->Vp3VersionNo <= 4) {                                          \
            Quality = pbi->FragQIndex[Block];                                  \
        }                                                                      \
        if (pbi->PostProcessingLevel > 5 && Variance > Thresh4) {              \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
        } else if (Variance > Thresh2) {                                        \
            DeringBlockStrong(pbi, SrcPtr + (col << 3),                        \
                              DestPtr + (col << 3), LineLength, Quality,       \
                              QuantScale);                                     \
        } else if (Variance > Thresh1) {                                        \
            DeringBlockWeak(pbi, SrcPtr + (col << 3),                          \
                            DestPtr + (col << 3), LineLength, Quality,         \
                            QuantScale);                                       \
        } else {                                                                \
            CopyBlock(SrcPtr + (col << 3), DestPtr + (col << 3), LineLength);  \
        }                                                                       \
        Block++;                                                                \
        col++;                                                                  \
    } while (0)

void DeringFrameInterlaced(POSTPROC_INSTANCE *pbi, unsigned char *Src,
                           unsigned char *Dst) {
    unsigned int Block;
    unsigned int col;
    unsigned int row;
    unsigned int BlocksAcross;
    unsigned int BlocksDown;
    unsigned int *QuantScale;
    unsigned int LineLength;
    int Thresh1;
    int Thresh2;
    int Thresh3;
    int Thresh4;
    unsigned char *SrcPtr;
    unsigned char *DestPtr;
    int Quality;

    Quality = pbi->FrameQIndex;
    if (pbi->Vp3VersionNo > 4) {
        Thresh1 = 0x180;
        Thresh2 = 0x900;
        Thresh3 = 0xB40;
        Thresh4 = 0x1680;
    } else {
        Thresh1 = 0x800;
        Thresh2 = 0x7800;
        Thresh3 = 0x16800;
        Thresh4 = 0x1E000;
    }

    if (pbi->Vp3VersionNo > 4) {
        QuantScale = DeringModifierV3;
    } else if (pbi->Vp3VersionNo > 1) {
        QuantScale = DeringModifierV2;
    } else {
        QuantScale = DeringModifierV1;
    }

    BlocksAcross = pbi->HFragments;
    BlocksDown = pbi->VFragments >> 1;
    LineLength = pbi->YStride << 1;
    SrcPtr = Src + pbi->ReconYDataOffset;
    DestPtr = Dst + pbi->ReconYDataOffset;
    Block = 0;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                DERING_PROCESS_Y_BLOCK();
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }

    SrcPtr = Src + pbi->ReconYDataOffset + pbi->YStride;
    DestPtr = Dst + pbi->ReconYDataOffset + pbi->YStride;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                DERING_PROCESS_Y_BLOCK();
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }

    BlocksAcross >>= 1;
    LineLength = pbi->UVStride;
    SrcPtr = Src + pbi->ReconUDataOffset;
    DestPtr = Dst + pbi->ReconUDataOffset;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                DERING_PROCESS_CHROMA_BLOCK();
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }

    SrcPtr = Src + pbi->ReconVDataOffset;
    DestPtr = Dst + pbi->ReconVDataOffset;
    row = 0;
    {
        int Variance;

        while (row < BlocksDown) {
            col = 0;
            while (col < BlocksAcross) {
                DERING_PROCESS_CHROMA_BLOCK();
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
            row++;
        }
    }
}

#undef DERING_PROCESS_Y_BLOCK
#undef DERING_PROCESS_CHROMA_BLOCK
