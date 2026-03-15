#ifndef PHYSICS_VEHICLEBEHAVIORS_H
#define PHYSICS_VEHICLEBEHAVIORS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Behavior.h"

// total size: 0x50
class VehicleBehavior : public Behavior {
  public:
    virtual ~VehicleBehavior() {}

    VehicleBehavior(const BehaviorParams &bp, unsigned int num_interfaces);

    class IVehicle *GetVehicle() const {
        return mVehicle;
    }

  private:
    IVehicle *mVehicle; // offset 0x4C, size 0x4
};

// total size: 0x28
struct RBSimpleParams : public Sim::Param {
    RBSimpleParams(const struct UMath::Vector3 &initPos, const struct UMath::Vector3 &initVel,
                   const struct UMath::Vector3 &initAngVel, const struct UMath::Matrix4 &initMat,
                   float initRadius, float initMass);

    RBSimpleParams(const RBSimpleParams &_ctor_arg);

    static UCrc32 TypeName() {
        static UCrc32 value = "RBSimpleParams";
        return value;
    }

    const struct UMath::Vector3 &finitPos;    // offset 0x10, size 0x4
    const struct UMath::Vector3 &finitVel;    // offset 0x14, size 0x4
    const struct UMath::Vector3 &finitAngVel; // offset 0x18, size 0x4
    const struct UMath::Matrix4 &finitMat;    // offset 0x1C, size 0x4
    float finitRadius;                 // offset 0x20, size 0x4
    float finitMass;                   // offset 0x24, size 0x4
};

inline RBSimpleParams::RBSimpleParams(const struct UMath::Vector3 &initPos, const struct UMath::Vector3 &initVel,
                                      const struct UMath::Vector3 &initAngVel, const struct UMath::Matrix4 &initMat,
                                      float initRadius, float initMass)
    : Sim::Param(UCrc32(0xa6b47fac), static_cast<RBSimpleParams *>(nullptr)), finitPos(initPos), finitVel(initVel), finitAngVel(initAngVel),
      finitMat(initMat), finitRadius(initRadius), finitMass(initMass) {}

inline RBSimpleParams::RBSimpleParams(const RBSimpleParams &_ctor_arg)
    : Sim::Param(_ctor_arg), finitPos(_ctor_arg.finitPos), finitVel(_ctor_arg.finitVel), finitAngVel(_ctor_arg.finitAngVel),
      finitMat(_ctor_arg.finitMat), finitRadius(_ctor_arg.finitRadius), finitMass(_ctor_arg.finitMass) {}

#endif
