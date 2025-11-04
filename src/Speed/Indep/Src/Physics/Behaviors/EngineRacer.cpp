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
                if (mTime <= 0.0f && mState == ENGAGING ) {
                    mState = ENGAGED;
                }
            }
            
            switch (mState) {
                case DISENGAGED:
                    return 0.25f;
                case ENGAGING:
                    return 1.0f - UMath::Ramp(mTime, 0.0f, mEngageTime) * 0.75f;
                case ENGAGED:
                    return 1.0f;
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
    static Behavior *Construct(const BehaviorParams &params);

    EngineRacer(const BehaviorParams &bp);
    GearID GuessGear(float speed) const;
    float GuessRPM(float speed, GearID atgear) const;
    ShiftPotential FindShiftPotential(GearID gear, float rpm) const;
    float GetDifferentialAngularVelocity(bool locked) const;
    float GetDriveWheelSlippage() const;
    void SetDifferentialAngularVelocity(float w);
    float CalcSpeedometer(float rpm, unsigned int gear) const;
    void LimitFreeWheels(float w);
    float GetBrakingTorque(float engine_torque, float rpm) const;
    void CalcShiftPoints();
    bool DoGearChange(GearID gear, bool automatic);
    void AutoShift();

    // Overrides
    override virtual ~EngineRacer();

    // IEngine
    override virtual void MatchSpeed(float speed);
    override virtual float GetHorsePower() const;

    // Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);
    override virtual void OnBehaviorChange(const UCrc32 &mechanic);

    // ITransmission
    override virtual float GetSpeedometer() const;
    override virtual float GetMaxSpeedometer() const;
    override virtual float GetShiftPoint(GearID from_gear, GearID to_gear) const;

    // IAttributeable
    override virtual void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey);

    // ITiptronic
    override virtual bool SportShift(GearID gear);
    
    // IEngineDamage
    override virtual void Sabotage(float time);
    override virtual bool Blow();

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

    // IEngine
    override virtual float GetMaxHorsePower() const { return mMaxHP; }
    override virtual float GetMinHorsePower() const { return FTLB2NM(Physics::Info::Torque(mEngineInfo, mEngineInfo.IDLE()) * mEngineInfo.IDLE()); }
    override virtual float GetRPM() const { return mRPM; }
    override virtual float GetMaxRPM() const { return mEngineInfo.MAX_RPM(); }
    override virtual float GetPeakTorqueRPM() const { return mPeakTorqueRPM; }
    override virtual float GetRedline() const { return mEngineInfo.RED_LINE(); }
    override virtual float GetMinRPM() const { return mEngineInfo.IDLE(); }
    override virtual float GetNOSCapacity() const { return mNOSCapacity; }
    override virtual float GetNOSBoost() const { return mNOSBoost; }
    override virtual bool IsNOSEngaged() const { return mNOSEngaged >= 1.0f; }
    override virtual bool HasNOS() const { return mNOSInfo.NOS_CAPACITY() > 0.0f && mNOSInfo.TORQUE_BOOST() > 0.0f; }
    override virtual float GetNOSFlowRate() const { return mNOSInfo.FLOW_RATE(); }

    override virtual void ChargeNOS(float charge) {
        if (HasNOS()) {
            mNOSCapacity = UMath::Clamp(mNOSCapacity + charge, 0.0f, 1.0f);
        }
    }

    virtual bool IsEngineBraking() { return mEngineBraking; }
    virtual bool IsShiftingGear() { return mGearShiftTimer > 0.0f; }
    override virtual bool IsReversing() const { return mGear == G_REVERSE; }

    // IInductable
    override virtual Physics::Info::eInductionType InductionType() const { return Physics::Info::InductionType(mInductionInfo); }
    override virtual float GetInductionPSI() const { return mPSI; }
    override virtual float InductionSpool() const { return mSpool; }
    override virtual float GetMaxInductionPSI() const { return mInductionInfo.PSI();}

    // IEngineDamage
    override virtual bool IsBlown() const { return mBlown; }
    override virtual void Repair() { mSabotage = 0.0f; mBlown = false; }
    override virtual bool IsSabotaged() const { return mSabotage > 0.0f; }

    // ITransmission
    override virtual float GetDriveTorque() const { return mDriveTorque; }
    override virtual GearID GetTopGear() const { return (GearID)(GetNumGearRatios() - 1); }
    override virtual GearID GetGear() const { return (GearID)mGear; }
    override virtual bool IsGearChanging() const { return mGearShiftTimer > 0.0f;}

    override virtual bool Shift(GearID gear) {
        return DoGearChange(gear, false);
    }
    override virtual ShiftStatus GetShiftStatus() const { return mShiftStatus; }
    override virtual ShiftPotential GetShiftPotential() const { return mShiftPotential; }

    virtual ShiftStatus OnGearChange(GearID gear);
    virtual bool UseRevLimiter() const { return true; }
    virtual void DoECU();
    virtual float DoThrottle();
    virtual void DoInduction(const Physics::Tunings *tunings, float dT);
    virtual float DoNos(const Physics::Tunings *tunings, float dT, bool engaged);
    virtual void DoShifting(float dT);
    virtual ShiftPotential UpdateShiftPotential(GearID gear, float rpm);
    virtual float GetEngineTorque(float rpm) const;
    
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

    const Attrib::Gen::transmission &GetTransmissionData() const {
        return mTranyInfo;
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

Behavior *EngineRacer::Construct(const BehaviorParams &params) {
    return new EngineRacer(params);
}

EngineRacer::EngineRacer(const BehaviorParams &bp)
: VehicleBehavior(bp, 0)
, ITransmission(bp.fowner)
, IEngine(bp.fowner)
, IInductable(bp.fowner)
, ITiptronic(bp.fowner)
, IRaceEngine(bp.fowner)
, IEngineDamage(bp.fowner)
, mDriveTorque(0.0f)
, mGear(G_NEUTRAL)
, mGearShiftTimer(0.0f)
, mThrottle(0.0f)
, mSpool(0.0f)
, mPSI(0.0f)
, mInductionBoost(0.0f)
, mAngularVelocity(0.0f)
, mAngularAcceleration(0.0f)
, mTransmissionVelocity(0.0f)
, mNOSCapacity(1.0f)
, mNOSBoost(1.0f)
, mNOSEngaged(0.0f)
, mClutchRPMDiff(0.0f)
, mEngineBraking(false)
, mSportShifting(0.0f)
, mIInput(NULL)
, mSuspension(NULL)
, mNOSInfo(this, 0)
, mInductionInfo(this, 0)
, mEngineInfo(this, 0)
, mTranyInfo(this, 0)
, mTireInfo(this, 0)
, mRPM(0.0f)
, mShiftStatus(SHIFT_STATUS_NONE)
, mShiftPotential(SHIFT_POTENTIAL_NONE)
, mPeakTorque(0.0f)
, mPeakTorqueRPM(0.0f)
, mMaxHP(0.0f)
, mClutch()
, mBlown(false)
, mSabotage(0.0f) {
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    EnableProfile("EngineRacer");

    GetOwner()->QueryInterface(&mIInput);
    GetOwner()->QueryInterface(&mSuspension);
    GetOwner()->QueryInterface(&mCheater);
    Reset();

    if (mNOSInfo.NOS_CAPACITY() > 0.0f) {
        mNOSCapacity = 1.0f;
    } else {
        mNOSCapacity = 0.0f;
    }

    CalcShiftPoints();
}

EngineRacer::~EngineRacer() {
    IAttributeable::UnRegister(this);
}

float EngineRacer::GetHorsePower() const {
    float engine_torque = GetEngineTorque(mRPM);
    return NM2HP(engine_torque * mThrottle, mRPM);
}

void EngineRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mCheater);
    }
}

