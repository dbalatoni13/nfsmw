void NullScale(unsigned char *dest, unsigned int destPitch, unsigned int destWidth) {}

extern void *memcpy(void *dest, const void *source, unsigned int size);

typedef struct CONFIG_TYPE {
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
} CONFIG_TYPE;

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
    CONFIG_TYPE Configuration;
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

typedef struct {
    int YWidth;
    int YHeight;
    int YStride;
    int UVWidth;
    int UVHeight;
    int UVStride;
    unsigned char *YBuffer;
    unsigned char *UBuffer;
    unsigned char *VBuffer;
} YUV_BUFFER_CONFIG;

extern void UpdateUMVBorder(POSTPROC_INSTANCE *ppi,
                            unsigned char *DestReconPtr);
extern void (*HorizontalLine_1_2_Scale)(void);
extern void (*VerticalBand_1_2_Scale)(void);
extern void (*LastVerticalBand_1_2_Scale)(void);
extern void (*HorizontalLine_3_5_Scale)(void);
extern void (*VerticalBand_3_5_Scale)(void);
extern void (*LastVerticalBand_3_5_Scale)(void);
extern void (*HorizontalLine_4_5_Scale)(void);
extern void (*VerticalBand_4_5_Scale)(void);
extern void (*LastVerticalBand_4_5_Scale)(void);

void HorizontalLine_Copy(const unsigned char *source, unsigned int sourceWidth, unsigned char *dest, unsigned int destWidth) {
    memcpy(dest, source, sourceWidth);
}

void HorizontalLine_1_2_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned char *des;
    const unsigned char *src;

    src = source;
    des = dest;
    sourceWidth--;
    i = 0;
    while (i < sourceWidth) {
        a = src[0];
        b = src[1];
        i++;
        des[0] = a;
        des[1] = (a + b + 1) >> 1;
        src++;
        des += 2;
    }
    a = src[0];
    des[0] = a;
    des[1] = a;
}

void VerticalBand_1_2_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b;
    unsigned char *des = dest;

    for (i = 0; i < destWidth; i++) {
        a = des[0];
        b = des[destPitch * 2];
        des[destPitch] = (unsigned char) ((a + b + 1) >> 1);
        des++;
    }
}

void LastVerticalBand_1_2_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        des[destPitch] = des[0];
        des++;
        i++;
    }
}

void HorizontalLine_4_5_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned i;
    unsigned int a, b, c;
    unsigned char *des = dest;
    const unsigned char *src = source;

    (void) destWidth;

    for (i = 0; i < sourceWidth - 4; i += 4) {
        a = src[0];
        b = src[1];
        des[0] = (unsigned char) a;
        des[1] = (unsigned char) ((a * 51 + 205 * b + 128) >> 8);
        c = src[2] * 154;
        a = src[3];
        des[2] = (unsigned char) ((b * 102 + c + 128) >> 8);
        des[3] = (unsigned char) ((c + 102 * a + 128) >> 8);
        b = src[4];
        des[4] = (unsigned char) ((a * 205 + 51 * b + 128) >> 8);
        src += 4;
        des += 5;
    }

    a = src[0];
    b = src[1];
    des[0] = (unsigned char) a;
    des[1] = (unsigned char) ((a * 51 + 205 * b + 128) >> 8);
    c = src[2] * 154;
    a = src[3];
    des[2] = (unsigned char) ((b * 102 + c + 128) >> 8);
    des[3] = (unsigned char) ((c + 102 * a + 128) >> 8);
    des[4] = (unsigned char) a;
}

void VerticalBand_4_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b, c, d;
    unsigned char *des = dest;

    for (i = 0; i < destWidth; i++) {
        a = des[0];
        b = des[destPitch];
        des[destPitch] = (unsigned char) ((a * 51 + 205 * b + 128) >> 8);
        c = des[destPitch * 2] * 154;
        d = des[destPitch * 3];
        des[destPitch * 2] = (unsigned char) ((b * 102 + c + 128) >> 8);
        des[destPitch * 3] = (unsigned char) ((c + 102 * d + 128) >> 8);
        a = des[destPitch * 5];
        des[destPitch * 4] = (unsigned char) ((d * 205 + 51 * a + 128) >> 8);
        des++;
    }
}

