#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x124
class EngineTraffic : protected VehicleBehavior, protected ITransmission, protected IEngine, public IAttributeable {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey) override;

    // IEngine
    Hp GetMaxHorsePower() const override;
    Hp GetHorsePower() const override;
    Hp GetMinHorsePower() const override {
        return FTLB2HP(Physics::Info::Torque(this->mEngineInfo, this->mEngineInfo.IDLE()) * this->mEngineInfo.IDLE(), 1.0f);
    }
    float GetRPM() const override {
        return this->mRPM;
    }
    float GetMaxRPM() const override {
        return this->mEngineInfo.MAX_RPM();
    }
    Rpm GetPeakTorqueRPM() const override {
        return this->mPeakTorqueRPM;
    }
    float GetRedline() const override {
        return this->mEngineInfo.RED_LINE();
    }
    float GetMinRPM() const override {
        return this->mEngineInfo.IDLE();
    }
    void MatchSpeed(float speed) override;
    float GetNOSCapacity() const override {
        return 0.0f;
    }
    bool IsNOSEngaged() const override {
        return false;
    }
    bool HasNOS() const override {
        return false;
    }
    Lbs GetNOSFlowRate() const override {
        return 0.0f;
    }
    void ChargeNOS(float charge) override {}
    float GetNOSBoost() const override {
        return 1.0f;
    }

    virtual bool IsEngineBraking() {
        return this->mEngineBraking;
    }
    virtual bool IsShiftingGear() {
        return this->mGearShiftTimer > 0.0f;
    }
    bool IsReversing() const override {
        return this->mGear == G_REVERSE;
    }

    virtual Physics::Info::eInductionType InductionType() const {
        return Physics::Info::INDUCTION_NONE;
    }
    virtual Psi GetInductionPSI() const {
        return 0.0f;
    }
    virtual float InductionSpool() const {
        return 0.0f;
    }
    virtual Psi GetMaxInductionPSI() const {
        return 0.0f;
    }

    // ITransmission
    float GetDriveTorque() const override {
        return this->mDriveTorque;
    }
    GearID GetTopGear() const override {
        return (GearID)(this->GetNumGearRatios() - 1);
    }
    GearID GetGear() const override {
        return (GearID)this->mGear;
    }
    bool IsGearChanging() const override {
        return this->mGearShiftTimer > 0.0f;
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

    // Behavior
    void Reset() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  protected:
    EngineTraffic(const BehaviorParams &bp);
    ~EngineTraffic() override;
    void DoShifting(float dT);
    Nm GetTorquePoint(Rpm rpm) const;
    float GetBrakingTorque(float engine_torque, float rpm) const;

    unsigned int GetNumGearRatios() const {
        return this->mTranyInfo.Num_GEAR_RATIO();
    }
    float GetGearRatio(unsigned int idx) const {
        return this->mTranyInfo->GEAR_RATIO(idx);
    }
    float GetGearEfficiency(unsigned int idx) const {
        return this->mTranyInfo->GEAR_EFFICIENCY(idx);
    }
    float GetFinalGear() const {
        return this->mTranyInfo.FINAL_GEAR();
    }

    float GetShiftDelay(unsigned int gear) const {
        return this->mTranyInfo.SHIFT_SPEED() * this->GetGearRatio(gear);
    }

    float GetDifferentialAngularVelocity() const;

    bool RearWheelDrive() const {
        return this->mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }
    bool FrontWheelDrive() const {
        return this->mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

    void AutoShift();

    Rpm GetShiftUpRPM(int gear) const {
        return this->mShiftUpRPM[gear];
    }
    Rpm GetShiftDownRPM(int gear) const {
        return this->mShiftDownRPM[gear];
    }

    void CalcShiftPoints();

  private:
    float mDriveTorque;                                     // offset 0x64, size 0x4
    int mGear;                                              // offset 0x68, size 0x4
    float mGearShiftTimer;                                  // offset 0x6C, size 0x4
    float mThrottle;                                        // offset 0x70, size 0x4
    Rpm mShiftUpRPM[10];                                    // offset 0x74, size 0x28
    Rpm mShiftDownRPM[10];                                  // offset 0x9C, size 0x28
    Radians mAngularVelocity;                               // offset 0xC4, size 0x4
    Radians mClutchVelocity;                                // offset 0xC8, size 0x4
    bool mEngineBraking;                                    // offset 0xCC, size 0x1
    IInput *mIInput;                                        // offset 0xD0, size 0x4
    ISuspension *mSuspension;                               // offset 0xD4, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo;      // offset 0xD8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo; // offset 0xEC, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;         // offset 0x100, size 0x14
    float mRPM;                                             // offset 0x114, size 0x4
    Nm mPeakTorque;                                         // offset 0x118, size 0x4
    Rpm mPeakTorqueRPM;                                     // offset 0x11C, size 0x4
    Hp mMaxHP;                                              // offset 0x120, size 0x4
};

BIND_BEHAVIOR_FACTORY(EngineTraffic);

Behavior *EngineTraffic::Construct(const BehaviorParams &params) {
    return new EngineTraffic(params);
}

EngineTraffic::EngineTraffic(const BehaviorParams &bp)
    : VehicleBehavior(bp, 0),   //
      ITransmission(bp.fowner), //
      IEngine(bp.fowner),       //
      mDriveTorque(0.0f),       //
      mGear(G_NEUTRAL),         //
      mGearShiftTimer(0.0f),    //
      mThrottle(0.0f),          //
      mAngularVelocity(0.0f),   //
      mClutchVelocity(0.0f),    //
      mEngineBraking(false),    //
      mIInput(nullptr),         //
      mSuspension(nullptr),     //
      mEngineInfo(this, 0),     //
      mTranyInfo(this, 0),      //
      mTireInfo(this, 0),       //
      mRPM(0.0f),               //
      mPeakTorque(0.0f),        //
      mPeakTorqueRPM(0.0f),     //
      mMaxHP(0.0f)              //
{
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    this->EnableProfile("EngineTraffic");

    this->GetOwner()->QueryInterface(&this->mIInput);
    this->GetOwner()->QueryInterface(&this->mSuspension);
    this->Reset();
    this->CalcShiftPoints();
}

EngineTraffic::~EngineTraffic() {
    IAttributeable::UnRegister(this);
}

Hp EngineTraffic::GetMaxHorsePower() const {
    return this->mMaxHP;
}

Hp EngineTraffic::GetHorsePower() const {
    float engine_torque = GetTorquePoint(this->mRPM);
    return NM2HP(engine_torque * mThrottle, this->mRPM);
}

void EngineTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
}

