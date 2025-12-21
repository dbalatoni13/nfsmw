#ifndef GAMECUBE_ECSTASY_ECSTASY_EX_H
#define GAMECUBE_ECSTASY_ECSTASY_EX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "dolphin/gx/GXEnum.h"
#include "dolphin/gx/GXStruct.h"
#include <dolphin.h>

struct cCaptureBuffer {
    cCaptureBuffer();
    virtual ~cCaptureBuffer();
    void Init(int xO, int yO, int w, int h, int format, int buffer_function);
    void Destroy();
    void CaptureEFB(int opt, int downsample, GXTexFmt capture_format);
    
    // total size: 0x20

    TextureInfo *pCaptureTexture;                             // offset 0x0, size 0x4
    int xOrigin;                                              // offset 0x4, size 0x4
    int yOrigin;                                              // offset 0x8, size 0x4
    int width;                                                // offset 0xC, size 0x4
    int height;                                               // offset 0x10, size 0x4
    char *pCapturePixels;                                     // offset 0x14, size 0x4
    unsigned char bInitialized;                               // offset 0x18, size 0x1
};

struct cSphereMap {    
    cSphereMap();
    virtual ~cSphereMap();
    void Init(int face_front, int face_right, int face_back, int face_left, int face_up, int face_down);
    void Destroy();
    TextureInfo *BuildSphereMap();
    void genSphere(void **display_list, unsigned long *size, unsigned short tess, GXVtxFmt fmt);
    void clrSphere(void **display_list, unsigned long *size);
    void genSphereMap(GXTexObj **cubemap, GXTexObj *spheremap, void *dl, unsigned long dlsz);
    
    // total size: 0x140
    GXTexObj *cubeTex[6]; // offset 0x0, size 0x18
    cCaptureBuffer cubeBuffer[6]; // offset 0x18, size 0xC0
    TextureInfo *sphereTexture; // offset 0xD8, size 0x4
    char *sphereTexturePixels; // offset 0xDC, size 0x4
    unsigned char bInitialized; // offset 0xE0, size 0x1
    void *DLSphere; // offset 0xE4, size 0x4
    unsigned long DLSphereSz; // offset 0xE8, size 0x4
    int SPHERE_MAP_SIZE_X; // offset 0xEC, size 0x4
    int SPHERE_MAP_SIZE_Y; // offset 0xF0, size 0x4
    GXTexFmt SPHERE_MAP_FMT; // offset 0xF4, size 0x4
    int SPHERE_MAP_TESS; // offset 0xF8, size 0x4
    int CUBEFACE_SIZE; // offset 0xFC, size 0x4
    unsigned char CubeFaceStart; // offset 0x100, size 0x1
    unsigned char CubeFaceEnd; // offset 0x101, size 0x1
    unsigned char CubeTevMode; // offset 0x102, size 0x1
    float angle1[6]; // offset 0x104, size 0x18
    char axis1[6]; // offset 0x11C, size 0x6
    float angle2[6]; // offset 0x124, size 0x18
};

struct cReflectMap {
    cReflectMap();
    virtual ~cReflectMap();
    void Init(int num_players);
    void Destroy();
    
    // total size: 0x14
    int REFLECT_MAP_SIZE_X;        // offset 0x0, size 0x4
    int REFLECT_MAP_SIZE_Y;        // offset 0x4, size 0x4
    int NumPlayers;                // offset 0x8, size 0x4
    cCaptureBuffer *reflectBuffer; // offset 0xC, size 0x4
};

struct cSpecularMap {    
    cSpecularMap();
    virtual ~cSpecularMap();
    void Init();
    void Destroy();
    
    // total size: 0x4C
    int SPEC_MAP_SIZE_X;                                  // offset 0x0, size 0x4
    int SPEC_MAP_SIZE_Y;                                  // offset 0x4, size 0x4
    cCaptureBuffer specBuffer[2];                         // offset 0x8, size 0x40
};

struct cQuarterSizeMap {
    cQuarterSizeMap();
    virtual ~cQuarterSizeMap();
    void Init(int create_depth_buffer, int texture_format, int buffer_function);
    void Destroy();

    // total size: 0x50
    int QUARTER_SIZE_X;                                      // offset 0x0, size 0x4
    int QUARTER_SIZE_Y;                                      // offset 0x4, size 0x4
    int DepthBufferFlag;                                     // offset 0x8, size 0x4
    cCaptureBuffer quarterSizeBuffer;                        // offset 0xC, size 0x20
    cCaptureBuffer quarterSizeDepthBuffer;                   // offset 0x2C, size 0x20
};
struct cFullSizeMap {
    cFullSizeMap();
    virtual ~cFullSizeMap();
    void Init();
    void Destroy();

    // total size: 0x2C
    int FULL_SIZE_X;                                      // offset 0x0, size 0x4
    int FULL_SIZE_Y;                                      // offset 0x4, size 0x4
    cCaptureBuffer fullSizeBuffer;                        // offset 0x8, size 0x20
};

struct eFogParams {
    // total size: 0x14
    float Start; // offset 0x0, size 0x4
    float End; // offset 0x4, size 0x4
    float Intensity; // offset 0x8, size 0x4
    float FalloffY; // offset 0xC, size 0x4
    GXColor FogColor; // offset 0x10, size 0x4
};

#endif
