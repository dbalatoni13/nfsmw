#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
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
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/Events/EEngineBlown.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerShift.h"
#include "Speed/Indep/Src/Generated/Events/EPerfectShift.h"
#include "Speed/Indep/Src/Generated/Events/EMissShift.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x124
class EngineTraffic : protected VehicleBehavior,
                    protected ITransmission,
                    protected IEngine,
                    public IAttributeable {
  public:
    // Methods
    static Behavior *Construct(const BehaviorParams &params);

    EngineTraffic(const BehaviorParams &bp);
    float GetTorquePoint(float rpm) const;
    float GetBrakingTorque(float engine_torque, float rpm) const;
    void CalcShiftPoints();
    void AutoShift();
    float GetDifferentialAngularVelocity() const;
    float CalcSpeedometer(float rpm, unsigned int gear) const;
    void DoShifting(float dT);

    // Overrides
    override virtual ~EngineTraffic();

    // IEngine
    override virtual void MatchSpeed(float speed);
    override virtual float GetMaxHorsePower() const;
    override virtual float GetHorsePower() const;

    // Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);
    override virtual void OnBehaviorChange(const UCrc32 &mechanic);

    // ITransmission
    override virtual bool Shift(GearID gear);
    override virtual float GetSpeedometer() const;
    override virtual float GetMaxSpeedometer() const;
    override virtual float GetShiftPoint(GearID from_gear, GearID to_gear) const;

    // IAttributeable
    override virtual void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey);

    // Inline virtuals

    // IEngine
    override virtual float GetMinHorsePower() const { return FTLB2NM(Physics::Info::Torque(mEngineInfo, mEngineInfo.IDLE()) * mEngineInfo.IDLE()); }
    override virtual float GetRPM() const { return mRPM; }
    override virtual float GetMaxRPM() const { return mEngineInfo.MAX_RPM(); }
    override virtual float GetPeakTorqueRPM() const { return mPeakTorqueRPM; }
    override virtual float GetRedline() const { return mEngineInfo.RED_LINE(); }
    override virtual float GetMinRPM() const { return mEngineInfo.IDLE(); }
    override virtual float GetNOSCapacity() const { return 0.0f; }
    override virtual bool IsNOSEngaged() const { return false; }
    override virtual bool HasNOS() const { return false; }
    override virtual float GetNOSFlowRate() const { return 0.0f; }
    override virtual void ChargeNOS(float charge) { }
    override virtual float GetNOSBoost() const { return 1.0f; }

    virtual bool IsEngineBraking() { return mEngineBraking; }
    virtual bool IsShiftingGear() { return mGearShiftTimer > 0.0f; }
    override virtual bool IsReversing() const { return mGear == G_REVERSE; }

    virtual Physics::Info::eInductionType InductionType() const { return Physics::Info::INDUCTION_NONE; }
    virtual float GetInductionPSI() const { return 0.0f; }
    virtual float InductionSpool() const { return 0.0f; }
    virtual float GetMaxInductionPSI() const { return 0.0f; }

    // ITransmission
    override virtual float GetDriveTorque() const { return mDriveTorque; }
    override virtual GearID GetTopGear() const { return (GearID)(GetNumGearRatios() - 1); }
    override virtual GearID GetGear() const { return (GearID)mGear; }
    override virtual bool IsGearChanging() const { return mGearShiftTimer > 0.0f; }

    override virtual ShiftStatus GetShiftStatus() const { return SHIFT_STATUS_NORMAL; }
    virtual ShiftPotential GetShiftPotential(GearID gear, float rpm) const { return SHIFT_POTENTIAL_NONE; }
    override virtual ShiftPotential GetShiftPotential() const { return SHIFT_POTENTIAL_NONE; }
    
    // Inlines
    unsigned int GetNumGearRatios() const {
        return mTranyInfo.Num_GEAR_RATIO();
    }

    float GetGearRatio(unsigned int idx) const {
        return mTranyInfo->GEAR_RATIO(idx);
    }

    float GetGearEfficiency(unsigned int idx) const {
        return mTranyInfo->GEAR_EFFICIENCY(idx);
    }

    float GetFinalGear() const {
        return mTranyInfo.FINAL_GEAR();
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
    float mDriveTorque;                                     // offset 0x64, size 0x4
    int mGear;                                              // offset 0x68, size 0x4
    float mGearShiftTimer;                                  // offset 0x6C, size 0x4
    float mThrottle;                                        // offset 0x70, size 0x4
    float mShiftUpRPM[10];                                  // offset 0x74, size 0x28
    float mShiftDownRPM[10];                                // offset 0x9C, size 0x28
    float mAngularVelocity;                                 // offset 0xC4, size 0x4
    float mClutchVelocity;                                  // offset 0xC8, size 0x4
    bool mEngineBraking;                                    // offset 0xCC, size 0x1
    IInput * mIInput;                                       // offset 0xD0, size 0x4
    ISuspension * mSuspension;                              // offset 0xD4, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo;      // offset 0xD8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo; // offset 0xEC, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;         // offset 0x100, size 0x14
    float mRPM;                                             // offset 0x114, size 0x4
    float mPeakTorque;                                      // offset 0x118, size 0x4
    float mPeakTorqueRPM;                                   // offset 0x11C, size 0x4
    float mMaxHP;                                           // offset 0x120, size 0x4
};

