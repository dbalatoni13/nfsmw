#ifndef EAXSOUND_EAXCARSTATE_H
#define EAXSOUND_EAXCARSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Sound {
enum Context {
    CONTEXT_PLAYER = 0,
    CONTEXT_AIRACER = 1,
    CONTEXT_COP = 2,
    CONTEXT_TRAFFIC = 3,
    CONTEXT_ONLINE = 4,
    CONTEXT_TRACTOR = 5,
    CONTEXT_TRAILER = 6,
    CONTEXT_MAX = 7,

    kRaceContext_QuickRace = CONTEXT_PLAYER,
    kRaceContext_TimeTrial = CONTEXT_AIRACER,
    kRaceContext_Career = CONTEXT_COP,
    kRaceContext_Count = CONTEXT_TRAFFIC,
};

enum MovementMode {
    NO_MOVEMENT = 0,
    PHYSICS_MOVEMENT = 1,
    TRAFFIC_MOVEMENT = 2,
    MELLOW_MOVEMENT = 3,
    ONLINE_MOVEMENT = 4,
    BELT_MOVEMENT = 5,
    NIS_MOVEMENT = 6,
    NUM_MOVEMENT_MODES = 7,
};

enum PlayerZones {
    PLAYER_ZONE_NONE = 0,
    PLAYER_ZONE_FREEZE = 1,
    PLAYER_ZONE_PREVIEW = 2,
    PLAYER_ZONE_SLOMO = 3,
    PLAYER_ZONE_SLOMO2 = 4,
    PLAYER_ZONE_WARP = 5,
    PLAYER_ZONE_JUMPVIEW = 6,
    PLAYER_ZONE_JUMPVIEW2 = 7,
    PLAYER_ZONE_COUNT = 8,
};

enum Gear {
    SPORT_SHIFT = -2,
    AUTOMATIC = -1,
    REVERSE = 0,
    NEUTRAL = 1,
    FIRST_GEAR = 2,
    SECOND_GEAR = 3,
    THIRD_GEAR = 4,
    FOURTH_GEAR = 5,
    FIFTH_GEAR = 6,
    SIXTH_GEAR = 7,
    SEVENTH_GEAR = 8,
};

enum ControlSource {
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 2,
    CONTROL_NIS = 3,
    CONTROL_ONLINE = 4,
};

struct Wheel {
    bVector2 mWheelSlip;          // offset 0x0
    float mWheelForceZ;           // offset 0x8
    float mPercentFsFkTransfer;   // offset 0xC
    int mWheelOnGround;           // offset 0x10
    SimSurface mTerrainType;      // offset 0x14
    SimSurface mPrevTerrainType;  // offset 0x28
    float mLoad;                  // offset 0x3C
    unsigned char mBlownState;    // offset 0x40
    unsigned char mPrevBlownState; // offset 0x41

    Wheel() : mWheelSlip(), mTerrainType(), mPrevTerrainType() {
        Reset();
    }

    inline void Reset() {
        mWheelOnGround = 1;
        mWheelForceZ = 0.0f;
        mWheelSlip = bVector2(0.0f, 0.0f);
        mTerrainType = SimSurface::kNull;
        mPrevBlownState = 0;
        mPercentFsFkTransfer = 0.0f;
        mLoad = 0.0f;
        mBlownState = 0;
    }
};

struct Engine {
    int mBoostFlag; // offset 0x0
    int mNOSFlag;   // offset 0x4
    float mNOS;     // offset 0x8
    float mRPMPct;  // offset 0xC
    float mThrottle; // offset 0x10
    float mBoost;   // offset 0x14
    int mBlownFlag; // offset 0x18

    Engine() {
        Reset();
    }

    inline void Reset() {
        mBoostFlag = 0;
        mNOSFlag = 0;
        mNOS = 0.0f;
        mRPMPct = 0.0f;
        mThrottle = 0.0f;
        mBoost = 0.0f;
        mBlownFlag = 0;
    }
};

struct Driveline {
    int mGearShiftFlag; // offset 0x0
    Gear mGear;         // offset 0x4

    Driveline() : mGearShiftFlag(0), mGear(NEUTRAL) {}
};
} // namespace Sound

struct EAX_CarState : public UTL::Collections::Listable<EAX_CarState, 10> {
    float mMaxTorque; // offset 0x4
    float mMaxRPM; // offset 0x8
    float mMinRPM; // offset 0xC
    float mRedline; // offset 0x10
    bMatrix4 mMatrix; // offset 0x14
    bVector3 mVel0; // offset 0x54
    int mRacePos; // offset 0x64
    bVector3 mVel1; // offset 0x68
    float mBrake; // offset 0x78
    bVector3 mAccel; // offset 0x7C
    float mEBrake; // offset 0x8C
    float mFWSpeed; // offset 0x90
    bool mIsShocked; // offset 0x94
    float mHealth; // offset 0x98
    bool mNosEmptyFlag; // offset 0x9C
    Sound::MovementMode mMovementMode; // offset 0xA0
    Sound::PlayerZones mPlayerZone; // offset 0xA4
    Sound::Wheel mWheel[4]; // offset 0xA8
    unsigned short mSteering; // offset 0x1B8
    unsigned short mAngle; // offset 0x1BA
    Sound::Engine mEngine; // offset 0x1BC
    Sound::Driveline mDriveline; // offset 0x1D8
    int mSirenState; // offset 0x1E0
    bool mHotPursuit; // offset 0x1E4
    Attrib::Gen::pvehicle mAttributes; // offset 0x1E8
    Attrib::Gen::engineaudio mEngineInfo; // offset 0x1FC
    Sound::Context mContext; // offset 0x210
    bool mSimUpdating; // offset 0x214
    bool mAssetsLoaded; // offset 0x218
    unsigned int mWorldID; // offset 0x21C
    HSIMABLE__ *mHandle; // offset 0x220
    unsigned int mTrailerID; // offset 0x224
    float mOversteer; // offset 0x228
    float mUndersteer; // offset 0x22C
    float mSlipAngle; // offset 0x230
    float mVisualRPM; // offset 0x234
    float mTimeSinceSeen; // offset 0x238
    int mNISCarID; // offset 0x23C
    float mDesiredSpeed; // offset 0x240
    Sound::ControlSource mControlSource; // offset 0x244

