#include "Speed/Indep/Src/Physics/Behaviors/SuspensionRacer.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include <cfloat>
#include <cmath>

float ZeroDegreeTable[6] = {0.0f};
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

int *pLatForceMultipliers = NULL;
Table *LoadSensitivityTable[] = {&ZeroDegree, &TwoDegree, &FourDegree, &SixDegree, &EightDegree, &TenDegree, &TwelveDegree};

// only the first steering remap table is actually used, the rest are HP2/UG1/UG2 leftovers

static float JoystickInputToSteerRemap1[] = {-1.0f,  -0.712f, -0.453f, -0.303f, -0.216f, -0.148f, -0.116f, -0.08f, -0.061f, -0.034f, 0.0f,
                                             0.034f, 0.061f,  0.08f,   0.116f,  0.148f,  0.216f,  0.303f,  0.453f, 0.712f,  1.0f};
static float JoystickInputToSteerRemap2[] = {-1.0f,  -0.736f, -0.542f, -0.4f, -0.292f, -0.214f, -0.16f, -0.123f, -0.078f, -0.036f, 0.0f,
                                             0.036f, 0.078f,  0.123f,  0.16f, 0.214f,  0.292f,  0.4f,   0.542f,  0.736f,  1.0f};
static float JoystickInputToSteerRemap3[] = {-1.0f,  -0.8f,  -0.615f, -0.483f, -0.388f, -0.288f, -0.22f, -0.161f, -0.111f, -0.057f, 0.0f,
                                             0.057f, 0.111f, 0.161f,  0.22f,   0.288f,  0.388f,  0.483f, 0.615f,  0.8f,    1.0f};
static float JoystickInputToSteerRemapDrift[] = {-1.0f, -1.0f,  -0.688f, -0.492f, -0.319f, -0.228f, -0.16f, -0.123f, -0.085f, -0.05f, 0.0f,
                                                 0.05f, 0.085f, 0.123f,  0.16f,   0.228f,  0.319f,  0.492f, 0.688f,  1.0f,    1.0f};
static Table SteerInputRemapTables[] = {Table(JoystickInputToSteerRemap1, 21, -1.0f, 1.0f), Table(JoystickInputToSteerRemap2, 21, -1.0f, 1.0f),
                                        Table(JoystickInputToSteerRemap3, 21, -1.0f, 1.0f), Table(JoystickInputToSteerRemapDrift, 21, -1.0f, 1.0f)};

// UNSOLVED but functionally matching
SuspensionRacer::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(1), mRadius(UMath::Max(radius, 0.1f)), mWheelIndex(index), mAxleIndex(index >> 1), mSpecs(specs), mBrakes(brakes) {
    mBrake = 0.0f;
    mEBrake = 0.0f;
    mAV = 0.0f;
    mLoad = 0.0f;
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
    mDriveTorque = 0.0f;
    mBrakeTorque = 0.0f;
    mLateralBoost = 1.0f;
    mTractionBoost = 1.0f;
    mSlip = 0.0f;
    mLastTorque = 0.0f;
    mRoadSpeed = 0.0f;
    mAngularAcc = 0.0f;
    mTraction = 1.0f;
    mBottomOutTime = 0.0f;
    mSlipAngle = 0.0f;
    mTractionCircle = UMath::Vector2Make(1.0f, 1.0f);
    mMaxSlip = 0.5f;
    mGripBoost = 1.0f;
    mDriftFriction = 1.0f;
    mLateralSpeed = 0.0f;
    mBrakeLocked = false;
    mLastSign = SuspensionRacer::Tire::WAS_ZERO;
    mDragReduction = 0.0f;
}

// UNSOLVED but functionally matching
void SuspensionRacer::Tire::BeginFrame(float max_slip, float grip_boost, float traction_boost, float drag_reduction) {
    mMaxSlip = max_slip;
    mDriveTorque = 0.0f;
    mBrakeTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mTractionCircle = UMath::Vector2Make(1.0f, 1.0f);
    mLongitudeForce = 0.0f;
    mTractionBoost = traction_boost;
    mGripBoost = grip_boost;
    mDriftFriction = 1.0f;
    mLateralForce = 0.0f;
    mDragReduction = drag_reduction;
}

void SuspensionRacer::Tire::EndFrame(float dT) {}