void EngineRacer::Sabotage(float time) {
    if ((mSabotage <= 0.0f) && (time > FLOAT_EPSILON) && !IsBlown()) {
        mSabotage = Sim::GetTime() + time;
    }
}

bool EngineRacer::Blow() {
    if (!mBlown) {
        mBlown = true;
        mSabotage = 0.0f;
        new EEngineBlown(GetOwner()->GetInstanceHandle());
        return true;
    }
    return false;
}

void EngineRacer::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

void EngineRacer::Reset() {
    mDriveTorque = 0.0f;
    mAngularVelocity = RPM2RPS(mEngineInfo.IDLE());
    mAngularAcceleration = 0.0f;
    mRPM = mEngineInfo.IDLE();
    mTransmissionVelocity = 0.0f;
    mClutch.Reset();
    mGearShiftTimer = 0.0f;
    mSpool = 0.0f;
    mPSI = 0.0f;
    mInductionBoost = 0.0f;
    mShiftStatus = SHIFT_STATUS_NONE;
    mShiftPotential = SHIFT_POTENTIAL_NONE;
    mGear = G_FIRST;
    mNOSEngaged = 0.0f;
    mClutchRPMDiff = 0.0f;
    mThrottle = 0.0f;
    mNOSBoost = 1.0f;
    mSportShifting = 0.0f;

    CalcShiftPoints();
}

float EngineRacer::GetEngineTorque(float rpm) const {
    float ftlbs = Physics::Info::Torque(mEngineInfo, rpm);
    float result = FTLB2NM(ftlbs);
    result *= 1.0f + mInductionBoost;

    if (GetVehicle()->GetDriverClass() != DRIVER_REMOTE) {
        result *= mNOSBoost;
    }

    return result;
}

GearID EngineRacer::GuessGear(float speed) const {
    if (speed < 0.0f) {
        return G_REVERSE;
    }

    GearID result = G_FIRST;
    for (int i = G_FIRST; i < GetTopGear(); ++i) {
        GearID this_gear = (GearID)i;
        GearID next_gear = (GearID)(i + 1);

        float shift_at = GetShiftPoint(this_gear, next_gear);
        float this_rpm = GuessRPM(speed, this_gear);
        if (this_rpm <= shift_at) {
            break;
        }
        result = next_gear;
    }

    return result;
}

float EngineRacer::GuessRPM(float speed, GearID atgear) const {
    float wheelrear = Physics::Info::WheelDiameter(mTireInfo, false) * 0.5f;
    float wheelfront = Physics::Info::WheelDiameter(mTireInfo, true) * 0.5f;
    float avg_wheel_radius = (wheelrear + wheelfront) * 0.5f;

    if (avg_wheel_radius <= 0.0f) {
        return mEngineInfo.IDLE();
    }
    
    float differential_w = UMath::Abs(speed) / avg_wheel_radius;
    float max_w = RPM2RPS(mEngineInfo.RED_LINE());
    float min_w = RPM2RPS(mEngineInfo.IDLE());
    float rear_end = GetFinalGear();
    float total_gear_ratio = GetFinalGear() * GetGearRatio(atgear);
    float av = RPS2RPM(min_w + differential_w * total_gear_ratio * (max_w - min_w) / max_w);
    av = UMath::Clamp(av, mEngineInfo.IDLE(), mEngineInfo.RED_LINE());
    return av;
}

void EngineRacer::MatchSpeed(float speed) {
    Reset();
    mAngularAcceleration = 0.0f;
    mSportShifting = 0.0f;
    mGearShiftTimer = 0.0f;
    mGear = GuessGear(speed);
    mRPM = GuessRPM(speed, (GearID)mGear);
    mAngularVelocity = RPM2RPS(mRPM);
    mTransmissionVelocity = mAngularVelocity;
    mClutch.Engage(0.0f);
}

// Credits: Brawltendo
float EngineRacer::GetBrakingTorque(float engine_torque, float rpm) const {
    float torque = engine_torque;
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
        return -torque * UMath::Clamp(load_pct, 0.f, 1.f);
    } else {
        return -torque * mEngineInfo.ENGINE_BRAKING(0);
    }
}

void EngineRacer::CalcShiftPoints() {
    bool shift_points_calced = Physics::Info::ShiftPoints(mTranyInfo, mEngineInfo, mInductionInfo, mShiftUpRPM, mShiftDownRPM, 10);
    Attrib::Gen::pvehicle pvehicle(GetOwner()->GetAttributes());

    mPeakTorque = Physics::Info::MaxInductedTorque(pvehicle, mPeakTorqueRPM, NULL);
    mPeakTorque = FTLB2NM(mPeakTorque);
    mMaxHP = Physics::Info::MaxInductedPower(pvehicle, NULL);
}


