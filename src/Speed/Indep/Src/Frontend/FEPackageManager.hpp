#ifndef FEPACKAGEMANAGER_H
#define FEPACKAGEMANAGER_H

#include "FEPackageData.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEObject.h"

// File: speed/indep/src/frontend/FEPackageManager.hpp
// total size: 0xC
// Decl: speed/indep/src/frontend/FEPackageManager.hpp:11
class FEPackageManager {
  public:
    static void Init();
    static void Destroy();
    static FEPackageManager *Get(); // Decl: speed/indep/src/frontend/FEPackageManager.hpp:15

    FEPackage *FindPackage(const char *pkg_name);

    void CloseAllPackages(int close_permanent);

    void Tick();

    void ErrorTick();

    void Loader(bChunk *chunk, bool hotchunk_flag);

    void UnLoader(bChunk *chunk, bool hotchunk_flag);

    void SetPermanent(const char *pkg_name, int flag);

    MenuScreen *FindScreen(const char *pkg_name);

    u32 GetActiveScreensChecksum();

    void SetVisibility(const char *pkg_name, bool visible);

    bool GetVisibility(const char *pkg_name);

    bool SetUseIdleList(const char *pPackageName, bool pUseIdleList);

    const char *GetBasePkgName(const char *pkg_name);

    bool SetPackageDataArg(const char *pPackageName, const int pArg);

  private:
    static FEPackageManager *mInstance; // size: 0x4, address: 0x8041CB64, Decl: speed/indep/src/frontend/FEPackageManager.cpp:4

    FEPackageManager() {}          // Decl: speed/indep/src/frontend/FEPackageManager.hpp:47
    virtual ~FEPackageManager() {} // Decl: speed/indep/src/frontend/FEPackageManager.hpp:48

    bTList<FEPackageData> ScreenList; // offset 0x0, size 0x8, Decl: speed/indep/src/frontend/FEPackageManager.hpp:50

    FEPackageData *FindFEPackageData(bChunk *chunk);

    FEPackageData *FindFEPackageData(const char *pkg_name);

    FEPackageData *FindFEPackageData(FEPackage *pkg);

    FEPackageData *Add(FEPackageData *screen) {} // Decl: speed/indep/src/frontend/FEPackageManager.hpp:57

    FEPackageData *Remove(FEPackageData *screen) {} // Decl: speed/indep/src/frontend/FEPackageManager.hpp:58

    void PackageWasLoaded(FEPackage *pkg);

    void PackageWillBeUnloaded(FEPackage *pkg);

    void BroadcastMessage(u32 msg);

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);

    void NotifySoundMessage(u32 Message, FEObject *obj, u32 controller_mask, u32 pkg_ptr);

    void *GetPackageData(const char *pkg_name);

    friend class cFEngGameInterface;
};

unsigned int FEngGetActiveScreensChecksum();
MenuScreen *FEngFindScreen(const char *package_name);

FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg /* r3 */);

#endif
