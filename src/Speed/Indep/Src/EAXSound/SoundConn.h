#ifndef EAXSOUND_SOUNDCONN_H
#define EAXSOUND_SOUNDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

struct CarSoundConn;
struct HeliSoundConn;

namespace SoundConn {

// total size: 0x10
class Pkt_Heli_Open : public Sim::Packet {
  public:
    Pkt_Heli_Open(const Attrib::Collection *spec, WUID worldid) : m_VehicleSpec(spec), mWorldID(worldid) {}

    // Virtual overrides
    // Packet
    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "HeliSoundConn";
        return hash;
    }

    unsigned int Size() override {
        return sizeof(*this);
    }

    unsigned int Type() override {
        return SType();
    }

    static unsigned int SType() {
        static UCrc32 hash = "Pkt_Heli_Open";
        return hash.GetValue();
    }

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
};

class Pkt_Car_Service : public Sim::Packet {
  public:
    explicit Pkt_Car_Service(float audible_rpm)
        : mRPMPercent(0.0f) //
        , mThrottlePercent(0.0f) //
        , mBrakePercent(0.0f) //
        , mEBrakePercent(0.0f) //
        , mSteering(0.0f) //
        , mGear(1) //
        , mSirenState(-1) //
        , mHotPursuit(false) //
        , mOversteer(0.0f) //
        , mUndersteer(0.0f) //
        , mSlipAngle(0.0f) //
        , mHealth(1.0f) //
        , mAudibleRPMPct(audible_rpm) //
        , mEngineBlown(0) //
        , mNOSFlag(false) //
        , mNOSCapacity(0.0f) //
        , mTrailer(0) //
        , mTimeSinceSeen(0.0f) //
        , mDesiredSpeed(0.0f) //
        , mControlSource(0) {
        const Attrib::Collection *const nullSpec = SimSurface::kNull.GetConstCollection();
        for (int i = 0; i < 4; ++i) {
            mTractionPct[i] = 1.0f;
            if (nullSpec != nullptr) {
                mWheelTerrain[i].Change(nullSpec);
            }
            mWheelSlip[i].x = 0.0f;
            mWheelSlip[i].y = 0.0f;
            mWheelLoad[i] = 0.0f;
            mWheelZforce[i] = 0.0f;
            mWheelOnGround[i] = false;
            mBlownTires[i] = 0;
        }
    }
    ~Pkt_Car_Service() override;
    UCrc32 ConnectionClass() override;
    unsigned int Size() override;
    unsigned int Type() override;
    static unsigned int SType();

    float mRPMPercent;
    float mThrottlePercent;
    float mBrakePercent;
    float mEBrakePercent;
    float mSteering;
    int mGear;
    float mTractionPct[4];
    int mSirenState;
    bool mHotPursuit;
    float mOversteer;
    float mUndersteer;
    float mSlipAngle;
    float mHealth;
    SimSurface mWheelTerrain[4];
    float mAudibleRPMPct;
    bVector2 mWheelSlip[4];
    float mWheelLoad[4];
    float mWheelZforce[4];
    bool mWheelOnGround[4];
    int mEngineBlown;
    bool mNOSFlag;
    float mNOSCapacity;
    unsigned int mTrailer;
    unsigned char mBlownTires[4];
    float mTimeSinceSeen;
    float mDesiredSpeed;
    int mControlSource;
};

class Pkt_Heli_Service : public Sim::Packet {
  public:
    Pkt_Heli_Service()
        : mVelocity(0.0f) //
        , mAcceleration(0.0f) {}
    ~Pkt_Heli_Service() override {}
    UCrc32 ConnectionClass() override;
    unsigned int Size() override;
    unsigned int Type() override;
    static unsigned int SType();

  private:
    float mVelocity;
    float mAcceleration;
};

void UpdateServices(float dT);

}; // namespace SoundConn

#endif
