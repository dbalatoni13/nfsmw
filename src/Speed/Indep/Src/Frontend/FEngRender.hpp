#ifndef __FENGRENDER_HPP
#define __FENGRENDER_HPP

#include <types.h>
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEModel.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

// total size: 0x54
struct FEClipInfo {
    bVector3 normals[4]; // offset 0x0, size 0x40
    float constants[4];  // offset 0x40, size 0x10
    uint32 flags;        // offset 0x50, size 0x4

    FEClipInfo() {}
};

// total size: 0xA0
// Decl: 40
struct RenderContext {
    bMatrix4 matrix;      // offset 0x0, size 0x40, Decl: 41
    FEClipInfo clipInfo;  // offset 0x40, size 0x54
    FEGroup *group;       // offset 0x94, size 0x4, Decl: 42
    FEObject *clipObject; // offset 0x98, size 0x4
    uint8 a, r, g, b;     // offset 0x9C, size 0x1, Decl: 43
};

// total size: 0x8
struct FEPackageRenderInfo {
    SlotPool *EpolySlotPool; // offset 0x0, size 0x4
    bool AllowOverflows;     // offset 0x4, size 0x1
};

// total size: 0x7084
// Decl: 61
class cFEngRender {
  public:
    cFEngRender(); // Decl: 82

  private:
    FEClipInfo *MakeRenderMatrix(FEObjData *pData, bMatrix4 *trans, FEColor &color, int32 GroupIndex,
                                 float extra_scale); // Decl: 79

    void RenderCBVImage(FEColoredImage *image, FERenderObject *cached,
                        FEPackageRenderInfo *pkg_render_info); // Decl: 64

    void RenderImage(FEImage *image, FERenderObject *cached,
                     FEPackageRenderInfo *pkg_render_info); // Decl: 65

    void RenderString(FEString *string, FERenderObject *cached,
                      FEPackageRenderInfo *pkg_render_info); // Decl: 66

    void RenderModel(FEModel *model, FERenderObject *cached); // Decl: 67

    void RenderMovie(FEMovie *movie, FERenderObject *cached,
                     FEPackageRenderInfo *pkg_render_info); // Decl: 68

    void RenderMultiImage(FEMultiImage *image, FERenderObject *cached,
                          FEPackageRenderInfo *pkg_render_info); // Decl: 69

    void GenerateClipInfo(RenderContext *context);

    FERenderObject *FindCachedRender(FEObject *object); // Decl: 76
    FERenderObject *CreateCachedRender(FEObject *object,
                                       TextureInfo *texture_info); // Decl: 77

  public:
    ~cFEngRender(); // Decl: 83

    RenderContext *GetRenderContext(uint16 RenderContext); // Decl: 85

    void RenderObject(FEObject *object, FEPackageRenderInfo *pkg_render_info); // Decl: 87

    void RemoveCachedRender(FEObject *object, FEPackageRenderInfo *sp); // Decl: 89

    void GenerateRenderContext(uint16 GroupContext, FEObject *pObject); // Decl: 91

    void PrepForPackage(FEPackage *pPackage); // Decl: 93

    void PackageFinished(FEPackage *pPackage); // Decl: 94

    void AddToRenderList(FEObject *pObject); // Decl: 95

    static cFEngRender *mInstance; // size: 0x4, Decl: 97

  private:
    uint32 Highwater;             // offset 0x0, size 0x4, Decl: 106
    RenderContext RContexts[180]; // offset 0x4, size 0x7080, Decl: 107
};

uint32 FEngColorToEpolyColor(FEColor c);

#endif
