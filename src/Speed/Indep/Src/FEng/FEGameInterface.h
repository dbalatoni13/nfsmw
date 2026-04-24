#ifndef FENG_FEGAMEINTERFACE_H
#define FENG_FEGAMEINTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "FECodeListBox.h"

struct FEPackage;
struct FEObject;
// total size: 0x8
struct FEObjectListEntry {
    FEObject *pObject; // offset 0x0
    unsigned long ulKey; // offset 0x4
};
struct FEMouse;
struct FEMouseInfo;
struct FEResourceRequest;
struct FEMatrix4;
enum FEng_WarningLevel {
    FEng_NonWarning = 0,
    FEng_SoftWarning = 1,
    FEng_HardWarning = 2,
};

// total size: 0x4
struct FEGameInterface {

    // vtable order must match PS2 dump (GCC 2.95 uses declaration order)
    virtual unsigned char* GetPackageData(const char*, unsigned char**, bool&) = 0;            // [1]
    virtual bool LoadResources(FEPackage*, long, FEResourceRequest*) = 0;                      // [2]
    virtual bool UnloadResources(FEPackage*, long, FEResourceRequest*) = 0;                    // [3]
    virtual void PackageWasLoaded(FEPackage*) = 0;                                             // [4]
    virtual bool PackageWillUnload(FEPackage*) = 0;                                            // [5]
    virtual bool UnloadUnreferencedLibrary(FEPackage*);                                        // [6]
    virtual void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) = 0; // [7]
    virtual void NotifySoundMessage(unsigned long, FEObject*, unsigned long, unsigned long) = 0;  // [8]
    virtual void BeginPackageRendering(FEPackage*) = 0;                                        // [9]
    virtual void EndPackageRendering(FEPackage*) = 0;                                          // [10]
    virtual void GenerateRenderContext(unsigned short, FEObject*) = 0;                         // [11]
    virtual bool GetContextTransform(unsigned short, FEMatrix4&) = 0;                          // [12]
    virtual void RenderObjectList(FEObjectListEntry* pList, unsigned long Count);              // [13]
    virtual void RenderObject(FEObject*) = 0;                                                  // [14]
    virtual void DrawMousePointer(FEMouse&) {}                                                 // [15]
    virtual void GetViewTransformation(FEMatrix4*) = 0;                                        // [16]
    virtual unsigned long GetJoyPadMask(unsigned char) = 0;                                    // [17]
    virtual void GetMouseInfo(FEMouseInfo&) = 0;                                               // [18]
    virtual bool DoesPointTouchObject(float, float, FEObject*) = 0;                            // [19]
    virtual bool SetCellData(FECodeListBox*, unsigned long, unsigned long);                    // [20]
    virtual void OutputWarning(const char* pString, FEng_WarningLevel) {}                      // [21]
    virtual void DebugMessageQueued(unsigned long, FEObject*, FEPackage*, FEObject*, unsigned long) {} // [22]
    virtual void DebugMessageProcessed(unsigned long, FEObject*, FEObject*, FEPackage*, unsigned long) {} // [23]
    virtual void DebugMessageBeginUpdate() {}                                                  // [24]
    virtual void DebugMessageEndUpdate() {}                                                    // [25]
};

// total size: 0xC
struct cFEngGameInterface : public FEGameInterface {
    bool RenderThisPackage;  // offset 0x4
    int iGameMode;           // offset 0x8

    static cFEngGameInterface* pInstance;

    cFEngGameInterface();
    ~cFEngGameInterface() override;

    bool LoadResources(FEPackage*, long, FEResourceRequest*) override;
    bool UnloadResources(FEPackage*, long, FEResourceRequest*) override;
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
