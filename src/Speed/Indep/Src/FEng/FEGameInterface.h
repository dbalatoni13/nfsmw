#ifndef FENG_FEGAMEINTERFACE_H
#define FENG_FEGAMEINTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "FEObject.h"
#include "FEMath.h"

class FEPackage;
struct FEResourceRequest;
struct FEObjectListEntry;
struct FEMouseInfo;

enum FEng_WarningLevel {
    FEng_NonWarning = 0,
    FEng_SoftWarning = 1,
    FEng_HardWarning = 2,
};

// TODO: FEGameInterface ownership is inferred from the existing stub header and FEGameInterface.cpp line info.
struct FEGameInterface {
    virtual ~FEGameInterface() {}

    virtual unsigned char *GetPackageData(const char *pPackageName, unsigned char **pBlockStart, bool &bDeleteBlock) {
        bDeleteBlock = false;
        return nullptr;
    }

    virtual bool LoadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) { return false; }
    virtual bool UnloadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) { return false; }
    virtual void PackageWasLoaded(FEPackage *pPackage) {}
    virtual bool PackageWillUnload(FEPackage *pPackage) { return false; }
    virtual bool UnloadUnreferencedLibrary() { return false; }
    virtual void NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) {}
    virtual void NotifySoundMessage(unsigned long Message, FEObject *pObject, unsigned long ControlMask, unsigned long pPackagePtr) {}
    virtual void BeginPackageRendering(FEPackage *pPackage) {}
    virtual void EndPackageRendering(FEPackage *pPackage) {}
    virtual void GenerateRenderContext(unsigned short uContext, FEObject *pObject) {}
    virtual bool GetContextTransform(unsigned short uContext, FEMatrix4 &Matrix) { return false; }
    virtual void RenderObjectList(FEObjectListEntry *pList, unsigned long Count) {}
    virtual void RenderObject(FEObject *pObject) {}
    virtual void DrawMousePointer() {}
    virtual void GetViewTransformation(FEMatrix4 *pView) {}
    virtual unsigned long GetJoyPadMask(unsigned char feng_pad_index) { return 0; }
    virtual void GetMouseInfo(FEMouseInfo &Info) {}
    virtual bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) { return false; }
    virtual bool SetCellData(unsigned long Param1, unsigned long Param2, unsigned long Param3, unsigned long Param4) { return false; }
    virtual void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) {}
    virtual void DebugMessageQueued(unsigned long Param1, unsigned long Param2, unsigned long Param3, unsigned long Param4) {}
    virtual void DebugMessageProcessed(unsigned long Param1, unsigned long Param2, unsigned long Param3, unsigned long Param4) {}
    virtual void DebugMessageBeginUpdate() {}
    virtual void DebugMessageEndUpdate() {}
};

// TODO: cFEngGameInterface ownership is inferred from FEGameInterface.cpp and the matching FEGameInterface header stub.
class cFEngGameInterface : public FEGameInterface {
  public:
    cFEngGameInterface();
    virtual ~cFEngGameInterface();

    bool LoadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) override;
    bool UnloadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) override;
    void NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) override;
    void NotifySoundMessage(unsigned long Message, FEObject *pObject, unsigned long ControlMask, unsigned long pPackagePtr) override;
    void GenerateRenderContext(unsigned short uContext, FEObject *pObject) override;
    bool GetContextTransform(unsigned short uContext, FEMatrix4 &Matrix) override;
    void RenderObject(FEObject *pObject) override;
    void GetViewTransformation(FEMatrix4 *pView) override;
    void BeginPackageRendering(FEPackage *pPackage) override;
    void EndPackageRendering(FEPackage *pPackage) override;
    void PackageWasLoaded(FEPackage *pPackage) override;
    bool PackageWillUnload(FEPackage *pPackage) override;
    unsigned char *GetPackageData(const char *pPackageName, unsigned char **pBlockStart, bool &bDeleteBlock) override;
    unsigned long GetJoyPadMask(unsigned char feng_pad_index) override;
    void GetMouseInfo(FEMouseInfo &Info) override;
    bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) override;
    void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) override;

    static cFEngGameInterface *pInstance;

  private:
    bool RenderThisPackage;
    int iGameMode;
};

#endif
