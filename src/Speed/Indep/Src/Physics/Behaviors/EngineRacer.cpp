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
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IInductable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITiptronic.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static const bool Tweak_EnableTorqueConverter = true;
static const float Tweak_ClutchEngageTime = 0.25f;
static const float Tweak_1stGearClutchEngageTime = 0.05f;
static const float Tweak_CheaterTorqueBoost = 0.5f;

bool Tweak_InfiniteNOS = false;

struct Clutch {
  public:
    enum State {
        ENGAGED = 0,
        ENGAGING = 1,
        DISENGAGED = 2,
    };

    Clutch() {
        this->mState = ENGAGED;
        this->mTime = 0.0f;
        this->mEngageTime = 0.0f;
    }

    void Disengage() {
        if (this->mState == ENGAGED) {
            this->mState = DISENGAGED;
        }
    }

    void Engage(float time) {
        if (this->mState == DISENGAGED) {
            if (time > 0.0f) {
                this->mState = ENGAGING;
            } else {
                this->mState = ENGAGED;
            }
            this->mTime = time;
            this->mEngageTime = time;
        }
    }

    void Reset() {
        this->mState = ENGAGED;
        this->mTime = 0.0f;
    }

    float Update(float dT) {
        if (this->mTime > 0.0f) {
            this->mTime -= dT;
            if (this->mTime <= 0.0f && this->mState == ENGAGING) {
                this->mState = ENGAGED;
            }
        }

        switch (this->mState) {
            case DISENGAGED:
                return 0.25f;
            case ENGAGING:
                return 1.0f - UMath::Ramp(this->mTime, 0.0f, this->mEngageTime) * 0.75f;
            case ENGAGED:
                return 1.0f;
            default:
                return 1.0f;
        }
    }

    State GetState() {
        return this->mState;
    }

  private:
    State mState;
    Seconds mTime;
    Seconds mEngageTime;
};

// static const float Tweak_PerfectLaunchMaxRPM; // TODO value and use
// static const float Tweak_PerfectLaunchRatio; // TODO value and use

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
    static Behavior *Construct(const BehaviorParams &params);

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) override;

    // IRaceEngine
    // Credits: Brawltendo
    Rpm GetPerfectLaunchRange(float &range) override {
        // perfect launch only applies to first gear
        if (this->mGear != G_FIRST) {
            range = 0.0f;
            return 0.0f;
        } else {
            range = (this->mEngineInfo.RED_LINE() - this->mEngineInfo.IDLE()) * 0.25f;
            float upper_limit = this->mEngineInfo.RED_LINE() + -500.0f;
            return UMath::Min(this->mPeakTorqueRPM + range, upper_limit) - range;
        }
    }

    // IEngine
    Hp GetMaxHorsePower() const override {
        return this->mMaxHP;
    }
    Hp GetMinHorsePower() const override {
        return FTLB2HP(Physics::Info::Torque(this->mEngineInfo, this->mEngineInfo.IDLE()) * this->mEngineInfo.IDLE(), 1.0f);
    }
    Hp GetHorsePower() const override;
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
        return this->mNOSCapacity;
    }

    float GetNOSBoost() const override {
        return this->mNOSBoost;
    }
    bool IsNOSEngaged() const override {
        return this->mNOSEngaged >= 1.0f;
    }
    bool HasNOS() const override {
        return this->mNOSInfo.NOS_CAPACITY() > 0.0f && this->mNOSInfo.TORQUE_BOOST() > 0.0f;
    }
    float GetNOSFlowRate() const override {
        return this->mNOSInfo.FLOW_RATE();
    }
    void ChargeNOS(float charge) override {
        if (this->HasNOS()) {
            this->mNOSCapacity = UMath::Clamp(this->mNOSCapacity + charge, 0.0f, 1.0f);
        }
    }

    virtual bool IsEngineBraking() {
        return this->mEngineBraking;
    }
    virtual bool IsShiftingGear() {
        return this->mGearShiftTimer > 0.0f;
    }

    // ITransmission
    bool IsReversing() const override {
        return this->mGear == G_REVERSE;
    }

    // IInductable
    Physics::Info::eInductionType InductionType() const override {
        return Physics::Info::InductionType(this->mInductionInfo);
    }
    Psi GetInductionPSI() const override {
        return this->mPSI;
    }
    float InductionSpool() const override {
        return this->mSpool;
    }
    Psi GetMaxInductionPSI() const override {
        return this->mInductionInfo.PSI();
    }

    // IEngineDamage
    bool IsBlown() const override {
        return this->mBlown;
    }
    bool Blow() override;
    void Sabotage(float time) override;
    void Repair() override {
        this->mSabotage = 0.0f;
        this->mBlown = false;
    }
    bool IsSabotaged() const override {
        return this->mSabotage > 0.0f;
    }

    // ITiptronic
    bool SportShift(GearID gear) override;

    // ITransmission
    float GetDriveTorque() const override {
        return this->mDriveTorque;
    }
    GearID GetTopGear() const override {
        return (GearID)(this->GetNumGearRatios() - 1);
    }
    GearID GetGear() const override {
        return (GearID)mGear;
    }
    bool IsGearChanging() const override {
        return this->mGearShiftTimer > 0.0f;
    }
    bool Shift(GearID gear) override {
        return this->DoGearChange(gear, false);
    }
    float GetSpeedometer() const override;
    float GetMaxSpeedometer() const override;
    float GetShiftPoint(GearID from_gear, GearID to_gear) const override;

    ShiftStatus GetShiftStatus() const override {
        return this->mShiftStatus;
    }
    ShiftPotential GetShiftPotential() const override {
        return this->mShiftPotential;
    }

    // Behavior
    void Reset() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  protected:
    EngineRacer(const BehaviorParams &bp);
    ~EngineRacer() override;

    virtual ShiftStatus OnGearChange(GearID gear);
    virtual bool UseRevLimiter() const {
        return true;
    }
    virtual void DoECU();
    virtual float DoThrottle();
    virtual void DoInduction(const Physics::Tunings *tunings, float dT);
    virtual float DoNos(const Physics::Tunings *tunings, float dT, bool engaged);
    virtual void DoShifting(float dT);
    virtual ShiftPotential UpdateShiftPotential(GearID gear, Rpm rpm);
    virtual Nm GetEngineTorque(Rpm rpm) const;

    Nm GetBrakingTorque(float engine_torque, float rpm) const;

    unsigned int GetNumGearRatios() const {
        return this->mTranyInfo.Num_GEAR_RATIO();
    }

    float GetGearRatio(unsigned int idx) const {
        return this->mTranyInfo.GEAR_RATIO(idx);
    }

    float GetGearEfficiency(unsigned int idx) const {
        return this->mTranyInfo.GEAR_EFFICIENCY(idx);
    }

    float GetFinalGear() const {
        return this->mTranyInfo.FINAL_GEAR();
    }

    float GetRatioChange(unsigned int from, unsigned int to) const {
        float ratio1 = this->mTranyInfo.GEAR_RATIO(from);
        float ratio2 = this->mTranyInfo.GEAR_RATIO(to);

        if (ratio1 > 0.0f && ratio2 > UMath::Epsilon) {
            return ratio1 / ratio2;
        } else {
            return 0.0f;
        }
    }

    float GetShiftDelay(unsigned int gear) const {
        return this->mTranyInfo.SHIFT_SPEED() * this->GetGearRatio(gear);
    }

    float GetDifferentialAngularVelocity(bool locked) const;
    float GetDriveWheelSlippage() const;
    void SetDifferentialAngularVelocity(float w);
    void LimitFreeWheels(float w);

    bool RearWheelDrive() const {
        return this->mTranyInfo.TORQUE_SPLIT() < 1.0f;
    }

    bool FrontWheelDrive() const {
        return this->mTranyInfo.TORQUE_SPLIT() > 0.0f;
    }

    void AutoShift();
    float CalcSpeedometer(float rpm, unsigned int gear) const;

    float GetShiftUpRPM(int gear) const {
        return this->mShiftUpRPM[gear];
    }

    float GetShiftDownRPM(int gear) const {
        return this->mShiftDownRPM[gear];
    }

    const Attrib::Gen::transmission &GetTransmissionData() const {
        return this->mTranyInfo;
    }

    void CalcShiftPoints();

  private:
    GearID GuessGear(Mps speed) const;
    float GuessRPM(Mps speed, GearID atgear) const;
    bool DoGearChange(GearID gear, bool automatic);
    ShiftPotential FindShiftPotential(GearID gear, Rpm rpm) const;

    float mDriveTorque;                                      // offset 0x84, size 0x4
    int mGear;                                               // offset 0x88, size 0x4
    float mGearShiftTimer;                                   // offset 0x8C, size 0x4
    float mThrottle;                                         // offset 0x90, size 0x4
    float mSpool;                                            // offset 0x94, size 0x4
    Psi mPSI;                                                // offset 0x98, size 0x4
    float mInductionBoost;                                   // offset 0x9C, size 0x4
    Rpm mShiftUpRPM[10];                                     // offset 0xA0, size 0x28
    Rpm mShiftDownRPM[10];                                   // offset 0xC8, size 0x28
    Radians mAngularVelocity;                                // offset 0xF0, size 0x4
    Radians mAngularAcceleration;                            // offset 0xF4, size 0x4
    Radians mTransmissionVelocity;                           // offset 0xF8, size 0x4
    float mNOSCapacity;                                      // offset 0xFC, size 0x4
    float mNOSBoost;                                         // offset 0x100, size 0x4
    float mNOSEngaged;                                       // offset 0x104, size 0x4
    float mClutchRPMDiff;                                    // offset 0x108, size 0x4
    bool mEngineBraking;                                     // offset 0x10C, size 0x1
    Seconds mSportShifting;                                  // offset 0x110, size 0x4
    IInput *mIInput;                                         // offset 0x114, size 0x4
    ISuspension *mSuspension;                                // offset 0x118, size 0x4
    ICheater *mCheater;                                      // offset 0x11C, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::nos> mNOSInfo;             // offset 0x120, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::induction> mInductionInfo; // offset 0x134, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo;       // offset 0x148, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mTranyInfo;  // offset 0x15C, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;          // offset 0x170, size 0x14
    Rpm mRPM;                                                // offset 0x184, size 0x4
    ShiftStatus mShiftStatus;                                // offset 0x188, size 0x4
    ShiftPotential mShiftPotential;                          // offset 0x18C, size 0x4
    Nm mPeakTorque;                                          // offset 0x190, size 0x4
    Rpm mPeakTorqueRPM;                                      // offset 0x194, size 0x4
    Hp mMaxHP;                                               // offset 0x198, size 0x4
    Clutch mClutch;                                          // offset 0x19C, size 0xC
    bool mBlown;                                             // offset 0x1A8, size 0x1
    Seconds mSabotage;                                       // offset 0x1AC, size 0x4
};

