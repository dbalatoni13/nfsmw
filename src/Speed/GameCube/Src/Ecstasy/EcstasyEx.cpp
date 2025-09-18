#include "./EcstasyEx.hpp"

#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/gx/GXCull.h"
#include "dolphin/gx/GXEnum.h"
#include "dolphin/gx/GXPixel.h"
#include "dolphin/gx/GXTev.h"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "dolphin/types.h"

void eSetCulling(GXCullMode mode) {
    static GXCullMode prevMode = GX_CULL_NONE;

    if (mode != prevMode) {
        GXSetCullMode(mode);
        prevMode = mode;
    }
}

// STRIPPED
bool IsZBufferEnabled(bool &write_enabled) { return false; }

Bool prevTest = 1;
Bool prevWrite = 1;

void eResetZBuffering() {
    prevTest = 1;
    prevWrite = 1;

    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
}

void eSetZBuffering(Bool eEnable, Bool eWriteEnable) {
    if (eEnable != prevTest || eWriteEnable != prevWrite) {
        GXSetZMode(eEnable, GX_LEQUAL, eWriteEnable);
        prevTest = eEnable;
        prevWrite = eWriteEnable;
    }
}

void eSetZCompLoc(Bool beforeTex) {
    static Bool prev = 1;

    if (beforeTex != prev) {
        GXSetZCompLoc((beforeTex == 0) ? GX_FALSE : GX_TRUE);
        prev = beforeTex;
    }
}

void eSetColourUpdate(Bool bRGB, Bool bAlpha) {
    static Bool prevRGB = 1;
    static Bool prevAlpha = 1;

    if (bRGB != prevRGB) {
        GXSetColorUpdate((bRGB == 0) ? GX_FALSE : GX_TRUE);
        prevRGB = bRGB;
    }
    if (bAlpha != prevAlpha) {
        GXSetAlphaUpdate((bAlpha == 0) ? GX_FALSE : GX_TRUE);
        prevAlpha = bAlpha;
    }
}

static unsigned char _alphaOn = 0;
static unsigned char _alphaRef = 0x28;

// STRIPPED
bool IsAlphaTestOn() { return false; }

