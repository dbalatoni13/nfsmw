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

extern unsigned char LimitVal_VP31[0x300];
extern unsigned int DeringModifierV1[64];
extern unsigned int *DCQuantScaleV2;
extern unsigned int *DCQuantScaleUV;
extern unsigned int *DCQuantScaleV1;
extern unsigned char *DeblockLimitValuesV2;
extern unsigned char *LoopFilterLimitValuesV2;
extern unsigned char DeblockLimitValuesVp4[0x100];
extern unsigned char DeblockLimitValuesVp5[0x100];
extern unsigned char DeblockLimitValuesVp6[0x100];
extern unsigned char LoopFilterLimitValuesVp4[0x100];
extern unsigned char LoopFilterLimitValuesVp5[0x100];
extern unsigned char LoopFilterLimitValuesVp6[0x100];
extern void PostProcMachineSpecificConfig(unsigned int Version);
extern void *duck_malloc(unsigned int blocksize, int type);
extern void duck_free(void *block);
extern void *memset(void *dest, int value, unsigned int size);
extern void *memcpy(void *dest, const void *source, unsigned int size);
extern double exp(double x);
extern double sqrt(double x);
extern int rand(void);

typedef void (*FAST_DEINTERLACE_FUNCTION)(unsigned char *, unsigned char *,
                                          int, int, int);
typedef void (*PLANE_ADD_NOISE_FUNCTION)(unsigned char *, unsigned int,
                                         unsigned int, int, int);
extern FAST_DEINTERLACE_FUNCTION FastDeInterlace;
extern PLANE_ADD_NOISE_FUNCTION PlaneAddNoise;
extern void DeblockFrame(POSTPROC_INSTANCE *ppi, unsigned char *SourceBuffer,
                         unsigned char *DestinationBuffer);
extern void DeblockFrameInterlaced(POSTPROC_INSTANCE *ppi,
                                   unsigned char *SourceBuffer,
                                   unsigned char *DestinationBuffer);
extern void UpdateUMVBorder(POSTPROC_INSTANCE *ppi,
                            unsigned char *DestReconPtr);
extern void DeringFrame(POSTPROC_INSTANCE *ppi, unsigned char *Src,
                        unsigned char *Dst);
extern void DeringFrameInterlaced(POSTPROC_INSTANCE *ppi, unsigned char *Src,
                                  unsigned char *Dst);
extern void SimpleDeblockFrame(POSTPROC_INSTANCE *ppi,
                               unsigned char *SrcBuffer,
                               unsigned char *DestBuffer);

void ChangePostProcConfiguration(POSTPROC_INSTANCE *ppi, CONFIG_TYPE *ConfigurationInit);

POSTPROC_INSTANCE *CreatePostProcInstance(CONFIG_TYPE *ConfigurationInit) {
    POSTPROC_INSTANCE *ppi = 0;
    int postproc_size;

    postproc_size = sizeof(POSTPROC_INSTANCE);
    ppi = (POSTPROC_INSTANCE *)duck_malloc(postproc_size, 0);
    if (ppi == 0) {
        return 0;
    }

    memset(ppi, 0, postproc_size);
    ChangePostProcConfiguration(ppi, ConfigurationInit);
    ppi->AddNoiseMode = 1;
    return ppi;
}

void DeletePostProcBuffers(POSTPROC_INSTANCE *ppi) {
    if (ppi->IntermediateBufferAlloc != 0) {
        duck_free(ppi->IntermediateBufferAlloc);
    }
    ppi->IntermediateBufferAlloc = 0;
    ppi->IntermediateBuffer = 0;
    if (ppi->FiltBoundingValueAlloc != 0) {
        duck_free(ppi->FiltBoundingValueAlloc);
    }
    ppi->FiltBoundingValueAlloc = 0;
    ppi->FiltBoundingValue = 0;
    if (ppi->DeblockBoundingValueAlloc != 0) {
        duck_free(ppi->DeblockBoundingValueAlloc);
    }
    ppi->DeblockBoundingValueAlloc = 0;
    ppi->DeblockBoundingValue = 0;
    if (ppi->FragQIndexAlloc != 0) {
        duck_free(ppi->FragQIndexAlloc);
    }
    ppi->FragQIndexAlloc = 0;
    ppi->FragQIndex = 0;
    if (ppi->FragmentVariancesAlloc != 0) {
        duck_free(ppi->FragmentVariancesAlloc);
    }
    ppi->FragmentVariancesAlloc = 0;
    ppi->FragmentVariances = 0;
    if (ppi->FragDeblockingFlagAlloc != 0) {
        duck_free(ppi->FragDeblockingFlagAlloc);
    }
    ppi->FragDeblockingFlagAlloc = 0;
    ppi->FragDeblockingFlag = 0;
}

