#ifndef FRONTEND_CFENGRENDER_H
#define FRONTEND_CFENGRENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct FEObject;
struct FEPackage;
struct FEPackageRenderInfo;
struct FEGroup;

struct FEClipInfo {
    bVector3 normals[4]; // offset 0x0, size 0x40
    float constants[4]; // offset 0x40, size 0x10
    unsigned int flags; // offset 0x50, size 0x4
};

struct RenderContext {
    bMatrix4 matrix; // offset 0x0, size 0x40
    FEClipInfo clipInfo; // offset 0x40, size 0x54
    FEGroup* group; // offset 0x94, size 0x4
    FEObject* clipObject; // offset 0x98, size 0x4
    unsigned char a; // offset 0x9C, size 0x1
    unsigned char r; // offset 0x9D, size 0x1
    unsigned char g; // offset 0x9E, size 0x1
    unsigned char b; // offset 0x9F, size 0x1
};

struct cFEngRender {
    unsigned int Highwater; // offset 0x0, size 0x4
    RenderContext RContexts[180]; // offset 0x4, size 0x7080

    static cFEngRender* mInstance;
    static cFEngRender* Get() { return mInstance; }

    cFEngRender();
    ~cFEngRender();

    RenderContext* GetRenderContext(unsigned short ctx);
    void GenerateRenderContext(unsigned short ctx, FEObject* obj);
    void PrepForPackage(FEPackage* pkg);
    void PackageFinished(FEPackage* pkg);
    void AddToRenderList(FEObject* obj);
    void RemoveCachedRender(FEObject* obj, FEPackageRenderInfo* info);
};

#endif
