#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACE_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACE_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/fengine.h"

typedef enum {
    FE_PACKAGE_PRIORITY_FIFTH_CLOSEST = 100,
    FE_PACKAGE_PRIORITY_FOURTH_CLOSEST = 101,
    FE_PACKAGE_PRIORITY_THIRD_CLOSEST = 102,
    FE_PACKAGE_PRIORITY_SECOND_CLOSEST = 103,
    FE_PACKAGE_PRIORITY_CLOSEST = 104,
    FE_PACKAGE_PRIORITY_ERROR = 105
} FE_PACKAGE_PRIORITY;

// total size: 0x8
class cFEng {
  public:
    cFEng();

    static void Init();
    static void Destroy();
    static cFEng *Get() {
        return mInstance;
    }

    void QueuePackagePush(const char *pPackageName, int pArg, u32 ControlMask, bool pSuppressSimPause);
    void QueuePackagePop(int numPackagesToPop);
    void QueuePackageSwitch(const char *pPackageName, int pArg, u32 ControlMask, bool pSuppressSimPause);
    void PushNoControlPackage(const char *pPackageName, FE_PACKAGE_PRIORITY pPriority);
    void PopNoControlPackage(const char *pPackageName);

    void PushErrorPackage(const char *pPackageName, int pArg, u32 ControlMask);
    void PopErrorPackage();
    void PopErrorPackage(int port);
    FEPackage *FindPackageWithControl();
    FEPackage *FindPackageAtBase();
    FEPackage *FindPackageActive(const char *pPackageName);
    FEPackage *FindPackageIdle(const char *pPackageName);
    FEPackage *FindPackage(const char *pPackageName);
    bool IsPackagePushed(const char *pPackageName);
    bool IsPackageInControl(const char *pPackageName);
    void PrintLoadedPackages();
    void UnloadPackage();
    void QueuePackageMessage(uint32 pMessage, const char *pPackageName, FEObject *obj);
    void QueueGameMessage(uint32 pMessage, const char *pPackageName, uint32 controlMask);
    void QueueSoundMessage(uint32 pMessage, const char *pPackageName);
    void QueueGameMessagePkg(uint32 pMessage, FEPackage *topkg);
    void Service();
    void ServiceFengOnly();
    void DrawForeground();
    bool RecordPackageMarker(const char *pkg_name);
    int32 GetNumPackageMarkers();
    const char *RecallPackageMarker();
    const char *PeekPackageMarker();
    void MakeLoadedPackagesDirty();
    void EnablePackageControl(FEPackage *pkg, bool bProcess);
    void QueuePopChildPackages(const char *pPackageName);

    bool IsErrorState() {
        return mFEng->IsErrorScreenMode();
    }

    FEngine *GetEngine() {
        return mFEng;
    };

  private:
    void QueueMessage(uint32 pMessage, const char *pPackageName, FEObject *to, uint32 controlMask);
    void PauseAllSystems();
    void ResumeAllSystems(bool flushActions);

    static cFEng *mInstance;

    FEngine *mFEng;  // offset 0x0, size 0x4
    bool bWasPaused; // offset 0x4, size 0x1
};

// Range: 0x801325E8 -> 0x80132694
int FEPrintf(struct FEString *text /* r3 */, const char *fmt /* r4 */, ...);

// Range: 0x80132694 -> 0x801327DC
int FEPrintf(const char *pkg_name /* r27 */, int object_hash /* r4 */, const char *fmt /* r28 */, ...);

// Range: 0x801327DC -> 0x80132914
int FEPrintf(const char *pkg_name /* r28 */, struct FEObject *obj /* r31 */, const char *fmt /* r4 */, ...);

#endif
