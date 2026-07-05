#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x164
class EngineSpline : protected VehicleBehavior, protected ITransmission, protected IEngine, public IAttributeable, public INISCarEngine {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey) override;

    // ITransmission
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
    float GetHorsePower() const override;
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
    void MatchSpeed(float speed) override;

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

    // ITransmission
    bool Shift(GearID gear) override;
    float GetSpeedometer() const override;
    float GetMaxSpeedometer() const override;
    Rpm GetShiftPoint(GearID from_gear, GearID to_gear) const override;
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
    void RestoreState() override;

    // INISCarEngine
    void SetNitro(bool b) override {
        mNitro = b;
    }
    void SetNeutralRev(bool b, float throttle, float speed) override {
        mNeutralRev = b;
        mNeutralRevRPM = mEngineInfo.IDLE() + (mEngineInfo.RED_LINE() - mEngineInfo.IDLE()) * throttle;
        mNeutralRevSpeed = speed;
    }

    // Behavior
    void Reset() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  protected:
    EngineSpline(const BehaviorParams &bp);
    ~EngineSpline() override;
    float GetAVRpmRatio(int whichGear);
    float GetGearMaxAV(int whichGear);
    float GetTorquePoint(float rpm) const;
    void AutoShift();

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

    float GetDifferentialAngularVelocity() const;

    bool RearWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }
    bool FrontWheelDrive() const {
        return mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

    float CalcSpeedometer(float rpm, unsigned int gear) const;
    float GetShiftUpRPM(int gear);
    float GetShiftDownRPM(int gear);
    void CalcShiftPoints();

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
    Nm mMaxTorque;                                           // offset 0x158, size 0x4
    Rpm mMaxTorqueRPM;                                       // offset 0x15C, size 0x4
    Hp mMaxHP;                                               // offset 0x160, size 0x4
};

BIND_BEHAVIOR_FACTORY(EngineSpline);

Behavior *EngineSpline::Construct(const BehaviorParams &params) {
    return new EngineSpline(params);
}

EngineSpline::EngineSpline(const BehaviorParams &bp)
    : VehicleBehavior(bp, 0),   //
      ITransmission(bp.fowner), //
      IEngine(bp.fowner),       //
      INISCarEngine(bp.fowner), //
      mGear(G_NEUTRAL),         //
      mDesiredGear(G_NEUTRAL),  //
      mGearShiftTimer(0.0f),    //
      mThrottle(0.0f),          //
      mAngularVelocity(0.0f),   //
      mEngineBraking(false),    //
      mIInput(nullptr),         //
      mSuspension(nullptr),     //
      mEngineInfo(this, 0),     //
      mTranyInfo(this, 0),      //
      mTireInfo(this, 0),       //
      mInductionInfo(this, 0),  //
      mNOSInfo(this, 0),        //
      mRPM(0.0f),               //
      mNeutralRev(false),       //
      mNeutralRevRPM(0.0f),     //
      mNeutralRevSpeed(0.0f),   //
      mNitro(false),            //
      mNOSCapacity(0.0f)        //
{
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    this->EnableProfile("EngineSpline");

    this->GetOwner()->QueryInterface(&this->mIInput);
    this->GetOwner()->QueryInterface(&this->mSuspension);
    this->Reset();

    Attrib::Gen::pvehicle pvehicle(this->GetOwner()->GetAttributes());
    this->mMaxTorque = Physics::Info::MaxInductedTorque(pvehicle, this->mMaxTorqueRPM, nullptr);
    this->mMaxTorque = FTLB2NM(this->mMaxTorque);
    this->mMaxHP = Physics::Info::MaxInductedPower(pvehicle, nullptr);

    this->CalcShiftPoints();
}

EngineSpline::~EngineSpline() {
    IAttributeable::UnRegister(this);
}

void EngineSpline::RestoreState() {
    this->mNeutralRev = false;
    this->mNeutralRevRPM = 0.0f;
    this->mNeutralRevSpeed = 0.0f;
    this->mNitro = false;
    this->mRPM = mEngineInfo.IDLE();
    this->mAngularVelocity = RPM2RPS(this->mRPM);
    this->mThrottle = 0.0f;
}

float EngineSpline::GetHorsePower() const {
    float engine_torque = GetTorquePoint(this->mRPM);
    return NM2HP(engine_torque * this->mThrottle, this->mRPM);
}

void EngineSpline::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&this->mSuspension);
    }
}

void EngineSpline::OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey) {}

void EngineSpline::Reset() {
    this->mAngularVelocity = 0.0f;
    this->mDesiredGear = G_FIRST;
    this->mGearShiftTimer = 0.0f;
    this->mThrottle = 0.0f;
    this->mGear = G_FIRST;

    this->CalcShiftPoints();
}

// STRIPPED
float EngineSpline::GetGearMaxAV(int whichGear) {
    return 0.0f;
}

// STRIPPED
float EngineSpline::GetAVRpmRatio(int whichGear) {
    return 0.0f;
}