void LastVerticalBand_4_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b, c, d;
    unsigned char *des = dest;

    for (i = 0; i < destWidth; i++) {
        a = des[0];
        b = des[destPitch];
        des[destPitch] = (unsigned char) ((a * 51 + 205 * b + 128) >> 8);
        c = des[destPitch * 2] * 154;
        d = des[destPitch * 3];
        des[destPitch * 2] = (unsigned char) ((b * 102 + c + 128) >> 8);
        des[destPitch * 3] = (unsigned char) ((c + 102 * d + 128) >> 8);
        des[destPitch * 4] = (unsigned char) d;
        des++;
    }
}

void HorizontalLine_3_5_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b, c;
    unsigned char *des = dest;
    const unsigned char *src = source;

    (void) destWidth;

    for (i = 0; i < sourceWidth - 3; i += 3) {
        a = src[0];
        b = src[1];
        des[0] = (unsigned char) a;
        des[1] = (unsigned char) ((a * 102 + 154 * b + 128) >> 8);
        c = src[2];
        des[2] = (unsigned char) ((b * 205 + c * 51 + 128) >> 8);
        des[3] = (unsigned char) ((b * 51 + c * 205 + 128) >> 8);
        a = src[3];
        des[4] = (unsigned char) ((c * 154 + a * 102 + 128) >> 8);
        src += 3;
        des += 5;
    }

    a = src[0];
    b = src[1];
    des[0] = (unsigned char) a;
    des[1] = (unsigned char) ((a * 102 + 154 * b + 128) >> 8);
    c = src[2];
    des[2] = (unsigned char) ((b * 205 + c * 51 + 128) >> 8);
    des[3] = (unsigned char) ((b * 51 + c * 205 + 128) >> 8);
    des[4] = (unsigned char) c;
}

void VerticalBand_3_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b, c;
    unsigned char *des = dest;

    for (i = 0; i < destWidth; i++) {
        a = des[0];
        b = des[destPitch];
        des[destPitch] = (unsigned char) ((a * 102 + 154 * b + 128) >> 8);
        c = des[destPitch * 2];
        des[destPitch * 2] = (unsigned char) ((b * 205 + c * 51 + 128) >> 8);
        des[destPitch * 3] = (unsigned char) ((b * 51 + c * 205 + 128) >> 8);
        a = des[destPitch * 5];
        des[destPitch * 4] = (unsigned char) ((c * 154 + a * 102 + 128) >> 8);
        des++;
    }
}

void LastVerticalBand_3_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned int a, b, c;
    unsigned char *des = dest;

    for (i = 0; i < destWidth; ++i) {
        a = des[0];
        b = des[destPitch];
        des[destPitch] = (unsigned char) ((a * 102 + 154 * b + 128) >> 8);
        c = des[destPitch * 2];
        des[destPitch * 2] = (unsigned char) ((b * 205 + c * 51 + 128) >> 8);
        des[destPitch * 3] = (unsigned char) ((b * 51 + c * 205 + 128) >> 8);
        des[destPitch * 4] = (unsigned char) c;
        des++;
    }
}

