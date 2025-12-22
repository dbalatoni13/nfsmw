#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Events/EEngineBlown.hpp"
#include "Speed/Indep/Src/Generated/Events/EMissShift.hpp"
#include "Speed/Indep/Src/Generated/Events/EPerfectShift.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerShift.hpp"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x164
class EngineSpline : protected VehicleBehavior, protected ITransmission, protected IEngine, public IAttributeable, public INISCarEngine {
  public:
    // Methods
    static Behavior *Construct(const BehaviorParams &params);

    EngineSpline(const BehaviorParams &bp);
    float GetGearMaxAV(int whichGear);
    float GetAVRpmRatio(int whichGear);
    float GetTorquePoint(float rpm) const;
    void CalcShiftPoints();
    float GetShiftUpRPM(int gear);
    float GetShiftDownRPM(int gear);
    void AutoShift();
    float GetDifferentialAngularVelocity() const;
    float CalcSpeedometer(float rpm, unsigned int gear) const;

    // Overrides
    ~EngineSpline() override;

    // IEngine
    void MatchSpeed(float speed) override;
    float GetHorsePower() const override;

    // Behavior
    void Reset() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // ITransmission
    bool Shift(GearID gear) override;
    float GetSpeedometer() const override;
    float GetMaxSpeedometer() const override;
    float GetShiftPoint(GearID from_gear, GearID to_gear) const override;

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) override;

    // INISCarEngine
    void RestoreState() override;

    // Inline virtuals

    float GetDriveTorque() const override {
        return 0.0f;
    }

    // IEngine
    float GetMaxHorsePower() const override {
        return mMaxHP;
    }
    Hp GetMinHorsePower() const override {
        return FTLB2HP(Physics::Info::Torque(mEngineInfo, mEngineInfo.IDLE()) * mEngineInfo.IDLE(), 1.0f);
    }
    float GetRPM() const override {
        return mRPM;
    }
    float GetMaxRPM() const override {
        return mEngineInfo.MAX_RPM();
    }
    float GetPeakTorqueRPM() const override {
        return mMaxTorqueRPM;
    }
    float GetRedline() const override {
        return mEngineInfo.RED_LINE();
    }
    float GetMinRPM() const override {
        return mEngineInfo.IDLE();
    }
    float GetNOSCapacity() const override {
        return mNOSCapacity;
    }
    bool IsNOSEngaged() const override {
        return mNitro;
    }
    bool HasNOS() const override {
        return mNOSInfo.NOS_CAPACITY() > 0.0f;
    }
    float GetNOSFlowRate() const override {
        return mNOSInfo.FLOW_RATE();
    }
    void ChargeNOS(float charge) override {
        mNOSCapacity = UMath::Clamp(mNOSCapacity + charge, 0.0f, 1.0f);
    }
    float GetNOSBoost() const override {
        return 1.0f;
    }

    virtual bool IsEngineBraking() {
        return mEngineBraking;
    }
    virtual bool IsShiftingGear() {
        return mGearShiftTimer > 0.0f;
    }
    bool IsReversing() const override {
        return mGear == G_REVERSE;
    }

    // ITransmission
    GearID GetTopGear() const override {
        return (GearID)(GetNumGearRatios() - 1);
    }
    GearID GetGear() const override {
        return (GearID)mGear;
    }
    bool IsGearChanging() const override {
        return mGearShiftTimer > 0.0f;
    }

    ShiftStatus GetShiftStatus() const override {
        return SHIFT_STATUS_NORMAL;
    }
    virtual ShiftPotential GetShiftPotential(GearID gear, float rpm) const {
        return SHIFT_POTENTIAL_NONE;
    }
    ShiftPotential GetShiftPotential() const override {
        return SHIFT_POTENTIAL_NONE;
    }

    // INISCarEngine
    void SetNitro(bool b) override {
        mNitro = b;
    }
    void SetNeutralRev(bool b, float throttle, float speed) override {
        mNeutralRev = b;
        mNeutralRevRPM = mEngineInfo.IDLE() + (mEngineInfo.RED_LINE() - mEngineInfo.IDLE()) * throttle;
        mNeutralRevSpeed = speed;
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

    float GetShiftDelay(unsigned int gear) const {
        return mTranyInfo.SHIFT_SPEED() * GetGearRatio(gear);
    }

    bool RearWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }

    bool FrontWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

  private:
    int mGear;                                               // offset 0x6C, size 0x4
    int mDesiredGear;                                        // offset 0x70, size 0x4
    float mGearShiftTimer;                                   // offset 0x74, size 0x4
    float mThrottle;                                         // offset 0x78, size 0x4
    float mShiftUpRPM[10];                                   // offset 0x7C, size 0x28
    float mShiftDownRPM[10];                                 // offset 0xA4, size 0x28
    float mAngularVelocity;                                  // offset 0xCC, size 0x4
    bool mEngineBraking;                                     // offset 0xD0, size 0x1
    IInput *mIInput;                                         // offset 0xD4, size 0x4
    ISuspension *mSuspension;                                // offset 0xD8, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo;       // offset 0xDC, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo;  // offset 0xF0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;          // offset 0x104, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::induction> mInductionInfo; // offset 0x118, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::nos> mNOSInfo;             // offset 0x12C, size 0x14
    float mRPM;                                              // offset 0x140, size 0x4
    bool mNeutralRev;                                        // offset 0x144, size 0x1
    float mNeutralRevRPM;                                    // offset 0x148, size 0x4
    float mNeutralRevSpeed;                                  // offset 0x14C, size 0x4
    bool mNitro;                                             // offset 0x150, size 0x1
    float mNOSCapacity;                                      // offset 0x154, size 0x4
    float mMaxTorque;                                        // offset 0x158, size 0x4
    float mMaxTorqueRPM;                                     // offset 0x15C, size 0x4
    float mMaxHP;                                            // offset 0x160, size 0x4
};

