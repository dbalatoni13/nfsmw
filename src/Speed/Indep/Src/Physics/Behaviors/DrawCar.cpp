#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/DrawVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarRenderConn.h"

// total size: 0xC4
class DrawCar : public DrawVehicle {
  public:
    typedef DrawVehicle Base;
    typedef UTL::Std::map<UCrc32, int, _type_map> Parts;

  protected:
    DrawCar(const BehaviorParams &params, CarRenderUsage usage);

    ~DrawCar() override;

    // Overrides: IModel
    void HidePart(const UCrc32 &name) override;
    bool IsPartVisible(const UCrc32 &name) const override;
    void ShowPart(const UCrc32 &name) override;

    // Overrides: IRenderable
    bool InView() const override {
        return this->mInView;
    }
    bool IsRenderable() const override {
        return this->CheckService(this->mRenderService) == Sim::CONNSTATUS_READY;
    }
    float DistanceToView() const override {
        return this->mDistanceToView;
    }

    // Overrides: Behavior
    void Reset() override {}
    void OnTaskSimulate(float dT) override {}

    // Overrides: IServiceable
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: IModel
    void ReleaseChildModels() override;
    void ReleaseModel() override;
    void HideModel() override;
    bool IsHidden() const override;

    virtual void OnService(RenderConn::Pkt_Car_Service &pkt);

    HSIMSERVICE mRenderService; // offset 0x88, size 0x4

  private:
    ISuspension *mSuspension;           // offset 0x8C, size 0x4
    IDamageable *mDamage;               // offset 0x90, size 0x4
    IVehicle *mVehicle;                 // offset 0x94, size 0x4
    IDamageableVehicle *mVehicleDamage; // offset 0x98, size 0x4
    IRBVehicle *mRBVehicle;             // offset 0x9C, size 0x4
    INISCarControl *mNIS;               // offset 0xA0, size 0x4
    ISpikeable *mSpikeDamage;           // offset 0xA4, size 0x4
    Parts mParts;                       // offset 0xA8, size 0x10
    bool mInView;                       // offset 0xB8, size 0x1
    float mDistanceToView;              // offset 0xBC, size 0x4
    bool mHidden;                       // offset 0xC0, size 0x1
};

DrawCar::DrawCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawVehicle(params),      //
      mRenderService(nullptr),  //
      mParts(), mInView(false), //
      mDistanceToView(0.0f),    //
      mHidden(false) {
    this->GetOwner()->QueryInterface(&this->mVehicle);
    this->GetOwner()->QueryInterface(&this->mSuspension);
    this->GetOwner()->QueryInterface(&this->mDamage);
    this->GetOwner()->QueryInterface(&this->mSpikeDamage);
    this->GetOwner()->QueryInterface(&this->mVehicleDamage);
    this->GetOwner()->QueryInterface(&this->mRBVehicle);
    this->GetOwner()->QueryInterface(&this->mNIS);

    if (this->mSuspension != nullptr) {
        const char *model = this->GetVehicle()->GetVehicleAttributes().MODEL().GetString();
        if (model == nullptr) {
            model = "";
        }

        RenderConn::Pkt_Car_Open pkt(model, this->GetOwner()->GetWorldID(), usage, this->GetVehicle()->GetCustomizations(),
                                     this->GetOwner()->GetAttributes().GetCollection(), this->GetVehicle()->IsSpooled());
        this->mRenderService = this->OpenService(UCRC32_ECSTASY, &pkt);
    }
}

bool DrawCar::IsHidden() const {
    return this->mHidden || this->DrawVehicle::IsHidden() || this->IsPaused();
}

void DrawCar::HideModel() {
    this->DrawVehicle::HideModel();
    this->mHidden = true;
}

void DrawCar::ReleaseModel() {
    this->DrawVehicle::ReleaseModel();
    if (this->mRenderService != nullptr) {
        this->CloseService(this->mRenderService);
        this->mRenderService = nullptr;
    }
}

