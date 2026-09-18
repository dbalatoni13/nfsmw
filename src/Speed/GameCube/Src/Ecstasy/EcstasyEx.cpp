#include "./EcstasyEx.hpp"
#include "./EcstasyE.hpp"

#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "TextureInfoPlat.hpp"
#include "dolphin.h"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "eViewPlat.hpp"
#include "eMatrixE.hpp"
#include "dolphin/gx/GXVert.h"

static float FogCurrentBrightness = 0.0f;

extern RegionQuery RegionInfo;
typedef float sn_ps __attribute__((mode(PS)));
#define V2(v) (*(sn_ps *)&(v))

void eSetCulling(GXCullMode mode) {
    static GXCullMode prevMode = GX_CULL_NONE;

    if (mode != prevMode) {
        GXSetCullMode(mode);
        prevMode = mode;
    }
}

// STRIPPED
bool IsZBufferEnabled(bool &write_enabled) {
    return false;
}

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
bool IsAlphaTestOn() {
    return false;
}

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
const char *GetBlendModeName() {
    return "";
}

// STRIPPED
const char *GetSrcBlendFactorName() {
    return "";
}

// STRIPPED
const char *GetDstBlendFactorName() {
    return "";
}

extern void eSetFogState(struct TextureInfo *texture_info /* r3 */, enum _GXBlendMode mode /* r4 */);

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
            this->pCapturePixels = (char *)bMalloc(size, __FILE__, __LINE__, 0x800);
            break;

        case 3:
        case 4:
        case 5:
            size = w * h * 2;
            this->pCapturePixels = (char *)bMalloc(size, __FILE__, __LINE__, 0x800);
            break;

        case 1:
        case 0x11:
        case 0x27:
            size = w * h;
            this->pCapturePixels = (char *)bMalloc(size, __FILE__, __LINE__, 0x800);
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

void cCaptureBuffer::Destroy() {
    if (this->pCaptureTexture) {
        eDestroyTextureInfo(this->pCaptureTexture);
        this->pCaptureTexture = nullptr;
    }
    if (this->pCapturePixels) {
        bFree(this->pCapturePixels);
        this->pCapturePixels = nullptr;
    }
}

void cCaptureBuffer::CaptureEFB(int opt, int downsample, GXTexFmt capture_format) {
    if (this->bInitialized != 0) {
        int src_width = this->width;
        int src_height = this->height;
        if (downsample != 0) {
            src_width += src_width;
            src_height += src_height;
        }

        eResetZBuffering();
        eSetColourUpdate(true, true);
        GXSetTexCopySrc(this->xOrigin, this->yOrigin, src_width, src_height);
        GXSetTexCopyDst(this->width, this->height, capture_format, (downsample != 0) ? GX_TRUE : GX_FALSE);

        GXCopyTex(this->pCapturePixels, (opt != 0) ? true : false);
        GXPixModeSync();
        GXInvalidateTexAll();
    }
}
char HORIZON_FOG_GRID_DISPLAY_LIST[768];
float HORIZON_FOG_GRID_POS_ARRAY[384];
unsigned int HORIZON_FOG_GRID_CLR_ARRAY[224];
float HORIZON_FOG_GRID_UVS_ARRAY[256];
void *DLHorizonFogGrid;
unsigned long DLHorizonFogGridSize;
unsigned char HorizonCurrentPOS;
unsigned char HorizonCurrentCLR;
unsigned char HorizonCurrentUVS;

int AddHorizonFogEntryPOS(float posX, float posY, float posZ) {
    float *data_pos = &HORIZON_FOG_GRID_POS_ARRAY[HorizonCurrentPOS * 3];
    int foundPOS = 0;
    int bufPOS_IDX = -1;
    unsigned int size_of_entry = 0xDF;
    unsigned int data_stride = 3;

    for (int i = 0; i < HorizonCurrentPOS; i++) {
        if ((HORIZON_FOG_GRID_POS_ARRAY[i * data_stride] == posX) && (HORIZON_FOG_GRID_POS_ARRAY[i * data_stride + 1] == posY)) {
            bufPOS_IDX = i;
            foundPOS = 1;
            break;
        }
    }
    if (foundPOS == 0) {
        *data_pos++ = posX;
        *data_pos++ = posY;
        *data_pos++ = posZ;

        bufPOS_IDX = HorizonCurrentPOS;
        if (HorizonCurrentPOS <= size_of_entry) {
            HorizonCurrentPOS++;
        }
    }
    return bufPOS_IDX;
}

int AddHorizonFogEntryCLR(unsigned int clr) {
    unsigned int *data_clr = &HORIZON_FOG_GRID_CLR_ARRAY[HorizonCurrentCLR];
    int foundCLR = 0;
    int bufCLR_IDX = -1;
    unsigned int size_of_entry = 0xDF;
    unsigned int data_stride = 1;

    *data_clr++ = clr;

    bufCLR_IDX = HorizonCurrentCLR;
    if (HorizonCurrentCLR <= size_of_entry) {
        HorizonCurrentCLR++;
    }

    return bufCLR_IDX;
}

int AddHorizonFogEntryUVS(float uvX, float uvY) {
    float *data_uvs = &HORIZON_FOG_GRID_UVS_ARRAY[HorizonCurrentUVS * 2];
    int foundUVS = 0;
    int bufUVS_IDX = -1;
    unsigned int size_of_entry = 0xDF;
    unsigned int data_stride = 2;

    for (int i = 0; i < HorizonCurrentUVS; i++) {
        if ((HORIZON_FOG_GRID_UVS_ARRAY[i * data_stride] == uvX) && (HORIZON_FOG_GRID_UVS_ARRAY[i * data_stride + 1] == uvY)) {
            bufUVS_IDX = i;
            foundUVS = 1;
            break;
        }
    }
    if (foundUVS == 0) {
        *data_uvs++ = uvX;
        *data_uvs++ = uvY;

        bufUVS_IDX = HorizonCurrentUVS;
        if (HorizonCurrentUVS <= size_of_entry) {
            HorizonCurrentUVS++;
        }
    }
    return bufUVS_IDX;
}

void GenerateHorizonFogDisplayList(void **display_list, unsigned long *size, GXVtxFmt vertex_format) {
    unsigned long dl_sz;
    unsigned long grid_color;
    int current_index;
    int verts_per_strip;      // r15
    int color_base;
    int color_mul;
    int pos_index;
    int clr_index;
    int uvs_index;
    float grid_pointX = 0.0f; // f28
    float grid_pointY = 0.0f; // f30
    float tex_coordX = 0.0f;  // f26
    float tex_coordY = 0.0f;  // f29
    int multiple;      // r0

    *display_list = HORIZON_FOG_GRID_DISPLAY_LIST;

    float vertex_spacingX = 42.666668f; // f20
    float vertex_spacingY = 68.571434f; // f22
    float uv_spacingX = 0.06666667;     // f21
    float uv_spacingY = 0.14285715;     // f23

    current_index = 0;
    verts_per_strip = 0x20;
    color_base = 10;
    color_mul = 0xF5;

    GXBeginDisplayList(*display_list, 0xC00);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);

    GXSetArray(GX_VA_POS, HORIZON_FOG_GRID_POS_ARRAY, 0xC);
    GXSetArray(GX_VA_CLR0, HORIZON_FOG_GRID_CLR_ARRAY, 0x4);
    GXSetArray(GX_VA_TEX0, HORIZON_FOG_GRID_UVS_ARRAY, 0x8);

    for (int j = 0; j < 7; j++) {
        GXBegin(GX_TRIANGLESTRIP, vertex_format, verts_per_strip);
        for (int i = 0; i < verts_per_strip; i++) {
            multiple = i / 2;

            grid_pointX = multiple * vertex_spacingX;
            grid_pointY = (i & 1) ? 0.0f : vertex_spacingY;
            grid_pointY += j * vertex_spacingY;

            tex_coordX = uv_spacingX * multiple;
            tex_coordY = (i & 1) ? 0.0f : uv_spacingY;
            tex_coordY += j * uv_spacingY;

            float red = bSin(grid_pointX * 6.0f);
            float blue = bCos(grid_pointY * 6.0f);
            unsigned char b_red = (unsigned char)(color_base + red * color_mul);
            unsigned char b_blue = (unsigned char)(color_base + blue * color_mul);

            grid_color = b_red << 24 | b_blue << 8 | 0xFF;

            pos_index = AddHorizonFogEntryPOS(grid_pointX, grid_pointY, 0.0f);
            clr_index = AddHorizonFogEntryCLR(grid_color);
            uvs_index = AddHorizonFogEntryUVS(tex_coordX, tex_coordY);

            GXPosition1x8(pos_index);
            GXColor1x8(current_index);
            GXTexCoord1x8(uvs_index);

            current_index++;
        }
        GXEnd();
    }

    *size = GXEndDisplayList();

    HorizonCurrentPOS--;
    HorizonCurrentCLR--;
    HorizonCurrentUVS--;

    if (*size > 0xC00) {
        OSPanic("d:/mw/speed/gamecube/src/ecstasy/EcstasyEx.cpp", 806, "Exiting");
    }
}

void eInitHorizonFogDisplayList() {
    GenerateHorizonFogDisplayList(&DLHorizonFogGrid, &DLHorizonFogGridSize, GX_VTXFMT0);
}

// STRIPPED
void GenerateHorizonFogVertexColors() {}

// STRIPPED
void RenderHorizonFog() {}

cSphereMap::cSphereMap() {
    this->CubeFaceStart = 0;
    this->CubeFaceEnd = 6;
    this->CubeTevMode = 0;

    this->angle1[0] = 90.0f;
    this->angle1[1] = 180.0f;
    this->angle1[2] = 270.0f;
    this->angle1[3] = 0.0f;
    this->angle1[4] = 90.0f;
    this->angle1[5] = -90.0f;

    this->axis1[0] = 0x79;
    this->axis1[1] = 0x79;
    this->axis1[2] = 0x79;
    this->axis1[3] = 0x79;
    this->axis1[4] = 0x78;
    this->axis1[5] = 0x78;

    this->angle2[0] = 0.0f;
    this->angle2[1] = 0.0f;
    this->angle2[2] = 0.0f;
    this->angle2[3] = 0.0f;
    this->angle2[4] = 180.0f;
    this->angle2[5] = 180.0f;

    this->sphereTexture = nullptr;
    this->sphereTexturePixels = nullptr;
    this->bInitialized = 0;
    this->DLSphere = nullptr;
    this->DLSphereSz = 0;
    this->SPHERE_MAP_SIZE_X = 0x80;
    this->SPHERE_MAP_SIZE_Y = 0x80;
    this->SPHERE_MAP_FMT = GX_TF_RGB565;
    this->SPHERE_MAP_TESS = 30;
    this->CUBEFACE_SIZE = 0x40;

    for (int i = 0; i < 6; i++) {
        this->cubeTex[i] = nullptr;
    }
}

cSphereMap::~cSphereMap() {
    this->Destroy();
}

void cSphereMap::Init(int face_front, int face_right, int face_back, int face_left, int face_up, int face_down) {
    this->cubeBuffer[0].Init(0, 0, face_front, face_front, 5, 1);
    this->cubeBuffer[1].Init(0, 0, face_right, face_right, 5, 1);
    this->cubeBuffer[2].Init(0, 0, face_back, face_back, 5, 1);
    this->cubeBuffer[3].Init(0, 0, face_left, face_left, 5, 1);
    this->cubeBuffer[4].Init(0, 0, face_up, face_up, 5, 1);
    this->cubeBuffer[5].Init(0, 0, face_down, face_down, 5, 1);

    this->cubeTex[0] = (GXTexObj *)this->cubeBuffer[1].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;
    this->cubeTex[1] = (GXTexObj *)this->cubeBuffer[0].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;
    this->cubeTex[2] = (GXTexObj *)this->cubeBuffer[3].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;
    this->cubeTex[3] = (GXTexObj *)this->cubeBuffer[2].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;
    this->cubeTex[4] = (GXTexObj *)this->cubeBuffer[4].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;
    this->cubeTex[5] = (GXTexObj *)this->cubeBuffer[5].pCaptureTexture->GetPlatInfo()->ImageInfos.obj.dummy;

    this->genSphere(&this->DLSphere, &this->DLSphereSz, this->SPHERE_MAP_TESS, GX_VTXFMT7);
    this->sphereTexturePixels = (char *)bMalloc(this->SPHERE_MAP_SIZE_X * this->SPHERE_MAP_SIZE_Y * 2, __FILE__, __LINE__, 0x800);

    this->sphereTexture = eCreateTextureInfo();
    this->sphereTexture->AlphaUsageType = 0;
    this->sphereTexture->AlphaBlendType = 0;
    this->sphereTexture->Width = this->SPHERE_MAP_SIZE_X;
    this->sphereTexture->Height = this->SPHERE_MAP_SIZE_Y;
    this->sphereTexture->NumMipMapLevels = 0;
    this->sphereTexture->ImageData = this->sphereTexturePixels;
    this->sphereTexture->PaletteData = nullptr;
    this->sphereTexture->TilableUV = 0;
    this->sphereTexture->GetPlatInfo()->Format = 4;
    this->sphereTexture->GetPlatInfo()->SetImage(this->sphereTexture);

    this->bInitialized = 1;
}

