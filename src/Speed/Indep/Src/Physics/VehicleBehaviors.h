#ifndef PHYSICS_VEHICLEBEHAVIORS_H
#define PHYSICS_VEHICLEBEHAVIORS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Behavior.h"

class VehicleBehavior : public Behavior {
  public:
    virtual ~VehicleBehavior();

    VehicleBehavior(const BehaviorParams &bp, unsigned int num_interfaces);

    class IVehicle *GetVehicle() const {
        return mVehicle;
    }

  private:
    IVehicle *mVehicle; // offset 0x4C, size 0x4
};

#endif