static void _alphaTestFunc() {
    if (_alphaOn != 0) {
        eSetZCompLoc(0);
        GXSetAlphaCompare(GX_GREATER, _alphaRef, GX_AOP_AND, GX_ALWAYS, 0);
    } else {
        eSetZCompLoc(1);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
}

// STRIPPED
void eSetAlphaRef(unsigned char ref) {}

void eSetAlphaTest(unsigned char bOn) {
    if (bOn != _alphaOn) {
        _alphaOn = bOn;
        _alphaTestFunc();
    }
}

// STRIPPED
const char *GetBlendModeName() { return ""; }

// STRIPPED
const char *GetSrcBlendFactorName() { return ""; }

// STRIPPED
const char * GetDstBlendFactorName() { return ""; }

extern void eSetFogState(struct TextureInfo * texture_info /* r3 */, enum _GXBlendMode mode /* r4 */);

GXBlendMode prevMode = GX_BM_NONE;
GXBlendFactor prevSrc = GX_BL_ONE;
GXBlendFactor prevDst = GX_BL_ZERO;

void eSetBlendMode(TextureInfo *texture_info, unsigned char opt) {
    if (texture_info->AlphaUsageType == 1) {
        eSetAlphaTest(1);
    } else {
        eSetAlphaTest(0);
    }

    GXBlendMode mode;
    GXBlendFactor srcFactor;
    GXBlendFactor dstFactor;
    switch (texture_info->AlphaBlendType) {
        case 1:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 2:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_ONE;
            break;
        case 3:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 4:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 5:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 6:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 7:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        case 8:
            mode = GX_BM_BLEND;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;

        case 0:
            mode = GX_BM_NONE;
            srcFactor = GX_BL_SRCALPHA;
            dstFactor = GX_BL_INVSRCALPHA;
            break;
        
        default:
            mode = GX_BM_NONE;
            srcFactor = GX_BL_ONE;
            dstFactor = GX_BL_ZERO;
    }

    if (mode == GX_BM_BLEND) {
        eSetZBuffering(true, false);
    } else {
        eSetZBuffering(true, true);
    }

    eSetFogState(texture_info, mode);
    if (mode != prevMode || srcFactor != prevSrc || dstFactor != prevDst) {
        GXSetBlendMode(mode, srcFactor, dstFactor, GX_LO_NOOP);
        prevMode = mode;
        prevSrc = srcFactor;
        prevDst = dstFactor;
    }
}

void eResetBlendMode() {
    prevMode = GX_BM_BLEND;
    prevSrc = GX_BL_ONE;
    prevDst = GX_BL_ZERO;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
}

void eSetBlendModeSrcInvSrc() {
    prevMode = GX_BM_BLEND;
    prevSrc = GX_BL_SRCALPHA;
    prevDst = GX_BL_INVSRCALPHA;
    
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
}

// STRIPPED
void eSetBlendModeSrcSrc() {}

// STRIPPED
void eSetBlendModeOneOne() {}

void eSetBlendModeSrcAlphaOne() {
    prevMode = GX_BM_BLEND;
    prevSrc = GX_BL_SRCALPHA;
    prevDst = GX_BL_ONE;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
}

void eSetBlendModeNone() {
    prevMode = GX_BM_NONE;
    prevSrc = GX_BL_SRCALPHA;
    prevDst = GX_BL_INVSRCALPHA;

    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
}

// STRIPPED
void eSetBlendModeTweak() {}

// STRIPPED
void eSetClrChannelClamping(GXChannelID cID, GXLightID lID) {}

// STRIPPED
void eResetClrChannelClamping(GXChannelID cID) {}

// STRIPPED
void eDrawLine(bVector3 &p1, bVector3 &p2, unsigned int cp1, unsigned int cp2) {}

cCaptureBuffer::cCaptureBuffer() {
    this->xOrigin = 0;
    this->yOrigin = 0;
    this->width = 0;
    this->height = 0;
    this->bInitialized = 0;
    this->pCaptureTexture = nullptr;
    this->pCapturePixels = nullptr;
}

cCaptureBuffer::~cCaptureBuffer() {
    this->Destroy();
}

void cCaptureBuffer::Init(int x0, int y0, int w, int h, int format, int buffer_function) {
    int size = 0;

    this->xOrigin = x0;
    this->yOrigin = y0;
    this->width = w;
    this->height = h;

    switch (format) {
        case 6:
        case 0x16:
            size = w * h * 4;
            this->pCapturePixels = (char *)bMalloc(size, 0x800);
            break;

        case 3:
        case 4:
        case 5:
            size = w * h * 2;
            this->pCapturePixels = (char *)bMalloc(size, 0x800);
            break;

        case 1:
        case 0x11:
        case 0x27:
            size = w * h;
            this->pCapturePixels = (char *)bMalloc(size, 0x800);
            break;
    }

    this->pCaptureTexture = eCreateTextureInfo();

    this->pCaptureTexture->AlphaUsageType = 0;
    this->pCaptureTexture->AlphaBlendType = 0;
    this->pCaptureTexture->Width = w;
    this->pCaptureTexture->Height = h;
    this->pCaptureTexture->NumMipMapLevels = 0;
    this->pCaptureTexture->ImageData = this->pCapturePixels;
    this->pCaptureTexture->PaletteData = nullptr;
    this->pCaptureTexture->TilableUV = 0;
    this->pCaptureTexture->GetPlatInfo()->Format = format;
    this->pCaptureTexture->GetPlatInfo()->SetImage(this->pCaptureTexture);
    this->pCaptureTexture->BaseImageSize = size;
    this->bInitialized = 1;
}

// end
void epCalculateLocalDirectionalPOS16(unsigned int * colour_table0 /* r29 */, unsigned int * colour_table1 /* r22 */, int num_colour_entries /* r5 */, unsigned short * position_table_16 /* r30 */, int * normal_table /* r23 */, unsigned char * index_buffer /* r31 */, int vertex_description /* r7 */, int num_indicies /* r28 */, struct eLightMaterial * light_material /* r11 */, struct eLightContext * light_context /* r12 */);