Behavior *EngineTraffic::Construct(const BehaviorParams &params) {
    return new EngineTraffic(params);
}

EngineTraffic::EngineTraffic(const BehaviorParams &bp)
: VehicleBehavior(bp, 0)
, ITransmission(bp.fowner)
, IEngine(bp.fowner)
, mDriveTorque(0.0f)
, mGear(G_NEUTRAL)
, mGearShiftTimer(0.0f)
, mThrottle(0.0f)
, mAngularVelocity(0.0f)
, mClutchVelocity(0.0f)
, mEngineBraking(false)
, mIInput(NULL)
, mSuspension(NULL)
, mEngineInfo(this, 0)
, mTranyInfo(this, 0)
, mTireInfo(this, 0)
, mRPM(0.0f)
, mPeakTorque(0.0f)
, mPeakTorqueRPM(0.0f)
, mMaxHP(0.0f) {
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    EnableProfile("EngineTraffic");

    GetOwner()->QueryInterface(&mIInput);
    GetOwner()->QueryInterface(&mSuspension);
    Reset();
    CalcShiftPoints();
}

EngineTraffic::~EngineTraffic() {
    IAttributeable::UnRegister(this);
}

float EngineTraffic::GetMaxHorsePower() const { return mMaxHP; }

float EngineTraffic::GetHorsePower() const {
    float engine_torque = GetTorquePoint(mRPM);
    return NM2HP(engine_torque * mThrottle, mRPM);
}

void EngineTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
}

void EngineTraffic::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

void EngineTraffic::Reset() {
    mDriveTorque = 0.0f;
    mAngularVelocity = RPM2RPS(mEngineInfo.IDLE());
    mRPM = mEngineInfo.IDLE();
    mClutchVelocity = 0.0f;
    mGearShiftTimer = 0.0f;
    mGear = G_FIRST;
    mThrottle = 0.0f;

    CalcShiftPoints();
}

float EngineTraffic::GetTorquePoint(float rpm) const {
    float ftlbs = Physics::Info::Torque(mEngineInfo, rpm);
    return FTLB2NM(ftlbs);
}

