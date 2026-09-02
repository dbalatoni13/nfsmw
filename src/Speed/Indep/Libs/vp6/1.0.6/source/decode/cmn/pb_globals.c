#include "../../../include/vp6_pbdll.h"

enum tmemtype {
    DMEM_GENERAL = 0,
    DMEM_TEMP = 1,
    DMEM_CDBUFF = 2,
    DMEM_FRAMEBUFF = 3
};

extern void *duck_malloc(unsigned int blocksize, enum tmemtype type);
extern void duck_free(void *block);
extern void *memset(void *dest, int value, unsigned int size);
extern void *memcpy(void *dest, const void *source, unsigned int size);

extern void VP6_DeleteQuantizer(void **quantizer);
extern void DeletePostProcInstance(struct POSTPROC_INSTANCE **postproc);
extern void InitPostProcessing(unsigned int *dcQuantScaleV2, unsigned int *dcQuantScaleUv,
                               unsigned int *dcQuantScaleV1, unsigned int version);
extern void VP6_DMachineSpecificConfig(void);
extern unsigned int VP6_GetProcessorFrequency(void);
extern void InitVPUtil(void);

extern short VP6_DcQuant[64];
extern void (*idct[65])(short *, short *, short *);

unsigned int CPUFrequency;
static unsigned int VP6_DCQuantScaleP[64];

void VP6_DeleteTmpBuffers(struct PB_INSTANCE *pbi) {
    if (pbi->ReconDataBufferAlloc != 0) {
        duck_free(pbi->ReconDataBufferAlloc);
    }
    if (pbi->LoopFilteredBlockAlloc != 0) {
        duck_free(pbi->LoopFilteredBlockAlloc);
    }
    if (pbi->TmpDataBufferAlloc != 0) {
        duck_free(pbi->TmpDataBufferAlloc);
    }
    if (pbi->TmpReconBufferAlloc != 0) {
        duck_free(pbi->TmpReconBufferAlloc);
    }
    if (pbi->ScaleBufferAlloc != 0) {
        duck_free(pbi->ScaleBufferAlloc);
    }

    pbi->ReconDataBufferAlloc = 0;
    pbi->LoopFilteredBlockAlloc = 0;
    pbi->TmpDataBufferAlloc = 0;
    pbi->TmpReconBufferAlloc = 0;
    pbi->ScaleBufferAlloc = 0;
    pbi->ReconDataBuffer = 0;
    pbi->LoopFilteredBlock = 0;
    pbi->TmpDataBuffer = 0;
    pbi->TmpReconBuffer = 0;
    pbi->ScaleBuffer = 0;
}

int VP6_AllocateTmpBuffers(struct PB_INSTANCE *pbi) {
    VP6_DeleteTmpBuffers(pbi);

    pbi->ReconDataBufferAlloc = (short *)duck_malloc(0xa0, DMEM_GENERAL);
    if (pbi->ReconDataBufferAlloc == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->ReconDataBuffer = (short *)(((unsigned int)pbi->ReconDataBufferAlloc + 0x1f) & ~0x1f);

    pbi->TmpDataBufferAlloc = (short *)duck_malloc(0xa0, DMEM_GENERAL);
    if (pbi->TmpDataBufferAlloc == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->TmpDataBuffer = (short *)(((unsigned int)pbi->TmpDataBufferAlloc + 0x1f) & ~0x1f);

    pbi->LoopFilteredBlockAlloc = (unsigned char *)duck_malloc(0x120, DMEM_GENERAL);
    if (pbi->LoopFilteredBlockAlloc == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->LoopFilteredBlock = (unsigned char *)(((unsigned int)pbi->LoopFilteredBlockAlloc + 0x1f) & ~0x1f);

    pbi->TmpReconBufferAlloc = (short *)duck_malloc(0xa0, DMEM_GENERAL);
    if (pbi->TmpReconBufferAlloc == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->TmpReconBuffer = (short *)(((unsigned int)pbi->TmpReconBufferAlloc + 0x1f) & ~0x1f);

    return 1;
}

void VP6_DeletePBInstance(struct PB_INSTANCE **pbi) {
    if (*pbi != 0) {
        VP6_DeleteTmpBuffers(*pbi);
        VP6_DeleteQuantizer((void **)&(*pbi)->quantizer);
        DeletePostProcInstance(&(*pbi)->postproc);
    }

    duck_free(*pbi);
    *pbi = 0;
}

struct PB_INSTANCE *VP6_CreatePBInstance(void) {
    struct PB_INSTANCE *pbi = 0;
    struct CONFIG_TYPE ConfigurationInit = {0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0};
    int pbi_size = sizeof(struct PB_INSTANCE);

    pbi = (struct PB_INSTANCE *)duck_malloc(pbi_size, DMEM_GENERAL);
    if (pbi == 0) {
        return 0;
    }

    memset((unsigned char *)pbi, 0, pbi_size);
    memcpy((void *)&pbi->Configuration, (void *)&ConfigurationInit, sizeof(struct CONFIG_TYPE));

    if (VP6_AllocateTmpBuffers(pbi) == 0) {
        duck_free(pbi);
        return 0;
    }

    pbi->CPUFree = 70;
    pbi->idct = idct;
    memset(pbi->DcProbs, 0, sizeof(pbi->DcProbs));
    memset(pbi->AcProbs, 0, sizeof(pbi->AcProbs));

    return pbi;
}

void VP6_VPInitLibrary(void) {
    int i;

    CPUFrequency = VP6_GetProcessorFrequency();
    VP6_DMachineSpecificConfig();

    i = 64;
    do {
        int dcScale = VP6_DcQuant[64 - i] / 2 + 2;
        VP6_DCQuantScaleP[64 - i] = dcScale;
    } while (--i);

    InitPostProcessing(VP6_DCQuantScaleP, VP6_DCQuantScaleP, VP6_DCQuantScaleP, 7);
    InitVPUtil();
}

void VP6_VPDeInitLibrary(void) {}
