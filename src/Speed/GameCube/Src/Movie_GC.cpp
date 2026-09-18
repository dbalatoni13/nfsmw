#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"

#include <dolphin.h>

#include <new>
#include "Speed/GameCube/Src/Ecstasy/EcstasyE.hpp"

struct tBigYUVSwizzler;

void DELETE_tBigYUVSwizzler(tBigYUVSwizzler *swizzler);

class GCHW_VD {
  public:
    GCHW_VD(RealShape::Shape *shape, bool isVP6Movie);
    ~GCHW_VD();
    void iDraw();

    tBigYUVSwizzler *mSwizzler; // offset 0x0
    GXTexObj mTexY;             // offset 0x4, size 0x20
    GXTexObj mTexV;             // offset 0x24, size 0x20
    GXTexObj mTexU;             // offset 0x44, size 0x20
    RealShape::Shape *mShape;   // offset 0x64
    int mIsVP6Movie;            // offset 0x68
};

// API del conversor YUV (definidas en otra unidad).
tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(GXTexObj *tex0, GXTexObj *tex1, GXTexObj *tex2);
void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler *swizzler, GXTexObj *tex0, GXTexObj *tex1, GXTexObj *tex2);

// Coordenadas de pantalla (otra unidad).
extern int ScreenWidth;
extern int ScreenHeight;

// Mitad redondeada de un entero sin signo truncada a 16 bits, tal como la
// codifica el original (extrwi tras el arranque de division con signo).
static inline unsigned short HalfDim(unsigned int dim) {
    return (unsigned short)((dim + (dim >> 31)) >> 1);
}

// Forma del GetData de ShapeElement del original: guarda temprana con el
// puntero base sumado despues de la expansion.
static inline const void *GetShapeData(const void *elem, int dataOfs) {
    if (dataOfs == 0) {
        return 0;
    }

    return (const char *)elem + dataOfs;
}

GCHW_VD *gGCVD;

void MoviePlayer::FillInTextureInfo(uint32 *frame_address, TextureInfo *texture_info, RealShape::Shape *shape) {
    if (gGCVD != 0) {
        gGCVD->mShape = shape;
    }
}

void GCDrawMovie() {
    if (gGCVD != 0) {
        gGCVD->iDraw();
    }
}

void PlatSetFirstMovieFrame(TextureInfo *texture_info, RealShape::Shape *yuv_shape, bool isVP6Movie) {
    if (gGCVD == 0) {
        gGCVD = new ("GCHW_VD from PlatSetFirstMovieFrame", 0) GCHW_VD(yuv_shape, isVP6Movie);
    }
}

int RCMP_GetMaxFramesOutStanding() {
    return 2;
}

void PlatFinishMovie() {
    if (gGCVD != 0) {
        delete gGCVD;
        gGCVD = 0;
    }
}