void cSphereMap::Destroy() {
    if (!this->bInitialized)
        return;
    this->bInitialized = 0;

    if (this->sphereTexture) {
        eDestroyTextureInfo(this->sphereTexture);
        this->sphereTexture = nullptr;
    }
    if (this->sphereTexturePixels) {
        bFree(this->sphereTexturePixels);
        this->sphereTexturePixels = nullptr;
    }

    this->clrSphere(&this->DLSphere, &this->DLSphereSz);

    for (int i = 0; i < 6; i++) {
        this->cubeTex[i] = nullptr;
        this->cubeBuffer[i].Destroy();
    }
}

TextureInfo *cSphereMap::BuildSphereMap() {
    if (this->bInitialized) {
        this->genSphereMap(&this->cubeTex[0], (GXTexObj *)this->sphereTexture->GetPlatInfo()->ImageInfos.obj.dummy, (void *)this->DLSphere,
                           this->DLSphereSz);
        return this->sphereTexture;
    }

    if (!this->bInitialized)
        return nullptr;
}

char ENV_MAP_DISPLAY_LIST[20480];

void cSphereMap::genSphere(void **display_list, unsigned long *size, unsigned short tess, GXVtxFmt fmt) {
    *display_list = ENV_MAP_DISPLAY_LIST;

    float gsPI = 3.1415927f;                                               // f20
    float r = 1.0f;                                                        // f19
    float r1;                                                              // f26
    float r2;                                                              // f25
    float z1;                                                              // f29
    float z2;                                                              // f28
    float n1x;                                                             // f31
    float n1y;                                                             // f1
    float n1z;                                                             // f21
    float n2x;                                                             // f31
    float n2y;                                                             // f0
    float n2z;                                                             // f24
    float theta;                                                           // f30
    float phi;                                                             // f30
    unsigned short nlat = tess;
    unsigned short nlon = tess;
    int i;
    int j;
    unsigned int nverts = (tess - 2) * (tess + 1) * 2 + 1;
    unsigned int dl_sz = (tess + nverts) * 0x18;
    dl_sz = OSRoundUp32B(dl_sz);

    GXBeginDisplayList(*display_list, dl_sz);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxAttrFmt(fmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(fmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);

    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT7, tess + 2);

    theta = gsPI / tess;
    r2 = sinf(theta);
    z2 = cosf(theta);

    n2z = (z2 * 2) * z2 - r;
    n1z = 1.0f;

    GXPosition3f32(0.0f, 0.0f, n1z);
    GXNormal3f32(0.0f, 0.0f, n1z);

    theta = 0.0f;
    for (j = 0; j <= tess;) {
        n2x = r2 * cosf(theta);
        n2y = r2 * sinf(theta);

        GXPosition3f32(n2x, n2y, z2);
        GXNormal3f32(n2x * 2 * z2, n2y * 2 * z2, n2z);

        j++;
        theta = ((gsPI * -2) * j) / nlon;
    }
    GXEnd();

    // ...

    for (i = 2; i < tess; i++) {
        theta = gsPI * float(i) / nlat;
        r1 = r * sinf(gsPI * float(i - 1) / nlat);
        z1 = r * cosf(gsPI * float(i - 1) / nlat);
        r2 = r * sinf(theta);
        z2 = r * cosf(theta);

        n1z = (z1 * 2) * z1 - 1.0f;
        n2z = (z2 * 2) * z2 - 1.0f;

        if (bAbs(z1) < 0.01f || bAbs(z2) < 0.01f)
            break;

        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT7, (tess + 1) * 2);
        phi = 0.0f;
        for (j = 0; j <= tess;) {
            n2x = r2 * cosf(phi);
            n2y = r2 * sinf(phi);

            GXPosition3f32(n2x, n2y, z2);
            GXNormal3f32(n2x * 2 * z2, n2y * 2 * z2, n2z);

            n1x = r1 * cosf(phi);
            n1y = r1 * sinf(phi);

            GXPosition3f32(n1x, n1y, z1);
            GXNormal3f32(n1x * 2 * z1, n1y * 2 * z1, n1z);

            j++;
            phi = ((gsPI * 2) * j) / nlon;
        }

        GXEnd();
    }

    *size = GXEndDisplayList();

    if (*size > dl_sz) {
        OSPanic("d:/mw/speed/gamecube/src/ecstasy/EcstasyEx.cpp", 1227, "Exiting");
    }
}

void cSphereMap::clrSphere(void **display_list, unsigned long *size) {}

static const GXColor TweakSphereMapClr = {0xC4, 0xC4, 0xC4, 0xFF};

void cSphereMap::genSphereMap(GXTexObj **cubemap, GXTexObj *spheremap, void *dl, unsigned long dlsz) {
    int i;
    GXColor color; // r1+0x184
    unsigned short width;
    unsigned short height;
    GXTexFmt fmt;
    void *data;            // r15
    GXLightObj ClipLight;  // r1+0x10
    float p[4][4];         // r1+0x50
    Mtx v;                 // r1+0x90
    Mtx tm;                // r1+0xC0
    Mtx tc;                // r1+0xF0
    Mtx tmp;               // r1+0x120
    if (!this->bInitialized)
        return;

    float camLoc_x = 0.0f; // f30
    float camLoc_y = 0.0f;
    float camLoc_z = 6.0f; // f31
    float up_x = 0.0f;
    float up_y = 1.0f; // f29
    float up_z = 0.0f;
    Point3d camLoc = {camLoc_x, camLoc_y, camLoc_z}; // r1+0x150
    Vec up = {up_x, up_y, up_z};                     // r1+0x160
    Point3d objPt = {0.0f, 0.0f, 0.0f};              // r1+0x170

    width = GXGetTexObjWidth(spheremap);
    height = GXGetTexObjHeight(spheremap);
    fmt = GXGetTexObjFmt(spheremap);
    data = GXGetTexObjData(spheremap);

    C_MTXOrtho(p, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
    GXSetProjection(p, GX_ORTHOGRAPHIC);
    C_MTXLookAt(v, &camLoc, &up, &objPt);
    GXLoadPosMtxImm(v, 0);

    GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
    eSetScissor(0, 0, width, height);
    GXSetTexCopySrc(0, 0, width, height);
    GXSetTexCopyDst(width, height, fmt, GX_FALSE);
    GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, 1, GX_DF_NONE, GX_AF_SPEC);

    color.r = TweakSphereMapClr.r;
    color.g = TweakSphereMapClr.g;
    color.b = TweakSphereMapClr.b;
    color.a = TweakSphereMapClr.a;
    GXInitLightColor(&ClipLight, color);
    GXSetChanMatColor(GX_COLOR0A0, color);

    color.a = 0;
    color.b = 0;
    color.g = 0;
    color.r = 0;
    GXSetChanAmbColor(GX_COLOR0A0, color);
    GXInitLightAttnA(&ClipLight, 0.0f, 2.0f, 0.0f);
    GXInitLightAttnK(&ClipLight, 0.0f, 1.0f, 0.0f);
    GXInitLightPos(&ClipLight, 0.0f, 0.0f, -1.0f);
    GXInitLightDir(&ClipLight, 0.0f, 0.0f, -1.0f);
    GXLoadLightObjImm(&ClipLight, GX_LIGHT0);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, 30);

    switch (this->CubeTevMode) {
        case 0:
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
            GXSetNumTevStages(1);
            break;
        case 1: {
            GXColor Kcolor0 = {0xFF, 0xFF, 0xFF, 0xFF};
            GXSetTevKColor(GX_KCOLOR0, Kcolor0);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetNumTevStages(1);
            break;
        }
        case 2:
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetNumTevStages(1);
            break;
        case 3:
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetNumTevStages(1);
            break;
        case 4:
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetNumTevStages(1);
            break;
        case 5: {
            GXSetNumTevStages(1);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXColor Kcolor5 = {0x00, 0x00, 0x00, 0xFF};
            GXSetTevKColor(GX_KCOLOR0, Kcolor5);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_TEXC, GX_CC_TEXC, GX_CC_TEXC);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_KONST, GX_CA_RASA, GX_CA_KONST, GX_CA_ZERO);
            break;
        }
    }

    GXSetNumTexGens(1);
    GXSetNumChans(1);

    C_MTXLightFrustum(tc, -1.07f, 1.07f, -1.07f, 1.07f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);

    for (i = this->CubeFaceStart; i < this->CubeFaceEnd; i++) {
        GXLoadTexObj(cubemap[i], GX_TEXMAP0);
        PSMTXIdentity(tm);

        if (this->angle2[i] != 0.0f) {
            PSMTXRotRad(tmp, 0x79, MTXDegToRad(this->angle2[i]));
            PSMTXConcat(tm, tmp, tm);
        }

        PSMTXRotRad(tmp, this->axis1[i], MTXDegToRad(this->angle1[i]));
        PSMTXConcat(tm, tmp, tm);
        PSMTXConcat(tc, tm, tm);
        GXLoadTexMtxImm(tm, 30, GX_MTX3x4);
        GXLoadNrmMtxImm(tm, 0);
        GXCallDisplayList(dl, dlsz);
    }

    GXCopyTex(data, GX_TRUE);
    GXPixModeSync();
    __InitGXlite();
}

// STRIPPED
cReflectMap::cReflectMap() {}
// STRIPPED
cReflectMap::~cReflectMap() {}
// STRIPPED
static void Init(cReflectMap *) {}
// STRIPPED
void cReflectMap::Destroy() {}

cSpecularMap::cSpecularMap() {
    this->SPEC_MAP_SIZE_X = 0x20;
    this->SPEC_MAP_SIZE_Y = 0x20;
}

cSpecularMap::~cSpecularMap() {}

void cSpecularMap::Init() {
    for (int i = 0; i <= 1; i++) {
        this->specBuffer[i].Init(0, 0, this->SPEC_MAP_SIZE_X, this->SPEC_MAP_SIZE_Y, 4, 8);
    }
}

// STRIPPED (guess)
void cSpecularMap::Destroy() {
    for (int i = 0; i <= 1; i++) {
        this->specBuffer[i].Destroy();
    }
}

cQuarterSizeMap::cQuarterSizeMap() {
    this->QUARTER_SIZE_X = 0x140;
    this->QUARTER_SIZE_Y = 0xF0;
}

cQuarterSizeMap::~cQuarterSizeMap() {}

void cQuarterSizeMap::Init(int create_depth_buffer, int texture_format, int buffer_function) {
    this->quarterSizeBuffer.Init(0, 0, this->QUARTER_SIZE_X, this->QUARTER_SIZE_Y, texture_format, buffer_function);

    this->DepthBufferFlag = create_depth_buffer;
    if (create_depth_buffer != 0) {
        this->quarterSizeDepthBuffer.Init(0, 0, this->QUARTER_SIZE_X, this->QUARTER_SIZE_Y, 17, 6);
    }
}

// STRIPPED
void cQuarterSizeMap::Destroy() {
    this->quarterSizeBuffer.Destroy();

    if (this->DepthBufferFlag) {
        this->quarterSizeDepthBuffer.Destroy();
    }
}

// STRIPPED
cFullSizeMap::cFullSizeMap() {}
// STRIPPED
cFullSizeMap::~cFullSizeMap() {}
// STRIPPED
void cFullSizeMap::Init() {}
// STRIPPED
void cFullSizeMap::Destroy() {}

// STRIPPED
void eFinish() {}

#include "dolphin/gx/GXPriv.h"

static inline void write_bp_cmd(unsigned long cmd) {
    GX_WRITE_U8(GX_LOAD_BP_REG);
    GX_WRITE_U32(cmd);
}

eFogParams g_FogParams;
// STRIPPED
static const int DisableDistantFog = 0;
static int FogEnableState = 0;
// static const float FogStart;
// static const float FogEnd;
// static const float FogIntensityScale;
// static const float FogBrightnessScale;
static int prevFogDistance = -1;
static int prevFogColour = -1;
unsigned char fog_red = 0;
unsigned char fog_green = 0;
unsigned char fog_blue = 0;

void eSetFogConstantZero() {
    unsigned char fog_r = fog_red;
    unsigned char fog_g = fog_green;
    unsigned char fog_b = fog_blue;
    int fog_colour = 0;

    if (fog_colour != prevFogColour) {
        write_bp_cmd((fog_b << 0) | (fog_g << 8) | (fog_r << 16) | (0xF2 << 24));

        gx->bpSentNot = prevFogColour = fog_colour;
    }
}

void eSetFogConstantColour() {
    // Local variables
    int fog_colour; // r12
    unsigned char fog_r = g_FogParams.FogColor.r;
    unsigned char fog_g = g_FogParams.FogColor.g;
    unsigned char fog_b = g_FogParams.FogColor.b;

    fog_colour = int((fog_r + fog_g + fog_b) * FogCurrentBrightness);
    if (fog_colour != prevFogColour) {
        prevFogColour = fog_colour;

        int scaled_r = int(fog_r * FogCurrentBrightness);
        int scaled_g = int(fog_g * FogCurrentBrightness);
        int scaled_b = int(fog_b * FogCurrentBrightness);

        write_bp_cmd(((scaled_b << 0) & 0x0000FF) | ((scaled_g << 8) & 0x00FF00) | ((scaled_r << 16) & 0xFF0000) | (0xF2 << 24));
        gx->bpSentNot = 0;
    }
}

