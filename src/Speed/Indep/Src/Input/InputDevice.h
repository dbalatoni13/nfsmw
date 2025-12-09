#ifndef INPUT_INPUTDEVICE_H
#define INPUT_INPUTDEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

enum DeviceScalarType {
    kJoyAxis = 0,
    kAnalogButton = 1,
    kDigitalButton = 2,
};

// total size: 0x10
class DeviceScalar {
  private:
    DeviceScalarType fType; // offset 0x0, size 0x4
    struct UCrc32 fName;    // offset 0x4, size 0x4
    float *fPrevValue;      // offset 0x8, size 0x4
    float *fCurrentValue;   // offset 0xC, size 0x4
};

// total size: 0x2C
class InputDevice : public UTL::COM::Object, public UTL::COM::Factory<int, InputDevice, UCrc32> {
  public:
    InputDevice(int deviceIndex);

    // Virtual functions
    ~InputDevice() override;

    virtual bool IsConnected() {}

    virtual bool IsWheel() {}

    virtual void Initialize() = 0;
    virtual void PollDevice() = 0;
    virtual int GetNumDeviceScalar() = 0;
    virtual void StartVibration() = 0;
    virtual void StopVibration() = 0;

    virtual UTL::COM::IUnknown *GetInterfaces() {}

    virtual UTL::COM::IUnknown *GetSecondaryDevice() {}

    virtual bool DeviceHasChanged();
    virtual bool DeviceHasAnyActivity();
    virtual float *SaveCurrentState();
    virtual void RestoreToState(float *currentState);

  protected:
    DeviceScalar *fDeviceScalar; // offset 0x14, size 0x4
    float *fPrevValues;          // offset 0x18, size 0x4
    float *fCurrentValues;       // offset 0x1C, size 0x4
  private:
    int fDeviceIndex;       // offset 0x20, size 0x4
    float fControllerCurve; // offset 0x24, size 0x4
};

#endif
