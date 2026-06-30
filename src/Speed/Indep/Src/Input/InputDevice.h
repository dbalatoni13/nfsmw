#ifndef INPUT_INPUTDEVICE_H
#define INPUT_INPUTDEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

enum DeviceScalarType {
    kJoyAxis = 0,
    kAnalogButton = 1,
    kDigitalButton = 2,
};

// total size: 0x10
class DeviceScalar {
public:
    USE_FASTALLOC(DeviceScalar)

    DeviceScalar();
    void InitializeDeviceScalar(DeviceScalarType type, const char *name, float *prev_value, float *current_value);


    DeviceScalarType GetScalarType() { return this->fType; }
    UCrc32 GetDeviceScalarName() {
        return this->fName;
    }

    float GetValue() { return *this->fCurrentValue; }
    float GetPrevValue() { return *this->fPrevValue; }
    bool HasChanged() { return this->fPrevValue != this->fCurrentValue; }

    void OverwriteValue(float newval) { *this->fCurrentValue = newval; } // probably
    void OverwritePrevValie(float newval) { *this->fPrevValue = newval; }

    bool isDown();
    bool isUp();
    bool isDownTransition();
    bool isUpTransition();
    bool isDownTransitionThreshold(float thresh);
    bool isUpTransitionThreshold(float thresh);
    bool isUpThreshold(float thresh);
    bool isDownThreshold(float thresh);

    bool isCenteredTransition(float thresh);
    bool isCentered(float thresh);

private:
    DeviceScalarType fType; // offset 0x0, size 0x4
    struct UCrc32 fName;    // offset 0x4, size 0x4
    float *fPrevValue;      // offset 0x8, size 0x4
    float *fCurrentValue;   // offset 0xC, size 0x4
};

// total size: 0x2C
class InputDevice : public UTL::COM::Object, public UTL::COM::Factory<int, InputDevice, UCrc32> {
public:
    USE_FASTALLOC(InputDevice)

    InputDevice(int deviceIndex);

    // Virtual functions
    virtual ~InputDevice();

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

    DeviceScalar *GetDeviceScalar(int i) {
        if (i >= 0 && i < this->GetNumDeviceScalar()) {
            return &this->fDeviceScalar[i];
        }
        return nullptr;
    };

protected:
    DeviceScalar *fDeviceScalar; // offset 0x14, size 0x4
    float *fPrevValues;          // offset 0x18, size 0x4
    float *fCurrentValues;       // offset 0x1C, size 0x4
private:
    int fDeviceIndex;       // offset 0x20, size 0x4
    float fControllerCurve; // offset 0x24, size 0x4
};

#endif
