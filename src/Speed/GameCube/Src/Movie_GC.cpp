#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "dolphin.h"

struct ElementInfo {
    unsigned int mType : 8;
    unsigned int mFlags : 24;
};

struct ShapeElement {
    ElementInfo mElementInfo;
    int mNextOffset;
    int mDataOffset;
    int mDataSize;
};

struct TextureElement : public ShapeElement {
    int mShapeX;
    int mShapeY;
    int mWidth;
    int mHeight;

    int GetWidth() const {
        return this->mWidth;
    }

    int GetHeight() const {
        return this->mHeight;
    }
};

namespace RealShape {
struct Shape : public ShapeElement {
    TextureElement *GetTexture() const;

    const void *GetData() const {
        if (mDataOffset == 0) {
            return 0;
        }
        return reinterpret_cast<const char *>(this) + mDataOffset;
    }

    int GetWidth() const {
        return GetTexture()->GetWidth();
    }

    int GetHeight() const {
        return GetTexture()->GetHeight();
    }
};
}

struct tBigYUVSwizzler;
struct tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(GXTexObj *tYTexp, GXTexObj *tUTexp, GXTexObj *tVTexp)
    __asm__("NEW_tBigYUVSwizzlerTexture__FP9_GXTexObjN20");
void tBigYUVSwizzler_DrawSetup(struct tBigYUVSwizzler *This, GXTexObj *tYTexp, GXTexObj *tUTexp, GXTexObj *tVTexp)
    __asm__("tBigYUVSwizzler_DrawSetup__FP15tBigYUVSwizzlerP9_GXTexObjN21");
void DELETE_tBigYUVSwizzler(struct tBigYUVSwizzler *This);
void __InitGXlite();

extern int ScreenWidth;
extern int ScreenHeight;

struct GCHW_VD {
    tBigYUVSwizzler *m_pYUVSwizzler;
    GXTexObj YTexObj;
    GXTexObj CbTexObj;
    GXTexObj CrTexObj;
    RealShape::Shape *mCurrentFrame;
    bool mIsVP6;

    GCHW_VD(RealShape::Shape *yuvshp, bool isVP6Movie);
    ~GCHW_VD();
    void iDraw();
};

struct MoviePlayer {
    void FillInTextureInfo(unsigned int *buffer, TextureInfo *texture_info, RealShape::Shape *yuv_shape);
};

GCHW_VD *gGCVD;

