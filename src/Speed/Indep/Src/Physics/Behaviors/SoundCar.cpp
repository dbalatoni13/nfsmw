#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICarAudio.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IInput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
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

enum ControlSource {
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 2,
    CONTROL_NIS = 3,
    CONTROL_ONLINE = 4,
};

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
// TODO(GC): packet tail after mWheelOnGround still needs exact field names from DWARF.
class Pkt_Car_Service : public Sim::Packet {
  public:
    bool &NOSFlag() {
        return *reinterpret_cast<bool *>(mTail + 0x10);
    }

    float &NOSCapacity() {
        return *reinterpret_cast<float *>(mTail + 0x14);
    }

    float &DesiredSpeed() {
        return *reinterpret_cast<float *>(mTail + 0x24);
    }

    ControlSource &GetControlSource() {
        return *reinterpret_cast<ControlSource *>(mTail + 0x28);
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

  private:
    unsigned char mTail[0x2C];   // offset 0xDC, size 0x2C
};

}; // namespace SoundConn

namespace {

float ClampZeroOne(float value) {
    if (value > 1.0f) {
        return 1.0f;
    }
    if (value < 0.0f) {
        return 0.0f;
    }
    return value;
}

float ClampSignedOne(float value) {
    if (value > 1.0f) {
        return 1.0f;
    }
    if (value < -1.0f) {
        return -1.0f;
    }
    return value;
}

float ApplySlipDeadzone(float slip, float tolerated) {
    float deadzone = tolerated * 0.2f;
    if (deadzone < slip) {
        return slip - deadzone;
    }
    if (slip < -deadzone) {
        return slip + deadzone;
    }
    return 0.0f;
}

Sound::WheelConfig GetPacketWheel(unsigned int wheelid) {
    if (wheelid < 2) {
        return static_cast<Sound::WheelConfig>(wheelid & 1);
    }
    if ((wheelid & 1) != 0) {
        return Sound::EAX4WD_RR;
    }
    return Sound::EAX4WD_RL;
}

}; // namespace

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

SoundCar::SoundCar(const BehaviorParams &params, Sound::Context ctx)
    : VehicleBehavior(params, 0), //
      ICarAudio(params.fowner), //
      IAudible(params.fowner), //
      mSoundService(nullptr), //
      mEngine(nullptr), //
      mInput(nullptr), //
      mTransmission(nullptr), //
      mSuspension(nullptr), //
      mAI(nullptr), //
      mHumanAI(nullptr), //
      mDamage(nullptr), //
      mSoundRPM(0.0f), //
      mNIS(nullptr), //
      mEngineInfo(params.fowner, 0) {
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
        float idle = mEngineInfo->IDLE();
        float redLineRange = mEngineInfo->RED_LINE() - idle;
        float rpmPercent = 0.0f;

        if (redLineRange > 0.0f) {
            rpmPercent = ClampZeroOne((mEngine->GetRPM() - idle) / redLineRange);
        }

        mSoundRPM = svc.mAudibleRPMPct * redLineRange + idle;
        svc.mRPMPercent = rpmPercent;
        svc.NOSFlag() = mEngine->IsNOSEngaged();
        svc.NOSCapacity() = mEngine->GetNOSCapacity();
        svc.mGear = static_cast<int>(mTransmission->GetGear());
    }

    if (mInput) {
        InputControls &controls = mInput->GetControls();

        svc.mThrottlePercent = ClampZeroOne(controls.fGas);
        svc.mBrakePercent = ClampZeroOne(controls.fBrake);
        svc.mEBrakePercent = ClampZeroOne(controls.fHandBrake);
        svc.mSteering = ClampSignedOne(controls.fSteering);

        if (mNIS) {
            svc.GetControlSource() = CONTROL_NIS;
        } else if (GetVehicle() && GetVehicle()->GetDriverClass() == DRIVER_REMOTE) {
            svc.GetControlSource() = CONTROL_ONLINE;
        } else if (mHumanAI && !mHumanAI->GetAiControl()) {
            svc.GetControlSource() = CONTROL_HUMAN;
        } else if (mAI) {
            svc.GetControlSource() = CONTROL_AI;
            svc.DesiredSpeed() = mAI->GetDriveSpeed();
        } else {
            svc.GetControlSource() = CONTROL_NONE;
        }
    }

    if (mDamage) {
        if (mDamage->IsDestroyed()) {
            svc.mHealth = 0.0f;
        } else {
            svc.mHealth = mDamage->GetHealth();
        }
    }

    if (GetVehicle()) {
        svc.mSlipAngle = GetVehicle()->GetSlipAngle() * 6.2831855f;
    }

    if (mSuspension) {
        for (unsigned int wheelid = 0; wheelid < mSuspension->GetNumWheels(); ++wheelid) {
            Sound::WheelConfig packetWheel = GetPacketWheel(wheelid);

            svc.mTractionPct[packetWheel] = mSuspension->GetWheelTraction(wheelid);
            svc.mWheelTerrain[packetWheel] = mSuspension->GetWheelRoadSurface(wheelid);
            svc.mWheelOnGround[packetWheel] = mSuspension->IsWheelOnGround(wheelid);
            svc.mWheelLoad[packetWheel] = mSuspension->GetWheelLoad(wheelid);
            svc.mWheelCompression[packetWheel] = mSuspension->GetCompression(wheelid);
            svc.mOversteer = mSuspension->CalculateOversteerFactor();
            svc.mUndersteer = mSuspension->CalculateUndersteerFactor();
            svc.mWheelSlip[packetWheel].x = ApplySlipDeadzone(mSuspension->GetWheelSlip(wheelid), mSuspension->GetToleratedSlip(wheelid));
            svc.mWheelSlip[packetWheel].y = -mSuspension->GetWheelSkid(wheelid);

            OnServiceTire(svc, wheelid, packetWheel);
        }
    }
}

void SoundCar::OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) {}
