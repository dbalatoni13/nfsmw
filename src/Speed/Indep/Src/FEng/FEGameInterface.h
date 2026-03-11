#ifndef FENG_FEGAMEINTERFACE_H
#define FENG_FEGAMEINTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct FEPackage;
struct FEObject;
struct FEObjectListEntry;
struct FEMouse;
struct FEMouseInfo;
struct FECodeListBox;
struct FEMatrix4;
struct FEResourceRequest;
enum FEng_WarningLevel {
    FEng_NonWarning = 0,
    FEng_SoftWarning = 1,
    FEng_HardWarning = 2,
};

// total size: 0x4
struct FEGameInterface {
    virtual ~FEGameInterface() {}

    virtual bool UnloadUnreferencedLibrary(FEPackage*) { return false; }
    virtual void RenderObjectList(FEObjectListEntry* pList, unsigned long Count) {}
    virtual void DrawMousePointer(FEMouse&) {}
    virtual bool SetCellData(FECodeListBox*, unsigned long, unsigned long) { return false; }
    virtual void OutputWarning(const char* pString, FEng_WarningLevel) {}
    virtual void DebugMessageQueued(unsigned long, FEObject*, FEPackage*, FEObject*, unsigned long) {}
    virtual void DebugMessageProcessed(unsigned long, FEObject*, FEObject*, FEPackage*, unsigned long) {}
    virtual void DebugMessageBeginUpdate() {}
    virtual void DebugMessageEndUpdate() {}

    virtual bool LoadResources(FEPackage*, int, FEResourceRequest*) = 0;
    virtual bool UnloadResources(FEPackage*, int, FEResourceRequest*) = 0;
    virtual void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) = 0;
    virtual void NotifySoundMessage(unsigned long, FEObject*, unsigned long, unsigned long) = 0;
    virtual void GenerateRenderContext(unsigned short, FEObject*) = 0;
    virtual bool GetContextTransform(unsigned short, FEMatrix4&) = 0;
    virtual void RenderObject(FEObject*) = 0;
    virtual void GetViewTransformation(FEMatrix4*) = 0;
    virtual void BeginPackageRendering(FEPackage*) = 0;
    virtual void EndPackageRendering(FEPackage*) = 0;
    virtual void PackageWasLoaded(FEPackage*) = 0;
    virtual bool PackageWillUnload(FEPackage*) = 0;
    virtual unsigned char* GetPackageData(const char*, unsigned char**, bool&) = 0;
    virtual unsigned long GetJoyPadMask(unsigned char) = 0;
    virtual void GetMouseInfo(FEMouseInfo&) = 0;
    virtual bool DoesPointTouchObject(float, float, FEObject*) = 0;
};

// total size: 0xC
struct cFEngGameInterface : public FEGameInterface {
    bool RenderThisPackage;  // offset 0x4
    int iGameMode;           // offset 0x8

    static cFEngGameInterface* pInstance;

    cFEngGameInterface();
    ~cFEngGameInterface() override;

    bool LoadResources(FEPackage*, int, FEResourceRequest*) override;
    bool UnloadResources(FEPackage*, int, FEResourceRequest*) override;
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    void NotifySoundMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    void GenerateRenderContext(unsigned short, FEObject*) override;
    bool GetContextTransform(unsigned short, FEMatrix4&) override;
    void RenderObject(FEObject*) override;
    void GetViewTransformation(FEMatrix4*) override;
    void BeginPackageRendering(FEPackage*) override;
    void EndPackageRendering(FEPackage*) override;
    void PackageWasLoaded(FEPackage*) override;
    bool PackageWillUnload(FEPackage*) override;
    unsigned char* GetPackageData(const char*, unsigned char**, bool&) override;
    unsigned long GetJoyPadMask(unsigned char) override;
    void GetMouseInfo(FEMouseInfo&) override;
    bool DoesPointTouchObject(float, float, FEObject*) override;
    void OutputWarning(const char*, FEng_WarningLevel) override;
    bool UnloadUnreferencedLibrary(FEPackage*) override;
    void RenderObjectList(FEObjectListEntry* pList, unsigned long Count) override;
    bool SetCellData(FECodeListBox*, unsigned long, unsigned long) override;
};

#endif
