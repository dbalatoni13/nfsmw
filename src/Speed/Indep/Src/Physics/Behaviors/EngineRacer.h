#ifndef PHYSICS_BEHAVIORS_ENGINERACER_H
#define PHYSICS_BEHAVIORS_ENGINERACER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IInductable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITiptronic.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// Credits: Brawltendo

// total size: 0x1B0
class EngineRacer : protected VehicleBehavior,
                    protected ITransmission,
                    protected IEngine,
                    public IAttributeable,
                    public IInductable,
                    public ITiptronic,
                    public IRaceEngine,
                    public IEngineDamage {
  public:
    struct Clutch {
      public:
        enum State { ENGAGED, ENGAGING, DISENGAGED };

        Clutch() {
            mState = ENGAGED;
            mTime = 0.0f;
            mEngageTime = 0.0f;
        }

        void Disengage() {
            if (mState == ENGAGED) {
                mState = DISENGAGED;
            }
        }

        void Engage(float time) {
            if (mState == DISENGAGED) {
                if (time > 0.0f) {
                    mState = ENGAGING;
                } else {
                    mState = ENGAGED;
                }
                mTime = time;
                mEngageTime = time;
            }
        }

        void Reset() {
            mState = ENGAGED;
            mTime = 0.0f;
        }

        float Update(float dT) {
            if (mTime > 0.0f) {
                mTime -= dT;
                // engage the clutch when the timer hits or drops below zero
                if (mTime <= 0.0f && (mState - 1) == ENGAGED)
                    mState = ENGAGED;
            }

            // return clutch ratio
            switch (mState) {
                case ENGAGED:
                    return 1.0f;
                case ENGAGING:
                    return 1.0f - UMath::Ramp(mTime, 0.0f, mEngageTime) * 0.75f;
                case DISENGAGED:
                    return 0.25f;
                default:
                    return 1.0f;
            }
        }

        State GetState() {
            return mState;
        }

        State mState;
        float mTime;
        float mEngageTime;
    };

    // Methods
    EngineRacer(const BehaviorParams &bp);
    ShiftPotential FindShiftPotential(GearID gear, float rpm) const;
    float GetDifferentialAngularVelocity(bool locked) const;
    float GetDriveWheelSlippage() const;
    void SetDifferentialAngularVelocity(float w);
    float CalcSpeedometer(float rpm, unsigned int gear) const;
    void LimitFreeWheels(float w);
    float GetBrakingTorque(float engine_torque, float rpm) const;
    bool DoGearChange(GearID gear, bool automatic);
    void AutoShift();

    // Overrides
    override virtual ~EngineRacer();

    // Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);
    override virtual void OnBehaviorChange(const UCrc32 &);

    // ITransmission
    override virtual float GetSpeedometer() const;
    override virtual float GetMaxSpeedometer() const;
    override virtual float GetShiftPoint(GearID from_gear, GearID to_gear) const;

    // Inline virtuals
    // IRaceEngine
    // Credits: Brawltendo
    override virtual float GetPerfectLaunchRange(float &range) {
        // perfect launch only applies to first gear
        if (mGear != G_FIRST) {
            range = 0.0f;
            return 0.0f;
        } else {
            range = (mEngineInfo.RED_LINE() - mEngineInfo.IDLE()) * 0.25f;
            float upper_limit = mEngineInfo.RED_LINE() + 500.0f;
            return UMath::Min(mPeakTorqueRPM + range, upper_limit) - range;
        }
    }

    // GetMaxHorsePower
    // GetMinHorsePower
    // GetRPM

    virtual bool IsEngineBraking() {
        return mEngineBraking;
    }

    virtual bool IsShiftingGear() {
        return mGearShiftTimer > 0.0f;
    }

    // ITransmission
    override virtual bool Shift(GearID gear) {
        return DoGearChange(gear, false);
    }

    virtual ShiftStatus OnGearChange(GearID gear);

    virtual bool UseRevLimiter() const {
        return true;
    }

    virtual void DoECU();
    virtual float DoThrottle();
    virtual void DoInduction(const Physics::Tunings *tunings, float dT);
    virtual float DoNos(const Physics::Tunings *tunings, float dT, bool engaged);
    virtual void DoShifting(float dT);
    virtual ShiftPotential UpdateShiftPotential(GearID gear, float rpm);
    virtual float GetEngineTorque(float rpm) const;

    // IEngine
    virtual bool IsNOSEngaged() const {
        return mNOSEngaged >= 1.0f;
    }

    virtual bool HasNOS() const {
        return mNOSInfo.NOS_CAPACITY() > 0.0f && mNOSInfo.TORQUE_BOOST() > 0.0f;
    }

    virtual float GetNOSFlowRate() const {
        return mNOSInfo.FLOW_RATE();
    }

    virtual void ChargeNOS(float charge) {
        if (HasNOS()) {
            mNOSCapacity = UMath::Clamp(mNOSCapacity + charge, 0.0f, 1.0f);
        }
    }

    // Inlines
    unsigned int GetNumGearRatios() const {
        return mTranyInfo.Num_GEAR_RATIO();
    }

    float GetGearRatio(unsigned int idx) const {
        return mTranyInfo.GEAR_RATIO(idx);
    }

    float GetGearEfficiency(unsigned int idx) const {
        return mTranyInfo.GEAR_EFFICIENCY(idx);
    }

    float GetFinalGear() const {
        return mTranyInfo.FINAL_GEAR();
    }

    float GetRatioChange(unsigned int from, unsigned int to) const {
        float ratio1 = mTranyInfo.GEAR_RATIO(from);
        float ratio2 = mTranyInfo.GEAR_RATIO(to);

        if (ratio1 > 0.0f && ratio2 > FLOAT_EPSILON) {
            return ratio1 / ratio2;
        } else {
            return 0.0f;
        }
    }

    float GetShiftDelay(unsigned int gear) const {
        return mTranyInfo.SHIFT_SPEED() * GetGearRatio(gear);
    }

    bool RearWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }

    bool FrontWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

    float GetShiftUpRPM(int gear) const {
        return mShiftUpRPM[gear];
    }

    float GetShiftDownRPM(int gear) const {
        return mShiftDownRPM[gear];
    }

  private:
    float mDriveTorque;                                      // offset 0x84, size 0x4
    int mGear;                                               // offset 0x88, size 0x4
    float mGearShiftTimer;                                   // offset 0x8C, size 0x4
    float mThrottle;                                         // offset 0x90, size 0x4
    float mSpool;                                            // offset 0x94, size 0x4
    float mPSI;                                              // offset 0x98, size 0x4
    float mInductionBoost;                                   // offset 0x9C, size 0x4
    float mShiftUpRPM[10];                                   // offset 0xA0, size 0x28
    float mShiftDownRPM[10];                                 // offset 0xC8, size 0x28
    float mAngularVelocity;                                  // offset 0xF0, size 0x4
    float mAngularAcceleration;                              // offset 0xF4, size 0x4
    float mTransmissionVelocity;                             // offset 0xF8, size 0x4
    float mNOSCapacity;                                      // offset 0xFC, size 0x4
    float mNOSBoost;                                         // offset 0x100, size 0x4
    float mNOSEngaged;                                       // offset 0x104, size 0x4
    float mClutchRPMDiff;                                    // offset 0x108, size 0x4
    bool mEngineBraking;                                     // offset 0x10C, size 0x1
    float mSportShifting;                                    // offset 0x110, size 0x4
    IInput *mIInput;                                         // offset 0x114, size 0x4
    ISuspension *mSuspension;                                // offset 0x118, size 0x4
    ICheater *mCheater;                                      // offset 0x11C, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::nos> mNOSInfo;             // offset 0x120, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::induction> mInductionInfo; // offset 0x134, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo;       // offset 0x148, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo;  // offset 0x15C, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;          // offset 0x170, size 0x14
    float mRPM;                                              // offset 0x184, size 0x4
    ShiftStatus mShiftStatus;                                // offset 0x188, size 0x4
    ShiftPotential mShiftPotential;                          // offset 0x18C, size 0x4
    float mPeakTorque;                                       // offset 0x190, size 0x4
    float mPeakTorqueRPM;                                    // offset 0x194, size 0x4
    float mMaxHP;                                            // offset 0x198, size 0x4
    struct Clutch mClutch;                                   // offset 0x19C, size 0xC
    bool mBlown;                                             // offset 0x1A8, size 0x1
    float mSabotage;                                         // offset 0x1AC, size 0x4
};

#endif
