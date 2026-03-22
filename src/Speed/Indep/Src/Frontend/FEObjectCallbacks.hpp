#ifndef FRONTEND_FEOBJECTCALLBACKS_H
#define FRONTEND_FEOBJECTCALLBACKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObjectCallback.h"

struct FEPackage;
struct FEPackageRenderInfo;
struct cFEngRender;

// total size: 0x8
struct FEngMovieStarter : public FEObjectCallback {
    FEPackage* pPackage; // offset 0x4

    inline FEngMovieStarter(FEPackage* pkg) : pPackage(pkg) {}
    bool Callback(FEObject* obj) override;
    ~FEngMovieStarter() override {}
};

// total size: 0x4
struct FEngMovieStopper : public FEObjectCallback {
    inline FEngMovieStopper() {}
    bool Callback(FEObject* obj) override;
    ~FEngMovieStopper() override {}
};

// total size: 0x4
struct FEngHidePCObjects : public FEObjectCallback {
    inline FEngHidePCObjects() {}
    bool Callback(FEObject* obj) override;
    ~FEngHidePCObjects() override {}
};

// total size: 0x8
struct FEngTransferFlagsToChildren : public FEObjectCallback {
    int FlagToTransfer; // offset 0x4

    inline FEngTransferFlagsToChildren(int flag) : FlagToTransfer(flag) {}
    bool Callback(FEObject* obj) override;
    ~FEngTransferFlagsToChildren() override {}
};

// total size: 0xC
struct RenderObjectDisconnect : public FEObjectCallback {
    FEPackageRenderInfo* PkgRenderInfo; // offset 0x4
    cFEngRender* pFEngRenderer;         // offset 0x8

    inline RenderObjectDisconnect() {}
    inline RenderObjectDisconnect(FEPackageRenderInfo* ri, cFEngRender* r)
        : PkgRenderInfo(ri) //
        , pFEngRenderer(r) {}
    bool Callback(FEObject* obj) override;
    ~RenderObjectDisconnect() override {}
};

// total size: 0x8
struct ObjectDirtySetter : public FEObjectCallback {
    FEPackageRenderInfo* pRenderInfo; // offset 0x4

    inline ObjectDirtySetter() {}
    inline ObjectDirtySetter(FEPackageRenderInfo* ri) : pRenderInfo(ri) {}
    bool Callback(FEObject* obj) override;
    ~ObjectDirtySetter() override {}
};

// total size: 0x8
struct ObjectVisibilitySetter : public FEObjectCallback {
    bool Visible; // offset 0x4

    inline ObjectVisibilitySetter(bool vis) : Visible(vis) {}
    bool Callback(FEObject* obj) override;
    ~ObjectVisibilitySetter() override {}
};

#endif
