#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/Chassis.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// Credits: Brawltendo
class SuspensionRacer : public Chassis, public Sim::Collision::IListener, public IAttributeable, Debugable {
  public:
    class Tire : public Wheel {
      public:
        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);
        void BeginFrame(float max_slip, float grip_boost, float traction_boost, float drag_reduction);
        void EndFrame(float dT);
        float ComputeLateralForce(float load, float slip_angle);
        float GetPilotFactor(const float speed);
        void CheckForBrakeLock(float ground_force);
        void CheckSign();
        void UpdateFree(float dT);
        float UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT);

        bool IsOnGround() const {
            return mCompression > 0.0f;
        }

        bool IsSlipping() const {
            return mTraction >= 1.0f;
        }

        float SetBrake(float brake) {
            mBrake = brake;
        }

        float SetEBrake(float ebrake) {
            mEBrake = ebrake;
        }

        float GetEBrake() const {
            return mEBrake;
        }

        float GetTraction() const {
            return mTraction;
        }

        float GetRoadSpeed() const {
            return mRoadSpeed;
        }

        float GetLoad() const {
            return mLoad;
        }

        float GetRadius() const {
            return mRadius;
        }

        float GetAngularVelocity() const {
            return mAV;
        }

        void SetAngularVelocity(float w) {
            mAV = w;
        }

        float GetLateralSpeed() const {
            return mLateralSpeed;
        }

        float GetCurrentSlip() const {
            return mSlip;
        }

        float GetToleratedSlip() const {
            return mMaxSlip;
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

        float GetTotalTorque() const {
            return mDriveTorque + mBrakeTorque;
        }

        float GetDriveTorque() const {
            return mDriveTorque;
        }

        float GetLongitudeForce() const {
            return mLongitudeForce;
        }

        bool IsBrakeLocked() const {
            return mBrakeLocked;
        }

        bool IsSteeringWheel() const {
            return mWheelIndex < 2;
        }

        void SetTractionCircle(const UMath::Vector2 &circle) {
            mTractionCircle = circle;
        }

        float GetSlipAngle() const {
            return mSlipAngle;
        }

        void MatchSpeed(float speed) {
            mAV = speed / mRadius;
            mRoadSpeed = speed;
            mTraction = 1.0f;
            mBrakeLocked = false;
            mSlip = 0.0f;
            mAngularAcc = 0.0f;
            mLastTorque = 0.0f;
            mBrake = 0.0f;
            mEBrake = 0.0f;
            mLateralSpeed = 0.0f;
        }

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
    };

    // Static functions
    static Behavior *Construct(const BehaviorParams &params);

    // Methods
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
    ~SuspensionRacer() override;
    void OnCollision(const COLLISION_INFO &cinfo) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) override;
    Meters GetRideHeight(unsigned int idx) const override;
    Radians GetWheelAngularVelocity(int index) const override;
    void MatchSpeed(float speed) override;
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    void Reset() override;
    void OnDebugDraw() override;

    override float GetWheelTraction(unsigned int index) const {
        return mTires[index]->GetTraction();
    }
    override float GetWheelRadius(unsigned int index) const {
        return mTires[index]->GetRadius();
    }
    override float GetWheelSlip(unsigned int idx) const {
        return mTires[idx]->GetCurrentSlip();
    }
    override float GetToleratedSlip(unsigned int idx) const {
        return mTires[idx]->GetToleratedSlip();
    }
    override float GetWheelSkid(unsigned int idx) const {
        return mTires[idx]->GetLateralSpeed();
    }
    override float GetWheelLoad(unsigned int i) const {
        return mTires[i]->GetLoad();
    }
    override void SetWheelAngularVelocity(int wheel, float w) {
        mTires[wheel]->SetAngularVelocity(w);
    }
    override unsigned int GetNumWheels() const {
        return 4;
    }
    override const UMath::Vector3 &GetWheelPos(unsigned int i) const {
        return mTires[i]->GetPosition();
    }
    override const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const {
        return mTires[i]->GetLocalArm();
    }
    override void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) {}
    override const float GetWheelRoadHeight(unsigned int i) const {
        return mTires[i]->GetNormal().w;
    }
    override float GetCompression(unsigned int i) const {
        return mTires[i]->GetCompression();
    }
    override const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const {
        return mTires[i]->GetNormal();
    }
    override bool IsWheelOnGround(unsigned int i) const {
        return mTires[i]->IsOnGround();
    }
    override const SimSurface &GetWheelRoadSurface(unsigned int i) const {
        return mTires[i]->GetSurface();
    }
    override const UMath::Vector3 &GetWheelVelocity(unsigned int i) const {
        return mTires[i]->GetVelocity();
    }
    override int GetNumWheelsOnGround() const {
        return mNumWheelsOnGround;
    }
    override float GetWheelSteer(unsigned int wheel) const {
        return wheel < 2 ? RAD2ANGLE(mSteering.Wheels[wheel]) : 0.0f;
    }
    override float GetMaxSteering() const {
        return DEG2ANGLE(mSteering.Maximum);
    }
    override float GetWheelSlipAngle(unsigned int idx) const {
        return mTires[idx]->GetSlipAngle();
    }

  public:
    struct Drift {
        Drift() : State(D_OUT), Value(0.0f) {}

        enum eState { D_OUT, D_ENTER, D_IN, D_EXIT } State; // offset 0x0, size 0x4
        float Value;                                        // offset 0x4, size 0x4

        void Reset() {
            State = SuspensionRacer::Drift::D_OUT;
            Value = 0.0f;
        }
    };

    struct Burnout {
        Burnout() : mState(0), mBurnOutTime(0.0f), mTraction(1.0f), mBurnOutAllow(0.0f) {}

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
    };

    struct Steering {
        Steering() : InputAverage(0.55f, 60.0f), InputSpeedCoeffAverage(0.15f, 60.0f) {
            Reset();
        }

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
    };

    struct Differential {
        void CalcSplit(bool locked);

        float angular_vel[2];
        int has_traction[2];
        float bias;
        float factor;
        float torque_split[2];
    };

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

  private:
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
    Drift mDrift;                                           // offset 0xFC, size 0x8
    Burnout mBurnOut;                                       // offset 0x104, size 0x10
    Steering mSteering;                                     // offset 0x114, size 0x98
    Tire *mTires[4];
};

float YawFrictionBoost(float yaw, float ebrake, float speed, float yawcontrol, float grade);

const float ZeroDegreeTable[6] = {0.0f};
float TwoDegreeTable[] = {0.0f, 1.2f, 2.3f, 3.0f, 3.0f, 2.8f};
float FourDegreeTable[] = {0.0f, 1.7f, 3.2f, 4.3f, 5.1f, 5.2f};
float SixDegreeTable[] = {0.0f, 1.8f, 3.5f, 4.9f, 5.8f, 6.1f};
float EightDegreeTable[] = {0.0f, 1.83f, 3.6f, 5.0f, 5.96f, 6.4f};
float TenDegreeTable[] = {0.0f, 1.86f, 3.7f, 5.1f, 6.13f, 6.7f};
float TwelveDegreeTable[] = {0.0f, 1.9f, 3.8f, 5.2f, 6.3f, 7.1f};

Table ZeroDegree = Table(ZeroDegreeTable, 6, 0.0f, 10.0f);
Table TwoDegree = Table(TwoDegreeTable, 6, 0.0f, 10.0f);
Table FourDegree = Table(FourDegreeTable, 6, 0.0f, 10.0f);
Table SixDegree = Table(SixDegreeTable, 6, 0.0f, 10.0f);
Table EightDegree = Table(EightDegreeTable, 6, 0.0f, 10.0f);
Table TenDegree = Table(TenDegreeTable, 6, 0.0f, 10.0f);
Table TwelveDegree = Table(TwelveDegreeTable, 6, 0.0f, 10.0f);

#ifdef TARGET_WIN32
// DRM protected variable
int *pLatForceMultipliers = NULL;
#endif
Table *LoadSensitivityTable[] = {&ZeroDegree, &TwoDegree, &FourDegree, &SixDegree, &EightDegree, &TenDegree, &TwelveDegree};
static const float NewCorneringScale = 1000.0f;
static const float LoadFactor = 0.8f;
static const float GripFactor = 2.5f;

static const float PostCollisionSteerReductionDuration = 1.0f;
bVector2 PostCollisionSteerReductionData[] = {bVector2(0.0f, 0.2f), bVector2(0.2f, 0.5f), bVector2(0.5f, 0.7f), bVector2(0.7f, 1.0f)};
Graph PostCollisionSteerReductionTable(PostCollisionSteerReductionData, 4);
static const float Tweak_CollisionImpulseSteerMax = 40.0f;
static const float Tweak_CollisionImpulseSteerMin = 10.0f;
static const float Tweak_WallSteerClosingSpeed = 56.25f;
static const float Tweak_WallSteerBodySpeed = 6.25f;

float BrakeSteeringRangeMultiplier = 1.45f;
float CounterSteerOn = MPH2MPS(30.f);
float CounterSteerOff;
float MAX_STEERING = 45.0f;