BIND_BEHAVIOR_FACTORY(EngineRacer);

Behavior *EngineRacer::Construct(const BehaviorParams &params) {
    return new EngineRacer(params);
}

EngineRacer::EngineRacer(const BehaviorParams &bp)
    : VehicleBehavior(bp, 0),                //
      ITransmission(bp.fowner),              //
      IEngine(bp.fowner),                    //
      IInductable(bp.fowner),                //
      ITiptronic(bp.fowner),                 //
      IRaceEngine(bp.fowner),                //
      IEngineDamage(bp.fowner),              //
      mDriveTorque(0.0f),                    //
      mGear(G_NEUTRAL),                      //
      mGearShiftTimer(0.0f),                 //
      mThrottle(0.0f),                       //
      mSpool(0.0f),                          //
      mPSI(0.0f),                            //
      mInductionBoost(0.0f),                 //
      mAngularVelocity(0.0f),                //
      mAngularAcceleration(0.0f),            //
      mTransmissionVelocity(0.0f),           //
      mNOSCapacity(1.0f),                    //
      mNOSBoost(1.0f),                       //
      mNOSEngaged(0.0f),                     //
      mClutchRPMDiff(0.0f),                  //
      mEngineBraking(false),                 //
      mSportShifting(0.0f),                  //
      mIInput(nullptr),                      //
      mSuspension(nullptr),                  //
      mNOSInfo(this, 0),                     //
      mInductionInfo(this, 0),               //
      mEngineInfo(this, 0),                  //
      mTranyInfo(this, 0),                   //
      mTireInfo(this, 0),                    //
      mRPM(0.0f),                            //
      mShiftStatus(SHIFT_STATUS_NONE),       //
      mShiftPotential(SHIFT_POTENTIAL_NONE), //
      mPeakTorque(0.0f),                     //
      mPeakTorqueRPM(0.0f),                  //
      mMaxHP(0.0f),                          //
      mClutch(),                             //
      mBlown(false),                         //
      mSabotage(0.0f) {
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    this->EnableProfile("EngineRacer");

    this->GetOwner()->QueryInterface(&mIInput);
    this->GetOwner()->QueryInterface(&mSuspension);
    this->GetOwner()->QueryInterface(&mCheater);
    this->Reset();

    if (this->mNOSInfo.NOS_CAPACITY() > 0.0f) {
        this->mNOSCapacity = 1.0f;
    } else {
        this->mNOSCapacity = 0.0f;
    }

    this->CalcShiftPoints();
}

EngineRacer::~EngineRacer() {
    IAttributeable::UnRegister(this);
}

float EngineRacer::GetHorsePower() const {
    float engine_torque = this->GetEngineTorque(this->mRPM);
    return NM2HP(engine_torque * this->mThrottle, this->mRPM);
}

void EngineRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mCheater);
    }
}

void EngineRacer::Sabotage(float time) {
    if ((this->mSabotage <= 0.0f) && (time > UMath::Epsilon) && !this->IsBlown()) {
        this->mSabotage = Sim::GetTime() + time;
    }
}

bool EngineRacer::Blow() {
    if (!this->mBlown) {
        this->mBlown = true;
        this->mSabotage = 0.0f;
        new EEngineBlown(this->GetOwner()->GetInstanceHandle());
        return true;
    }
    return false;
}

void EngineRacer::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

void EngineRacer::Reset() {
    this->mDriveTorque = 0.0f;
    this->mAngularVelocity = RPM2RPS(this->mEngineInfo.IDLE());
    this->mAngularAcceleration = 0.0f;
    this->mRPM = this->mEngineInfo.IDLE();
    this->mTransmissionVelocity = 0.0f;
    this->mClutch.Reset();
    this->mGearShiftTimer = 0.0f;
    this->mSpool = 0.0f;
    this->mPSI = 0.0f;
    this->mInductionBoost = 0.0f;
    this->mShiftStatus = SHIFT_STATUS_NONE;
    this->mShiftPotential = SHIFT_POTENTIAL_NONE;
    this->mGear = G_FIRST;
    this->mNOSEngaged = 0.0f;
    this->mClutchRPMDiff = 0.0f;
    this->mThrottle = 0.0f;
    this->mNOSBoost = 1.0f;
    this->mSportShifting = 0.0f;

    this->CalcShiftPoints();
}

