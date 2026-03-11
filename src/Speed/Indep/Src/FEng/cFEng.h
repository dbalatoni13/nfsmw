#ifndef _CFENG
#define _CFENG

#include "FEPackage.h"

struct FEngine;

// total size: 0x8
struct cFEng {
    static cFEng* mInstance;

    FEngine* mFEng; // offset 0x0, size 0x4
    bool bWasPaused; // offset 0x4, size 0x1

    static inline cFEng* Get() { return mInstance; }

    bool IsErrorState();

    FEPackage* FindPackage(const char* pPackageName);

    void QueueGameMessagePkg(unsigned int pMessage, FEPackage* topkg);
    void QueueGameMessage(unsigned int pMessage, const char* pPackageName, unsigned int controlMask);

    void QueuePackageMessage(unsigned int msg, const char* pkg_name, FEObject* obj);

    void QueuePackageSwitch(const char* pkg_name, int arg, unsigned long param, bool b);
    void QueuePackagePush(const char* pPackageName, int pArg, unsigned long ControlMask, bool pSuppressSimPause);
    void QueuePackagePop(int numPackagesToPop);
    void QueueSoundMessage(unsigned int pMessage, const char* pPackageName);
};

#endif
