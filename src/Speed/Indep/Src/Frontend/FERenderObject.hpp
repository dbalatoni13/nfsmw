#ifndef __FERENDEROBJECT_HPP__
#define __FERENDEROBJECT_HPP__

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *mpobFERenderObjectSlotPool;

// total size: 0xA4
class FERenderEPoly : public bTNode<FERenderEPoly> {
  public:
    void *operator new(size_t size);
    void operator delete(void *ptr);

    ePoly EPoly;               // offset 0x8, size 0x94
    TextureInfo *pTexture;     // offset 0x9C, size 0x4
    TextureInfo *pTextureMask; // offset 0xA0, size 0x4
};

// total size: 0x64
// Decl: 111
class FERenderObject : public bTNode<FERenderObject> {
  public:
    FEObject *mpobOwner;               // offset 0x8, size 0x4, Decl: 114
    uint32 mulFlags;                   // offset 0xC, size 0x4, Decl: 115
    uint32 mulNumTimesRendered;        // offset 0x10, size 0x4, Decl: 116
    TextureInfo *mpobTexture;          // offset 0x14, size 0x4, Decl: 117
    bTList<FERenderEPoly> mobPolyList; // offset 0x18, size 0x8, Decl: 118
    uint32 mPolyCount;                 // offset 0x20, size 0x4, Decl: 119
    bMatrix4 mstTransform;             // offset 0x24, size 0x40, Decl: 120

    FERenderObject(FEObject *pOwner, TextureInfo *pTexture); // Decl: 124
    ~FERenderObject();                                       // Decl: 125

    static void *operator new(size_t size) { // Decl: 127
        return bOMalloc(mpobFERenderObjectSlotPool);
    }

    static void operator delete(void *ptr) { // Decl: 135
        bFree(mpobFERenderObjectSlotPool, ptr);
    }

    void SetTransform(bMatrix4 *pMatrix); // Decl: 145

    FERenderEPoly *AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *colors,
                           FEPackageRenderInfo *pkg_render_info);

    void AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *colors, TextureInfo *texture,
                 FEPackageRenderInfo *pkg_render_info);

    void AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *in_colors, FEClipInfo *pClipInfo,
                 FEPackageRenderInfo *pkg_render_info);

    void AddPolyWithMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2, float t2, float s3,
                         float t3, uint32 *colors, TextureInfo *texture,
                         TextureInfo *textureMask); // Decl: 152

    void AddPolyWithRotatedMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2, float t2, float s3,
                                float t3, float s4, float t4, float s5, float t5, uint32 *colors, TextureInfo *texture,
                                TextureInfo *textureMask); // Decl: 159

    uint32 CountPolys() {} // Decl: 161

    FEObject *const GetObject() const { // Decl: 163
        return mpobOwner;
    }

    void SetTexture(TextureInfo *texture) { // Decl: 165
        mpobTexture = texture;
    }
    void Render(); // Decl: 166

    void ReadyToRender() { // Decl: 168
        mulNumTimesRendered++;
        mulFlags |= 2;
    }
    bool IsReadyToRender() { // Decl: 169
        return (mulFlags & 2) != 0;
    }

    void DeleteMeAfterRendering() {}         // Decl: 171
    bool ShouldIBeDeletedAfterRendering() {} // Decl: 172

    void Clear(FEPackageRenderInfo *pkg_render_info); // Decl: 174

    static void Initialize(); // Decl: 176
    static void Terminate();  // Decl: 177

  private:
    uint32 ClipGeneral(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);

    uint32 ClipAligned(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);
};

#endif