float SteeringRangeData[] = {40.0f, 20.0f, 10.0f, 5.5f, 4.5f, 3.25f, 2.9f, 2.9f, 2.9f, 2.9f};
float SteeringSpeedData[] = {1.0f, 1.0f, 1.0f, 0.56f, 0.5f, 0.35f, 0.3f, 0.3f, 0.3f, 0.3f};
float SteeringWheelRangeData[] = {45.0f, 15.0f, 11.0f, 8.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f};
float SteeringInputSpeedData[] = {1.0f, 1.05f, 1.1f, 1.5f, 2.2f, 3.1f};
float SteeringInputData[] = {1.0f, 1.05f, 1.1f, 1.2f, 1.3f, 1.4f};

// only the first steering remap table is actually used, the rest are HP2/UG1/UG2 leftovers

static float JoystickInputToSteerRemap1[] = {-1.0f,  -0.712f, -0.453f, -0.303f, -0.216f, -0.148f, -0.116f, -0.08f, -0.061f, -0.034f, 0.0f,
                                             0.034f, 0.061f,  0.08f,   0.116f,  0.148f,  0.216f,  0.303f,  0.453f, 0.712f,  1.0f};
static float JoystickInputToSteerRemap2[] = {-1.0f,  -0.736f, -0.542f, -0.4f, -0.292f, -0.214f, -0.16f, -0.123f, -0.078f, -0.036f, 0.0f,
                                             0.036f, 0.078f,  0.123f,  0.16f, 0.214f,  0.292f,  0.4f,   0.542f,  0.736f,  1.0f};
static float JoystickInputToSteerRemap3[] = {-1.0f,  -0.8f,  -0.615f, -0.483f, -0.388f, -0.288f, -0.22f, -0.161f, -0.111f, -0.057f, 0.0f,
                                             0.057f, 0.111f, 0.161f,  0.22f,   0.288f,  0.388f,  0.483f, 0.615f,  0.8f,    1.0f};
static float JoystickInputToSteerRemapDrift[] = {-1.0f, -1.0f,  -0.688f, -0.492f, -0.319f, -0.228f, -0.16f, -0.123f, -0.085f, -0.05f, 0.0f,
                                                 0.05f, 0.085f, 0.123f,  0.16f,   0.228f,  0.319f,  0.492f, 0.688f,  1.0f,    1.0f};
static const int STEER_REMAP_DEAD = 0;
static const int STEER_REMAP_MEDIUM = 1;
static const int STEER_REMAP_TWITCHY = 2;
static const int STEER_REMAP_DRIFT = 3;
static const int SteerInputRemapping = STEER_REMAP_MEDIUM;
static const int SteerInputRemappingDrift = STEER_REMAP_DRIFT;
static Table SteerInputRemapTables[] = {Table(JoystickInputToSteerRemap1, 21, -1.0f, 1.0f), Table(JoystickInputToSteerRemap2, 21, -1.0f, 1.0f),
                                        Table(JoystickInputToSteerRemap3, 21, -1.0f, 1.0f), Table(JoystickInputToSteerRemapDrift, 21, -1.0f, 1.0f)};

float SteeringRangeCoeffData[] = {1.0f, 1.0f, 1.1f, 1.2f, 1.25f, 1.35f};
Table SteeringRangeTable = Table(SteeringRangeData, 10, 0.0f, 160.0f);
Table SteeringWheelRangeTable = Table(SteeringWheelRangeData, 10, 0.0f, 160.0f);
Table SteeringRangeCoeffTable = Table(SteeringRangeCoeffData, 6, 0.0f, 1.0f);
Table SteeringSpeedTable = Table(SteeringSpeedData, 10, 0.0f, 160.0f);
Table SteeringInputSpeedCoeffTable = Table(SteeringInputSpeedData, 6, 0.0f, 10.0f);
Table SteeringInputCoeffTable = Table(SteeringInputData, 6, 0.0f, 1.0f);

static const float HardTurnSteeringThreshold = 0.5f;
static const float HardTurnTightenSpeed = 0.0f;
static const float Tweak_GameBreakerMaxSteer = 60.0f;
static const float Tweak_TuningSteering_Ratio = 0.2f;

SuspensionRacer::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(1), mRadius(UMath::Max(radius, 0.1f)), mWheelIndex(index), mAxleIndex(index >> 1), mSpecs(specs), mBrakes(brakes), mBrake(0.0f),
      mEBrake(0.0f), mAV(0.0f), mLoad(0.0f), mLateralForce(0.0f), mLongitudeForce(0.0f), mDriveTorque(0.0f), mBrakeTorque(0.0f), mLateralBoost(1.0f),
      mTractionBoost(1.0f), mSlip(0.0f), mLastTorque(0.0f), mRoadSpeed(0.0f), mAngularAcc(0.0f), mTraction(1.0f), mBottomOutTime(0.0f),
      mSlipAngle(0.0f), mTractionCircle(UMath::Vector2Make(1.0f, 1.0f)), mMaxSlip(0.5f), mGripBoost(1.0f), mDriftFriction(1.0f), mLateralSpeed(0.0f),
      mBrakeLocked(false), mLastSign(SuspensionRacer::Tire::WAS_ZERO), mDragReduction(0.0f) {}

void SuspensionRacer::Tire::BeginFrame(float max_slip, float grip_boost, float traction_boost, float drag_reduction) {
    mMaxSlip = max_slip;
    mDriveTorque = 0.0f;
    mBrakeTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
    mTractionCircle = UMath::Vector2Make(1.0f, 1.0f);
    mTractionBoost = traction_boost;
    mGripBoost = grip_boost;
    mDriftFriction = 1.0f;
    mDragReduction = drag_reduction;
}

void SuspensionRacer::Tire::EndFrame(float dT) {}

// Credits: Brawltendo
Newtons SuspensionRacer::Tire::ComputeLateralForce(float load, float slip_angle) {
    float angle = ANGLE2DEG(slip_angle);
    float norm_angle = angle * 0.5f;
    int slip_angle_table = (int)norm_angle;
    load *= 0.001f; // convert to kN
    float extra = norm_angle - slip_angle_table;
    load *= LoadFactor;

    if (slip_angle_table > 5) {
#ifdef TARGET_WIN32
        // PC version uses a DRM protected variable in the calculation here
#else
        return (mSpecs->GRIP_SCALE().At(mAxleIndex) * NewCorneringScale) * mGripBoost * GripFactor * LoadSensitivityTable[6]->GetValue(load);
#endif
    } else {
        float low = LoadSensitivityTable[slip_angle_table]->GetValue(load);
        float high = LoadSensitivityTable[slip_angle_table + 1]->GetValue(load);
#ifdef TARGET_WIN32
        // PC version uses a DRM protected variable in the calculation here
#else
        return (mSpecs->GRIP_SCALE().At(mAxleIndex) * NewCorneringScale) * mGripBoost * GripFactor * (extra * (high - low) + low);
#endif
    }
}

// Credits: Brawltendo
float SuspensionRacer::Tire::GetPilotFactor(const float speed) {
    float PilotFactor = 0.85f;

    if (mBrakeLocked) {
        return 1.0f;
    }
    if (mAV < 0.0f) {
        return 1.0f;
    }
    if (IsSteeringWheel()) {
        return 1.0f;
    }

    float speed_factor = (speed - MPH2MPS(30.0f)) / MPH2MPS(20.0f);
    float val = UMath::Clamp(speed_factor, 0.0f, 1.0f);
    return val * (1.0f - PilotFactor) + PilotFactor;
}

float BrakingTorque = 4.0f;
float EBrakingTorque = 10.0f;

// Credits: Brawltendo
void SuspensionRacer::Tire::CheckForBrakeLock(float ground_force) {
    const float brake_spec = mBrakes->BRAKE_LOCK().At(mAxleIndex) * FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * BrakingTorque;
    const float ebrake_spec = FTLB2NM(mBrakes->EBRAKE()) * EBrakingTorque;
    static float StaticToDynamicBrakeForceRatio = 1.2f;
    static float BrakeLockAngularVelocityFactor = 100.0f;

    float bt = mBrake * brake_spec;
    float ebt = mEBrake * ebrake_spec;
    float available_torque = (bt + ebt) * StaticToDynamicBrakeForceRatio;

    if (available_torque > ground_force * GetRadius() + UMath::Abs(mAV) * BrakeLockAngularVelocityFactor) {
        if (available_torque > 1.0f) {
            mBrakeLocked = true;
        } else {
            mBrakeLocked = false;
        }
        mAV = 0.0f;
    } else {
        mBrakeLocked = false;
    }
}

// Credits: Brawltendo
void SuspensionRacer::Tire::CheckSign() {
    if (mLastSign == WAS_POSITIVE) {
        if (mAV < 0.0f) {
            mAV = 0.0f;
        }
    } else if (mLastSign == WAS_NEGATIVE && mAV > 0.0f) {
        mAV = 0.0f;
    }

    if (mAV > FLOAT_EPSILON) {
        mLastSign = WAS_POSITIVE;
    } else if (mAV < -FLOAT_EPSILON) {
        mLastSign = WAS_NEGATIVE;
    } else {
        mLastSign = WAS_ZERO;
    }
}