void eSetupFog(eView *view) {
    RegionInfo.CalculateRegionInfo(view, REGION_FOG, 0);

    float fogscale = RegionInfo.DistFogPower * (1.0f - RegionInfo.FogFalloff) * 2.0f;

    if (fogscale > 0.00001f) {

        g_FogParams.Start = 30.0f / fogscale;
        g_FogParams.End = 200.0f / fogscale;

        unsigned int colour = RegionInfo.DistFogColour;
        g_FogParams.Intensity = (float)(colour >> 24) * 1.5f;
        g_FogParams.FalloffY = RegionInfo.FogFalloffY;

        g_FogParams.FogColor.r = (unsigned char)(float)(colour & 0xFF);
        g_FogParams.FogColor.g = (unsigned char)(float)((colour >> 8) & 0xFF);
        g_FogParams.FogColor.b = (unsigned char)(float)((colour >> 16) & 0xFF);

    } else {

        g_FogParams.Intensity = 0.0f;
    }
}

void eSetFogEnable(int nDistance) {
    static int fog_enable = -1;

    if (nDistance == 0) {

        if (fog_enable != -1) {

            GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, g_FogParams.FogColor);
            prevFogDistance = nDistance;
            fog_enable = -1;
        }

    } else if (fog_enable != nDistance) {

        float start = g_FogParams.Start;
        float intensity = g_FogParams.Intensity;
        float end = g_FogParams.End;

        GXSetFog(GX_FOG_LIN, start, end, 1.0f, intensity, g_FogParams.FogColor);
        fog_enable = nDistance;
    }
}

void eSetFogState(TextureInfo *texture_info, GXBlendMode blend_mode) {
    if (FogEnableState == 0 || texture_info->NameHash == 0x57A4740E || texture_info->NameHash == 0x67688AB0) {

        eSetFogEnable(0);

    } else if (blend_mode == GX_BM_BLEND) {

        eSetFogEnable(1);
        eSetFogConstantZero();

    } else {

        eSetFogEnable(1);
        eSetFogConstantColour();
    }
}

// end
void eSetFogEnableState(int state) {
    FogEnableState = state;
}

void eSetFogBrightnessConstant(float brightness) {
    FogCurrentBrightness = brightness;
}

// Las cuatro entradas del banco de swap del TEV y el ultimo escenario tocado:
// eResetTevSwapStages deshace hacia atras lo que eSetTevSwapStage dejo puesto.
// r59 -- ORDEN de .data: modo, canal y seleccion, y lastTevSwapStageID DETRAS
// del hueco de 8 B (0x8041B014).
static unsigned char eTevSwapModeTable[4][4] = {
    { 0, 1, 2, 3 },
    { 0, 0, 0, 0 },
    { 1, 1, 1, 1 },
    { 2, 2, 2, 2 },
};
static GXTevColorChan eTevSwapColorChannelTable[4] = { GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA };
static GXTevSwapSel eTevSwapSelectionTable[4] = { GX_TEV_SWAP0, GX_TEV_SWAP1, GX_TEV_SWAP2, GX_TEV_SWAP3 };
static int lastTevSwapStageID = -1;
SlotPool *eTextureBucketSlotPool = 0;
SlotPool *eDataRenderSlotPool = 0;
int g_NumTextureBuckets = 0;
int SphericalPS = 1;
float testl1[2] = {0.6666667f, 0.6666667f};
float testl2[2] = {0.4f, 0.4f};
float testl3[2] = {0.2857143f, 0.2857143f};
float teste1[2] = {0.24999869f, 0.24999869f};
float teste2[2] = {0.031257585f, 0.031257585f};
float teste3[2] = {0.0025913713f, 0.0025913713f};
float testc0[2] = {0.0f, 0.0f};
float testc1[2] = {1.0f, 1.0f};
void eLoadTevSwapTable() {

    unsigned int i;

    for (i = 0; i < 4; i++) {

        GXSetTevSwapModeTable(eTevSwapSelectionTable[i], eTevSwapColorChannelTable[eTevSwapModeTable[i][0]], eTevSwapColorChannelTable[eTevSwapModeTable[i][1]], eTevSwapColorChannelTable[eTevSwapModeTable[i][2]], eTevSwapColorChannelTable[eTevSwapModeTable[i][3]]);
    }
}

void eSetTevSwapStage(GXTevStageID stage, int ras_sel, int tex_sel) {
    lastTevSwapStageID = stage;
    GXSetTevSwapMode(stage, eTevSwapSelectionTable[ras_sel], eTevSwapSelectionTable[tex_sel]);
}

void eResetTevSwapStages() {
    while (lastTevSwapStageID >= 0) {

        GXSetTevSwapMode((GXTevStageID)lastTevSwapStageID, eTevSwapSelectionTable[0], eTevSwapSelectionTable[0]);

        lastTevSwapStageID--;
    }
}

void eForceResetTevSwapStages() {
    lastTevSwapStageID = -1;

    // Los dos escenarios que el resto del motor deja tocados.
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    // Y la tabla de swap por defecto: RGBA, RRRA, GGGA, BBBA.
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
}

void eResetIndirectTextureSetup() {
    GXSetNumIndStages(0);

    // Las seis etapas vuelven a directo.
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevDirect(GX_TEVSTAGE2);
    GXSetTevDirect(GX_TEVSTAGE3);
    GXSetTevDirect(GX_TEVSTAGE4);
    GXSetTevDirect(GX_TEVSTAGE5);
}

// ---- eDataRender::Render y sus inlines: declaraciones previas ----
enum eModelPixelShaderId {
    EPSHADER_NONE = -1,
    EPSHADER_WORLD = 0,
    EPSHADER_WORLDREFLECT = 1,
    EPSHADER_CARLIGHT_HARMONICS = 2,
    EPSHADER_DYNAMICLIGHT = 3,
    EPSHADER_LAYERED_SKY = 4,
    EPSHADER_PARTICLE = 5,
    EPSHADER_SHADOWMODULATE = 6,
    EPSHADER_FIRST = 0,
    EPSHADER_LAST = 6,
    EPSHADER_NUM = 7,
};

int ps_Model(eModelPixelShaderId opt, eLightMaterial *light_material, int enable_envmap);
int ps_Lighting(int numLights, int bVertexColour, int opt);
int ps_Lighting2VertexChannels(int opt);
int psReplaceNoAlpha();
void gx_Lighting();
int vsModel(int VertexFormat, int VertexDescription);
int eSetTexture(TextureInfo *texture_info, int stage);

extern int SphericalPS;
extern int EnableEnvMap;
extern bool EnableRainIn2P;
extern TextureInfo *pTextureInfoCarSelectEnvMap;
extern Mtx g_ScreenPositionMatrix;
extern bVector4 eLpos[3];

static int cBfW = 0;
int PrintSolidViewLocalWorldTest = 0;

// total size: 0x6
struct eVect3i16 {
    short x;
    short y;
    short z;
};

static const float recipUV = 1.0f / 256.0f;
static const float recipW = 1.0f / 255.0f;
static const float recipN = 1.0f / 64.5f;
static const int nBf = 2;
static const int nTv = 48;
struct eTextureBucket;
bTList<eTextureBucket> g_TextureBucketList[16];
static bVector3 tP[nBf][nTv];
static bVector3 tPC;
static bVector3 tN[nBf][nTv];
static bVector3 tNC;
bVector4 MyLightPos[3] = {bVector4(0.0f, 10.0f, 0.0f, 0.0f), bVector4(0.0f, -10.0f, 0.0f, 0.0f), bVector4(0.0f, 0.0f, 10.0f, 0.0f)};
extern float pt[2];
static eVect3i16 tT[nBf][nTv];
static const float epsW1 = 0.25f;

static inline void eePrefetch(void *offs, void *addr) {
    asm("dcbt %0, %1" : : "b"(offs), "r"(addr));
}

static inline float eTexGetScrollTimeStep(TextureInfo *t) {
    return *(const short *)((const char *)t + 0x58) * (1.0f / 256.0f);
}
static inline float eTexGetScrollSpeedS(TextureInfo *t) {
    return *(const short *)((const char *)t + 0x5A) * (1.0f / 1024.0f);
}
static inline float eTexGetScrollSpeedT(TextureInfo *t) {
    return *(const short *)((const char *)t + 0x5C) * (1.0f / 1024.0f);
}
static inline float eTexGetOffsetS(TextureInfo *t) {
    return *(const short *)((const char *)t + 0x5E) * (1.0f / 1024.0f);
}
static inline float eTexGetOffsetT(TextureInfo *t) {
    return *(const short *)((const char *)t + 0x60) * (1.0f / 1024.0f);
}
static inline float eTexGetScrollS(TextureInfo *t, float time_s) {
    return t->GetScroll(time_s, eTexGetScrollSpeedS(t), t->ScrollType, eTexGetScrollTimeStep(t));
}
static inline float eTexGetScrollT(TextureInfo *t, float time_s) {
    return t->GetScroll(time_s, eTexGetScrollSpeedT(t), t->ScrollType, eTexGetScrollTimeStep(t));
}

// ============================================================================
//  eco_dr2_body.txt
//  eDataRender::Render(TextureInfo *)  -  5980 B / 1495 instrucciones
//  con eSetTextureAnimation, eDataSetLighting, eDataRenderStripBlend y
//  eDataRenderStripRigid expandidos (los cuatro los INLINA el original).
//
//  Los comentarios // NNNN son la linea del EcstasyEx.cpp ORIGINAL segun
//  symbols/debug_lines.txt.  Las lineas no afectan al codigo generado: sirven
//  para auditar el orden de sentencias.
//
//  Requiere el bloque de eco_dr2_decl.txt delante.
// ============================================================================

// ----------------------------------------------------------------------------
// eSetTextureAnimation:  FUNCION LIBRE (el volcado la lista como
// `inline int eSetTextureAnimation(struct TextureInfo *, int)`, no como
// miembro).  Va ANTES de struct eDataRender para que se inline en Render.
// Rango inlinado: 0x80116E7C - 0x80117024, lineas 3912-3947.
// ----------------------------------------------------------------------------
static inline int eSetTextureAnimation(TextureInfo *texture_info, int stage) {
    Mtx scrollingUV_matrix;                                                   // 3909
    float offset_s = 0.0f;                                                    // 3910
    float offset_t;                                                           // 3911

    PSMTXIdentity(scrollingUV_matrix);                                        // 3912

    if (texture_info->ScrollType != 0) {                                      // 3918
        float world_time = WorldTimer.GetSeconds();                           // 3920  (Timer.hpp:98)

        if (texture_info->ScrollType == TEXSCROLL_OFFSETSCALE) {              // 3922
            offset_s = -eTexGetOffsetS(texture_info);                         // 3924  (Texture.hpp:331)
            offset_t = -eTexGetOffsetT(texture_info);                         // 3925  (Texture.hpp:332)
        } else {
            // El volcado declara estas dos locales SIN ubicacion: no emiten
            // codigo.  Dejarlas sin inicializar es lo unico que garantiza
            // cero instrucciones (vease "dudas" #4).
            float speed_s = 1.0f;                                             // 3930
            float speed_t;                                                    // 3931

            offset_s = eTexGetScrollS(texture_info, world_time);              // 3939  (Texture.hpp:339)
            offset_t = eTexGetScrollT(texture_info, world_time);              // 3940  (Texture.hpp:340)
        }

        PSMTXTransApply(scrollingUV_matrix, scrollingUV_matrix,               // 3944
                        offset_s, offset_t, 0.0f);
    }

    GXLoadTexMtxImm(scrollingUV_matrix, GX_TEXMTX0 + stage * 3, GX_MTX2x4);   // 3947
    return 1;                                                                 // 3948
}

// ============================================================================
//  struct eDataRender  -  los cuatro cuerpos van DENTRO de la clase
//  (§8 del playbook: la copia fuera de linea al final del TU delata cuerpo
//  in-class).  Orden de declaracion tomado del volcado :721444.
// ============================================================================
struct eDataRender : public bTNode<eDataRender> {

    static void *operator new(unsigned int size) {
        return bOMalloc(eDataRenderSlotPool);
    }

    static void operator delete(void *ptr) {
        bFree(eDataRenderSlotPool, ptr);
    }

    eDataRender(void *data, unsigned short entries, eView *view, eSolid *solid, unsigned int flags, bMatrix4 *local_world,
                eLightContext *light_context, eLightMaterial *light_mat, bMatrix4 *blending_matrices, eDataRenderDynamic *drd) {
        Data = data;
        View = view;
        Solid = solid;
        Flags = flags;
        LightContext = light_context;
        LightMaterial = light_mat;
        LocalWorld = local_world;
        BlendingMatrices = blending_matrices;
        Entries = entries;
        DRD = *drd;
    }

    ~eDataRender() {}

