#include "../../../include/vp6_pbdll.h"
#include <stdio.h>

typedef struct {
    unsigned int FrameQIndex;
    unsigned int ThisFrameQuantizerValue;
    short round[8];
    short mult[8];
    short zbin[8];
    unsigned int LastQuantizerValue;
    unsigned int QThreshTable[64];
    unsigned int *transIndex;
    unsigned char quant_index[64];
    short *dequant_coeffs[2];
    short *dequant_coeffsAlloc[2];
    int QuantCoeffs[64][2];
    int QuantRound[64][2];
    int ZeroBinSize[64][2];
    int ZlrZbinCorrections[64][2];
} QUANTIZER;

typedef struct {
    int YWidth;
    int YHeight;
    int YStride;
    int UVWidth;
    int UVHeight;
    int UVStride;
    char *YBuffer;
    char *UBuffer;
    char *VBuffer;
    char *YBufferStart;
} YUV_BUFFER_CONFIG;

typedef enum {
    PBC_SET_POSTPROC = 0,
    PBC_SET_CPUFREE = 1,
    PBC_MAX_PARAM = 2,
    PBC_SET_TESTMODE = 3,
    PBC_SET_PBSTRUCT = 4,
    PBC_SET_BLACKCLAMP = 5,
    PBC_SET_WHITECLAMP = 6,
    PBC_SET_REFERENCEFRAME = 7,
    PBC_SET_DEINTERLACEMODE = 8,
    PBC_SET_ADDNOISE = 9
} PB_COMMAND_TYPE;

extern struct PB_INSTANCE *VP6_CreatePBInstance(void);
extern void *duck_malloc(unsigned int blocksize, int type);
extern void VP6_DeletePBInstance(struct PB_INSTANCE **pbi);
extern int VP6_InitFrameDetails(struct PB_INSTANCE *pbi);
extern void VP6_InitialiseConfiguration(struct PB_INSTANCE *pbi);
extern void VP6_DeleteFragmentInfo(struct PB_INSTANCE *pbi);
extern void VP6_DeleteFrameInfo(struct PB_INSTANCE *pbi);
extern QUANTIZER *VP6_CreateQuantizer(void);
extern void VP6_DeleteQuantizer(QUANTIZER **qi);
extern struct POSTPROC_INSTANCE *CreatePostProcInstance(struct CONFIG_TYPE *ConfigurationInit);
extern void DeletePostProcInstance(struct POSTPROC_INSTANCE **ppi);
extern void SetAddNoiseMode(struct POSTPROC_INSTANCE *ppi, int AddNoiseMode);
extern void CopyFrame(struct POSTPROC_INSTANCE *ppi, YUV_BUFFER_CONFIG *b,
                      unsigned char *DestReconPtr);
extern int AllocatePostProcBuffers(struct POSTPROC_INSTANCE *ppi);
extern void PostProcess(struct POSTPROC_INSTANCE *ppi, int Vp3VersionNo,
                        int FrameType, int PostProcessingLevel, int FrameQIndex,
                        unsigned char *LastFrameRecon,
                        unsigned char *PostProcessBuffer,
                        unsigned char *FragInfo,
                        unsigned int FragInfoElementSize,
                        unsigned int FragInfoCodedMask);
extern void (*ClampLevels)(struct POSTPROC_INSTANCE *ppi, int BlackClamp,
                           int WhiteClamp, unsigned char *Src,
                           unsigned char *Dst);
extern void ScaleOrCenter(struct POSTPROC_INSTANCE *ppi,
                          unsigned char *FrameBuffer, YUV_BUFFER_CONFIG *YuvConfig);
extern void InitHeaderBuffer(struct FRAME_HEADER *Header, unsigned char *Buffer);
extern int VP6_LoadFrame(struct PB_INSTANCE *pbi);
extern void VP6_StartDecode(void *br, unsigned char *source);
extern int VP6_DecodeFrameMbs(struct PB_INSTANCE *pbi);
extern void UpdateUMVBorder(struct POSTPROC_INSTANCE *ppi);
extern unsigned int CPUFrequency;

int CurrentFrame;