float WheelMomentOfInertia = 10.0f;
static const float kOneMPH = 0.44703f;

// Credits: Brawltendo
// Updates forces for an unloaded/airborne tire
void SuspensionRacer::Tire::UpdateFree(float dT) {
    mLoad = 0.0f;
    mSlip = 0.0f;
    mTraction = 0.0f;
    mSlipAngle = 0.0f;
    CheckForBrakeLock(0.0f);

    if (mBrakeLocked) {
        mAngularAcc = 0.0f;
        mAV = 0.0f;
    } else {
        const float brake_spec = FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * BrakingTorque;
        const float ebrake_spec = FTLB2NM(mBrakes->EBRAKE()) * EBrakingTorque;
        float bt = mBrake * brake_spec;
        float ebt = mEBrake * ebrake_spec;
        ApplyBrakeTorque(mAV > 0.0f ? -bt : bt);
        ApplyBrakeTorque(mAV > 0.0f ? -ebt : ebt);

        mAngularAcc = GetTotalTorque() / WheelMomentOfInertia;
        mAV += mAngularAcc * dT;
    }
    CheckSign();
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

float RollingFriction = 2.0f;
static const float TireForceEllipseRatio = 1.5f;
static const float InvTireForceEllipseRatio = 1.0 / TireForceEllipseRatio;

// Credits: Brawltendo
float SuspensionRacer::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT) {
    const float brake_spec = FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * BrakingTorque;
    const float ebrake_spec = FTLB2NM(mBrakes->EBRAKE()) * EBrakingTorque;
    const float dynamicgrip_spec = mSpecs->DYNAMIC_GRIP().At(mAxleIndex);
    const float staticgrip_spec = mSpecs->STATIC_GRIP().At(mAxleIndex);
    // free rolling wheel
    if (mLoad <= 0.0f && !mBrakeLocked) {
        mAV = fwd_vel / mRadius;
    }

    float fwd_acc = (fwd_vel - mRoadSpeed) / dT;

    mRoadSpeed = fwd_vel;
    mLoad = UMath::Max(load, 0.0f);
    mLateralSpeed = lat_vel;

    float bt = mBrake * brake_spec;
    float ebt = mEBrake * ebrake_spec;
    float abs_fwd = UMath::Abs(fwd_vel);
    if (abs_fwd < 1.0f) {
        // when car is nearly stopped, apply brake torque using forward velocity and wheel load
        bt = -mBrake * load * fwd_vel / mRadius;
        ebt = -mEBrake * load * fwd_vel / mRadius;

        ApplyDriveTorque(-GetDriveTorque() * mEBrake);
        ApplyBrakeTorque(bt);
        ApplyBrakeTorque(ebt);
    } else {
        ApplyBrakeTorque(mAV > 0.0f ? -bt : bt);
        ApplyBrakeTorque(mAV > 0.0f ? -ebt : ebt);
    }

    mSlipAngle = UMath::Atan2a(lat_vel, abs_fwd);
    float groundfriction = 0.0f;
    float slip_speed = mAV * mRadius - fwd_vel;
    float dynamicfriction = 1.0f;
    mSlip = slip_speed;
    float skid_speed = UMath::Sqrt(slip_speed * slip_speed + lat_vel * lat_vel);
    float pilot_factor = GetPilotFactor(body_speed);
    if (skid_speed > FLOAT_EPSILON && (lat_vel != 0.0f || fwd_vel != 0.0f)) {
        dynamicfriction = dynamicgrip_spec * mTractionBoost;
        dynamicfriction *= pilot_factor;
        groundfriction = mLoad * dynamicfriction / skid_speed;
        float slipgroundfriction = mLoad * dynamicfriction / UMath::Sqrt(fwd_vel * fwd_vel + lat_vel * lat_vel);
        CheckForBrakeLock(abs_fwd * slipgroundfriction);
    }

    if (mTraction < 1.0f || mBrakeLocked) {
        mLongitudeForce = groundfriction;
        mLongitudeForce *= slip_speed;
        mLateralForce = -groundfriction * lat_vel;

        if (body_speed < kOneMPH && dynamicfriction > 0.1f) {
            mLateralForce /= dynamicfriction;
            mLongitudeForce /= dynamicfriction;
        }
        mLongitudeForce = UMath::Limit(mLongitudeForce, GetTotalTorque() / mRadius);
    } else {
        mBrakeLocked = false;
        mLongitudeForce = GetTotalTorque() / mRadius;
        float slip_ang = mSlipAngle;
        mLateralForce = ComputeLateralForce(mLoad, UMath::Abs(mSlipAngle));
        if (lat_vel > 0.0f) {
            mLateralForce = -mLateralForce;
        }
    }

    mLateralForce *= mLateralBoost;
    if (mTraction >= 1.0f && !mBrakeLocked) {
        float acc_diff = mAngularAcc * mRadius - fwd_acc;
        mLongitudeForce += acc_diff * WheelMomentOfInertia / mRadius;
    }

    bool use_ellipse = false;
    if (GetTotalTorque() * fwd_vel > 0.0f && !mBrakeLocked) {
        use_ellipse = true;
        mLongitudeForce *= TireForceEllipseRatio;
    }

    mLateralForce *= mTractionCircle.x;
    mLongitudeForce *= mTractionCircle.y;

    float len_force = UMath::Sqrt(mLateralForce * mLateralForce + mLongitudeForce * mLongitudeForce);
    float max_force = mLoad * staticgrip_spec * mTractionBoost * mDriftFriction;

    max_force *= pilot_factor;

    mTraction = 1.0f;
    float max_slip = mMaxSlip;

    if (len_force > max_force && len_force > 0.001f) {
        float ratio = max_force / len_force;
        mTraction = ratio;
        mLateralForce *= ratio;
        mLongitudeForce *= ratio;
        max_slip = (ratio * ratio) * max_slip;
    } else if (use_ellipse) {
        mLongitudeForce *= InvTireForceEllipseRatio;
    }

    if (UMath::Abs(slip_speed) > max_slip) {
        mTraction *= max_slip / UMath::Abs(slip_speed);
    }

    // factor surface friction into the tire force
    mLateralForce *= mSurface.LATERAL_GRIP();
    mLongitudeForce *= mSurface.DRIVE_GRIP();

    if (fwd_vel > 1.0f) {
        mLongitudeForce -= UMath::Sina(mSlipAngle) * mLateralForce * mDragReduction / mSpecs->GRIP_SCALE().At(mAxleIndex);
    } else {
        mLateralForce *= UMath::Min(UMath::Abs(lat_vel), 1.0f);
    }

    if (mBrakeLocked) {
        mAngularAcc = 0.0f;

    } else {
        if (mTraction < 1.0f) {
            float torque = (GetTotalTorque() - mLongitudeForce * mRadius + mLastTorque) * 0.5f;
            mLastTorque = torque;
            float rolling_resistance = RollingFriction * mSurface.ROLLING_RESISTANCE();
            float effective_torque = torque - mAV * rolling_resistance;
            mAngularAcc = (effective_torque / WheelMomentOfInertia) - (mTraction * mSlip) / (mRadius * dT);
        }

        mAngularAcc = UMath::Lerp(mAngularAcc, fwd_acc / mRadius, mTraction);
    }

    mAV += mAngularAcc * dT;
    CheckSign();
    return mLateralForce;
}

Behavior *SuspensionRacer::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionRacer(params, sp);
}

SuspensionRacer::SuspensionRacer(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp), mTireInfo(this, 0), mBrakeInfo(this, 0), mSuspensionInfo(this, 0), mTranyInfo(this, 0), mRB(NULL), mCollisionBody(NULL),
      mGameBreaker(0.0f), mNumWheelsOnGround(0), mLastGroundCollision(0.0f), mDrift(), mBurnOut(), mSteering() {
    EnableProfile("SuspensionRacer");
    MakeDebugable(DBG_PHYSICS_RACERS);
    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mCollisionBody);
    GetOwner()->QueryInterface(&mTransmission);
    GetOwner()->QueryInterface(&mHumanAI);
    Sim::Collision::AddListener(this, GetOwner(), "SuspensionRacer");
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);
    // IAttributeable::Register(this, 0);

    for (int i = 0; i < 4; ++i) {
        mTires[i] = NULL;
    }
    CreateTires();
}

SuspensionRacer::~SuspensionRacer() {

    Sim::Collision::RemoveListener(this);
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
    IAttributeable::UnRegister(this);
}