void DrawCar::ReleaseChildModels() {
    this->DrawVehicle::ReleaseChildModels();
    this->mParts.clear();
}

bool DrawCar::IsPartVisible(const UCrc32 &name) const {
    Parts::const_iterator iter = this->mParts.find(name);
    return iter == this->mParts.end();
}

void DrawCar::HidePart(const UCrc32 &name) {
    Parts::iterator iter = this->mParts.find(name);
    if (iter == this->mParts.end()) {
        this->mParts[name] = 1;
    } else {
        (*iter).second++;
    }
}

void DrawCar::ShowPart(const UCrc32 &name) {
    Parts::iterator iter = this->mParts.find(name);
    if (iter != this->mParts.end()) {
        (*iter).second--;
        if ((*iter).second < 1) {
            this->mParts.erase(iter);
        }
    }
}

void DrawCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
        this->GetOwner()->QueryInterface(&this->mNIS);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBVehicle);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mDamage);
        this->GetOwner()->QueryInterface(&this->mVehicleDamage);
        this->GetOwner()->QueryInterface(&this->mSpikeDamage);
    }
    Behavior::OnBehaviorChange(mechanic);
}

DrawCar::~DrawCar() {
    if (this->mRenderService != nullptr) {
        this->CloseService(this->mRenderService);
        this->mRenderService = nullptr;
    }
}

void DrawCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    this->mInView = pkt.InView();
    this->mDistanceToView = pkt.DistanceToView();

    if (this->mRBVehicle != nullptr) {
        switch (this->mRBVehicle->GetInvulnerability()) {
            case INVULNERABLE_FROM_MANUAL_RESET:
            case INVULNERABLE_FROM_RESET:
                pkt.SetFlashing(true);
                break;
            default:
                break;
        }
    }

    const VehicleFX::Maps *fxm = VehicleFX::GetMaps();
    while (fxm->id != VehicleFX::LIGHT_NONE) {
        if (fxm->marker != 0) {
            if (this->mVehicleDamage != nullptr && this->mVehicleDamage->IsLightDamaged(fxm->id)) {
                pkt.SetBrokenLightState(fxm->id, true);
            } else {
                pkt.SetLightState(fxm->id, this->mVehicle->IsGlareOn(fxm->id));
            }
        }
        ++fxm;
    }

    IDynamicsEntity *ide = nullptr;
    if (this->GetOwner()->QueryInterface(&ide) && Dynamics::Articulation::IsJoined(ide)) {
        pkt.SetExtraBodyRoll(0.0f);
        pkt.SetExtraBodyPitch(0.0f);
    } else if (this->mSuspension != nullptr) {
        const float motion = this->mSuspension->GetRenderMotion();
        pkt.SetExtraBodyPitch(motion);
        pkt.SetExtraBodyRoll(motion);
    }

    if (this->mDamage != nullptr) {
        pkt.SetDamage(this->mDamage->GetZoneDamage());
        pkt.SetHealth(this->mDamage->GetHealth());
    }

    for (Parts::iterator iter = this->mParts.begin(); iter != this->mParts.end(); iter++) {
        pkt.HidePart((*iter).first);
    }

    if (this->mSuspension != nullptr) {
        IRigidBody *irb = this->GetOwner()->GetRigidBody();

        for (unsigned int i = 0; i < this->mSuspension->GetNumWheels(); ++i) {
            const UMath::Vector3 &vel = this->mSuspension->GetWheelVelocity(i);
            float pos_x = this->mSuspension->GetWheelLocalPos(i).x;
            float steer = UMath::Clamp(ANGLE2RAD(this->mSuspension->GetWheelSteer(i)), -UMath::PI, UMath::PI);
            float compression = this->mSuspension->GetCompression(i) - this->mSuspension->GetRideHeight(i) + this->mSuspension->GetWheelRadius(i);
            float av = this->mSuspension->GetWheelAngularVelocity(i);
            float radius = this->mSuspension->GetWheelRadius(i);
            float slip = 0.0f;
            float skid = 0.0f;
            bool onground = false;

            if (this->mSuspension->IsWheelOnGround(i)) {
                skid = UMath::Abs(this->mSuspension->GetWheelSkid(i));
                slip = UMath::Abs(this->mSuspension->GetWheelSlip(i));

                float tolerance = this->mSuspension->GetToleratedSlip(i);
                if (slip > tolerance) {
                    slip -= tolerance;
                } else {
                    slip = 0.0f;
                }

                onground = true;
            }

            RenderConn::eTireIdx idx;
            if (Physics::Wheels::IsFront(i)) {
                idx = Physics::Wheels::IsRight(i) ? RenderConn::TIRE_FR : RenderConn::TIRE_FL;
                pkt.SetSteering(idx, -steer);
            } else {
                idx = Physics::Wheels::IsRight(i) ? RenderConn::TIRE_RR : RenderConn::TIRE_RL;
            }

            if (this->mSpikeDamage != nullptr && this->mSpikeDamage->GetTireDamage(i) == TIRE_DAMAGE_BLOWN) {
                pkt.SetTireBlowOut(idx);
            }

            pkt.SetWheelSlip(idx, slip);
            pkt.SetWheelSpeed(idx, av * radius);
            pkt.SetWheelCompression(idx, compression);
            pkt.SetWheelSkid(idx, skid);
            pkt.SetWheelOnGround(idx, onground);
        }
    }

    if (this->mNIS != nullptr) {
        pkt.SetAnimatedPitch(this->mNIS->GetAnimPitch());
        pkt.SetAnimatedRoll(this->mNIS->GetAnimRoll());
        pkt.SetAnimatedShake(this->mNIS->GetAnimShake());
    }
}