    // ------------------------------------------------------------------------
    // eDataSetLighting  -  inlinada en Render, rango 0x8011710C - 0x80117284,
    // lineas 2938-3090.
    // ------------------------------------------------------------------------
    void eDataSetLighting(unsigned short strip_flags, unsigned short disable_mask, Mtx &texcoordgen_reflection_matrix, int skinned_mesh,
                          TextureInfo *texture) {                             // 2938

        if (LightContext != 0 && (strip_flags & 0x80)) {                      // 2940

            ps_Lighting2VertexChannels(0);                                    // 2967

            if (skinned_mesh == 0) {                                          // 2972
                {
                    int enable_envmap;                                        // 2975

                    if (pTextureInfoCarSelectEnvMap != 0) {                   // 2978
                        eSetTexture(pTextureInfoCarSelectEnvMap, 1);          // 2979
                    } else {                                                  // 2982
                        eSetTexture(SphereMap.sphereTexture, 1);              // 2983  (EcstasyEx.hpp:123 = cSphereMap::GetTexture)
                    }

                    enable_envmap = strip_flags & disable_mask;               // 2986
                    ps_Model(EPSHADER_CARLIGHT_HARMONICS, LightMaterial,      // 2987
                             (enable_envmap >> 8) & 1);
                }
            } else {
                gx_Lighting();                                                // 2992
                ps_Lighting(3, 0, 0);                                         // 2993

                ps_Model(EPSHADER_DYNAMICLIGHT, 0, 0);                        // 2995
            }

        } else {

            ps_NoLighting(1, 0);                                              // 3001

            if (strip_flags & 0x40) {                                         // 3003
                {
                    bool rain_disabled_forP1;                                 // 3005
                    int view_id = View->GetID();                              // 3006  (Ecstasy.hpp:874 = eView::GetID)

                    GXLoadTexMtxImm(texcoordgen_reflection_matrix,            // 3007
                                    GX_TEXMTX2, GX_MTX3x4);

                    rain_disabled_forP1 = !EnableRainIn2P &&                  // 3012
                                          (eGetCurrentViewMode() == EVIEWMODE_TWOH ||
                                           eGetCurrentViewMode() == EVIEWMODE_TWOV);

                    if (view_id == 1) {                                       // 3014
                        eSetTexture(SpecularMap.specBuffer[0].GetTexture(), 1); // 3019  (EcstasyEx.hpp:102)
                    } else if (view_id == 2) {                                // 3021
                        eSetTexture(SpecularMap.specBuffer[1].GetTexture(), 1); // 3027
                    }

                    if (view_id >= 16 && view_id <= 21) {                     // 3032
                        psReplaceNoAlpha();                                   // 3034
                    } else {
                        ps_Model(EPSHADER_WORLDREFLECT, 0, 0);                // 3054
                    }
                }
            } else if (Flags & 0x20000) {                                     // 3075
                if (texture->ClassNameHash == 0x89BBF85A) {                   // 3077
                    eSetBlendModeSrcInvSrc();                                 // 3079
                    ps_Model(EPSHADER_SHADOWMODULATE, 0, 0);                  // 3080
                } else {
                    eSetBlendModeSrcInvSrc();                                 // 3084
                    ps_Model(EPSHADER_LAYERED_SKY, 0, 0);                     // 3085
                }
            } else {
                ps_Model(EPSHADER_WORLD, 0, 0);                               // 3090
            }
        }
    }

    // ------------------------------------------------------------------------
    // eDataRenderStripBlend  -  inlinada en Render, rango 0x801172BC -
    // 0x80117714, lineas 3110-3320.  Es el tramo paired-single.
    // ------------------------------------------------------------------------
    void eDataRenderStripBlend(eStripEntry *strip_entry, int crtVtxFmt, unsigned short strip_flags) {
        // 3110/3111 (strip_dl, strip_data) no emiten insn propia: el mapa de
        // lineas pone las DOS cargas y el add en 3112, y con la suma escrita
        // en una sola sentencia el `lhz` del tamano sale PRIMERO, como el
        // objetivo (92 -> 78 diffs).
        signed char *crt_strip_data = (signed char *)strip_entry->DataOffset + // 3112
                                      strip_entry->DataDisplayListSize;

        eePrefetch(0, crt_strip_data);                                        // 3113  (EcstasyEx.hpp:278)
        eePrefetch(0, BlendingMatrices);                                      // 3114  (EcstasyEx.hpp:274/278)

        // El `beq cr4` de 3121 salta a 0x80117714 = el test del bucle EXTERNO
        // (linea 4201), o sea que TODO lo que sigue -incluido GXBegin- esta
        // dentro de este `if`.  El volcado da UN SOLO bloque anonimo con
        // `int i` cubriendo los dos bucles: `i` se declara una vez y se reusa.
        if (strip_flags & 0x4000) {                                           // 3121
            int i = 0;                                                        // 3127

            for (; i < strip_entry->NumVerts; i++) {                          // 3128
                {
                    int vMI0;                                                 // 3131
                    int vMI1;                                                 // 3132
                    int vMI2;                                                 // 3133
                    float vW0;                                                // 3134
                    float vW1;
                    float vW2;
                    bMatrix4 mB2L;
                    sn_ps FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9;
                    sn_ps FP10, FP11, FP12, FP13, FP14, FP15, FP16, FP17;

                    eIdentity(&mB2L);                                         // 3135  (bMath.hpp:1276 -> PSMTX44Identity)

                    tPC.x = ((float *)crt_strip_data)[0];                     // 3137
                    tPC.y = ((float *)crt_strip_data)[1];                     // 3138
                    tPC.z = ((float *)crt_strip_data)[2];                     // 3139

                    tN[cBfW][i].x = crt_strip_data[12] * recipN;              // 3147
                    tN[cBfW][i].y = crt_strip_data[13] * recipN;              // 3148
                    tN[cBfW][i].z = crt_strip_data[14] * recipN;              // 3149

                    tT[cBfW][i].x = ((unsigned short *)crt_strip_data)[8];    // 3152
                    tT[cBfW][i].y = ((unsigned short *)crt_strip_data)[9];    // 3153

                    vMI0 = *(int *)&tPC.x & 0xFF;                             // 3156
                    vMI1 = *(int *)&tPC.y & 0xFF;                             // 3157
                    vMI2 = *(int *)&tPC.z & 0xFF;                             // 3158

                    vW1 = (unsigned int)((unsigned char *)crt_strip_data)[15] * recipW;     // 3161

                    if (vW1 > epsW1) {                                        // 3163
                        vW0 = 1.0f - vW1;                                     // 3165
                    } else {                                                  // 3168
                        vW0 = 1.0f;                                           // 3172
                        vW1 = 0.0f;                                           // 3173
                    }

                    FP0 = __builtin_psq_l(&BlendingMatrices[vMI0].v0.x, 0, 0);      // 3203
                    FP1 = __builtin_psq_l(&BlendingMatrices[vMI0].v0.z, 0, 0);      // 3204

                    FP2 = __builtin_psq_l(&BlendingMatrices[vMI0].v1.x, 0, 0);      // 3206
                    FP3 = __builtin_psq_l(&BlendingMatrices[vMI0].v1.z, 0, 0);      // 3207

                    FP4 = __builtin_psq_l(&BlendingMatrices[vMI0].v2.x, 0, 0);      // 3209
                    FP5 = __builtin_psq_l(&BlendingMatrices[vMI0].v2.z, 0, 0);      // 3210

                    FP6 = __builtin_psq_l(&BlendingMatrices[vMI0].v3.x, 0, 0);      // 3212
                    FP7 = __builtin_psq_l(&BlendingMatrices[vMI0].v3.z, 0, 0);      // 3213

                    FP8 = __builtin_psq_l(&vW0, 0, 0);                        // 3215
                    FP8 = __builtin_ps_merge00(FP8, FP8);                     // 3216

                    FP10 = __builtin_ps_mul(FP0, FP8);                        // 3218
                    FP11 = __builtin_ps_mul(FP1, FP8);                        // 3219

                    FP12 = __builtin_ps_mul(FP2, FP8);                        // 3221
                    FP13 = __builtin_ps_mul(FP3, FP8);                        // 3222

                    FP14 = __builtin_ps_mul(FP4, FP8);                        // 3224
                    FP15 = __builtin_ps_mul(FP5, FP8);                        // 3225

                    FP16 = __builtin_ps_mul(FP6, FP8);                        // 3227
                    FP17 = __builtin_ps_mul(FP7, FP8);                        // 3228

                    if (vW1 > epsW1) {                                        // 3230
                        FP0 = __builtin_psq_l(&BlendingMatrices[vMI1].v0.x, 0, 0);  // 3232
                        FP1 = __builtin_psq_l(&BlendingMatrices[vMI1].v0.z, 0, 0);  // 3233

                        FP2 = __builtin_psq_l(&BlendingMatrices[vMI1].v1.x, 0, 0);  // 3235
                        FP3 = __builtin_psq_l(&BlendingMatrices[vMI1].v1.z, 0, 0);  // 3236

                        FP4 = __builtin_psq_l(&BlendingMatrices[vMI1].v2.x, 0, 0);  // 3238
                        FP5 = __builtin_psq_l(&BlendingMatrices[vMI1].v2.z, 0, 0);  // 3239

                        FP6 = __builtin_psq_l(&BlendingMatrices[vMI1].v3.x, 0, 0);  // 3241
                        FP7 = __builtin_psq_l(&BlendingMatrices[vMI1].v3.z, 0, 0);  // 3242

                        FP9 = __builtin_psq_l(&vW1, 0, 0);                    // 3244
                        FP9 = __builtin_ps_merge00(FP9, FP9);                 // 3245

                        FP10 = __builtin_ps_madd(FP0, FP9, FP10);             // 3248
                        FP11 = __builtin_ps_madd(FP1, FP9, FP11);             // 3249

                        FP12 = __builtin_ps_madd(FP2, FP9, FP12);             // 3251
                        FP13 = __builtin_ps_madd(FP3, FP9, FP13);             // 3252

                        FP14 = __builtin_ps_madd(FP4, FP9, FP14);             // 3254
                        FP15 = __builtin_ps_madd(FP5, FP9, FP15);             // 3255

                        FP16 = __builtin_ps_madd(FP6, FP9, FP16);             // 3257
                        FP17 = __builtin_ps_madd(FP7, FP9, FP17);             // 3258
                    }

                    __builtin_psq_st(&mB2L.v0.x, FP10, 0, 0);                  // 3260
                    __builtin_psq_st(&mB2L.v0.z, FP11, 0, 0);                  // 3261

                    __builtin_psq_st(&mB2L.v1.x, FP12, 0, 0);                  // 3264
                    __builtin_psq_st(&mB2L.v1.z, FP13, 0, 0);                  // 3265

                    __builtin_psq_st(&mB2L.v2.x, FP14, 0, 0);                  // 3267
                    __builtin_psq_st(&mB2L.v2.z, FP15, 0, 0);                  // 3268

                    __builtin_psq_st(&mB2L.v3.x, FP16, 0, 0);                  // 3270
                    __builtin_psq_st(&mB2L.v3.z, FP17, 0, 0);                  // 3271

                    eMulVector(&tP[cBfW][i], &mB2L, &tPC);                    // 3277

                    crt_strip_data += 20;                                     // 3284
                    eePrefetch(0, crt_strip_data);                            // 3285  (EcstasyEx.hpp:278)
                }
            }                                                                 // 3286

            GXBegin(GX_TRIANGLESTRIP, (GXVtxFmt)crtVtxFmt, strip_entry->NumVerts); // 3303

            for (i = 0; i < strip_entry->NumVerts; i++) {                     // 3304
                GXPosition3f32(tP[cBfW][i].x, tP[cBfW][i].y, tP[cBfW][i].z);  // 3306  (GXVert.h:334)
                GXNormal3f32(tN[cBfW][i].x, tN[cBfW][i].y, tN[cBfW][i].z);    // 3307  (GXVert.h:351)
                GXTexCoord2s16(tT[cBfW][i].x, tT[cBfW][i].y);                 // 3308  (GXVert.h:373)

                eePrefetch(0, &tP[cBfW][i + 1]);                              // 3310
                eePrefetch(0, &tN[cBfW][i + 1]);                              // 3311
                eePrefetch(0, &tT[cBfW][i + 1]);                              // 3312
            }                                                                 // 3313

            GXEnd();                                                          // 3315

            if (cBfW < 1) {                                                   // 3317
                cBfW = cBfW + 1;                                              // 3318
            } else {                                                          // 3319
                cBfW = 0;                                                     // 3320
            }
        }
    }

