#ifndef FRONTEND_FEPACKAGEMANAGER_H
#define FRONTEND_FEPACKAGEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEPackageData.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"

// total size: 0xC
class FEPackageManager {
  public:
    static void Init();

    static void Destroy();

    static struct FEPackageManager *Get();

    bool SetUseIdleList(const char *pPackageName, bool pUseIdleList);

    void BroadcastMessage(u32 msg);

    unsigned long GetActiveScreensChecksum();

    void NotifySoundMessage(u32 Message, FEObject *obj, u32 controller_mask, u32 pkg_ptr);

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);

    const char *GetBasePkgName(const char *pkg_name);

    struct FEPackage *FindPackage(const char *pkg_name);

    void *GetPackageData(const char *pkg_name);

    void CloseAllPackages(int close_permanent);

    void SetPermanent(const char *pkg_name, int flag);

    bool GetVisibility(const char *pkg_name);

    void SetVisibility(const char *pkg_name, bool visible);

    struct MenuScreen *FindScreen(const char *pkg_name);

    FEPackageData *FindFEPackageData(bChunk *chunk);

    FEPackageData *FindFEPackageData(const char *pkg_name);

    FEPackageData *FindFEPackageData(struct FEPackage *pkg);

    bool SetPackageDataArg(const char *pPackageName, const int pArg);

    void PackageWasLoaded(struct FEPackage *pkg);

    void PackageWillBeUnloaded(struct FEPackage *pkg);

    void Loader(bChunk *chunk, bool hotchunk_flag);

    void UnLoader(bChunk *chunk, bool hotchunk_flag);

    void ErrorTick();

    void Tick();

    FEPackageManager() {}

    virtual ~FEPackageManager() {}

    // FEPackageData *Add(FEPackageData *screen) {}

    // FEPackageData *Remove(FEPackageData *screen) {}

  private:
    static FEPackageManager *mInstance; // size: 0x4, address: 0x8041CB64

    bTList<FEPackageData> ScreenList; // offset 0x0, size 0x8
};

unsigned int FEngGetActiveScreensChecksum();

#endif
