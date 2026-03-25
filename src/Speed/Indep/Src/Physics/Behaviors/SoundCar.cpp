#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICarAudio.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IInput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

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
};

enum WheelConfig {
    EAX4WD_FL = 0,
    EAX4WD_FR = 1,
    EAX4WD_RR = 2,
    EAX4WD_RL = 3,
};

}; // namespace Sound

HINTERFACE ICarAudio::_IHandle() {
    return (HINTERFACE)_IHandle;
}

enum ControlSource {
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 2,
    CONTROL_NIS = 3,
    CONTROL_ONLINE = 4,
};

extern "C" UCrc32 VehicleClass_TRAILER asm("_12VehicleClass.TRAILER");
extern "C" UCrc32 VehicleClass_TRACTOR asm("_12VehicleClass.TRACTOR");

namespace SoundConn {

// total size: 0x18
class Pkt_Car_Open : public Sim::Packet {
  public:
    Pkt_Car_Open(const Attrib::Collection *spec, WUID worldid, Sound::Context ctx, bool spool_load, HSIMABLE handle)
        : m_VehicleSpec(spec), //
          mWorldID(worldid),   //
          mCarContext(ctx),    //
          mSpoolLoad(spool_load), //
          mHandle(handle) {}

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

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
    Sound::Context mCarContext;             // offset 0xC, size 0x4
    bool mSpoolLoad;                        // offset 0x10, size 0x1
    HSIMABLE mHandle;                       // offset 0x14, size 0x4
};

// total size: 0x108
class Pkt_Car_Service : public Sim::Packet {
  public:
    void SetSlipAngle(float sa) {
        mSlipAngle = sa;
    }

    void SetTraction(Sound::WheelConfig id, float pct) {
        mTractionPct[id] = pct;
    }

    void SetWheelTerrain(int idx, SimSurface s, bool onground) {
        mWheelTerrain[idx] = s;
        mWheelOnGround[idx] = onground;
    }

    void SetOversteer(float factor) {
        mOversteer = factor;
    }

    void SetUndersteer(float factor) {
        mUndersteer = factor;
    }

    void SetWheelSlip(int idx, float fwd, float lateral) {
        mWheelSlip[idx].x = fwd;
        mWheelSlip[idx].y = lateral;
    }

    void SetWheelLoad(Sound::WheelConfig id, float load) {
        mWheelLoad[id] = load;
    }

    void SetWheelZForce(Sound::WheelConfig id, float load) {
        mWheelCompression[id] = load;
    }

    void SetSiren(SirenState s) {
        mSirenState = s;
    }

    void SetTimeSinceSeen(float seconds) {
        mTimeSinceSeen = seconds;
    }

    void SetHotPursuit(bool h) {
        mHotPursuit = h;
    }

    void SetEngineBlown(int b) {
        mEngineBlown = b;
    }

    void SetTrailer(WUID trailer) {
        mTrailer = trailer;
    }

    void SetTireBlown(Sound::WheelConfig id, eTireDamage state) {
        mBlownTires[id] = state;
    }

    float mRPMPercent;           // offset 0x4, size 0x4
    float mThrottlePercent;      // offset 0x8, size 0x4
    float mBrakePercent;         // offset 0xC, size 0x4
    float mEBrakePercent;        // offset 0x10, size 0x4
    float mSteering;             // offset 0x14, size 0x4
    int mGear;                   // offset 0x18, size 0x4
    float mTractionPct[4];       // offset 0x1C, size 0x10
    SirenState mSirenState;      // offset 0x2C, size 0x4
    bool mHotPursuit;            // offset 0x30, size 0x1
    float mOversteer;            // offset 0x34, size 0x4
    float mUndersteer;           // offset 0x38, size 0x4
    float mSlipAngle;            // offset 0x3C, size 0x4
    float mHealth;               // offset 0x40, size 0x4
    SimSurface mWheelTerrain[4]; // offset 0x44, size 0x50
    float mAudibleRPMPct;        // offset 0x94, size 0x4
    bVector2 mWheelSlip[4];      // offset 0x98, size 0x20
    float mWheelLoad[4];         // offset 0xB8, size 0x10
    float mWheelCompression[4];  // offset 0xC8, size 0x10
    bool mWheelOnGround[4];      // offset 0xD8, size 0x4
    int mEngineBlown;            // offset 0xE8, size 0x4
    bool mNOSFlag;               // offset 0xEC, size 0x1
    float mNOSCapacity;          // offset 0xF0, size 0x4
    WUID mTrailer;               // offset 0xF4, size 0x4
    unsigned char mBlownTires[4]; // offset 0xF8, size 0x4
    float mTimeSinceSeen;        // offset 0xFC, size 0x4
    float mDesiredSpeed;         // offset 0x100, size 0x4
    ControlSource mControlSource; // offset 0x104, size 0x4
};

}; // namespace SoundConn