    // ------------------------------------------------------------------------
    // eDataRenderStripRigid  -  inlinada en Render, rango 0x80117740 -
    // 0x801181C0, lineas 3342-3811.
    //
    // Los desplazamientos y las zancadas estan transcritos LITERALMENTE del
    // objetivo. Varios son "raros" (el 16-bit del case 0x08 lee en +0, el uv
    // del case 0x02 en +2, la zancada de 0x0A/0x12/0x18 es 5 y no 6): son
    // erratas del original, NO errores de transcripcion.  El volcado da los
    // nombres reales de esas constantes (mw_dwarfdump.nothpp:749240-749345,
    // VF2_MIXED_xx_xx_xx_xx_NRM/CLR/UVS y VF2_MIXED_NX16_MX8_STRIDE); aqui van
    // como literales para que no haya ambiguedad.
    // ------------------------------------------------------------------------
    void eDataRenderStripRigid(eStripEntry *strip_entry, int crtVtxFmt, unsigned short strip_flags) {
        void *strip_dl = (void *)strip_entry->DataOffset;                     // 3342
        void *strip_data = (void *)((unsigned char *)strip_dl +               // 3343
                                    strip_entry->DataDisplayListSize);
        int vertex_format = strip_entry->VertexFormat;                        // 3345
        int vertex_description = strip_entry->VertexDescription & 0x3F;       // 3346
        int vertex_display_list_packed;                                       // 3347
        int vertex_display_list_header;                                       // 3348

        if ((strip_entry->VertexDescription & 0x80) != 0 &&                   // 3385
            strip_entry->DataDisplayListSize != 0) {

            GXCallDisplayList(strip_dl, strip_entry->DataDisplayListSize);    // 3400

        } else {
            unsigned char *ib8 = (unsigned char *)strip_dl;                   // 3430
            unsigned short *ib16 = (unsigned short *)ib8;                     // 3431
            unsigned char *mixed_index_08;
            unsigned short *mixed_index_16;
            unsigned short *mixed_offset_index_16;
            unsigned int data_stride;
            unsigned int data_stride_adjust;
            unsigned int jj;

            eePrefetch(0, strip_dl);                                          // 3434  (EcstasyEx.hpp:278)

            GXBegin(GX_TRIANGLESTRIP, (GXVtxFmt)crtVtxFmt, strip_entry->NumVerts); // 3436

            if (vertex_format == 2 || vertex_format == 0x16) {                // 3438
                switch (vertex_description) {                                 // 3440

                case -1:                                                      // (vease "dudas" #1)
                    break;

                case 0x00:                                                    // 3442
                    {                                                         // 3443
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3444
                            jj = i * 4;                                       // 3446
                            unsigned char *mixed_index_08 = &ib8[jj];          // 3447

                            GXPosition1x8(ib8[jj]);                           // 3449
                            GXNormal1x8(mixed_index_08[1]);                   // 3450
                            GXColor1x8(mixed_index_08[2]);                    // 3451
                            GXColor1x8(mixed_index_08[2]);                    // 3452
                            GXTexCoord1x8(mixed_index_08[3]);                 // 3453
                        }                                                     // 3454
                    }
                    break;                                                    // 3455

                case 0x02:                                                    // 3457
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3459
                            jj = i * 5;                                       // 3461
                            mixed_index_08 = &ib8[jj];                        // 3462
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3463

                            GXPosition1x8(ib8[jj]);                           // 3465
                            GXNormal1x8(mixed_index_08[1]);                   // 3466
                            GXColor1x8(mixed_index_08[2]);                    // 3467
                            GXColor1x8(mixed_index_08[2]);                    // 3468
                            GXTexCoord1x16(mixed_index_16[1]);                // 3469
                        }                                                     // 3470
                    }
                    break;                                                    // 3471

                case 0x04:                                                    // 3472
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3473
                            jj = i * 5;                                       // 3475
                            mixed_index_08 = &ib8[jj];                        // 3476
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3477

                            GXPosition1x8(ib8[jj]);                           // 3479
                            GXNormal1x8(mixed_index_08[1]);                   // 3480
                            GXColor1x16(mixed_index_16[1]);                   // 3481
                            GXColor1x16(mixed_index_16[1]);                   // 3482
                            GXTexCoord1x8(mixed_index_08[4]);                 // 3483
                        }                                                     // 3484
                    }
                    break;                                                    // 3485

                case 0x08:                                                    // 3486
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3487
                            jj = i * 5;                                       // 3489
                            mixed_index_08 = &ib8[jj];                        // 3490
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3491

                            GXPosition1x8(ib8[jj]);                           // 3493
                            GXNormal1x16(mixed_index_16[0]);                  // 3494
                            GXColor1x8(mixed_index_08[3]);                    // 3495
                            GXColor1x8(mixed_index_08[3]);                    // 3496
                            GXTexCoord1x8(mixed_index_08[4]);                 // 3497
                        }                                                     // 3498
                    }
                    break;                                                    // 3499

                case 0x10:                                                    // 3500
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3501
                            jj = i * 5;                                       // 3503
                            mixed_index_08 = &ib8[jj];                        // 3504

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3507
                            GXNormal1x8(mixed_index_08[2]);                   // 3508
                            GXColor1x8(mixed_index_08[3]);                    // 3509
                            GXColor1x8(mixed_index_08[3]);                    // 3510
                            GXTexCoord1x8(mixed_index_08[4]);                 // 3511
                        }                                                     // 3512
                    }
                    break;                                                    // 3513

                case 0x18:                                                    // 3516
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3517
                            jj = i * 5;                                       // 3519
                            mixed_index_08 = &ib8[jj];                        // 3520
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3521

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3523
                            GXNormal1x16(mixed_index_16[1]);                  // 3524
                            GXColor1x8(mixed_index_08[4]);                    // 3525
                            GXColor1x8(mixed_index_08[4]);                    // 3526
                            GXTexCoord1x8(mixed_index_08[5]);                 // 3527
                        }                                                     // 3528
                    }
                    break;                                                    // 3529

                case 0x12:                                                    // 3530
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3531
                            jj = i * 5;                                       // 3533
                            mixed_index_08 = &ib8[jj];                        // 3534
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3535

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3537
                            GXNormal1x8(mixed_index_08[2]);                   // 3538
                            GXColor1x8(mixed_index_08[3]);                    // 3539
                            GXColor1x8(mixed_index_08[3]);                    // 3540
                            GXTexCoord1x16(mixed_index_16[2]);                // 3541
                        }                                                     // 3542
                    }
                    break;                                                    // 3543

                case 0x06:                                                    // 3544
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3545
                            jj = i * 6;                                       // 3547
                            mixed_index_08 = &ib8[jj];                        // 3548
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3549

                            GXPosition1x8(ib8[jj]);                           // 3551
                            GXNormal1x8(mixed_index_08[1]);                   // 3552
                            GXColor1x16(mixed_index_16[1]);                   // 3553
                            GXColor1x16(mixed_index_16[1]);                   // 3554
                            GXTexCoord1x16(mixed_index_16[2]);                // 3555
                        }                                                     // 3556
                    }
                    break;                                                    // 3557

                case 0x0C:                                                    // 3558
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3559
                            jj = i * 6;                                       // 3561
                            mixed_index_08 = &ib8[jj];                        // 3562
                            mixed_offset_index_16 = (unsigned short *)&ib8[jj + 1]; // 3564

                            GXPosition1x8(ib8[jj]);                           // 3566
                            GXNormal1x16(mixed_offset_index_16[0]);           // 3567
                            GXColor1x16(mixed_offset_index_16[1]);            // 3568
                            GXColor1x16(mixed_offset_index_16[1]);            // 3569
                            GXTexCoord1x8(mixed_index_08[5]);                 // 3570
                        }                                                     // 3571
                    }
                    break;                                                    // 3572

                case 0x14:                                                    // 3573
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3574
                            jj = i * 6;                                       // 3576
                            mixed_index_08 = &ib8[jj];                        // 3577
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3578

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3580
                            GXNormal1x8(mixed_index_08[2]);                   // 3581
                            GXColor1x16(mixed_index_16[1]);                   // 3582
                            GXColor1x16(mixed_index_16[1]);                   // 3583
                            GXTexCoord1x8(mixed_index_08[5]);                 // 3584
                        }                                                     // 3585
                    }
                    break;                                                    // 3586

                case 0x0A:                                                    // 3587
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3588
                            jj = i * 5;                                       // 3590
                            mixed_index_08 = &ib8[jj];                        // 3591
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3592

                            GXPosition1x8(ib8[jj]);                           // 3594
                            GXNormal1x16(mixed_index_16[0]);                  // 3595
                            GXColor1x8(mixed_index_08[5]);                    // 3596
                            GXColor1x8(mixed_index_08[5]);                    // 3597
                            GXTexCoord1x16(mixed_index_16[2]);                // 3598
                        }                                                     // 3599
                    }
                    break;                                                    // 3600

                case 0x1C:                                                    // 3603
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3604
                            jj = i * 7;                                       // 3606
                            mixed_index_08 = &ib8[jj];                        // 3607
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3608

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3610
                            GXNormal1x16(mixed_index_16[1]);                  // 3611
                            GXColor1x16(mixed_index_16[2]);                   // 3612
                            GXColor1x16(mixed_index_16[2]);                   // 3613
                            GXTexCoord1x8(mixed_index_08[6]);                 // 3614
                        }                                                     // 3615
                    }
                    break;                                                    // 3616

                case 0x1A:                                                    // 3617
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3618
                            jj = i * 7;                                       // 3620
                            mixed_index_08 = &ib8[jj];                        // 3621
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3622

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3624
                            GXNormal1x16(mixed_index_16[1]);                  // 3625
                            GXColor1x8(mixed_index_08[4]);                    // 3626
                            GXColor1x8(mixed_index_08[4]);                    // 3627
                            GXTexCoord1x16(mixed_index_16[2]);                // 3628
                        }                                                     // 3629
                    }
                    break;                                                    // 3630

                case 0x16:                                                    // 3631
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3632
                            jj = i * 7;                                       // 3634
                            mixed_index_08 = &ib8[jj];                        // 3635
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3636

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3638
                            GXNormal1x8(mixed_index_08[2]);                   // 3639
                            GXColor1x16(mixed_index_16[1]);                   // 3640
                            GXColor1x16(mixed_index_16[1]);                   // 3641
                            GXTexCoord1x16(mixed_index_16[2]);                // 3642
                        }                                                     // 3643
                    }
                    break;                                                    // 3644

                case 0x0E:                                                    // 3645
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3646
                            jj = i * 7;                                       // 3648
                            mixed_offset_index_16 = (unsigned short *)&ib8[jj + 1]; // 3651

                            GXPosition1x8(ib8[jj]);                           // 3653
                            GXNormal1x16(mixed_offset_index_16[0]);           // 3654
                            GXColor1x16(mixed_offset_index_16[1]);            // 3655
                            GXColor1x16(mixed_offset_index_16[1]);            // 3656
                            GXTexCoord1x16(mixed_offset_index_16[2]);         // 3657
                        }                                                     // 3658
                    }
                    break;                                                    // 3659

                case 0x1E:                                                    // 3661
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3662
                            jj = i * 8;                                       // 3664
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3665

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3668
                            GXNormal1x16(mixed_index_16[1]);                  // 3669
                            GXColor1x16(mixed_index_16[2]);                   // 3670
                            GXColor1x16(mixed_index_16[2]);                   // 3671
                            GXTexCoord1x16(mixed_index_16[3]);                // 3672
                        }                                                     // 3673
                    }
                    break;                                                    // 3686
                }

            } else if (vertex_format == 3 || vertex_format == 0x20 ||         // 3689
                       vertex_format == 1) {

                switch (vertex_description) {                                 // 3691

                case -1:                                                      // 3693  (vease "dudas" #1)
                    break;

                case 0x00:                                                    // 3700
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3701
                            jj = i * 3;                                       // 3708
                            unsigned char *mixed_index_08 = &ib8[jj];          // 3709

                            GXPosition1x8(ib8[jj]);                           // 3711
                            GXColor1x8(mixed_index_08[1]);                    // 3712
                            GXTexCoord1x8(mixed_index_08[2]);                 // 3713
                        }                                                     // 3714
                    }
                    break;                                                    // 3715

                case 0x02:                                                    // 3725
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3726
                            jj = i * 4;                                       // 3727
                            mixed_index_08 = &ib8[jj];                        // 3728
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3729

                            GXPosition1x8(ib8[jj]);                           // 3731
                            GXColor1x8(mixed_index_08[1]);                    // 3732
                            GXTexCoord1x16(mixed_index_16[1]);                // 3733
                        }                                                     // 3734
                    }
                    break;                                                    // 3735

                case 0x04:                                                    // 3736
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3737
                            jj = i * 4;                                       // 3739
                            mixed_index_08 = &ib8[jj];                        // 3740
                            mixed_offset_index_16 = (unsigned short *)&ib8[jj + 1]; // 3742

                            GXPosition1x8(ib8[jj]);                           // 3744
                            GXColor1x16(mixed_offset_index_16[0]);            // 3745
                            GXTexCoord1x8(mixed_index_08[3]);                 // 3746
                        }                                                     // 3747
                    }
                    break;                                                    // 3748

                case 0x10:                                                    // 3749
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3750
                            jj = i * 4;                                       // 3752
                            mixed_index_08 = &ib8[jj];                        // 3753

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3756
                            GXColor1x8(mixed_index_08[2]);                    // 3757
                            GXTexCoord1x8(mixed_index_08[3]);                 // 3758
                        }                                                     // 3759
                    }
                    break;                                                    // 3760

                case 0x12:                                                    // 3761
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3762
                            jj = i * 5;                                       // 3764
                            mixed_index_08 = &ib8[jj];                        // 3765
                            mixed_offset_index_16 = (unsigned short *)&ib8[jj + 1]; // 3767

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3769
                            GXColor1x8(mixed_index_08[2]);                    // 3770
                            GXTexCoord1x16(mixed_offset_index_16[1]);         // 3771
                        }                                                     // 3772
                    }
                    break;                                                    // 3773

                case 0x06:                                                    // 3774
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3775
                            jj = i * 5;                                       // 3777
                            mixed_offset_index_16 = (unsigned short *)&ib8[jj + 1]; // 3780

                            GXPosition1x8(ib8[jj]);                           // 3782
                            GXColor1x16(mixed_offset_index_16[0]);            // 3783
                            GXTexCoord1x16(mixed_offset_index_16[1]);         // 3784
                        }                                                     // 3785
                    }
                    break;                                                    // 3786

                case 0x14:                                                    // 3788
                case 0x1C:                                                    // 3789
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3790
                            jj = i * 5;                                       // 3791
                            mixed_index_08 = &ib8[jj];                        // 3792
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3793

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3795
                            GXColor1x16(mixed_index_16[1]);                   // 3796
                            GXTexCoord1x8(mixed_index_08[4]);                 // 3797
                        }                                                     // 3798
                    }
                    break;                                                    // 3799

                case 0x16:                                                    // 3801
                case 0x1E:                                                    // 3802
                    {
                        for (int i = 0; i < strip_entry->NumVerts; i++) {     // 3803
                            jj = i * 6;                                       // 3804
                            mixed_index_16 = (unsigned short *)&ib8[jj];      // 3805

                            GXPosition1x16(*(unsigned short *)&ib8[jj]);      // 3808
                            GXColor1x16(mixed_index_16[1]);                   // 3809
                            GXTexCoord1x16(mixed_index_16[2]);                // 3810
                        }                                                     // 3811
                    }
                    break;                                                    // 3812
                }
            }

            GXEnd();                                                          // 3815
        }
    }

    // ------------------------------------------------------------------------
    // Render  -  el simbolo, 0x80116AC0, 5980 B, lineas 3963-4235.
    // ------------------------------------------------------------------------
    void Render(TextureInfo *texture) {                                       // 3963
        static eSolid *pPrevSolid = 0;                                        // 3966  @0x8041B0A0
        static eView *pPrevView = 0;                                          // 3967  @0x8041B0A4
        static bMatrix4 *pPrevLocalWorld = 0;                                 // 3968  @0x8041B0A8

        static float *position_table = 0;                                     // 3971  @0x8041B0B0
        static unsigned short *position_table_16 = 0;                         // 3972  @0x8041B0B4
        static int *normal_table = 0;                                         // 3973  @0x8041B0B8
        static unsigned int *colour_table0 = 0;                               // 3974  @0x8041B0BC
        static unsigned int *colour_table1 = 0;                               // 3975  @0x8041B0C0
        static unsigned short *uv_table = 0;                                  // 3976  @0x8041B0C4

        bool CalcSet1;                                                        // 3978
        eStripEntry *strip_entry;
        unsigned short strip_flags;
        int ignore_solid_textures;
        unsigned short extra_strip_flags;
        unsigned int gloss_map_in_alpha;
        int draw_group;

        if (PrintSolidViewLocalWorldTest) {                                   // 3982
            if (LocalWorld != pPrevLocalWorld) {                              // 3992
                // Cuerpo VACIO en el original: GCC emite las 12 comparaciones
                // y nada mas (el ultimo `fcmpu`, 0x80116BF0, se queda sin salto).
                // Cada salto es `beq -> salida`, o sea jumpifnot(a != b): la
                // condicion es DISTINTO, no igual.
                if (pPrevLocalWorld != 0 &&                                   // 4011
                    LocalWorld->v0.x != pPrevLocalWorld->v0.x && LocalWorld->v0.y != pPrevLocalWorld->v0.y &&
                    LocalWorld->v0.z != pPrevLocalWorld->v0.z && LocalWorld->v0.w != pPrevLocalWorld->v0.w &&
                    LocalWorld->v1.x != pPrevLocalWorld->v1.x && LocalWorld->v1.y != pPrevLocalWorld->v1.y &&
                    LocalWorld->v1.z != pPrevLocalWorld->v1.z && LocalWorld->v1.w != pPrevLocalWorld->v1.w &&
                    LocalWorld->v2.x != pPrevLocalWorld->v2.x && LocalWorld->v2.y != pPrevLocalWorld->v2.y &&
                    LocalWorld->v2.z != pPrevLocalWorld->v2.z && LocalWorld->v2.w != pPrevLocalWorld->v2.w &&
                    LocalWorld->v3.x != pPrevLocalWorld->v3.x && LocalWorld->v3.y != pPrevLocalWorld->v3.y &&
                    LocalWorld->v3.z != pPrevLocalWorld->v3.z && LocalWorld->v3.w != pPrevLocalWorld->v3.w) {
                }
            }
        }

        CalcSet1 = (Solid != pPrevSolid) || (View != pPrevView) ||            // 4018
                   (LocalWorld != pPrevLocalWorld);

        if (CalcSet1) {                                                       // 4019

            if (View != pPrevView || LocalWorld != pPrevLocalWorld) {         // 4021
                bMatrix4 *world_view = View->GetPlatInfo()->GetWorldViewMatrix(); // 4023  (Ecstasy.hpp:824)
                bMatrix4 mL2V;                                                // 4025  (bMath.hpp:1171)

                eMulMatrix(&mL2V, LocalWorld, world_view);                    // 4027

                // r3 NO se recarga antes de PSMTXInvXpose (0x80116D04): nMtx es
                // el VALOR DE RETORNO de eLoadPosMtxImm, que devuelve Mtx*.
                Mtx *nMtx = eLoadPosMtxImm(mL2V, GX_PNMTX0);                  // 4029

                if (LightContext != 0 && (Solid->GetPlatInfo()->StripFlags & 0x80)) { // 4032
                    if (BlendingMatrices != 0) {                              // 4034
                        {
                            eMulVector(&eLpos[0], world_view, &MyLightPos[0]); // 4041
                            eMulVector(&eLpos[1], world_view, &MyLightPos[1]); // 4042
                            eMulVector(&eLpos[2], world_view, &MyLightPos[2]); // 4043
                        }
                    } else {
                        Mtx mv;                                               // 4052

                        PSMTXInvXpose(*nMtx, mv);                             // 4054

                        GXLoadTexMtxImm(mv, GX_TEXMTX1, GX_MTX3x4);           // 4057
                        GXLoadTexMtxImm(g_ScreenPositionMatrix,               // 4058
                                        GX_PTTEXMTX1, GX_MTX3x4);
                    }
                }
            }

            if (BlendingMatrices == 0 && Solid != pPrevSolid) {               // 4063
                position_table = (float *)(Solid->GetPlatInfo()->StripDataStart +                       // 4067
                                           Solid->GetPlatInfo()->DataOffset0);
                position_table_16 = (unsigned short *)position_table;                                   // 4068
                normal_table = (int *)(Solid->GetPlatInfo()->StripDataStart +                           // 4069
                                       Solid->GetPlatInfo()->DataOffset1);
                colour_table0 = (unsigned int *)(Solid->GetPlatInfo()->StripDataStart +                 // 4070
                                                 Solid->GetPlatInfo()->DataOffset2);
                colour_table1 = colour_table0;                                                          // 4071
                uv_table = (unsigned short *)(Solid->GetPlatInfo()->StripDataStart +                    // 4072
                                              Solid->GetPlatInfo()->DataOffset3);
            }

            if (Solid != pPrevSolid) {                                        // 4076
                pPrevSolid = Solid;                                           // 4078
            }
            if (View != pPrevView) {                                          // 4080
                pPrevView = View;                                             // 4082
            }
            if (LocalWorld != pPrevLocalWorld) {                              // 4084
                pPrevLocalWorld = LocalWorld;                                 // 4086
            }
        }

        strip_entry = (eStripEntry *)Data;                                    // 4094

        ignore_solid_textures = 0;                                            // 4096

        draw_group = 1;                                                       // 4100

        strip_flags = strip_entry->Flags;                                     // 4103

        if (Flags & 0x20) {                                                   // 4105
            ignore_solid_textures = 1;                                        // 4107
        }

        if (strip_flags & 0x400) draw_group = 0;                              // 4110
        draw_group &= (ignore_solid_textures == 0);                            // 4111

        if (draw_group) {                                                     // 4113

            unsigned short disable_mask = 0;                                  // 4118
            int crtVtxFmt;                                                    // 4116

            if (!EnableEnvMap || (Flags & 0x1)) disable_mask |= 0x100;        // 4119
            if (Flags & 0x2) disable_mask |= 0x80;                            // 4120

            if (LightContext == 0) {                                          // 4122
                disable_mask |= 0x1C0;
            }

            if (Flags & 0x1000) {                                             // 4128
                disable_mask |= 0x40;                                         // 4130
            }

            disable_mask = ~disable_mask;                                     // 4134

            eSetTexture(texture, 0);                                          // 4137
            eSetBlendMode(texture, 0);                                        // 4138

            eSetTextureAnimation(texture, 0);                                 // 4140

            if (SphericalPS && LightContext != 0 && (strip_flags & 0x4180)) { // 4144
                colour_table0 = DRD.colourtable0;                             // 4147
                colour_table1 = DRD.colourtable1;                             // 4148
            }

            if (strip_entry->VertexFormat == 1) {                             // 4152
                GXSetArray(GX_VA_POS, position_table, 12);                    // 4154
            } else {
                GXSetArray(GX_VA_POS, position_table_16, 6);                  // 4163
            }

            GXSetArray(GX_VA_NRM, normal_table, 4);                           // 4165
            GXSetArray(GX_VA_CLR0, colour_table0, 4);                         // 4166
            GXSetArray(GX_VA_CLR1, colour_table1, 4);                         // 4167
            GXSetArray(GX_VA_TEX0, uv_table, 4);                              // 4168

            eDataSetLighting(strip_flags, disable_mask, *DRD.trm,             // 4177
                             BlendingMatrices != 0, texture);

            crtVtxFmt = vsModel(strip_entry->VertexFormat,                    // 4179
                                strip_entry->VertexDescription & 0x3F);

            if (BlendingMatrices != 0) {                                      // 4194
                for (int iEntries = 0; iEntries < Entries; iEntries++) {      // 4196
                    eDataRenderStripBlend(strip_entry, crtVtxFmt, strip_flags); // 4198
                    strip_entry++;                                            // 4200
                }                                                             // 4201
            } else {
                for (int iEntries = 0; iEntries < Entries; iEntries++) {      // 4205
                    eDataRenderStripRigid(strip_entry, crtVtxFmt, strip_flags); // 4207
                    strip_entry++;                                            // 4234
                }                                                             // 4235
            }
        }
    }

    // Miembros (total size 0x38)
    void *Data;                     // offset 0x08, size 0x4
    eView *View;                    // offset 0x0C, size 0x4
    eSolid *Solid;                  // offset 0x10, size 0x4
    unsigned int Flags;             // offset 0x14, size 0x4
    eLightContext *LightContext;    // offset 0x18, size 0x4
    eLightMaterial *LightMaterial;  // offset 0x1C, size 0x4
    bMatrix4 *LocalWorld;           // offset 0x20, size 0x4
    bMatrix4 *BlendingMatrices;     // offset 0x24, size 0x4
    unsigned short Entries;         // offset 0x28, size 0x2
    eDataRenderDynamic DRD;         // offset 0x2C, size 0xC
};

