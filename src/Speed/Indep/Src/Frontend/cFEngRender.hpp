#ifndef FRONTEND_CFENGRENDER_H
#define FRONTEND_CFENGRENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FEObject;
struct FEPackage;
struct FEPackageRenderInfo;
struct RenderContext;

struct cFEngRender {
    static cFEngRender* mInstance;
    static cFEngRender* Get() { return mInstance; }
    RenderContext* GetRenderContext(unsigned short ctx);
    void GenerateRenderContext(unsigned short ctx, FEObject* obj);
    void PrepForPackage(FEPackage* pkg);
    void PackageFinished(FEPackage* pkg);
    void AddToRenderList(FEObject* obj);
    void RemoveCachedRender(FEObject* obj, FEPackageRenderInfo* info);
};

#endif