void EngineTraffic::MatchSpeed(float speed) {
    float wheelrear = Physics::Info::WheelDiameter(mTireInfo, false) * 0.5f;
    float wheelfront = Physics::Info::WheelDiameter(mTireInfo, true) * 0.5f;
    float avg_wheel_radius = (wheelrear + wheelfront) * 0.5f;

    if (avg_wheel_radius <= 0.0f) {
        return;
    }

    float differential_w = speed / avg_wheel_radius;
    float max_w = RPM2RPS(mEngineInfo->RED_LINE());
    float min_w = RPM2RPS(mEngineInfo->IDLE());
    float rear_end = GetFinalGear();

    if (speed == 0.0f) {
        mGearShiftTimer = 0.0f;
        mGear = G_FIRST;
        mAngularVelocity = RPM2RPS(mEngineInfo->IDLE());
        mClutchVelocity  = mAngularVelocity;
        return;
    }
    
    if (speed < 0.0f) {
        mGearShiftTimer = 0.0f;
        mGear = G_REVERSE;
    } else {
        mGear = G_FIRST;
        for (unsigned int gear = G_FIRST; gear < GetNumGearRatios(); ++gear) {
            float gear_ratio = GetGearRatio(gear) * rear_end;
            if (gear_ratio > 0.0f) {
                float gear_topspeed = avg_wheel_radius * max_w / gear_ratio;
                if (gear_topspeed < speed) {
                    mGear = gear;
                }
            }
        }
    }
    
    float total_gear_ratio = GetGearRatio(mGear) * rear_end;
    float power_range = (max_w - min_w) / max_w;
    mAngularVelocity = differential_w * total_gear_ratio * power_range + min_w;
    mClutchVelocity  = mAngularVelocity;
    mRPM = RPS2RPM(mAngularVelocity);
}

float EngineTraffic::GetBrakingTorque(float engine_torque, float rpm) const {
    unsigned int numpts = mEngineInfo.Num_ENGINE_BRAKING();
    if (numpts > 1) {
        float rpm_min = mEngineInfo.IDLE();
        float rpm_max = mEngineInfo.MAX_RPM();
        float ratio;
        unsigned int index =
            UTIL_InterprolateIndex(numpts - 1, UMath::Clamp(rpm, mEngineInfo.IDLE(), mEngineInfo.RED_LINE()), rpm_min, rpm_max, ratio);

        float base = mEngineInfo.ENGINE_BRAKING(index);
        unsigned int secondIndex = index + 1;
        float step = mEngineInfo.ENGINE_BRAKING(UMath::Min(numpts - 1, secondIndex));
        float load_pct = (step - base) * ratio + base;
        return -engine_torque * UMath::Clamp(load_pct, 0.f, 1.f);
    } else {
        return -engine_torque * mEngineInfo.ENGINE_BRAKING(0);
    }
}

void EngineTraffic::CalcShiftPoints() {
    bool shift_points_calced = Physics::Info::ShiftPoints(mTranyInfo, mEngineInfo,
                                                          Attrib::Gen::induction((Attrib::Collection *)NULL, 0, NULL),
                                                          mShiftUpRPM, mShiftDownRPM, 10);

    Attrib::Gen::pvehicle pvehicle(GetOwner()->GetAttributes());
    mPeakTorque = Physics::Info::MaxInductedTorque(pvehicle, mPeakTorqueRPM, NULL);
    mPeakTorque = FTLB2NM(mPeakTorque);
    mMaxHP = Physics::Info::MaxInductedPower(pvehicle, NULL);
}

