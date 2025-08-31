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
        UMath::Matrix4 matrix;
        UMath::Vector3 local_vel;
        float gas_input;
        UMath::Vector3 linear_vel;
        float brake_input;
        UMath::Vector3 angular_vel;
        float ground_effect;
        UMath::Vector3 cog;
        float ebrake_input;
        UMath::Vector3 dimension;
        float steer_input;
        UMath::Vector3 local_angular_vel;
        float slipangle;
        UMath::Vector3 inertia;
        float mass;
        UMath::Vector3 world_cog;
        float speed;
        float time;
        int flags;
        short driver_style;
        short driver_class;
        short gear;
        short blown_tires;
        float nos_boost;
        float shift_boost;
        struct WCollider *collider;

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

    Chassis(BehaviorParams &bp);

    float ComputeMaxSlip(const State &state) const;
    void OnTaskSimulate(float dT);
    float ComputeLateralGripScale(const Chassis::State &state) const;
    float ComputeTractionScale(const Chassis::State &state) const;
    void ComputeAckerman(const float steering, const State &state, UMath::Vector4 *left, UMath::Vector4 *right) const;
    void SetCOG(float extra_bias, float extra_ride);
    void ComputeState(State &state, float dT);
    void DoAerodynamics(const Chassis::State &state, float drag_pct, float aero_pct, float aero_front_z, float aero_rear_z,
                        const Physics::Tunings *tunings);
    void DoJumpStabilizer(State &state);

  private:
    ICollisionBody *mRBComplex;
    IInput *mInput;
    IEngine *mEngine;
    ITransmission *mTransmission;
    IDragTransmission *mDragTrany;
    IEngineDamage *mEngineDamage;
    ISpikeable *mSpikeDamage;
    Attrib::Gen::chassis mAttributes;
    float mJumpTime;
    float mJumpAltitude;
    float mTireHeat;
};

#endif