static inline void OSInitFastCast(void) {
    __asm__("li 3, 4\n"
            "oris 3, 3, 4\n"
            "mtspr 914, 3\n"
            "li 3, 5\n"
            "oris 3, 3, 5\n"
            "mtspr 915, 3\n"
            "li 3, 6\n"
            "oris 3, 3, 6\n"
            "mtspr 916, 3\n"
            "li 3, 7\n"
            "oris 3, 3, 7\n"
            "mtspr 917, 3");
}

static inline void OSSetGQR6(unsigned int type, unsigned int scale) {
    unsigned int val;

    val = type | (scale << 8);
    val |= val << 16;
    __asm__("mtspr 918, %0" : : "r"(val) : "r0");
}

int VP6_StartDecoder(struct PB_INSTANCE **pbi, unsigned int ImageWidth,
                     unsigned int ImageHeight) {
    *pbi = VP6_CreatePBInstance();
    (*pbi)->ScaleWidth = ImageWidth;
    (*pbi)->ScaleHeight = ImageHeight;
    (*pbi)->OutputWidth = ImageWidth;
    (*pbi)->OutputHeight = ImageHeight;
    (*pbi)->Configuration.VideoFrameWidth = ImageWidth;
    (*pbi)->Configuration.VideoFrameHeight = ImageHeight;
    (*pbi)->postproc = CreatePostProcInstance(&(*pbi)->Configuration);
    (*pbi)->quantizer = VP6_CreateQuantizer();
    (*pbi)->ProcessorFrequency = CPUFrequency;

    if (VP6_InitFrameDetails(*pbi) == 0) {
        VP6_DeletePBInstance(pbi);
        return 0;
    }

    (*pbi)->quantizer->LastQuantizerValue = 0;
    VP6_InitialiseConfiguration(*pbi);
    return 1;
}

void VP6_SetPbParam(struct PB_INSTANCE *pbi, PB_COMMAND_TYPE Command,
                    unsigned int Parameter) {
    {
        double PixelsPerMhz;
    }

    switch (Command) {
    case 1:
        pbi->CPUFree = Parameter;
        pbi->PostProcessingLevel = 0;
        break;
    case 9:
        pbi->AddNoiseMode = Parameter;
        SetAddNoiseMode(pbi->postproc, Parameter);
        break;
    case 7:
        CopyFrame(pbi->postproc, (YUV_BUFFER_CONFIG *)Parameter,
                  pbi->LastFrameRecon);
        CopyFrame(pbi->postproc, (YUV_BUFFER_CONFIG *)Parameter,
                  pbi->GoldenFrame);
        break;
    case 0:
        if (Parameter == 9) {
            VP6_SetPbParam(pbi, 1, 70);
        } else {
            pbi->PostProcessingLevel = Parameter;
            pbi->CPUFree = 0;
        }
        break;
    case 8:
        pbi->DeInterlaceMode = Parameter;
        break;
    case 5:
        pbi->BlackClamp = Parameter;
        break;
    case 6:
        pbi->WhiteClamp = Parameter;
        break;
    default:
        break;
    }
}