// total size: 0x9C
class SoundCar : public VehicleBehavior, public ICarAudio, public IAudible {
  public:
    SoundCar(const BehaviorParams &params, Sound::Context ctx);
    ~SoundCar() override;

    void Reset() override;
    void OnTaskSimulate(float dT) override;
    bool IsAudible() const override;
    Rpm GetRPM() const override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

    virtual void OnService(SoundConn::Pkt_Car_Service &svc);
    virtual void OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId);

  private:
    HSIMSERVICE mSoundService;                        // offset 0x60, size 0x4
    IEngine *mEngine;                                // offset 0x64, size 0x4
    IInput *mInput;                                  // offset 0x68, size 0x4
    ITransmission *mTransmission;                    // offset 0x6C, size 0x4
    ISuspension *mSuspension;                        // offset 0x70, size 0x4
    IVehicleAI *mAI;                                 // offset 0x74, size 0x4
    IHumanAI *mHumanAI;                              // offset 0x78, size 0x4
    IDamageable *mDamage;                            // offset 0x7C, size 0x4
    Rpm mSoundRPM;                                   // offset 0x80, size 0x4
    INISCarControl *mNIS;                            // offset 0x84, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo; // offset 0x88, size 0x14
};

// total size: 0xA0
class SoundTraffic : public SoundCar {
  public:
    SoundTraffic(const BehaviorParams &params, Sound::Context ctx);
    ~SoundTraffic() override {}

    static Behavior *Construct(const BehaviorParams &params);

    void LocateTrailer();
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnService(SoundConn::Pkt_Car_Service &svc) override;

  private:
    WUID mTrailer; // offset 0x9C, size 0x4
};

// total size: 0xA4
class SoundCop : public SoundCar {
  public:
    SoundCop(const BehaviorParams &params);
    ~SoundCop() override {}

    static Behavior *Construct(const BehaviorParams &params);

    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnService(SoundConn::Pkt_Car_Service &svc) override;

  private:
    IPursuitAI *mPursuitAI;                 // offset 0x9C, size 0x4
    IDamageableVehicle *mVehicleDamage;    // offset 0xA0, size 0x4
};

// total size: 0xA4
class SoundRacer : public SoundCar {
  public:
    SoundRacer(const BehaviorParams &params, Sound::Context ctx);
    ~SoundRacer() override {}

    static Behavior *Construct(const BehaviorParams &params);

    void OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) override;
    void OnService(SoundConn::Pkt_Car_Service &svc) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IEngineDamage *mEngineDamage; // offset 0x9C, size 0x4
    ISpikeable *mSpikeDamage;     // offset 0xA0, size 0x4
};

SoundCar::SoundCar(const BehaviorParams &params, Sound::Context ctx)
    : VehicleBehavior(params, 0), //
      ICarAudio(params.fowner), //
      IAudible(params.fowner), //
      mSoundService(nullptr), //
      mSoundRPM(0.0f), //
      mEngineInfo(GetOwner(), 0) {
    GetOwner()->QueryInterface(&mEngine);
    GetOwner()->QueryInterface(&mInput);
    GetOwner()->QueryInterface(&mTransmission);
    GetOwner()->QueryInterface(&mSuspension);
    GetOwner()->QueryInterface(&mDamage);
    GetOwner()->QueryInterface(&mAI);
    GetOwner()->QueryInterface(&mHumanAI);
    GetOwner()->QueryInterface(&mNIS);

    mSoundRPM = mEngineInfo->IDLE();

    SoundConn::Pkt_Car_Open pkt(GetOwner()->GetAttributes().GetConstCollection(),
                                GetOwner()->GetWorldID(),
                                ctx,
                                GetVehicle()->IsSpooled(),
                                GetOwner()->GetInstanceHandle());
    mSoundService = OpenService(UCrc32(0xa3f44e2e), &pkt);
}

SoundCar::~SoundCar() {
    if (mSoundService) {
        CloseService(mSoundService);
    }
}

void SoundCar::Reset() {}

void SoundCar::OnTaskSimulate(float dT) {}

bool SoundCar::IsAudible() const {
    return CheckService(mSoundService) == Sim::CONNSTATUS_READY;
}

Rpm SoundCar::GetRPM() const {
    return mSoundRPM;
}

void SoundCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mEngine);
        GetOwner()->QueryInterface(&mTransmission);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
        GetOwner()->QueryInterface(&mNIS);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mDamage);
    } else if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mAI);
        GetOwner()->QueryInterface(&mHumanAI);
    }
}

bool SoundCar::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (!INIS::Exists() && hCon == mSoundService && !IsPaused()) {
        OnService(*static_cast<SoundConn::Pkt_Car_Service *>(pkt));
        return true;
    }
    return false;
}

void SoundCar::OnService(SoundConn::Pkt_Car_Service &svc) {
    if (mEngine && mTransmission) {
        float rpm = mEngine->GetRPM();
        float idle = mEngineInfo->IDLE();
        float redLineRange = mEngineInfo->RED_LINE() - idle;
        float rpmPercent = 0.0f;

        if (redLineRange > 0.0f) {
            rpmPercent = UMath::Min((rpm - idle) / redLineRange, 1.0f);
            rpmPercent = UMath::Max(0.0f, rpmPercent);
        }

        mSoundRPM = svc.mAudibleRPMPct * redLineRange + idle;
        svc.mRPMPercent = rpmPercent;
        svc.mNOSFlag = mEngine->IsNOSEngaged();
        svc.mNOSCapacity = mEngine->GetNOSCapacity();
        svc.mGear = static_cast<int>(mTransmission->GetGear());
    }

    if (mInput) {
        float throttle = UMath::Min(mInput->GetControls().fGas, 1.0f);
        throttle = UMath::Max(0.0f, throttle);

        float brake = UMath::Min(mInput->GetControls().fBrake, 1.0f);
        brake = UMath::Max(0.0f, brake);

        float eBrake = UMath::Min(mInput->GetControls().fHandBrake, 1.0f);
        eBrake = UMath::Max(0.0f, eBrake);

        float steering = UMath::Min(mInput->GetControls().fSteering, 1.0f);
        steering = UMath::Max(-1.0f, steering);

        svc.mThrottlePercent = throttle;
        svc.mBrakePercent = brake;
        svc.mEBrakePercent = eBrake;
        svc.mSteering = steering;

        if (mNIS) {
            svc.mControlSource = CONTROL_NIS;
        } else if (GetVehicle()->GetDriverClass() == DRIVER_REMOTE) {
            svc.mControlSource = CONTROL_ONLINE;
        } else if (mHumanAI && !mHumanAI->GetAiControl()) {
            svc.mControlSource = CONTROL_HUMAN;
        } else if (mAI) {
            svc.mControlSource = CONTROL_AI;
            svc.mDesiredSpeed = mAI->GetDriveSpeed();
        }
    }

    if (mDamage) {
        if (mDamage->IsDestroyed()) {
            svc.mHealth = 0.0f;
        } else {
            svc.mHealth = mDamage->GetHealth();
        }
    }

    svc.SetSlipAngle(GetVehicle()->GetSlipAngle() * 6.2831855f);

    if (mSuspension) {
        for (unsigned int wheelid = 0; wheelid < mSuspension->GetNumWheels(); ++wheelid) {
            Sound::WheelConfig packetWheel;
            const UMath::Vector3 &vel = mSuspension->GetWheelVelocity(wheelid);
            float pos_x = mSuspension->GetWheelLocalPos(wheelid).x;
            float pct = mSuspension->GetWheelTraction(wheelid);
            float slip;
            float toleratedSlip;

            if (wheelid < 2) {
                packetWheel = Sound::EAX4WD_FL;
                if ((wheelid & 1) != 0) {
                    packetWheel = Sound::EAX4WD_FR;
                }
            } else if ((wheelid & 1) != 0) {
                packetWheel = Sound::EAX4WD_RR;
            } else {
                packetWheel = Sound::EAX4WD_RL;
            }

            slip = mSuspension->GetWheelSlip(wheelid);
            toleratedSlip = mSuspension->GetToleratedSlip(wheelid) * 0.2f;
            if (slip > toleratedSlip) {
                slip -= toleratedSlip;
            } else if (slip < -toleratedSlip) {
                slip += toleratedSlip;
            } else {
                slip = 0.0f;
            }

            SimSurface roadsurf = mSuspension->GetWheelRoadSurface(wheelid);
            svc.SetWheelTerrain(packetWheel, roadsurf, mSuspension->IsWheelOnGround(wheelid));
            svc.SetTraction(packetWheel, pct);
            svc.SetOversteer(mSuspension->CalculateOversteerFactor());
            svc.SetUndersteer(mSuspension->CalculateUndersteerFactor());
            svc.SetWheelSlip(packetWheel, slip, -mSuspension->GetWheelSkid(wheelid));
            svc.SetWheelLoad(packetWheel, mSuspension->GetWheelLoad(wheelid));
            svc.SetWheelZForce(packetWheel, mSuspension->GetCompression(wheelid));

            OnServiceTire(svc, wheelid, packetWheel);
        }
    }
}

