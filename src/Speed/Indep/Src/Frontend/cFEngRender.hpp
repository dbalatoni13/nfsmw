#ifndef FRONTEND_CFENGRENDER_H
#define FRONTEND_CFENGRENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"

// TODO
//  struct FEObject;
//  struct FEPackage;
//  struct FEPackageRenderInfo;
//  struct FEGroup;
struct FEModel;
// struct FERenderObject;
struct FEMovie;
// struct FEColoredImage;
// struct FEImage;
// struct FEMultiImage;
// struct FEString;

struct FEClipInfo {
    bVector3 normals[4]; // offset 0x0, size 0x40
    float constants[4];  // offset 0x40, size 0x10
    uint32 flags;        // offset 0x50, size 0x4
};

struct RenderContext {
    bMatrix4 matrix;      // offset 0x0, size 0x40
    FEClipInfo clipInfo;  // offset 0x40, size 0x54
    FEGroup *group;       // offset 0x94, size 0x4
    FEObject *clipObject; // offset 0x98, size 0x4
    uint8 a;              // offset 0x9C, size 0x1
    uint8 r;              // offset 0x9D, size 0x1
    uint8 g;              // offset 0x9E, size 0x1
    uint8 b;              // offset 0x9F, size 0x1
};

// total size: 0x8
struct FEPackageRenderInfo {
    SlotPool *EpolySlotPool; // offset 0x0, size 0x4
    bool AllowOverflows;     // offset 0x4, size 0x1
};

class cFEngRender {
  private:
    FEClipInfo *MakeRenderMatrix(FEObjData *pData, bMatrix4 *trans, FEColor &color, int GroupIndex, float extra_scale);
    void RenderCBVImage(FEColoredImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);
    void RenderImage(FEImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);
    void RenderString(FEString *string, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);
    void RenderModel(FEModel *model, FERenderObject *cached);
    void RenderMovie(FEMovie *movie, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);
    void RenderMultiImage(FEMultiImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);
    void GenerateClipInfo(RenderContext *context);
    FERenderObject *FindCachedRender(FEObject *object);
    FERenderObject *CreateCachedRender(FEObject *object, TextureInfo *texture_info);

  public:
    cFEngRender();
    ~cFEngRender();
    RenderContext *GetRenderContext(uint16 RenderContext);
    void RenderObject(FEObject *object, FEPackageRenderInfo *pkg_render_info);
    void RemoveCachedRender(FEObject *object, FEPackageRenderInfo *sp);
    void GenerateRenderContext(uint16 GroupContext, FEObject *pObject);
    void PrepForPackage(FEPackage *pPackage);
    void PackageFinished(FEPackage *pPackage);
    void AddToRenderList(FEObject *pObject);

    static cFEngRender *mInstance;

  private:
    uint32 Highwater;             // offset 0x0, size 0x4
    RenderContext RContexts[180]; // offset 0x4, size 0x7080
};

uint32 FEngColorToEpolyColor(FEColor c);

#endif
