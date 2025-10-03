#ifndef INPUT_STEERINGWHEELDEVICE_H
#define INPUT_STEERINGWHEELDEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ISteeringWheel.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct SteeringWheelDevice : public UTL::COM::Object, public ISteeringWheel {
    // total size: 0x24
    int mDeviceIndex;         // offset 0x18, size 0x4
    bool mManualTransmission; // offset 0x1C, size 0x1
    bool isActivated;         // offset 0x20, size 0x1

  public:
    static void InitWheelSupport();
    static void PollWheels();
    static bool WheelConnected(int port);

    static struct LGWheels *lgwheels;

    SteeringWheelDevice(int deviceIndex) : UTL::COM::Object(0), ISteeringWheel(nullptr) {}

    // ISteeringWheel
    virtual void UpdateForces(IPlayer *player);
    virtual void ReadInput(float *inputBuffer);

    virtual bool IsConnected();
    virtual ISteeringWheel::SteeringType GetSteeringType();
    virtual ~SteeringWheelDevice();

  private:
    void StopAllForces();
    float ConvertWheelRotation(int);
    float ScaledForceParam(float);

    static float sPedalScale;
};

#endif