void SoundCar::OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) {}

SoundTraffic::SoundTraffic(const BehaviorParams &params, Sound::Context ctx)
    : SoundCar(params, ctx), //
      mTrailer(0) {
    LocateTrailer();
}

Behavior *SoundTraffic::Construct(const BehaviorParams &params) {
    IVehicle *vehicle = nullptr;
    static_cast<ISimable *>(params.fowner)->QueryInterface(&vehicle);
    bool haveVehicle = vehicle;
    if (!haveVehicle) {
        return nullptr;
    }

    UCrc32 vehicleClass = vehicle->GetVehicleClass();
    Sound::Context ctx = Sound::CONTEXT_TRAFFIC;
    if (vehicleClass == VehicleClass_TRACTOR) {
        ctx = Sound::CONTEXT_TRACTOR;
    } else if (vehicleClass == VehicleClass_TRAILER) {
        ctx = Sound::CONTEXT_TRAILER;
    }

    return new SoundTraffic(params, ctx);
}

void SoundTraffic::LocateTrailer() {
    IArticulatedVehicle *iarticulation = nullptr;

    mTrailer = 0;
    bool haveArticulation = GetOwner()->QueryInterface(&iarticulation);
    if (haveArticulation) {
        IVehicle *trailer = iarticulation->GetTrailer();
        if (trailer) {
            mTrailer = trailer->GetSimable()->GetWorldID();
        }
    }
}

void SoundTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        LocateTrailer();
    }
}

void SoundTraffic::OnService(SoundConn::Pkt_Car_Service &svc) {
    SoundCar::OnService(svc);
    svc.SetTrailer(mTrailer);
}

SoundCop::SoundCop(const BehaviorParams &params)
    : SoundCar(params, Sound::CONTEXT_COP) {
    GetOwner()->QueryInterface(&mPursuitAI);
    GetOwner()->QueryInterface(&mVehicleDamage);
}

Behavior *SoundCop::Construct(const BehaviorParams &params) {
    return new SoundCop(params);
}

void SoundCop::OnBehaviorChange(const UCrc32 &mechanic) {
    SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mPursuitAI);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mVehicleDamage);
    }
}

void SoundCop::OnService(SoundConn::Pkt_Car_Service &svc) {
    SoundCar::OnService(svc);

    if (mPursuitAI) {
        svc.SetSiren(mPursuitAI->GetSirenState());
        if (mPursuitAI->GetInFormation()) {
            svc.SetHotPursuit(true);
        }
        svc.SetTimeSinceSeen(mPursuitAI->GetTimeSinceTargetSeen());
    }

    if (mVehicleDamage && mVehicleDamage->IsLightDamaged(VehicleFX::LIGHT_COPS)) {
        svc.SetSiren(SIREN_DIE);
    }
}

SoundRacer::SoundRacer(const BehaviorParams &params, Sound::Context ctx)
    : SoundCar(params, ctx) {
    GetOwner()->QueryInterface(&mEngineDamage);
    GetOwner()->QueryInterface(&mSpikeDamage);
}

Behavior *SoundRacer::Construct(const BehaviorParams &params) {
    IVehicle *vehicle = nullptr;
    static_cast<ISimable *>(params.fowner)->QueryInterface(&vehicle);
    bool haveVehicle = vehicle;
    if (!haveVehicle) {
        return nullptr;
    }

    Sound::Context ctx = Sound::CONTEXT_AIRACER;
    if (vehicle->GetDriverClass() == DRIVER_HUMAN) {
        ctx = Sound::CONTEXT_PLAYER;
    }
    return new SoundRacer(params, ctx);
}

void SoundRacer::OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) {
    if (mSpikeDamage) {
        pkt.SetTireBlown(sndId, mSpikeDamage->GetTireDamage(wheelid));
    }
}

void SoundRacer::OnService(SoundConn::Pkt_Car_Service &svc) {
    SoundCar::OnService(svc);

    if (mEngineDamage) {
        if (mEngineDamage->IsBlown()) {
            svc.SetEngineBlown(1);
        }
        if (mEngineDamage->IsSabotaged()) {
            svc.SetEngineBlown(2);
        }
    }
}

void SoundRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mEngineDamage);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mSpikeDamage);
    }
}
