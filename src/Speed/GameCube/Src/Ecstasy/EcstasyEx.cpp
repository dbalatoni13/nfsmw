#include "./EcstasyEx.hpp"

#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "dolphin/gx/GXCull.h"
#include "dolphin/gx/GXDispList.h"
#include "dolphin/gx/GXEnum.h"
#include "dolphin/gx/GXFrameBuffer.h"
#include "dolphin/gx/GXGeometry.h"
#include "dolphin/gx/GXManage.h"
#include "dolphin/gx/GXPixel.h"
#include "dolphin/gx/GXStruct.h"
#include "dolphin/gx/GXTev.h"
#include "dolphin/gx/GXTexture.h"
#include "dolphin/gx/GXVert.h"
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
void * DLHorizonFogGrid;
unsigned long DLHorizonFogGridSize;
unsigned char HorizonCurrentPOS;
unsigned char HorizonCurrentCLR;
unsigned char HorizonCurrentUVS;

int AddHorizonFogEntryPOS(float posX, float posY, float posZ) {
    float* data_pos = &HORIZON_FOG_GRID_POS_ARRAY[HorizonCurrentPOS * 3];
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
    float* data_uvs = &HORIZON_FOG_GRID_UVS_ARRAY[HorizonCurrentUVS * 2];
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
    int current_index; // r24
    int verts_per_strip; // r15
    int color_base; // r21
    int color_mul; // r22
    int pos_index; // r29
    int clr_index;
    int uvs_index;
    float grid_pointX; // f28
    float grid_pointY; // f30
    float tex_coordX; // f26
    float tex_coordY; // f29
    int multiple; // r0
    
    *display_list = HORIZON_FOG_GRID_DISPLAY_LIST;

    float vertex_spacingX = 42.666668f; // f20
    float vertex_spacingY = 68.571434f; // f22
    float uv_spacingX = 0.06666667; // f21
    float uv_spacingY = 0.14285715; // f23
    
    current_index = 0;
    verts_per_strip = 0x20;
    color_base = 10;
    color_mul = 0xF5;

    GXBeginDisplayList(*display_list, 0xC00);
    
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);

    GXSetArray(GX_VA_POS,  HORIZON_FOG_GRID_POS_ARRAY, 0xC);
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
    this->DLSphereSz = nullptr;
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
    this->cubeBuffer[2].Init(0, 0, face_back,  face_back,  5, 1);
    this->cubeBuffer[3].Init(0, 0, face_left,  face_left,  5, 1);
    this->cubeBuffer[4].Init(0, 0, face_up,    face_up,    5, 1);
    this->cubeBuffer[5].Init(0, 0, face_down,  face_down , 5, 1);

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
    this->sphereTexture->Width  = this->SPHERE_MAP_SIZE_X;
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
    if (!this->bInitialized) return;
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
        this->genSphereMap(&this->cubeTex[0], (GXTexObj *)this->sphereTexture->GetPlatInfo()->ImageInfos.obj.dummy, (void *)this->DLSphere, this->DLSphereSz);
        return this->sphereTexture;
    }
    
    if (!this->bInitialized) return nullptr;
}

char ENV_MAP_DISPLAY_LIST[20480];

// end
void epCalculateLocalDirectionalPOS16(unsigned int * colour_table0 /* r29 */, unsigned int * colour_table1 /* r22 */, int num_colour_entries /* r5 */, unsigned short * position_table_16 /* r30 */, int * normal_table /* r23 */, unsigned char * index_buffer /* r31 */, int vertex_description /* r7 */, int num_indicies /* r28 */, struct eLightMaterial * light_material /* r11 */, struct eLightContext * light_context /* r12 */);
