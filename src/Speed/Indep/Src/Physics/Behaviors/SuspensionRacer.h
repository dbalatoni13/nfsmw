#ifndef PHYSICS_BEHAVIORS_SUSPENSIONRACER_H
#define PHYSICS_BEHAVIORS_SUSPENSIONRACER_H

#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/Chassis.h"
#include "Speed/Indep/Src/Physics/Wheel.h"

// Credits: Brawltendo
class SuspensionRacer : public Chassis, public Sim::Collision::IListener, public IAttributeable, Debugable {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    SuspensionRacer(const BehaviorParams &bp, const SuspensionParams &sp);
    void CreateTires();
    void OnTaskSimulate(float dT);
    void DoDrifting(const Chassis::State &state);
    void TuneWheelParams(Chassis::State &state);
    void DoWheelForces(Chassis::State &state);
    float CalculateMaxSteering(Chassis::State &state, ISteeringWheel::SteeringType steer_type);
    float CalculateSteeringSpeed(Chassis::State &state);
    void DoWallSteer(Chassis::State &state);
    void DoDriveForces(Chassis::State &state);
    float DoHumanSteering(Chassis::State &state);
    float DoAISteering(Chassis::State &state);
    void DoSteering(Chassis::State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void DoAerobatics(Chassis::State &state);
    float CalcYawControlLimit(float speed) const;

    // Overrides
    virtual ~SuspensionRacer();
    virtual void OnCollision(const Sim::Collision::Info &cinfo);
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
    virtual void OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey);
    virtual Meters GetRideHeight(unsigned int idx) const;
    Radians GetWheelAngularVelocity(int index) const;
    virtual void MatchSpeed(float speed);
    virtual UMath::Vector3 GetWheelCenterPos(unsigned int i) const;
    virtual void Reset();
    virtual void OnDebugDraw();

    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;         // offset 0x90, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;       // offset 0xA4, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo; // offset 0xB8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo; // offset 0xCC, size 0x14
    IRigidBody *mRB;                                        // offset 0xE0, size 0x4
    ICollisionBody *mCollisionBody;                         // offset 0xE4, size 0x4
    ITransmission *mTransmission;                           // offset 0xE8, size 0x4
    IHumanAI *mHumanAI;                                     // offset 0xEC, size 0x4
    float mGameBreaker;                                     // offset 0xF0, size 0x4
    unsigned int mNumWheelsOnGround;                        // offset 0xF4, size 0x4
    float mLastGroundCollision;                             // offset 0xF8, size 0x4

    struct Drift {
        enum eState { D_OUT, D_ENTER, D_IN, D_EXIT } State; // offset 0x0, size 0x4
        float Value;                                        // offset 0x4, size 0x4

        void Reset() {
            State = SuspensionRacer::Drift::D_OUT;
            Value = 0.0f;
        }
    } mDrift; // offset 0xFC, size 0x8

    struct Burnout {
        void Update(const float dT, const float speedmph, const float max_slip, const int max_slip_wheel, const float yaw);

        int GetState() {
            return mState;
        }

        float GetTraction() {
            return mTraction;
        }

        void Reset() {
            mState = 0;
            mBurnOutTime = 0.0f;
            mTraction = 1.0f;
            mBurnOutAllow = 0.0f;
        }

        void SetState(int s) {
            mState = s;
        }

        void SetBurnOutTime(float t) {
            mBurnOutTime = t;
        }

        void SetTraction(float t) {
            mTraction = t;
        }

        float GetBurnOutTime(float t) {
            return mBurnOutTime;
        }

        void DecBurnOutTime(float t) {
            mBurnOutTime -= t;
        }

        void ClearBurnOutAllow() {
            mBurnOutAllow = 0.0f;
        }

        void IncBurnOutAllow(float t) {
            mBurnOutAllow += t;
        }

      private:
        int mState;          // offset 0x0, size 0x4
        float mBurnOutTime;  // offset 0x4, size 0x4
        float mTraction;     // offset 0x8, size 0x4
        float mBurnOutAllow; // offset 0xC, size 0x4
    } mBurnOut;              // offset 0x104, size 0x10

    struct Steering {
        float Previous;                       // offset 0x0, size 0x4
        float Wheels[2];                      // offset 0x4, size 0x8
        float Maximum;                        // offset 0xC, size 0x4
        float LastMaximum;                    // offset 0x10, size 0x4
        float LastInput;                      // offset 0x14, size 0x4
        AverageWindow InputAverage;           // offset 0x18, size 0x38
        AverageWindow InputSpeedCoeffAverage; // offset 0x50, size 0x38
        float CollisionTimer;                 // offset 0x88, size 0x4
        float WallNoseTurn;                   // offset 0x8C, size 0x4
        float WallSideTurn;                   // offset 0x90, size 0x4
        float YawControl;                     // offset 0x94, size 0x4

        void Reset() {
            Previous = 0.0f;
            Wheels[1] = 0.0f;
            Wheels[0] = 0.0f;
            Maximum = 45.0f;
            LastMaximum = 45.0f;
            LastInput = 0.0f;
            InputAverage.Reset(0.0f);
            InputSpeedCoeffAverage.Reset(0.0f);
            CollisionTimer = 0.0f;
            WallNoseTurn = 0.0f;
            WallSideTurn = 0.0f;
            YawControl = 1.0f;
        }
    } mSteering; // offset 0x114, size 0x98