GCHW_VD::GCHW_VD(RealShape::Shape *yuvshp, bool isVP6Movie) : mIsVP6(isVP6Movie) {
    TextureElement *texture = yuvshp->GetTexture();
    int w = texture->GetWidth();
    int h = texture->GetHeight();

    if (mIsVP6) {
        w += 0x60;
        h += 0x60;
    }

    GXInitTexObjCI(&YTexObj, 0, static_cast<u16>(w), static_cast<u16>(h), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 2);
    GXInitTexObjLOD(&YTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    w /= 2;
    h /= 2;

    GXInitTexObjCI(&CrTexObj, 0, static_cast<u16>(w), static_cast<u16>(h), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 1);
    GXInitTexObjLOD(&CrTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    GXInitTexObjCI(&CbTexObj, 0, static_cast<u16>(w), static_cast<u16>(h), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 0);
    GXInitTexObjLOD(&CbTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    m_pYUVSwizzler = NEW_tBigYUVSwizzlerTexture(&YTexObj, &CrTexObj, &CbTexObj);
}

GCHW_VD::~GCHW_VD() {
    DELETE_tBigYUVSwizzler(m_pYUVSwizzler);
}

void GCHW_VD::iDraw() {
    if (mCurrentFrame) {
        char *y = reinterpret_cast<char *>(const_cast<void *>(mCurrentFrame->GetData()));
        int w = mCurrentFrame->GetWidth();
        int h = mCurrentFrame->GetHeight();
        char *cb;
        char *cr;
        unsigned long size;
        float u0;
        float u1;
        float v0;
        float v1;
        float m_l;
        float m_t;
        float m_r;
        float m_b;
        float m_z;

        if (!mIsVP6) {
            cb = y + w * h;
            cr = cb + (w / 2) * (h / 2);
        } else {
            const int vp6Border = 0x60;
            int dataOfs = w + vp6Border;
            int uvOfs = (dataOfs / 2) * 0x18;

            cb = y + dataOfs * (h + vp6Border);
            y += dataOfs * 0x30;
            cr = cb + (dataOfs / 2) * ((h + vp6Border) / 2) + uvOfs;
            cb += uvOfs;
            w = dataOfs;
        }

        GXSetCullMode(GX_CULL_NONE);
        GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
        size = static_cast<unsigned long>(w * h) >> 2;
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetNumTevStages(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
        GXSetNumTexGens(1);
        GXSetNumChans(1);
        GXSetChanCtrl(static_cast<GXChannelID>(4), GX_FALSE, static_cast<GXColorSrc>(0), static_cast<GXColorSrc>(1), GX_LIGHT_NULL,
                      static_cast<GXDiffuseFn>(2), static_cast<GXAttnFn>(2));
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
        DCFlushRangeNoSync(y, w * h);
        DCFlushRangeNoSync(cr, size);
        DCFlushRangeNoSync(cb, size);
        GXInitTexObjCI(&YTexObj, y, static_cast<u16>(w), static_cast<u16>(h), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 2);
        GXInitTexObjLOD(&YTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXInitTexObjCI(&CrTexObj, cr, static_cast<u16>(w / 2), static_cast<u16>(h / 2), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 1);
        GXInitTexObjLOD(&CrTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXInitTexObjCI(&CbTexObj, cb, static_cast<u16>(w / 2), static_cast<u16>(h / 2), static_cast<GXCITexFmt>(9), GX_CLAMP, GX_CLAMP, 0, 0);
        GXInitTexObjLOD(&CbTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        tBigYUVSwizzler_DrawSetup(m_pYUVSwizzler, &YTexObj, &CrTexObj, &CbTexObj);
        PPCSync();

        if (mIsVP6) {
            u0 = 48.0f / static_cast<float>(w);
            u1 = static_cast<float>(w - 0x30) / static_cast<float>(w);
            v0 = 1.0f;
            v1 = 0.0f;
        } else {
            u0 = 0.0f;
            u1 = 1.0f;
            v0 = 0.0f;
            v1 = 1.0f;
        }

        m_l = 0.0f;
        m_t = 0.0f;
        m_r = static_cast<float>(ScreenWidth) - 1.0f;
        m_b = static_cast<float>(ScreenHeight) - 1.0f;
        m_z = 0.0f;

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(m_l, m_t, m_z);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(m_r, m_t, m_z);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(m_r, m_b, m_z);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(m_l, m_b, m_z);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(u0, v1);
        GXEnd();

        GXSetNumIndStages(0);
        GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(0), static_cast<GXTevColorChan>(0), static_cast<GXTevColorChan>(1),
                              static_cast<GXTevColorChan>(2), static_cast<GXTevColorChan>(3));
        GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(1), static_cast<GXTevColorChan>(0), static_cast<GXTevColorChan>(0),
                              static_cast<GXTevColorChan>(0), static_cast<GXTevColorChan>(3));
        GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(2), static_cast<GXTevColorChan>(1), static_cast<GXTevColorChan>(1),
                              static_cast<GXTevColorChan>(1), static_cast<GXTevColorChan>(3));
        GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(3), static_cast<GXTevColorChan>(2), static_cast<GXTevColorChan>(2),
                              static_cast<GXTevColorChan>(2), static_cast<GXTevColorChan>(3));
        GXSetTevDirect(static_cast<GXTevStageID>(0));
        GXSetTevDirect(static_cast<GXTevStageID>(1));
        GXSetTevDirect(static_cast<GXTevStageID>(2));
        GXSetTevDirect(static_cast<GXTevStageID>(3));
        GXSetTevDirect(static_cast<GXTevStageID>(4));
        GXSetTevSwapMode(static_cast<GXTevStageID>(1), static_cast<GXTevSwapSel>(0), static_cast<GXTevSwapSel>(0));
        GXSetTevSwapMode(static_cast<GXTevStageID>(2), static_cast<GXTevSwapSel>(0), static_cast<GXTevSwapSel>(0));
        __InitGXlite();
    }
}

void MoviePlayer::FillInTextureInfo(unsigned int *buffer, TextureInfo *texture_info, RealShape::Shape *yuv_shape) {
    if (gGCVD) {
        gGCVD->mCurrentFrame = yuv_shape;
    }
}

void GCDrawMovie() {
    if (gGCVD) {
        gGCVD->iDraw();
    }
}

void PlatSetFirstMovieFrame(TextureInfo *texture_info, RealShape::Shape *yuv_shape, bool isVP6Movie) {
    if (!gGCVD) {
        gGCVD = new GCHW_VD(yuv_shape, isVP6Movie);
    }
}

int RCMP_GetMaxFramesOutStanding() {
    return 2;
}

void PlatFinishMovie() {
    if (gGCVD) {
        delete gGCVD;
        gGCVD = 0;
    }
}