// Tres texturas CI8: la luminancia a tamano completo y las dos de crominancia
// a la mitad. La VP6 anhade un borde de 0x60 texels al tamano del shape.
GCHW_VD::GCHW_VD(RealShape::Shape *shape, bool isVP6Movie) {
    this->mIsVP6Movie = isVP6Movie;

    int w = *(int *)(shape->GetTexture() + 0x18 / 4);
    int h = *(int *)(shape->GetTexture() + 0x1C / 4);
    if (this->mIsVP6Movie != 0) {
        w += 0x60;
        h += 0x60;
    }

    GXInitTexObjCI(&this->mTexY, 0, (unsigned short)w, (unsigned short)h, (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 2);
    GXInitTexObjLOD(&this->mTexY, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    unsigned short halfW = HalfDim(w);
    unsigned short halfH = HalfDim(h);

    GXInitTexObjCI(&this->mTexU, 0, halfW, halfH, (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 1);
    GXInitTexObjLOD(&this->mTexU, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    GXInitTexObjCI(&this->mTexV, 0, halfW, halfH, (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 0);
    GXInitTexObjLOD(&this->mTexV, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    this->mSwizzler = NEW_tBigYUVSwizzlerTexture(&this->mTexY, &this->mTexU, &this->mTexV);
}

GCHW_VD::~GCHW_VD() {
    DELETE_tBigYUVSwizzler(this->mSwizzler);
}

// Sube el frame YUV del shape y pinta un quad a pantalla completa muestreando
// las tres texturas. La VP6 recorta 0x30 texels de borde por lado en U.
void GCHW_VD::iDraw() {
    unsigned char *y;
    unsigned char *cb;
    unsigned char *cr;
    int w;
    int h;
    unsigned int size;
    float u0;
    float u1;
    float v0;
    float v1;

    if (this->mShape == 0) {
        return;
    }

    y = (unsigned char *)GetShapeData(this->mShape, *(int *)((char *)this->mShape + 8));

    w = *(int *)(this->mShape->GetTexture() + 0x18 / 4);
    h = *(int *)(this->mShape->GetTexture() + 0x1C / 4);

    if (this->mIsVP6Movie != 0) {
        const int vp6Border = 0x30;
        int dataOfs;

        w += vp6Border * 2;
        h += vp6Border * 2;

        cb = y + w * h;
        cr = cb + (w / 2) * (h / 2);

        dataOfs = w * vp6Border;
        y = y + dataOfs;

        h -= vp6Border * 2;

        dataOfs = (w / 2) * (vp6Border / 2);
        cb = cb + dataOfs;
        cr = cr + dataOfs;
    } else {
        cb = y + w * h;
        cr = cb + (w / 2) * (h / 2);
    }

    size = w * h;

    GXSetCullMode(GX_CULL_NONE);
    GXSetBlendMode((GXBlendMode)0, (GXBlendFactor)1, (GXBlendFactor)0, (GXLogicOp)5);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetNumTevStages(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, (GXTexGenType)1, (GXTexGenSrc)4, 0x3C, GX_FALSE, 0x7D);
    GXSetNumTexGens(1);
    GXSetNumChans(1);
    GXSetChanCtrl((GXChannelID)4, GX_FALSE, (GXColorSrc)0, (GXColorSrc)1, 0, (GXDiffuseFn)2, (GXAttnFn)2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, (GXTevMode)0);

    DCFlushRangeNoSync(y, size);
    DCFlushRangeNoSync(cr, size / 4);
    DCFlushRangeNoSync(cb, size / 4);

    GXInitTexObjCI(&this->mTexY, y, (unsigned short)w, (unsigned short)h, (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 2);
    GXInitTexObjLOD(&this->mTexY, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    GXInitTexObjCI(&this->mTexU, cr, HalfDim(w), HalfDim(h), (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 1);
    GXInitTexObjLOD(&this->mTexU, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    GXInitTexObjCI(&this->mTexV, cb, HalfDim(w), HalfDim(h), (GXCITexFmt)GX_TF_C8, GX_CLAMP, GX_CLAMP, 0, 0);
    GXInitTexObjLOD(&this->mTexV, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    tBigYUVSwizzler_DrawSetup(this->mSwizzler, &this->mTexY, &this->mTexU, &this->mTexV);

    PPCSync();

    if (this->mIsVP6Movie != 0) {
        u0 = 48.0f / (float)w;
        u1 = (float)(w - 0x30) / (float)w;
        v0 = 1.0f;
        v1 = 0.0f;
    } else {
        u0 = 0.0f;
        u1 = 1.0f;
        v0 = 0.0f;
        v1 = 1.0f;
    }

    {
        float m_l = 0.0f;
        float m_t = 0.0f;
        float m_r = (float)ScreenWidth - 1.0f;
        float m_b = (float)ScreenHeight - 1.0f;
        float m_z = 0.0f;

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
    }

    GXSetNumIndStages(0);
    GXSetTevSwapModeTable((GXTevSwapSel)0, (GXTevColorChan)0, (GXTevColorChan)1, (GXTevColorChan)2, (GXTevColorChan)3);
    GXSetTevSwapModeTable((GXTevSwapSel)1, (GXTevColorChan)0, (GXTevColorChan)0, (GXTevColorChan)0, (GXTevColorChan)3);
    GXSetTevSwapModeTable((GXTevSwapSel)2, (GXTevColorChan)1, (GXTevColorChan)1, (GXTevColorChan)1, (GXTevColorChan)3);
    GXSetTevSwapModeTable((GXTevSwapSel)3, (GXTevColorChan)2, (GXTevColorChan)2, (GXTevColorChan)2, (GXTevColorChan)3);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevDirect(GX_TEVSTAGE2);
    GXSetTevDirect(GX_TEVSTAGE3);
    GXSetTevDirect(GX_TEVSTAGE4);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    __InitGXlite();
}
