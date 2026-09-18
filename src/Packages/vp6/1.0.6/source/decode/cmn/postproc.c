typedef int INT32;
typedef unsigned char UINT8;
typedef unsigned int UINT32;

typedef struct {
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

extern void *duck_malloc(unsigned long size, int type);
extern void duck_free(void *mem);
extern void PostProcMachineSpecificConfig(UINT32 Version);
extern void *memset(void *, int, unsigned long);
extern void *memcpy();
extern void SimpleDeblockFrame(POSTPROC_INSTANCE *ppi, UINT8 *SrcPtr, UINT8 *DesPtr);
extern void DeblockFrame(POSTPROC_INSTANCE *ppi, UINT8 *SrcPtr, UINT8 *DesPtr);
extern void DeblockFrameInterlaced(POSTPROC_INSTANCE *ppi, UINT8 *SrcPtr, UINT8 *DesPtr);
extern void DeringFrame(POSTPROC_INSTANCE *ppi, UINT8 *SrcPtr, UINT8 *DesPtr);
extern void DeringFrameInterlaced(POSTPROC_INSTANCE *ppi, UINT8 *SrcPtr, UINT8 *DesPtr);
extern void UpdateUMVBorder(POSTPROC_INSTANCE *ppi, UINT8 *DesPtr);
extern double exp(double);
extern int rand(void);
extern double sqrt(double);

extern UINT32 *DCQuantScaleV2;
extern UINT32 *DCQuantScaleUV;
extern UINT32 *DCQuantScaleV1;
extern UINT32 DeringModifierV1[64];
extern UINT32 *LoopFilterLimitValuesV2;
extern UINT32 *DeblockLimitValuesV2;
extern UINT32 LoopFilterLimitValuesVp4[64];
extern UINT32 LoopFilterLimitValuesVp5[64];
extern UINT32 LoopFilterLimitValuesVp6[64];
extern UINT32 DeblockLimitValuesVp4[64];
extern UINT32 DeblockLimitValuesVp5[64];
extern UINT32 DeblockLimitValuesVp6[64];

UINT8 LimitVal_VP31[768];

void (*FilteringVert_12)(unsigned int, unsigned char *, int);
void (*FilteringHoriz_12)(unsigned int, unsigned char *, int);
void (*FilteringVert_8)(unsigned int, unsigned char *, int);
void (*FilteringHoriz_8)(unsigned int, unsigned char *, int);
void (*VerticalBand_4_5_Scale)(unsigned char *, unsigned int, unsigned int);
void (*LastVerticalBand_4_5_Scale)(unsigned char *, unsigned int, unsigned int);
void (*VerticalBand_3_5_Scale)(unsigned char *, unsigned int, unsigned int);
void (*LastVerticalBand_3_5_Scale)(unsigned char *, unsigned int, unsigned int);
void (*HorizontalLine_1_2_Scale)(const unsigned char *, unsigned int, unsigned char *, unsigned int);
void (*HorizontalLine_3_5_Scale)(const unsigned char *, unsigned int, unsigned char *, unsigned int);
void (*HorizontalLine_4_5_Scale)(const unsigned char *, unsigned int, unsigned char *, unsigned int);
void (*VerticalBand_1_2_Scale)(unsigned char *, unsigned int, unsigned int);
void (*LastVerticalBand_1_2_Scale)(unsigned char *, unsigned int, unsigned int);
void (*FilterHoriz_Simple)(POSTPROC_INSTANCE *, unsigned char *, int, int *);
void (*FilterVert_Simple)(POSTPROC_INSTANCE *, unsigned char *, int, int *);
void (*DeringBlockWeak)(POSTPROC_INSTANCE *, const unsigned char *, unsigned char *, int, unsigned int,
                        unsigned int *);
void (*DeringBlockStrong)(POSTPROC_INSTANCE *, const unsigned char *, unsigned char *, int,
                          unsigned int, unsigned int *);
void (*DeblockLoopFilteredBand)(POSTPROC_INSTANCE *, unsigned char *, unsigned char *, unsigned int,
                                unsigned int, unsigned int, unsigned int *);
void (*DeblockNonFilteredBand)(POSTPROC_INSTANCE *, unsigned char *, unsigned char *, unsigned int,
                               unsigned int, unsigned int, unsigned int *);
void (*DeblockNonFilteredBandNewFilter)(POSTPROC_INSTANCE *, unsigned char *, unsigned char *,
                                        unsigned int, unsigned int, unsigned int, unsigned int *);
int *(*SetupBoundingValueArray)(POSTPROC_INSTANCE *, int);
int *(*SetupDeblockValueArray)(POSTPROC_INSTANCE *, int);
void (*FilterHoriz)(POSTPROC_INSTANCE *, unsigned char *, int, int *);
void (*FilterVert)(POSTPROC_INSTANCE *, unsigned char *, int, int *);
void (*ClampLevels)(POSTPROC_INSTANCE *, int, int, unsigned char *, unsigned char *);
void (*FastDeInterlace)(unsigned char *, unsigned char *, int, int, int);
void (*PlaneAddNoise)(unsigned char *, unsigned int, unsigned int, int, int);

void InitPostProcessing(UINT32 *DCQuantScaleV2p, UINT32 *DCQuantScaleUVp, UINT32 *DCQuantScaleV1p,
                        UINT32 Version) {
    int i;

    for (i = 0; i < 768; i++) {
        int x = i - 256;
        if (x >= 0) {
            if (x > 255) {
                x = 255;
            }
            LimitVal_VP31[i] = x;
        } else {
            LimitVal_VP31[i] = 0;
        }
    }

    DCQuantScaleV2 = DCQuantScaleV2p;
    DCQuantScaleUV = DCQuantScaleUVp;
    DCQuantScaleV1 = DCQuantScaleV1p;

    for (i = 0; i < 64; i++) {
        DeringModifierV1[i] = DCQuantScaleV1p[i];
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

void DeletePostProcBuffers(POSTPROC_INSTANCE *ppi) {
    if (ppi->IntermediateBufferAlloc) {
        duck_free(ppi->IntermediateBufferAlloc);
    }
    ppi->IntermediateBufferAlloc = 0;
    ppi->IntermediateBuffer = 0;

    if (ppi->FiltBoundingValueAlloc) {
        duck_free(ppi->FiltBoundingValueAlloc);
    }
    ppi->FiltBoundingValueAlloc = 0;
    ppi->FiltBoundingValue = 0;

    if (ppi->DeblockBoundingValueAlloc) {
        duck_free(ppi->DeblockBoundingValueAlloc);
    }
    ppi->DeblockBoundingValueAlloc = 0;
    ppi->DeblockBoundingValue = 0;

    if (ppi->FragQIndexAlloc) {
        duck_free(ppi->FragQIndexAlloc);
    }
    ppi->FragQIndexAlloc = 0;
    ppi->FragQIndex = 0;

    if (ppi->FragmentVariancesAlloc) {
        duck_free(ppi->FragmentVariancesAlloc);
    }
    ppi->FragmentVariancesAlloc = 0;
    ppi->FragmentVariances = 0;

    if (ppi->FragDeblockingFlagAlloc) {
        duck_free(ppi->FragDeblockingFlagAlloc);
    }
    ppi->FragDeblockingFlagAlloc = 0;
    ppi->FragDeblockingFlag = 0;
}

int AllocatePostProcBuffers(POSTPROC_INSTANCE *ppi) {
    DeletePostProcBuffers(ppi);

    ppi->IntermediateBufferAlloc = duck_malloc(
        (ppi->YStride * (ppi->Configuration.VideoFrameHeight + ppi->MVBorder * 2) * 3) / 2 + 32, 0);
    if (ppi->IntermediateBufferAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->IntermediateBuffer = (UINT8 *)(((UINT32)ppi->IntermediateBufferAlloc + 31) & ~31);

    ppi->FiltBoundingValueAlloc = duck_malloc(256 * 8 + 32, 0);
    if (ppi->FiltBoundingValueAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FiltBoundingValue = (int *)(((UINT32)ppi->FiltBoundingValueAlloc + 31) & ~31);

    ppi->DeblockBoundingValueAlloc = duck_malloc(256 * 8 + 32, 0);
    if (ppi->DeblockBoundingValueAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->DeblockBoundingValue = (int *)(((UINT32)ppi->DeblockBoundingValueAlloc + 31) & ~31);

    ppi->FragQIndexAlloc = duck_malloc(ppi->UnitFragments * sizeof(int) + 32, 0);
    if (ppi->FragQIndexAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragQIndex = (int *)(((UINT32)ppi->FragQIndexAlloc + 31) & ~31);

    ppi->FragmentVariancesAlloc = duck_malloc(ppi->UnitFragments * sizeof(int) + 32, 0);
    if (ppi->FragmentVariancesAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragmentVariances = (int *)(((UINT32)ppi->FragmentVariancesAlloc + 31) & ~31);

    ppi->FragDeblockingFlagAlloc = duck_malloc(ppi->UnitFragments + 32, 0);
    if (ppi->FragDeblockingFlagAlloc == 0) {
        DeletePostProcBuffers(ppi);
        return 0;
    }
    ppi->FragDeblockingFlag = (UINT8 *)(((UINT32)ppi->FragDeblockingFlagAlloc + 31) & ~31);

    return 1;
}

void ChangePostProcConfiguration(POSTPROC_INSTANCE *ppi, CONFIG_TYPE *ConfigurationInit) {
    memcpy((void *)&ppi->Configuration, (void *)ConfigurationInit, sizeof(CONFIG_TYPE));

    ppi->HFragments = ppi->Configuration.VideoFrameWidth >> 3;
    ppi->VFragments = ppi->Configuration.VideoFrameHeight >> 3;
    ppi->YStride = ppi->Configuration.YStride;
    ppi->UVStride = ppi->Configuration.UVStride;
    ppi->YPlaneFragments = ppi->HFragments * ppi->VFragments;
    ppi->UVPlaneFragments = ppi->YPlaneFragments / 4;
    ppi->UnitFragments = ppi->YPlaneFragments + 2 * ppi->UVPlaneFragments;
    ppi->MVBorder = (ppi->YStride - 8 * ppi->HFragments) / 2;

    ppi->ReconYDataOffset = ppi->MVBorder * ppi->YStride + ppi->MVBorder;
    ppi->ReconYDataOffset = ppi->MVBorder * ppi->YStride + ppi->MVBorder;
    ppi->ReconUDataOffset =
        (ppi->YStride * (ppi->Configuration.VideoFrameHeight + ppi->MVBorder * 2)) +
        ppi->MVBorder / 2 * ppi->UVStride + ppi->MVBorder / 2;
    ppi->ReconVDataOffset =
        (ppi->YStride * (ppi->Configuration.VideoFrameHeight + ppi->MVBorder * 2)) +
        (ppi->UVStride * (ppi->Configuration.VideoFrameHeight / 2 + ppi->MVBorder)) +
        ppi->MVBorder / 2 * ppi->UVStride + ppi->MVBorder / 2;
}

POSTPROC_INSTANCE *CreatePostProcInstance(CONFIG_TYPE *ConfigurationInit) {
    POSTPROC_INSTANCE *ppi;

    ppi = duck_malloc(sizeof(POSTPROC_INSTANCE), 0);
    if (ppi) {
        memset(ppi, 0, sizeof(POSTPROC_INSTANCE));
        ChangePostProcConfiguration(ppi, ConfigurationInit);
        ppi->AddNoiseMode = 1;
        return ppi;
    }

    return 0;
}

void DeletePostProcInstance(POSTPROC_INSTANCE **ppi) {
    if (*ppi) {
        DeletePostProcBuffers(*ppi);
        duck_free(*ppi);
        *ppi = 0;
    }
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

void UpdateFragQIndex(POSTPROC_INSTANCE *ppi) {
    unsigned int i;
    int FrameQIndex = ppi->FrameQIndex;

    for (i = 0; i < ppi->UnitFragments; i++) {
        if (ppi->FragInfo[i * ppi->FragInfoElementSize] & ppi->FragInfoCodedMask) {
            ppi->FragQIndex[i] = FrameQIndex;
        }
    }
}

double gaussian(double sigma, double mu, double x) {
    return 1 / (sigma * sqrt(6.2831853)) * (exp(-(x - mu) * (x - mu) / (2 * sigma * sigma)));
}

void PlaneAddNoise_C(UINT8 *Start, UINT32 Width, UINT32 Height, int Pitch, int q) {
    unsigned int i, j;
    char BlackClamp[16];
    char WhiteClamp[16];
    char BothClamp[16];
    char CharDist[300];
    char Rand[2048];
    double sigma;

    sigma = 1 + .8 * (63 - q) / 63.0;

    {
        double i;
        int next, j;

        next = 0;

        for (i = -32; i < 32; i++) {
            int a = (int)(.5 + 256 * gaussian(sigma, 0, i));

            if (a) {
                for (j = 0; j < a; j++) {
                    CharDist[next + j] = (char)i;
                }
                next = next + j;
            }
        }

        for (next = next; next < 256; next++) {
            CharDist[next] = 0;
        }
    }

    for (i = 0; i < 2048; i++) {
        Rand[i] = CharDist[rand() & 0xff];
    }

    for (i = 0; i < 16; i++) {
        BlackClamp[i] = -CharDist[0];
        WhiteClamp[i] = -CharDist[0];
        BothClamp[i] = -2 * CharDist[0];
    }

    for (i = 0; i < Height; i++) {
        UINT8 *Pos = Start + i * Pitch;
        char *Ref = &Rand[rand() & 0xff];

        for (j = 0; j < Width; j++) {
            if (Pos[j] < -CharDist[0]) {
                Pos[j] = -CharDist[0];
            }

            if (Pos[j] > 255 - CharDist[0]) {
                Pos[j] = 255 - CharDist[0];
            }

            Pos[j] += Ref[j];
        }
    }
}

void PostProcess(POSTPROC_INSTANCE *ppi, INT32 Vp3VersionNo, INT32 FrameType,
                 INT32 PostProcessingLevel, INT32 FrameQIndex, UINT8 *LastFrameRecon,
                 UINT8 *PostProcessBuffer, UINT8 *FragInfo, UINT32 FragInfoElementSize,
                 UINT32 FragInfoCodedMask) {
    UINT32 PlaneSize;

    ppi->Vp3VersionNo = Vp3VersionNo;
    ppi->FrameType = FrameType;
    ppi->PostProcessingLevel = PostProcessingLevel;
    ppi->FrameQIndex = FrameQIndex;
    ppi->LastFrameRecon = LastFrameRecon;
    ppi->PostProcessBuffer = PostProcessBuffer;
    ppi->FragInfo = FragInfo;
    ppi->FragInfoElementSize = FragInfoElementSize;
    ppi->FragInfoCodedMask = FragInfoCodedMask;

    switch (ppi->PostProcessingLevel) {
    case 8:
        UpdateFragQIndex(ppi);

        if (ppi->Vp3VersionNo > 1) {
            if (ppi->Configuration.Interlaced && ppi->DeInterlaceMode) {
                SimpleDeblockFrame(ppi, ppi->LastFrameRecon, ppi->IntermediateBuffer);

                PlaneSize = ppi->VFragments * (ppi->YStride + ppi->YStride);
                memcpy(ppi->PostProcessBuffer + ppi->ReconUDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconUDataOffset, PlaneSize);
                memcpy(ppi->PostProcessBuffer + ppi->ReconVDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconVDataOffset, PlaneSize);

                FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                                ppi->PostProcessBuffer + ppi->ReconYDataOffset,
                                ppi->HFragments * 8, ppi->VFragments * 8, ppi->YStride);
            } else {
                SimpleDeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
            }
        } else {
            DeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
        }
        break;

    case 5:
    case 6:
        if (ppi->Vp3VersionNo <= 4) {
            UpdateFragQIndex(ppi);
        } else if (ppi->Configuration.Interlaced) {
            if (ppi->DeInterlaceMode == 0) {
                DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
                UpdateUMVBorder(ppi, ppi->PostProcessBuffer);
                DeringFrameInterlaced(ppi, ppi->PostProcessBuffer, ppi->PostProcessBuffer);
            } else {
                DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->IntermediateBuffer);
                UpdateUMVBorder(ppi, ppi->IntermediateBuffer);
                DeringFrameInterlaced(ppi, ppi->IntermediateBuffer, ppi->IntermediateBuffer);

                PlaneSize = ppi->VFragments * (ppi->YStride + ppi->YStride);
                memcpy(ppi->PostProcessBuffer + ppi->ReconUDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconUDataOffset, PlaneSize);
                memcpy(ppi->PostProcessBuffer + ppi->ReconVDataOffset,
                       ppi->IntermediateBuffer + ppi->ReconVDataOffset, PlaneSize);

                FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                                ppi->PostProcessBuffer + ppi->ReconYDataOffset,
                                ppi->HFragments * 8, ppi->VFragments * 8, ppi->YStride);
            }
            break;
        }

        DeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
        UpdateUMVBorder(ppi, ppi->PostProcessBuffer);
        DeringFrame(ppi, ppi->PostProcessBuffer, ppi->PostProcessBuffer);

        if (ppi->AddNoiseMode) {
            PlaneAddNoise(ppi->PostProcessBuffer + ppi->ReconYDataOffset, ppi->HFragments * 8,
                          ppi->VFragments * 8, ppi->YStride, FrameQIndex);
        }
        break;

    case 7:
        if (ppi->Vp3VersionNo > 4) {
            if (ppi->Configuration.Interlaced) {
                if (ppi->DeInterlaceMode) {
                    DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->IntermediateBuffer);

                    PlaneSize = ppi->VFragments * (ppi->YStride + ppi->YStride);
                    memcpy(ppi->PostProcessBuffer + ppi->ReconUDataOffset,
                           ppi->IntermediateBuffer + ppi->ReconUDataOffset, PlaneSize);
                    memcpy(ppi->PostProcessBuffer + ppi->ReconVDataOffset,
                           ppi->IntermediateBuffer + ppi->ReconVDataOffset, PlaneSize);

                    FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                                    ppi->PostProcessBuffer + ppi->ReconYDataOffset,
                                    ppi->HFragments * 8, ppi->VFragments * 8, ppi->YStride);
                } else {
                    DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
                }
                break;
            }
        } else {
            UpdateFragQIndex(ppi);
        }

        DeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
        UpdateUMVBorder(ppi, ppi->PostProcessBuffer);
        DeringFrame(ppi, ppi->PostProcessBuffer, ppi->PostProcessBuffer);
        break;

    case 4:
        if (ppi->Vp3VersionNo > 4) {
            if (ppi->Configuration.Interlaced) {
                if (ppi->DeInterlaceMode == 0) {
                    DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
                } else {
                    DeblockFrameInterlaced(ppi, ppi->LastFrameRecon, ppi->IntermediateBuffer);

                    PlaneSize = ppi->VFragments * (ppi->YStride + ppi->YStride);
                    memcpy(ppi->PostProcessBuffer + ppi->ReconUDataOffset,
                           ppi->IntermediateBuffer + ppi->ReconUDataOffset, PlaneSize);
                    memcpy(ppi->PostProcessBuffer + ppi->ReconVDataOffset,
                           ppi->IntermediateBuffer + ppi->ReconVDataOffset, PlaneSize);

                    FastDeInterlace(ppi->IntermediateBuffer + ppi->ReconYDataOffset,
                                    ppi->PostProcessBuffer + ppi->ReconYDataOffset,
                                    ppi->HFragments * 8, ppi->VFragments * 8, ppi->YStride);
                }
                break;
            }
        } else {
            UpdateFragQIndex(ppi);
        }

        DeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
        break;

    case 1:
        UpdateFragQIndex(ppi);
        break;

    case 0:
        if (ppi->Configuration.Interlaced && ppi->DeInterlaceMode) {
            PlaneSize = ppi->VFragments * (ppi->YStride + ppi->YStride);
            memcpy(ppi->PostProcessBuffer + ppi->ReconUDataOffset,
                   ppi->LastFrameRecon + ppi->ReconUDataOffset, PlaneSize);
            memcpy(ppi->PostProcessBuffer + ppi->ReconVDataOffset,
                   ppi->LastFrameRecon + ppi->ReconVDataOffset, PlaneSize);

            FastDeInterlace(ppi->LastFrameRecon + ppi->ReconYDataOffset,
                            ppi->PostProcessBuffer + ppi->ReconYDataOffset,
                            ppi->HFragments * 8, ppi->VFragments * 8, ppi->YStride);
        }
        break;

    default:
        DeblockFrame(ppi, ppi->LastFrameRecon, ppi->PostProcessBuffer);
        UpdateUMVBorder(ppi, ppi->PostProcessBuffer);
        DeringFrame(ppi, ppi->PostProcessBuffer, ppi->PostProcessBuffer);
        break;
    }
}
