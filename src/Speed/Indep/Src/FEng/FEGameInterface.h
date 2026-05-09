#ifndef FENG_FEGAMEINTERFACE_H
#define FENG_FEGAMEINTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEMouse.h"

typedef enum { FR_UnnecessaryLoad = -0x80000000, FR_ShapeResource = 0x40000000 } FEResourceRequestFlags;

typedef struct { // 0x8
    FEObject *pObj;
    u32 uSortKey;
} FEObjectListEntry;

typedef enum { FEng_NonWarning = 0, FEng_SoftWarning = 1, FEng_HardWarning = 2 } FEng_WarningLevel;

// total size: 0x4
class FEGameInterface {
  public:
    virtual u8 *GetPackageData(const char *pPackageName, u8 **pBlockStart, bool &bDeleteBlock) = 0; // [1]
    virtual bool LoadResources(FEPackage *pPackage, long Count, FEResourceRequest *pList) = 0;      // [2]
    virtual bool UnloadResources(FEPackage *pPackage, long Count, FEResourceRequest *pList) = 0;    // [3]
    virtual void PackageWasLoaded(FEPackage *pPackage) = 0;                                         // [4]
    virtual bool PackageWillUnload(FEPackage *pPackage) = 0;                                        // [5]
    virtual bool UnloadUnreferencedLibrary(FEPackage *pPackage) {
        return false;
    }; // [6]
    virtual void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) = 0;          // [7]
    virtual void NotifySoundMessage(u32 Message, FEObject *pObject, u32 ControlMask, u32 pPackagePtr) = 0; // [8]
    virtual void BeginPackageRendering(FEPackage *pPackage) = 0;                                           // [9]
    virtual void EndPackageRendering(FEPackage *pPackage) = 0;                                             // [10]
    virtual void GenerateRenderContext(u16 uContext, FEObject *pObject) = 0;                               // [11]
    virtual bool GetContextTransform(u16 uContext, FEMatrix4 &Matrix) = 0;                                 // [12]
    virtual void RenderObjectList(FEObjectListEntry *pList, u32 Count) {
        while (Count) {
            Count--;
            RenderObject(pList[Count].pObj);
        }
    }; // [13]
    virtual void RenderObject(FEObject *pObject) = 0;                                 // [14]
    virtual void DrawMousePointer(FEMouse &) {}                                       // [15]
    virtual void GetViewTransformation(FEMatrix4 *pView) = 0;                         // [16]
    virtual u32 GetJoyPadMask(u8 feng_pad_index) = 0;                                 // [17]
    virtual void GetMouseInfo(FEMouseInfo &Info) = 0;                                 // [18]
    virtual bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) = 0; // [19]
    virtual bool SetCellData(FECodeListBox *, u32, u32) {
        return false;
    }; // [20]
    virtual void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) {}   // [21]
    virtual void DebugMessageQueued(u32, FEObject *, FEPackage *, FEObject *, u32) {}    // [22]
    virtual void DebugMessageProcessed(u32, FEObject *, FEObject *, FEPackage *, u32) {} // [23]
    virtual void DebugMessageBeginUpdate() {}                                            // [24]
    virtual void DebugMessageEndUpdate() {}                                              // [25]
};

// total size: 0xC
class cFEngGameInterface : public FEGameInterface {
  public:
    cFEngGameInterface();
    virtual ~cFEngGameInterface();

    // TODO: how are these Counts longs when all the symbols say i32/int
    bool LoadResources(FEPackage *pPackage, long Count, FEResourceRequest *pList) override;
    bool UnloadResources(FEPackage *pPackage, long Count, FEResourceRequest *pList) override;
    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override;
    void NotifySoundMessage(u32 Message, FEObject *pObject, u32 ControlMask, u32 pPackagePtr) override;
    void GenerateRenderContext(u16 uContext, FEObject *pObject) override;
    bool GetContextTransform(u16 uContext, FEMatrix4 &Matrix) override;
    void RenderObject(FEObject *pObject) override;
    void GetViewTransformation(FEMatrix4 *pView) override;
    void BeginPackageRendering(FEPackage *pPackage) override;
    void EndPackageRendering(FEPackage *pPackage) override;
    void PackageWasLoaded(FEPackage *pPackage) override;
    bool PackageWillUnload(FEPackage *pPackage) override;
    u8 *GetPackageData(const char *pPackageName, u8 **pBlockStart, bool &bDeleteBlock) override;
    u32 GetJoyPadMask(u8 feng_pad_index) override;
    void GetMouseInfo(FEMouseInfo &Info) override;
    bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) override;
    void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) override;
    bool UnloadUnreferencedLibrary(FEPackage *pPackage) override;
    void RenderObjectList(FEObjectListEntry *pList, u32 Count) override;
    bool SetCellData(FECodeListBox *, u32, u32) override;

    static cFEngGameInterface *pInstance;

  private:
    bool RenderThisPackage; // offset 0x4
    int iGameMode;          // offset 0x8
};

void HackClearCache(FEPackage *pkg);

#endif
