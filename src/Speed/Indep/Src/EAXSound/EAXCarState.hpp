#ifndef EAX_CAR_STATE_HPP
#define EAX_CAR_STATE_HPP

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/CarTypes.hpp"
#include "Speed/Indep/Src/Physics/CarBasics.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/Player.hpp"
#include "Speed/Indep/Src/World/WorldTypes.h"

#define NUM_UPGRADE_LEVELS 4

// total size: 0x248

class EAX_CarState : public UTL::Collections::Listable<EAX_CarState, 10> {
  public:
    static EAX_CarState *Find(WUID objectid) {}

    EAX_CarState(const Attrib::Collection *atr, Sound::Context context, WUID wuid, HSIMABLE handle)
        : mVel0(0.0f, 0.0f, 0.0f),         //
          mRacePos(0),                     //
          mBrake(0.0f),                    //
          mEBrake(0.0f),                   //
          mNosEmptyFlag(false),            //
          mMovementMode(PHYSICS_MOVEMENT), //
          mPlayerZone(PLAYER_ZONE_NONE),   //
          mSteering(0),                    //
          mAngle(0),                       //
          mSirenState(Sound::SIREN_OFF),   //
          mHotPursuit(false),              //
          mAttributes(atr, 0, nullptr),    //
          mWorldID(wuid),                  //
          mEngineInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) {
        static int PlayerUpgrade;
        float max_torque_rpm;

        mContext = context;
        mSimUpdating = true;
        mHandle = handle;
        mTrailerID = 0;
        mNISCarID = -1;
        mAssetsLoaded = false;
        mControlSource = Sound::CONTROL_AI;
        mDesiredSpeed = 0.0f;
        mVel1 = mVel0;
        mOversteer = 0.0f;
        mUndersteer = 0.0f;
        mSlipAngle = 0.0f;
        mVisualRPM = 0.0f;
        // TODO do the rest
    }

    ~EAX_CarState() {}

    EAX_CarState *GetState() {
        return this;
    }
    EAX_CarState *GetDriver() {
        return this;
    }

    float GetMaxTurboTorque() {}

    float GetMaxEngineTorque() {
        return this->mMaxTorque;
    }

    int GetUniqueDriverNumber() {}

    float GetOptimalShiftUpRPM(Gear gear) {}

    float GetMaxRPM() {
        return this->mMaxRPM;
    }

    float GetIdleRPM() {
        return this->mMinRPM;
    }

    float GetRedlineRPM() {
        return this->mRedline;
    }

    float GetZero60Time() {
        return 4.5f;
    }

    float GetTheoreticalTopSpeed() {
        return 100.0f;
    }
    float GetShiftDownRPM(Gear Gear) {}
    float GetShiftUpRPM(Gear Gear) {}
    CarType GetCarType() {}
    float CalculateUndersteerFactor() {
        return this->mUndersteer;
    }
    float CalculateOversteerFactor() {
        return this->mOversteer;
    }

    bool IsLocalPlayerCar() {
        return this->mContext == Sound::CONTEXT_PLAYER;
    }
    bool IsAICar() {
        return this->mContext == Sound::CONTEXT_AIRACER;
    }
    bool IsCopCar() {
        return this->mContext == Sound::CONTEXT_COP;
    }
    bool IsOnlineCar() {}
    int GetTopGear() {
        return SEVENTH_GEAR;
    }

    int GetRacePosition() {}
    const bVector3 *GetUpVector() {
        return reinterpret_cast<const bVector3 *>(&mMatrix.v2);
    }
    const bVector3 *GetLeftVector() {
        return reinterpret_cast<const bVector3 *>(&mMatrix.v1);
    }
    const bVector3 *GetForwardVector() {
        return reinterpret_cast<const bVector3 *>(&mMatrix.v0);
    }
    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&mMatrix.v3);
    }
    const bVector2 *GetPosition2D() {
        return reinterpret_cast<const bVector2 *>(GetPosition());
    }
    float GetForwardSpeed() {
        return this->mFWSpeed;
    }
    MovementMode GetMovementMode() {
        return this->mMovementMode;
    }
    PlayerZones GetZone() {}
    Sound::SirenState GetSirenState() {
        return this->mSirenState;
    }
    bool IsHotPursuit() {}
    bool IsShocked() {}
    float GetHeath() {}

    Gear GetGear() {
        return this->mDriveline.mGear;
    }
    bool IsShifting() {
        return this->mDriveline.mGearShiftFlag != 0;
    }

    bool GetNitroFlag() {
        return mEngine.mNOSFlag != 0;
    }

    void SetNosEmptyFlag(bool flag) {
        this->mNosEmptyFlag = flag;
    }
    bool GetNosEmptyFlag() {
        return this->mNosEmptyFlag;
    }

    float GetNosPercentageLeft() {
        return this->mEngine.mNOS;
    }
    float GetRPMPct() {
        return this->mEngine.mRPMPct;
    }
    float GetTurboBoost() {
        return this->mEngine.mBoost;
    }
    float GetThrottle() {
        return this->mEngine.mThrottle;
    }
    float GetBrake() {
        return this->mBrake;
    }
    float GetEBrake() {
        return this->mEBrake;
    }
    bAngle GetSteering() {
        return this->mSteering;
    }
    const bVector3 *GetAcceleration() {}

    const bVector3 *GetOldVel() {
        return &this->mVel1;
    }
    const bVector3 *GetVelocity() {
        return &this->mVel0;
    }
    const bVector2 *GetVelocity2D() {
        return reinterpret_cast<const bVector2 *>(this->GetVelocity());
    }
    float GetVelocityMagnitude() {
        return bLength(this->mVel0);
    }
    float GetVelocityMagnitudeMPH() {
        return MPS2MPH(this->GetVelocityMagnitude());
    }
    float GetYaw() {}
    const bMatrix4 *GetBodyMatrix() {
        return &this->mMatrix;
    }
    bAngle GetAngle() {
        return this->mAngle;
    }

    int GetWheelsOnGround() {
        int num = 0;
        for (int i = 0; i < 4; i++) {
            if (mWheel[i].mWheelOnGround != 0) {
                num++;
            }
        }
        return num;
    }
    SimSurface GetWheelTerrain(int w) {
        return this->mWheel[w].mTerrainType;
    }
    SimSurface GetPrevWheelTerrain(int w) {
        return this->mWheel[w].mPrevTerrainType;
    }

    float GetWheelTractionUsage(int w) {
        return this->mWheel[w].mPercentFsFkTransfer;
    }
    bool IsWheelTouchingGround(int w) {
        return this->mWheel[w].mWheelOnGround != 0;
    }
    float GetWheelZForce(int w) {
        return this->mWheel[w].mWheelForceZ;
    }
    eTireDamage TireState(int w) {
        return static_cast<eTireDamage>(this->mWheel[w].mBlownState);
    }
    bool DidTireJustBlow(int w) {
        return this->mWheel[w].mBlownState == 2 && this->mWheel[w].mPrevBlownState != 2;
    }
    bool DidTireJustPucture(int w) {
        return this->mWheel[w].mBlownState == 1 && this->mWheel[w].mPrevBlownState != 1;
    }

    bVector2 GetWheelSlip(int w) {
        return bVector2(this->mWheel[w].mWheelSlip.x, this->mWheel[w].mWheelSlip.y);
    }
    float GetWheelLoad(int wheel_ndx) {
        return this->mWheel[wheel_ndx].mLoad;
    }

    Attrib::Gen::pvehicle *GetAttributes() {
        return &this->mAttributes;
    }
    Attrib::Gen::engineaudio *GetEngineInfo() {
        return &this->mEngineInfo;
    }

    Sound::Context GetContext() {
        return this->mContext;
    }
    bool IsSimUpdating() {
        return this->mSimUpdating;
    }

    void SetVisualRPM(float pct) {
        this->mVisualRPM = pct;
    }

    void SetNISCarID(int _id) {
        this->mNISCarID = _id;
    }
    int GetNISCarID() {
        return this->mNISCarID;
    }

    bool IsEngineBlown() {
        return this->mEngine.mBlownFlag == 1;
    }
    bool IsEngineSabotaged() {
        return mEngine.mBlownFlag == 2;
    }

    float mMaxTorque;                     // offset 0x4, size 0x4, Decl: 143
    float mMaxRPM;                        // offset 0x8, size 0x4, Decl: 147
    float mMinRPM;                        // offset 0xC, size 0x4, Decl: 149
    float mRedline;                       // offset 0x10, size 0x4, Decl: 151
    bMatrix4 mMatrix;                     // offset 0x14, size 0x40, Decl: 251
    bVector3 mVel0;                       // offset 0x54, size 0x10, Decl: 252
    int mRacePos;                         // offset 0x64, size 0x4, Decl: 253
    bVector3 mVel1;                       // offset 0x68, size 0x10, Decl: 254
    float mBrake;                         // offset 0x78, size 0x4, Decl: 255
    bVector3 mAccel;                      // offset 0x7C, size 0x10, Decl: 256
    float mEBrake;                        // offset 0x8C, size 0x4, Decl: 257
    float mFWSpeed;                       // offset 0x90, size 0x4, Decl: 258
    bool mIsShocked;                      // offset 0x94, size 0x1, Decl: 259
    float mHealth;                        // offset 0x98, size 0x4, Decl: 260
    bool mNosEmptyFlag;                   // offset 0x9C, size 0x1, Decl: 261
    MovementMode mMovementMode;           // offset 0xA0, size 0x4
    PlayerZones mPlayerZone;              // offset 0xA4, size 0x4, Decl: 266
    Sound::Wheel mWheel[NUM_WHEELS];      // offset 0xA8, size 0x110, Decl: 268
    bAngle mSteering;                     // offset 0x1B8, size 0x2, Decl: 269
    bAngle mAngle;                        // offset 0x1BA, size 0x2, Decl: 270
    Sound::Engine mEngine;                // offset 0x1BC, size 0x1C, Decl: 271
    Sound::Driveline mDriveline;          // offset 0x1D8, size 0x8, Decl: 272
    Sound::SirenState mSirenState;        // offset 0x1E0, size 0x4, Decl: 273
    bool mHotPursuit;                     // offset 0x1E4, size 0x1, Decl: 274
    Attrib::Gen::pvehicle mAttributes;    // offset 0x1E8, size 0x14, Decl: 277
    Attrib::Gen::engineaudio mEngineInfo; // offset 0x1FC, size 0x14, Decl: 278
    Sound::Context mContext;              // offset 0x210, size 0x4, Decl: 282
    bool mSimUpdating;                    // offset 0x214, size 0x1, Decl: 284
    bool mAssetsLoaded;                   // offset 0x218, size 0x1, Decl: 285
    const WUID mWorldID;                  // offset 0x21C, size 0x4, Decl: 286
    HSIMABLE mHandle;                     // offset 0x220, size 0x4, Decl: 287
    WUID mTrailerID;                      // offset 0x224, size 0x4, Decl: 289
    float mOversteer;                     // offset 0x228, size 0x4, Decl: 290
    float mUndersteer;                    // offset 0x22C, size 0x4, Decl: 291
    float mSlipAngle;                     // offset 0x230, size 0x4, Decl: 292
    float mVisualRPM;                     // offset 0x234, size 0x4, Decl: 294
    float mTimeSinceSeen;                 // offset 0x238, size 0x4, Decl: 295
    int mNISCarID;                        // offset 0x23C, size 0x4, Decl: 297
    float mDesiredSpeed;                  // offset 0x240, size 0x4, Decl: 298
    Sound::ControlSource mControlSource;  // offset 0x244, size 0x4, Decl: 299
};

#endif