// ============================================================================
//  r57-ecs: eTextureBucket y sus tres funciones estaban en eViewPlat.cpp, que se
//  parsea DESPUES. El volcado de lineas del original las da en EcstasyEx.cpp, y
//  el orden del objetivo es eFlushTextureBucketList (334), eSubmitMesh (335) y
//  luego InitSlotPoolsEx (336). Aqui van detras de struct eDataRender --que
//  AddMeshRender necesita completo-- y el bloque InitSlotPoolsEx..epCalculate se
//  ha bajado detras de ellas para que salga el orden del objetivo.
//
//  Flush va FUERA de la clase y marcada `inline`: dentro de la clase GCC la
//  expande en eFlushTextureBucketList (medido: 296 B contra los 204 del
//  objetivo). Fuera y detras de su unico llamante no tiene RTL guardado cuando
//  se compila ese llamante, asi que sale la llamada, y por ser `inline` se
//  emite en la cola de finish_file, que es donde la pone el objetivo (536).
// ============================================================================
// total size: 0x14
struct eTextureBucket : public bTNode<eTextureBucket> {
    void *operator new(size_t size) {
        return bOMalloc(eTextureBucketSlotPool);
    }

    eTextureBucket(TextureInfo *texture_info) {
        this->Texture = texture_info;
        this->DataRenderList.InitList();
    }

    void AddMeshRender(eStripEntry *mesh, unsigned short entries, eView *view, eSolid *solid, uint32 flags, bMatrix4 *local_world,
                       eLightContext *light_context, eLightMaterial *light_mat, bMatrix4 *blending_matrices, eDataRenderDynamic *drd) {
        eDataRender *data = static_cast<eDataRender *>(bOMalloc(eDataRenderSlotPool));

        data->Data = mesh;
        data->Solid = solid;
        data->View = view;
        data->Flags = flags;
        data->LightContext = light_context;
        data->LightMaterial = light_mat;
        data->LocalWorld = local_world;
        data->BlendingMatrices = blending_matrices;
        data->Entries = entries;
        data->DRD.colourtable0 = drd->colourtable0;
        data->DRD.colourtable1 = drd->colourtable1;
        data->DRD.trm = drd->trm;

        this->DataRenderList.AddTail(data);
    }
    void Flush();

    TextureInfo *Texture;               // offset 0x8, size 0x4
    bTList<eDataRender> DataRenderList; // offset 0xC, size 0x8
};

extern bTList<eTextureBucket> g_TextureBucketList[16];

void eFlushTextureBucketList() {
    for (int i = 0; i <= 15; i++) {
        eTextureBucket *bucket;
        while ((bucket = g_TextureBucketList[i].GetHead()) != g_TextureBucketList[i].EndOfList()) {
            bucket->Remove();
            bucket->Flush();
            if (bucket) {
                eDataRender *data;
                bTList<eDataRender> *list = &bucket->DataRenderList;
                while ((data = list->GetHead()) != list->EndOfList()) {
                    data->Remove();
                    if (data) {
                        bFree(eDataRenderSlotPool, data);
                    }
                }
                bFree(eTextureBucketSlotPool, bucket);
            }
        }
    }
    g_NumTextureBuckets = 0;
}

