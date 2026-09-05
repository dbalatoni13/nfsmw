#include "rcmp/rcmp.h"
#include "dolphin.h"

struct tBigSwizzler;

struct tBigSTPix {
    unsigned char X;
    unsigned char Y;
};

struct tBigSwizzler {
    unsigned int TileWidth;
    unsigned int TileHeight;
    unsigned int Width;
    unsigned int Height;
    tBigSTPix *TextureData;
    GXTexObj TextureObj;
    unsigned int TextureWidth;
    unsigned int TextureHeight;
    tBigSTPix *TextureData2;
    GXTexObj TextureObj2;
};

extern "C" char lbl_804100A4[];

extern void DELETE_tBigSwizzler(tBigSwizzler *This);

struct tBigYUVSwizzler {
    unsigned char m_YTable[0x200];
    unsigned char m_UTable[0x200];
    unsigned char m_VTable[0x200];
    tBigSwizzler *m_YSwizzler;
    tBigSwizzler *m_UVSwizzler;
    GXTlutObj m_YTlut;
    GXTlutObj m_UTlut;
    GXTlutObj m_VTlut;
};

void DELETE_tBigYUVSwizzler(tBigYUVSwizzler *This) {
    DELETE_tBigSwizzler(This->m_YSwizzler);
    DELETE_tBigSwizzler(This->m_UVSwizzler);
    RCMP::rcmp_sys.FreeMem(This);
}

extern tBigSwizzler *NEW_tBigSwizzlerTexture(struct _GXTexObj *tTexp);

struct tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(struct _GXTexObj *tYTexp,
                                                   struct _GXTexObj *tUTexp,
                                                   struct _GXTexObj *) {
    int i;
    float fi;
    tBigYUVSwizzler *This;

    This = static_cast<tBigYUVSwizzler *>(RCMP::rcmp_sys.AllocMem(
        lbl_804100A4, 0x640, 0x20, 0, RCMP::rcmp_sys.m_DefaultMemDir));
    This->m_YSwizzler = NEW_tBigSwizzlerTexture(tYTexp);
    This->m_UVSwizzler = NEW_tBigSwizzlerTexture(tUTexp);

    for (i = 0; i < 0x100; ++i) {
        fi = static_cast<float>(i) - 128.0f;
        This->m_YTable[i * 2] = 0xff;
        This->m_YTable[i * 2 + 1] = static_cast<unsigned char>(i);
        This->m_UTable[i * 2] = static_cast<unsigned char>(fi * 0.7009999752044678f + 128.0f);
        This->m_UTable[i * 2 + 1] = static_cast<unsigned char>(fi * -0.35705000162124634f + 85.33333587646484f);
        This->m_VTable[i * 2] = static_cast<unsigned char>(fi * -0.17204999923706055f + 42.66666793823242f);
        This->m_VTable[i * 2 + 1] = static_cast<unsigned char>(fi * 0.8859999775886536f + 128.0f);
    }

    DCFlushRange(This->m_YTable, 0x200);
    DCFlushRange(This->m_UTable, 0x200);
    DCFlushRange(This->m_VTable, 0x200);
    GXInitTlutObj(&This->m_YTlut, This->m_YTable, GX_TL_IA8, 0x100);
    GXInitTlutObj(&This->m_UTlut, This->m_UTable, GX_TL_IA8, 0x100);
    GXInitTlutObj(&This->m_VTlut, This->m_VTable, GX_TL_IA8, 0x100);
    return This;
}