    class Tire : public Wheel {
      public:
        bool IsOnGround() {
            return mCompression > 0.0f;
        }

        bool IsSlipping() {
            return mTraction >= 1.0f;
        }

        float SetBrake(float brake) {
            mBrake = brake;
        }

        float SetEBrake(float ebrake) {
            mEBrake = ebrake;
        }

        float GetEBrake() {
            return mEBrake;
        }

        float GetRoadSpeed() {
            return mRoadSpeed;
        }

        float GetRadius() {
            return mRadius;
        }

        float GetAngularVelocity() {
            return mAV;
        }

        float GetLateralSpeed() const {
            return mLateralSpeed;
        }

        float GetCurrentSlip() {
            return mSlip;
        }

        float GetToleratedSlip() {
            // TODO
            // return mSlip;
        }

        void SetLateralBoost(float f) {
            mLateralBoost = f;
        }

        void SetBottomOutTime(float time) {
            mBottomOutTime = time;
        }

        void ScaleTractionBoost(float scale) {
            mTractionBoost *= scale;
        }

        void SetDriftFriction(float scale) {
            mDriftFriction = scale;
        }

        void ApplyDriveTorque(float torque) {
            if (!mBrakeLocked) {
                mDriveTorque += torque;
            }
        }

        void ApplyBrakeTorque(float torque) {
            if (!mBrakeLocked) {
                mBrakeTorque += torque;
            }
        }

        float GetTotalTorque() {
            return mDriveTorque + mBrakeTorque;
        }

        float GetDriveTorque() {
            return mDriveTorque;
        }

        float GetLongitudeForce() {
            return mLongitudeForce;
        }

        bool IsBrakeLocked() {
            return mBrakeLocked;
        }

        bool IsSteeringWheel() {
            return mWheelIndex < 2;
        }

        void SetTractionCircle(const UMath::Vector2 &circle) {
            mTractionCircle = circle;
        }

        float GetSlipAngle() const {
            return mSlipAngle;
        }

        // UNSOLVED why does it load 1.0f first?...
        void MatchSpeed(float speed) {
            mLateralSpeed = 0.0f;
            mRoadSpeed = speed;
            mTraction = 1.0f;
            mBrakeLocked = false;
            mSlip = 0.0f;
            mAngularAcc = 0.0f;
            mLastTorque = 0.0f;
            mBrake = 0.0f;
            mEBrake = 0.0f;
            mAV = speed / mRadius;
        }

        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);
        void BeginFrame(float max_slip, float grip_boost, float traction_boost, float drag_reduction);
        void EndFrame(float dT);
        float ComputeLateralForce(float load, float slip_angle);
        float GetPilotFactor(const float speed);
        void CheckForBrakeLock(float ground_force);
        void CheckSign();
        void UpdateFree(float dT);
        float UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT);

      private:
        // total size: 0x140
        const float mRadius;                // offset 0xC4, size 0x4
        float mBrake;                       // offset 0xC8, size 0x4
        float mEBrake;                      // offset 0xCC, size 0x4
        float mAV;                          // offset 0xD0, size 0x4
        float mLoad;                        // offset 0xD4, size 0x4
        float mLateralForce;                // offset 0xD8, size 0x4
        float mLongitudeForce;              // offset 0xDC, size 0x4
        float mDriveTorque;                 // offset 0xE0, size 0x4
        float mBrakeTorque;                 // offset 0xE4, size 0x4
        float mLateralBoost;                // offset 0xE8, size 0x4
        float mTractionBoost;               // offset 0xEC, size 0x4
        float mSlip;                        // offset 0xF0, size 0x4
        float mLastTorque;                  // offset 0xF4, size 0x4
        const int mWheelIndex;              // offset 0xF8, size 0x4
        float mRoadSpeed;                   // offset 0xFC, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0x100, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0x104, size 0x4
        float mAngularAcc;                  // offset 0x108, size 0x4
        const int mAxleIndex;               // offset 0x10C, size 0x4
        float mTraction;                    // offset 0x110, size 0x4
        float mBottomOutTime;               // offset 0x114, size 0x4
        float mSlipAngle;                   // offset 0x118, size 0x4
        UMath::Vector2 mTractionCircle;     // offset 0x11C, size 0x8
        float mMaxSlip;                     // offset 0x124, size 0x4
        float mGripBoost;                   // offset 0x128, size 0x4
        float mDriftFriction;               // offset 0x12C, size 0x4
        float mLateralSpeed;                // offset 0x130, size 0x4
        bool mBrakeLocked;                  // offset 0x134, size 0x4

        enum LastRotationSign { WAS_POSITIVE, WAS_ZERO, WAS_NEGATIVE } mLastSign; // offset 0x138, size 0x4

        float mDragReduction; // offset 0x13C, size 0x4
    } *mTires[4];

    struct Differential {
        void CalcSplit(bool locked);

        float angular_vel[2];
        int has_traction[2];
        float bias;
        float factor;
        float torque_split[2];
    };

  public:
    bool RearWheelDrive() {
        return mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }

    bool FrontWheelDrive() {
        return mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

    bool IsDriveWheel(unsigned int i) {
        return (IsRear(i) && RearWheelDrive()) || (IsFront(i) && FrontWheelDrive());
    }

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
        return *mTires[i];
    }
};

float YawFrictionBoost(float yaw, float ebrake, float speed, float yawcontrol, float grade);

#endif