void EngineTraffic::AutoShift() {
    if (mGear == G_REVERSE || mGearShiftTimer > 0.0f)
        return;

    float rpm = RPS2RPM(mAngularVelocity);
    if (mGear == G_NEUTRAL) {
        mGear = G_FIRST;
    } else if (mGearShiftTimer <= 0.0f) {
        float shift_up_rpm = GetShiftUpRPM(mGear);
        float shift_down_rpm = GetShiftDownRPM(mGear);

        float lower_gear_shift_up_rpm = 0.0f;
        float lower_gear_ratio = GetGearRatio(mGear - 1);
        if (mGear != G_FIRST && lower_gear_ratio > 0.0f) {
            lower_gear_shift_up_rpm = GetShiftUpRPM(mGear - 1);
            lower_gear_shift_up_rpm *= GetGearRatio(mGear) / lower_gear_ratio;
            lower_gear_shift_up_rpm -= 200.0f;
        }

        if (mGear <= G_FIRST) {
            shift_down_rpm = mThrottle < 0.2f ? mEngineInfo->IDLE() + 100.0f : 0.0f;
        } 

        if (rpm >= shift_up_rpm && mGear < GetTopGear()) {
            Shift((GearID)(mGear + 1));
        } else if (rpm <= shift_down_rpm && mGear > G_NEUTRAL && rpm <= lower_gear_shift_up_rpm) {
            Shift((GearID)(mGear - 1));
        }
    }
}

bool EngineTraffic::Shift(GearID gear) {
    if (gear < GetNumGearRatios() && gear != mGear && gear >= G_REVERSE) {
        if (gear < mGear) {
            mGearShiftTimer = GetShiftDelay(gear) * 0.25f;
        } else {
            mGearShiftTimer = GetShiftDelay(gear);
        }
        mGear = gear;
        return true;
    }

    return false;
}

float EngineTraffic::GetDifferentialAngularVelocity() const {
    float into_gearbox = 0.0f;
    bool in_reverse = GetGear() == G_REVERSE;

    if (FrontWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(0) + mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        into_gearbox = in_reverse ? UMath::Min(w_vel, 0.0f) : UMath::Max(w_vel, 0.0f);
    }

    if (RearWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(2) + mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        into_gearbox = in_reverse ? UMath::Min(w_vel, into_gearbox) : UMath::Max(w_vel, into_gearbox);
    }

    return into_gearbox;
}

float EngineTraffic::GetMaxSpeedometer() const {
    return Physics::Info::Speedometer(mTranyInfo, mEngineInfo, mTireInfo, GetRedline(), GetTopGear(), NULL);
}

float EngineTraffic::GetSpeedometer() const {
    return UMath::Abs(GetVehicle()->GetLocalVelocity().z);
}

void EngineTraffic::DoShifting(float dT) {
    if (mGear > G_REVERSE) {
        AutoShift();
    }

    if (mGearShiftTimer > 0.0f) {
        mGearShiftTimer -= dT;
        if (mGearShiftTimer <= 0.0f) {
            mGearShiftTimer = 0.0f;
        }
    }
}