void eSubmitMesh(eStripEntry *mesh, unsigned short entries, eView *view, eSolid *solid, uint32 flags, TextureInfo *texture_info,
                 bMatrix4 *local_world, eLightContext *light_context, eLightMaterial *light_material, bMatrix4 *blending_matrices,
                 eDataRenderDynamic *drd) {
    TextureInfoPlatInfo *plat_info = texture_info->GetPlatInfo();

    if (plat_info->GetActiveBucket() == nullptr) {
        eTextureBucket *bucket = new eTextureBucket(texture_info);

        plat_info->pActiveBucket = bucket;
        if (texture_info->ApplyAlphaSorting) {
            g_TextureBucketList[texture_info->RenderingOrder + 8].AddTail(bucket);
        } else {
            g_TextureBucketList[texture_info->RenderingOrder].AddTail(bucket);
        }
        g_NumTextureBuckets++;
    }

    plat_info->GetActiveBucket()->AddMeshRender(mesh, entries, view, solid, flags, local_world, light_context, light_material,
                                                blending_matrices, drd);
}

inline void eTextureBucket::Flush() {
    {
        eDataRender *data;

        while ((data = this->DataRenderList.GetHead()) != this->DataRenderList.EndOfList()) {
            data->Remove();
            if (data) {
                data->Render(this->Texture);
                bFree(eDataRenderSlotPool, data);
            }
        }
    }
    this->Texture->GetPlatInfo()->pActiveBucket = nullptr;
}

void InitSlotPoolsEx() {
    eTextureBucketSlotPool = bNewSlotPool(0x14, 0x200, "eTextureBucketSlotPool", 0);
    eTextureBucketSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);

    eDataRenderSlotPool = bNewSlotPool(0x38, 0x800, "eMeshRender", 0);
    eDataRenderSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
}

float g_contrast_gain[4] = { 0.2f, 0.2f, 0.2f, 0.25f };
int g_contrast_res = 64;
int b_alpha_shift = 24;
int b_red_shift = 16;
int b_green_shift = 8;
int b_blue_shift = 0;

int zero_red = 0;
int zero_blue = 1;
int zero_green = 0;
int zero_alpha = 1;

int arn_EnableDiffuse = 1;
int arn_EnableAlpha = 1;
int arn_EnableSpec = 1;
int arn_EnableEnv = 1;
float hack_LightColourScale = 0.8f;
float hack_SpecScale = 18.0f;
int arn_HackAlpha = 0;
float arn_AlphaMin = 0.0f;
float arn_AlphaMax = 1.0f;
float hack_SpecScale_InGame = 1.0f;

float pt[2];

inline float fClamp(float v, float vmin, float vmax) {
    float t0 = v - vmin;
    asm("fsel %0, %1, %2, %3" : "=f"(v) : "f"(t0), "f"(v), "f"(vmin));
    float t1 = v - vmax;
    asm("fsel %0, %1, %2, %3" : "=f"(v) : "f"(t1), "f"(vmax), "f"(v));
    return v;
}

inline sn_ps my_fpow3(sn_ps x1, sn_ps pow, sn_ps bias) {
    sn_ps zero = *(sn_ps *)testc0;
    sn_ps one = *(sn_ps *)testc1;

    sn_ps l1 = *(sn_ps *)testl1;
    sn_ps l2 = *(sn_ps *)testl2;
    sn_ps l3 = *(sn_ps *)testl3;
    sn_ps x2 = __builtin_ps_mul(x1, x1);
    sn_ps x3 = __builtin_ps_mul(x2, x1);
    sn_ps l = __builtin_ps_mul(l1, x1);
    l = __builtin_ps_madd(l2, x2, l);
    l = __builtin_ps_madd(l3, x3, l);

    sn_ps e1 = *(sn_ps *)teste1;
    sn_ps e2 = *(sn_ps *)teste2;
    sn_ps e3 = *(sn_ps *)teste3;
    sn_ps p1 = __builtin_ps_mul(l, pow);
    sn_ps p2 = __builtin_ps_mul(p1, p1);
    sn_ps p3 = __builtin_ps_mul(p2, p1);

    sn_ps e = __builtin_ps_madd(e1, p1, one);
    e = __builtin_ps_madd(e2, p2, e);
    e = __builtin_ps_madd(e3, p3, e);

    e = __builtin_ps_mul(e, e);
    e = __builtin_ps_mul(e, e);

    e = __builtin_ps_res(e);

    sn_ps t0 = __builtin_ps_sub(e, zero);
    sn_ps t1 = __builtin_ps_sub(e, one);
    t0 = __builtin_ps_sel(t0, e, zero);
    return __builtin_ps_sel(t1, one, t0);
}

inline float my_fpow(float v, float pow_scale, float pow_bias) {
    float clamp_min = 0.01f;
    float clamp_max = 1.0f;

    pt[0] = fClamp(v, clamp_min, clamp_max);
    float t0 = *(int *)pt;
    float t1 = t0 * pow_scale + pow_bias;
    *(int *)pt = (int)t1;
    return fClamp(pt[0], clamp_min, clamp_max);
}

// gain racional de Schlick: la rama t<0.5 devuelve t/(f+1) y la otra (f-t)/(f-1),
// con f = (1/g - 2)(1 - 2t). Por eso el numerador solo se reescribe en el else.
float gain(float val, float g) {
    float one = 1.0f;
    float den = one / g - 2.0f;
    float tmp = den * (one - (val + val));

    if (val < 0.5f) {

        den = tmp + one;

    } else {

        val = tmp - val;
        den = tmp - one;
    }

    return val / den;
}

// Reordena una textura RGBA lineal al formato en tiles de 4x4 del GX: cada tile
// guarda primero los 16 pares AR y luego los 16 pares GB, de ahi el +0x20.
inline void eSwizzle32Bit(unsigned int *dest, unsigned int *src, long XSize, long YSize) {
    long x;
    long y;
    long xx;
    long yy;
    long i;
    unsigned char *pTempData;
    unsigned char *pDest;
    unsigned int *pSrc;
    unsigned int *pTempSrc;

    pDest = (unsigned char *)dest;
    pSrc = src;
    for (y = 0; y < YSize; y += 4) {

        for (x = 0; x < XSize; x += 4) {

            pTempSrc = pSrc;
            i = 0;
            for (yy = 0; yy < 4; yy++) {

                for (xx = 0; xx < 4; xx++) {

                    unsigned char *work = (unsigned char *)pTempSrc + xx * 4;
                    pDest[i] = work[0];
                    pDest[i + 1] = work[1];
                    pDest[i + 0x20] = work[2];
                    pDest[i + 0x21] = work[3];

                    i += 2;
                }
                pTempSrc += XSize;
            }

            DCFlushRangeNoSync(pDest, 64);

            pSrc += 4;
            pDest += 64;
        }
        pSrc += XSize * 3;
    }

    PPCSync();
}

float g_contrast_current_gain[4];
TextureInfo *pContrastRampTextureInfo;
unsigned int *pContrastRampPixels;
unsigned int *pContrastLinearSource;

// Rellena la rampa de contraste: verde/alfa se indexan por la fila y rojo/azul
// por la columna, cada canal pasado por su propio gain.
void eResetContrastSurface() {
    for (int i = 0; i < g_contrast_res; i++) {

        for (int j = 0; j < g_contrast_res; j++) {

            float alpha = i / (float)g_contrast_res;
            float red = j / (float)g_contrast_res;
            float green = i / (float)g_contrast_res;
            float blue = j / (float)g_contrast_res;

            alpha = gain(alpha, g_contrast_gain[3]);
            red = gain(red, g_contrast_gain[0]);
            green = gain(green, g_contrast_gain[1]);
            blue = gain(blue, g_contrast_gain[2]);
            unsigned char b_alpha = (unsigned char)(alpha * 256.0f);
            unsigned char b_red = (unsigned char)(red * 256.0f);
            unsigned char b_green = (unsigned char)(green * 256.0f);
            unsigned char b_blue = (unsigned char)(blue * 256.0f);

            if (zero_red) { b_red = 0; }
            if (zero_green) { b_green = 0; }
            if (zero_blue) { b_blue = 0; }
            if (zero_alpha) { b_alpha = 0; }

            pContrastLinearSource[i + j * g_contrast_res] = (b_alpha << b_alpha_shift) | (b_red << b_red_shift) | (b_green << b_green_shift) | (b_blue << b_blue_shift);

        }
    }

    eSwizzle32Bit(pContrastRampPixels, pContrastLinearSource, g_contrast_res, g_contrast_res);

    g_contrast_current_gain[0] = g_contrast_gain[0];
    g_contrast_current_gain[1] = g_contrast_gain[1];
    g_contrast_current_gain[2] = g_contrast_gain[2];
    g_contrast_current_gain[3] = g_contrast_gain[3];
}

void eInitContrastSurface() {
    pContrastLinearSource = (unsigned int *)bMalloc(g_contrast_res * g_contrast_res * 4, 0x800);
    bMemSet(pContrastLinearSource, 0, g_contrast_res * g_contrast_res * 4);

    pContrastRampPixels = (unsigned int *)bMalloc(g_contrast_res * g_contrast_res * 4, 0x800);
    bMemSet(pContrastRampPixels, 0, g_contrast_res * g_contrast_res * 4);

    pContrastRampTextureInfo = eCreateTextureInfo();
    pContrastRampTextureInfo->AlphaUsageType = 0;
    pContrastRampTextureInfo->AlphaBlendType = 0;
    pContrastRampTextureInfo->Width = g_contrast_res;
    pContrastRampTextureInfo->Height = g_contrast_res;
    pContrastRampTextureInfo->NumMipMapLevels = 0;
    pContrastRampTextureInfo->ImageData = pContrastRampPixels;
    pContrastRampTextureInfo->PaletteData = nullptr;
    pContrastRampTextureInfo->TilableUV = 0;
    pContrastRampTextureInfo->GetPlatInfo()->Format = 6;
    pContrastRampTextureInfo->GetPlatInfo()->SetImage(pContrastRampTextureInfo);
    eResetContrastSurface();
}

    // INFORME FINAL epCalculate (base congelada: 2072/2072 B, 93,305016 %,
    // 155 filas, frame 0x178; firma DWARF y params/regs verificados en cabecera).
    // El diff son dos familias: ~50 filas de frame +8 (0x170 objetivo) y ~105 de
    // rotacion FPR (f12/f7/f4 objetivo vs f0/f5/f13 nuestra) con foco en 364-453.
    // VIAS MUERTAS (todas medidas, no repetir):
    //  1. Frame por padding/orden de decls: 16 ordenes sn_ps (W2-1) + sensibilidad
    //     -> frame inmovil en 0x178. El layout no lo decide el orden aqui.
    //  2. Muertos jj/flag fuera (ya aplicados): neutro. display_list_packed
    //     fusionado: neutro.
    //  3. Biseccion: sin cuerpo no hay frame; sin big-if el frame ES 0x170
    //     (contribuyente dentro del big-if), pero sin trafico que lo ate a local.
    //  4. Orden DWARF: no existe fuente accesible (mw_dwarfdump no cubre EcstasyEx;
    //     CSVs sin variables; Ghidra 11.4 borrado) Y seria inutil por (1).
    //  5. Cadena-l my_fpow3 invertida: 89,98 %/174/2080 (peor). Swap zero/one:
    //     absorbido. my_fpow3 solo se usa aqui.
    //  6. Replicar f12/f7 con pines: cascada segura. Reordenar 4479-4482 para el
    //     empate de filas 68-71: sched sigue LUID y mueve posiciones (medido:
    //     bias/power swap -> 0x40-load sube a la 68, arbol roto).
    // HERRAMIENTAS VIVAS: Temp\opencode\w2calc.py (banco 2 s, reproduce exacto),
    // Temp\opencode\framecheck.py (stwu por funcion; unico mismatch 1/421 en
    // zEcstasy), dwarf1_gcc_line_info.py sobre nuestro .o (mapa fila->linea).
    // REAPERTURA: solo con el layout original de locales (DWARF vars, hoy
    // inaccesible) o palanca que cambie desempate sin mover posiciones ni tamano
    // (desconocida). NO TOCAR sin reabrir por una de esas dos vias.
