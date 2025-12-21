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

static float FogCurrentBrightness = 0.0f;

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

// TODO
void GenerateHorizonFogDisplayList(void **display_list, unsigned long *size, GXVtxFmt vertex_format) {
    unsigned long dl_sz;
    unsigned long grid_color; // r30
    int current_index;        // r24
    int verts_per_strip;      // r15
    int color_base;           // r21
    int color_mul;            // r22
    int pos_index;            // r29
    int clr_index;
    int uvs_index;
    float grid_pointX; // f28
    float grid_pointY; // f30
    float tex_coordX;  // f26
    float tex_coordY;  // f29
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
            grid_pointX = (((i >> 31) + i) >> 1) * vertex_spacingX;
            grid_pointY = (((i >> 31) + i) >> 1) * vertex_spacingY + (i & 1) ? 68.571434f : 0.0f;

            float red = bSin(grid_pointX * 6.0f);
            float blue = bCos(grid_pointY * 6.0f);
            unsigned char b_red = (unsigned char)(red * color_mul + color_base);
            unsigned char b_blue = (unsigned char)(blue * color_mul + color_base);

            grid_color = b_red << 24 | b_blue << 8 | 0xFF;

            pos_index = AddHorizonFogEntryPOS(grid_pointX, grid_pointY, 0.0f);
            clr_index = AddHorizonFogEntryCLR(grid_color);
            uvs_index = AddHorizonFogEntryUVS(tex_coordX, tex_coordY);

            GXPosition1x8(pos_index);
            GXColor1x8(current_index++);
            GXTexCoord1x8(uvs_index);
        }
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
    this->sphereTexturePixels = (char *)bMalloc(this->SPHERE_MAP_SIZE_X * this->SPHERE_MAP_SIZE_Y * 2, 0x800);

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

// TODO / EQUIVALENT
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
    unsigned short nlon = tess;                                            // r25
    unsigned short nlat = tess;                                            // r27
    int i;                                                                 // r30
    int j;                                                                 // r31
    unsigned long dl_sz = ((tess - 2) * (tess + 1) * 2 + tess + 1) * 0x18; // r24

    GXBeginDisplayList(*display_list, *size);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxAttrFmt(fmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(fmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);

    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT7, tess + 2);

    theta = gsPI / tess;
    r2 = sinf(theta);
    z2 = cosf(theta);

    n1z = 1.0f;
    n2z = z2 * 4 - r;

    GXPosition3f32(0.0f, 0.0f, n1z);
    GXNormal3f32(0.0f, 0.0f, n1z);

    theta = 0.0f;
    for (j = 0; j <= tess; j++) {
        n2x = r2 * cosf(theta);
        n2y = r2 * sinf(theta);

        GXPosition3f32(n2x, n2y, z2);
        GXNormal3f32(n2x * 2 * z2, n2y * 2 * z2, n2z);

        theta = (gsPI * (float(j) * -2.0f)) / nlon;
    }
    GXEnd();

    // ...

    for (i = 0; i < tess; i++) {
        theta = gsPI * float(i) / nlat;
        phi = gsPI * float(i - 1) / nlat;
        r1 = r * sinf(phi);
        z1 = r * cosf(phi);
        r2 = r * sinf(theta);
        z2 = r * cosf(theta);

        n1z = (z1 * 2) * z1 - 1.0f;
        n2z = (z2 * 2) * z2 - 1.0f;

        if (bAbs(z1) < 0.01f || bAbs(z2) < 0.01f)
            break;

        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT7, (tess + 1) * 2);
        phi = 0.0f;
        for (j = 0; j <= tess; j++) {
            n2x = r2 * cosf(phi);
            n2y = r2 * sinf(phi);

            GXPosition3f32(n2x, n2y, z2);
            GXNormal3f32(n2x * 2 * z2, n2y * 2 * z2, n2z);

            n1x = r1 * cosf(phi);
            n1y = r1 * sinf(phi);

            GXPosition3f32(n1x, n1y, z1);
            GXNormal3f32(n1x * 2 * z1, n1y * 2 * z1, n1z);

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

static GXColor TweakSphereMapClr = {0xC4, 0xC4, 0xC4, 0xFF};

void cSphereMap::genSphereMap(GXTexObj **cubemap, GXTexObj *spheremap, void *dl, unsigned long dlsz) {
    int i;         // r27
    GXColor color; // r1+0x184
    unsigned short width;
    unsigned short height;
    GXTexFmt fmt;          // r21
    void *data;            // r15
    GXLightObj ClipLight;  // r1+0x10
    float p[4][4];         // r1+0x50
    Mtx v;                 // r1+0x90
    Mtx tm;                // r1+0xC0
    Mtx tc;                // r1+0xF0
    Mtx tmp;               // r1+0x120
    float camLoc_x = 0.0f; // f30
    float camLoc_y = 0.0f;
    float camLoc_z = 6.0f; // f31
    float up_x = 0.0f;
    float up_y = 1.0f; // f29
    float up_z = 0.0f;
    Point3d camLoc = {camLoc_x, camLoc_y, camLoc_z}; // r1+0x150
    Vec up = {up_x, up_y, up_z};                     // r1+0x160
    Point3d objPt;                                   // r1+0x170

    if (!this->bInitialized)
        return;

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

    color = TweakSphereMapClr;
    GXInitLightColor(&ClipLight, color);
    GXSetChanMatColor(GX_COLOR0A0, color);

    GXSetChanAmbColor(GX_COLOR0A0, (GXColor){0, 0, 0, 0xFF});
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
            break;
        case 1: {
            GXColor Kcolor0 = {0x00, 0x00, 0x00, 0xFF};
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
        case 5:
            GXSetNumTevStages(1);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD4, GX_TEXMAP0, GX_COLOR0);
            GXSetTevKColor(GX_KCOLOR0, (GXColor){0, 0, 0, 0xFF});
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_TEXC, GX_CC_TEXC, GX_CC_TEXC);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_KONST, GX_CA_RASA, GX_CA_KONST, GX_CA_ZERO);
            break;
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

        PSMTXRotRad(tmp, this->axis1[i], MTXDegToRad(this->angle2[i]));
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

    if (prevFogColour != fog_colour) {
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

        write_bp_cmd(((int(fog_r * FogCurrentBrightness) << 16) & 0xFF0000) | ((int(fog_g * FogCurrentBrightness) << 8) & 0x00FF00) |
                     ((int(fog_b * FogCurrentBrightness) << 0) & 0x0000FF) | (0xF2 << 24));
        gx->bpSentNot = 0;
    }
}

// end
void epCalculateLocalDirectionalPOS16(unsigned int *colour_table0 /* r29 */, unsigned int *colour_table1 /* r22 */, int num_colour_entries /* r5 */,
                                      unsigned short *position_table_16 /* r30 */, int *normal_table /* r23 */, unsigned char *index_buffer /* r31 */,
                                      int vertex_description /* r7 */, int num_indicies /* r28 */, struct eLightMaterial *light_material /* r11 */,
                                      struct eLightContext *light_context /* r12 */);
