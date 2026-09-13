#ifndef FEOBJECTCALLBACKS_H
#define FEOBJECTCALLBACKS_H

#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"

// total size: 0x8
// Decl: 8
class FEngMovieStarter : public FEObjectCallback {
  private:
    FEPackage *pPackage; // offset 0x4, size 0x4, Decl: 10

  public:
    FEngMovieStarter(FEPackage *pkg) : pPackage(pkg) {} // Decl: 12

    bool Callback(FEObject *obj) override;
};

// total size: 0x4
// Decl: 18
class FEngMovieStopper : public FEObjectCallback {
  public:
    FEngMovieStopper() {}

    bool Callback(FEObject *obj) override;
};

// total size: 0x4
// Decl: 25
class FEngHidePCObjects : public FEObjectCallback {
  public:
    FEngHidePCObjects() {}

    bool Callback(FEObject *obj) override;
};

// total size: 0x8
// Decl: 32
class FEngTransferFlagsToChildren : public FEObjectCallback {
  public:
    int32 FlagToTransfer; // offset 0x4, size 0x4, Decl: 35

    FEngTransferFlagsToChildren(int32 flag) : FlagToTransfer(flag) {} // Decl: 37

    bool Callback(FEObject *obj) override;
};

// total size: 0xC
// Decl: 44
class RenderObjectDisconnect : public FEObjectCallback {
  public:
    FEPackageRenderInfo *PkgRenderInfo; // offset 0x4, size 0x4
    cFEngRender *pFEngRenderer;         // offset 0x8, size 0x4, Decl: 48

    RenderObjectDisconnect() {}

    bool Callback(FEObject *pObj) override;
};

// total size: 0x8
// Decl: 53
class ObjectDirtySetter : public FEObjectCallback {
  public:
    FEPackageRenderInfo *pRenderInfo; // offset 0x4, size 0x4

    ObjectDirtySetter() {}

    bool Callback(FEObject *obj) override;
};

// total size: 0x8
// Decl: 60
class ObjectVisibilitySetter : public FEObjectCallback {
  public:
    ObjectVisibilitySetter(bool visible) : Visible(visible) {} // Decl: 63

    bool Callback(FEObject *obj) override;

  private:
    bool Visible; // offset 0x4, size 0x1, Decl: 66
};

#endif