void EngineTraffic::OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey) {}

void EngineTraffic::Reset() {
    this->mDriveTorque = 0.0f;
    this->mAngularVelocity = RPM2RPS(this->mEngineInfo.IDLE());
    this->mRPM = mEngineInfo.IDLE();
    this->mClutchVelocity = 0.0f;
    this->mGearShiftTimer = 0.0f;
    this->mGear = G_FIRST;
    this->mThrottle = 0.0f;

    this->CalcShiftPoints();
}

float EngineTraffic::GetTorquePoint(Rpm rpm) const {
    float ftlbs = Physics::Info::Torque(this->mEngineInfo, rpm);
    return FTLB2NM(ftlbs);
}

void EngineTraffic::MatchSpeed(float speed) {
    float wheelrear = Physics::Info::WheelDiameter(this->mTireInfo, false) * 0.5f;
    float wheelfront = Physics::Info::WheelDiameter(this->mTireInfo, true) * 0.5f;
    float avg_wheel_radius = (wheelrear + wheelfront) * 0.5f;

    if (avg_wheel_radius <= 0.0f) {
        return;
    }

    float differential_w = speed / avg_wheel_radius;
    float max_w = RPM2RPS(this->mEngineInfo->RED_LINE());
    float min_w = RPM2RPS(this->mEngineInfo->IDLE());
    float rear_end = this->GetFinalGear();

    if (speed == 0.0f) {
        this->mGearShiftTimer = 0.0f;
        this->mGear = G_FIRST;
        this->mAngularVelocity = RPM2RPS(this->mEngineInfo->IDLE());
        this->mClutchVelocity = this->mAngularVelocity;
        return;
    }

    if (speed < 0.0f) {
        this->mGearShiftTimer = 0.0f;
        this->mGear = G_REVERSE;
    } else {
        this->mGear = G_FIRST;
        for (unsigned int gear = G_FIRST; gear < this->GetNumGearRatios(); ++gear) {
            float gear_ratio = this->GetGearRatio(gear) * rear_end;
            if (gear_ratio > 0.0f) {
                float gear_topspeed = avg_wheel_radius * max_w / gear_ratio;
                if (gear_topspeed < speed) {
                    mGear = gear;
                }
            }
        }
    }

    float total_gear_ratio = GetGearRatio(this->mGear) * rear_end;
    float power_range = (max_w - min_w) / max_w;
    this->mAngularVelocity = min_w + differential_w * total_gear_ratio * power_range;
    this->mClutchVelocity = this->mAngularVelocity;
    this->mRPM = RPS2RPM(this->mAngularVelocity);
}