float EngineSpline::GetTorquePoint(float rpm) const {
    float ftlbs = Physics::Info::Torque(this->mEngineInfo, rpm);
    float boost = Physics::Info::InductionBoost(this->mEngineInfo, this->mInductionInfo, rpm, 1.0f, nullptr, nullptr) + 1.0f;
    return FTLB2NM(ftlbs * boost);
}

void EngineSpline::MatchSpeed(float speed) {
    float avg_wheel_radius = 0.0f;

    if (mSuspension != nullptr) {
        unsigned int numwheels = this->mSuspension->GetNumWheels();
        for (unsigned int i = 0; i < numwheels; ++i) {
            avg_wheel_radius += this->mSuspension->GetWheelRadius(i);
        }
        avg_wheel_radius /= numwheels;
    }

    if (avg_wheel_radius <= 0.0f) {
        return;
    }

    float differential_w = speed / avg_wheel_radius;
    float max_w = RPM2RPS(this->mEngineInfo.RED_LINE());
    float min_w = RPM2RPS(this->mEngineInfo.IDLE());

    if (speed == 0.0f) {
        this->mGearShiftTimer = 0.0f;
        this->mGear = G_FIRST;
        this->mAngularVelocity = RPM2RPS(this->mEngineInfo.IDLE());
        return;
    }

    if (speed < 0.0f) {
        this->mGearShiftTimer = 0.0f;
        this->mGear = G_REVERSE;
    } else {
        this->mGear = G_FIRST;
        for (unsigned int gear = G_FIRST; gear < this->GetNumGearRatios(); ++gear) {
            float gear_ratio = this->GetGearRatio(gear) * this->GetFinalGear();
            if (gear_ratio > 0.0f) {
                float gear_topspeed = avg_wheel_radius * max_w / gear_ratio;
                if (gear_topspeed < speed) {
                    mGear = gear;
                }
            }
        }
    }

    float total_gear_ratio = this->GetGearRatio(mGear) * this->GetFinalGear();
    float power_range = (max_w - min_w) / max_w;
    mAngularVelocity = min_w + differential_w * total_gear_ratio * power_range;
}

void EngineSpline::CalcShiftPoints() {
    bool shift_points_calced =
        Physics::Info::ShiftPoints(this->mTranyInfo, this->mEngineInfo, this->mInductionInfo, this->mShiftUpRPM, this->mShiftDownRPM, 10);
}

float EngineSpline::GetShiftUpRPM(int gear) {
    return this->mShiftUpRPM[gear];
}

float EngineSpline::GetShiftDownRPM(int gear) {
    return this->mShiftDownRPM[gear];
}

void EngineSpline::AutoShift() {
    if (this->mGear == G_REVERSE || this->mGearShiftTimer > 0.0f)
        return;

    float rpm = RPS2RPM(this->mAngularVelocity);
    if (this->mGear == G_NEUTRAL) {
        this->mGear = G_FIRST;
    } else if (this->mGearShiftTimer <= 0.0f) {
        float shift_up_rpm = this->GetShiftUpRPM(this->mGear);
        float shift_down_rpm = this->GetShiftDownRPM(this->mGear);

        float lower_gear_shift_up_rpm = 0.0f;
        float lower_gear_ratio = this->GetGearRatio(mGear - 1);
        if (mGear != G_FIRST && lower_gear_ratio > 0.0f) {
            lower_gear_shift_up_rpm = this->GetShiftUpRPM(mGear - 1);
            lower_gear_shift_up_rpm *= this->GetGearRatio(mGear) / lower_gear_ratio;
            lower_gear_shift_up_rpm -= 200.0f;
        }

        if (mGear <= G_FIRST) {
            shift_down_rpm = this->mThrottle < 0.2f ? this->mEngineInfo->IDLE() + 100.0f : 0.0f;
        }

        if (rpm >= shift_up_rpm && mGear < this->GetTopGear()) {
            int have_traction = 1;
            for (int i = 0; i < 4; ++i) {
                have_traction &= static_cast<int>(this->mSuspension->IsWheelOnGround(i) && this->mSuspension->GetWheelSlip(i) < 4.0f);
            }

            if (have_traction) {
                this->Shift((GearID)(this->mGear + 1));
            }
        } else if (rpm <= shift_down_rpm && this->mGear > G_NEUTRAL && rpm <= lower_gear_shift_up_rpm) {
            this->Shift((GearID)(this->mGear - 1));
        }
    }
}

bool EngineSpline::Shift(GearID gear) {
    if (gear < this->GetNumGearRatios() && gear != this->mGear && gear >= G_REVERSE) {
        if (gear < this->mGear) {
            this->mGearShiftTimer = this->GetShiftDelay(gear) * 0.25f;
        } else {
            this->mGearShiftTimer = this->GetShiftDelay(gear);
        }
        this->mGear = gear;
        return true;
    }

    return false;
}

