#include <stdlib.h>

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

void DeringBlockStrong_C(const POSTPROC_INSTANCE *pbi, const unsigned char *SrcPtr,
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
    unsigned int rowOffset = 0;
    unsigned int round = 64;
    unsigned int QValue = QuantScale[FragQIndex];
    int Sharpen = SharpenModifier[FragQIndex];
    const unsigned char *Src = SrcPtr;
    const unsigned char *curRow;
    const unsigned char *lastRow = SrcPtr - Pitch;
    const unsigned char *nextRow = SrcPtr + Pitch;
    unsigned char *dstRow;

    Low = 0;
    High = QValue * 3;
    if (High > 32) {
        High = 32;
    }

    for (k = 0; k <= 8; k++) {
        curRow = Src;
        for (j = 0; j < 8; j++) {
            TmpMod = 32 + QValue - (abs(curRow[j] - curRow[j - Pitch]));
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < Low) {
                TmpMod = Low;
            } else if (TmpMod > High) {
                TmpMod = High;
            }
            UDMod[k * 8 + j] = TmpMod;
        }
        Src += Pitch;
    }

    Src = SrcPtr;
    for (k = 0; k <= 7; k++) {
        curRow = Src;
        for (j = 0; j <= 8; j++) {
            TmpMod = 32 + QValue - (abs(curRow[j] - curRow[j - 1]));
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < Low) {
                TmpMod = Low;
            } else if (TmpMod > High) {
                TmpMod = High;
            }
            LRMod[k * 9 + j] = TmpMod;
        }
        Src += Pitch;
    }

    {
        int newPixel[8];

        for (k = 0; k < 8; k++) {
            p = SrcPtr[rowOffset + 0];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 0 - 1];
            al = LRMod[k * 9 + 0];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 0];
            au = UDMod[k * 8 + 0];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 0];
            ad = UDMod[(k + 1) * 8 + 0];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 0 + 1];
            ar = LRMod[k * 9 + 0 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[0] = newVal;
            } else {
                newPixel[0] = 0;
            }
            p = SrcPtr[rowOffset + 1];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 1 - 1];
            al = LRMod[k * 9 + 1];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 1];
            au = UDMod[k * 8 + 1];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 1];
            ad = UDMod[(k + 1) * 8 + 1];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 1 + 1];
            ar = LRMod[k * 9 + 1 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[1] = newVal;
            } else {
                newPixel[1] = 0;
            }
            p = SrcPtr[rowOffset + 2];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 2 - 1];
            al = LRMod[k * 9 + 2];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 2];
            au = UDMod[k * 8 + 2];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 2];
            ad = UDMod[(k + 1) * 8 + 2];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 2 + 1];
            ar = LRMod[k * 9 + 2 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[2] = newVal;
            } else {
                newPixel[2] = 0;
            }
            p = SrcPtr[rowOffset + 3];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 3 - 1];
            al = LRMod[k * 9 + 3];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 3];
            au = UDMod[k * 8 + 3];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 3];
            ad = UDMod[(k + 1) * 8 + 3];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 3 + 1];
            ar = LRMod[k * 9 + 3 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[3] = newVal;
            } else {
                newPixel[3] = 0;
            }
            p = SrcPtr[rowOffset + 4];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 4 - 1];
            al = LRMod[k * 9 + 4];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 4];
            au = UDMod[k * 8 + 4];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 4];
            ad = UDMod[(k + 1) * 8 + 4];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 4 + 1];
            ar = LRMod[k * 9 + 4 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[4] = newVal;
            } else {
                newPixel[4] = 0;
            }
            p = SrcPtr[rowOffset + 5];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 5 - 1];
            al = LRMod[k * 9 + 5];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 5];
            au = UDMod[k * 8 + 5];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 5];
            ad = UDMod[(k + 1) * 8 + 5];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 5 + 1];
            ar = LRMod[k * 9 + 5 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[5] = newVal;
            } else {
                newPixel[5] = 0;
            }
            p = SrcPtr[rowOffset + 6];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 6 - 1];
            al = LRMod[k * 9 + 6];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 6];
            au = UDMod[k * 8 + 6];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 6];
            ad = UDMod[(k + 1) * 8 + 6];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 6 + 1];
            ar = LRMod[k * 9 + 6 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[6] = newVal;
            } else {
                newPixel[6] = 0;
            }
            p = SrcPtr[rowOffset + 7];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + 7 - 1];
            al = LRMod[k * 9 + 7];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + 7];
            au = UDMod[k * 8 + 7];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + 7];
            ad = UDMod[(k + 1) * 8 + 7];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + 7 + 1];
            ar = LRMod[k * 9 + 7 + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                newPixel[7] = newVal;
            } else {
                newPixel[7] = 0;
            }
            DstPtr[rowOffset + 0] = newPixel[0];
            DstPtr[rowOffset + 1] = newPixel[1];
            DstPtr[rowOffset + 2] = newPixel[2];
            DstPtr[rowOffset + 3] = newPixel[3];
            DstPtr[rowOffset + 4] = newPixel[4];
            DstPtr[rowOffset + 5] = newPixel[5];
            DstPtr[rowOffset + 6] = newPixel[6];
            DstPtr[rowOffset + 7] = newPixel[7];
            rowOffset += Pitch;
        }
    }
}