int AnyRatio_2D_Scale(POSTPROC_INSTANCE *ppi,
                      const unsigned char *source,
                      unsigned int sourcePitch,
                      unsigned int sourceWidth,
                      unsigned int sourceHeight,
                      unsigned char *dest,
                      unsigned int destPitch,
                      unsigned int destWidth,
                      unsigned int destHeight) {
    unsigned int i, k;
    unsigned int srcBandHeight = 0;
    unsigned int destBandHeight = 0;
    int hs = ppi->Configuration.HScale;
    int hr = ppi->Configuration.HRatio;
    int vs = ppi->Configuration.VScale;
    int vr = ppi->Configuration.VRatio;
    int RatioScalable = 1;
    void (*HorizLineScale)(const unsigned char *, unsigned int,
                           unsigned char *, unsigned int) = 0;
    void (*VertBandScale)(unsigned char *, unsigned int, unsigned int) = 0;
    void (*LastVertBandScale)(unsigned char *, unsigned int, unsigned int) = 0;

    (void) ppi;

    switch (hr * 10 / hs) {
    case 8:
        HorizLineScale = HorizontalLine_4_5_Scale;
        break;
    case 6:
        HorizLineScale = HorizontalLine_3_5_Scale;
        break;
    case 5:
        HorizLineScale = HorizontalLine_1_2_Scale;
        break;
    case 10:
        HorizLineScale = HorizontalLine_Copy;
        break;
    default:
        RatioScalable = 0;
        break;
    }

    switch (vr * 10 / vs) {
    case 8:
        VertBandScale = VerticalBand_4_5_Scale;
        LastVertBandScale = LastVerticalBand_4_5_Scale;
        srcBandHeight = 4;
        destBandHeight = 5;
        break;
    case 6:
        VertBandScale = VerticalBand_3_5_Scale;
        LastVertBandScale = LastVerticalBand_3_5_Scale;
        srcBandHeight = 3;
        destBandHeight = 5;
        break;
    case 5:
        VertBandScale = VerticalBand_1_2_Scale;
        LastVertBandScale = LastVerticalBand_1_2_Scale;
        srcBandHeight = 1;
        destBandHeight = 2;
        break;
    case 10:
        VertBandScale = NullScale;
        LastVertBandScale = NullScale;
        srcBandHeight = 4;
        destBandHeight = 4;
        break;
    default:
        RatioScalable = 0;
        break;
    }

    if (RatioScalable == 0) {
        return RatioScalable;
    }

    HorizLineScale(source, sourceWidth, dest, destWidth);
    for (k = 0; k < (destHeight + destBandHeight - 1) /
                        destBandHeight - 1; k++) {
        for (i = 1; i < srcBandHeight; i++) {
            HorizLineScale(source + i * sourcePitch, sourceWidth,
                           dest + i * destPitch, destWidth);
        }
        HorizLineScale(source + srcBandHeight * sourcePitch, sourceWidth,
                       dest + destBandHeight * destPitch, destWidth);
        VertBandScale(dest, destPitch, destWidth);
        source += srcBandHeight * sourcePitch;
        dest += destBandHeight * destPitch;
    }

    for (i = 1; i < srcBandHeight; i++) {
        HorizLineScale(source + i * sourcePitch, sourceWidth,
                       dest + i * destPitch, destWidth);
    }
    LastVertBandScale(dest, destPitch, destWidth);
    return RatioScalable;
}

int AnyRatioFrameScale(POSTPROC_INSTANCE *ppi,
                       unsigned char *FrameBuffer,
                       YUV_BUFFER_CONFIG *YuvConfig,
                       int YOffset,
                       int UVOffset) {
    int i;
    int ew;
    int eh;
    int hs = ppi->Configuration.HScale;
    int hr = ppi->Configuration.HRatio;
    int vs = ppi->Configuration.VScale;
    int vr = ppi->Configuration.VRatio;
    int RatioScalable = 1;
    int sw = (ppi->Configuration.ExpandedFrameWidth * hr + hs - 1) / hs;
    int sh = (ppi->Configuration.ExpandedFrameHeight * vr + vs - 1) / vs;
    int dw = ppi->Configuration.ExpandedFrameWidth;
    int dh = ppi->Configuration.ExpandedFrameHeight;

    if (hr == 3) {
        ew = (sw + 2) / 3 * 3 * hs / hr;
    } else {
        ew = (sw + 7) / 8 * 8 * hs / hr;
    }

    if (vr == 3) {
        eh = (sh + 2) / 3 * 3 * vs / vr;
    } else {
        eh = (sh + 7) / 8 * 8 * vs / vr;
    }

    RatioScalable = AnyRatio_2D_Scale(
        ppi, &FrameBuffer[ppi->ReconYDataOffset],
        ppi->Configuration.VideoFrameWidth + ppi->MVBorder * 2,
        sw, sh, YuvConfig->YBuffer + YOffset, YuvConfig->YStride, dw, dh);

    for (i = 0; i < eh; i++) {
        memset(YuvConfig->YBuffer + YOffset + i * YuvConfig->YStride + dw,
               0, ew - dw);
    }
    for (i = dh; i < eh; i++) {
        memset(YuvConfig->YBuffer + YOffset + i * YuvConfig->YStride,
               0, ew);
    }

    if (RatioScalable == 0) {
        return RatioScalable;
    }

    sw = (sw + 1) >> 1;
    sh = (sh + 1) >> 1;
    dw = (dw + 1) >> 1;
    dh = (dh + 1) >> 1;

    AnyRatio_2D_Scale(
        ppi, &FrameBuffer[ppi->ReconUDataOffset],
        ppi->Configuration.VideoFrameWidth / 2 + ppi->MVBorder,
        sw, sh, YuvConfig->UBuffer + UVOffset, YuvConfig->UVStride, dw, dh);
    AnyRatio_2D_Scale(
        ppi, &FrameBuffer[ppi->ReconVDataOffset],
        ppi->Configuration.VideoFrameWidth / 2 + ppi->MVBorder,
        sw, sh, YuvConfig->VBuffer + UVOffset, YuvConfig->UVStride, dw, dh);

    return RatioScalable;
}