// Credits: Brawltendo
// UNSOLVED
Newtons SuspensionRacer::Tire::ComputeLateralForce(float load, float slip_angle) {
    float angle = ANGLE2DEG(slip_angle);
    float norm_angle = angle * 0.5f;
    // there are 3 instructions here that are out of order but everything else matches
    // Black Box what did you do???????
    // why??????
    int slip_angle_table = (int)norm_angle;
    load *= 0.001f;
    load *= 0.8f;

    if (slip_angle_table >= 6) {
        float grip_scale = mSpecs->GRIP_SCALE().At(mAxleIndex);
        return (((LoadSensitivityTable[6]->GetValue(load) * pLatForceMultipliers[2]) * mGripBoost) * grip_scale) * 2500.0f;
    } else {
        float low = LoadSensitivityTable[slip_angle_table]->GetValue(load);
        float high = LoadSensitivityTable[slip_angle_table + 1]->GetValue(load);
        float grip_scale = mSpecs->GRIP_SCALE().At(mAxleIndex);
        float delta = norm_angle - slip_angle_table;
        return ((((delta * (high - low) + low) * grip_scale) * pLatForceMultipliers[2]) * mGripBoost) * 2500.0f;
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
// UNSOLVED
void SuspensionRacer::Tire::CheckForBrakeLock(float ground_force) {
    float brake_force = ((FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * mBrakes->BRAKE_LOCK().At(mAxleIndex)) * BrakingTorque) * mBrake;
    static float StaticToDynamicBrakeForceRatio = 1.2f;
    static float BrakeLockAngularVelocityFactor = 100.0f;

    brake_force += (FTLB2NM(mBrakes->EBRAKE()) * EBrakingTorque) * mEBrake;
    brake_force *= StaticToDynamicBrakeForceRatio;

    float radius = mRadius;
    float abs_av = mAV;
    UMath::Abs(abs_av);
    if (brake_force > BrakeLockAngularVelocityFactor * abs_av + radius * ground_force) {
        if (brake_force > 1.0f) {
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
// UNSOLVED the stack is too large
void SuspensionRacer::Tire::CheckSign() {
    // this function is called but doesn't actually seem to be used for anything other than debugging
    // mLastSign isn't used anywhere outside of this function as far as I know

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

// Credits: Brawltendo
// Updates forces for an unloaded/airborne tire
// UNSOLVED regswaps
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
        const float brake_spec = FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * BrakingTorque * mBrake;
        const float ebrake_spec = FTLB2NM(mBrakes->EBRAKE()) * EBrakingTorque * mEBrake;
        float bt = mAV > 0.0f ? -brake_spec : brake_spec;
        ApplyBrakeTorque(bt);
        float ebt = mAV > 0.0f ? -ebrake_spec : ebrake_spec;
        ApplyBrakeTorque(ebt);

        mAngularAcc = GetTotalTorque() / WheelMomentOfInertia;
        mAV += mAngularAcc * dT;
    }
    CheckSign();
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

// Credits: Brawltendo
// UNSOLVED
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
        // TODO huh? asm diff
        steer_input = SteerInputRemapTables[0].GetValue(steer_input);
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

    // if in speedbreaker, increase the current steering angle beyond the normal maximum
    // this change is instant, so the visual steering angle while in speedbreaker doesn't accurately represent this
    // instead it interpolates to this value so it looks nicer
    if (mGameBreaker > 0.0f) {
        newsteer = UMath::Lerp(newsteer, state.steer_input * 60.0f, mGameBreaker);
    }

    mSteering.InputAverage.Record(mSteering.LastInput, Sim::GetTime());
    return DEG2ANGLE(newsteer);
}

float BrakeSteeringRangeMultiplier = 1.45f;
float SteeringRangeData[] = {40.0f, 20.0f, 10.0f, 5.5f, 4.5f, 3.25f, 2.9f, 2.9f, 2.9f, 2.9f};
float SteeringRangeCoeffData[] = {1.0f, 1.0f, 1.1f, 1.2f, 1.25f, 1.35f};
float SteeringSpeedData[] = {1.0f, 1.0f, 1.0f, 0.56f, 0.5f, 0.35f, 0.3f, 0.3f, 0.3f, 0.3f};
float SteeringWheelRangeData[] = {45.0f, 15.0f, 11.0f, 8.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f};
Table SteeringRangeTable = Table(SteeringRangeData, 10, 0.0f, 160.0f);
Table SteeringWheelRangeTable = Table(SteeringWheelRangeData, 10, 0.0f, 160.0f);
Table SteeringRangeCoeffTable = Table(SteeringRangeCoeffData, 6, 0.0f, 1.0f);
Table SteeringSpeedTable = Table(SteeringSpeedData, 10, 0.0f, 160.0f);
bVector2 PostCollisionSteerReductionData[] = {bVector2(0.0f, 0.2f), bVector2(0.2f, 0.5f), bVector2(0.5f, 0.7f), bVector2(0.7f, 1.0f)};
Graph PostCollisionSteerReductionTable(PostCollisionSteerReductionData, 4);

#define MAX_STEERING_ANGLE (45.0f)

// Credits: Brawltendo
// UNSOLVED
float SuspensionRacer::CalculateMaxSteering(Chassis::State &state, ISteeringWheel::SteeringType steer_type) {
    const float steer_input = state.steer_input;

    // max possible steering output scales with the car's forward speed
    float max_steering = SteeringRangeTable.GetValue(state.local_vel.z);
    // there are 2 racing wheel input types, one scales with speed and the other doesn't
    if (steer_type == ISteeringWheel::kWheelSpeedSensitive) {
        max_steering = SteeringWheelRangeTable.GetValue(state.local_vel.z);
    } else if (steer_type == ISteeringWheel::kWheelSpeedInsensitive) {
        return MAX_STEERING_ANGLE;
    }

    /*
     *  manipulate the max steering range based on the throttle, brake, and handbrake inputs
     *  pressing only the throttle will give the regular steering range
     *  throttle + a brake input will boost the steering range by 25% of BrakeSteeringRangeMultiplier
     *  throttle + both brake inputs will boost the steering range by 50% of BrakeSteeringRangeMultiplier
     *  a single brake input will boost the steering range by 75% of BrakeSteeringRangeMultiplier
     *  both brake inputs will boost the steering range by 100% of BrakeSteeringRangeMultiplier
     */
    float tbcoeff = 1.0f - (state.gas_input + 1.0f - (state.brake_input + state.ebrake_input) * 0.5f) * 0.5f;
    max_steering *= BrakeSteeringRangeMultiplier * tbcoeff * SteeringSpeedTable.GetValue(state.local_vel.z) + 1.0f;
    max_steering *= SteeringRangeCoeffTable.GetValue(bAbs(mSteering.InputAverage.GetValue()));

    const Physics::Tunings *tunings = GetVehicle()->GetTunings();
    // steering tuning allows for up to a 20% increase or decrease to the max steering range
    if (tunings) {
        max_steering *= tunings->Value[Physics::Tunings::STEERING] * 0.2f + 1.0f;
    }

    // reduce steering range after a collision
    if (mSteering.CollisionTimer > 0.0f) {
        // steering range reduction lasts for one second
        float secs = 1.0f - mSteering.CollisionTimer;
        if (secs < 1.0f && secs > 0.0f) {
            float speed_coeff = bMin(1.0f, state.local_vel.z / (MPH2MPS(170.0f) * 0.7f));
            speed_coeff = (1.0f - speed_coeff); // only matches with parentheses
            float collision_coeff = PostCollisionSteerReductionTable.GetValue(secs);
            max_steering *= speed_coeff * (1.0f - collision_coeff) + collision_coeff;
        }
    }

    // when steering, the max possible steering range can't go below the slip angle of the rear wheel on the inside of the turn
    float yaw_left = ANGLE2DEG(mTires[2]->GetSlipAngle());
    float yaw_right = ANGLE2DEG(mTires[3]->GetSlipAngle());
    // steering right
    if (steer_input > 0.0f && yaw_right > 0.0f) {
        // TODO
        max_steering = UMath::Min(UMath::Max(max_steering, MAX_STEERING_ANGLE), yaw_right);
    }
    // steering left
    else if (steer_input < 0.0f && yaw_left < 0.0f) {
        // TODO
        max_steering = UMath::Min(UMath::Max(max_steering, MAX_STEERING_ANGLE), -yaw_left);
    } else if (bAbs(mSteering.InputAverage.GetValue()) >= 0.5f) {
        // NOTE: multiplying parts of equations by zero to nullify them is a recurring Black Box coding style quirk
        // I have no idea why they did it that way instead of just commenting them out

        // don't let the max steering range go below the previous frame's value
        max_steering = bMax(max_steering, mSteering.LastMaximum - state.time * 0.0f);
    }

    // this is kinda pointless for the first 2 checks since they already clamp to MAX_STEERING_ANGLE
    max_steering = bMin(max_steering, MAX_STEERING_ANGLE);
    mSteering.LastMaximum = max_steering;
    return max_steering;
}

float SteeringInputSpeedData[] = {1.0f, 1.05f, 1.1f, 1.5f, 2.2f, 3.1f};
float SteeringInputData[] = {1.0f, 1.05f, 1.1f, 1.2f, 1.3f, 1.4f};
Table SteeringInputSpeedCoeffTable = Table(SteeringInputSpeedData, 6, 0.0f, 10.0f);
Table SteeringInputCoeffTable = Table(SteeringInputData, 6, 0.0f, 1.0f);

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

float BurnOutCancelSlipValue = 0.5f;
float BurnOutYawCancel = 0.5f;
float BurnOutAllowTime = 1.0f;
float BurnOutMaxSpeed = 20.0f;
float BurnOutFishTailTime = 2.0f;
int BurnOutFishTails = 6;
GraphEntry<float> BurnoutFrictionData[] = {{0.0f, 1.0f}, {5.0f, 0.8f}, {9.0f, 0.9f}, {12.6f, 0.833f}, {17.1f, 0.72f}, {25.0f, 0.65f}};
tGraph<float> BurnoutFrictionTable(BurnoutFrictionData, 6);

// Credits: Brawltendo
// UNSOLVED
void SuspensionRacer::Burnout::Update(const float dT, const float speedmph, const float max_slip, const int max_slip_wheel, const float yaw) {
    // continue burnout/fishtailing state and end when certain conditions are met
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
    else if (speedmph < BurnOutMaxSpeed && max_slip > 0.5f) {
        float burnout_coeff;
        // wrong
        BurnoutFrictionTable.GetValue(&burnout_coeff, max_slip);
        SetTraction(burnout_coeff / 1.4f);
        // burnout state changes according to what side of the axle the wheel that's slipping the most is on
        SetState((int)((1.5f - burnout_coeff) * BurnOutFishTails + (max_slip_wheel & 1)));
        SetBurnOutTime(BurnOutFishTailTime);
        ClearBurnOutAllow();
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
        // TODO
        UMath::Vector3 chg = {}; // {0.0f, UMath::Abs(wall) * dW, 0.0f};
        chg.y = UMath::Abs(wall) * dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(chg, state.angular_vel, chg);
        mRB->SetAngularVelocity(chg);
    }

    wall = mSteering.WallSideTurn;
    float dW = state.steer_input * state.gas_input;
    // side turn is only applied when in reverse and if touching a wall parallel to the car
    // it helps the player move their car away from the wall when backing up
    if (dW * wall > 0.0f && mNumWheelsOnGround > 2 && !state.gear) {
        dW *= -0.125f;
        // TODO
        UMath::Vector3 chg = {}; // = {0.0f, UMath::Abs(wall) * dW, 0.0f};
        chg.y = UMath::Abs(wall) * dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(chg, state.angular_vel, chg);
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
// UNSOLVED
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

        if (mTires[0]->IsOnGround() || (center_diff.has_traction[0] = 0, mTires[1]->IsOnGround())) {
            center_diff.has_traction[0] = 1;
        }
        if (mTires[2]->IsOnGround() || (center_diff.has_traction[1] = 0, mTires[3]->IsOnGround())) {
            center_diff.has_traction[1] = 1;
        }
        center_diff.CalcSplit(false);
    } else {
        center_diff.torque_split[0] = mTranyInfo.TORQUE_SPLIT();
        center_diff.torque_split[1] = 1.0f - center_diff.torque_split[0];
    }

    unsigned int axle = 0;
    // loop through 2 wheels at a time so we can easily deal with both wheels on the axle
    for (SuspensionRacer::Tire **tire = mTires; axle < 2; tire += 2) {
        float axle_torque = drive_torque * center_diff.torque_split[axle];
        if (UMath::Abs(axle_torque) > FLOAT_EPSILON) {
            SuspensionRacer::Differential diff;
            diff.bias = 0.5f;
            float traction_control[2] = {1.0f, 1.0f};
            float traction_boost[2] = {1.0f, 1.0f};
            diff.factor = mTranyInfo.DIFFERENTIAL(axle);
            diff.angular_vel[0] = tire[0]->GetAngularVelocity();
            diff.has_traction[0] = tire[0]->IsOnGround();

            float fwd_slip = tire[0]->GetCurrentSlip() * center_diff.torque_split[axle] * 0.5f;
            float lat_slip = tire[0]->GetLateralSpeed() * center_diff.torque_split[axle];

            diff.angular_vel[1] = tire[1]->GetAngularVelocity();
            lat_slip *= 0.5f;
            diff.has_traction[1] = tire[1]->IsOnGround();

            bool locked_diff = false;
            fwd_slip += tire[1]->GetCurrentSlip() * center_diff.torque_split[axle] * 0.5f;
            lat_slip += tire[1]->GetLateralSpeed() * center_diff.torque_split[axle] * 0.5f;

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
                if (delta_lat_slip * state.steer_input > 0.0f && fwd_slip * axle_torque > 0.0f) {
                    float delta_fwd_slip = fwd_slip - state.local_vel.z;
                    float traction_control_limit = UMath::Ramp(delta_fwd_slip, 1.0f, 20.0f);

                    if (traction_control_limit > 0.0f) {
                        float traction_angle = UMath::Atan2d(delta_lat_slip, UMath::Abs(delta_fwd_slip)) * state.steer_input;
                        traction_angle = UMath::Abs(traction_angle);
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

            if (tire[0]->IsOnGround()) {
                tire[0]->ApplyDriveTorque(diff.torque_split[0] * traction_control[0] * axle_torque);
                tire[0]->ScaleTractionBoost(traction_boost[0]);
            }
            if (tire[1]->IsOnGround()) {
                tire[1]->ApplyDriveTorque(diff.torque_split[1] * traction_control[1] * axle_torque);
                tire[1]->ScaleTractionBoost(traction_boost[1]);
            }
        }
        ++axle;
    }
}

float RollingFriction = 2.0f;
float TireForceEllipseRatio = 2.0 / 3.0f;

// Credits: Brawltendo
// UNSOLVED
float SuspensionRacer::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT) {
    float bt = (mBrakes->BRAKES().At(mAxleIndex) * 1.3558f) * BrakingTorque;
    float ebt = (mBrakes->EBRAKE() * 1.3558f) * EBrakingTorque;
    const float dynamicgrip_spec = mSpecs->DYNAMIC_GRIP().At(mAxleIndex);
    const float staticgrip_spec = mSpecs->STATIC_GRIP().At(mAxleIndex);

    if (mLoad <= 0.0f && !mBrakeLocked) {
        mAV = fwd_vel / mRadius;
    }
    float fwd_acc = (fwd_vel - mRoadSpeed) / dT;
    mRoadSpeed = fwd_vel;

    // float temp_load = UMath::Max(load, 0.0f);
    mLoad = UMath::Max(load, 0.0f);
    mLateralSpeed = lat_vel;
    // bt *= mBrake;
    // ebt *= mEBrake;
    float bt_add = bt * mBrake;
    float ebt_add = ebt * mEBrake;
    float abs_fwd = UMath::Abs(fwd_vel);
    if (abs_fwd < 1.0f) {
        // when car is nearly stopped, apply brake torque using forward velocity and wheel load
        float inv_radius = 1.0f / mRadius;
        float stopping_bt = -((inv_radius * mBrake) * fwd_vel * load);
        float stopping_ebt = -((inv_radius * mEBrake) * fwd_vel * load);
        float r_drive_torque = mEBrake;
        r_drive_torque *= -mDriveTorque;
        ApplyDriveTorque(r_drive_torque);
        ApplyBrakeTorque(stopping_bt);
        ApplyBrakeTorque(stopping_ebt);
    } else {
        ApplyBrakeTorque(mAV > 0.0f ? -bt_add : bt_add);
        ApplyBrakeTorque(mAV > 0.0f ? -ebt_add : ebt_add);
    }

    mSlipAngle = VU0_Atan2(lat_vel, abs_fwd);
    // float slip_speed = mAV;
    float slip_speed = mAV * mRadius - fwd_vel;
    float slip_ground_friction = 0.0f;
    float dynamic_friction = 1.0f;
    mSlip = slip_speed;
    float skid_speed = UMath::Sqrt(slip_speed * slip_speed + lat_vel * lat_vel);
    float pilot_factor = GetPilotFactor(body_speed);
    if (skid_speed > FLOAT_EPSILON && (lat_vel != 0.0f || fwd_vel != 0.0f)) {
        dynamic_friction = (mTractionBoost * pilot_factor) * dynamicgrip_spec;
        slip_ground_friction = (dynamic_friction / skid_speed) * mLoad;
        float vel_len = UMath::Sqrt(fwd_vel * fwd_vel + lat_vel * lat_vel);
        // float wheel_speed = mLoad / UMath::Sqrt(fwd_vel * fwd_vel + lat_vel * lat_vel);
        // float ground_speed = ((mLoad / vel_len) * dynamic_friction) * abs_fwd;
        CheckForBrakeLock(((mLoad / vel_len) * dynamic_friction) * abs_fwd);
    }

    if (mTraction < 1.0f || mBrakeLocked) {
        // float long_force;// = slip_speed * slip_ground_friction;
        mLongitudeForce = slip_ground_friction;
        mLongitudeForce *= slip_speed;
        mLateralForce = -(slip_ground_friction * lat_vel);

        // 0.44703 mps = 1 mph
        const float one_mph = 0.44703f;
        if (body_speed < one_mph && dynamic_friction > 0.1f) {
            mLateralForce /= dynamic_friction;
            mLongitudeForce /= dynamic_friction;
        }
        mLongitudeForce = UMath::Limit(mLongitudeForce, GetTotalTorque() / mRadius);
    } else {
        mBrakeLocked = false;
        mLongitudeForce = GetTotalTorque() / mRadius;
        float slip_ang = mSlipAngle;
        // float temp_load = mLoad;
        float lat_force = ComputeLateralForce(mLoad, UMath::Abs(slip_ang)); // < 0.0f ? -slip_ang : slip_ang);
        mLateralForce = lat_force;
        if (lat_vel > 0.0f) {
            mLateralForce = -lat_force;
        }
    }

    mLateralForce *= mLateralBoost;
    if (mTraction >= 1.0f && !mBrakeLocked) {
        mLongitudeForce += (mAngularAcc * mRadius - fwd_acc) / mRadius * WheelMomentOfInertia;
    }

    float combined_torque = GetTotalTorque();
    bool use_ellipse = false;
    // combined_torque *= fwd_vel;
    if (combined_torque * fwd_vel > 0.0f && !mBrakeLocked) {
        use_ellipse = true;
        mLongitudeForce *= TireForceEllipseRatio;
    }

    // float ellipse_x = mLateralForce * mTractionCircle.x;
    mLateralForce *= mTractionCircle.x;
    // float ellipse_y = mLongitudeForce * mTractionCircle.y;
    mLongitudeForce *= mTractionCircle.y;
    // float len_force = UMath::Sqrt(mLongitudeForce * mLongitudeForce + mLateralForce * mLateralForce);
    float len_force = mLongitudeForce * mLongitudeForce + mLateralForce * mLateralForce;
    len_force = UMath::Sqrt(len_force);
    mTraction = 1.0f;
    float traction_scale = ((mDriftFriction * mTractionBoost) * staticgrip_spec) * mLoad * pilot_factor;
    float tolerated_slip = mMaxSlip;
    if (len_force > traction_scale && len_force > 0.001f) {
        float ratio = traction_scale / len_force;
        mTraction = ratio;
        mLateralForce *= ratio;
        mLongitudeForce *= ratio;
        tolerated_slip = (ratio * ratio) * tolerated_slip;
    } else if (use_ellipse) {
        mLongitudeForce *= TireForceEllipseRatio;
    }

    if (UMath::Abs(slip_speed) > tolerated_slip) {
        mTraction /= UMath::Abs(slip_speed);
        mTraction *= tolerated_slip;
    }

    // factor surface friction into the tire force
    mLateralForce *= mSurface.LATERAL_GRIP();
    mLongitudeForce *= mSurface.DRIVE_GRIP();
    if (fwd_vel > 1.0f) {
        mLongitudeForce -= UMath::Sina(mSlipAngle * (float)M_TWOPI) * (mDragReduction / mSpecs->GRIP_SCALE().At(mAxleIndex)) * mLateralForce;

    } else {
        float abs_lat_slip = UMath::Min(UMath::Abs(lat_vel), 1.0f);
        mLateralForce *= abs_lat_slip;
    }

    if (mBrakeLocked) {
        mAngularAcc = 0.0f;

    } else {
        if (mTraction < 1.0f) {
            float long_force = mLongitudeForce;
            float last_torque = (GetTotalTorque() - mLongitudeForce * mRadius + mLastTorque) * 0.5f;
            mLastTorque = last_torque;
            mAngularAcc =
                (last_torque - (RollingFriction * mSurface.ROLLING_RESISTANCE()) * mAV) / WheelMomentOfInertia - (mTraction * mSlip) / (dT * mRadius);
        }
        // pointless temp var so that this can match
        float ang_acc = mAngularAcc;
        mAngularAcc = ((fwd_acc / mRadius) - ang_acc) * mTraction + ang_acc;
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
            float newCompression = GetRideHeight(i) * upness + wheel.GetNormal().w;
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

void SuspensionRacer::OnCollision(const Sim::Collision::Info &cinfo) {
    // TODO fix this -1U
    if (cinfo.type - 1U < Sim::Collision::Info::WORLD) {
        float impulse = !cinfo.objAImmobile ? cinfo.impulseA : 0.0f;
        if (cinfo.objB == GetOwner()->GetInstanceHandle()) {
            impulse = !cinfo.objBImmobile ? cinfo.impulseB : 0.0f;
        }
        if (impulse > 10.0f) {
            float damper = UMath::Ramp(impulse, 10.0f, 40.0f);
            mSteering.CollisionTimer = UMath::Max(damper, mSteering.CollisionTimer);
        }
    }
    if (cinfo.type == Sim::Collision::Info::GROUND) {
        mLastGroundCollision = Sim::GetTime();
    }
    if (cinfo.type == Sim::Collision::Info::WORLD) {
        if (UMath::Abs(cinfo.normal.y) < 0.1f && this->mCollisionBody && (UMath::LengthSquare(cinfo.closingVel) < 56.25f)) {
            const UMath::Vector3 &vFoward = mCollisionBody->GetForwardVector();
            const UMath::Vector3 &vRight = mCollisionBody->GetRightVector();
            if ((mSteering.WallNoseTurn == 0.0f) && UMath::LengthSquare(cinfo.objAVel) < 6.25f && UMath::Dot(cinfo.normal, vFoward) <= 0.0f) {
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
// UNSOLVED
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
            // TODO
            ratio -= index1;
            unsigned int index2 = UMath::Min(index1 + 1, numunits - 1);
            float a = mTireInfo.YAW_CONTROL(index1);
            float b = mTireInfo.YAW_CONTROL(index2);
            return (b - a) * ratio + a;
        }
    }
    return mTireInfo.YAW_CONTROL(0);
}

float DriftRearFrictionData[] = {1.1f, 0.95f, 0.87f, 0.77f, 0.67f, 0.6f, 0.51f, 0.43f, 0.37f, 0.34f};
Table DriftRearFrictionTable(DriftRearFrictionData, 10, 0.0f, 1.0f);
GraphEntry<float> DriftStabilizerData[] = {{0.0f, 0.0f},        {0.2617994f, 0.1f},  {0.52359879f, 0.45f}, {0.78539819f, 0.85f},
                                           {1.0471976f, 0.95f}, {1.5533431f, 1.15f}, {1.5707964f, 0.0f}};
tGraph<float> DriftStabilizerTable(DriftStabilizerData, 7);

// Credits: Brawltendo
void SuspensionRacer::DoDrifting(const Chassis::State &state) {
    if (mDrift.State && ((state.flags & 1) || state.driver_style == STYLE_DRAG)) {
        mDrift.Reset();
        return;
    }

    float drift_change = 0.0f;
    switch (mDrift.State) {
        case SuspensionRacer::Drift::D_OUT:
        case SuspensionRacer::Drift::D_EXIT:
            // the drift value will decrement by (dT * 2) when not drifting or exiting a drift
            drift_change = -2.0f;
            break;
        case SuspensionRacer::Drift::D_ENTER:
        case SuspensionRacer::Drift::D_IN:
            // the drift value will increment by (dT * 8) when entering and holding a drift
            drift_change = 8.0f;
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
        if ((state.local_angular_vel.y * state.slipangle) < 0.0f && UMath::Abs(state.slipangle) <= 0.25f && state.speed > MPH2MPS(30.00005f) &&
            (avg_steer * state.slipangle) <= 0.0f && UMath::Abs(state.slipangle) > DEG2ANGLE(12.0f)) {
            mDrift.State = SuspensionRacer::Drift::D_IN;
        } else if ((state.steer_input * state.slipangle) * state.gas_input > DEG2ANGLE(12.0f) && state.speed > MPH2MPS(30.00005f)) {
            mDrift.State = SuspensionRacer::Drift::D_IN;
        } else if (!((UMath::Abs(state.slipangle) * state.gas_input) > DEG2ANGLE(12.0f))) {
            mDrift.State = SuspensionRacer::Drift::D_EXIT;
        } else {
            mDrift.State = SuspensionRacer::Drift::D_ENTER;
        }
    } else if (state.speed > MPH2MPS(30.00005f) && (state.ebrake_input > 0.5f || UMath::Abs(state.slipangle) > DEG2ANGLE(12.0f))) {
        mDrift.State = SuspensionRacer::Drift::D_ENTER;
    }

    if (!(mDrift.Value <= 0.0f)) {
        float yaw = state.local_angular_vel.y;
        // chassis slip angle is stored as a value in the range [-1,1]
        // so multiplying by 2pi gives the entire possible angle range in radians
        float slipangle_radians = state.slipangle * (float)M_TWOPI;

        // charge speedbreaker if not in use and drifting is detected
        if (mGameBreaker <= 0.0f && state.speed > MPH2MPS(35.000058f) && UMath::Abs(slipangle_radians) > DEG2RAD(30.0f)) // 0.52358997f)
        {
            IPlayer *player = GetOwner()->GetPlayer();
            if (player) {
                player->ChargeGameBreaker(state.time * 0.5f * mDrift.Value);
            }
        }

        // apply yaw damping torque
        if ((yaw * slipangle_radians) < 0.0f && mNumWheelsOnGround >= 2) {
            float damping;
            // TODO
            DriftStabilizerTable.GetValue(&damping, UMath::Abs(slipangle_radians));
            float yaw_coef = state.inertia.y * mDrift.Value * damping * yaw * -4.0f;
            UMath::Vector3 moment;
            // multiply up vector by yaw coefficient to get the final amount of damping to apply
            UMath::Scale(state.GetUpVector(), yaw_coef, moment);
            mRB->ResolveTorque(moment);
        }

        // detect counter steering
        float countersteer = 0.0f;
        if ((slipangle_radians * state.steer_input) > 0.0f)
            countersteer = UMath::Abs(state.steer_input);

        float abs_slipangle = UMath::Abs(slipangle_radians);
        float abs_yaw = UMath::Abs(yaw);
        float driftmult_rear = DriftRearFrictionTable.GetValue(((abs_yaw + abs_slipangle) * 0.5f + countersteer * 4.0f) * mDrift.Value);
        mTires[2]->SetDriftFriction(driftmult_rear);
        mTires[3]->SetDriftFriction(driftmult_rear);
    }
}

float EBrakeYawControlMin = 0.5f;
float EBrakeYawControlOnSpeed = 1.0f;
float EBrakeYawControlOffSpeed = 20.0f;
float EBrake180Yaw = 0.3f;
float EBrake180Speed = 80.0f;

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

float ENABLE_ROLL_STOPS_THRESHOLD = 0.2f;

// Credits: Brawltendo
// UNSOLVED
void SuspensionRacer::DoWheelForces(Chassis::State &state) {
    const float dT = state.time;

    UVector3 steerR(0.0f);
    UVector3 steerL(0.0f);
    DoSteering(state, steerR, steerL);
    TuneWheelParams(state);

    UMath::Vector4 vUp;
    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
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
    // the compiler unrolls this loop
    // how fun
    for (int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(mSuspensionInfo.SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - mSuspensionInfo.SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(mSuspensionInfo.SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(mSuspensionInfo.SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(mSuspensionInfo.TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(mSuspensionInfo.RIDE_HEIGHT().At(i)) + INCH2METERS(ride_extra);
        progression[i] = mSuspensionInfo.SPRING_PROGRESSION().At(i);
    }

    // UMath::Vector3 vFwd;
    float sway_stiffness[4];
    UMath::Vector4 steering_normals[4];
    sway_stiffness[0] = (mTires[0]->GetCompression() - mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    float delta = (mTires[2]->GetCompression() - mTires[3]->GetCompression());
    // vFwd = steerL;
    // vFwd.w = 1.0f;
    bool resolve = false;
    sway_stiffness[2] = delta * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    steering_normals[0] = UMath::Vector4Make(steerL, 1.0f);

    // vFwd = steerR;
    // vFwd.w = 1.0f;
    steering_normals[1] = UMath::Vector4Make(steerR, 1.0f);

    // vFwd = state.GetForwardVector();
    // vFwd.w = 1.0f;
    steering_normals[2] = UMath::Vector4Make(state.GetForwardVector(), 1.0f);

    // vFwd = state.GetForwardVector();
    // vFwd.w = 1.0f;
    steering_normals[3] = UMath::Vector4Make(state.GetForwardVector(), 1.0f);

    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i >> 1;
        Tire &wheel = GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, state.world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);
        const UVector3 groundNormal(wheel.GetNormal());
        const UVector3 forwardNormal(steering_normals[i]);
        UVector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        // how angled the wheel is relative to the ground
        // TODO GetUpVector is only called once in the dwarf
        float upness = UMath::Clamp(UMath::Dot(groundNormal, state.GetUpVector()), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = upness * rideheight_specs[axle] + penetration;
        float max_compression = travel_specs[axle];
        if (oldCompression == 0.0f) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
        }
        float clamped_comp;
        // newCompression = UMath::Max(newCompression, 0.0f);
        if (newCompression > 0.0f) {
            clamped_comp = newCompression;
        } else {
            clamped_comp = 0.0f;
        }
        newCompression = clamped_comp;

        // handle the suspension bottoming out
        if (newCompression > max_compression) {
            float delta = newCompression - max_compression;
            // maxDelta = UMath::Max(maxDelta, delta);
            if (maxDelta > delta) {
                delta = maxDelta;
            }
            maxDelta = delta;
            // suspension can't compress past the max travel length, so clamp it here
            newCompression = max_compression;
            wheel.SetBottomOutTime(time);
        }

        if (newCompression > 0.0f && upness > ENABLE_ROLL_STOPS_THRESHOLD) {
            ++wheelsOnGround;
            const float diff = (newCompression - wheel.GetCompression());
            float rise = diff / dT;
            float spring = (newCompression * progression[axle] + 1.0f) * spring_specs[axle] * newCompression;
            if (shock_valving[axle] > FLOAT_EPSILON && shock_digression[axle] < 1.0f) {
                float abs_rise = UMath::Abs(rise);
                float valving = shock_valving[axle];
                max_compression = abs_rise;
                if (abs_rise > valving) {
                    float digression = powf(max_compression / valving, shock_digression[axle]) * valving;
                    if (rise > 0.0f) {
                        rise = digression;
                    } else {
                        rise = -digression;
                    }
                }
            }

            float damp = rise > 0.0f ? shock_specs[axle] : shock_ext_specs[axle];
            damp *= rise;
            if (damp > mass * mSuspensionInfo.SHOCK_BLOWOUT() * 9.81f) {
                damp = 0.0f;
            }
            float springForce = damp + sway_stiffness[i] + spring;
            springForce = UMath::Max(springForce, 0.0f);

            UVector3 verticalForce = state.GetUpVector();

            // UMath::Vector3 up = (UMath::Vector3&)state.GetUpVector() * springForce;
            verticalForce = verticalForce * springForce;
            UVector3 driveForce;
            UVector3 lateralForce;
            UVector3 c;
            UMath::Cross(forwardNormal, groundNormal, c);
            UMath::Cross(c, forwardNormal, c);

            float d2 = UMath::Dot(c, groundNormal);
            float load = springForce * UMath::Max(d2 * 4.0f - 3.0f, 0.3f);
            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            float traction_force = wheel.UpdateLoaded(xspeed, zspeed, state.speed, load, state.time);
            float max_traction = (xspeed / dT) * 0.25f * mass;
            traction_force = UMath::Clamp(traction_force, max_traction, UMath::Abs(max_traction));
            lateralForce = lateralNormal * traction_force;

            UMath::Vector3 force;
            UMath::UnitCross(lateralNormal, groundNormal, force);
            UMath::Scale(force, wheel.GetLongitudeForce(), driveForce);
            UMath::Add(force, driveForce, force);
            UMath::Add(force, lateralForce, force);
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
            wheel.IncAirTime(0.0f);
        }
        wheel.SetCompression(UMath::Max(newCompression, 0.0f));
    }

    if (resolve) {
        UMath::Vector3 total_torque = UMath::Vector3::kZero;
        UMath::Vector3 total_force = UMath::Vector3::kZero;

        for (unsigned int i = 0; i < GetNumWheels(); ++i) {
            Tire &wheel = GetWheel(i);
            UVector3 p(wheel.GetLocalArm());
            p.y += wheel.GetCompression() - rideheight_specs[i >> 1];
            const UMath::Vector3 &force = wheel.GetForce();
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);
            UMath::Vector3 torque;
            UMath::Vector3 r; // TODO
            // TODO
            UMath::Sub(p, state.world_cog, r);
            UMath::Cross(r, force, torque);
            UMath::Add(total_force, force, total_force);
            UMath::Add(total_torque, torque, total_torque);
        }

        float yaw = UMath::Dot(total_torque, state.GetUpVector());
        float counter_yaw = yaw * mTireInfo.YAW_SPEED();
        if (state.driver_style == STYLE_DRAG) {
            counter_yaw *= 1.6f;
        }
        counter_yaw -= yaw;
        UMath::ScaleAdd(state.GetUpVector(), counter_yaw, total_torque, total_torque);
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
