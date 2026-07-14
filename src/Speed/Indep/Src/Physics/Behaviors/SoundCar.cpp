#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICarAudio.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// total size: 0x9C
class SoundCar : public VehicleBehavior, public ICarAudio, public IAudible {
  public:
    SoundCar(const BehaviorParams &params, Sound::Context ctx);

  protected:
    ~SoundCar() override;

    void Reset() override {}
    void OnTaskSimulate(float dT) override {}
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    bool IsAudible() const override {
        return this->CheckService(this->mSoundService) == Sim::CONNSTATUS_READY;
    }

    // Overrides: ICarAudio
    Rpm GetRPM() const override {
        return this->mSoundRPM;
    }

    virtual void OnService(SoundConn::Pkt_Car_Service &svc);

    virtual void OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) {}

  private:
    HSIMSERVICE mSoundService;                         // offset 0x60, size 0x4
    IEngine *mEngine;                                  // offset 0x64, size 0x4
    IInput *mInput;                                    // offset 0x68, size 0x4
    ITransmission *mTransmission;                      // offset 0x6C, size 0x4
    ISuspension *mSuspension;                          // offset 0x70, size 0x4
    IVehicleAI *mAI;                                   // offset 0x74, size 0x4
    IHumanAI *mHumanAI;                                // offset 0x78, size 0x4
    IDamageable *mDamage;                              // offset 0x7C, size 0x4
    Rpm mSoundRPM;                                     // offset 0x80, size 0x4
    INISCarControl *mNIS;                              // offset 0x84, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::engine> mEngineInfo; // offset 0x88, size 0x14
};

SoundCar::SoundCar(const BehaviorParams &params, Sound::Context ctx)
    : VehicleBehavior(params, 0), //
      ICarAudio(params.fowner),   //
      IAudible(params.fowner),    //
      mSoundService(nullptr),     //
      mSoundRPM(0.0f),            //
      mEngineInfo(this, 0) {
    this->GetOwner()->QueryInterface(&this->mEngine);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mTransmission);
    this->GetOwner()->QueryInterface(&this->mSuspension);
    this->GetOwner()->QueryInterface(&this->mDamage);
    this->GetOwner()->QueryInterface(&this->mAI);
    this->GetOwner()->QueryInterface(&this->mHumanAI);
    this->GetOwner()->QueryInterface(&this->mNIS);

    this->mSoundRPM = this->mEngineInfo.IDLE();

    SoundConn::Pkt_Car_Open pkt(this->GetOwner()->GetAttributes().GetConstCollection(), this->GetOwner()->GetWorldID(), ctx,
                                this->GetVehicle()->IsSpooled(), this->GetOwner()->GetInstanceHandle());

    this->mSoundService = this->OpenService(UCRC32_EAXSOUND, &pkt);
}

static const float Tweak_AudioSlipToleranceScale = 0.2f;

SoundCar::~SoundCar() {
    if (this->mSoundService != nullptr) {
        this->CloseService(this->mSoundService);
    }
}

