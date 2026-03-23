#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACE_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class cFEng {
  public:
    void QueueGameMessage(unsigned int pMessage, const char *pPackageName, unsigned int controlMask);
    bool IsPackagePushed(const char *name);
    bool IsPackageInControl(const char *name);
    void PopNoControlPackage(const char *name);
    void QueuePackagePop(int all);
    void ServiceFengOnly();
    void QueuePackageSwitch(const char *name, int param2, unsigned int param3, bool param4);

    static cFEng *Get() {
        return mInstance;
    }

  private:
    static cFEng *mInstance;

    // FEngine *mFEng;  // offset 0x0, size 0x4
    // bool bWasPaused; // offset 0x4, size 0x1
};

#endif
