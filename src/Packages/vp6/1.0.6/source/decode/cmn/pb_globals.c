extern void *duck_malloc(unsigned long size, void *user);
extern void duck_free(void *mem);
extern void *memset();
extern int VP6_GetProcessorFrequency(void);
extern void VP6_DMachineSpecificConfig(void);
extern void VP6_DeleteQuantizer(void *quantizer);
extern void DeletePostProcInstance(void *ppi);
extern void InitPostProcessing(int *dcScale, int *dcScale2, int *dcScale3, int five);
extern void InitVPUtil(void);

extern short VP6_DcQuant[64];
extern void (*idct[65])(void);

typedef struct {
    unsigned char pad0[0x264];
    void *field_264;
    void *field_268;
    unsigned char pad1[0x274 - 0x26c];
    void *field_274;
    void *field_278;
    unsigned char pad2[4];
    void *field_280;
    void *field_284;
    void *field_288;
    void *field_28c;
    void *field_290;
    void *field_294;
    void *field_298;
} PB_INSTANCE;

unsigned int VP6_ModeUsesMC[10] = {
    0, 0, 1, 1, 1, 0, 1, 1, 1, 1,
};

static int VP6_DCQuantScaleP[64];
int CPUFrequency;

typedef struct {
    unsigned int a, b, c, d;
    unsigned int e, f;
    unsigned int g, h, i, j, k, l, m, n;
} FRAG_CONFIG;

void VP6_DeleteTmpBuffers(PB_INSTANCE *pbi) {
    if (pbi->field_278 != 0) {
        duck_free(pbi->field_278);
    }
    if (pbi->field_290 != 0) {
        duck_free(pbi->field_290);
    }
    if (pbi->field_28c != 0) {
        duck_free(pbi->field_28c);
    }
    if (pbi->field_284 != 0) {
        duck_free(pbi->field_284);
    }
    if (pbi->field_268 != 0) {
        duck_free(pbi->field_268);
    }

    pbi->field_278 = 0;
    pbi->field_290 = 0;
    pbi->field_28c = 0;
    pbi->field_284 = 0;
    pbi->field_268 = 0;
    pbi->field_274 = 0;
    pbi->field_294 = 0;
    pbi->field_288 = 0;
    pbi->field_280 = 0;
    pbi->field_264 = 0;
}

int VP6_AllocateTmpBuffers(PB_INSTANCE *pbi) {
    VP6_DeleteTmpBuffers(pbi);

    pbi->field_278 = duck_malloc(0xa0, 0);
    if (pbi->field_278 == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->field_274 = (void *)(((unsigned long)pbi->field_278 + 31) & ~(unsigned long)31);

    pbi->field_28c = duck_malloc(0xa0, 0);
    if (pbi->field_28c == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->field_288 = (void *)(((unsigned long)pbi->field_28c + 31) & ~(unsigned long)31);

    pbi->field_290 = duck_malloc(0x120, 0);
    if (pbi->field_290 == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->field_294 = (void *)(((unsigned long)pbi->field_290 + 31) & ~(unsigned long)31);

    pbi->field_284 = duck_malloc(0xa0, 0);
    if (pbi->field_284 == 0) {
        VP6_DeleteTmpBuffers(pbi);
        return 0;
    }
    pbi->field_280 = (void *)(((unsigned long)pbi->field_284 + 31) & ~(unsigned long)31);

    return 1;
}

void VP6_DeletePBInstance(PB_INSTANCE **ppbi) {
    if (*ppbi != 0) {
        VP6_DeleteTmpBuffers(*ppbi);
        VP6_DeleteQuantizer((void *)((char *)*ppbi + 0x13c));
        DeletePostProcInstance((void *)((char *)*ppbi + 0x29c));
    }

    duck_free(*ppbi);
    *ppbi = 0;
}

PB_INSTANCE *VP6_CreatePBInstance(void) {
    PB_INSTANCE *pbi;
    FRAG_CONFIG ConfigurationInit;
    int pbi_size = 0x4958;

    memset(&ConfigurationInit, 0, sizeof(ConfigurationInit));
    ConfigurationInit.e = 8;
    ConfigurationInit.f = 8;

    pbi = duck_malloc(pbi_size, 0);
    if (pbi == 0) {
        return 0;
    }

    memset(pbi, 0, pbi_size);
    *(FRAG_CONFIG *)((char *)pbi + 0x1b0) = ConfigurationInit;

    if (VP6_AllocateTmpBuffers(pbi) == 0) {
        duck_free(pbi);
        return 0;
    }

    *(int *)((char *)pbi + 0x1a8) = 0x46;
    pbi->field_298 = (void *)idct;

    memset((char *)pbi + 0x3a4, 0, 0x16);
    memset((char *)pbi + 0x3ba, 0, 0x18c);

    return pbi;
}

void VP6_VPInitLibrary(void) {
    int i;

    CPUFrequency = VP6_GetProcessorFrequency();
    VP6_DMachineSpecificConfig();

    for (i = 0; i < 64; i++) {
        VP6_DCQuantScaleP[i] = (VP6_DcQuant[i] / 2) + 2;
    }

    InitPostProcessing(VP6_DCQuantScaleP, VP6_DCQuantScaleP, VP6_DCQuantScaleP, 7);
    InitVPUtil();
}

void VP6_VPDeInitLibrary(void) {
}