// Credits: Brawltendo
void EngineRacer::AutoShift() {
    if (mGear == G_REVERSE || mGearShiftTimer > 0.0f || GetVehicle()->IsStaging() || mSportShifting > 0.0f)
        return;

    // skip neutral when using auto transmission
    if (mGear == G_NEUTRAL) {
        mGear = G_FIRST;
        return;
    }

    switch (mShiftPotential) {
        case SHIFT_POTENTIAL_DOWN: {
            int next_gear = mGear - 1;
            if (next_gear > G_FIRST) {
                float current_rpm = RPS2RPM(mTransmissionVelocity);
                float rpm = current_rpm * GetRatioChange(next_gear, mGear);
                for (; next_gear > G_FIRST && FindShiftPotential((GearID)next_gear, rpm) == SHIFT_POTENTIAL_DOWN; ) {
                    rpm = current_rpm * GetRatioChange(--next_gear, mGear);
                }
            }
            DoGearChange((GearID)next_gear, true);
            break;
        }
        case SHIFT_POTENTIAL_NONE:
            break;
        case SHIFT_POTENTIAL_UP:
        case SHIFT_POTENTIAL_PERFECT:
        case SHIFT_POTENTIAL_MISS: {
            int have_traction = 1;
            for (int i = 0; i < 4; ++i) {
                have_traction &= mSuspension->IsWheelOnGround(i) && mSuspension->GetWheelSlip(i) < 4.f;
            }
            if (have_traction) {
                DoGearChange((GearID)(mGear + 1), true);
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
ShiftPotential EngineRacer::FindShiftPotential(GearID gear, float rpm) const {
    if (gear <= G_NEUTRAL)
        return SHIFT_POTENTIAL_NONE;

    float shift_up_rpm = GetShiftUpRPM(gear);
    float shift_down_rpm = GetShiftDownRPM(gear);
    float lower_gear_ratio = GetGearRatio(gear - 1);

    // is able to shift down
    if (gear != G_FIRST && lower_gear_ratio > 0.0f) {
        float lower_gear_shift_up_rpm = ((GetShiftUpRPM(gear - 1) * GetGearRatio(gear)) / lower_gear_ratio) - 200.0f;
        
        if (Tweak_CoastShifting) {
            // lower downshift RPM when coasting
            float off_throttle_rpm = UMath::Lerp(mEngineInfo.IDLE(), shift_down_rpm, Tweak_CoastingPercent);
            shift_down_rpm = UMath::Lerp(off_throttle_rpm, shift_down_rpm, mThrottle);
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

ShiftPotential EngineRacer::UpdateShiftPotential(GearID gear, float rpm) {
    return FindShiftPotential(gear, rpm);
}

bool EngineRacer::SportShift(GearID gear) {
    if (gear == mGear || gear <= G_NEUTRAL || IsGearChanging())
        return false;

    ShiftPotential potential = GetShiftPotential();
    if (gear > mGear && potential == SHIFT_POTENTIAL_DOWN)
        return false;

    if (gear < mGear && potential == SHIFT_POTENTIAL_UP)
        return false;

    if (DoGearChange(gear, false)) {
        mSportShifting = 1.25f;
        return true;
    }

    return false;
}

// Credits: Brawltendo
ShiftStatus EngineRacer::OnGearChange(GearID gear) {
    if (gear >= GetNumGearRatios())
        return SHIFT_STATUS_NONE;
    // new gear can't be the same as the old one
    if (gear != mGear && gear >= G_REVERSE) {
        if (gear < mGear) {
            mGearShiftTimer = GetShiftDelay(gear) * 0.25f;
        } else {
            mGearShiftTimer = GetShiftDelay(gear);
        }
        mGear = gear;
        mClutch.Disengage();
        return SHIFT_STATUS_NORMAL;
    }

    return SHIFT_STATUS_NONE;
}

// Credits: Brawltendo
bool EngineRacer::DoGearChange(GearID gear, bool automatic) {
    if (gear > GetTopGear()) {
        return false;
    }
    if (gear < G_REVERSE) {
        return false;
    }

    GearID previous = (GearID)mGear;
    ShiftStatus status = OnGearChange(gear);
    if (status != SHIFT_STATUS_NONE) {
        mShiftStatus = status;
        mShiftPotential = SHIFT_POTENTIAL_NONE;
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
    bool in_reverse = GetGear() == G_REVERSE;

    if (FrontWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(0) + mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        if (!locked) {
            if (UMath::Abs(w_vel) > 0.0f)
                into_gearbox = w_vel;
        } else {
            into_gearbox = in_reverse ? UMath::Min(w_vel, 0.0f) : UMath::Max(w_vel, 0.0f);
        }
    }

    if (RearWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(2) + mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
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
    if (RearWheelDrive()) {
        drivewheels += 2;
        retval += mSuspension->GetWheelSlip(TIRE_RR) + mSuspension->GetWheelSlip(TIRE_RL);
    }
    if (FrontWheelDrive()) {
        drivewheels += 2;
        retval += mSuspension->GetWheelSlip(TIRE_FL) + mSuspension->GetWheelSlip(TIRE_FR);
    }

    return retval / drivewheels;
}

// Credits: Brawltendo
void EngineRacer::SetDifferentialAngularVelocity(float w) {
    float current = GetDifferentialAngularVelocity(0);
    float diff = w - current;
    float speed = MPS2MPH(GetVehicle()->GetAbsoluteSpeed());
    int lockdiff = speed < 40.0f;
    if (RearWheelDrive()) {
        if (!mSuspension->IsWheelOnGround(2) && !mSuspension->IsWheelOnGround(3))
            lockdiff = 1;

        float w1 = mSuspension->GetWheelAngularVelocity(2);
        float w2 = mSuspension->GetWheelAngularVelocity(3);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        mSuspension->SetWheelAngularVelocity(2, w1 + diff);
        mSuspension->SetWheelAngularVelocity(3, w2 + diff);
    }

    lockdiff = speed < 40.0f;
    if (FrontWheelDrive()) {
        if (!mSuspension->IsWheelOnGround(0) && !mSuspension->IsWheelOnGround(1))
            lockdiff = 1;

        float w1 = mSuspension->GetWheelAngularVelocity(0);
        float w2 = mSuspension->GetWheelAngularVelocity(1);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        mSuspension->SetWheelAngularVelocity(0, w1 + diff);
        mSuspension->SetWheelAngularVelocity(1, w2 + diff);
    }
}

// Credits: Brawltendo
float EngineRacer::CalcSpeedometer(float rpm, unsigned int gear) const {
    Physics::Tunings *tunings = GetVehicle()->GetTunings();
    return Physics::Info::Speedometer(mTranyInfo, mEngineInfo, mTireInfo, rpm, (GearID)gear, tunings);
}

// Credits: Brawltendo
float EngineRacer::GetMaxSpeedometer() const {
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

// Credits: Brawltendo
float EngineRacer::GetSpeedometer() const {
    return CalcSpeedometer(RPS2RPM(mTransmissionVelocity), mGear);
}

// Credits: Brawltendo
void EngineRacer::LimitFreeWheels(float w) {
    unsigned int numwheels = mSuspension->GetNumWheels();
    for (unsigned int i = 0; i < numwheels; ++i) {
        if (!mSuspension->IsWheelOnGround(i)) {
            if (i < 2) {
                if (!FrontWheelDrive())
                    continue;
            } else if (!RearWheelDrive())
                continue;

            float ww = mSuspension->GetWheelAngularVelocity(i);
            if (ww * w < 0.0f) {
                ww = 0.0f;
            } else if (ww > 0.0f) {
                ww = UMath::Min(ww, w);
            } else if (ww < 0.0f) {
                ww = UMath::Max(ww, w);
            }

            mSuspension->SetWheelAngularVelocity(i, ww);
        }
    }
}

float SmoothRPMDecel[] = {2.5f, 15.0f};

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
    if (GetGear() <= G_NEUTRAL) {
        return;
    }
    // the speed at which the limiter starts to kick in
    float limiter = MPH2MPS(mEngineInfo.SPEED_LIMITER(0));
    if (limiter > 0.0f) {
        // the speed for the limiter to take full effect
        float cutoff = MPH2MPS(mEngineInfo.SPEED_LIMITER(1));
        if (cutoff > 0.0f) {
            float speedometer = GetSpeedometer();
            if (speedometer > limiter) {
                float limiter_range = speedometer - limiter;
                mThrottle *= (1.0f - UMath::Clamp(limiter_range / cutoff, 0.f, 1.0f));
            }
        }
    }
}

bool Tweak_InfiniteNOS = false;
static const float Tweak_MinSpeedForNosMPH = 10.0f;
static const float Tweak_MaxNOSRechargeCheat = 2.0f;
static const float Tweak_MaxNOSDischargeCheat = 0.5f;

// Credits: Brawltendo
float EngineRacer::DoNos(const Physics::Tunings *tunings, float dT, bool engaged) {
    if (!HasNOS())
        return 1.0f;

    float speed_mph = MPS2MPH(GetVehicle()->GetAbsoluteSpeed());
    float recharge_rate = 0.0f;
    IPlayer *player = GetOwner()->GetPlayer();

    if (!player || player->CanRechargeNOS()) {
        float min_speed = mNOSInfo.RECHARGE_MIN_SPEED();
        float max_speed = mNOSInfo.RECHARGE_MAX_SPEED();
        if (speed_mph >= min_speed && mGear >= G_FIRST) {
            float t = UMath::Ramp(speed_mph, min_speed, max_speed);
            recharge_rate = UMath::Lerp(mNOSInfo.RECHARGE_MIN(), mNOSInfo.RECHARGE_MAX(), t);
        }
    }

    if (mGear < G_FIRST || mThrottle <= 0.0f || this->IsBlown())
        engaged = false;
    if (speed_mph < Tweak_MinSpeedForNosMPH && !IsNOSEngaged() || speed_mph < Tweak_MinSpeedForNosMPH * 0.5f && IsNOSEngaged())
        engaged = false;

    float nos_discharge = Physics::Info::NosCapacity(mNOSInfo, tunings);
    float nos_torque_scale = 1.0f;
    if (nos_discharge > 0.0f) {
        float nos_disengage = mNOSInfo.NOS_DISENGAGE();
        if (engaged && mNOSCapacity > 0.0f) {
            float discharge = dT / nos_discharge;
            // don't deplete nitrous
            if (Tweak_InfiniteNOS || GetVehicle()->GetDriverClass() == DRIVER_REMOTE)
                discharge = 0.0f;
            // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
            if (mCheater)
                discharge *= UMath::Lerp(1.0f, Tweak_MaxNOSDischargeCheat, mCheater->GetCatchupCheat());
            mNOSCapacity -= discharge;
            nos_torque_scale = Physics::Info::NosBoost(mNOSInfo, tunings);
            mNOSEngaged = 1.0f;
            mNOSCapacity = UMath::Max(mNOSCapacity, 0.0f);
        } else if (mNOSEngaged > 0.0f && nos_disengage > 0.0f) {
            // nitrous can't start recharging until the disengage timer runs out
            mNOSEngaged -= dT / nos_disengage;
            mNOSEngaged = UMath::Max(mNOSEngaged, 0.0f);
        } else if (mNOSCapacity < 1.0f && recharge_rate > 0.0f) {
            float recharge = dT / recharge_rate;
            // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
            if (mCheater)
                recharge *= UMath::Lerp(1.0f, Tweak_MaxNOSRechargeCheat, mCheater->GetCatchupCheat());
            mNOSCapacity += recharge;
            mNOSCapacity = UMath::Min(mNOSCapacity, 1.0f);
            mNOSEngaged = 0.0f;
        } else {
            mNOSEngaged = 0.0f;
        }

    } else {
        mNOSCapacity = 0.0f;
        mNOSEngaged = 0.0f;
    }
    return nos_torque_scale;
}

// Credits: Brawltendo
void EngineRacer::DoInduction(const Physics::Tunings *tunings, float dT) {
    Physics::Info::eInductionType type = Physics::Info::InductionType(mInductionInfo);
    if (type == Physics::Info::INDUCTION_NONE) {
        mSpool = 0.0f;
        mInductionBoost = 0.0f;
        mPSI = 0.0f;
        return;
    }

    float desired_spool = UMath::Ramp(mThrottle, 0.0f, 0.5f);
    float rpm = RPS2RPM(mAngularVelocity);

    if (IsGearChanging())
        desired_spool = 0.0f;
    // turbocharger can't start spooling up until the engine rpm is >= the boost threshold
    if (type == Physics::Info::INDUCTION_TURBO_CHARGER && rpm < Physics::Info::InductionRPM(mEngineInfo, mInductionInfo, tunings)) {
        desired_spool = 0.0f;
    }

    if (mSpool > desired_spool) {
        float spool_time = mInductionInfo.SPOOL_TIME_DOWN();
        if (spool_time > FLOAT_EPSILON) {
            mSpool -= dT / spool_time;
            mSpool = UMath::Max(mSpool, desired_spool);
        } else {
            mSpool = desired_spool;
        }
    } else if (mSpool < desired_spool) {
        float spool_time = mInductionInfo.SPOOL_TIME_UP();
        if (spool_time > FLOAT_EPSILON) {
            mSpool += dT / spool_time;
            mSpool = UMath::Min(mSpool, desired_spool);
        } else {
            mSpool = desired_spool;
        }
    }

    float target_psi;
    mSpool = UMath::Clamp(mSpool, 0.0f, 1.0f);
    mInductionBoost = Physics::Info::InductionBoost(mEngineInfo, mInductionInfo, rpm, mSpool, tunings, &target_psi);
    if (mPSI > target_psi) {
        mPSI = UMath::Max(mPSI - dT * 20.0f, target_psi);
    } else if (mPSI < target_psi) {
        mPSI = UMath::Min(mPSI + dT * 20.0f, target_psi);
    }
}

// Credits: Brawltendo
float EngineRacer::DoThrottle() {
    if (IsBlown() || !mIInput) {
        // cut the throttle when the engine is blown
        return 0.0f;
    }
    return mIInput->GetControls().fGas;

    return mIInput->GetControls().fGas;
}

// Credits: Brawltendo
void EngineRacer::DoShifting(float dT) {
    if (mIInput && mIInput->IsAutomaticShift()) {
        AutoShift();
    }

    if (mGearShiftTimer > 0.0f) {
        mGearShiftTimer -= dT;
        if (mGearShiftTimer <= 0.0f) {
            mGearShiftTimer = 0.0f;
        }
    }

    if (mSportShifting > 0.0f && mShiftPotential) {
        if (mIInput) {
            float gas = mIInput->GetControls().fGas;
            mSportShifting = UMath::Max(mSportShifting - dT * (2.0f - gas), 0.0f);
        } else {
            mSportShifting = 0.0f;
        }
    }
}

static const bool Tweak_EnableTorqueConverter = true;
static const float Tweak_ClutchEngageTime = 0.25f;
static const float Tweak_1stGearClutchEngageTime = 0.05f;
static const float Tweak_CheaterTorqueBoost = 0.5f;
float ClutchStiffness = 20.0f;
GraphEntry<float> ClutchPlayData[] =
{
    {-10.f, 1.f}, {-7.5f, 0.96f}, {-3.5f, 0.925f}, {-0.3f, 0.875f}, {-0.05f, 0.f}
};
tGraph<float> ClutchPlayTable(ClutchPlayData, 5);
static const bool Tweak_PrintShiftPotentials = false;
static const float Tweak_IdleClutchRPM = 800.0f;
static const bool Tweak_PrintClutch = false;
static const float ClutchReductionFactor = 0.0f;
static const int ClutchPlayEnabled = true;
static const bool PrintClutchPlayDebugInfo = false;
static const float Tweak_SeizeRPM = 2000.0f;
float ClutchLimiter = 300.0f;
static const float Tweak_EngineDamageFrequency = 12.0f;
static const float Tweak_EngineDamageAmplitude = 0.5f;
static const float Tweak_EngineCounterClutch = 0.0f;

void EngineRacer::OnTaskSimulate(float dT) {
    IInput *iinput = mIInput;
    if (iinput == NULL || mSuspension == NULL) {
        return;
    }

    if (mSuspension->GetNumWheels() != 4) {
        return;
    }

    Physics::Tunings *tunings = GetVehicle()->GetTunings();
    bool is_staging = GetVehicle()->IsStaging();
    mThrottle = DoThrottle();
    mNOSBoost = DoNos(tunings, dT, iinput->GetControls().fNOS);
    DoECU();
    DoInduction(tunings, dT);
    DoShifting(dT);

    float max_rpm = UseRevLimiter() ? mEngineInfo.RED_LINE() : mEngineInfo.MAX_RPM();
    float max_w = RPM2RPS(max_rpm);
    bool was_engaged = mClutch.GetState() == Clutch::ENGAGED;
    float min_w = RPM2RPS(mEngineInfo.IDLE());
    float engine_inertia = Physics::Info::EngineInertia(mEngineInfo, mGear != G_NEUTRAL);
    float axle_w = GetDifferentialAngularVelocity(false);
    float differential_w = GetDifferentialAngularVelocity(true);
    int num_wheels_onground = mSuspension->GetNumWheelsOnGround();
    float wheel_ratio = bMax(0.25f, num_wheels_onground * 0.25f);
    float clutch_ratio = mClutch.Update(dT);
    float gear_direction = mGear == G_REVERSE ? -1.0f : 1.0f;
    const float gear_ratio = GetGearRatio(mGear);
    float total_gear_ratio = GetGearRatio(mGear) * GetFinalGear() * gear_direction;
    float rpm = RPS2RPM(mAngularVelocity);

    float torque_converter = mTranyInfo.TORQUE_CONVERTER();
    if (Tweak_EnableTorqueConverter && torque_converter > 0.0f) {
        float converter_ratio = torque_converter * mThrottle * (1.0f - UMath::Ramp(rpm, mEngineInfo.IDLE(), mPeakTorqueRPM));
        if (IsGearChanging()) {
            converter_ratio *= clutch_ratio;
        }
        total_gear_ratio *= 1.0f + converter_ratio;
    }

    if (total_gear_ratio == 0.0f && mGear != G_NEUTRAL) {
        return;
    }

    if (mGear == G_REVERSE || mGear == G_FIRST) {
        float idle_limit = Tweak_IdleClutchRPM;
        if (mGear == G_REVERSE) {
            idle_limit *= 3.0f;
        }

        float idle_w = RPM2RPS(idle_limit) + min_w;
        if (mAngularVelocity > idle_w || mTransmissionVelocity > idle_w || mThrottle >= 0.1f) {
            mClutch.Engage(Tweak_1stGearClutchEngageTime);
        } else {
            mClutch.Disengage();
        }
    } else if (mGear == G_NEUTRAL) {
        mClutch.Disengage();
    } else {
        mClutch.Engage(Tweak_ClutchEngageTime);
    }

    float engine_torque = GetEngineTorque(rpm);
    float braking_torque = GetBrakingTorque(engine_torque, rpm);
    if (!is_staging) {
        float perfect_launch = GetVehicle()->GetPerfectLaunch();
        if (perfect_launch > 0.0f && mThrottle > 0.0f) {
            // force the engine to operate at peak torque during a perfect launch
            mThrottle      = 1.0f;
            engine_torque  = mPeakTorque * mNOSBoost;
            braking_torque = 0.0f;
        }
    }

    float old_clutchv = mTransmissionVelocity;
    mTransmissionVelocity = min_w + differential_w * total_gear_ratio * (max_w - min_w) / max_w;
    float trans_acceleration = (mTransmissionVelocity - old_clutchv) / dT;
    float overrev_torque = 0.0f;
    if (mGear != G_NEUTRAL && mClutch.GetState() == Clutch::ENGAGED && braking_torque * axle_w * gear_direction > overrev_torque) {
        braking_torque = -braking_torque;
    }
    
    float total_engine_torque = engine_torque * mThrottle + braking_torque * (1.0f - mThrottle);
    float drive_torque = 0.0f;
    float road_torque  = overrev_torque;
    mEngineBraking = total_engine_torque < 0.0f;

    if (mGear != G_NEUTRAL) {
        switch (mClutch.GetState()) {
            case Clutch::ENGAGED:
            {
                mClutchRPMDiff = 0.0f;
                drive_torque   = total_engine_torque;
                road_torque    -= total_engine_torque * wheel_ratio;
                break;
            }
            case Clutch::ENGAGING:
            {
                float diff = mAngularVelocity - mTransmissionVelocity;
                if (diff > ClutchLimiter) {
                    diff = ClutchLimiter;
                } else if (diff < -ClutchLimiter) {
                    diff = -ClutchLimiter;
                }

                float stiffness = ClutchStiffness;
                float rpmdiff   = RPS2RPM(mTransmissionVelocity - mAngularVelocity);
                float clutchingtorque;
                if (mClutchRPMDiff != 0.0f 
                &&  rpmdiff * mClutchRPMDiff < 0.0f 
                &&  bAbs(rpmdiff) > Tweak_SeizeRPM
                &&  bAbs(mClutchRPMDiff) > Tweak_SeizeRPM)
                {
                    stiffness *= 0.5f;
                }

                mClutchRPMDiff  = rpmdiff;
                clutchingtorque = diff * stiffness * clutch_ratio;
                drive_torque    += clutchingtorque;
                road_torque     -= clutchingtorque * wheel_ratio;
                break;
            }
            case Clutch::DISENGAGED:
            {
                mClutchRPMDiff = 0.0f;
                break;
            }
            default:
                break;
        }
    }

    if (mGear != G_NEUTRAL) {
        if (mClutch.GetState() == Clutch::ENGAGED) {
            float ae   = (total_engine_torque + road_torque) / engine_inertia;
            float diff = ae - trans_acceleration;
            float wheel_response = 0.1f;
            float response = 1.0f / engine_inertia;
            float response1;
            float torquesplit = 1.0f - mTranyInfo.TORQUE_SPLIT();
            if (FrontWheelDrive()) {
                float front_gear = UMath::Abs(total_gear_ratio);
                response += (1.0f - torquesplit) * wheel_response * front_gear * front_gear * 0.5f;
            }
            if (RearWheelDrive()) {
                float rear_gear = UMath::Abs(total_gear_ratio);
                response += torquesplit * wheel_response * rear_gear * rear_gear * 0.5f;
            }

            response1 = diff / response;
            drive_torque  += UMath::Min(response1, 0.0f);
            road_torque   -= response1 * wheel_ratio;
        }
    }

    if (mGear != G_NEUTRAL) {
        if (mClutch.GetState() == Clutch::ENGAGED && num_wheels_onground > 0) {
            float slip  = GetDriveWheelSlippage();
            float delta = mTransmissionVelocity - mAngularVelocity;
            if (mThrottle > 0.2f && slip * gear_direction > 0.1f && mGear <= G_FIRST && delta < 0.0f) {
                mTransmissionVelocity = mAngularVelocity;
                SetDifferentialAngularVelocity(mAngularVelocity / total_gear_ratio);
            } else {
                float max_torque     = bAbs(total_engine_torque);
                float counter_torque = -max_torque;
                road_torque += bClamp(delta * ClutchStiffness, counter_torque, max_torque);
            }
        }

        if (mGear == G_FIRST || mGear == G_REVERSE) {
            float etorque     = total_engine_torque;
            float rtorque     = road_torque;
            float torque_diff = total_engine_torque + road_torque;
            if (rtorque < etorque && torque_diff < 0.f) {
                float clutch_play_coeff = ClutchPlayTable.GetValue(torque_diff * 1000.f);
                float clutch_torque     = clutch_play_coeff * torque_diff;
                road_torque += clutch_torque * mTranyInfo.CLUTCH_SLIP();
            }
        }
    }

    // used for when the player's car breaks down in the prologue
    if (mSabotage > 0.f) {
        float count_down = mSabotage - Sim::GetTime();
        if (count_down <= 0.f) {
            mSabotage = 0.f;
            Blow();
        } else {
            // oscillate engine angular vel while the engine breaks down
            mAngularVelocity += Tweak_EngineDamageAmplitude * UMath::Sina(count_down * Tweak_EngineDamageFrequency) * mAngularVelocity;
        }
    }

    if (mGear != G_NEUTRAL 
    &&  mThrottle > 0.0f 
    &&  mGear <= G_FIRST 
    &&  mClutch.GetState() == Clutch::ENGAGED
    &&  road_torque * total_engine_torque < 0.0f) {
        float clutch_slip = mTranyInfo.CLUTCH_SLIP() * mThrottle;
        float power_ratio = 1.0f - mThrottle * UMath::Ramp(rpm, mEngineInfo.IDLE(), mPeakTorqueRPM);
        clutch_slip *= power_ratio;
        float allowed_road_torque = 1.0f - clutch_slip;
        road_torque *= allowed_road_torque * allowed_road_torque;
    }

    mAngularAcceleration = (total_engine_torque + road_torque) / engine_inertia;
    mAngularVelocity += mAngularAcceleration * dT;
    mAngularVelocity = UMath::Clamp(mAngularVelocity, min_w, max_w);

    if (total_gear_ratio != 0.f) {
        LimitFreeWheels(max_w / total_gear_ratio);
    }

    if (mTransmissionVelocity > max_w && total_gear_ratio != 0.f) {
        if (drive_torque * total_gear_ratio > 0.f)  drive_torque = 0.f;
        mTransmissionVelocity = max_w;
        SetDifferentialAngularVelocity(max_w / total_gear_ratio);
    }

    // apply catch up torque for AI racers (rubberbanding)
    if (drive_torque > 0.f && mCheater) {
        drive_torque *= mCheater->GetCatchupCheat() * Tweak_CheaterTorqueBoost + 1.0f;
    }

    mDriveTorque = drive_torque * total_gear_ratio * GetGearEfficiency(mGear);
    mRPM = Engine_SmoothRPM(IsShiftingGear() || mClutch.GetState() == Clutch::DISENGAGED, 
                            GetGear(), dT, mRPM, RPS2RPM(mAngularVelocity), engine_inertia);
    
    if (mClutch.GetState() || GetVehicle()->IsStaging()) {
        mShiftPotential = SHIFT_POTENTIAL_NONE;
    } else {
        mShiftPotential = UpdateShiftPotential((GearID)mGear, RPS2RPM(mTransmissionVelocity));
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
        return mShiftUpRPM[from_gear];
    }
    if (to_gear < from_gear) {
        return mShiftDownRPM[from_gear];
    }
    
    return 0.0f;
}



// ----------------------------------------------------------------------------------------
// EngineDragster
// ----------------------------------------------------------------------------------------

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
class EngineDragster : public EngineRacer,
                    public IDragEngine,
                    public IDragTransmission {
  public:
    // Methods
    static Behavior *Construct(const BehaviorParams &params);

    EngineDragster(const BehaviorParams &bp);
    float CalcPotentialShiftBonus(float rpm, GearID gear, GearID nextgear) const;
    void ComputeEngineHeat(float t);

    // Overrides
    override virtual ~EngineDragster() {}

    // EngineRacer
    override virtual ShiftStatus OnGearChange(GearID gear);
    override virtual ShiftPotential UpdateShiftPotential(GearID gear, float rpm);
    override virtual float GetEngineTorque(float rpm) const;

    // Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);
    override virtual void OnBehaviorChange(const UCrc32 &mechanic);
    
    // IEngineDamage
    override virtual void Repair();
    override virtual bool Blow();

    // Inline virtuals

    // ITiptronic
    // manumatic disabled because drag races are always in manual transmission mode
    override virtual bool SportShift(GearID gear) { return false; }

    // IDragTransmission
    override virtual float GetShiftBoost() const {
        if (!IsGearChanging() && mBoost > 0.0f) {
            return UMath::Lerp(1.0f, 2.0f, mBoost / PerfectShiftSplit * mPerfectShiftTime);
        }
        return 1.0f;
    }

    // IDragEngine
    override virtual float GetOverRev() const { return mOverrev; }
    override virtual float GetHeat() const { return UMath::Clamp(mHeat, 0.0f, 1.0f); }
    
    // EngineRacer
    override virtual bool UseRevLimiter() const { return GetVehicle()->IsStaging() != false; }

    // Inlines

    float GetOptimalShiftRange(GearID to_gear) const {
        return GetGearRatio(to_gear) * GetTransmissionData().OPTIMAL_SHIFT();
    }

  private:
    float mPotentialBonus;     // offset 0x1C0, size 0x4
    float mPerfectShiftTime;   // offset 0x1C4, size 0x4
    float mBoost;              // offset 0x1C8, size 0x4
    float mOverrev;            // offset 0x1CC, size 0x4
    float mHeat;               // offset 0x1D0, size 0x4
    ISuspension * mSuspension; // offset 0x1D4, size 0x4
};

EngineDragster::EngineDragster(const BehaviorParams &bp)
: EngineRacer(bp)
, IDragEngine(bp.fowner)
, IDragTransmission(bp.fowner)
, mPotentialBonus(0.0f)
, mPerfectShiftTime(0.0f)
, mBoost(0.0f)
, mOverrev(0.0f)
, mHeat(0.0f)
, mSuspension(NULL) {
    GetOwner()->QueryInterface(&mSuspension);
}

Behavior *EngineDragster::Construct(const BehaviorParams &params) {
    return new EngineDragster(params);
}

void EngineDragster::Reset() {
    EngineRacer::Reset();

    mPotentialBonus = 0.0f;
    mPerfectShiftTime = 0.0f;
    mBoost = 0.0f;
    mOverrev = 0.0f;
}

void EngineDragster::Repair() {
    EngineRacer::Repair();
    mHeat = 0.0f;
}

bool EngineDragster::Blow() {
    if (EngineRacer::Blow()) {
        mHeat = EngineHeatBlownFlag;
        return true;
    }
    
    return false;
}

void EngineDragster::OnBehaviorChange(const UCrc32 &mechanic) {
    EngineRacer::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
}

float EngineDragster::GetEngineTorque(float rpm) const {
    float result = EngineRacer::GetEngineTorque(rpm);
    result *= GetShiftBoost();

    if (mHeat > 0.0f && result > 0.0f && GetGear() > G_FIRST) {
        float penalty = UMath::Ramp(mHeat, 0.0f, EngineHeatPenaltyThreshold);
        result *= 1.0f - penalty * EngineHeatTorquePenalty;
    }

    return result;
}

void EngineDragster::OnTaskSimulate(float dT) {
    mPotentialBonus = 0.0f;
    EngineRacer::OnTaskSimulate(dT);
    ComputeEngineHeat(dT);

    if (mBoost > 0.0f && !IsGearChanging()) {
        mPerfectShiftTime -= dT;
        if (mPerfectShiftTime <= 0.0f) {
            mPerfectShiftTime = 0.0f;
            mBoost = 0.0f;
        }
    }

    float red_line = GetRedline();
    float max_rmp = GetMaxRPM();
    float rpm = GetRPM();
    if (rpm > red_line) {
        float diff = max_rmp - red_line;
        if (diff > 0.0f) {
            mOverrev = UMath::Clamp((rpm - red_line) / diff, 0.0f, 1.0f);
        }
    } else {
        mOverrev = 0.0f;
    }
}

ShiftStatus EngineDragster::OnGearChange(GearID gear) {
    if (!bUseReverseInDrag && gear == G_REVERSE) {
        return SHIFT_STATUS_NONE;
    }

    if (GetVehicle()->IsStaging()) {
        // at the starting line, only allow shifting into first gear or neutral
        switch (gear) {
            case G_FIRST:
            case G_NEUTRAL:
                return EngineRacer::OnGearChange(gear);
            default:
                return SHIFT_STATUS_NONE;
        }
    }

    if (!Tweak_AllowAIPerfectShift && !GetOwner()->IsPlayer()) {
        return EngineRacer::OnGearChange(gear);
    }

    GearID oldgear = GetGear();
    ShiftPotential potential = GetShiftPotential();
    mPerfectShiftTime = 0.0f;
    mBoost = 0.0f;
    ShiftStatus status = EngineRacer::OnGearChange(gear);
    if (status != SHIFT_STATUS_NONE) {
        if (gear > oldgear && oldgear > G_NEUTRAL) {
            if (potential == SHIFT_POTENTIAL_PERFECT) {
                mPerfectShiftTime = PerfectShiftSplit;
                mBoost = mPotentialBonus * PerfectShiftBoost;
                new EPerfectShift(GetOwner()->GetInstanceHandle(), mPotentialBonus);
                return SHIFT_STATUS_PERFECT;
            } else if (potential == SHIFT_POTENTIAL_MISS) {
                new EMissShift(GetOwner()->GetInstanceHandle(), mPotentialBonus);
                return SHIFT_STATUS_MISSED;
            } else if (potential == SHIFT_POTENTIAL_GOOD) {
                return SHIFT_STATUS_GOOD;
            }
        }
    }

    return status;
}

ShiftPotential EngineDragster::UpdateShiftPotential(GearID gear, float rpm) {
    if (!Tweak_AllowAIPerfectShift && !GetOwner()->IsPlayer()) {
        return EngineRacer::UpdateShiftPotential(gear, rpm);
    }

    rpm = GetRPM();
    if (EngineRacer::UpdateShiftPotential(gear, rpm) == SHIFT_POTENTIAL_DOWN) {
        return SHIFT_POTENTIAL_DOWN;
    }
    
    if (GetVehicle()->IsStaging()) {
        return SHIFT_POTENTIAL_NONE;
    }
    
    int have_traction = 1;
    if (gear == G_FIRST) {
        for (int i = 0; i < 4; ++i) {
            have_traction &= mSuspension->IsWheelOnGround(i) && mSuspension->GetWheelSlip(i) < 4.0f;
        }
    }

    if (gear < GetTopGear() && gear > G_NEUTRAL && have_traction) {
        GearID nextgear = (GearID)(gear + 1);
        mPotentialBonus = CalcPotentialShiftBonus(rpm, gear, nextgear);
        if (mPotentialBonus > 0.0f) {
            return SHIFT_POTENTIAL_PERFECT;
        } else if (mPotentialBonus < 0.0f) {
            return SHIFT_POTENTIAL_MISS;
        } else {
            float good_shift = GetShiftUpRPM(gear) - GetOptimalShiftRange(nextgear) * Tweak_GoodShiftRangeMult;
            if (rpm > good_shift) {
                return SHIFT_POTENTIAL_GOOD;
            }
        }
    }

    return SHIFT_POTENTIAL_NONE;
}

float EngineDragster::CalcPotentialShiftBonus(float rpm, GearID gear, GearID nextgear) const {
    if ((!Tweak_AllowAIPerfectShift && !GetOwner()->IsPlayer())
    || gear >= nextgear
    || gear == G_REVERSE
    || nextgear <= G_NEUTRAL
    || gear <= G_NEUTRAL
    || nextgear > GetTopGear()) {
        return 0.0f;
    }

    float redline = GetRedline();
    if (rpm >= redline) {
        return -1.0f;
    }

    float perfect_shift_point = GetShiftUpRPM(gear) - GetOptimalShiftRange(nextgear);
    if (rpm >= perfect_shift_point) {
        float bonus = UMath::Ramp(rpm, perfect_shift_point, redline);
        return UMath::Lerp(1.0f, 0.5f, bonus);
    }

    return 0.0f;
}

void EngineDragster::ComputeEngineHeat(float t) {
    if (GetVehicle()->IsStaging()) {
        return;
    }

    if (GetVehicle()->GetDriverClass() != DRIVER_HUMAN) {
        return;
    }

    IPlayer *player = GetOwner()->GetPlayer();
    if (player && player->InGameBreaker()) {
        return;
    }

    if (IsBlown()) {
        return;
    }

    if (GetGear() != G_FIRST || mBoost <= 0.0f) {
        float delta = mOverrev > 0.0f ? EngineHeatUpRedLine : EngineHeatCoolDown;
        mHeat = bMax(0.0f, delta * t + mHeat);
        if (mHeat >= EngineHeatBlownThreshold && Blow()) {
            mHeat = EngineHeatBlownFlag;
        }
    }
}
