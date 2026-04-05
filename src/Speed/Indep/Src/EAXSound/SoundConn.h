#ifndef EAXSOUND_SOUNDCONN_H
#define EAXSOUND_SOUNDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

struct EAX_HeliState;

class CarSoundConn : public Sim::Connection, public UTL::Collections::Listable<CarSoundConn, 10> {
  public:
    bool mConnected;                // offset 0x14, size 0x1
    EAX_CarState *mState;           // offset 0x18, size 0x4
    WorldConn::Reference mTarget;   // offset 0x1C, size 0x10

    CarSoundConn(const Sim::ConnectionData &data);
    ~CarSoundConn() override;
    inline virtual void OnReceive(Sim::Packet *pkt) override {}
    inline void OnClose() override { delete this; }
    Sim::ConnStatus OnStatusCheck() override;
    void UpdateState(float dT);
    static Sim::Connection *Construct(const Sim::ConnectionData &data);
    static inline void SetAssetsLoaded(CarSoundConn *conn) {
        if (conn->mConnected && conn->mState != nullptr) {
            conn->mState->mAssetsLoaded = true;
        }
    }
};

class HeliSoundConn : public Sim::Connection, public UTL::Collections::Listable<HeliSoundConn, 10> {
  public:
    EAX_HeliState *mState;          // offset 0x14, size 0x4
    WorldConn::Reference mTarget;   // offset 0x18, size 0x10

    HeliSoundConn(const Sim::ConnectionData &data);
    ~HeliSoundConn() override;
    inline virtual void OnReceive(Sim::Packet *pkt) override {}
    inline void OnClose() override { delete this; }
    inline Sim::ConnStatus OnStatusCheck() override { return Sim::CONNSTATUS_READY; }
    void UpdateState(float dT);
    static Sim::Connection *Construct(const Sim::ConnectionData &data);
};

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

// total size: 0x18
class Pkt_Car_Open : public Sim::Packet {
  public:
    Pkt_Car_Open(const Attrib::Collection *spec, WUID worldid, Sound::Context ctx, bool spool_load, HSIMABLE__ *handle)
        : m_VehicleSpec(spec) //
        , mWorldID(worldid) //
        , mCarContext(ctx) //
        , mSpoolLoad(spool_load) //
        , mHandle(handle) {}

    ~Pkt_Car_Open() override {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "CarSoundConn";
        return hash;
    }

    unsigned int Size() override {
        return sizeof(*this);
    }

    unsigned int Type() override {
        return SType();
    }

    static unsigned int SType() {
        static UCrc32 hash = "Pkt_Car_Open";
        return hash.GetValue();
    }

    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
    Sound::Context mCarContext;              // offset 0xC, size 0x4
    bool mSpoolLoad;                         // offset 0x10, size 0x1
    HSIMABLE__ *mHandle;                     // offset 0x14, size 0x4
};

// total size: 0x108
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
        , mControlSource(Sound::CONTROL_NONE) {
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

    float mRPMPercent;             // offset 0x4, size 0x4
    float mThrottlePercent;        // offset 0x8, size 0x4
    float mBrakePercent;           // offset 0xC, size 0x4
    float mEBrakePercent;          // offset 0x10, size 0x4
    float mSteering;               // offset 0x14, size 0x4
    int mGear;                     // offset 0x18, size 0x4
    float mTractionPct[4];         // offset 0x1C, size 0x10
    SirenState mSirenState;        // offset 0x2C, size 0x4
    bool mHotPursuit;              // offset 0x30, size 0x1
    float mOversteer;              // offset 0x34, size 0x4
    float mUndersteer;             // offset 0x38, size 0x4
    float mSlipAngle;              // offset 0x3C, size 0x4
    float mHealth;                 // offset 0x40, size 0x4
    SimSurface mWheelTerrain[4];   // offset 0x44, size 0x50
    float mAudibleRPMPct;          // offset 0x94, size 0x4
    bVector2 mWheelSlip[4];        // offset 0x98, size 0x20
    float mWheelLoad[4];           // offset 0xB8, size 0x10
    float mWheelZforce[4];         // offset 0xC8, size 0x10
    bool mWheelOnGround[4];        // offset 0xD8, size 0x4
    int mEngineBlown;              // offset 0xE8, size 0x4
    bool mNOSFlag;                 // offset 0xEC, size 0x1
    float mNOSCapacity;            // offset 0xF0, size 0x4
    unsigned int mTrailer;         // offset 0xF4, size 0x4
    unsigned char mBlownTires[4];  // offset 0xF8, size 0x4
    float mTimeSinceSeen;          // offset 0xFC, size 0x4
    float mDesiredSpeed;           // offset 0x100, size 0x4
    Sound::ControlSource mControlSource; // offset 0x104, size 0x4
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