void SuspensionRacer::CreateTires() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
        bool is_front = IsFront(i);
        float diameter = Physics::Info::WheelDiameter(mTireInfo, is_front);
        mTires[i] = new Tire(diameter * 0.5f, i, mTireInfo, mBrakeInfo);
    }
    UMath::Vector3 dimension;
    GetOwner()->GetRigidBody()->GetDimension(dimension);

    float wheelbase = mSuspensionInfo.WHEEL_BASE();
    float axle_width_f = mSuspensionInfo.TRACK_WIDTH().At(0) - mTireInfo.SECTION_WIDTH().At(0) * 0.001f;
    float axle_width_r = mSuspensionInfo.TRACK_WIDTH().At(1) - mTireInfo.SECTION_WIDTH().At(1) * 0.001f;
    float front_axle = mSuspensionInfo.FRONT_AXLE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);

    GetWheel(0).SetLocalArm(fl);
    GetWheel(1).SetLocalArm(fr);
    GetWheel(2).SetLocalArm(rl);
    GetWheel(3).SetLocalArm(rr);
}

void SuspensionRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mTransmission);
    } else if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mHumanAI);
    }
}

void SuspensionRacer::OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) {}

Meters SuspensionRacer::GetRideHeight(unsigned int idx) const {
    float ride = Chassis::GetRideHeight(idx);
    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    if (tunings) {
        ride += INCH2METERS(tunings->Value[Physics::Tunings::RIDEHEIGHT]);
    }
    return ride;
}

Radians SuspensionRacer::GetWheelAngularVelocity(int index) const {
    SuspensionRacer::Tire *tire = mTires[index];
    if (tire->IsBrakeLocked()) {
        return 0.0f;
    }
    if (!tire->IsOnGround() || !tire->IsSlipping()) {
        return tire->GetAngularVelocity();
    }
    return tire->GetRoadSpeed() / tire->GetRadius();
}

void SuspensionRacer::DoAerobatics(Chassis::State &state) {
    DoJumpStabilizer(state);
}

static const float Tweak_SteerDragReduction = 0.15f;
static const float Tweak_GameBreakerSteerDragReduction = 0.15f;
static const float Tweak_GameBreakerExtraGs = -2.0f;
static const float TweakGameBreakerRampOutPhysicsTime = 1.0f / 3.0f;
static const float Tweak_DragAeroMult = 1.5f;

void SuspensionRacer::OnTaskSimulate(float dT) {
    if (!mCollisionBody || !mRB) {
        return;
    }

    ISimable *owner = GetOwner();

    float ride_extra = 0.0f;
    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    if (tunings) {
        ride_extra = tunings->Value[Physics::Tunings::RIDEHEIGHT];
    }
    SetCOG(0.0, ride_extra);

    State state;
    ComputeState(dT, state);

    mSteering.CollisionTimer = UMath::Max(mSteering.CollisionTimer - state.time, 0.0f);
    mGameBreaker = 0.0f;

    IPlayer *player = GetOwner()->GetPlayer();
    if (player && player->InGameBreaker()) {
        mGameBreaker = 1.0f;
    } else if (mGameBreaker > 0.0f) {
        mGameBreaker -= state.time * TweakGameBreakerRampOutPhysicsTime;
        mGameBreaker = UMath::Max(mGameBreaker, 0.0f);
    }
    if (mGameBreaker > 0.0f) {
        UMath::Vector3 extra_df;
        UMath::Scale(state.GetUpVector(), Tweak_GameBreakerExtraGs * mGameBreaker * state.mass * 9.81f, extra_df);
        mRB->ResolveForce(extra_df);
    }

    float max_slip = ComputeMaxSlip(state);
    float grip_scale = ComputeLateralGripScale(state);
    float traction_scale = ComputeTractionScale(state);
    float steerdrag_reduction = UMath::Lerp(Tweak_SteerDragReduction, 1.0f, mGameBreaker);
    if ((state.flags & 1) == 0) {
        float launch = GetVehicle()->GetPerfectLaunch();
        if (launch > 0.0f) {
            traction_scale += launch * 0.25f;
        }
    }

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame(max_slip, grip_scale, traction_scale, steerdrag_reduction);
    }

    float drag_pct = 1.0f - mGameBreaker * 0.75f;
    float aero_pct = 1.0f;
    if (state.driver_style == STYLE_DRAG) {
        aero_pct = Tweak_DragAeroMult;
    }
    DoAerodynamics(state, drag_pct, aero_pct, GetWheel(0).GetLocalArm().z, GetWheel(2).GetLocalArm().z, tunings);
    DoDriveForces(state);
    DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->UpdateTime(dT);
    }

    mSteering.WallNoseTurn = 0.0f;
    mSteering.WallSideTurn = 0.0f;

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->EndFrame(dT);
    }

    DoTireHeat(state);
    DoAerobatics(state);
    DoSleep(state);
    Chassis::OnTaskSimulate(dT);
}

void SuspensionRacer::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        mTires[i]->MatchSpeed(speed);
    }
    mBurnOut.Reset();
    mDrift.Reset();
}