Behavior *EngineSpline::Construct(const BehaviorParams &params) {
    return new EngineSpline(params);
}

EngineSpline::EngineSpline(const BehaviorParams &bp)
    : VehicleBehavior(bp, 0), ITransmission(bp.fowner), IEngine(bp.fowner), INISCarEngine(bp.fowner), mGear(G_NEUTRAL), mDesiredGear(G_NEUTRAL),
      mGearShiftTimer(0.0f), mThrottle(0.0f), mAngularVelocity(0.0f), mEngineBraking(false), mIInput(NULL), mSuspension(NULL), mEngineInfo(this, 0),
      mTranyInfo(this, 0), mTireInfo(this, 0), mInductionInfo(this, 0), mNOSInfo(this, 0), mRPM(0.0f), mNeutralRev(false), mNeutralRevRPM(0.0f),
      mNeutralRevSpeed(0.0f), mNitro(false), mNOSCapacity(0.0f) {
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    EnableProfile("EngineSpline");

    GetOwner()->QueryInterface(&mIInput);
    GetOwner()->QueryInterface(&mSuspension);
    Reset();

    Attrib::Gen::pvehicle pvehicle(GetOwner()->GetAttributes());
    mMaxTorque = Physics::Info::MaxInductedTorque(pvehicle, mMaxTorqueRPM, NULL);
    mMaxTorque = FTLB2NM(mMaxTorque);
    mMaxHP = Physics::Info::MaxInductedPower(pvehicle, NULL);

    CalcShiftPoints();
}

EngineSpline::~EngineSpline() {
    IAttributeable::UnRegister(this);
}

void EngineSpline::RestoreState() {
    mNeutralRev = false;
    mNeutralRevRPM = 0.0f;
    mNeutralRevSpeed = 0.0f;
    mNitro = false;
    mRPM = mEngineInfo.IDLE();
    mAngularVelocity = RPM2RPS(mRPM);
    mThrottle = 0.0f;
}