float EngineRacer::GetEngineTorque(Rpm rpm) const {
    float ftlbs = Physics::Info::Torque(this->mEngineInfo, rpm);
    float result = FTLB2NM(ftlbs);
    result *= 1.0f + this->mInductionBoost;

    if (this->GetVehicle()->GetDriverClass() != DRIVER_REMOTE) {
        result *= this->mNOSBoost;
    }

    return result;
}

GearID EngineRacer::GuessGear(Mps speed) const {
    if (speed < 0.0f) {
        return G_REVERSE;
    }

    GearID result = G_FIRST;
    for (int i = G_FIRST; i < GetTopGear(); ++i) {
        GearID this_gear = (GearID)i;
        GearID next_gear = (GearID)(i + 1);

        float shift_at = this->GetShiftPoint(this_gear, next_gear);
        float this_rpm = this->GuessRPM(speed, this_gear);
        if (this_rpm <= shift_at) {
            break;
        }
        result = next_gear;
    }

    return result;
}

float EngineRacer::GuessRPM(Mps speed, GearID atgear) const {
    float wheelrear = Physics::Info::WheelDiameter(mTireInfo, false) * 0.5f;
    float wheelfront = Physics::Info::WheelDiameter(mTireInfo, true) * 0.5f;
    float avg_wheel_radius = (wheelrear + wheelfront) * 0.5f;

    if (avg_wheel_radius <= 0.0f) {
        return this->mEngineInfo.IDLE();
    }

    float differential_w = UMath::Abs(speed) / avg_wheel_radius;
    float max_w = RPM2RPS(this->mEngineInfo.RED_LINE());
    float min_w = RPM2RPS(this->mEngineInfo.IDLE());
    float rear_end = this->GetFinalGear();
    float total_gear_ratio = this->GetFinalGear() * this->GetGearRatio(atgear);
    float av = RPS2RPM(min_w + differential_w * total_gear_ratio * (max_w - min_w) / max_w);
    av = UMath::Clamp(av, this->mEngineInfo.IDLE(), this->mEngineInfo.RED_LINE());
    return av;
}

void EngineRacer::MatchSpeed(float speed) {
    this->Reset();
    this->mAngularAcceleration = 0.0f;
    this->mSportShifting = 0.0f;
    this->mGearShiftTimer = 0.0f;
    this->mGear = this->GuessGear(speed);
    this->mRPM = this->GuessRPM(speed, (GearID)this->mGear);
    this->mAngularVelocity = RPM2RPS(mRPM);
    this->mTransmissionVelocity = this->mAngularVelocity;
    this->mClutch.Engage(0.0f);
}

// Credits: Brawltendo
float EngineRacer::GetBrakingTorque(float engine_torque, float rpm) const {
    float torque = engine_torque;
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
        float load_pct = base + (step - base) * ratio;
        return -torque * UMath::Clamp(load_pct, 0.f, 1.f);
    } else {
        return -torque * this->mEngineInfo.ENGINE_BRAKING(0);
    }
}

void EngineRacer::CalcShiftPoints() {
    bool shift_points_calced =
        Physics::Info::ShiftPoints(this->mTranyInfo, this->mEngineInfo, this->mInductionInfo, this->mShiftUpRPM, this->mShiftDownRPM, 10);
    Attrib::Gen::pvehicle pvehicle(this->GetOwner()->GetAttributes());

    mPeakTorque = Physics::Info::MaxInductedTorque(pvehicle, this->mPeakTorqueRPM, nullptr);
    mPeakTorque = FTLB2NM(mPeakTorque);
    mMaxHP = Physics::Info::MaxInductedPower(pvehicle, nullptr);
}

static const bool Tweak_SkipDownShiftGears = true; // TODO USE

// Credits: Brawltendo
void EngineRacer::AutoShift() {
    if (this->mGear == G_REVERSE || this->mGearShiftTimer > 0.0f || this->GetVehicle()->IsStaging() || this->mSportShifting > 0.0f)
        return;

    // skip neutral when using auto transmission
    if (this->mGear == G_NEUTRAL) {
        this->mGear = G_FIRST;
        return;
    }

    switch (this->mShiftPotential) {
        case SHIFT_POTENTIAL_DOWN: {
            int next_gear = mGear - 1;
            if (next_gear > G_FIRST) {
                float current_rpm = RPS2RPM(this->mTransmissionVelocity);
                float rpm = current_rpm * this->GetRatioChange(next_gear, mGear);
                for (; next_gear > G_FIRST && this->FindShiftPotential((GearID)next_gear, rpm) == SHIFT_POTENTIAL_DOWN;) {
                    rpm = current_rpm * this->GetRatioChange(--next_gear, mGear);
                }
            }
            this->DoGearChange((GearID)next_gear, true);
            break;
        }
        case SHIFT_POTENTIAL_NONE:
            break;
        case SHIFT_POTENTIAL_UP:
        case SHIFT_POTENTIAL_PERFECT:
        case SHIFT_POTENTIAL_MISS: {
            int have_traction = 1;
            for (int i = 0; i < 4; ++i) {
                have_traction &= static_cast<int>(this->mSuspension->IsWheelOnGround(i) && this->mSuspension->GetWheelSlip(i) < 4.f);
            }
            if (have_traction) {
                this->DoGearChange((GearID)(this->mGear + 1), true);
            }
            break;
        }

        default:
            break;
    }
}

static const bool Tweak_CoastShifting = true;
static const float Tweak_CoastingPercent = 0.65f;

// Credits: Brawltendo
ShiftPotential EngineRacer::FindShiftPotential(GearID gear, Rpm rpm) const {
    if (gear <= G_NEUTRAL)
        return SHIFT_POTENTIAL_NONE;

    float shift_up_rpm = this->GetShiftUpRPM(gear);
    float shift_down_rpm = this->GetShiftDownRPM(gear);
    float lower_gear_ratio = this->GetGearRatio(gear - 1);

    // is able to shift down
    if (gear != G_FIRST && lower_gear_ratio > 0.0f) {
        float lower_gear_shift_up_rpm = ((this->GetShiftUpRPM(gear - 1) * this->GetGearRatio(gear)) / lower_gear_ratio) - 200.0f;

        if (Tweak_CoastShifting) {
            // lower downshift RPM when coasting
            float off_throttle_rpm = UMath::Lerp(this->mEngineInfo.IDLE(), shift_down_rpm, Tweak_CoastingPercent);
            shift_down_rpm = UMath::Lerp(off_throttle_rpm, shift_down_rpm, this->mThrottle);
            shift_down_rpm = UMath::Min(shift_down_rpm, lower_gear_shift_up_rpm);
        }
    }

    if (rpm >= shift_up_rpm && gear < GetTopGear()) {
        return SHIFT_POTENTIAL_UP;
    }
    if (rpm <= shift_down_rpm && gear > G_FIRST) {
        return SHIFT_POTENTIAL_DOWN;
    }
    return SHIFT_POTENTIAL_NONE;
}

ShiftPotential EngineRacer::UpdateShiftPotential(GearID gear, Rpm rpm) {
    return FindShiftPotential(gear, rpm);
}

static const bool Tweak_UseSportShift = true; // TODO use
// static const float Tweak_SportShiftTime; // TODO value and use

bool EngineRacer::SportShift(GearID gear) {
    if (gear == this->mGear || gear <= G_NEUTRAL || this->IsGearChanging())
        return false;

    ShiftPotential potential = this->GetShiftPotential();
    if (gear > this->mGear && potential == SHIFT_POTENTIAL_DOWN)
        return false;

    if (gear < this->mGear && potential == SHIFT_POTENTIAL_UP)
        return false;

    if (this->DoGearChange(gear, false)) {
        this->mSportShifting = 1.25f;
        return true;
    }

    return false;
}

