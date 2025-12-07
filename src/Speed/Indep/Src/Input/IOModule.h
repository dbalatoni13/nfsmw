#ifndef INPUT_IOMODULE_H
#define INPUT_IOMODULE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "InputDevice.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x50
class IOModule {
  public:
    static IOModule &GetIOModule();

    InputDevice *GetDevice(int i) {
        return fDevices[i];
    }

  private:
    bool fUpdateEnabled;         // offset 0x0, size 0x1
    int fNumDevices;             // offset 0x4, size 0x4
    InputDevice *fDevices[16];   // offset 0x8, size 0x40
    int fDisconnected;           // offset 0x48, size 0x4
    Timer LastTimeDeviceChanged; // offset 0x4C, size 0x4
};

#endif