void DeringBlockWeak_C(const POSTPROC_INSTANCE *pbi, const unsigned char *SrcPtr,
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
    unsigned int rowOffset = 0;
    unsigned int round = 64;
    unsigned int QValue = QuantScale[FragQIndex];
    int Sharpen = SharpenModifier[FragQIndex];
    const unsigned char *Src = SrcPtr;
    const unsigned char *curRow;
    const unsigned char *lastRow = SrcPtr - Pitch;
    const unsigned char *nextRow = SrcPtr + Pitch;
    unsigned char *dstRow;

    Low = 0;
    High = QValue * 3;
    if (High > 24) {
        High = 24;
    }

    for (k = 0; k <= 8; k++) {
        curRow = Src;
        for (j = 0; j < 8; j++) {
            TmpMod = 32 + QValue - (2 * abs(curRow[j] - curRow[j - Pitch]));
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < Low) {
                TmpMod = Low;
            } else if (TmpMod > High) {
                TmpMod = High;
            }
            UDMod[k * 8 + j] = TmpMod;
        }
        Src += Pitch;
    }

    Src = SrcPtr;
    for (k = 0; k <= 7; k++) {
        curRow = Src;
        for (j = 0; j <= 8; j++) {
            TmpMod = 32 + QValue - (2 * abs(curRow[j] - curRow[j - 1]));
            if (TmpMod < -64) {
                TmpMod = Sharpen;
            } else if (TmpMod < Low) {
                TmpMod = Low;
            } else if (TmpMod > High) {
                TmpMod = High;
            }
            LRMod[k * 9 + j] = TmpMod;
        }
        Src += Pitch;
    }

    for (k = 0; k < 8; k++) {
        for (j = 0; j < 8; j++) {
            p = SrcPtr[rowOffset + j];
            atot = 128;
            B = round;
            pl = SrcPtr[rowOffset + j - 1];
            al = LRMod[k * 9 + j];
            atot -= al;
            B += al * pl;
            pu = lastRow[rowOffset + j];
            au = UDMod[k * 8 + j];
            atot -= au;
            B += au * pu;
            pd = nextRow[rowOffset + j];
            ad = UDMod[(k + 1) * 8 + j];
            atot -= ad;
            B += ad * pd;
            pr = SrcPtr[rowOffset + j + 1];
            ar = LRMod[k * 9 + j + 1];
            atot -= ar;
            B += ar * pr;
            newVal = (atot * p + B) >> 7;
            if (newVal >= 0) {
                if (newVal > 255) {
                    newVal = 255;
                }
                DstPtr[rowOffset + j] = newVal;
            } else {
                DstPtr[rowOffset + j] = 0;
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

// NON_MATCHING: dispatch agrees with retail; loop/register ownership still differs.
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
    for (row = 0; row < BlocksDown; row++) {
            for (col = 0; col < BlocksAcross; col++) {
                int Variance;
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
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
    }

    BlocksAcross >>= 1;
    BlocksDown >>= 1;
    LineLength >>= 1;
    SrcPtr = Src + pbi->ReconUDataOffset;
    DestPtr = Dst + pbi->ReconUDataOffset;
    for (row = 0; row < BlocksDown; row++) {
            for (col = 0; col < BlocksAcross; col++) {
                int Variance;
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
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
    }

    SrcPtr = Src + pbi->ReconVDataOffset;
    DestPtr = Dst + pbi->ReconVDataOffset;
    for (row = 0; row < BlocksDown; row++) {
            for (col = 0; col < BlocksAcross; col++) {
                int Variance;
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
            }
            SrcPtr += LineLength << 3;
            DestPtr += LineLength << 3;
    }
}

// NON_MATCHING: dispatch agrees with retail; loop/register ownership still differs.
void DeringFrameInterlaced(POSTPROC_INSTANCE *pbi, unsigned char *Src,
                           unsigned char *Dst) {
    unsigned int Block;
    unsigned int col;
    unsigned int row;
    unsigned int BlocksAcross;
    unsigned int BlocksDown;
    unsigned int LineLength;
    unsigned int *QuantScale;
    int Thresh1;
    int Thresh2;
    int Thresh3;
    int Thresh4;
    unsigned char *SrcPtr;
    unsigned char *DestPtr;
    int Quality = pbi->FrameQIndex;

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
    BlocksDown = pbi->VFragments / 2;
    LineLength = pbi->YStride * 2;
    SrcPtr = Src + pbi->ReconYDataOffset;
    DestPtr = Dst + pbi->ReconYDataOffset;
    Block = 0;
    for (row = 0; row < BlocksDown; row++) {
        for (col = 0; col < BlocksAcross; col++) {
            int Variance;

            Variance = pbi->FragmentVariances[Block];
            if (pbi->PostProcessingLevel > 5 && Variance > Thresh3) {
                DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                  DestPtr + (col << 3), LineLength, Quality,
                                  QuantScale);
                if ((col != 0 &&
                     (Variance = pbi->FragmentVariances[Block - 1]) > Thresh4) ||
                    (col + 1 < BlocksAcross &&
                     (Variance = pbi->FragmentVariances[Block + 1]) > Thresh4) ||
                    (row + 1 < BlocksDown &&
                     (Variance = pbi->FragmentVariances[Block + BlocksAcross]) > Thresh4) ||
                    (row != 0 &&
                     (Variance = pbi->FragmentVariances[Block - BlocksAcross]) > Thresh4)) {
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
        }
        SrcPtr += LineLength * 8;
        DestPtr += LineLength * 8;
    }

    SrcPtr = Src + pbi->ReconYDataOffset + pbi->YStride;
    DestPtr = Dst + pbi->ReconYDataOffset + pbi->YStride;
    for (row = 0; row < BlocksDown; row++) {
        for (col = 0; col < BlocksAcross; col++) {
            int Variance;

            Variance = pbi->FragmentVariances[Block];
            if (pbi->PostProcessingLevel > 5 && Variance > Thresh3) {
                DeringBlockStrong(pbi, SrcPtr + (col << 3),
                                  DestPtr + (col << 3), LineLength, Quality,
                                  QuantScale);
                if ((col != 0 &&
                     (Variance = pbi->FragmentVariances[Block - 1]) > Thresh4) ||
                    (col + 1 < BlocksAcross &&
                     (Variance = pbi->FragmentVariances[Block + 1]) > Thresh4) ||
                    (row + 1 < BlocksDown &&
                     (Variance = pbi->FragmentVariances[Block + BlocksAcross]) > Thresh4) ||
                    (row != 0 &&
                     (Variance = pbi->FragmentVariances[Block - BlocksAcross]) > Thresh4)) {
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
        }
        SrcPtr += LineLength * 8;
        DestPtr += LineLength * 8;
    }

    BlocksAcross /= 2;
    LineLength /= 4;
    SrcPtr = Src + pbi->ReconUDataOffset;
    DestPtr = Dst + pbi->ReconUDataOffset;
    for (row = 0; row < BlocksDown; row++) {
        for (col = 0; col < BlocksAcross; col++) {
            int Variance;

            Variance = pbi->FragmentVariances[Block];
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
        }
        SrcPtr += LineLength * 8;
        DestPtr += LineLength * 8;
    }

    SrcPtr = Src + pbi->ReconVDataOffset;
    DestPtr = Dst + pbi->ReconVDataOffset;
    for (row = 0; row < BlocksDown; row++) {
        for (col = 0; col < BlocksAcross; col++) {
            int Variance;

            Variance = pbi->FragmentVariances[Block];
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
        }
        SrcPtr += LineLength * 8;
        DestPtr += LineLength * 8;
    }
}