bool DrawCar::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == this->mRenderService) {
        if (!this->IsHidden()) {
            this->OnService(*static_cast<RenderConn::Pkt_Car_Service *>(pkt));
            return true;
        }
        this->mInView = false;
    }
    return false;
}

// total size: 0xC4
class DrawTraffic : public DrawCar {
  public:
    typedef DrawCar Base;

    ~DrawTraffic() override {}

    static Behavior *Construct(const BehaviorParams &params);

  protected:
    DrawTraffic(const BehaviorParams &params);
};

BIND_BEHAVIOR_FACTORY(DrawTraffic);

Behavior *DrawTraffic::Construct(const BehaviorParams &params) {
    return new DrawTraffic(params);
}

DrawTraffic::DrawTraffic(const BehaviorParams &params) : DrawCar(params, CarRenderUsage_AITraffic) {}

// total size: 0xCC
class DrawPerformanceCar : public DrawCar {
  public:
    typedef DrawCar Base;

    ~DrawPerformanceCar() override {}

  protected:
    DrawPerformanceCar(const BehaviorParams &params, CarRenderUsage usage);

    // Overrides: DrawCar
    void OnService(RenderConn::Pkt_Car_Service &pkt) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    ITransmission *mTransmission; // offset 0xC4, size 0x4
    IEngine *mEngine;             // offset 0xC8, size 0x4
};

DrawPerformanceCar::DrawPerformanceCar(const BehaviorParams &params, CarRenderUsage usage) : DrawCar(params, usage) {
    this->GetOwner()->QueryInterface(&this->mTransmission);
    this->GetOwner()->QueryInterface(&this->mEngine);
}

void DrawPerformanceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mTransmission);
        this->GetOwner()->QueryInterface(&this->mEngine);
    }
    this->DrawCar::OnBehaviorChange(mechanic);
}

void DrawPerformanceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    this->DrawCar::OnService(pkt);

    if (this->mEngine != nullptr && this->mTransmission != nullptr) {
        pkt.SetGear(this->mTransmission->GetGear());
        pkt.SetShiftStatus(this->mTransmission->GetShiftStatus());
        pkt.SetNOS(this->mEngine->IsNOSEngaged());

        if (this->mTransmission->GetGear() < 2 || this->GetVehicle()->IsStaging()) {
            float rpm_ratio = UMath::Ramp(this->mEngine->GetRPM(), this->mEngine->GetMinRPM(), this->mEngine->GetMaxRPM());
            float max_power = this->mEngine->GetMaxHorsePower();
            float min_power = this->mEngine->GetMinHorsePower();
            float power_ratio = UMath::Ramp(this->mEngine->GetHorsePower(), min_power, max_power);

            pkt.SetEnginePower(power_ratio);
            pkt.SetEngineSpeed(rpm_ratio);
        }
    }
}

// total size: 0xCC
class DrawNISCar : public DrawPerformanceCar {
  public:
    typedef DrawPerformanceCar Base;

    ~DrawNISCar() override {}

    static Behavior *Construct(const BehaviorParams &params);

  protected:
    DrawNISCar(const BehaviorParams &params);
};

BIND_BEHAVIOR_FACTORY(DrawNISCar);

Behavior *DrawNISCar::Construct(const BehaviorParams &params) {
    return new DrawNISCar(params);
}

DrawNISCar::DrawNISCar(const BehaviorParams &params) : DrawPerformanceCar(params, carRenderUsage_NISCar) {}

// total size: 0xCC
class DrawCopCar : public DrawPerformanceCar {
  public:
    typedef DrawPerformanceCar Base;

    ~DrawCopCar() override {}

    static Behavior *Construct(const BehaviorParams &params);

  protected:
    DrawCopCar(const BehaviorParams &params);
};

BIND_BEHAVIOR_FACTORY(DrawCopCar);

Behavior *DrawCopCar::Construct(const BehaviorParams &params) {
    return new DrawCopCar(params);
}

DrawCopCar::DrawCopCar(const BehaviorParams &params) : DrawPerformanceCar(params, CarRenderUsage_AICop) {}

// total size: 0xD0

class DrawRaceCar : public DrawPerformanceCar {
  public:
    typedef DrawPerformanceCar Base;

    ~DrawRaceCar() override {}

    static Behavior *Construct(const BehaviorParams &params);

  protected:
    DrawRaceCar(const BehaviorParams &params, CarRenderUsage usage);

    // Overrides: DrawCar
    void OnService(RenderConn::Pkt_Car_Service &pkt) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IEngineDamage *mEngineDamage; // offset 0xCC, size 0x4
};

BIND_BEHAVIOR_FACTORY(DrawRaceCar);

Behavior *DrawRaceCar::Construct(const BehaviorParams &params) {
    ISimable *isimable = static_cast<ISimable *>(params.fowner);
    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        return nullptr;
    }

    CarRenderUsage usage;
    switch (ivehicle->GetDriverClass()) {
        case DRIVER_RACER:
            usage = CarRenderUsage_AIRacer;
            break;
        case DRIVER_HUMAN:
            usage = CarRenderUsage_Player;
            break;
        case DRIVER_REMOTE:
            usage = CarRenderUsage_RemotePlayer;
            break;
        default:
            usage = CarRenderUsage_AIRacer;
            break;
    }

    return new DrawRaceCar(params, usage);
}

DrawRaceCar::DrawRaceCar(const BehaviorParams &params, CarRenderUsage usage) : DrawPerformanceCar(params, usage) {
    this->GetOwner()->QueryInterface(&this->mEngineDamage);
}

void DrawRaceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mEngineDamage);
    }
    this->DrawPerformanceCar::OnBehaviorChange(mechanic);
}

void DrawRaceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    this->DrawPerformanceCar::OnService(pkt);
    if (this->mEngineDamage != nullptr) {
        pkt.SetEngineBlown(this->mEngineDamage->IsBlown());
    }
}