void EngineTraffic::OnTaskSimulate(float dT) {
    IInput *iinput = mIInput;
    if (iinput == NULL || mSuspension == NULL) {
        return;
    }

    if (mSuspension->GetNumWheels() != 4) {
        return;
    }

    mThrottle = mIInput->GetControls().fGas;
    float steering = iinput->GetControls().fSteering; // unused, from debug info
    DoShifting(dT);
    float max_w = RPM2RPS(mEngineInfo->RED_LINE());
    float min_w = RPM2RPS(mEngineInfo->IDLE());
    float target_w = max_w * mThrottle;
    float engine_inertia = Physics::Info::EngineInertia(mEngineInfo, true);
    float differential_w = GetDifferentialAngularVelocity();
    float clutch_ratio = 1.0f;
    if (mGearShiftTimer > 0.0f) {
        clutch_ratio = 0.0f;
    }
    float torque_total = 0.0f;
    float gear_direction = mGear == G_REVERSE ? -1.0f : 1.0f;
    float total_gear_ratio = GetGearRatio(mGear) * GetFinalGear() * gear_direction;
    
    float converter_ratio = 1.0f;
    float torque_converter = mTranyInfo.TORQUE_CONVERTER();
    if (torque_converter > 0.0f) {
        float rpm = RPS2RPM(mAngularVelocity);
        converter_ratio = 1.0f + torque_converter * clutch_ratio * mThrottle * (1.0f - UMath::Ramp(rpm, mEngineInfo.IDLE(), mPeakTorqueRPM));
        total_gear_ratio *= converter_ratio;
    }

    UVector3 vUp;
    GetOwner()->GetRigidBody()->GetUpVector(vUp);
    bool collision_disengage = vUp.y < 0.3f;
    mEngineBraking = false;
    if (mGear == G_NEUTRAL || collision_disengage || total_gear_ratio == 0.0f) {
        float rpm = RPS2RPM(mAngularVelocity);
        float engine_torque = GetTorquePoint(rpm);
        float braking_torque = GetBrakingTorque(engine_torque, rpm);
        float neutral_torque = (engine_torque * mThrottle + braking_torque * (1.0f - mThrottle)) / engine_inertia;

        float dW = 4.0f * dT;
        mAngularVelocity += neutral_torque * dW;
        if (neutral_torque > 0.0f) {
            mAngularVelocity = UMath::Min(mAngularVelocity, target_w);
        } else {
            mAngularVelocity = UMath::Max(mAngularVelocity, target_w);
        }

        mDriveTorque = 0.0f;
        mClutchVelocity = min_w;
        clutch_ratio = 0.0f;
    } else {
        float rpm = RPS2RPM(mAngularVelocity);
        float counter_torque; // unused, from debug info
        float engine_torque = GetTorquePoint(rpm);  // unused, from debug info
        float drive_torque = engine_torque;
        float breaking_torque = GetBrakingTorque(drive_torque, rpm); // yes it's misspelled here despite being correct above
        if (differential_w * total_gear_ratio < 0.0f) {
            breaking_torque = -breaking_torque;
        }
        if (differential_w == 0.0f) {
            breaking_torque = 0.0;
        }

        float power_range = (max_w - min_w) / max_w;
        float transmission_velocity = differential_w * total_gear_ratio * power_range + min_w;
        mClutchVelocity = transmission_velocity;
        torque_total += drive_torque * mThrottle + breaking_torque * (1.0f - mThrottle);

        if (transmission_velocity > max_w) {
            torque_total = breaking_torque * (transmission_velocity / max_w - 1.0f) + breaking_torque * (1.0f - mThrottle);
        }

        if (transmission_velocity > target_w) {
            float d = transmission_velocity - mAngularVelocity;
            // debug info says UMath::Abs got inlined but it was optimized away
            // A124 has fewer optmizations enabled and gives us this
            float dW = UMath::Abs(0.0f) / engine_inertia * dT;
            mAngularVelocity += UMath::Min(dW, d);
            mEngineBraking = true;
        } else if (transmission_velocity == target_w) {
            torque_total = 0.0f;
        } else if (transmission_velocity < target_w) {
            float engine_acceleration; // unused, from debug info
            float dW = dT * torque_total / engine_inertia;
            float clutch_w = transmission_velocity; // unused, from debug info
            float delta_w = mAngularVelocity - clutch_w;
            if (delta_w > dW) {
                mAngularVelocity -= dW * 4.0f;
            } else if (delta_w < -dW) {
                mAngularVelocity += dW;
            } else {
                mAngularVelocity = transmission_velocity;
            }
            mAngularVelocity = UMath::Min(mAngularVelocity, target_w);
        }
    }

    mAngularVelocity = UMath::Clamp(mAngularVelocity, min_w, max_w);
    mDriveTorque = torque_total * total_gear_ratio * GetGearEfficiency(mGear) * clutch_ratio;
    mRPM = RPS2RPM(mAngularVelocity);
}

float EngineTraffic::GetShiftPoint(GearID from_gear, GearID to_gear) const {
    if (from_gear <= G_REVERSE) {
        return 0.0f;
    }
    if (to_gear <= G_NEUTRAL) {
        return 0.0f;
    }
    if (to_gear > from_gear) {
        return mShiftUpRPM[from_gear];
    }
    if (to_gear < from_gear) {
        return mShiftDownRPM[from_gear];
    }
    
    return 0.0f;
}