void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler *This, struct _GXTexObj *YImage,
                               struct _GXTexObj *CrImage, struct _GXTexObj *CbImage) {
    float mtx[2][3];
    tBigSwizzler *YSwizzler;
    tBigSwizzler *UVSwizzler;
    GXColor RGMask = {0xff, 0xff, 0, 0};
    GXColor GBMask = {0, 0xff, 0xff, 0};
    YSwizzler = This->m_YSwizzler;
    UVSwizzler = This->m_UVSwizzler;

    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);

    GXInitTexObjLOD(CbImage, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE,
                    GX_FALSE, GX_ANISO_1);
    GXInitTexObjLOD(CrImage, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE,
                    GX_FALSE, GX_ANISO_1);
    GXInitTexObjLOD(YImage, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE,
                    GX_FALSE, GX_ANISO_1);

    GXLoadTlut(&This->m_VTlut, GX_TLUT0);
    GXLoadTlut(&This->m_UTlut, GX_TLUT1);
    GXLoadTlut(&This->m_YTlut, GX_TLUT2);

    GXLoadTexObj(CbImage, GX_TEXMAP0);
    GXLoadTexObj(CrImage, GX_TEXMAP1);
    GXLoadTexObj(YImage, GX_TEXMAP2);
    GXLoadTexObj(&UVSwizzler->TextureObj, GX_TEXMAP3);
    GXLoadTexObj(&UVSwizzler->TextureObj2, GX_TEXMAP4);
    GXLoadTexObj(&YSwizzler->TextureObj, GX_TEXMAP5);
    GXLoadTexObj(&YSwizzler->TextureObj2, GX_TEXMAP6);

    GXSetTevKColor(GX_KCOLOR0, RGMask);
    GXSetTevKColor(GX_KCOLOR1, GBMask);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_ALPHA, GX_CH_RED, GX_CH_RED,
                          GX_CH_RED);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_ALPHA, GX_CH_GREEN,
                          GX_CH_GREEN);

    GXSetNumTexGens(2);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEXCOORD0, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEXCOORD0, GX_IDENTITY);

    mtx[0][0] = static_cast<float>(UVSwizzler->TileWidth) * 0.0625f;
    mtx[0][1] = 0.0f;
    mtx[0][2] = 0.0f;
    mtx[1][0] = 0.0f;
    mtx[1][1] = static_cast<float>(UVSwizzler->TileHeight) * 0.0625f;
    mtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_0, reinterpret_cast<const float (*)[3]>(mtx), 4);

    mtx[0][0] = 0.0625f;
    mtx[0][1] = 0.0f;
    mtx[0][2] = 0.0f;
    mtx[1][0] = 0.0f;
    mtx[1][1] = 0.0625f;
    mtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_1, reinterpret_cast<const float (*)[3]>(mtx), 4);

    GXSetNumIndStages(4);
    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP3);
    GXSetTevIndirect(GX_TEVSTAGE0, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE,
                     GX_ITM_0, GX_ITW_0, GX_ITW_0, GX_FALSE, GX_FALSE,
                     GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD0, GX_TEXMAP4);
    GXSetTevIndirect(GX_TEVSTAGE1, GX_INDTEXSTAGE1, GX_ITF_8, GX_ITB_T,
                     GX_ITM_1, GX_ITW_0, GX_ITW_OFF, GX_TRUE, GX_FALSE,
                     GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE2, GX_TEXCOORD1, GX_TEXMAP5);
    GXSetTevIndirect(GX_TEVSTAGE3, GX_INDTEXSTAGE2, GX_ITF_8, GX_ITB_NONE,
                     GX_ITM_0, GX_ITW_0, GX_ITW_0, GX_FALSE, GX_FALSE,
                     GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE3, GX_TEXCOORD1, GX_TEXMAP6);
    GXSetTevIndirect(GX_TEVSTAGE4, GX_INDTEXSTAGE3, GX_ITF_8, GX_ITB_T,
                     GX_ITM_1, GX_ITW_0, GX_ITW_OFF, GX_TRUE, GX_FALSE,
                     GX_ITBA_OFF);

    GXSetNumTevStages(5);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevIndRepeat(GX_TEVSTAGE2);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);

    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO,
                    GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_ZERO);

    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST,
                    GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP1, GX_TEV_SWAP2);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);

    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2,
                    GX_FALSE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST,
                    GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP1, GX_TEV_SWAP1);
    GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);

    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_FALSE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO,
                    GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_ZERO);

    GXSetTevColorOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE4, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO,
                    GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1,
                    GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_TEXA);
}