    bool IsShifting() {
        return mDriveline.mGearShiftFlag != 0;
    }

    bool GetNitroFlag() {
        return mEngine.mNOSFlag != 0;
    }

    EAX_CarState *GetState() {
        return this;
    }

    EAX_CarState *GetDriver() {
        return this;
    }

    int GetNISCarID() {
        return mNISCarID;
    }

    void SetNISCarID(int _id) {
        mNISCarID = _id;
    }

    bool IsLocalPlayerCar() {
        return mContext == Sound::CONTEXT_PLAYER;
    }

    bool IsAICar() {
        return mContext == Sound::CONTEXT_AIRACER;
    }

    unsigned short GetSteering() {
        return mSteering;
    }

    float GetBrake() {
        return mBrake;
    }

    Attrib::Gen::engineaudio *GetEngineInfo() {
        return &mEngineInfo;
    }

    Sound::Gear GetGear() {
        return mDriveline.mGear;
    }

    Sound::Context GetContext() {
        return mContext;
    }

    Attrib::Gen::pvehicle *GetAttributes() {
        return &mAttributes;
    }

    float GetMaxEngineTorque() { return mMaxTorque; }
    float GetMaxRPM() { return mMaxRPM; }
    float GetIdleRPM() { return mMinRPM; }
    float GetRedlineRPM() { return mRedline; }

    float GetWheelTractionUsage(int w) {
        return mWheel[w].mPercentFsFkTransfer;
    }

    bVector2 GetWheelSlip(int w) {
        return bVector2(mWheel[w].mWheelSlip.x, mWheel[w].mWheelSlip.y);
    }

    float GetWheelLoad(int wheel_ndx) {
        return mWheel[wheel_ndx].mLoad;
    }

    SimSurface GetWheelTerrain(int w) {
        return mWheel[w].mTerrainType;
    }

    eTireDamage TireState(int w) {
        return static_cast<eTireDamage>(mWheel[w].mBlownState);
    }

    bool IsWheelTouchingGround(int w) {
        return mWheel[w].mWheelOnGround != 0;
    }

    const bVector3 *GetForwardVector() {
        return static_cast<const bVector3 *>(static_cast<const void *>(&mMatrix.v0));
    }

    const bVector3 *GetLeftVector() {
        return static_cast<const bVector3 *>(static_cast<const void *>(&mMatrix.v1));
    }

    bVector3 *GetPosition() {
        return static_cast<bVector3 *>(static_cast<void *>(&mMatrix.v3));
    }

    const bVector2 *GetPosition2D() {
        return static_cast<const bVector2 *>(static_cast<const void *>(GetPosition()));
    }

    float GetForwardSpeed() {
        return mFWSpeed;
    }

    float GetThrottle() {
        return mEngine.mThrottle;
    }

    const bVector3 *GetVelocity() {
        return &mVel0;
    }

    const bVector3 *GetOldVel() {
        return &mVel1;
    }

    float GetVelocityMagnitude() {
        return bLength(mVel0);
    }

    float GetTheoreticalTopSpeed() {
        return 100.0f;
    }

    int GetTopGear() {
        return Sound::SEVENTH_GEAR;
    }

    float GetVelocityMagnitudeMPH() {
        return MPS2MPH(GetVelocityMagnitude());
    }

    bool IsEngineBlown() {
        return mEngine.mBlownFlag == 1;
    }

    bool IsEngineSabotaged() {
        return mEngine.mBlownFlag == 2;
    }

    static EAX_CarState *Find(unsigned int objectid) {
        {
            EAX_CarState *const *iter = GetList().begin();
            while (iter != GetList().end()) {
                {
                    EAX_CarState *state = *iter;
                    if (state->mWorldID == objectid) {
                        return state;
                    }
                }
                ++iter;
            }
        }
        return nullptr;
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

    void SetVisualRPM(float pct) {
        mVisualRPM = pct;
    }

    ~EAX_CarState() {}

    EAX_CarState(const Attrib::Collection *atr, Sound::Context context, unsigned int wuid, HSIMABLE__ *handle);
};

typedef char EAX_CarStateSizeCheck[(sizeof(EAX_CarState) == 0x248) ? 1 : -1];

#endif
