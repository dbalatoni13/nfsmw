#ifndef EAXSOUND_SOUNDCONN_H
#define EAXSOUND_SOUNDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

class CarSoundConn;
class HeliSoundConn;

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
        , mSirenState(SIREN_OFF) //
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
        mTractionPct[3] = 1.0f;
        mTractionPct[2] = 1.0f;
        mTractionPct[1] = 1.0f;
        mTractionPct[0] = 1.0f;

        mBlownTires[3] = 0;
        mBlownTires[2] = 0;
        mBlownTires[1] = 0;
        mBlownTires[0] = 0;

        bVector2 zeroWheelSlip(0.0f, 0.0f);
        mWheelSlip[0] = zeroWheelSlip;
        mWheelSlip[1] = zeroWheelSlip;
        mWheelSlip[2] = zeroWheelSlip;
        mWheelSlip[3] = zeroWheelSlip;

        mWheelLoad[0] = 0.0f;
        mWheelLoad[1] = 0.0f;
        mWheelLoad[2] = 0.0f;
        mWheelLoad[3] = 0.0f;

        mWheelZforce[0] = 0.0f;
        mWheelZforce[1] = 0.0f;
        mWheelZforce[2] = 0.0f;
        mWheelZforce[3] = 0.0f;

        mWheelOnGround[3] = false;
        mWheelOnGround[2] = false;
        mWheelOnGround[1] = false;
        mWheelOnGround[0] = false;
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
    SirenState mSirenState;
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
