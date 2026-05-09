#ifndef FRONTEND_FEOBJECTCALLBACKS_H
#define FRONTEND_FEOBJECTCALLBACKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"

// total size: 0x8
class FEngMovieStarter : public FEObjectCallback {
  public:
    FEngMovieStarter(FEPackage *pkg) : pPackage(pkg) {}
    ~FEngMovieStarter() override {}
    bool Callback(FEObject *obj) override;

  private:
    FEPackage *pPackage; // offset 0x4
};

// total size: 0x4
class FEngMovieStopper : public FEObjectCallback {
  public:
    FEngMovieStopper() {}
    ~FEngMovieStopper() override {}
    bool Callback(FEObject *obj) override;
};

// total size: 0x4
class FEngHidePCObjects : public FEObjectCallback {
  public:
    FEngHidePCObjects() {}
    ~FEngHidePCObjects() override {}
    bool Callback(FEObject *obj) override;
};

// total size: 0x8
class FEngTransferFlagsToChildren : public FEObjectCallback {
  public:
    FEngTransferFlagsToChildren(int flag) : FlagToTransfer(flag) {}
    ~FEngTransferFlagsToChildren() override {}
    bool Callback(FEObject *obj) override;

    int32 FlagToTransfer; // offset 0x4
};

// total size: 0xC
class RenderObjectDisconnect : public FEObjectCallback {
  public:
    RenderObjectDisconnect() {}
    RenderObjectDisconnect(FEPackageRenderInfo *ri, cFEngRender *r) : PkgRenderInfo(ri), pFEngRenderer(r) {}
    ~RenderObjectDisconnect() override {}
    bool Callback(FEObject *obj) override;

    FEPackageRenderInfo *PkgRenderInfo; // offset 0x4
    cFEngRender *pFEngRenderer;         // offset 0x8
};

// total size: 0x8
class ObjectDirtySetter : public FEObjectCallback {
  public:
    ObjectDirtySetter() {}
    ObjectDirtySetter(FEPackageRenderInfo *ri) : pRenderInfo(ri) {}
    ~ObjectDirtySetter() override {}
    bool Callback(FEObject *obj) override;

    FEPackageRenderInfo *pRenderInfo; // offset 0x4
};

// total size: 0x8
class ObjectVisibilitySetter : public FEObjectCallback {
  public:
    ObjectVisibilitySetter(bool vis) : Visible(vis) {}
    ~ObjectVisibilitySetter() override {}
    bool Callback(FEObject *obj) override;

  private:
    bool Visible; // offset 0x4
};

#endif