void epCalculateLocalDirectionalPOS16(unsigned int *colour_table0 /* r29 */, unsigned int *colour_table1 /* r22 */, int num_colour_entries /* r5 */,
                                      unsigned short *position_table_16 /* r30 */, int *normal_table /* r23 */, unsigned char *index_buffer /* r31 */,
                                      int vertex_description /* r7 */, int num_indicies /* r28 */, eLightMaterial *light_material /* r11 */,
                                      eLightContext *light_context /* r12 */) {
    eLightMaterialPlatInfo *light_material_plat_info = light_material->PlatInfo;

    bool is_white_paint = (light_material->NameHash == 0x93C0EAF6 || light_material->NameHash == 0xD3A9E9FA);
    bool is_in_game = TheGameFlowManager.IsInGame();
    bool is_white_paint_ingame_hack = is_white_paint && is_in_game;

    float specular_power = light_material_plat_info->SpecularPower;
    float specular_bias = light_material_plat_info->SpecularPowerBias;
    float envmap_power = light_material_plat_info->EnvmapPower;
    float envmap_bias = light_material_plat_info->EnvmapPowerBias;

    sn_ps dmin_rg = V2(light_material_plat_info->DiffuseMinR);
    sn_ps dmin_ba = V2(light_material_plat_info->DiffuseMinB);
    sn_ps drng_rg = V2(light_material_plat_info->DiffuseRngR);
    sn_ps drng_ba = V2(light_material_plat_info->DiffuseRngB);
    sn_ps smin_rg = V2(light_material_plat_info->SpecularMinR);
    sn_ps smin_b = V2(light_material_plat_info->SpecularMinB);
    sn_ps srng_rg = V2(light_material_plat_info->SpecularRngR);
    sn_ps srng_b = V2(light_material_plat_info->SpecularRngB);
    sn_ps pow_scale = V2(light_material_plat_info->SpecularPower);
    sn_ps pow_bias = V2(light_material_plat_info->SpecularPowerBias);

    float alpha_rng = arn_AlphaMax - arn_AlphaMin;

    if (arn_HackAlpha) {
        dmin_ba = __builtin_ps_merge00(dmin_ba, (sn_ps)arn_AlphaMin);
        drng_ba = __builtin_ps_merge00(drng_ba, (sn_ps)alpha_rng);
    }

    if (light_context == 0 || light_context->Type != 0) {
        return;
    }

    eDynamicLightContext *dynamicContext = (eDynamicLightContext *)light_context;

    sn_ps exy = V2(dynamicContext->LocalEyePosition.x);
    sn_ps ez = V2(dynamicContext->LocalEyePosition.z);

    sn_ps ld01x = V2(dynamicContext->LocalDirectionMatrix[0].x);
    sn_ps ld01y = V2(dynamicContext->LocalDirectionMatrix[1].x);
    sn_ps ld01z = V2(dynamicContext->LocalDirectionMatrix[2].x);
    sn_ps ld2x = V2(dynamicContext->LocalDirectionMatrix[0].z);
    sn_ps ld2y = V2(dynamicContext->LocalDirectionMatrix[1].z);
    sn_ps ld2z = V2(dynamicContext->LocalDirectionMatrix[2].z);

    sn_ps lc0rg = V2(dynamicContext->LocalColourMatrix[0].x);
    sn_ps lc1rg = V2(dynamicContext->LocalColourMatrix[1].x);
    sn_ps lc2rg = V2(dynamicContext->LocalColourMatrix[2].x);
    sn_ps lc0b = V2(dynamicContext->LocalColourMatrix[0].z);
    sn_ps lc1b = V2(dynamicContext->LocalColourMatrix[1].z);
    sn_ps lc2b = V2(dynamicContext->LocalColourMatrix[2].z);

    float hack_scale = hack_LightColourScale;
    if (is_white_paint_ingame_hack) {
        hack_scale = hack_scale * 0.7f;
    }

    lc0rg = __builtin_ps_muls0(lc0rg, (sn_ps)hack_scale);
    lc1rg = __builtin_ps_muls0(lc1rg, (sn_ps)hack_scale);
    lc2rg = __builtin_ps_muls0(lc2rg, (sn_ps)hack_scale);
    lc0b = __builtin_ps_muls0(lc0b, (sn_ps)hack_scale);
    lc1b = __builtin_ps_muls0(lc1b, (sn_ps)hack_scale);
    lc2b = __builtin_ps_muls0(lc2b, (sn_ps)hack_scale);

    sn_ps zero = __builtin_ps_sub(lc2b, lc2b);

    int lit_vertex_description = vertex_description & 0x3F;
    int display_list_packed;
    unsigned char display_list_header_offset = 0;
    unsigned int data_stride;
    unsigned int data_stride_adjust = 0;

    if ((display_list_packed = (vertex_description & 0x80)) != 0) {
        display_list_header_offset = 3;

        switch (lit_vertex_description) {
        case 0x00:
        case 0x02:
        case 0x08:
        case 0x0A:
        case 0x10:
        case 0x12:
        case 0x18:
        case 0x1A:
            data_stride_adjust = 1;
            break;
        case 0x04:
        case 0x06:
        case 0x0C:
        case 0x0E:
        case 0x14:
        case 0x16:
        case 0x1C:
        case 0x1E:
            data_stride_adjust = 2;
            break;
        }
    }

    unsigned char *ib8 = index_buffer + display_list_header_offset;
    unsigned short *ib16 = (unsigned short *)ib8;
    unsigned char *mixed_index_08;
    unsigned short *mixed_index_16;
    unsigned short *mixed_offset_index_16;

    for (int i = 0; i < num_indicies; i++) {
        unsigned int index_xyz = 0;
        unsigned int index_norm = 0;
        unsigned int index_rgba = 0;
        unsigned int jj;

        switch (lit_vertex_description) {
        case 0x20:
            data_stride = i * 3;
            mixed_index_08 = &((unsigned char *)ib16)[data_stride];
            index_xyz = ((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_08[1];
            index_rgba = mixed_index_08[2];
            break;

        case 0x3C:
            data_stride = i * 6;
            mixed_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_xyz = *(unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_16[1];
            index_rgba = mixed_index_16[2];
            break;

        case 0x00:
            data_stride = i * (4 + data_stride_adjust);
            mixed_index_08 = &((unsigned char *)ib16)[data_stride];
            index_xyz = ((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_08[1];
            index_rgba = mixed_index_08[2];
            break;

        case 0x04:
            data_stride = i * (5 + data_stride_adjust);
            mixed_index_08 = &((unsigned char *)ib16)[data_stride];
            mixed_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_xyz = ((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_08[1];
            index_rgba = mixed_index_16[1];
            break;

        case 0x0C:
            data_stride = i * (6 + data_stride_adjust);
            mixed_offset_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride + 1];
            index_xyz = ((unsigned char *)ib16)[data_stride];
            index_norm = mixed_offset_index_16[0];
            index_rgba = mixed_offset_index_16[1];
            break;

        case 0x14:
            data_stride = i * (6 + data_stride_adjust);
            mixed_index_08 = &((unsigned char *)ib16)[data_stride];
            mixed_offset_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride + 1];
            index_xyz = *(unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_08[2];
            index_rgba = mixed_offset_index_16[1];
            break;

        case 0x1C:
            data_stride = i * (7 + data_stride_adjust);
            mixed_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_xyz = *(unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_16[1];
            index_rgba = mixed_index_16[2];
            break;

        case 0x0E:
            data_stride = i * (7 + data_stride_adjust);
            mixed_offset_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride + 1];
            index_xyz = ((unsigned char *)ib16)[data_stride];
            index_norm = mixed_offset_index_16[0];
            index_rgba = mixed_offset_index_16[1];
            break;

        case 0x1E:
            data_stride = i * (8 + data_stride_adjust);
            mixed_index_16 = (unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_xyz = *(unsigned short *)&((unsigned char *)ib16)[data_stride];
            index_norm = mixed_index_16[1];
            index_rgba = mixed_index_16[2];
            break;

        case -1:
            break;
        }

        unsigned char *out_colour0 = (unsigned char *)&colour_table0[index_rgba];
        unsigned char *out_colour1 = (unsigned char *)&colour_table1[index_rgba];

        int flag;

        if ((*(unsigned char *)&colour_table0[index_rgba] & 1) == 0) {

            signed char *normal_table8 = (signed char *)&normal_table[index_norm];

            sn_ps pxy = __builtin_psq_l(&position_table_16[index_xyz * 3], 0, 5);
            sn_ps pz = __builtin_psq_l(&position_table_16[index_xyz * 3 + 2], 1, 5);

            sn_ps nxy = __builtin_psq_l(&normal_table8[0], 0, 7);
            sn_ps nz = __builtin_psq_l(&normal_table8[2], 1, 7);

            sn_ps vxy = __builtin_ps_sub(exy, pxy);
            sn_ps vz = __builtin_ps_sub(ez, pz);
            sn_ps vl = __builtin_ps_mul(vxy, vxy);
            vl = __builtin_ps_sum0(vl, vl, vl);
            vl = __builtin_ps_madds0(vz, vz, vl);
            vl = __builtin_ps_rsqrte(vl);
            vxy = __builtin_ps_muls0(vxy, vl);
            vz = __builtin_ps_muls0(vz, vl);

            sn_ps tvdotn = __builtin_ps_mul(vxy, nxy);
            tvdotn = __builtin_ps_sum0(tvdotn, tvdotn, tvdotn);
            tvdotn = __builtin_ps_madds0(vz, nz, tvdotn);

            float vdotn = __builtin_ps_merge00(tvdotn, tvdotn);
            vdotn = __builtin_ps_sel((sn_ps)vdotn, (sn_ps)vdotn, zero);

            sn_ps pow_se = my_fpow3(__builtin_ps_merge00((sn_ps)vdotn, (sn_ps)vdotn), pow_scale, pow_bias);

            float specvdotn = __builtin_ps_merge00(pow_se, pow_se);
            float envvdotn = __builtin_ps_merge11(pow_se, pow_se);
            envvdotn = envvdotn + envvdotn;

            sn_ps rxy = __builtin_ps_muls0(nxy, tvdotn);
            sn_ps rz = __builtin_ps_muls0(nz, tvdotn);
            rxy = __builtin_ps_add(rxy, rxy);
            rz = __builtin_ps_add(rz, rz);
            rxy = __builtin_ps_sub(rxy, vxy);
            rz = __builtin_ps_sub(rz, vz);

            sn_ps dsrg;
            sn_ps dsba = __builtin_ps_madds0(drng_ba, (sn_ps)vdotn, dmin_ba);
            sn_ps ssrg = __builtin_ps_madds0(srng_rg, (sn_ps)specvdotn, smin_rg);
            sn_ps ssb = __builtin_ps_madds0(srng_b, (sn_ps)specvdotn, smin_b);

            sn_ps ndotl01 = __builtin_ps_muls0(ld01x, nxy);
            ndotl01 = __builtin_ps_madds1(ld01y, nxy, ndotl01);
            ndotl01 = __builtin_ps_madds0(ld01z, nz, ndotl01);
            ndotl01 = __builtin_ps_sel(ndotl01, ndotl01, zero);

            sn_ps ndotl2 = __builtin_ps_muls0(ld2x, nxy);
            ndotl2 = __builtin_ps_madds1(ld2y, nxy, ndotl2);
            ndotl2 = __builtin_ps_madds0(ld2z, nz, ndotl2);
            ndotl2 = __builtin_ps_sel(ndotl2, ndotl2, zero);

            sn_ps dottmp;

            sn_ps dcrg = __builtin_ps_muls0(lc0rg, ndotl01);
            dcrg = __builtin_ps_madds1(lc1rg, ndotl01, dcrg);
            dcrg = __builtin_ps_madds0(lc2rg, ndotl2, dcrg);

            sn_ps dcba = __builtin_ps_muls0(lc0b, ndotl01);
            dcba = __builtin_ps_madds1(lc1b, ndotl01, dcba);
            dcba = __builtin_ps_madds0(lc2b, ndotl2, dcba);

            dsrg = __builtin_ps_madds0(drng_rg, (sn_ps)vdotn, dmin_rg);
            dcrg = __builtin_ps_mul(dcrg, dsrg);
            dcba = __builtin_ps_mul(dcba, dsba);
            dcba = __builtin_ps_merge01(dcba, dsba);

            float specular_a;

            sn_ps rdotl0 = __builtin_ps_muls0(rxy, ld01x);
            rdotl0 = __builtin_ps_madds0(rxy, ld01y, rdotl0);
            rdotl0 = __builtin_ps_madds0(rz, ld01z, rdotl0);
            rdotl0 = __builtin_ps_sel(rdotl0, rdotl0, zero);
            dottmp = __builtin_ps_mul(rdotl0, rdotl0);
            rdotl0 = __builtin_ps_mul(rdotl0, dottmp);
            rdotl0 = __builtin_ps_mul(rdotl0, rdotl0);

            specular_a = my_fpow(__builtin_ps_merge11(rdotl0, rdotl0), specular_power, specular_bias) * 0.5f;

            sn_ps scrg = __builtin_ps_muls0(lc0rg, rdotl0);
            sn_ps scba = __builtin_ps_muls0(lc0b, rdotl0);

            ssrg = __builtin_ps_muls0(ssrg, (sn_ps)specular_a);
            ssb = __builtin_ps_muls0(ssb, (sn_ps)specular_a);
            scrg = __builtin_ps_mul(scrg, ssrg);
            scba = __builtin_ps_mul(scba, ssb);

            if (TheGameFlowManager.IsInGame()) {
                hack_scale = hack_SpecScale_InGame;
            } else {
                hack_scale = hack_SpecScale;
            }

            scrg = __builtin_ps_muls0(scrg, (sn_ps)hack_scale);
            scba = __builtin_ps_muls0(scba, (sn_ps)hack_scale);

            scba = __builtin_ps_merge00(scba, (sn_ps)envvdotn);

            out_colour0 -= 2;
            out_colour1 -= 2;

            __builtin_psq_st(&out_colour0[2], dcrg, 0, 6);
            __builtin_psq_st(&out_colour0[4], dcba, 0, 6);

            __builtin_psq_st(&out_colour1[2], scrg, 0, 6);
            __builtin_psq_st(&out_colour1[4], scba, 0, 6);

            if (!arn_EnableDiffuse) {
                out_colour0[2] = 0;
                out_colour0[3] = 0;
                out_colour0[4] = 0;
            }
            if (!arn_EnableAlpha) {
                out_colour0[5] = 0;
            }
            if (!arn_EnableSpec) {
                out_colour1[2] = 0;
                out_colour1[3] = 0;
                out_colour1[4] = 0;
            }
            if (!arn_EnableEnv) {
                out_colour1[5] = 0;
            }

            out_colour0[2] |= 1;
        }
    }
}
