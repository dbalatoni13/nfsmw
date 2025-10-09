#ifndef INPUT_ISTEERINGWHEEL_H
#define INPUT_ISTEERINGWHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

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

    virtual void UpdateForces(struct IPlayer *player);
    virtual void ReadInput(float *inputBuffer);
    virtual bool IsConnected();
    virtual SteeringType GetSteeringType();
};

#endif
