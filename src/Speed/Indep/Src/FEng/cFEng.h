#ifndef _CFENG
#define _CFENG

#include "FEPackage.h"
#include "FEngine.h"

enum FE_PACKAGE_PRIORITY {
    FE_PACKAGE_PRIORITY_FIFTH_CLOSEST = 100,
    FE_PACKAGE_PRIORITY_FOURTH_CLOSEST = 101,
    FE_PACKAGE_PRIORITY_THIRD_CLOSEST = 102,
    FE_PACKAGE_PRIORITY_SECOND_CLOSEST = 103,
    FE_PACKAGE_PRIORITY_CLOSEST = 104,
    FE_PACKAGE_PRIORITY_ERROR = 105,
};

// total size: 0x8
struct cFEng {
    static cFEng* mInstance;

    FEngine* mFEng; // offset 0x0, size 0x4
    bool bWasPaused; // offset 0x4, size 0x1

    static void Init();
    static inline cFEng* Get() { return mInstance; }

    cFEng();
    bool IsErrorState() { return mFEng->bErrorScreenMode; }

    FEPackage* FindPackage(const char* pPackageName);
    FEPackage* FindPackageWithControl();

    void QueueGameMessagePkg(unsigned int pMessage, FEPackage* topkg);
    void QueueGameMessage(unsigned int pMessage, const char* pPackageName, unsigned int controlMask);
    bool IsPackagePushed(const char* packageName);
    void PushNoControlPackage(const char* pPackageName, FE_PACKAGE_PRIORITY pPriority);
    void PopNoControlPackage(const char* pPackageName);

    void QueuePackageMessage(unsigned int msg, const char* pkg_name, FEObject* obj);

    void QueuePackageSwitch(const char* pkg_name, int arg, unsigned long param, bool b);
    void QueuePackagePush(const char* pPackageName, int pArg, unsigned long ControlMask, bool pSuppressSimPause);
    void QueuePackagePop(int numPackagesToPop);
    void QueueSoundMessage(unsigned int pMessage, const char* pPackageName);

    void DrawForeground();
    void PushErrorPackage(const char* pPackageName, int pArg, unsigned long ControlMask);
    void PopErrorPackage();
    void PopErrorPackage(int);
    void PauseAllSystems();
    void ResumeAllSystems(bool);
    void Service();
    void ServiceFengOnly();
    FEPackage* FindPackageAtBase();
    FEPackage* FindPackageActive(const char* pPackageName);
    FEPackage* FindPackageIdle(const char* pPackageName);
    void PrintLoadedPackages();
    void QueueMessage(unsigned int msg, const char* pkg_name, FEObject* obj, unsigned int controlMask);
    bool IsPackageInControl(const char* packageName);
    void MakeLoadedPackagesDirty();
};

#endif