float EngineSpline::GetHorsePower() const {
    float engine_torque = GetTorquePoint(mRPM);
    return NM2HP(engine_torque * mThrottle, mRPM);
}

void EngineSpline::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
}

void EngineSpline::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

void EngineSpline::Reset() {
    mAngularVelocity = 0.0f;
    mDesiredGear = G_FIRST;
    mGearShiftTimer = 0.0f;
    mThrottle = 0.0f;
    mGear = G_FIRST;

    CalcShiftPoints();
}

float EngineSpline::GetTorquePoint(float rpm) const {
    float ftlbs = Physics::Info::Torque(mEngineInfo, rpm);
    float boost = Physics::Info::InductionBoost(mEngineInfo, mInductionInfo, rpm, 1.0f, NULL, NULL) + 1.0f;
    return FTLB2NM(ftlbs * boost);
}

void EngineSpline::MatchSpeed(float speed) {
    float avg_wheel_radius = 0.0f;

    if (mSuspension) {
        unsigned int numwheels = mSuspension->GetNumWheels();
        for (unsigned int i = 0; i < numwheels; ++i) {
            avg_wheel_radius += mSuspension->GetWheelRadius(i);
        }
        avg_wheel_radius /= numwheels;
    }

    if (avg_wheel_radius <= 0.0f) {
        return;
    }

    float differential_w = speed / avg_wheel_radius;
    float max_w = RPM2RPS(mEngineInfo.RED_LINE());
    float min_w = RPM2RPS(mEngineInfo.IDLE());

    if (speed == 0.0f) {
        mGearShiftTimer = 0.0f;
        mGear = G_FIRST;
        mAngularVelocity = RPM2RPS(mEngineInfo.IDLE());
        return;
    }

    if (speed < 0.0f) {
        mGearShiftTimer = 0.0f;
        mGear = G_REVERSE;
    } else {
        mGear = G_FIRST;
        for (unsigned int gear = G_FIRST; gear < GetNumGearRatios(); ++gear) {
            float gear_ratio = GetGearRatio(gear) * GetFinalGear();
            if (gear_ratio > 0.0f) {
                float gear_topspeed = avg_wheel_radius * max_w / gear_ratio;
                if (gear_topspeed < speed) {
                    mGear = gear;
                }
            }
        }
    }

    float total_gear_ratio = GetGearRatio(mGear) * GetFinalGear();
    float power_range = (max_w - min_w) / max_w;
    mAngularVelocity = min_w + differential_w * total_gear_ratio * power_range;
}

void EngineSpline::CalcShiftPoints() {
    bool shift_points_calced = Physics::Info::ShiftPoints(mTranyInfo, mEngineInfo, mInductionInfo, mShiftUpRPM, mShiftDownRPM, 10);
}

float EngineSpline::GetShiftUpRPM(int gear) {
    return mShiftUpRPM[gear];
}

float EngineSpline::GetShiftDownRPM(int gear) {
    return mShiftDownRPM[gear];
}

void EngineSpline::AutoShift() {
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
            int have_traction = 1;
            for (int i = 0; i < 4; ++i) {
                have_traction &= mSuspension->IsWheelOnGround(i) && mSuspension->GetWheelSlip(i) < 4.f;
            }

            if (have_traction) {
                Shift((GearID)(mGear + 1));
            }
        } else if (rpm <= shift_down_rpm && mGear > G_NEUTRAL && rpm <= lower_gear_shift_up_rpm) {
            Shift((GearID)(mGear - 1));
        }
    }
}