float EngineTraffic::GetBrakingTorque(float engine_torque, float rpm) const {
    unsigned int numpts = this->mEngineInfo.Num_ENGINE_BRAKING();
    if (numpts > 1) {
        float rpm_min = this->mEngineInfo.IDLE();
        float rpm_max = this->mEngineInfo.MAX_RPM();
        float ratio;
        unsigned int index =
            UTIL_InterprolateIndex(numpts - 1, UMath::Clamp(rpm, this->mEngineInfo.IDLE(), this->mEngineInfo.RED_LINE()), rpm_min, rpm_max, ratio);

        float base = this->mEngineInfo.ENGINE_BRAKING(index);
        unsigned int secondIndex = index + 1;
        float step = this->mEngineInfo.ENGINE_BRAKING(UMath::Min(numpts - 1, secondIndex));
        float load_pct = (step - base) * ratio + base;
        return -engine_torque * UMath::Clamp(load_pct, 0.f, 1.f);
    } else {
        return -engine_torque * this->mEngineInfo.ENGINE_BRAKING(0);
    }
}

void EngineTraffic::CalcShiftPoints() {
    bool shift_points_calced =
        Physics::Info::ShiftPoints(this->mTranyInfo, this->mEngineInfo, Attrib::Gen::induction((Attrib::Collection *)nullptr, 0, nullptr),
                                   this->mShiftUpRPM, this->mShiftDownRPM, 10);

    Attrib::Gen::pvehicle pvehicle(GetOwner()->GetAttributes());
    this->mPeakTorque = Physics::Info::MaxInductedTorque(pvehicle, this->mPeakTorqueRPM, nullptr);
    this->mPeakTorque = FTLB2NM(this->mPeakTorque);
    this->mMaxHP = Physics::Info::MaxInductedPower(pvehicle, nullptr);
}

void EngineTraffic::AutoShift() {
    if (this->mGear == G_REVERSE || this->mGearShiftTimer > 0.0f)
        return;

    float rpm = RPS2RPM(this->mAngularVelocity);
    if (this->mGear == G_NEUTRAL) {
        this->mGear = G_FIRST;
    } else if (this->mGearShiftTimer <= 0.0f) {
        float shift_up_rpm = this->GetShiftUpRPM(this->mGear);
        float shift_down_rpm = this->GetShiftDownRPM(this->mGear);

        float lower_gear_shift_up_rpm = 0.0f;
        float lower_gear_ratio = this->GetGearRatio(this->mGear - 1);
        if (this->mGear != G_FIRST && lower_gear_ratio > 0.0f) {
            lower_gear_shift_up_rpm = this->GetShiftUpRPM(this->mGear - 1);
            lower_gear_shift_up_rpm *= this->GetGearRatio(this->mGear) / lower_gear_ratio;
            lower_gear_shift_up_rpm -= 200.0f;
        }

        if (this->mGear <= G_FIRST) {
            shift_down_rpm = this->mThrottle < 0.2f ? this->mEngineInfo->IDLE() + 100.0f : 0.0f;
        }

        if (rpm >= shift_up_rpm && this->mGear < this->GetTopGear()) {
            this->Shift((GearID)(this->mGear + 1));
        } else if (rpm <= shift_down_rpm && this->mGear > G_NEUTRAL && rpm <= lower_gear_shift_up_rpm) {
            this->Shift((GearID)(this->mGear - 1));
        }
    }
}

