#ifndef INPUT_ISTEERINGWHEEL_H
#define INPUT_ISTEERINGWHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

class ISteeringWheel : public UTL::COM::IUnknown {
  public:
    enum SteeringType {
        kGamePad = 0,
        kWheelSpeedSensitive = 1,
        kWheelSpeedInsensitive = 2,
        kWiiRemote = 3,
    };

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISteeringWheel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISteeringWheel() {}

    virtual void UpdateForces(IPlayer *player);
    virtual void ReadInput(float *inputBuffer);
    virtual bool IsConnected();
    virtual SteeringType GetSteeringType();
};

// total size: 0x24
class SteeringWheelDevice : public UTL::COM::Object, public ISteeringWheel {
  public:
    inline ~SteeringWheelDevice() override {}
    inline SteeringWheelDevice(int deviceIndex) : ISteeringWheel(this), UTL::COM::Object(1) {
        mDeviceIndex = deviceIndex;
        mManualTransmission = false;
        isActivated = true;
    }
    void UpdateForces(struct IPlayer *player) override;
    void ReadInput(float *inputBuffer) override;
    bool IsConnected() override;
    bool IsActivated() {
        return isActivated;
    }
    void Deactivated() {
        isActivated = false;
    }
    SteeringType GetSteeringType() override;
    static void InitWheelSupport();
    static void PollWheels();
    static bool WheelConnected(int port);

    static struct LGWheels *lgwheels; // size: 0x4, address: 0x8041E4C0

  private:
    void StopAllForces();
    float ConvertWheelRotation(int channel);
    float ConvertAcceleratorPosition();
    float ConvertBrakePosition();
    float GetShiftUpValue();
    float GetShiftDownValue();
    float GetNOSValue();
    float GetHandbrakeValue();
    float GetGameBrakerValue();
    float GetLookBackvalue();
    float ScaledForceParam(float forceParam);

    int mDeviceIndex;         // offset 0x18, size 0x4
    static float sPedalScale; // size: 0x4, address: 0xFFFFFFFF
    bool mManualTransmission; // offset 0x1C, size 0x1
    bool isActivated;         // offset 0x20, size 0x1
};

#endif