UMath::Vector3 SuspensionRacer::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mCollisionBody) {
        return pos;
    } else {
        UMath::ScaleAdd(mCollisionBody->GetUpVector(), GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionRacer::Reset() {
    ISimable *owner = GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    UMath::Vector3 vUp;
    rigidBody.GetUpVector(vUp);

    unsigned int numonground = 0;
    this->mGameBreaker = 0.0f;
    for (int i = 0; i < GetNumWheels(); ++i) {
        SuspensionRacer::Tire &wheel = GetWheel(i);
        wheel.Reset();
        if (wheel.InitPosition(rigidBody, wheel.GetRadius())) {
            float upness = UMath::Clamp(UMath::Dot(UMath::Vector4To3(wheel.GetNormal()), vUp), 0.0f, 1.0f);
            float newCompression = wheel.GetNormal().w + GetRideHeight(i) * upness;
            if (newCompression < 0.0f) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    }
    this->mNumWheelsOnGround = numonground;
    mSteering.Reset();
    mBurnOut.Reset();
    mDrift.Reset();
}

void SuspensionRacer::OnCollision(const COLLISION_INFO &cinfo) {
    // TODO fix this -1U
    if (cinfo.type - 1U < COLLISION_INFO::WORLD) {
        float impulse = !cinfo.objAImmobile ? cinfo.impulseA : 0.0f;
        if (cinfo.objB == GetOwner()->GetInstanceHandle()) {
            impulse = !cinfo.objBImmobile ? cinfo.impulseB : 0.0f;
        }
        if (impulse > 10.0f) {
            float damper = UMath::Ramp(impulse, Tweak_CollisionImpulseSteerMin, Tweak_CollisionImpulseSteerMax);
            mSteering.CollisionTimer = UMath::Max(damper, mSteering.CollisionTimer);
        }
    }
    if (cinfo.type == COLLISION_INFO::GROUND) {
        mLastGroundCollision = Sim::GetTime();
    }
    if (cinfo.type == COLLISION_INFO::WORLD) {
        if (UMath::Abs(cinfo.normal.y) < 0.1f && this->mCollisionBody && (UMath::LengthSquare(cinfo.closingVel) < Tweak_WallSteerClosingSpeed)) {
            const UMath::Vector3 &vFoward = mCollisionBody->GetForwardVector();
            const UMath::Vector3 &vRight = mCollisionBody->GetRightVector();
            if ((mSteering.WallNoseTurn == 0.0f) && UMath::LengthSquare(cinfo.objAVel) < Tweak_WallSteerBodySpeed &&
                UMath::Dot(cinfo.normal, vFoward) <= 0.0f) {
                UMath::Vector3 rpos;
                UMath::Sub(cinfo.position, mRB->GetPosition(), rpos);
                float dirdot = UMath::Dot(rpos, vFoward);
                if (dirdot > mRB->GetDimension().z * 0.75f) {
                    float dot = UMath::Dot(cinfo.normal, vRight);
                    mSteering.WallNoseTurn = (dot > 0.0f ? 1.0f : -1.0f) - dot;
                }
            }
            if (mSteering.WallSideTurn == 0.0f && GetVehicle()->GetSpeed() < 0.0f) {
                float dirdot = UMath::Dot(cinfo.normal, vRight);
                if (UMath::Abs(dirdot) > FLOAT_EPSILON) {
                    UMath::Vector3 rpos;
                    UMath::Sub(cinfo.position, mRB->GetPosition(), rpos);
                    if (UMath::Abs(UMath::Dot(rpos, vFoward)) > (mRB->GetDimension().z * 0.75f)) {
                        mSteering.WallSideTurn = dirdot;
                    }
                }
            }
        }
    }
}

// Credits: Brawltendo
float SuspensionRacer::DoHumanSteering(Chassis::State &state) {
    float steer_input = state.steer_input;
    float steer = mSteering.Previous;

    if (steer >= 180.0f) {
        steer -= 360.0f;
    }

    float steering_coeff = mTireInfo.STEERING();
    ISteeringWheel::SteeringType steer_type = ISteeringWheel::kGamePad;

    IPlayer *player = GetOwner()->GetPlayer();
    if (player) {
        ISteeringWheel *device = player->GetSteeringDevice();

        if (device && device->IsConnected()) {
            steer_type = device->GetSteeringType();
        }
    }

    float max_steering;
    float newsteer = steer_input * CalculateMaxSteering(state, steer_type) * steering_coeff;
    newsteer = bClamp(newsteer, -45.0f, 45.0f);

    if (steer_type == ISteeringWheel::kGamePad) {
        int steer_remapping = SteerInputRemapping;
        steer_input = SteerInputRemapTables[steer_remapping].GetValue(steer_input);
        float steering_speed = (CalculateSteeringSpeed(state) * steering_coeff) * state.time;
        float max_diff = steer + steering_speed;
        newsteer = bClamp(newsteer, steer - steering_speed, max_diff);
        // this is absolutely pointless but it's part of the steering calculation for whatever reason
        if (bAbs(newsteer) < 0.0f) {
            newsteer = 0.0f;
        }
    }
    mSteering.LastInput = steer_input;
    mSteering.Previous = newsteer;

    // speedbreaker increases the current steering angle beyond the normal maximum
    // this change is instant, so the visual steering angle while using speedbreaker doesn't accurately represent this
    // instead it interpolates to this value so it looks nicer
    if (mGameBreaker > 0.0f) {
        newsteer = UMath::Lerp(newsteer, state.steer_input * Tweak_GameBreakerMaxSteer, mGameBreaker);
    }

    mSteering.InputAverage.Record(mSteering.LastInput, Sim::GetTime());
    return DEG2ANGLE(newsteer);
}

// Credits: Brawltendo
float SuspensionRacer::CalculateMaxSteering(Chassis::State &state, ISteeringWheel::SteeringType steer_type) {
    const float steer_input = state.steer_input;

    // max possible steering output scales with the car's forward speed
    float max_steering = SteeringRangeTable.GetValue(state.local_vel.z);
    // there are 2 racing wheel input types, one scales with speed and the other doesn't
    if (steer_type == ISteeringWheel::kWheelSpeedSensitive) {
        max_steering = SteeringWheelRangeTable.GetValue(state.local_vel.z);
    } else if (steer_type == ISteeringWheel::kWheelSpeedInsensitive) {
        return MAX_STEERING;
    }

    float tbcoeff = 1.0f - (state.gas_input + 1.0f - (state.brake_input + state.ebrake_input) * 0.5f) * 0.5f;
    max_steering *= BrakeSteeringRangeMultiplier * tbcoeff * SteeringSpeedTable.GetValue(state.local_vel.z) + 1.0f;
    max_steering *= SteeringRangeCoeffTable.GetValue(bAbs(mSteering.InputAverage.GetValue()));

    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    if (tunings) {
        max_steering *= tunings->Value[Physics::Tunings::STEERING] * Tweak_TuningSteering_Ratio + 1.0f;
    }

    float collision_coeff;
    // reduce steering range after collisions
    if (mSteering.CollisionTimer > 0.0f) {
        float secs = PostCollisionSteerReductionDuration - mSteering.CollisionTimer;
        if (secs < PostCollisionSteerReductionDuration && secs > 0.0f) {
            float speed_coeff = bMin(1.0f, state.local_vel.z / (MPH2MPS(170.0f) * 0.7f));
            speed_coeff = 1.0f - speed_coeff;
            collision_coeff = PostCollisionSteerReductionTable.GetValue(secs);
            max_steering *= speed_coeff * (1.0f - collision_coeff) + collision_coeff;
        }
    }

    float yaw_left = ANGLE2DEG(mTires[2]->GetSlipAngle());
    float yaw_right = ANGLE2DEG(mTires[3]->GetSlipAngle());
    // clamp the max steering range to [rear slip, MAX_STEERING] when countersteering
    if (steer_input > 0.0f && yaw_right > 0.0f) {
        max_steering = UMath::Max(max_steering, yaw_right);
        max_steering = UMath::Min(max_steering, MAX_STEERING);
    } else if (steer_input < 0.0f && yaw_left < 0.0f) {
        max_steering = UMath::Max(max_steering, -yaw_left);
        max_steering = UMath::Min(max_steering, MAX_STEERING);
    } else if (bAbs(mSteering.InputAverage.GetValue()) >= HardTurnSteeringThreshold) {
        max_steering = bMax(max_steering, mSteering.LastMaximum - state.time * HardTurnTightenSpeed);
    }

    max_steering = bMin(max_steering, MAX_STEERING);
    mSteering.LastMaximum = max_steering;
    return max_steering;
}

// Credits: Brawltendo
float SuspensionRacer::CalculateSteeringSpeed(Chassis::State &state) {
    // get a rough approximation of how fast the player is steering
    // this ends up creating a bit of a difference in how fast you can actually steer on a controller under normal circumstances
    // using a keyboard will always give you the fastest steering possible
    float steer_input_speed = (state.steer_input - mSteering.LastInput) / state.time;

    mSteering.InputSpeedCoeffAverage.Record(SteeringInputSpeedCoeffTable.GetValue(bAbs(steer_input_speed)), Sim::GetTime());

    // steering speed scales with vehicle forward speed
    float steer_speed = 180.0f;
    steer_speed *= (SteeringSpeedTable.GetValue(state.local_vel.z));
    steer_speed *= mSteering.InputSpeedCoeffAverage.GetValue();

    float steer_input = bAbs(mSteering.InputAverage.GetValue());
    return steer_speed * SteeringInputCoeffTable.GetValue(steer_input);
}

// Credits: Brawltendo
float SuspensionRacer::DoAISteering(Chassis::State &state) {
    mSteering.Maximum = 45.0f;
    if (state.driver_style != STYLE_DRAG)
        mSteering.Maximum = mTireInfo.STEERING() * 45.0f;

    return DEG2ANGLE(mSteering.Maximum * state.steer_input);
}

// Credits: Brawltendo
void SuspensionRacer::DoSteering(Chassis::State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float truesteer;
    UMath::Vector4 r4;
    UMath::Vector4 l4;

    if (mHumanAI && mHumanAI->IsPlayerSteering()) {
        truesteer = DoHumanSteering(state);
    } else {
        truesteer = DoAISteering(state);
    }

    ComputeAckerman(truesteer, state, &l4, &r4);
    right = Vector4To3(r4);
    left = Vector4To3(l4);
    mSteering.Wheels[0] = l4.w;
    mSteering.Wheels[1] = r4.w;
    DoWallSteer(state);
}

GraphEntry<float> BurnoutFrictionData[] = {{0.0f, 1.0f}, {5.0f, 0.8f}, {9.0f, 0.9f}, {12.6f, 0.833f}, {17.1f, 0.72f}, {25.0f, 0.65f}};
tGraph<float> BurnoutFrictionTable(BurnoutFrictionData, 6);
float BurnOutCancelSlipValue = 0.5f;
float asd[] = {1.0f, 2.0f, 3.0f};
float BurnOutYawCancel = 0.5f;
float BurnOutAllowTime = 1.0f;
float BurnOutMaxSpeed = 20.0f;
float BurnOutFishTailTime = 2.0f;
int BurnOutFishTails = 6;
static const float Tweak_MinThrottleForBurnout = 1.0f;

// Credits: Brawltendo
void SuspensionRacer::Burnout::Update(float dT, float speedmph, float max_slip, int max_slip_wheel, float yaw) {
    // continue burnout/fishtailing state
    if (GetState()) {
        if (speedmph > 5.0f && UMath::Abs(ANGLE2RAD(yaw)) > BurnOutYawCancel) {
            Reset();
        } else if (max_slip < BurnOutCancelSlipValue) {
            IncBurnOutAllow(dT);
            if (mBurnOutAllow > BurnOutAllowTime)
                Reset();
        } else {
            ClearBurnOutAllow();
            DecBurnOutTime(dT);
            if (mBurnOutTime < 0.0f) {
                SetState(GetState() - 1);
                SetBurnOutTime(BurnOutFishTailTime);
            }
        }
    }
    // initialize burnout/fishtailing state
    else if (speedmph < BurnOutMaxSpeed) {
        const float friction_mult = 1.4f;
        // these conditions were split, there was probably some debug stuff here
        if (max_slip > 0.5f) {
            float burnout_coeff;
            BurnoutFrictionTable.GetValue(&burnout_coeff, max_slip);
            SetTraction(burnout_coeff / friction_mult);
            float friction_cut = 1.5f - burnout_coeff;
            // burnout state changes according to what side of the axle the wheel that's slipping the most is on
            SetState(BurnOutFishTails * friction_cut + (max_slip_wheel & 1));
            SetBurnOutTime(BurnOutFishTailTime);
            ClearBurnOutAllow();
        }
    }
}

// Calculates artificial steering for when the car is touching a wall
// Credits: Brawltendo
void SuspensionRacer::DoWallSteer(Chassis::State &state) {
    float wall = mSteering.WallNoseTurn;
    // nose turn is applied when the car is perpendicular to the wall
    // allows the player to easily turn their car away from the wall after a head-on crash without reversing
    if (wall != 0.0f && mNumWheelsOnGround > 2 && state.gas_input > 0.0f) {
        float dW = state.steer_input * state.gas_input * 0.125f;
        if (wall * dW < 0.0f) {
            return;
        }

        dW *= UMath::Abs(wall);
        UMath::Vector3 chg = {};
        chg.y = dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(state.angular_vel, chg, chg);
        mRB->SetAngularVelocity(chg);
    }

    wall = mSteering.WallSideTurn;
    // side turn is only applied when in reverse and if touching a wall parallel to the car
    // it helps the player move their car away from a wall when backing up
    if (wall * state.steer_input * state.gas_input > 0.0f && mNumWheelsOnGround > 2 && !state.gear) {
        float dW = -state.steer_input * state.gas_input * 0.125f;
        dW *= UMath::Abs(wall);

        UMath::Vector3 chg = {};
        chg.y = dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(state.angular_vel, chg, chg);
        mRB->SetAngularVelocity(chg);
    }
}

static float LowSpeedSpeed = 0.0f;
static float HighSpeedSpeed = 30.0f;
static float MaxYawBonus = 0.35f;
static float LowSpeedYawBoost = 0.0f;
static float HighSpeedYawBoost = 1.0f;
static float YawEBrakeThreshold = 0.5f;
static float YawAngleThreshold = 20.0f;

// Credits: Brawltendo
float YawFrictionBoost(float yaw, float ebrake, float speed, float yawcontrol, float grade) {
    yaw = bAbs(yaw);
    float retval = 1.0f;
    retval += bAbs(grade);
    if (ebrake > YawEBrakeThreshold && yaw < bDegToRad(YawAngleThreshold))
        return retval;

    float speed_factor = (speed - LowSpeedSpeed) / (HighSpeedSpeed - LowSpeedSpeed);
    float boost = LowSpeedYawBoost + (HighSpeedYawBoost - LowSpeedYawBoost) * speed_factor;
    float bonus = yaw * yawcontrol * boost;
    if (bonus > MaxYawBonus)
        bonus = MaxYawBonus;
    return retval + bonus;
}

// Credits: Brawltendo
float SuspensionRacer::CalcYawControlLimit(float speed) const {
    if (mTransmission) {
        float maxspeed = mTransmission->GetMaxSpeedometer();
        if (maxspeed <= 0.0f) {
            return 0.0f;
        }
        float percent = UMath::Min(UMath::Abs(speed) / maxspeed, 1.0f);
        unsigned int numunits = mTireInfo.Num_YAW_CONTROL();
        if (numunits > 1) {
            float ratio = (numunits - 1) * percent;
            unsigned int index1 = ratio;
            ratio -= index1;
            unsigned int index2 = UMath::Min(numunits - 1, index1 + 1);
            float a = mTireInfo.YAW_CONTROL(index1);
            float b = mTireInfo.YAW_CONTROL(index2);
            return a + (b - a) * ratio;
        }
    }
    return mTireInfo.YAW_CONTROL(0);
}

GraphEntry<float> DriftStabilizerData[] = {{0.0f, 0.0f},        {0.2617994f, 0.1f},  {0.52359879f, 0.45f}, {0.78539819f, 0.85f},
                                           {1.0471976f, 0.95f}, {1.5533431f, 1.15f}, {1.5707964f, 0.0f}};
float DriftRearFrictionData[] = {1.1f, 0.95f, 0.87f, 0.77f, 0.67f, 0.6f, 0.51f, 0.43f, 0.37f, 0.34f};
tGraph<float> DriftStabilizerTable(DriftStabilizerData, 7);
Table DriftRearFrictionTable(DriftRearFrictionData, 10, 0.0f, 1.0f);
static const float DriftRotationalStabalizer = 4.0f;
static const float DriftYawAngularVelCoeff = 0.5f;
static const float Tweak_MinDriftSpeedMPH = 30.0f;
static const float Tweak_DriftSlipAngle = 12.0f;
static const float Tweak_DriftEnterSpeed = 30.0f;
static const float Tweak_DriftExitSpeed = 30.0f;
static const float Tweak_DriftCounterSteer = 4.0f;
static const float Tweak_GameBreakerDriftRechargePerSec = 0.5f;
static const float Tweak_GameBreakerDriftRechargeMinSpeed = 35.0f;
static const float Tweak_GameBreakerDriftRechargeYaw = 30.0f;
static const bool Tweak_AlwaysDrift = false;
static const bool DoDriftPhysics = true;

// Credits: Brawltendo
void SuspensionRacer::DoDrifting(const Chassis::State &state) {
    if (mDrift.State && ((state.flags & 1) || state.driver_style == STYLE_DRAG)) {
        mDrift.Reset();
        return;
    }

    float drift_change = 0.0f;
    switch (mDrift.State) {
        case SuspensionRacer::Drift::D_IN:
        case SuspensionRacer::Drift::D_ENTER:
            // the drift value will increment by (dT * 8) when entering and holding a drift
            drift_change = 8.0f;
            break;
        case SuspensionRacer::Drift::D_OUT:
        case SuspensionRacer::Drift::D_EXIT:
            // the drift value will decrement by (dT * 2) when not drifting or exiting a drift
            drift_change = -2.0f;
            break;
        default:
            break;
    }

    mDrift.Value += drift_change * state.time;
    // clamp the drift value between 0 and 1
    if (mDrift.Value <= 0.0f) {
        mDrift.State = SuspensionRacer::Drift::D_OUT;
        mDrift.Value = 0.0f;
    } else if (mDrift.Value >= 1.0f) {
        mDrift.State = SuspensionRacer::Drift::D_IN;
        mDrift.Value = 1.0f;
    }

    if (mDrift.State > SuspensionRacer::Drift::D_ENTER) {
        float avg_steer = mSteering.InputAverage.GetValue();
        if ((state.local_angular_vel.y * state.slipangle) < 0.0f && UMath::Abs(state.slipangle) <= 0.25f &&
            state.speed > MPH2MPS(Tweak_MinDriftSpeedMPH) && (avg_steer * state.slipangle) <= 0.0f &&
            UMath::Abs(state.slipangle) > DEG2ANGLE(Tweak_DriftSlipAngle)) {
            mDrift.State = SuspensionRacer::Drift::D_IN;
        } else if (state.gas_input * state.steer_input * state.slipangle > DEG2ANGLE(Tweak_DriftSlipAngle) &&
                   state.speed > MPH2MPS(Tweak_MinDriftSpeedMPH)) {
            mDrift.State = SuspensionRacer::Drift::D_IN;
        } else if (state.gas_input * UMath::Abs(state.slipangle) > DEG2ANGLE(Tweak_DriftSlipAngle)) {
            mDrift.State = SuspensionRacer::Drift::D_ENTER;
        } else {
            mDrift.State = SuspensionRacer::Drift::D_EXIT;
        }
    } else if (state.speed > MPH2MPS(Tweak_DriftEnterSpeed) &&
               (state.ebrake_input > 0.5f || UMath::Abs(state.slipangle) > DEG2ANGLE(Tweak_DriftSlipAngle))) {
        mDrift.State = SuspensionRacer::Drift::D_ENTER;
    }

    if (mDrift.Value <= 0.0f)
        return;
    {
        float yaw = ANGLE2RAD(state.slipangle);
        float ang_vel = state.local_angular_vel.y;

        // charge speedbreaker if not in use and drifting is detected
        if (mGameBreaker <= 0.0f && state.speed > MPH2MPS(Tweak_GameBreakerDriftRechargeMinSpeed) &&
            UMath::Abs(yaw) > DEG2RAD(Tweak_GameBreakerDriftRechargeYaw)) {
            IPlayer *player = GetOwner()->GetPlayer();
            if (player) {
                player->ChargeGameBreaker(state.time * Tweak_GameBreakerDriftRechargePerSec * mDrift.Value);
            }
        }

        // apply yaw damping torque
        if ((yaw * ang_vel) < 0.0f && mNumWheelsOnGround >= 2) {
            float yaw_coef = DriftStabilizerTable.GetValue(UMath::Abs(yaw)) * DriftRotationalStabalizer;
            UMath::Vector3 moment;

            UMath::Scale(state.GetUpVector(), (mDrift.Value * -ang_vel) * yaw_coef * state.inertia.y, moment);
            mRB->ResolveTorque(moment);
        }

        // detect counter steering
        float countersteer = 0.0f;
        if (state.steer_input * yaw > 0.0f) {
            countersteer = UMath::Abs(state.steer_input);
        }

        float yawangularvel_coeff = DriftYawAngularVelCoeff;
        float driftcoeff = UMath::Abs(yaw) * yawangularvel_coeff + countersteer * Tweak_DriftCounterSteer + UMath::Abs(ang_vel) * yawangularvel_coeff;
        float driftmult_rear = DriftRearFrictionTable.GetValue(driftcoeff * mDrift.Value);
        mTires[2]->SetDriftFriction(driftmult_rear);
        mTires[3]->SetDriftFriction(driftmult_rear);
    }
}

float EBrakeYawControlMin = 0.5f;
float EBrakeYawControlOnSpeed = 1.0f;
float EBrakeYawControlOffSpeed = 20.0f;
float EBrake180Yaw = 0.3f;
float EBrake180Speed = 80.0f;
static const float Tweak_GameBreakerGripIncrease = 0.75f;
static const float Tweak_StagingTraction = 0.25f;
static const float Tweak_DragYawControl = 0.1f;
static const float Tweak_StabilityControl = 2.5f;
static const float Tweak_BlownTireEbrake = 0.0f;
static const float Tweak_BlownTireBrake = 1.0f;
static const float Tweak_BlownTireTraction = 0.3f;

// Credits: Brawltendo
void SuspensionRacer::TuneWheelParams(Chassis::State &state) {
    float ebrake = state.ebrake_input;
    float t = state.time;
    float speedmph = MPS2MPH(state.local_vel.z);
    float car_yaw = ANGLE2RAD(state.slipangle);
    float yawcontrol = mSteering.YawControl;

    // engaging the handbrake decreases steering yaw control
    if (ebrake >= 0.5f) {
        yawcontrol -= EBrakeYawControlOffSpeed * t;
        if (yawcontrol < EBrakeYawControlMin) {
            yawcontrol = EBrakeYawControlMin;
        }
    } else {
        yawcontrol += EBrakeYawControlOnSpeed * t;
        if (yawcontrol > 1.0f) {
            yawcontrol = 1.0f;
        }
    }
    mSteering.YawControl = yawcontrol;

    float brake_biased[2] = {state.brake_input, state.brake_input};
    yawcontrol *= (1.0f - mDrift.Value); // pointless parentheses for matching purposes
    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    if (tunings) {
        // brake tuning adjusts the brake bias
        brake_biased[0] += brake_biased[0] * tunings->Value[Physics::Tunings::BRAKES] * 0.5f;
        brake_biased[1] -= brake_biased[1] * tunings->Value[Physics::Tunings::BRAKES] * 0.5f;
    }
    float suspension_yaw_control_limit = CalcYawControlLimit(state.speed);
    IPlayer *player = GetOwner()->GetPlayer();
    if (state.driver_style == STYLE_DRAG) {
        suspension_yaw_control_limit = 0.1f;
    } else if (player) {
        PlayerSettings *settings = player->GetSettings();
        if (settings) {
            // increase yaw control limit when stability control is off (unused by normal means)
            if (!settings->Handling) {
                suspension_yaw_control_limit += 2.5f;
            }
        }
    }

    float max_slip = 0.0f;
    int max_slip_wheel = 0;
    for (int i = 0; i < 4; ++i) {
        float lateral_boost = 1.0f;

        // at speeds below 10 mph, 5% of the current speed in mph is applied as the brake scale for driven wheels
        if (state.gas_input > 0.8f && state.brake_input > 0.5f && UMath::Abs(speedmph) < 10.0f && IsDriveWheel(i)) {
            mTires[i]->SetBrake(UMath::Abs(speedmph) * 0.05f);
        } else {
            mTires[i]->SetBrake(brake_biased[i >> 1]);
        }

        // handbrake only applies to the rear wheels
        if (IsRear(i)) {
            float b = ebrake;
            // increase handbrake multiplier when a hard handbrake turn is detected
            if (ebrake > 0.2f && car_yaw > EBrake180Yaw && speedmph < EBrake180Speed) {
                b += 0.5f;
            }
            mTires[i]->SetEBrake(b);
        } else {
            mTires[i]->SetEBrake(0.0f);
        }

        float friction_boost = 1.0f;
        // rear wheels get extra boost according to the yaw control
        if (IsRear(i)) {
            float grade = state.GetForwardVector().y;
            float boost = YawFrictionBoost(car_yaw, mTires[i]->GetEBrake(), state.speed, suspension_yaw_control_limit, grade) - 1.0f;
            friction_boost = yawcontrol * boost + 1.0f;
        }

        // speedbreaker increases front tire friction relative to the absolute steering input
        if (mGameBreaker > 0.0f && IsFront(i)) {
            float over_boost = mGameBreaker * UMath::Abs(state.steer_input) * 0.75f + 1.0f;
            lateral_boost = over_boost;
            friction_boost *= over_boost;
        }
        mTires[i]->ScaleTractionBoost(friction_boost);
        mTires[i]->SetLateralBoost(lateral_boost);

        if (tunings) {
            UMath::Vector2 circle;
            circle.x = tunings->Value[Physics::Tunings::HANDLING] * 0.2f + 1.0f;
            circle.y = 1.0f - tunings->Value[Physics::Tunings::HANDLING] * 0.2f;
            mTires[i]->SetTractionCircle(circle);
        }
        // traction is increased by perfect shifts in drag races and also by engaging the nitrous
        mTires[i]->ScaleTractionBoost(state.nos_boost * state.shift_boost); // TODO might not match on MSVC

        // popped tires are permanently braking and have reduced traction
        if ((1 << i) & state.blown_tires) {
            mTires[i]->SetEBrake(0.0f);
            mTires[i]->SetBrake(1.0f);
            mTires[i]->ScaleTractionBoost(0.3f);
        }

        // find the highest slip of all tires for the burnout/fishtailing state
        if (mTires[i]->GetCurrentSlip() > max_slip) {
            max_slip = mTires[i]->GetCurrentSlip();
            max_slip_wheel = i;
        }
    }

    // burnout state only applies when in first gear and the throttle is fully pressed outside of drag events
    if (state.driver_style != STYLE_DRAG && state.gear == G_FIRST && state.gas_input >= 1.0f) {
        mBurnOut.Update(state.time, MPS2MPH(state.local_vel.z), max_slip, max_slip_wheel, state.slipangle);
    } else {
        mBurnOut.Reset();
    }

    // lower traction for all wheels when staging
    if (state.flags & 1) {
        for (int i = 0; i < 4; ++i) {
            mTires[i]->ScaleTractionBoost(0.25f);
        }
    }

    DoDrifting(state);
}

// Credits: Brawltendo
void SuspensionRacer::Differential::CalcSplit(bool locked) {
    if (!has_traction[0] || !has_traction[1] || locked || (factor <= 0.0f)) {
        torque_split[0] = bias;
        torque_split[1] = 1.0f - bias;
        return;
    }
    float av_0 = angular_vel[0] * (1.0f - bias);
    float av_1 = angular_vel[1] * bias;
    float combined_av = UMath::Abs(av_0 + av_1);

    if (combined_av > FLOAT_EPSILON) {
        torque_split[0] = ((1.0f - factor) * bias) + ((factor * UMath::Abs(av_1)) / combined_av);
        torque_split[1] = ((1.0f - factor) * (1.0f - bias)) + ((factor * UMath::Abs(av_0)) / combined_av);
    } else {
        torque_split[0] = bias;
        torque_split[1] = 1.0f - bias;
    }

    torque_split[0] = UMath::Clamp(torque_split[0], 0.0f, 1.0f);
    torque_split[1] = UMath::Clamp(torque_split[1], 0.0f, 1.0f);
}

// Credits: Brawltendo
void SuspensionRacer::DoDriveForces(Chassis::State &state) {
    if (!mTransmission) {
        return;
    }

    float drive_torque = mTransmission->GetDriveTorque();
    SuspensionRacer::Differential center_diff;
    if (drive_torque == 0.0f) {
        return;
    }

    center_diff.factor = mTranyInfo.DIFFERENTIAL(2);
    if (center_diff.factor > 0.0f) {
        center_diff.bias = mTranyInfo.TORQUE_SPLIT();
        center_diff.angular_vel[0] = mTires[0]->GetAngularVelocity() + mTires[1]->GetAngularVelocity();
        center_diff.angular_vel[1] = mTires[2]->GetAngularVelocity() + mTires[3]->GetAngularVelocity();
        center_diff.has_traction[0] = mTires[0]->IsOnGround() || mTires[1]->IsOnGround();
        center_diff.has_traction[1] = mTires[2]->IsOnGround() || mTires[3]->IsOnGround();
        center_diff.CalcSplit(false);
    } else {
        center_diff.torque_split[0] = mTranyInfo.TORQUE_SPLIT();
        center_diff.torque_split[1] = 1.0f - center_diff.torque_split[0];
    }

    for (unsigned int axle = 0; axle < 2; ++axle) {
        float axle_torque = drive_torque * center_diff.torque_split[axle];
        if (UMath::Abs(axle_torque) > FLOAT_EPSILON) {
            SuspensionRacer::Differential diff;
            diff.bias = 0.5f;

            float fwd_slip = 0.0f;
            float lat_slip = 0.0f;

            float traction_control[2] = {1.0f, 1.0f};
            float traction_boost[2] = {1.0f, 1.0f};
            diff.factor = mTranyInfo.DIFFERENTIAL(axle);

            for (unsigned int i = 0; i < 2; ++i) {
                unsigned int tire = axle * 2 + i;
                diff.angular_vel[i] = mTires[tire]->GetAngularVelocity();
                diff.has_traction[i] = mTires[tire]->IsOnGround();

                fwd_slip += center_diff.torque_split[axle] * mTires[tire]->GetCurrentSlip() * 0.5f;
                lat_slip += center_diff.torque_split[axle] * mTires[tire]->GetLateralSpeed() * 0.5f;
            }

            bool locked_diff = false;
            if ((mBurnOut.GetState() & 1) != 0) {
                traction_boost[1] = mBurnOut.GetTraction();
                diff.bias = mBurnOut.GetTraction() * 0.5f;
                locked_diff = true;
            } else if ((mBurnOut.GetState() & 2) != 0) {
                traction_boost[0] = mBurnOut.GetTraction();
                diff.bias = 1.0f - mBurnOut.GetTraction() * 0.5f;
                locked_diff = true;
            } else {
                float delta_lat_slip = lat_slip - state.local_vel.x;
                if (delta_lat_slip * state.steer_input > 0.0f && axle_torque * fwd_slip > 0.0f) {
                    float delta_fwd_slip = fwd_slip - state.local_vel.z;
                    float traction_control_limit = UMath::Ramp(delta_fwd_slip, 1.0f, 20.0f);

                    if (traction_control_limit > 0.0f) {
                        float traction_angle = UMath::Abs(state.steer_input * UMath::Atan2d(delta_lat_slip, UMath::Abs(delta_fwd_slip)));
                        traction_control_limit *= UMath::Ramp(traction_angle, 1.0f, 16.0f);
                        if (traction_control_limit > 0.0f) {
                            if (delta_lat_slip > 0.0f) {
                                traction_control[1] = 1.0f - traction_control_limit;
                                traction_control[0] = 1.0f - traction_control_limit * 0.5f;
                                traction_boost[0] = traction_control_limit + 1.0f;
                            } else {
                                traction_control[0] = 1.0f - traction_control_limit;
                                traction_control[1] = 1.0f - traction_control_limit * 0.5f;
                                traction_boost[1] = traction_control_limit + 1.0f;
                            }
                        }
                    }
                }
            }
            diff.CalcSplit(locked_diff);

            for (unsigned int i = 0; i < 2; ++i) {
                unsigned int tire = axle * 2 + i;
                if (mTires[tire]->IsOnGround()) {
                    mTires[tire]->ApplyDriveTorque(axle_torque * diff.torque_split[i] * traction_control[i]);
                    mTires[tire]->ScaleTractionBoost(traction_boost[i]);
                }
            }
        }
    }
}

static const float Tweak_DragYawSpeed = 1.6f;

// Credits: Brawltendo
// TODO stack frame is wrong
void SuspensionRacer::DoWheelForces(Chassis::State &state) {
    const float dT = state.time;

    UVector3 steerR;
    UVector3 steerL;
    DoSteering(state, steerR, steerL);
    TuneWheelParams(state);

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;

    const UMath::Vector3 &vFwd = state.GetForwardVector();
    const UMath::Vector3 &vUp = state.GetUpVector();

    const float mass = state.mass;

    float ride_extra = 0.0f;
    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    if (tunings) {
        ride_extra = tunings->Value[Physics::Tunings::RIDEHEIGHT];
    }

    float time = Sim::GetTime();
    float shock_specs[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float shock_ext_specs[2];
    float shock_valving[2];
    float shock_digression[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(mSuspensionInfo.SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - mSuspensionInfo.SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(mSuspensionInfo.SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(mSuspensionInfo.SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(mSuspensionInfo.TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(mSuspensionInfo.RIDE_HEIGHT().At(i) + ride_extra);
        progression[i] = mSuspensionInfo.SPRING_PROGRESSION().At(i);
    }

    float sway_stiffness[4];
    sway_stiffness[0] = (mTires[0]->GetCompression() - mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    sway_stiffness[2] = (mTires[2]->GetCompression() - mTires[3]->GetCompression()) * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    UMath::Vector4 steering_normals[4];
    steering_normals[0] = UMath::Vector4Make(steerL, 1.0f);
    steering_normals[1] = UMath::Vector4Make(steerR, 1.0f);
    steering_normals[2] = UMath::Vector4Make(vFwd, 1.0f);
    steering_normals[3] = UMath::Vector4Make(vFwd, 1.0f);

    bool resolve = false;
    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i / 2;
        Tire &wheel = GetWheel(i);
        UMath::Vector3 wp = wheel.GetWorldArm();
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, state.world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);
        const UVector3 groundNormal(wheel.GetNormal());
        const UVector3 forwardNormal(steering_normals[i]);
        UVector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        // how angled the wheel is relative to the ground
        float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + penetration;
        float max_compression = travel_specs[axle];
        if (wheel.GetCompression() == 0.0f) {
            maxDelta = UMath::Max(maxDelta, newCompression - max_compression);
        }
        newCompression = UMath::Max(newCompression, 0.0f);

        // handle the suspension bottoming out
        if (newCompression > max_compression) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
            // spring can't compress past the travel length
            newCompression = max_compression;
            wheel.SetBottomOutTime(time);
        }

        if (newCompression > 0.0f && upness > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            ++wheelsOnGround;

            const float diff = newCompression - wheel.GetCompression();
            float rise = diff / dT;

            float spring = (newCompression * spring_specs[axle]) * (newCompression * progression[axle] + 1.0f);
            if (shock_valving[axle] > FLOAT_EPSILON && shock_digression[axle] < 1.0f) {
                float abs_rise = UMath::Abs(rise);
                float valving = shock_valving[axle];
                if (abs_rise > valving) {
                    float digression = valving * UMath::Pow(abs_rise / valving, shock_digression[axle]);
                    rise = (rise > 0.0f) ? digression : -digression;
                }
            }

            float damp = rise > 0.0f ? rise * shock_specs[axle] : rise * shock_ext_specs[axle];

            if (damp > mSuspensionInfo.SHOCK_BLOWOUT() * 9.81f * mass) {
                damp = 0.0f;
            }

            float springForce = UMath::Max(damp + spring + sway_stiffness[i], 0.0f);

            UVector3 verticalForce(vUp * springForce);

            UVector3 driveForce;
            UVector3 lateralForce;
            UVector3 c;
            UMath::Cross(forwardNormal, groundNormal, c);
            UMath::Cross(c, forwardNormal, c);

            float d2 = UMath::Dot(c, groundNormal) * 4.0f - 3.0f;
            float load = UMath::Max(d2, 0.3f) * springForce;
            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            float traction_force = wheel.UpdateLoaded(xspeed, zspeed, state.speed, load, state.time);
            float max_traction = UMath::Abs(xspeed / dT * (0.25f * mass));
            lateralForce = lateralNormal * UMath::Clamp(traction_force, -max_traction, max_traction);

            UMath::Vector3 force;
            UMath::UnitCross(lateralNormal, groundNormal, driveForce);
            UMath::Scale(driveForce, wheel.GetLongitudeForce(), driveForce);
            UMath::Add(lateralForce, driveForce, force);
            UMath::Add(force, verticalForce, force);

            wheel.SetForce(force);
            resolve = true;
        } else {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(dT);
        }

        if (newCompression == 0.0f) {
            wheel.IncAirTime(dT);
        } else {
            wheel.SetAirTime(0.0f);
        }
        wheel.SetCompression(newCompression);
    }

    if (resolve) {
        UMath::Vector3 total_torque = UMath::Vector3::kZero;
        UMath::Vector3 total_force = UMath::Vector3::kZero;

        for (unsigned int i = 0; i < GetNumWheels(); ++i) {
            Tire &wheel = GetWheel(i);
            UVector3 p(wheel.GetLocalArm());
            p.y += wheel.GetCompression();
            p.y -= rideheight_specs[i / 2u];
            const UMath::Vector3 &force = wheel.GetForce();
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, state.world_cog, r);
            UMath::Sub(r, state.GetPosition(), r);
            UMath::Cross(r, force, torque);
            UMath::Add(total_force, force, total_force);
            UMath::Add(total_torque, torque, total_torque);
        }

        float yaw = UMath::Dot(UMath::Vector4To3(state.matrix.v1), total_torque);
        float counter_yaw = yaw * mTireInfo.YAW_SPEED();
        if (state.driver_style == STYLE_DRAG) {
            counter_yaw *= Tweak_DragYawSpeed;
        }
        UMath::ScaleAdd(UMath::Vector4To3(state.matrix.v1), counter_yaw - yaw, total_torque, total_torque);
        mRB->Resolve(total_force, total_torque);
    }

    if (maxDelta > 0.0f) {
        for (int i = 0; i < GetNumWheels(); ++i) {
            Wheel &wheel = GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + maxDelta);
        }
        mRB->ModifyYPos(maxDelta);
    }

    if (wheelsOnGround != 0 && !mNumWheelsOnGround) {
        state.local_angular_vel.z *= 0.5f;
        state.local_angular_vel.y *= 0.5f;
        UMath::Rotate(state.local_angular_vel, state.matrix, state.angular_vel);
        mRB->SetAngularVelocity(state.angular_vel);
    }
    mNumWheelsOnGround = wheelsOnGround;
}

void SuspensionRacer::OnDebugDraw() {}