void SoundCar::OnService(SoundConn::Pkt_Car_Service &svc) {
    if (this->mEngine != nullptr && this->mTransmission != nullptr) {
        float rpm = this->mEngine->GetRPM();
        float minrpm = this->mEngineInfo.IDLE();
        float range = this->mEngineInfo.RED_LINE() - minrpm;
        float pctrpm = 0.0f;

        if (range > 0.0f) {
            pctrpm = UMath::Clamp((rpm - minrpm) / range, 0.0f, 1.0f);
        }

        this->mSoundRPM = svc.GetAudibleRPMPercent() * range + minrpm;
        svc.SetRPMPercent(pctrpm);
        svc.SetNOSFlag(this->mEngine->IsNOSEngaged());
        svc.SetNOSCapacity(this->mEngine->GetNOSCapacity());

        int gear = static_cast<int>(this->mTransmission->GetGear());
        svc.SetGear(gear);
    }

    if (this->mInput != nullptr) {
        float throttle = UMath::Clamp(this->mInput->GetControls().fGas, 0.0f, 1.0f);
        float brake = UMath::Clamp(this->mInput->GetControls().fBrake, 0.0f, 1.0f);
        float ebrake = UMath::Clamp(this->mInput->GetControls().fHandBrake, 0.0f, 1.0f);
        float steering = UMath::Clamp(this->mInput->GetControls().fSteering, -1.0f, 1.0f);

        svc.SetThrottlePercent(throttle);
        svc.SetBrakePercent(brake);
        svc.SetEBrakePercent(ebrake);
        svc.SetSteering(steering);

        if (this->mNIS != nullptr) {
            svc.SetControlSource(Sound::CONTROL_NIS);
        } else if (this->GetVehicle()->GetDriverClass() == DRIVER_REMOTE) {
            svc.SetControlSource(Sound::CONTROL_ONLINE);
        } else if (this->mHumanAI != nullptr && !this->mHumanAI->GetAiControl()) {
            svc.SetControlSource(Sound::CONTROL_HUMAN);
        } else if (this->mAI != nullptr) {
            svc.SetControlSource(Sound::CONTROL_AI);
            svc.SetDesiredSpeed(this->mAI->GetDriveSpeed());
        }
    }

    if (this->mDamage != nullptr) {
        if (this->mDamage->IsDestroyed()) {
            svc.SetHealth(0.0f);
        } else {
            svc.SetHealth(this->mDamage->GetHealth());
        }
    }

    svc.SetSlipAngle(ANGLE2RAD(this->GetVehicle()->GetSlipAngle()));

    if (this->mSuspension != nullptr) {
        for (unsigned int i = 0; i < this->mSuspension->GetNumWheels(); i++) {
            const UMath::Vector3 &vel = this->mSuspension->GetWheelVelocity(i);
            float pos_x = this->mSuspension->GetWheelLocalPos(i).x;
            float pct = this->mSuspension->GetWheelTraction(i);
            Sound::WheelConfig idx;

            if (Physics::Wheels::IsFront(i)) {
                idx = Sound::EAX4WD_FL;
                if (Physics::Wheels::IsRight(i)) {
                    idx = Sound::EAX4WD_FR;
                }
            } else if (Physics::Wheels::IsRight(i)) {
                idx = Sound::EAX4WD_RR;
            } else {
                idx = Sound::EAX4WD_RL;
            }

            float slip = this->mSuspension->GetWheelSlip(i);
            float tolerance = this->mSuspension->GetToleratedSlip(i) * Tweak_AudioSlipToleranceScale;

            if (slip > tolerance) {
                slip -= tolerance;
            } else if (slip < -tolerance) {
                slip += tolerance;
            } else {
                slip = 0.0f;
            }

            SimSurface roadsurf = this->mSuspension->GetWheelRoadSurface(i);
            svc.SetWheelTerrain(idx, roadsurf, this->mSuspension->IsWheelOnGround(i));
            svc.SetTraction(idx, pct);
            svc.SetOversteer(this->mSuspension->CalculateOversteerFactor());
            svc.SetUndersteer(this->mSuspension->CalculateUndersteerFactor());
            svc.SetWheelSlip(idx, slip, -this->mSuspension->GetWheelSkid(i));
            svc.SetWheelLoad(idx, this->mSuspension->GetWheelLoad(i));
            svc.SetWheelZForce(idx, this->mSuspension->GetCompression(i));

            this->OnServiceTire(svc, i, idx);
        }
    }
}

void SoundCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mEngine);
        this->GetOwner()->QueryInterface(&this->mTransmission);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
        this->GetOwner()->QueryInterface(&this->mNIS);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mDamage);
    } else if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mAI);
        this->GetOwner()->QueryInterface(&this->mHumanAI);
    }
}

bool SoundCar::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (!INIS::Exists() && hCon == this->mSoundService && !this->IsPaused()) {
        this->OnService(*static_cast<SoundConn::Pkt_Car_Service *>(pkt));
        return true;
    }
    return false;
}

// total size: 0xA0

class SoundTraffic : public SoundCar {
  public:
    typedef SoundCar Base;

    ~SoundTraffic() override {}

    SoundTraffic(const BehaviorParams &params, Sound::Context ctx);

    static Behavior *Construct(const BehaviorParams &params);

    // Overrides: SoundCar
    void OnService(SoundConn::Pkt_Car_Service &svc) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    void LocateTrailer();

    WUID mTrailer; // offset 0x9C, size 0x4
};

BIND_BEHAVIOR_FACTORY(SoundTraffic);

SoundTraffic::SoundTraffic(const BehaviorParams &params, Sound::Context ctx)
    : SoundCar(params, ctx), //
      mTrailer(0) {
    this->LocateTrailer();
}

Behavior *SoundTraffic::Construct(const BehaviorParams &params) {
    ISimable *isimable = params.fowner;
    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        return nullptr;
    }

    Sound::Context ctx = Sound::CONTEXT_TRAFFIC;
    UCrc32 vehicle_class = ivehicle->GetVehicleClass();
    if (vehicle_class == VehicleClass::TRACTOR) {
        ctx = Sound::CONTEXT_TRACTOR;
    } else if (vehicle_class == VehicleClass::TRAILER) {
        ctx = Sound::CONTEXT_TRAILER;
    }

    return new SoundTraffic(params, ctx);
}

