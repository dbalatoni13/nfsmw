#ifndef SOUND_CONN_H
#define SOUND_CONN_H

#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#define SOUND_SERVICE UCrc32(UCRC32_EAXSOUND)
#define DECLARE_SOUNDPACKET(_PKT_, _HANDLER_)                                                                                                        \
    friend class ::_HANDLER_;                                                                                                                        \
    UCrc32 ConnectionClass() override {                                                                                                              \
        static UCrc32 hash(#_HANDLER_);                                                                                                              \
        return hash;                                                                                                                                 \
    }                                                                                                                                                \
    DECLARE_SIMPACKET(_PKT_, #_PKT_)

struct HeliSoundConn;
struct CarSoundConn;

namespace SoundConn {

// total size: 0x18
class Pkt_Car_Open : public Sim::Packet {
  public:
    ~Pkt_Car_Open() override {} // TODO is this required?

    DECLARE_SOUNDPACKET(Pkt_Car_Open, CarSoundConn);

    Pkt_Car_Open(const Attrib::Collection *spec, unsigned int worldid, Sound::Context ctx, bool spool_load, HSIMABLE handle) {}

  private:
    Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                     // offset 0x8, size 0x4
    Sound::Context mCarContext;        // offset 0xC, size 0x4
    bool mSpoolLoad;                   // offset 0x10, size 0x1
    HSIMABLE mHandle;                  // offset 0x14, size 0x4
};

// total size: 0x108
class Pkt_Car_Service : public Sim::Packet {
  public:
    ~Pkt_Car_Service() override; // TODO is this required?

    DECLARE_SOUNDPACKET(Pkt_Car_Service, CarSoundConn);

    float GetAudibleRPMPercent() const {
        return this->mAudibleRPMPct;
    }

    void SetNOSFlag(bool b) {
        this->mNOSFlag = b;
    }

    void SetEngineBlown(int b) {
        this->mEngineBlown = b;
    }

    void SetGear(int gear) {
        this->mGear = gear;
    }

    void SetSteering(float pct) {
        this->mSteering = pct;
    }

    void SetWheelSlip(int idx, float fwd, float lateral) {
        this->mWheelSlip[idx].x = fwd;
        this->mWheelSlip[idx].y = lateral;
    }

    void SetHealth(float h) {
        this->mHealth = h;
    }

    void SetTireBlown(Sound::WheelConfig id, eTireDamage state) {
        this->mBlownTires[id] = state;
    }

    void SetControlSource(Sound::ControlSource source) {
        this->mControlSource = source;
    }

    void SetThrottlePercent(float pct) {
        this->mThrottlePercent = pct;
    }

    void SetRPMPercent(float pct) {
        this->mRPMPercent = pct;
    }

    void SetBrakePercent(float pct) {
        this->mBrakePercent = pct;
    }

    void SetEBrakePercent(float pct) {
        this->mEBrakePercent = pct;
    }

    void SetTraction(Sound::WheelConfig id, float pct) {
        this->mTractionPct[id] = pct;
    }

    void SetWheelLoad(Sound::WheelConfig id, float load) {
        this->mWheelLoad[id] = load;
    }

    void SetWheelZForce(Sound::WheelConfig id, float load) {
        this->mWheelZforce[id] = load;
    }

    void SetWheelTerrain(int idx, SimSurface s, bool onground) {
        this->mWheelTerrain[idx] = s;
        this->mWheelOnGround[idx] = onground;
    }

    void SetSiren(Sound::SirenState s) {
        this->mSirenState = s;
    }

    void SetTimeSinceSeen(float seconds) {
        this->mTimeSinceSeen = seconds;
    }

    void SetHotPursuit(bool h) {
        this->mHotPursuit = h;
    }

    void SetOversteer(float factor) {
        this->mOversteer = factor;
    }

    void SetUndersteer(float factor) {
        this->mUndersteer = factor;
    }

    void SetSlipAngle(Radians sa) {
        this->mSlipAngle = sa;
    }

    void SetNOSCapacity(float capacity) {
        this->mNOSCapacity = capacity;
    }

    void SetTrailer(WUID trailer) {
        this->mTrailer = trailer;
    }

    void SetDesiredSpeed(float speed) {
        this->mDesiredSpeed = speed;
    }

  private:
    Pkt_Car_Service(float audible_rpm) : mAudibleRPMPct(audible_rpm) {}

    float mRPMPercent;                   // offset 0x4, size 0x4
    float mThrottlePercent;              // offset 0x8, size 0x4
    float mBrakePercent;                 // offset 0xC, size 0x4
    float mEBrakePercent;                // offset 0x10, size 0x4
    float mSteering;                     // offset 0x14, size 0x4
    int mGear;                           // offset 0x18, size 0x4
    float mTractionPct[4];               // offset 0x1C, size 0x10
    Sound::SirenState mSirenState;       // offset 0x2C, size 0x4
    bool mHotPursuit;                    // offset 0x30, size 0x1
    float mOversteer;                    // offset 0x34, size 0x4
    float mUndersteer;                   // offset 0x38, size 0x4
    float mSlipAngle;                    // offset 0x3C, size 0x4
    float mHealth;                       // offset 0x40, size 0x4
    SimSurface mWheelTerrain[4];         // offset 0x44, size 0x50
    float mAudibleRPMPct;                // offset 0x94, size 0x4
    bVector2 mWheelSlip[4];              // offset 0x98, size 0x20
    float mWheelLoad[4];                 // offset 0xB8, size 0x10
    float mWheelZforce[4];               // offset 0xC8, size 0x10
    bool mWheelOnGround[4];              // offset 0xD8, size 0x4
    int mEngineBlown;                    // offset 0xE8, size 0x4
    bool mNOSFlag;                       // offset 0xEC, size 0x1
    float mNOSCapacity;                  // offset 0xF0, size 0x4
    WUID mTrailer;                       // offset 0xF4, size 0x4
    uint8 mBlownTires[4];                // offset 0xF8, size 0x4
    float mTimeSinceSeen;                // offset 0xFC, size 0x4
    float mDesiredSpeed;                 // offset 0x100, size 0x4
    Sound::ControlSource mControlSource; // offset 0x104, size 0x4
};

// total size: 0x10
class Pkt_Heli_Open : public Sim::Packet {
  public:
    ~Pkt_Heli_Open() override {} // TODO is this required?

    DECLARE_SOUNDPACKET(Pkt_Heli_Open, HeliSoundConn);

    Pkt_Heli_Open(const Attrib::Collection *spec, WUID worldid) : m_VehicleSpec(spec), mWorldID(worldid) {}

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
};

// total size: 0xC

class Pkt_Heli_Service : public Sim::Packet {
  public:
    ~Pkt_Heli_Service() override {} // TODO is this required?
    DECLARE_SOUNDPACKET(Pkt_Heli_Service, HeliSoundConn);

  private:
    Pkt_Heli_Service() {}

    float mVelocity;     // offset 0x4, size 0x4
    float mAcceleration; // offset 0x8, size 0x4
};

void InitServices();
void RestoreServices();
void UpdateServices(float dT);

}; // namespace SoundConn

#endif