float EngineSpline::GetDifferentialAngularVelocity() const {
    float into_gearbox = 0.0f;

    if (this->RearWheelDrive()) {
        float ave_rear = (this->mSuspension->GetWheelAngularVelocity(2) + this->mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        into_gearbox = ave_rear;
    }
    if (this->FrontWheelDrive()) {
        float ave_front = (this->mSuspension->GetWheelAngularVelocity(0) + this->mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        into_gearbox += ave_front;
        if (this->RearWheelDrive()) {
            into_gearbox *= 0.5f;
        }
    }

    return into_gearbox;
}

float EngineSpline::CalcSpeedometer(float rpm, unsigned int gear) const {
    return Physics::Info::Speedometer(this->mTranyInfo, this->mEngineInfo, this->mTireInfo, rpm, (GearID)gear, nullptr);
}

float EngineSpline::GetMaxSpeedometer() const {
    unsigned int num_ratios = this->GetNumGearRatios();
    if (num_ratios > 0) {
        float limiter = MPH2MPS(this->mEngineInfo.SPEED_LIMITER(0));
        float max_speedometer = this->CalcSpeedometer(this->mEngineInfo.RED_LINE(), num_ratios - 1);
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
    return this->CalcSpeedometer(RPS2RPM(mAngularVelocity), mGear);
}

extern float Engine_SmoothRPM(bool is_shifting, GearID gear, float dT, float old_rpm, float new_rpm, float engine_inertia);

void EngineSpline::OnTaskSimulate(float dT) {
    IInput *iinput = this->mIInput;
    if (iinput == nullptr || this->mSuspension == nullptr) {
        return;
    }

    if (this->mNeutralRev) {
        float rpm_diff = this->mEngineInfo.RED_LINE() - this->mEngineInfo.IDLE();

        if (this->mNeutralRevSpeed <= UMath::Epsilon) {
            this->mRPM = this->mNeutralRevRPM;
        } else if (this->mRPM < this->mNeutralRevRPM) {
            this->mRPM += this->mNeutralRevSpeed * rpm_diff * dT;
            this->mRPM = UMath::Min(mNeutralRevRPM, mRPM);
        } else {
            this->mRPM -= this->mNeutralRevSpeed * rpm_diff * dT;
            this->mRPM = UMath::Max(this->mNeutralRevRPM, this->mRPM);
        }

        this->mGear = G_NEUTRAL;
        this->mAngularVelocity = RPM2RPS(this->mRPM);
        this->mThrottle = UMath::Ramp(this->mRPM, this->mEngineInfo.IDLE(), this->mEngineInfo.RED_LINE());
    } else {
        float differential_w = this->GetDifferentialAngularVelocity();
        if (differential_w < -UMath::PI && this->GetGear() != G_REVERSE) {
            this->Shift(G_REVERSE);
        } else if (differential_w > UMath::PI && this->GetGear() <= G_NEUTRAL) {
            this->Shift(G_FIRST);
        } else {
            this->AutoShift();
        }

        if (this->mGearShiftTimer <= 0.0f) {
            this->mGearShiftTimer = 0.0f;
        } else {
            this->mGearShiftTimer -= dT;
            if (this->mGearShiftTimer < 0.0f) {
                this->mGearShiftTimer = 0.0f;
            }
        }

        float max_w = RPM2RPS(this->mEngineInfo->RED_LINE());
        float min_w = RPM2RPS(this->mEngineInfo->IDLE());
        float engine_inertia = Physics::Info::EngineInertia(this->mEngineInfo, this->mGear != G_NEUTRAL);
        float total_gear_ratio = this->GetGearRatio(this->mGear) * this->GetFinalGear();
        float gear_direction = this->mGear == G_REVERSE ? -1.0f : 1.0f; // unused, from debug info
        float power_range = (max_w - min_w) / max_w;
        float av = min_w + UMath::Abs(differential_w) * total_gear_ratio * power_range;
        mEngineBraking = av < this->mAngularVelocity;
        av = UMath::Clamp(av, min_w, max_w);
        this->mAngularVelocity = av;
        this->mRPM = Engine_SmoothRPM(this->IsShiftingGear(), this->GetGear(), dT, mRPM, RPS2RPM(this->mAngularVelocity), engine_inertia);
        this->mThrottle = UMath::Clamp((this->mAngularVelocity - min_w) / (max_w - min_w), 0.0f, 1.0f);
    }

    mIInput->SetControlGas(mThrottle);
}

Rpm EngineSpline::GetShiftPoint(GearID from_gear, GearID to_gear) const {
    if (from_gear <= G_REVERSE) {
        return 0.0f;
    }
    if (to_gear <= G_NEUTRAL) {
        return 0.0f;
    }
    if (to_gear > from_gear) {
        return this->mShiftUpRPM[from_gear];
    }
    if (to_gear < from_gear) {
        return this->mShiftDownRPM[from_gear];
    }

    return 0.0f;
}
