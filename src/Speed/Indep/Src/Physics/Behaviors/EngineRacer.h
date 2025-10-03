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

namespace Physics {
struct Tunings;
} // namespace Physics

// Credits: Brawltendo
class EngineRacer : protected VehicleBehavior,
                    protected ITransmission,
                    protected IEngine,
                    public IAttributeable,
                    public IInductable,
                    public ITiptronic,
                    public IRaceEngine,
                    public IEngineDamage {
  public:
    virtual ~EngineRacer();
    virtual void OnService();
    virtual void Reset();
    virtual void GetPriority();
    virtual void OnOwnerAttached();
    virtual void OnOwnerDetached();
    virtual void OnTaskSimulate(float dT);
    virtual void OnBehaviorChange(const UCrc32 &);
    virtual void OnPause();
    virtual void OnUnPause();

    virtual bool IsEngineBraking() {
        return mEngineBraking;
    }

    virtual bool IsShiftingGear() {
        return mGearShiftTimer > 0.0f;
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

    EngineRacer(const BehaviorParams &bp);

    unsigned int GetNumGearRatios() const {
        return mTrannyInfo.Num_GEAR_RATIO();
    }

    float GetGearRatio(unsigned int idx) const {
        return mTrannyInfo.GEAR_RATIO(idx);
    }

    float GetGearEfficiency(unsigned int idx) const {
        return mTrannyInfo.GEAR_EFFICIENCY(idx);
    }

    float GetFinalGear() const {
        return mTrannyInfo.FINAL_GEAR();
    }

    float GetRatioChange(unsigned int from, unsigned int to) const {
        float ratio1 = mTrannyInfo.GEAR_RATIO(from);
        float ratio2 = mTrannyInfo.GEAR_RATIO(to);

        if (ratio1 > 0.0f && ratio2 > FLOAT_EPSILON)
            return ratio1 / ratio2;
        else
            return 0.0f;
    }

    float GetShiftDelay(unsigned int gear) const {
        return mTrannyInfo.SHIFT_SPEED() * GetGearRatio(gear);
    }

    bool RearWheelDrive() const {
        return mTrannyInfo.TORQUE_SPLIT() < 1.0f;
    }

    bool FrontWheelDrive() const {
        return mTrannyInfo.TORQUE_SPLIT() > 0.0f;
    }

    float GetShiftUpRPM(int gear) const {
        return mShiftUpRPM[gear];
    }

    float GetShiftDownRPM(int gear) const {
        return mShiftDownRPM[gear];
    }

    ShiftPotential FindShiftPotential(GearID gear, float rpm) const;
    float GetDifferentialAngularVelocity(bool locked) const;
    float GetDriveWheelSlippage() const;
    void SetDifferentialAngularVelocity(float w);
    float CalcSpeedometer(float rpm, unsigned int gear) const;
    void LimitFreeWheels(float w);
    float GetBrakingTorque(float engine_torque, float rpm) const;
    bool DoGearChange(GearID gear, bool automatic);
    void AutoShift();

    // ITransmission

    bool Shift(GearID gear);
    float GetSpeedometer() const;
    float GetMaxSpeedometer() const;
    float GetShiftPoint(GearID from_gear, GearID to_gear) const;

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

    // IRaceEngine

    float GetPerfectLaunchRange(float &range);

  private:
    float mDriveTorque;
    GearID mGear;
    float mGearShiftTimer;
    float mThrottle;
    float mSpool;
    float mPSI;
    float mInductionBoost;
    float mShiftUpRPM[10];
    float mShiftDownRPM[10];
    float mAngularVelocity;
    float mAngularAcceleration;
    float mTransmissionVelocity;
    float mNOSCapacity;
    float mNOSBoost;
    float mNOSEngaged;
    float mClutchRPMDiff;
    bool mEngineBraking;
    float mSportShifting;
    IInput *mIInput;
    ISuspension *mSuspension;
    ICheater *mCheater;
    Attrib::Gen::nos mNOSInfo;
    Attrib::Gen::induction mInductionInfo;
    Attrib::Gen::engine mEngineInfo;
    Attrib::Gen::transmission mTrannyInfo;
    Attrib::Gen::tires mTireInfo;
    float mRPM;
    ShiftStatus mShiftStatus;
    ShiftPotential mShiftPotential;
    float mPeakTorque;
    float mPeakTorqueRPM;
    float mMaxHP;

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
    } mClutch;

    float mBlown;
    float mSabotage;
    int gap158;
};

#endif