void VP6_GetYUVConfig(struct PB_INSTANCE *pbi, YUV_BUFFER_CONFIG *YuvConfig) {
    {
    }

    if (pbi->PostProcessingLevel != 0 ||
        (pbi->Configuration.Interlaced != 0 && pbi->DeInterlaceMode != 0)) {
        if (pbi->PostProcessBuffer == 0) {
            pbi->PostProcessBufferAlloc = (unsigned char *)duck_malloc(
                (pbi->Configuration.YStride + 32) +
                    (pbi->ReconYPlaneSize +
                     (pbi->ReconUVPlaneSize + pbi->ReconUVPlaneSize)), 0);
            pbi->PostProcessBuffer = (unsigned char *)
                (((unsigned int)pbi->PostProcessBufferAlloc + 31) & ~31);
            AllocatePostProcBuffers(pbi->postproc);
        }
        PostProcess(pbi->postproc, pbi->Vp3VersionNo, pbi->FrameType,
                    pbi->PostProcessingLevel, pbi->AvgFrameQIndex,
                    pbi->LastFrameRecon, pbi->PostProcessBuffer,
                    (unsigned char *)pbi->FragInfo, 4, 1);
        if (pbi->BlackClamp != 0) {
            ClampLevels(pbi->postproc, pbi->BlackClamp, pbi->WhiteClamp,
                        pbi->PostProcessBuffer, pbi->PostProcessBuffer);
        }
    }

    if (pbi->Configuration.VideoFrameWidth <
            (pbi->OutputWidth + (pbi->Configuration.VideoFrameWidth * 0)) ||
        pbi->Configuration.VideoFrameHeight <
            (pbi->OutputHeight + (pbi->Configuration.VideoFrameHeight * 0))) {
        YuvConfig->YWidth = pbi->OutputWidth + 32;
        YuvConfig->YHeight = pbi->OutputHeight + 32;
        YuvConfig->YStride = YuvConfig->YWidth;
        YuvConfig->UVWidth = YuvConfig->YWidth / 2;
        YuvConfig->UVStride = YuvConfig->UVWidth;
        YuvConfig->UVHeight = YuvConfig->YHeight / 2;
        YuvConfig->YBuffer = (char *)pbi->ScaleBuffer;
        YuvConfig->UBuffer = YuvConfig->YBuffer +
                             YuvConfig->YStride * YuvConfig->YHeight;
        YuvConfig->VBuffer = YuvConfig->UBuffer +
                             YuvConfig->UVStride * YuvConfig->UVHeight;
        YuvConfig->YBufferStart = YuvConfig->YBuffer;
        if (pbi->PostProcessingLevel != 0) {
            ScaleOrCenter(pbi->postproc, pbi->PostProcessBuffer, YuvConfig);
        } else {
            ScaleOrCenter(pbi->postproc, pbi->LastFrameRecon, YuvConfig);
        }

        YuvConfig->YBuffer +=
            ((YuvConfig->YHeight - pbi->OutputHeight) >> 1) *
                YuvConfig->YStride +
            ((YuvConfig->YWidth - pbi->OutputWidth) >> 1);
        YuvConfig->YWidth = pbi->OutputWidth;
        YuvConfig->YHeight = pbi->OutputHeight;
        YuvConfig->UBuffer +=
            ((YuvConfig->UVHeight - pbi->OutputHeight / 2) >> 1) *
                YuvConfig->UVStride +
            ((YuvConfig->UVWidth - pbi->OutputWidth / 2) >> 1);
        YuvConfig->VBuffer +=
            ((YuvConfig->UVHeight - pbi->OutputHeight / 2) >> 1) *
                YuvConfig->UVStride +
            ((YuvConfig->UVWidth - pbi->OutputWidth / 2) >> 1);
        YuvConfig->UVWidth = pbi->OutputWidth / 2;
        YuvConfig->UVHeight = pbi->OutputHeight / 2;
    } else {
        YuvConfig->YWidth = pbi->Configuration.VideoFrameWidth;
        YuvConfig->YHeight = pbi->Configuration.VideoFrameHeight;
        YuvConfig->YStride = pbi->Configuration.YStride;
        YuvConfig->UVWidth = pbi->Configuration.VideoFrameWidth / 2;
        YuvConfig->UVHeight = pbi->Configuration.VideoFrameHeight / 2;
        YuvConfig->UVStride = pbi->Configuration.UVStride;

        if (pbi->PostProcessingLevel != 0 ||
            (pbi->Configuration.Interlaced != 0 && pbi->DeInterlaceMode != 0)) {
            YuvConfig->YBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconYDataOffset +
                                  pbi->Configuration.YStride * 48 + 48);
            YuvConfig->UBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconUDataOffset +
                                  pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->VBuffer = (char *)pbi->PostProcessBuffer +
                                 (pbi->ReconVDataOffset +
                                  pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->YBufferStart = (char *)pbi->PostProcessBuffer +
                                      pbi->ReconYDataOffset;
        } else {
            YuvConfig->YBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconYDataOffset +
                                  pbi->Configuration.YStride * 48 + 48);
            YuvConfig->UBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconUDataOffset +
                                  pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->VBuffer = (char *)pbi->LastFrameRecon +
                                 (pbi->ReconVDataOffset +
                                  pbi->Configuration.UVStride * 24 + 24);
            YuvConfig->YBufferStart = (char *)pbi->LastFrameRecon +
                                      pbi->ReconYDataOffset;
        }
    }
}

