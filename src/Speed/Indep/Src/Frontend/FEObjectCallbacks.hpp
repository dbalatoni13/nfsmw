#ifndef FEOBJECTCALLBACKS_H
#define FEOBJECTCALLBACKS_H

#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"

// File: speed/indep/src/frontend/FEObjectCallbacks.hpp
// total size: 0x8
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:8
class FEngMovieStarter : public FEObjectCallback {
  private:
    FEPackage *pPackage; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:10

  public:
    FEngMovieStarter(struct FEPackage *pkg) {} // Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:12

    bool Callback(struct FEObject *obj) override;
};

// total size: 0x4
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:18
class FEngMovieStopper : public FEObjectCallback {
  public:
    FEngMovieStopper() {}

    bool Callback(struct FEObject *obj) override;
};

// total size: 0x4
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:25
class FEngHidePCObjects : public FEObjectCallback {
  public:
    FEngHidePCObjects() {}

    bool Callback(struct FEObject *obj) override;
};

// total size: 0x8
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:32
class FEngTransferFlagsToChildren : public FEObjectCallback {
  public:
    int32 FlagToTransfer; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:35

    FEngTransferFlagsToChildren(int32 flag) {} // Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:37

    bool Callback(struct FEObject *obj) override;
};

// total size: 0xC
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:44
class RenderObjectDisconnect : public FEObjectCallback {
  public:
    FEPackageRenderInfo *PkgRenderInfo; // offset 0x4, size 0x4
    cFEngRender *pFEngRenderer;         // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:48

    RenderObjectDisconnect() {}

    bool Callback(struct FEObject *pObj) override;
};

// total size: 0x8
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:53
class ObjectDirtySetter : public FEObjectCallback {
  public:
    FEPackageRenderInfo *pRenderInfo; // offset 0x4, size 0x4

    ObjectDirtySetter() {}

    bool Callback(struct FEObject *obj) override;
};

// total size: 0x8
// Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:60
class ObjectVisibilitySetter : public FEObjectCallback {
  public:
    ObjectVisibilitySetter(bool visible) {} // Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:63

    bool Callback(struct FEObject *obj) override;

  private:
    bool Visible; // offset 0x4, size 0x1, Decl: speed/indep/src/frontend/FEObjectCallbacks.hpp:66
};

#endif
