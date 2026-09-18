#ifndef __FEGAMEINTERFACE_HPP
#define __FEGAMEINTERFACE_HPP

#include "Speed/Indep/Src/FEng/FEGameInterface.h"

// File: speed/indep/src/frontend/fenginterfaces/FEGameInterface.hpp
// total size: 0xC
// Decl: speed/indep/src/frontend/fenginterfaces/FEGameInterface.hpp:20
class cFEngGameInterface : public FEGameInterface {
  public:
    cFEngGameInterface();
    virtual ~cFEngGameInterface();

    bool LoadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) override;

    bool UnloadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) override;

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override;

    void NotifySoundMessage(u32 Message, FEObject *pObject, u32 ControlMask, u32 pPackagePtr) override;

    void GenerateRenderContext(u16 uContext, FEObject *pObject) override;

    bool GetContextTransform(u16 uContext, FEMatrix4 &Matrix) override;

    void RenderObject(FEObject *pObject) override;

    void GetViewTransformation(FEMatrix4 *pView) override;

    void BeginPackageRendering(FEPackage *pPackage) override;

    void EndPackageRendering(FEPackage *pPackage) override;

    u8 *GetPackageData(const char *pPackageName, u8 **pBlockStart, bool &bDeleteBlock) override;

    void PackageWasLoaded(FEPackage *pPackage) override;

    bool PackageWillUnload(FEPackage *pPackage) override;

    u32 GetJoyPadMask(u8 feng_pad_index) override;

    void GetMouseInfo(FEMouseInfo &Info) override;

    bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) override;

    void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) override;

    static cFEngGameInterface *pInstance; // size: 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEGameInterface.cpp:90

  private:
    bool RenderThisPackage; // offset 0x4, size 0x1, Decl: speed/indep/src/frontend/fenginterfaces/FEGameInterface.hpp:83
    int iGameMode;          // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEGameInterface.hpp:91
};

#endif