void CenterImage(POSTPROC_INSTANCE *ppi, unsigned char *FrameBuffer,
                 YUV_BUFFER_CONFIG *YuvConfig) {
    unsigned int i;
    int RowOffset, ColOffset;
    unsigned char *SrcDataPointer;
    unsigned char *DstDataPointer;

    RowOffset = (YuvConfig->YHeight - ppi->Configuration.VideoFrameHeight) / 2;
    ColOffset = (YuvConfig->YWidth - ppi->Configuration.VideoFrameWidth) / 2;

    SrcDataPointer = &FrameBuffer[ppi->ReconYDataOffset];
    DstDataPointer = YuvConfig->YBuffer + RowOffset * YuvConfig->YWidth + ColOffset;
    for (i = 0; i < ppi->Configuration.VideoFrameHeight; i++) {
        memcpy(DstDataPointer, SrcDataPointer,
               ppi->Configuration.VideoFrameWidth);
        DstDataPointer += YuvConfig->YWidth;
        SrcDataPointer += ppi->YStride;
    }

    SrcDataPointer = &FrameBuffer[ppi->ReconUDataOffset];
    DstDataPointer = YuvConfig->UBuffer + RowOffset / 2 * YuvConfig->UVWidth + ColOffset / 2;
    for (i = 0; i < ppi->Configuration.VideoFrameHeight / 2; i++) {
        memcpy(DstDataPointer, SrcDataPointer,
               ppi->Configuration.VideoFrameWidth / 2);
        DstDataPointer += YuvConfig->UVWidth;
        SrcDataPointer += ppi->UVStride;
    }

    SrcDataPointer = &FrameBuffer[ppi->ReconVDataOffset];
    DstDataPointer = YuvConfig->VBuffer + RowOffset / 2 * YuvConfig->UVWidth + ColOffset / 2;
    for (i = 0; i < ppi->Configuration.VideoFrameHeight / 2; i++) {
        memcpy(DstDataPointer, SrcDataPointer,
               ppi->Configuration.VideoFrameWidth / 2);
        DstDataPointer += YuvConfig->UVWidth;
        SrcDataPointer += ppi->UVStride;
    }
}

void ScaleOrCenter(POSTPROC_INSTANCE *ppi, unsigned char *FrameBuffer,
                   YUV_BUFFER_CONFIG *YuvConfig) {
    if (ppi->PostProcessingLevel) {
        UpdateUMVBorder(ppi, FrameBuffer);
    }

    switch (ppi->Configuration.ScalingMode) {
    case 0:
    case 1: {
        int row = (YuvConfig->YHeight -
                   ppi->Configuration.ExpandedFrameHeight) / 2;
        int col = (YuvConfig->YWidth -
                   ppi->Configuration.ExpandedFrameWidth) / 2;
        int YOffset = row * YuvConfig->YWidth + col;
        int UVOffset = (row >> 1) * YuvConfig->UVWidth + (col >> 1);

        AnyRatioFrameScale(ppi, FrameBuffer, YuvConfig, YOffset, UVOffset);
        break;
    }
    case 2:
        CenterImage(ppi, FrameBuffer, YuvConfig);
        break;
    default:
        break;
    }
}
