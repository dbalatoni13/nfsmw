#ifndef PHYSICS_BEHAVIORS_CHASSIS_H
#define PHYSICS_BEHAVIORS_CHASSIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// Credits: Brawltendo
class Chassis : public VehicleBehavior, public ISuspension {
  public:
    struct State {
        UMath::Matrix4 matrix;            // offset 0x0, size 0x40
        UMath::Vector3 local_vel;         // offset 0x40, size 0xC
        float gas_input;                  // offset 0x4C, size 0x4
        UMath::Vector3 linear_vel;        // offset 0x50, size 0xC
        float brake_input;                // offset 0x5C, size 0x4
        UMath::Vector3 angular_vel;       // offset 0x60, size 0xC
        float ground_effect;              // offset 0x6C, size 0x4
        UMath::Vector3 cog;               // offset 0x70, size 0xC
        float ebrake_input;               // offset 0x7C, size 0x4
        UMath::Vector3 dimension;         // offset 0x80, size 0xC
        float steer_input;                // offset 0x8C, size 0x4
        UMath::Vector3 local_angular_vel; // offset 0x90, size 0xC
        float slipangle;                  // offset 0x9C, size 0x4
        UMath::Vector3 inertia;           // offset 0xA0, size 0xC
        float mass;                       // offset 0xAC, size 0x4
        UMath::Vector3 world_cog;         // offset 0xB0, size 0xC
        float speed;                      // offset 0xBC, size 0x4
        float time;                       // offset 0xC0, size 0x4
        int flags;                        // offset 0xC4, size 0x4
        short driver_style;               // offset 0xC8, size 0x2
        short driver_class;               // offset 0xCA, size 0x2
        short gear;                       // offset 0xCC, size 0x2
        short blown_tires;                // offset 0xCE, size 0x2
        float nos_boost;                  // offset 0xD0, size 0x4
        float shift_boost;                // offset 0xD4, size 0x4
        const struct WCollider *collider; // offset 0xD8, size 0x4

        enum Flags { IS_STAGING = 1, IS_DESTROYED };

        const UMath::Vector3 &GetRightVector() const {
            return UMath::ExtractAxis(matrix, 0);
        }
        const UMath::Vector3 &GetUpVector() const {
            return UMath::ExtractAxis(matrix, 1);
        }
        const UMath::Vector3 &GetForwardVector() const {
            return UMath::ExtractAxis(matrix, 2);
        }
        const UMath::Vector3 &GetPosition() const {
            return UMath::ExtractAxis(matrix, 3);
        }
    };

    enum SleepState {
        SS_LATERAL = 2,
        SS_ALL = 1,
        SS_NONE = 0,
    };

    Chassis(const BehaviorParams &bp);

    Mps ComputeMaxSlip(const State &state) const;
    void DoTireHeat(const Chassis::State &state);
    float ComputeLateralGripScale(const Chassis::State &state) const;
    float ComputeTractionScale(const Chassis::State &state) const;
    SleepState DoSleep(const Chassis::State &state);
    void ComputeAckerman(const float steering, const State &state, UMath::Vector4 *left, UMath::Vector4 *right) const;
    void SetCOG(float extra_bias, float extra_ride);
    void ComputeState(float dT, State &state) const;
    void DoAerodynamics(const Chassis::State &state, float drag_pct, float aero_pct, float aero_front_z, float aero_rear_z,
                        const Physics::Tunings *tunings);
    void DoJumpStabilizer(const State &state);

    /* Overrides */
    // virtual ~Chassis();
    virtual Meters GuessCompression(unsigned int id, float downforce) const;
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
    virtual float GetRenderMotion() const;
    virtual Meters GetRideHeight(unsigned int idx) const;
    virtual float CalculateUndersteerFactor() const;
    virtual float CalculateOversteerFactor() const;
    virtual void OnTaskSimulate(float dT);

  private:
    // total size: 0x94
    ICollisionBody *mRBComplex;                         // offset 0x58, size 0x4
    IInput *mInput;                                     // offset 0x5C, size 0x4
    IEngine *mEngine;                                   // offset 0x60, size 0x4
    ITransmission *mTransmission;                       // offset 0x64, size 0x4
    IDragTransmission *mDragTrany;                      // offset 0x68, size 0x4
    IEngineDamage *mEngineDamage;                       // offset 0x6C, size 0x4
    ISpikeable *mSpikeDamage;                           // offset 0x70, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::chassis> mAttributes; // offset 0x74, size 0x14
    float mJumpTime;                                    // offset 0x88, size 0x4
    float mJumpAlititude;                               // offset 0x8C, size 0x4
    float mTireHeat;                                    // offset 0x90, size 0x4
};

#endif