static int VP6_DecodeFrameToYUV_internal(struct PB_INSTANCE *pbi,
                                         char *VideoBufferPtr,
                                         unsigned int ByteCount,
                                         unsigned int ImageWidth,
                                         unsigned int ImageHeight) {
    unsigned char *tmp;

    {
        struct __sFILE *f;
    }

    pbi->CurrentFrameSize = ByteCount;
    InitHeaderBuffer(&pbi->Header, (unsigned char *)VideoBufferPtr);
    if (VP6_LoadFrame(pbi) == 0) {
        return -1;
    }
    if (pbi->MultiStream == 0) {
        if (pbi->VpProfile == 0) {
            if (pbi->UseHuffman != 0) {
                pbi->br3.bitsinremainder = 0;
                pbi->br3.remainder = 0;
                pbi->br3.position = (unsigned char *)VideoBufferPtr +
                                    pbi->Buff2Offset;
            } else {
                VP6_StartDecode(&pbi->br2,
                                (unsigned char *)VideoBufferPtr + pbi->Buff2Offset);
            }
        }
    }
    VP6_DecodeFrameMbs(pbi);

    tmp = pbi->OtherFrameRecon != 0 ? pbi->OtherFrameRecon :
          pbi->LastFrameRecon;
    pbi->LastFrameRecon = pbi->ThisFrameRecon;
    pbi->OtherFrameRecon = 0;
    pbi->ThisFrameRecon = tmp;
    UpdateUMVBorder(pbi->postproc);

    if (pbi->FrameType != 0 && pbi->RefreshGoldenFrame != 0) {
        pbi->OtherFrameRecon = pbi->GoldenFrame;
        pbi->GoldenFrame = pbi->LastFrameRecon;
    }

    if (pbi->FrameType == 0) {
        pbi->AvgFrameQIndex = pbi->quantizer->QThreshTable[0];
    } else {
        pbi->AvgFrameQIndex = (pbi->AvgFrameQIndex * 3 +
                               pbi->quantizer->QThreshTable[0] + 2) >> 2;
    }

    if (pbi->br.pos > pbi->CurrentFrameSize) {
        struct __sFILE *f;

        f = fopen("badframes.stt", "a");
        fprintf(f, "%8d %8d %8d \n", CurrentFrame,
                pbi->br.pos, pbi->CurrentFrameSize);
        fclose(f);
    }
    CurrentFrame++;
    return 0;
}

int VP6_DecodeFrameToYUV(struct PB_INSTANCE *pbi, char *VideoBufferPtr,
                         unsigned int ByteCount, unsigned int ImageWidth,
                         unsigned int ImageHeight) {
    int result;
    unsigned int _gqrStates[5];

    __asm__ volatile("mfspr 9, 914\n"
                     "mfspr 11, 915\n"
                     "mfspr 10, 916\n"
                     "mfspr 8, 917\n"
                     "mfspr 30, 918\n"
                     "mr 0, 3\n"
                     "stw 9, 8(1)\n"
                     "stw 11, 12(1)\n"
                     "stw 10, 16(1)\n"
                     "stw 8, 20(1)\n"
                     "stw 30, 24(1)"
                     : : : "r8", "r9", "r10", "r11", "r30");
    OSInitFastCast();
    OSSetGQR6(5, 0x39);
    __asm__("mr 3, 0");
    result = VP6_DecodeFrameToYUV_internal(pbi, VideoBufferPtr, ByteCount,
                                           ImageWidth, ImageHeight);
    __asm__ volatile("lwz 9, 24(1)\n"
                     "mtspr 918, 9\n"
                     "lwz 11, 20(1)\n"
                     "mtspr 917, 11\n"
                     "lwz 9, 16(1)\n"
                     "mtspr 916, 9\n"
                     "lwz 11, 12(1)\n"
                     "mtspr 915, 11\n"
                     "lwz 9, 8(1)\n"
                     "mtspr 914, 9"
                     : : : "r9", "r11");
    return result;
}

int VP6_StopDecoder(struct PB_INSTANCE **pbi) {
    if (*pbi != 0) {
        VP6_DeleteQuantizer((QUANTIZER **)&(*pbi)->quantizer);
        DeletePostProcInstance(&(*pbi)->postproc);
        VP6_DeleteFragmentInfo(*pbi);
        VP6_DeleteFrameInfo(*pbi);
        VP6_DeletePBInstance(pbi);
    }
    return 1;
}
