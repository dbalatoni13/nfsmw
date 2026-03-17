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
struct Shape {
    TextureElement *GetTexture() const;
};
}

struct tBigYUVSwizzler;
struct tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(GXTexObj *tYTexp, GXTexObj *tUTexp, GXTexObj *tVTexp)
    __asm__("NEW_tBigYUVSwizzlerTexture__FP9_GXTexObjN20");
void DELETE_tBigYUVSwizzler(struct tBigYUVSwizzler *This);

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

void PlatFinishMovie() {
    if (gGCVD) {
        delete gGCVD;
        gGCVD = 0;
    }
}
