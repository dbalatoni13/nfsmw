#pragma once

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include <dolphin.h>

struct cCaptureBuffer {
  // total size: 0x20
  TextureInfo *pCaptureTexture;                             // offset 0x0, size 0x4
  int xOrigin;                                              // offset 0x4, size 0x4
  int yOrigin;                                              // offset 0x8, size 0x4
  int width;                                                // offset 0xC, size 0x4
  int height;                                               // offset 0x10, size 0x4
  char *pCapturePixels;                                     // offset 0x14, size 0x4
  unsigned char bInitialized;                               // offset 0x18, size 0x1
  /* const struct __vtbl_ptr_type *_vptr.cCaptureBuffer; */ // offset 0x1C, size 0x4

  cCaptureBuffer();
  cCaptureBuffer(const int __in_chrg); // TODO virtual?
  void Init(int xO, int yO, int w, int h, int format, int buffer_function);
  void Destroy();
  void CaptureEFB(int opt, int downsample, GXTexFmt capture_format);
};

struct cReflectMap {
  // total size: 0x14
  int REFLECT_MAP_SIZE_X;        // offset 0x0, size 0x4
  int REFLECT_MAP_SIZE_Y;        // offset 0x4, size 0x4
  int NumPlayers;                // offset 0x8, size 0x4
  cCaptureBuffer *reflectBuffer; // offset 0xC, size 0x4
  //   const struct __vtbl_ptr_type *_vptr.cReflectMap; // offset 0x10, size 0x4

  cReflectMap();
  cReflectMap(const int __in_chrg);
  static void Init(cReflectMap *);
  void Destroy();
};

struct cSpecularMap {
  // total size: 0x4C
  int SPEC_MAP_SIZE_X;                                  // offset 0x0, size 0x4
  int SPEC_MAP_SIZE_Y;                                  // offset 0x4, size 0x4
  cCaptureBuffer specBuffer[2];                         // offset 0x8, size 0x40
  /*const struct __vtbl_ptr_type *_vptr.cSpecularMap;*/ // offset 0x48, size 0x4
};
struct cQuarterSizeMap {
  // total size: 0x50
  int QUARTER_SIZE_X;                                      // offset 0x0, size 0x4
  int QUARTER_SIZE_Y;                                      // offset 0x4, size 0x4
  int DepthBufferFlag;                                     // offset 0x8, size 0x4
  cCaptureBuffer quarterSizeBuffer;                        // offset 0xC, size 0x20
  cCaptureBuffer quarterSizeDepthBuffer;                   // offset 0x2C, size 0x20
  /*const struct __vtbl_ptr_type *_vptr.cQuarterSizeMap;*/ // offset 0x4C, size 0x4

  cQuarterSizeMap();
  static void Init(cQuarterSizeMap *);
};
struct cFullSizeMap {
  // total size: 0x2C
  int FULL_SIZE_X;                                      // offset 0x0, size 0x4
  int FULL_SIZE_Y;                                      // offset 0x4, size 0x4
  cCaptureBuffer fullSizeBuffer;                        // offset 0x8, size 0x20
  /*const struct __vtbl_ptr_type *_vptr.cFullSizeMap;*/ // offset 0x28, size 0x4
};