bool EngineSpline::Shift(GearID gear) {
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

float EngineSpline::GetDifferentialAngularVelocity() const {
    float into_gearbox = 0.0f;

    if (RearWheelDrive()) {
        float ave_rear = (mSuspension->GetWheelAngularVelocity(2) + mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        into_gearbox = ave_rear;
    }
    if (FrontWheelDrive()) {
        float ave_front = (mSuspension->GetWheelAngularVelocity(0) + mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        into_gearbox += ave_front;
        if (RearWheelDrive()) {
            into_gearbox *= 0.5f;
        }
    }

    return into_gearbox;
}

float EngineSpline::CalcSpeedometer(float rpm, unsigned int gear) const {
    return Physics::Info::Speedometer(mTranyInfo, mEngineInfo, mTireInfo, rpm, (GearID)gear, NULL);
}

float EngineSpline::GetMaxSpeedometer() const {
    unsigned int num_ratios = GetNumGearRatios();
    if (num_ratios > 0) {
        float limiter = MPH2MPS(mEngineInfo.SPEED_LIMITER(0));
        float max_speedometer = CalcSpeedometer(mEngineInfo.RED_LINE(), num_ratios - 1);
        if (limiter > 0.0f) {
            return UMath::Min(max_speedometer, limiter);
        } else {
            return max_speedometer;
        }
    } else {
        return 0.0f;
    }
}

float EngineSpline::GetSpeedometer() const {
    return CalcSpeedometer(RPS2RPM(mAngularVelocity), mGear);
}

extern float Engine_SmoothRPM(bool is_shifting, GearID gear, float dT, float old_rpm, float new_rpm, float engine_inertia);

void EngineSpline::OnTaskSimulate(float dT) {
    IInput *iinput = mIInput;
    if (iinput == NULL || mSuspension == NULL) {
        return;
    }

    if (mNeutralRev) {
        float rpm_diff = mEngineInfo.RED_LINE() - mEngineInfo.IDLE();

        if (mNeutralRevSpeed <= FLOAT_EPSILON) {
            mRPM = mNeutralRevRPM;
        } else if (mRPM < mNeutralRevRPM) {
            mRPM += mNeutralRevSpeed * rpm_diff * dT;
            mRPM = UMath::Min(mNeutralRevRPM, mRPM);
        } else {
            mRPM -= mNeutralRevSpeed * rpm_diff * dT;
            mRPM = UMath::Max(mNeutralRevRPM, mRPM);
        }

        mGear = G_NEUTRAL;
        mAngularVelocity = RPM2RPS(mRPM);
        mThrottle = UMath::Ramp(mRPM, mEngineInfo.IDLE(), mEngineInfo.RED_LINE());
    } else {
        float differential_w = GetDifferentialAngularVelocity();
        if (differential_w < -(float)M_PI && GetGear() != G_REVERSE) {
            Shift(G_REVERSE);
        } else if (differential_w > (float)M_PI && GetGear() <= G_NEUTRAL) {
            Shift(G_FIRST);
        } else {
            AutoShift();
        }

        if (mGearShiftTimer <= 0.0f) {
            mGearShiftTimer = 0.0f;
        } else {
            mGearShiftTimer -= dT;
            if (mGearShiftTimer < 0.0f) {
                mGearShiftTimer = 0.0f;
            }
        }

        float max_w = RPM2RPS(mEngineInfo->RED_LINE());
        float min_w = RPM2RPS(mEngineInfo->IDLE());
        float engine_inertia = Physics::Info::EngineInertia(mEngineInfo, mGear != G_NEUTRAL);
        float total_gear_ratio = GetGearRatio(mGear) * GetFinalGear();
        float gear_direction = mGear == G_REVERSE ? -1.0f : 1.0f; // unused, from debug info
        float power_range = (max_w - min_w) / max_w;
        float av = min_w + UMath::Abs(differential_w) * total_gear_ratio * power_range;
        mEngineBraking = av < mAngularVelocity;
        av = UMath::Clamp(av, min_w, max_w);
        mAngularVelocity = av;
        mRPM = Engine_SmoothRPM(IsShiftingGear(), GetGear(), dT, mRPM, RPS2RPM(mAngularVelocity), engine_inertia);
        mThrottle = UMath::Clamp((mAngularVelocity - min_w) / (max_w - min_w), 0.0f, 1.0f);
    }

    mIInput->SetControlGas(mThrottle);
}

float EngineSpline::GetShiftPoint(GearID from_gear, GearID to_gear) const {
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