bool EngineTraffic::Shift(GearID gear) {
    if (gear < this->GetNumGearRatios() && gear != this->mGear && gear >= G_REVERSE) {
        if (gear < mGear) {
            this->mGearShiftTimer = this->GetShiftDelay(gear) * 0.25f;
        } else {
            this->mGearShiftTimer = this->GetShiftDelay(gear);
        }
        this->mGear = gear;
        return true;
    }

    return false;
}

float EngineTraffic::GetDifferentialAngularVelocity() const {
    float into_gearbox = 0.0f;
    bool in_reverse = this->GetGear() == G_REVERSE;

    if (this->FrontWheelDrive()) {
        float w_vel = (this->mSuspension->GetWheelAngularVelocity(0) + this->mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        into_gearbox = in_reverse ? UMath::Min(w_vel, 0.0f) : UMath::Max(w_vel, 0.0f);
    }

    if (this->RearWheelDrive()) {
        float w_vel = (this->mSuspension->GetWheelAngularVelocity(2) + this->mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        into_gearbox = in_reverse ? UMath::Min(w_vel, into_gearbox) : UMath::Max(w_vel, into_gearbox);
    }

    return into_gearbox;
}

float EngineTraffic::GetMaxSpeedometer() const {
    return Physics::Info::Speedometer(this->mTranyInfo, this->mEngineInfo, this->mTireInfo, this->GetRedline(), this->GetTopGear(), nullptr);
}

float EngineTraffic::GetSpeedometer() const {
    return UMath::Abs(this->GetVehicle()->GetLocalVelocity().z);
}

void EngineTraffic::DoShifting(float dT) {
    if (this->mGear > G_REVERSE) {
        this->AutoShift();
    }

    if (this->mGearShiftTimer > 0.0f) {
        this->mGearShiftTimer -= dT;
        if (this->mGearShiftTimer <= 0.0f) {
            this->mGearShiftTimer = 0.0f;
        }
    }
}

void EngineTraffic::OnTaskSimulate(float dT) {
    IInput *iinput = this->mIInput;
    if (iinput == nullptr || this->mSuspension == nullptr) {
        return;
    }

    if (this->mSuspension->GetNumWheels() != 4) {
        return;
    }

    this->mThrottle = this->mIInput->GetControls().fGas;
    float steering = iinput->GetControls().fSteering; // unused, from debug info
    this->DoShifting(dT);
    float max_w = RPM2RPS(this->mEngineInfo->RED_LINE());
    float min_w = RPM2RPS(this->mEngineInfo->IDLE());
    float target_w = max_w * this->mThrottle;
    float engine_inertia = Physics::Info::EngineInertia(this->mEngineInfo, true);
    float differential_w = this->GetDifferentialAngularVelocity();
    float clutch_ratio = 1.0f;
    if (mGearShiftTimer > 0.0f) {
        clutch_ratio = 0.0f;
    }
    float torque_total = 0.0f;
    float gear_direction = this->mGear == G_REVERSE ? -1.0f : 1.0f;
    float total_gear_ratio = this->GetGearRatio(this->mGear) * this->GetFinalGear() * gear_direction;

    float converter_ratio = 1.0f;
    float torque_converter = this->mTranyInfo.TORQUE_CONVERTER();
    if (torque_converter > 0.0f) {
        float rpm = RPS2RPM(this->mAngularVelocity);
        converter_ratio =
            1.0f + torque_converter * clutch_ratio * this->mThrottle * (1.0f - UMath::Ramp(rpm, this->mEngineInfo.IDLE(), this->mPeakTorqueRPM));
        total_gear_ratio *= converter_ratio;
    }

    UVector3 vUp;
    this->GetOwner()->GetRigidBody()->GetUpVector(vUp);
    bool collision_disengage = vUp.y < 0.3f;
    this->mEngineBraking = false;
    if (this->mGear == G_NEUTRAL || collision_disengage || total_gear_ratio == 0.0f) {
        float rpm = RPS2RPM(this->mAngularVelocity);
        float engine_torque = this->GetTorquePoint(rpm);
        float braking_torque = this->GetBrakingTorque(engine_torque, rpm);
        float neutral_torque = (engine_torque * this->mThrottle + braking_torque * (1.0f - this->mThrottle)) / engine_inertia;

        float dW = 4.0f * dT;
        this->mAngularVelocity += neutral_torque * dW;
        if (neutral_torque > 0.0f) {
            this->mAngularVelocity = UMath::Min(this->mAngularVelocity, target_w);
        } else {
            this->mAngularVelocity = UMath::Max(this->mAngularVelocity, target_w);
        }

        this->mDriveTorque = 0.0f;
        this->mClutchVelocity = min_w;
        clutch_ratio = 0.0f;
    } else {
        float rpm = RPS2RPM(this->mAngularVelocity);
        float counter_torque;                            // unused, from debug info
        float engine_torque = this->GetTorquePoint(rpm); // unused, from debug info
        float drive_torque = engine_torque;
        float breaking_torque = this->GetBrakingTorque(drive_torque, rpm); // yes it's misspelled here despite being correct above
        if (differential_w * total_gear_ratio < 0.0f) {
            breaking_torque = -breaking_torque;
        }
        if (differential_w == 0.0f) {
            breaking_torque = 0.0;
        }

        float power_range = (max_w - min_w) / max_w;
        float transmission_velocity = differential_w * total_gear_ratio * power_range + min_w;
        mClutchVelocity = transmission_velocity;
        torque_total += drive_torque * this->mThrottle + breaking_torque * (1.0f - this->mThrottle);

        if (transmission_velocity > max_w) {
            torque_total = breaking_torque * (transmission_velocity / max_w - 1.0f) + breaking_torque * (1.0f - this->mThrottle);
        }

        if (transmission_velocity > target_w) {
            float d = transmission_velocity - this->mAngularVelocity;
            // debug info says UMath::Abs got inlined but it was optimized away
            // A124 has fewer optmizations enabled and gives us this
            float dW = UMath::Abs(0.0f) / engine_inertia * dT;
            this->mAngularVelocity += UMath::Min(dW, d);
            this->mEngineBraking = true;
        } else if (transmission_velocity == target_w) {
            torque_total = 0.0f;
        } else if (transmission_velocity < target_w) {
            float engine_acceleration; // unused, from debug info
            float dW = dT * torque_total / engine_inertia;
            float clutch_w = transmission_velocity; // unused, from debug info
            float delta_w = this->mAngularVelocity - clutch_w;
            if (delta_w > dW) {
                this->mAngularVelocity -= dW * 4.0f;
            } else if (delta_w < -dW) {
                this->mAngularVelocity += dW;
            } else {
                this->mAngularVelocity = transmission_velocity;
            }
            this->mAngularVelocity = UMath::Min(this->mAngularVelocity, target_w);
        }
    }

    this->mAngularVelocity = UMath::Clamp(this->mAngularVelocity, min_w, max_w);
    this->mDriveTorque = torque_total * total_gear_ratio * this->GetGearEfficiency(this->mGear) * clutch_ratio;
    this->mRPM = RPS2RPM(this->mAngularVelocity);
}

float EngineTraffic::GetShiftPoint(GearID from_gear, GearID to_gear) const {
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
