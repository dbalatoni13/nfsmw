#ifndef VEHICLEBEHAVIORS_H
#define VEHICLEBEHAVIORS_H

#include "Behavior.h"

// total size: 0x10
struct AIParams : public Sim::Param {
    AIParams() : Sim::Param(UCrc32(0xa6b47fac), this) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "AIParams";
        return value;
    }
};

// total size: 0x38
struct RBComplexParams : public Sim::Param {
    static UCrc32 TypeName() {
        static UCrc32 value = "RBComplexParams";
        return value;
    }

    RBComplexParams(const UMath::Vector3 &initPos, const UMath::Vector3 &initVel, const UMath::Vector3 &initAngVel, const UMath::Matrix4 &initMat,
                    float initMass, const UMath::Vector3 &initMoment, const UMath::Vector3 &dimension,
                    const CollisionGeometry::Bounds *geoms, bool active, unsigned int collision_mask)
        : Sim::Param(UCrc32(0xa6b47fac), this), //
          finitPos(initPos),                    //
          finitVel(initVel),                    //
          finitAngVel(initAngVel),              //
          finitMat(initMat),                    //
          finitMass(initMass),                  //
          finitMoment(initMoment),              //
          fdimension(dimension),                //
          factive(active),                      //
          fgeoms(geoms),                        //
          fCollisionMask(collision_mask) {}

    const UMath::Vector3 &finitPos;          // offset 0x10, size 0x4
    const UMath::Vector3 &finitVel;          // offset 0x14, size 0x4
    const UMath::Vector3 &finitAngVel;       // offset 0x18, size 0x4
    const UMath::Matrix4 &finitMat;          // offset 0x1C, size 0x4
    float finitMass;                         // offset 0x20, size 0x4
    const UMath::Vector3 &finitMoment;       // offset 0x24, size 0x4
    const UMath::Vector3 &fdimension;        // offset 0x28, size 0x4
    bool factive;                            // offset 0x2C, size 0x1
    const CollisionGeometry::Bounds *fgeoms; // offset 0x30, size 0x4
    unsigned int fCollisionMask;             // offset 0x34, size 0x4
};

// total size: 0x28
struct RBSimpleParams : public Sim::Param {
    static UCrc32 TypeName() {
        static UCrc32 value = "RBSimpleParams";
        return value;
    }

    RBSimpleParams(const UMath::Vector3 &initPos, const UMath::Vector3 &initVel, const UMath::Vector3 &initAngVel,
                   const UMath::Matrix4 &initMat, float initRadius, float initMass)
        : Sim::Param(UCrc32(0xa6b47fac), this), //
          finitPos(initPos),                    //
          finitVel(initVel),                    //
          finitAngVel(initAngVel),              //
          finitMat(initMat),                    //
          finitRadius(initRadius),              //
          finitMass(initMass) {}

    const UMath::Vector3 &finitPos;    // offset 0x10, size 0x4
    const UMath::Vector3 &finitVel;    // offset 0x14, size 0x4
    const UMath::Vector3 &finitAngVel; // offset 0x18, size 0x4
    const UMath::Matrix4 &finitMat;    // offset 0x1C, size 0x4
    float finitRadius;                 // offset 0x20, size 0x4
    float finitMass;                   // offset 0x24, size 0x4
};

// total size: 0x10
struct SuspensionParams : public Sim::Param {
    // TODO
    SuspensionParams() : Sim::Param(UCrc32(0xa6b47fac), this) {}

    // DECLARE_SIM_PARAM, but why does that one have SuspensionParams::TypeName?
    static UCrc32 TypeName() {
        static UCrc32 value = "SuspensionParams";
        return value;
    }
};

// total size: 0x10
struct EngineParams : public Sim::Param {
    EngineParams() : Sim::Param(UCrc32(0xa6b47fac), this) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "EngineParams";
        return value;
    }
};

// total size: 0x10
struct DamageParams : public Sim::Param {
    DamageParams() : Sim::Param(UCrc32(0xa6b47fac), this) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "DamageParams";
        return value;
    }
};

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

#endif