// Credits: Brawltendo
ShiftStatus EngineRacer::OnGearChange(GearID gear) {
    if (gear >= this->GetNumGearRatios())
        return SHIFT_STATUS_NONE;
    // new gear can't be the same as the old one
    if (gear != this->mGear && gear >= G_REVERSE) {
        if (gear < this->mGear) {
            this->mGearShiftTimer = this->GetShiftDelay(gear) * 0.25f;
        } else {
            this->mGearShiftTimer = this->GetShiftDelay(gear);
        }
        this->mGear = gear;
        this->mClutch.Disengage();
        return SHIFT_STATUS_NORMAL;
    }

    return SHIFT_STATUS_NONE;
}

// Credits: Brawltendo
bool EngineRacer::DoGearChange(GearID gear, bool automatic) {
    if (gear > this->GetTopGear()) {
        return false;
    }
    if (gear < G_REVERSE) {
        return false;
    }

    GearID previous = (GearID)this->mGear;
    ShiftStatus status = this->OnGearChange(gear);
    if (status != SHIFT_STATUS_NONE) {
        this->mShiftStatus = status;
        this->mShiftPotential = SHIFT_POTENTIAL_NONE;
        ISimable *owner = GetOwner();

        if (owner->IsPlayer()) {
            // dispatch shift event
            new EPlayerShift(owner->GetInstanceHandle(), status, automatic, previous, gear);
        }
        return true;
    }

    return false;
}