void DeletePostProcInstance(POSTPROC_INSTANCE **ppi) {
    if (*ppi != 0) {
        DeletePostProcBuffers(*ppi);
    }
    duck_free(*ppi);
    *ppi = 0;
}

int AllocatePostProcBuffers(POSTPROC_INSTANCE *ppi) {
    DeletePostProcBuffers(ppi);

    ppi->IntermediateBufferAlloc = (unsigned char *)duck_malloc(
        ppi->YStride * (ppi->Configuration.VideoFrameHeight + 2 * ppi->MVBorder) * 3 / 2 + 0x20,
        0);
    if (ppi->IntermediateBufferAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->IntermediateBuffer = (unsigned char *)(((unsigned int)ppi->IntermediateBufferAlloc + 0x1f) & ~0x1f);

    ppi->FiltBoundingValueAlloc = (int *)duck_malloc(0x820, 0);
    if (ppi->FiltBoundingValueAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FiltBoundingValue = (int *)(((unsigned int)ppi->FiltBoundingValueAlloc + 0x1f) & ~0x1f);

    ppi->DeblockBoundingValueAlloc = (int *)duck_malloc(0x820, 0);
    if (ppi->DeblockBoundingValueAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->DeblockBoundingValue = (int *)(((unsigned int)ppi->DeblockBoundingValueAlloc + 0x1f) & ~0x1f);

    ppi->FragQIndexAlloc = (int *)duck_malloc(ppi->UnitFragments * 4 + 0x20, 0);
    if (ppi->FragQIndexAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragQIndex = (int *)(((unsigned int)ppi->FragQIndexAlloc + 0x1f) & ~0x1f);

    ppi->FragmentVariancesAlloc = (int *)duck_malloc(ppi->UnitFragments * 4 + 0x20, 0);
    if (ppi->FragmentVariancesAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragmentVariances = (int *)(((unsigned int)ppi->FragmentVariancesAlloc + 0x1f) & ~0x1f);

    ppi->FragDeblockingFlagAlloc = (unsigned char *)duck_malloc(ppi->UnitFragments * 4 + 0x20, 0);
    if (ppi->FragDeblockingFlagAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragDeblockingFlag = (unsigned char *)(((unsigned int)ppi->FragDeblockingFlagAlloc + 0x1f) & ~0x1f);

    return 1;
}

void UpdateFragQIndex(POSTPROC_INSTANCE *ppi) {
    unsigned int i;
    unsigned int ThisFrameQIndex;

    ThisFrameQIndex = ppi->FrameQIndex;
    for (i = 0; i < ppi->UnitFragments; i++) {
        if (ppi->FragInfo[i * ppi->FragInfoElementSize] & ppi->FragInfoCodedMask) {
            ppi->FragQIndex[i] = ThisFrameQIndex;
        }
    }
}

double gaussian(double sigma, double mu, double x) {
    return (1.0 / (sigma * sqrt(6.28318530717958647692))) *
           exp((-(x - mu) * (x - mu)) / ((sigma + sigma) * sigma));
}

void PlaneAddNoise_C(unsigned char *Start, unsigned int Width, unsigned int Height,
                     int Pitch, int q) {
    unsigned int i;
    unsigned int j;
    unsigned char blackclamp[16];
    unsigned char whiteclamp[16];
    unsigned char bothclamp[16];
    char CharDist[300];
    char Rand[2048];
    double sigma;

    sigma = (double)(63 - q) * 0.012698412698412698 + 1.0;
    {
        double i;
        int next;
        int j;

        i = -32.0;
        j = 0;
        while (i < 32.0) {
            next = (int)(gaussian(sigma, 0.0, i) * 32.0 + 0.5);
            if (next != 0) {
                int a;

                for (a = 0; a < next; a++) {
                    CharDist[j + a] = (char)i;
                }
                j += a;
            }
            i += 1.0;
        }

        if (j <= 0xff) {
            for (; j <= 0xff; j++) {
                CharDist[j] = 0;
            }
        }
    }

    for (i = 0; i <= 0x7ff; i++) {
        Rand[i] = CharDist[rand() & 0xff];
    }

    for (i = 0; i < 16; i++) {
        blackclamp[i] = -CharDist[0];
        whiteclamp[i] = -CharDist[0];
        bothclamp[i] = -2 * CharDist[0];
    }

    {
        unsigned char *Pos;
        signed char *Ref;

        for (i = 0; i < Height; i++) {
            Pos = Start + i * Pitch;
            Ref = (signed char *)Rand + (rand() & 0xff);
            for (j = 0; j < Width; j++) {
                if (Pos[j] < -CharDist[0]) {
                    Pos[j] = -CharDist[0];
                }
                if (Pos[j] > 255 - CharDist[0]) {
                    Pos[j] = ~CharDist[0];
                }
                Pos[j] += Ref[j];
            }
        }
    }
}

void PostProcess(POSTPROC_INSTANCE *ppi, int Vp3VersionNo, int FrameType,
                 int PostProcessingLevel, int FrameQIndex,
                 unsigned char *LastFrameRecon,
                 unsigned char *PostProcessBuffer, unsigned char *FragInfo,
                 unsigned int FragInfoElementSize,
                 unsigned int FragInfoCodedMask) {
    int ReconUVPlaneSize;

    ppi->FrameType = FrameType;
    ppi->FragInfo = FragInfo;
    ppi->FragInfoElementSize = FragInfoElementSize;
    ppi->FragInfoCodedMask = FragInfoCodedMask;
    ppi->Vp3VersionNo = Vp3VersionNo;
    ppi->PostProcessingLevel = PostProcessingLevel;
    ppi->FrameQIndex = FrameQIndex;
    ppi->LastFrameRecon = LastFrameRecon;
    ppi->PostProcessBuffer = PostProcessBuffer;

    switch (PostProcessingLevel) {
    case 0:
        if (ppi->Configuration.Interlaced != 0 &&
            ppi->DeInterlaceMode != 0) {
            ReconUVPlaneSize = ppi->VFragments *
                               (ppi->YStride + ppi->YStride);
            memcpy(PostProcessBuffer + ppi->ReconUDataOffset,
                   LastFrameRecon + ppi->ReconUDataOffset,
                   ReconUVPlaneSize);
            memcpy(PostProcessBuffer + ppi->ReconVDataOffset,
                   LastFrameRecon + ppi->ReconVDataOffset,
                   ReconUVPlaneSize);
            FastDeInterlace(LastFrameRecon + ppi->ReconYDataOffset,
                            PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments << 3, ppi->VFragments << 3,
                            ppi->YStride);
        }
        break;

    case 1:
        UpdateFragQIndex(ppi);
        break;

    case 4:
        if (Vp3VersionNo <= 4) {
            UpdateFragQIndex(ppi);
            DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
        } else if (ppi->Configuration.Interlaced == 0) {
            DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
        } else if (ppi->DeInterlaceMode == 0) {
            DeblockFrameInterlaced(ppi, LastFrameRecon, PostProcessBuffer);
        } else {
            DeblockFrameInterlaced(ppi, LastFrameRecon,
                                   ppi->IntermediateBuffer);
            ReconUVPlaneSize = ppi->VFragments *
                               (ppi->YStride + ppi->YStride);
            memcpy(PostProcessBuffer + ppi->ReconUDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconUDataOffset,
                   ReconUVPlaneSize);
            memcpy(PostProcessBuffer + ppi->ReconVDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconVDataOffset,
                   ReconUVPlaneSize);
            FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                            PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments << 3, ppi->VFragments << 3,
                            ppi->YStride);
        }
        break;

    case 5:
    case 6:
        if (Vp3VersionNo <= 4) {
            UpdateFragQIndex(ppi);
        }
        if (Vp3VersionNo > 4 && ppi->Configuration.Interlaced != 0) {
            if (ppi->DeInterlaceMode == 0) {
                DeblockFrameInterlaced(ppi, LastFrameRecon,
                                       PostProcessBuffer);
                UpdateUMVBorder(ppi, PostProcessBuffer);
                DeringFrameInterlaced(ppi, PostProcessBuffer,
                                      PostProcessBuffer);
            } else {
                DeblockFrameInterlaced(ppi, LastFrameRecon,
                                       ppi->IntermediateBuffer);
                UpdateUMVBorder(ppi, ppi->IntermediateBuffer);
                DeringFrameInterlaced(ppi, ppi->IntermediateBuffer,
                                      ppi->IntermediateBuffer);
                ReconUVPlaneSize = ppi->VFragments *
                                   (ppi->YStride + ppi->YStride);
                memcpy(PostProcessBuffer + ppi->ReconUDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconUDataOffset,
                       ReconUVPlaneSize);
                memcpy(PostProcessBuffer + ppi->ReconVDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconVDataOffset,
                       ReconUVPlaneSize);
                FastDeInterlace(ppi->IntermediateBuffer +
                                    ppi->ReconYDataOffset,
                                PostProcessBuffer + ppi->ReconYDataOffset,
                                ppi->HFragments << 3, ppi->VFragments << 3,
                                ppi->YStride);
            }
        } else {
            DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
            UpdateUMVBorder(ppi, PostProcessBuffer);
            DeringFrame(ppi, PostProcessBuffer, PostProcessBuffer);
            if (ppi->AddNoiseMode != 0) {
                PlaneAddNoise(PostProcessBuffer + ppi->ReconYDataOffset,
                              ppi->HFragments << 3, ppi->VFragments << 3,
                              ppi->YStride, FrameQIndex);
            }
            FastDeInterlace(PostProcessBuffer + ppi->ReconYDataOffset,
                            PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments << 3, ppi->VFragments << 3,
                            ppi->YStride);
        }
        break;

    case 7:
        if (Vp3VersionNo <= 4) {
            UpdateFragQIndex(ppi);
            DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
        } else if (ppi->Configuration.Interlaced == 0) {
            DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
            UpdateUMVBorder(ppi, PostProcessBuffer);
            DeringFrame(ppi, PostProcessBuffer, PostProcessBuffer);
        } else if (ppi->DeInterlaceMode == 0) {
            DeblockFrameInterlaced(ppi, LastFrameRecon, PostProcessBuffer);
        } else {
            DeblockFrameInterlaced(ppi, LastFrameRecon,
                                   ppi->IntermediateBuffer);
            ReconUVPlaneSize = ppi->VFragments *
                               (ppi->YStride + ppi->YStride);
            memcpy(PostProcessBuffer + ppi->ReconUDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconUDataOffset,
                   ReconUVPlaneSize);
            memcpy(PostProcessBuffer + ppi->ReconVDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconVDataOffset,
                   ReconUVPlaneSize);
            FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                            PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments << 3, ppi->VFragments << 3,
                            ppi->YStride);
        }
        break;

    case 8:
        UpdateFragQIndex(ppi);
        if (Vp3VersionNo <= 1) {
            break;
        }
        if (ppi->Configuration.Interlaced == 0 ||
            ppi->DeInterlaceMode == 0) {
            SimpleDeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
        } else {
            SimpleDeblockFrame(ppi, LastFrameRecon,
                               ppi->IntermediateBuffer);
            ReconUVPlaneSize = ppi->VFragments *
                               (ppi->YStride + ppi->YStride);
            memcpy(PostProcessBuffer + ppi->ReconUDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconUDataOffset,
                   ReconUVPlaneSize);
            memcpy(PostProcessBuffer + ppi->ReconVDataOffset,
                   ppi->IntermediateBuffer + ppi->ReconVDataOffset,
                   ReconUVPlaneSize);
            FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                            PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments << 3, ppi->VFragments << 3,
                            ppi->YStride);
        }
        break;

    default:
        DeblockFrame(ppi, LastFrameRecon, PostProcessBuffer);
        UpdateUMVBorder(ppi, PostProcessBuffer);
        DeringFrame(ppi, PostProcessBuffer, PostProcessBuffer);
        break;
    }
}

void InitPostProcessing(unsigned int *DCQuantScaleV2p, unsigned int *DCQuantScaleUVp,
                        unsigned int *DCQuantScaleV1p, unsigned int Version) {
    int i;
    int x;

    for (i = 0; i < 0x300; i++) {
        x = i - 0x100;
        if (x >= 0) {
            if (x > 0xff) {
                x = 0xff;
            }
            *(i + LimitVal_VP31) = x;
        } else {
            *(i + LimitVal_VP31) = 0;
        }
    }

    DCQuantScaleV2 = DCQuantScaleV2p;
    DCQuantScaleUV = DCQuantScaleUVp;
    DCQuantScaleV1 = DCQuantScaleV1p;

    for (i = 0; i < 0x100; i += 4) {
        *(unsigned int *)((unsigned char *)DeringModifierV1 + i) =
            *(unsigned int *)(i + (unsigned char *)DCQuantScaleV1p);
    }

    if (Version > 5) {
        LoopFilterLimitValuesV2 = LoopFilterLimitValuesVp6;
        DeblockLimitValuesV2 = DeblockLimitValuesVp6;
    } else if (Version > 4) {
        LoopFilterLimitValuesV2 = LoopFilterLimitValuesVp5;
        DeblockLimitValuesV2 = DeblockLimitValuesVp5;
    } else {
        LoopFilterLimitValuesV2 = LoopFilterLimitValuesVp4;
        DeblockLimitValuesV2 = DeblockLimitValuesVp4;
    }

    PostProcMachineSpecificConfig(Version);
}

void ChangePostProcConfiguration(POSTPROC_INSTANCE *ppi, CONFIG_TYPE *ConfigurationInit) {
    memcpy(&ppi->Configuration, ConfigurationInit, sizeof(CONFIG_TYPE));
    ppi->HFragments = ppi->Configuration.VideoFrameWidth / 8;
    ppi->VFragments = ppi->Configuration.VideoFrameHeight / 8;
    ppi->YStride = ppi->Configuration.YStride;
    ppi->UVStride = ppi->Configuration.UVStride;
    ppi->YPlaneFragments = ppi->HFragments * ppi->VFragments;
    ppi->UVPlaneFragments = ppi->YPlaneFragments / 4;
    ppi->UnitFragments = ppi->YPlaneFragments + 2 * ppi->UVPlaneFragments;
    ppi->MVBorder = (ppi->YStride - ppi->HFragments * 8) / 2;
    ppi->ReconYDataOffset = ppi->MVBorder * ppi->YStride + ppi->MVBorder;
    ppi->ReconUDataOffset =
        ppi->YStride * (ppi->Configuration.VideoFrameHeight + 2 * ppi->MVBorder) +
        ppi->UVStride * (ppi->MVBorder / 2) + (ppi->MVBorder / 2);
    ppi->ReconVDataOffset =
        ppi->ReconUDataOffset +
        ppi->UVStride * ((ppi->Configuration.VideoFrameHeight / 2) + ppi->MVBorder) +
        ppi->UVStride * (ppi->MVBorder / 2) + (ppi->MVBorder / 2);
}

void SetPPInterlacedMode(POSTPROC_INSTANCE *ppi, int Interlaced) {
    ppi->Configuration.Interlaced = Interlaced;
}

void SetDeInterlaceMode(POSTPROC_INSTANCE *ppi, int DeInterlaceMode) {
    ppi->DeInterlaceMode = DeInterlaceMode;
}

void SetAddNoiseMode(POSTPROC_INSTANCE *ppi, int AddNoiseMode) {
    ppi->AddNoiseMode = AddNoiseMode;
}