void SoundTraffic::LocateTrailer() {
    IArticulatedVehicle *iarticulation = nullptr;

    this->mTrailer = 0;
    if (this->GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *trailer = iarticulation->GetTrailer();
        if (trailer != nullptr) {
            this->mTrailer = trailer->GetSimable()->GetWorldID();
        }
    }
}

void SoundTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    this->SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->LocateTrailer();
    }
}

void SoundTraffic::OnService(SoundConn::Pkt_Car_Service &svc) {
    this->SoundCar::OnService(svc);
    svc.SetTrailer(this->mTrailer);
}

// total size: 0xA4
class SoundCop : public SoundCar {
  public:
    typedef SoundCar Base;

    ~SoundCop() override {}

    static Behavior *Construct(const BehaviorParams &params);

    // Overrides: SoundCar
    void OnService(SoundConn::Pkt_Car_Service &svc) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    SoundCop(const BehaviorParams &params);

    IPursuitAI *mPursuitAI;             // offset 0x9C, size 0x4
    IDamageableVehicle *mVehicleDamage; // offset 0xA0, size 0x4
};

BIND_BEHAVIOR_FACTORY(SoundCop);

SoundCop::SoundCop(const BehaviorParams &params) : SoundCar(params, Sound::CONTEXT_COP) {
    this->GetOwner()->QueryInterface(&this->mPursuitAI);
    this->GetOwner()->QueryInterface(&this->mVehicleDamage);
}

Behavior *SoundCop::Construct(const BehaviorParams &params) {
    return new SoundCop(params);
}

void SoundCop::OnBehaviorChange(const UCrc32 &mechanic) {
    this->SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mPursuitAI);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mVehicleDamage);
    }
}

void SoundCop::OnService(SoundConn::Pkt_Car_Service &svc) {
    this->SoundCar::OnService(svc);

    if (this->mPursuitAI != nullptr) {
        svc.SetSiren(this->mPursuitAI->GetSirenState());
        if (this->mPursuitAI->GetInFormation()) {
            svc.SetHotPursuit(true);
        }
        svc.SetTimeSinceSeen(this->mPursuitAI->GetTimeSinceTargetSeen());
    }

    if (this->mVehicleDamage != nullptr && this->mVehicleDamage->IsLightDamaged(VehicleFX::LIGHT_COPS)) {
        svc.SetSiren(Sound::SIREN_DIE);
    }
}

// total size: 0xA4

class SoundRacer : public SoundCar {
  public:
    typedef SoundCar Base;

    ~SoundRacer() override {}

    SoundRacer(const BehaviorParams &params, enum Sound::Context ctx);

    static Behavior *Construct(const BehaviorParams &params);

    // Overrides: SoundCar
    void OnService(SoundConn::Pkt_Car_Service &svc) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: SoundCar
    void OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, enum Sound::WheelConfig sndId) override;

  private:
    IEngineDamage *mEngineDamage; // offset 0x9C, size 0x4
    ISpikeable *mSpikeDamage;     // offset 0xA0, size 0x4
};

BIND_BEHAVIOR_FACTORY(SoundRacer);

SoundRacer::SoundRacer(const BehaviorParams &params, Sound::Context ctx) : SoundCar(params, ctx) {
    this->GetOwner()->QueryInterface(&this->mEngineDamage);
    this->GetOwner()->QueryInterface(&this->mSpikeDamage);
}

Behavior *SoundRacer::Construct(const BehaviorParams &params) {
    ISimable *isimable = params.fowner;
    IVehicle *ivehicle = nullptr;
    if (!isimable->QueryInterface(&ivehicle)) {
        return nullptr;
    }

    DriverClass driver_class = ivehicle->GetDriverClass();
    Sound::Context ctx = Sound::CONTEXT_AIRACER;
    if (driver_class == DRIVER_HUMAN) {
        ctx = Sound::CONTEXT_PLAYER;
    }
    return new SoundRacer(params, ctx);
}

void SoundRacer::OnServiceTire(SoundConn::Pkt_Car_Service &pkt, unsigned int wheelid, Sound::WheelConfig sndId) {
    if (this->mSpikeDamage != nullptr) {
        pkt.SetTireBlown(sndId, this->mSpikeDamage->GetTireDamage(wheelid));
    }
}

void SoundRacer::OnService(SoundConn::Pkt_Car_Service &svc) {
    this->SoundCar::OnService(svc);

    if (this->mEngineDamage != nullptr) {
        if (this->mEngineDamage->IsBlown()) {
            svc.SetEngineBlown(1);
        }
        if (this->mEngineDamage->IsSabotaged()) {
            svc.SetEngineBlown(2);
        }
    }
}

void SoundRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    this->SoundCar::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mEngineDamage);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mSpikeDamage);
    }
}