// Credits: Brawltendo
float EngineRacer::GetDifferentialAngularVelocity(bool locked) const {
    float into_gearbox = 0.0f;
    bool in_reverse = this->GetGear() == G_REVERSE;

    if (this->FrontWheelDrive()) {
        float w_vel = (this->mSuspension->GetWheelAngularVelocity(0) + this->mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        if (!locked) {
            if (UMath::Abs(w_vel) > 0.0f)
                into_gearbox = w_vel;
        } else {
            into_gearbox = in_reverse ? UMath::Min(w_vel, 0.0f) : UMath::Max(w_vel, 0.0f);
        }
    }

    if (this->RearWheelDrive()) {
        float w_vel = (this->mSuspension->GetWheelAngularVelocity(2) + this->mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        if (!locked) {
            if (UMath::Abs(w_vel) > UMath::Abs(into_gearbox))
                into_gearbox = w_vel;
        } else {
            into_gearbox = in_reverse ? UMath::Min(w_vel, into_gearbox) : UMath::Max(w_vel, into_gearbox);
        }
    }

    return into_gearbox;
}

// Credits: Brawltendo
float EngineRacer::GetDriveWheelSlippage() const {
    float retval = 0.0f;
    int drivewheels = 0;
    if (this->RearWheelDrive()) {
        drivewheels += 2;
        retval += this->mSuspension->GetWheelSlip(TIRE_RR) + this->mSuspension->GetWheelSlip(TIRE_RL);
    }
    if (this->FrontWheelDrive()) {
        drivewheels += 2;
        retval += this->mSuspension->GetWheelSlip(TIRE_FL) + this->mSuspension->GetWheelSlip(TIRE_FR);
    }

    return retval / drivewheels;
}

// Credits: Brawltendo
void EngineRacer::SetDifferentialAngularVelocity(float w) {
    float current = this->GetDifferentialAngularVelocity(false);
    float diff = w - current;
    float speed = MPS2MPH(this->GetVehicle()->GetAbsoluteSpeed());
    int lockdiff = static_cast<int>(speed < 40.0f);
    if (this->RearWheelDrive()) {
        if (!this->mSuspension->IsWheelOnGround(2) && !this->mSuspension->IsWheelOnGround(3))
            lockdiff = 1;

        float w1 = this->mSuspension->GetWheelAngularVelocity(2);
        float w2 = this->mSuspension->GetWheelAngularVelocity(3);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        this->mSuspension->SetWheelAngularVelocity(2, w1 + diff);
        this->mSuspension->SetWheelAngularVelocity(3, w2 + diff);
    }

    lockdiff = static_cast<int>(speed < 40.0f);
    if (this->FrontWheelDrive()) {
        if (!this->mSuspension->IsWheelOnGround(0) && !this->mSuspension->IsWheelOnGround(1))
            lockdiff = 1;

        float w1 = this->mSuspension->GetWheelAngularVelocity(0);
        float w2 = this->mSuspension->GetWheelAngularVelocity(1);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        this->mSuspension->SetWheelAngularVelocity(0, w1 + diff);
        this->mSuspension->SetWheelAngularVelocity(1, w2 + diff);
    }
}

// Credits: Brawltendo
float EngineRacer::CalcSpeedometer(float rpm, unsigned int gear) const {
    const Physics::Tunings *tunings = this->GetVehicle()->GetTunings();
    return Physics::Info::Speedometer(this->mTranyInfo, this->mEngineInfo, this->mTireInfo, rpm, (GearID)gear, tunings);
}

float ClutchStiffness = 20.0f;

// Credits: Brawltendo
float EngineRacer::GetMaxSpeedometer() const {
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

// Credits: Brawltendo
float EngineRacer::GetSpeedometer() const {
    return this->CalcSpeedometer(RPS2RPM(this->mTransmissionVelocity), mGear);
}

// Credits: Brawltendo
void EngineRacer::LimitFreeWheels(float w) {
    unsigned int numwheels = this->mSuspension->GetNumWheels();
    for (unsigned int i = 0; i < numwheels; ++i) {
        if (!this->mSuspension->IsWheelOnGround(i)) {
            if (i < 2) {
                if (!this->FrontWheelDrive())
                    continue;
            } else if (!this->RearWheelDrive())
                continue;

            float ww = this->mSuspension->GetWheelAngularVelocity(i);
            if (ww * w < 0.0f) {
                ww = 0.0f;
            } else if (ww > 0.0f) {
                ww = UMath::Min(ww, w);
            } else if (ww < 0.0f) {
                ww = UMath::Max(ww, w);
            }

            this->mSuspension->SetWheelAngularVelocity(i, ww);
        }
    }
}

const float SmoothRPMDecel[] = {2.5f, 15.0f};

// Credits: Brawltendo
float Engine_SmoothRPM(bool is_shifting, GearID gear, float dT, float old_rpm, float new_rpm, float engine_inertia) {
    bool fast_shifting = is_shifting && gear > G_FIRST || gear == G_NEUTRAL;
    // this ternary is dumb but that's what makes it match
    float max_rpm_decel = -SmoothRPMDecel[fast_shifting ? 1 : 0];
    float rpm = new_rpm;
    float max_decel = max_rpm_decel * 1000.0f / engine_inertia;
    if (dT > 0.0f && (rpm - old_rpm) / dT < max_decel) {
        float newrpm = max_decel * dT + old_rpm;

        if (newrpm < rpm) {
            newrpm = rpm;
        }
        rpm = newrpm;
    }

    return rpm * 0.55f + old_rpm * 0.45f;
}

// Credits: Brawltendo
void EngineRacer::DoECU() {
    if (this->GetGear() <= G_NEUTRAL) {
        return;
    }
    // the speed at which the limiter starts to kick in
    float limiter = MPH2MPS(this->mEngineInfo.SPEED_LIMITER(0));
    if (limiter > 0.0f) {
        // the speed for the limiter to take full effect
        float cutoff = MPH2MPS(this->mEngineInfo.SPEED_LIMITER(1));
        if (cutoff > 0.0f) {
            float speedometer = GetSpeedometer();
            if (speedometer > limiter) {
                float limiter_range = speedometer - limiter;
                this->mThrottle *= (1.0f - UMath::Clamp(limiter_range / cutoff, 0.f, 1.0f));
            }
        }
    }
}

static const float Tweak_MinSpeedForNosMPH = 10.0f;
static const float Tweak_MaxNOSRechargeCheat = 2.0f;
static const float Tweak_MaxNOSDischargeCheat = 0.5f;

// Credits: Brawltendo
float EngineRacer::DoNos(const Physics::Tunings *tunings, float dT, bool engaged) {
    if (!this->HasNOS())
        return 1.0f;

    float speed_mph = MPS2MPH(this->GetVehicle()->GetAbsoluteSpeed());
    float recharge_rate = 0.0f;
    IPlayer *player = this->GetOwner()->GetPlayer();

    if ((player == nullptr) || player->CanRechargeNOS()) {
        float min_speed = this->mNOSInfo.RECHARGE_MIN_SPEED();
        float max_speed = this->mNOSInfo.RECHARGE_MAX_SPEED();
        if (speed_mph >= min_speed && this->mGear >= G_FIRST) {
            float t = UMath::Ramp(speed_mph, min_speed, max_speed);
            recharge_rate = UMath::Lerp(this->mNOSInfo.RECHARGE_MIN(), this->mNOSInfo.RECHARGE_MAX(), t);
        }
    }

    if (this->mGear < G_FIRST || this->mThrottle <= 0.0f || this->IsBlown())
        engaged = false;
    if (speed_mph < Tweak_MinSpeedForNosMPH && !this->IsNOSEngaged() || speed_mph < Tweak_MinSpeedForNosMPH * 0.5f && this->IsNOSEngaged())
        engaged = false;

    float nos_discharge = Physics::Info::NosCapacity(this->mNOSInfo, tunings);
    float nos_torque_scale = 1.0f;
    if (nos_discharge > 0.0f) {
        float nos_disengage = this->mNOSInfo.NOS_DISENGAGE();
        if (engaged && this->mNOSCapacity > 0.0f) {
            float discharge = dT / nos_discharge;
            // don't deplete nitrous
            if (Tweak_InfiniteNOS || this->GetVehicle()->GetDriverClass() == DRIVER_REMOTE) {
                discharge = 0.0f;
            }
            // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
            if (mCheater != nullptr) {
                discharge *= UMath::Lerp(1.0f, Tweak_MaxNOSDischargeCheat, this->mCheater->GetCatchupCheat());
            }
            this->mNOSCapacity -= discharge;
            nos_torque_scale = Physics::Info::NosBoost(this->mNOSInfo, tunings);
            this->mNOSEngaged = 1.0f;
            this->mNOSCapacity = UMath::Max(this->mNOSCapacity, 0.0f);
        } else if (this->mNOSEngaged > 0.0f && nos_disengage > 0.0f) {
            // nitrous can't start recharging until the disengage timer runs out
            this->mNOSEngaged -= dT / nos_disengage;
            this->mNOSEngaged = UMath::Max(this->mNOSEngaged, 0.0f);
        } else if (this->mNOSCapacity < 1.0f && recharge_rate > 0.0f) {
            float recharge = dT / recharge_rate;
            // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
            if (this->mCheater != nullptr) {
                recharge *= UMath::Lerp(1.0f, Tweak_MaxNOSRechargeCheat, this->mCheater->GetCatchupCheat());
            }
            this->mNOSCapacity += recharge;
            this->mNOSCapacity = UMath::Min(this->mNOSCapacity, 1.0f);
            this->mNOSEngaged = 0.0f;
        } else {
            this->mNOSEngaged = 0.0f;
        }

    } else {
        this->mNOSCapacity = 0.0f;
        this->mNOSEngaged = 0.0f;
    }
    return nos_torque_scale;
}

// static const float Tweak_MaxPSIChangePerSec; // TODO value and use

// Credits: Brawltendo
void EngineRacer::DoInduction(const Physics::Tunings *tunings, float dT) {
    Physics::Info::eInductionType type = Physics::Info::InductionType(this->mInductionInfo);
    if (type == Physics::Info::INDUCTION_NONE) {
        this->mSpool = 0.0f;
        this->mInductionBoost = 0.0f;
        this->mPSI = 0.0f;
        return;
    }

    float desired_spool = UMath::Ramp(this->mThrottle, 0.0f, 0.5f);
    float rpm = RPS2RPM(this->mAngularVelocity);

    if (this->IsGearChanging())
        desired_spool = 0.0f;
    // turbocharger can't start spooling up until the engine rpm is >= the boost threshold
    if (type == Physics::Info::INDUCTION_TURBO_CHARGER && rpm < Physics::Info::InductionRPM(this->mEngineInfo, this->mInductionInfo, tunings)) {
        desired_spool = 0.0f;
    }

    if (this->mSpool > desired_spool) {
        float spool_time = this->mInductionInfo.SPOOL_TIME_DOWN();
        if (spool_time > UMath::Epsilon) {
            this->mSpool -= dT / spool_time;
            this->mSpool = UMath::Max(this->mSpool, desired_spool);
        } else {
            this->mSpool = desired_spool;
        }
    } else if (this->mSpool < desired_spool) {
        float spool_time = this->mInductionInfo.SPOOL_TIME_UP();
        if (spool_time > UMath::Epsilon) {
            this->mSpool += dT / spool_time;
            this->mSpool = UMath::Min(this->mSpool, desired_spool);
        } else {
            this->mSpool = desired_spool;
        }
    }

    float target_psi;
    this->mSpool = UMath::Clamp(this->mSpool, 0.0f, 1.0f);
    this->mInductionBoost = Physics::Info::InductionBoost(this->mEngineInfo, this->mInductionInfo, rpm, this->mSpool, tunings, &target_psi);
    if (this->mPSI > target_psi) {
        this->mPSI = UMath::Max(this->mPSI - dT * 20.0f, target_psi);
    } else if (mPSI < target_psi) {
        this->mPSI = UMath::Min(this->mPSI + dT * 20.0f, target_psi);
    }
}

// Credits: Brawltendo
float EngineRacer::DoThrottle() {
    if (this->IsBlown() || (this->mIInput == nullptr)) {
        // cut the throttle when the engine is blown
        return 0.0f;
    }
    return this->mIInput->GetControls().fGas;
}

// Credits: Brawltendo
void EngineRacer::DoShifting(float dT) {
    if ((this->mIInput != nullptr) && this->mIInput->IsAutomaticShift()) {
        this->AutoShift();
    }

    if (this->mGearShiftTimer > 0.0f) {
        this->mGearShiftTimer -= dT;
        if (this->mGearShiftTimer <= 0.0f) {
            this->mGearShiftTimer = 0.0f;
        }
    }

    if (this->mSportShifting > 0.0f && this->mShiftPotential) {
        if (this->mIInput != nullptr) {
            float gas = this->mIInput->GetControls().fGas;
            this->mSportShifting = UMath::Max(this->mSportShifting - dT * (2.0f - gas), 0.0f);
        } else {
            this->mSportShifting = 0.0f;
        }
    }
}

#define NUM_ENTRIES(a) (sizeof(a) / sizeof(*(a)))

GraphEntry<float> ClutchPlayData[] = {{-10.f, 1.f}, {-7.5f, 0.96f}, {-3.5f, 0.925f}, {-0.3f, 0.875f}, {-0.05f, 0.f}};
tGraph<float> ClutchPlayTable(ClutchPlayData, NUM_ENTRIES(ClutchPlayData));
static const bool Tweak_PrintShiftPotentials = false;
static const float Tweak_IdleClutchRPM = 800.0f;
static const bool Tweak_PrintClutch = false;
static const float ClutchReductionFactor = 0.0f;
static const int ClutchPlayEnabled = 1;
static const bool PrintClutchPlayDebugInfo = false;
static const float Tweak_SeizeRPM = 2000.0f;
float ClutchLimiter = 300.0f;
static const float Tweak_EngineDamageFrequency = 12.0f;
static const float Tweak_EngineDamageAmplitude = 0.5f;
static const float Tweak_EngineCounterClutch = 0.0f;

void EngineRacer::OnTaskSimulate(float dT) {
    IInput *iinput = this->mIInput;
    if (iinput == nullptr || this->mSuspension == nullptr) {
        return;
    }

    if (this->mSuspension->GetNumWheels() != 4) {
        return;
    }

    const Physics::Tunings *tunings = this->GetVehicle()->GetTunings();
    bool is_staging = this->GetVehicle()->IsStaging();
    this->mThrottle = this->DoThrottle();
    this->mNOSBoost = this->DoNos(tunings, dT, iinput->GetControls().fNOS);
    this->DoECU();
    this->DoInduction(tunings, dT);
    this->DoShifting(dT);

    float max_rpm = this->UseRevLimiter() ? this->mEngineInfo.RED_LINE() : this->mEngineInfo.MAX_RPM();
    float max_w = RPM2RPS(max_rpm);
    bool was_engaged = this->mClutch.GetState() == Clutch::ENGAGED;
    float min_w = RPM2RPS(this->mEngineInfo.IDLE());
    float engine_inertia = Physics::Info::EngineInertia(this->mEngineInfo, mGear != G_NEUTRAL);
    float axle_w = this->GetDifferentialAngularVelocity(false);
    float differential_w = this->GetDifferentialAngularVelocity(true);
    int num_wheels_onground = this->mSuspension->GetNumWheelsOnGround();
    float wheel_ratio = bMax(0.25f, num_wheels_onground * 0.25f);
    float clutch_ratio = this->mClutch.Update(dT);
    float gear_direction = this->mGear == G_REVERSE ? -1.0f : 1.0f;
    const float gear_ratio = this->GetGearRatio(this->mGear);
    float total_gear_ratio = this->GetGearRatio(this->mGear) * this->GetFinalGear() * gear_direction;
    float rpm = RPS2RPM(this->mAngularVelocity);

    float torque_converter = this->mTranyInfo.TORQUE_CONVERTER();
    if (Tweak_EnableTorqueConverter && torque_converter > 0.0f) {
        float converter_ratio = torque_converter * this->mThrottle * (1.0f - UMath::Ramp(rpm, this->mEngineInfo.IDLE(), this->mPeakTorqueRPM));
        if (this->IsGearChanging()) {
            converter_ratio *= clutch_ratio;
        }
        total_gear_ratio *= 1.0f + converter_ratio;
    }

    if (total_gear_ratio == 0.0f && this->mGear != G_NEUTRAL) {
        return;
    }

    if (this->mGear == G_REVERSE || this->mGear == G_FIRST) {
        float idle_limit = Tweak_IdleClutchRPM;
        if (this->mGear == G_REVERSE) {
            idle_limit *= 3.0f;
        }

        float idle_w = RPM2RPS(idle_limit) + min_w;
        if (this->mAngularVelocity > idle_w || this->mTransmissionVelocity > idle_w || this->mThrottle >= 0.1f) {
            this->mClutch.Engage(Tweak_1stGearClutchEngageTime);
        } else {
            this->mClutch.Disengage();
        }
    } else if (this->mGear == G_NEUTRAL) {
        this->mClutch.Disengage();
    } else {
        this->mClutch.Engage(Tweak_ClutchEngageTime);
    }

    float engine_torque = this->GetEngineTorque(rpm);
    float braking_torque = this->GetBrakingTorque(engine_torque, rpm);
    if (!is_staging) {
        float perfect_launch = this->GetVehicle()->GetPerfectLaunch();
        if (perfect_launch > 0.0f && this->mThrottle > 0.0f) {
            // force the engine to operate at peak torque during a perfect launch
            this->mThrottle = 1.0f;
            engine_torque = this->mPeakTorque * this->mNOSBoost;
            braking_torque = 0.0f;
        }
    }

    float old_clutchv = this->mTransmissionVelocity;
    this->mTransmissionVelocity = min_w + differential_w * total_gear_ratio * (max_w - min_w) / max_w;
    float trans_acceleration = (this->mTransmissionVelocity - old_clutchv) / dT;
    float overrev_torque = 0.0f;
    if (this->mGear != G_NEUTRAL && this->mClutch.GetState() == Clutch::ENGAGED && braking_torque * axle_w * gear_direction > overrev_torque) {
        braking_torque = -braking_torque;
    }

    float total_engine_torque = engine_torque * this->mThrottle + braking_torque * (1.0f - this->mThrottle);
    float drive_torque = 0.0f;
    float road_torque = overrev_torque;
    this->mEngineBraking = total_engine_torque < 0.0f;

    if (this->mGear != G_NEUTRAL) {
        switch (mClutch.GetState()) {
            case Clutch::ENGAGED: {
                this->mClutchRPMDiff = 0.0f;
                drive_torque = total_engine_torque;
                road_torque -= total_engine_torque * wheel_ratio;
                break;
            }
            case Clutch::ENGAGING: {
                float diff = this->mAngularVelocity - this->mTransmissionVelocity;
                if (diff > ClutchLimiter) {
                    diff = ClutchLimiter;
                } else if (diff < -ClutchLimiter) {
                    diff = -ClutchLimiter;
                }

                float stiffness = ClutchStiffness;
                float rpmdiff = RPS2RPM(this->mTransmissionVelocity - this->mAngularVelocity);
                float clutchingtorque;
                if (this->mClutchRPMDiff != 0.0f && rpmdiff * this->mClutchRPMDiff < 0.0f && bAbs(rpmdiff) > Tweak_SeizeRPM &&
                    bAbs(this->mClutchRPMDiff) > Tweak_SeizeRPM) {
                    stiffness *= 0.5f;
                }

                this->mClutchRPMDiff = rpmdiff;
                clutchingtorque = diff * stiffness * clutch_ratio;
                drive_torque += clutchingtorque;
                road_torque -= clutchingtorque * wheel_ratio;
                break;
            }
            case Clutch::DISENGAGED: {
                this->mClutchRPMDiff = 0.0f;
                break;
            }
            default:
                break;
        }
    }

    if (this->mGear != G_NEUTRAL) {
        if (this->mClutch.GetState() == Clutch::ENGAGED) {
            float ae = (total_engine_torque + road_torque) / engine_inertia;
            float diff = ae - trans_acceleration;
            float wheel_response = 0.1f;
            float response = 1.0f / engine_inertia;
            float response1;
            float torquesplit = 1.0f - this->mTranyInfo.TORQUE_SPLIT();
            if (this->FrontWheelDrive()) {
                float front_gear = UMath::Abs(total_gear_ratio);
                response += (1.0f - torquesplit) * wheel_response * front_gear * front_gear * 0.5f;
            }
            if (this->RearWheelDrive()) {
                float rear_gear = UMath::Abs(total_gear_ratio);
                response += torquesplit * wheel_response * rear_gear * rear_gear * 0.5f;
            }

            response1 = diff / response;
            drive_torque += UMath::Min(response1, 0.0f);
            road_torque -= response1 * wheel_ratio;
        }
    }

    if (this->mGear != G_NEUTRAL) {
        if (this->mClutch.GetState() == Clutch::ENGAGED && num_wheels_onground > 0) {
            float slip = this->GetDriveWheelSlippage();
            float delta = this->mTransmissionVelocity - this->mAngularVelocity;
            if (this->mThrottle > 0.2f && slip * gear_direction > 0.1f && mGear <= G_FIRST && delta < 0.0f) {
                this->mTransmissionVelocity = this->mAngularVelocity;
                this->SetDifferentialAngularVelocity(this->mAngularVelocity / total_gear_ratio);
            } else {
                float max_torque = bAbs(total_engine_torque);
                float counter_torque = -max_torque;
                road_torque += bClamp(delta * ClutchStiffness, counter_torque, max_torque);
            }
        }

        if (this->mGear == G_FIRST || this->mGear == G_REVERSE) {
            float etorque = total_engine_torque;
            float rtorque = road_torque;
            float torque_diff = total_engine_torque + road_torque;
            if (rtorque < etorque && torque_diff < 0.f) {
                float clutch_play_coeff = ClutchPlayTable.GetValue(torque_diff * 1000.f);
                float clutch_torque = clutch_play_coeff * torque_diff;
                road_torque += clutch_torque * this->mTranyInfo.CLUTCH_SLIP();
            }
        }
    }

    // used for when the player's car breaks down in the prologue
    if (this->mSabotage > 0.f) {
        float count_down = this->mSabotage - Sim::GetTime();
        if (count_down <= 0.f) {
            this->mSabotage = 0.f;
            this->Blow();
        } else {
            // oscillate engine angular vel while the engine breaks down
            this->mAngularVelocity += Tweak_EngineDamageAmplitude * UMath::Sina(count_down * Tweak_EngineDamageFrequency) * this->mAngularVelocity;
        }
    }

    if (this->mGear != G_NEUTRAL && this->mThrottle > 0.0f && this->mGear <= G_FIRST && this->mClutch.GetState() == Clutch::ENGAGED &&
        road_torque * total_engine_torque < 0.0f) {
        float clutch_slip = this->mTranyInfo.CLUTCH_SLIP() * this->mThrottle;
        float power_ratio = 1.0f - this->mThrottle * UMath::Ramp(rpm, this->mEngineInfo.IDLE(), this->mPeakTorqueRPM);
        clutch_slip *= power_ratio;
        float allowed_road_torque = 1.0f - clutch_slip;
        road_torque *= allowed_road_torque * allowed_road_torque;
    }

    this->mAngularAcceleration = (total_engine_torque + road_torque) / engine_inertia;
    this->mAngularVelocity += this->mAngularAcceleration * dT;
    this->mAngularVelocity = UMath::Clamp(this->mAngularVelocity, min_w, max_w);

    if (total_gear_ratio != 0.f) {
        this->LimitFreeWheels(max_w / total_gear_ratio);
    }

    if (this->mTransmissionVelocity > max_w && total_gear_ratio != 0.f) {
        if (drive_torque * total_gear_ratio > 0.f)
            drive_torque = 0.f;
        this->mTransmissionVelocity = max_w;
        this->SetDifferentialAngularVelocity(max_w / total_gear_ratio);
    }

    // apply catch up torque for AI racers (rubberbanding)
    if (drive_torque > 0.f && (this->mCheater != nullptr)) {
        drive_torque *= this->mCheater->GetCatchupCheat() * Tweak_CheaterTorqueBoost + 1.0f;
    }

    this->mDriveTorque = drive_torque * total_gear_ratio * this->GetGearEfficiency(this->mGear);
    this->mRPM = Engine_SmoothRPM(this->IsShiftingGear() || mClutch.GetState() == Clutch::DISENGAGED, this->GetGear(), dT, this->mRPM,
                                  RPS2RPM(this->mAngularVelocity), engine_inertia);

    if (this->mClutch.GetState() || this->GetVehicle()->IsStaging()) {
        this->mShiftPotential = SHIFT_POTENTIAL_NONE;
    } else {
        this->mShiftPotential = this->UpdateShiftPotential((GearID)this->mGear, RPS2RPM(this->mTransmissionVelocity));
    }
}

// Credits: Brawltendo
float EngineRacer::GetShiftPoint(GearID from_gear, GearID to_gear) const {
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

static const float EngineHeatUpRedLine = 0.45f;
static const float EngineHeatCoolDown = -0.1f;
static const float EngineHeatBlownThreshold = 1.0f;
static const float EngineHeatBlownFlag = 1.0f;
static const float EngineHeatPenaltyThreshold = 0.6f;
static const float EngineHeatTorquePenalty = 0.75f;
static const float PerfectShiftSplit = 3.0f;
static const float ShiftClutchPenalty = 0.0f;
static const bool bUseReverseInDrag = false;
static const float PerfectShiftBoost = 0.75f;
static const float Tweak_GoodShiftRangeMult = 1.5f;
static const bool Tweak_AllowAIPerfectShift = false;
static const bool bNoEngineBlown = false;

// total size: 0x1D8
class EngineDragster : public EngineRacer, public IDragEngine, public IDragTransmission {
  public:
    // Methods
    static Behavior *Construct(const BehaviorParams &params);

    EngineDragster(const BehaviorParams &bp);
    float CalcPotentialShiftBonus(Rpm rpm, GearID gear, GearID nextgear) const;
    void ComputeEngineHeat(float t);

    // Overrides
    ~EngineDragster() override {}

    // EngineRacer
    ShiftStatus OnGearChange(GearID gear) override;
    ShiftPotential UpdateShiftPotential(GearID gear, Rpm rpm) override;
    Nm GetEngineTorque(Rpm rpm) const override;

    // Behavior
    void Reset() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IEngineDamage
    void Repair() override;
    bool Blow() override;

    // Inline virtuals

    // ITiptronic
    // manumatic disabled because drag races are always in manual transmission mode
    bool SportShift(GearID gear) override {
        return false;
    }

    // IDragTransmission
    float GetShiftBoost() const override {
        if (!this->IsGearChanging() && this->mBoost > 0.0f) {
            return UMath::Lerp(1.0f, 2.0f, this->mBoost / PerfectShiftSplit * this->mPerfectShiftTime);
        }
        return 1.0f;
    }

    // IDragEngine
    float GetOverRev() const override {
        return this->mOverrev;
    }
    float GetHeat() const override {
        return UMath::Clamp(this->mHeat, 0.0f, 1.0f);
    }

    // EngineRacer
    bool UseRevLimiter() const override {
        return this->GetVehicle()->IsStaging() != false;
    }

    // Inlines

    float GetOptimalShiftRange(GearID to_gear) const {
        return this->GetGearRatio(to_gear) * this->GetTransmissionData().OPTIMAL_SHIFT();
    }

  private:
#ifdef EA_BUILD_A124
    float mShiftBonus;
#endif
    float mPotentialBonus;    // offset 0x1C0, size 0x4
    float mPerfectShiftTime;  // offset 0x1C4, size 0x4
    float mBoost;             // offset 0x1C8, size 0x4
    float mOverrev;           // offset 0x1CC, size 0x4
    float mHeat;              // offset 0x1D0, size 0x4
    ISuspension *mSuspension; // offset 0x1D4, size 0x4
};

EngineDragster::EngineDragster(const BehaviorParams &bp)
    : EngineRacer(bp), IDragEngine(bp.fowner), IDragTransmission(bp.fowner), mPotentialBonus(0.0f), mPerfectShiftTime(0.0f), mBoost(0.0f),
      mOverrev(0.0f), mHeat(0.0f), mSuspension(nullptr) {
    this->GetOwner()->QueryInterface(&mSuspension);
}

Behavior *EngineDragster::Construct(const BehaviorParams &params) {
    return new EngineDragster(params);
}

void EngineDragster::Reset() {
    EngineRacer::Reset();

    this->mPotentialBonus = 0.0f;
    this->mPerfectShiftTime = 0.0f;
    this->mBoost = 0.0f;
    this->mOverrev = 0.0f;
}

void EngineDragster::Repair() {
    EngineRacer::Repair();
    this->mHeat = 0.0f;
}

bool EngineDragster::Blow() {
    if (EngineRacer::Blow()) {
        this->mHeat = EngineHeatBlownFlag;
        return true;
    }

    return false;
}

void EngineDragster::OnBehaviorChange(const UCrc32 &mechanic) {
    EngineRacer::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
}

Nm EngineDragster::GetEngineTorque(Rpm rpm) const {
    Nm result = EngineRacer::GetEngineTorque(rpm);
    result *= this->GetShiftBoost();

    if (this->mHeat > 0.0f && result > 0.0f && this->GetGear() > G_FIRST) {
        float penalty = UMath::Ramp(this->mHeat, 0.0f, EngineHeatPenaltyThreshold);
        result *= 1.0f - penalty * EngineHeatTorquePenalty;
    }

    return result;
}

void EngineDragster::OnTaskSimulate(float dT) {
    this->mPotentialBonus = 0.0f;
    EngineRacer::OnTaskSimulate(dT);
    this->ComputeEngineHeat(dT);

    if (this->mBoost > 0.0f && !this->IsGearChanging()) {
        this->mPerfectShiftTime -= dT;
        if (this->mPerfectShiftTime <= 0.0f) {
            this->mPerfectShiftTime = 0.0f;
            this->mBoost = 0.0f;
        }
    }

    float red_line = this->GetRedline();
    float max_rmp = this->GetMaxRPM();
    float rpm = this->GetRPM();
    if (rpm > red_line) {
        float diff = max_rmp - red_line;
        if (diff > 0.0f) {
            this->mOverrev = UMath::Clamp((rpm - red_line) / diff, 0.0f, 1.0f);
        }
    } else {
        this->mOverrev = 0.0f;
    }
}

ShiftStatus EngineDragster::OnGearChange(GearID gear) {
    if (!bUseReverseInDrag && gear == G_REVERSE) {
        return SHIFT_STATUS_NONE;
    }

    if (this->GetVehicle()->IsStaging()) {
        // at the starting line, only allow shifting into first gear or neutral
        switch (gear) {
            case G_FIRST:
            case G_NEUTRAL:
                return EngineRacer::OnGearChange(gear);
            default:
                return SHIFT_STATUS_NONE;
        }
    }

    if (!Tweak_AllowAIPerfectShift && !this->GetOwner()->IsPlayer()) {
        return EngineRacer::OnGearChange(gear);
    }

    GearID oldgear = this->GetGear();
    ShiftPotential potential = this->GetShiftPotential();
    this->mPerfectShiftTime = 0.0f;
    this->mBoost = 0.0f;
    ShiftStatus status = EngineRacer::OnGearChange(gear);
    if (status != SHIFT_STATUS_NONE) {
        if (gear > oldgear && oldgear > G_NEUTRAL) {
            if (potential == SHIFT_POTENTIAL_PERFECT) {
                this->mPerfectShiftTime = PerfectShiftSplit;
                this->mBoost = this->mPotentialBonus * PerfectShiftBoost;
                new EPerfectShift(this->GetOwner()->GetInstanceHandle(), this->mPotentialBonus);
                return SHIFT_STATUS_PERFECT;
            } else if (potential == SHIFT_POTENTIAL_MISS) {
                new EMissShift(this->GetOwner()->GetInstanceHandle(), this->mPotentialBonus);
                return SHIFT_STATUS_MISSED;
            } else if (potential == SHIFT_POTENTIAL_GOOD) {
                return SHIFT_STATUS_GOOD;
            }
        }
    }

    return status;
}

ShiftPotential EngineDragster::UpdateShiftPotential(GearID gear, Rpm rpm) {
    if (!Tweak_AllowAIPerfectShift && !this->GetOwner()->IsPlayer()) {
        return EngineRacer::UpdateShiftPotential(gear, rpm);
    }

    rpm = this->GetRPM();
    if (EngineRacer::UpdateShiftPotential(gear, rpm) == SHIFT_POTENTIAL_DOWN) {
        return SHIFT_POTENTIAL_DOWN;
    }

    if (this->GetVehicle()->IsStaging()) {
        return SHIFT_POTENTIAL_NONE;
    }

    int have_traction = 1;
    if (gear == G_FIRST) {
        for (int i = 0; i < 4; ++i) {
            have_traction &= static_cast<int>(this->mSuspension->IsWheelOnGround(i) && this->mSuspension->GetWheelSlip(i) < 4.0f);
        }
    }

    if (gear < GetTopGear() && gear > G_NEUTRAL && have_traction) {
        GearID nextgear = (GearID)(gear + 1);
        this->mPotentialBonus = this->CalcPotentialShiftBonus(rpm, gear, nextgear);
        if (this->mPotentialBonus > 0.0f) {
            return SHIFT_POTENTIAL_PERFECT;
        } else if (this->mPotentialBonus < 0.0f) {
            return SHIFT_POTENTIAL_MISS;
        } else {
            float good_shift = this->GetShiftUpRPM(gear) - this->GetOptimalShiftRange(nextgear) * Tweak_GoodShiftRangeMult;
            if (rpm > good_shift) {
                return SHIFT_POTENTIAL_GOOD;
            }
        }
    }

    return SHIFT_POTENTIAL_NONE;
}

float EngineDragster::CalcPotentialShiftBonus(Rpm rpm, GearID gear, GearID nextgear) const {
    if ((!Tweak_AllowAIPerfectShift && !this->GetOwner()->IsPlayer()) || gear >= nextgear || gear == G_REVERSE || nextgear <= G_NEUTRAL ||
        gear <= G_NEUTRAL || nextgear > this->GetTopGear()) {
        return 0.0f;
    }

    float redline = this->GetRedline();
    if (rpm >= redline) {
        return -1.0f;
    }

    float perfect_shift_point = this->GetShiftUpRPM(gear) - this->GetOptimalShiftRange(nextgear);
    if (rpm >= perfect_shift_point) {
        float bonus = UMath::Ramp(rpm, perfect_shift_point, redline);
        return UMath::Lerp(1.0f, 0.5f, bonus);
    }

    return 0.0f;
}

void EngineDragster::ComputeEngineHeat(float t) {
    if (this->GetVehicle()->IsStaging()) {
        return;
    }

    if (this->GetVehicle()->GetDriverClass() != DRIVER_HUMAN) {
        return;
    }

    IPlayer *player = this->GetOwner()->GetPlayer();
    if ((player != nullptr) && player->InGameBreaker()) {
        return;
    }

    if (this->IsBlown()) {
        return;
    }

    if (this->GetGear() != G_FIRST || mBoost <= 0.0f) {
        float delta = this->mOverrev > 0.0f ? EngineHeatUpRedLine : EngineHeatCoolDown;
        this->mHeat = bMax(0.0f, delta * t + this->mHeat);
        if (this->mHeat >= EngineHeatBlownThreshold && Blow()) {
            this->mHeat = EngineHeatBlownFlag;
        }
    }
}
