#include "Speed/Indep/Src/Generated/AttribSys/Classes/effects.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DrawVehicle.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x114
class DrawHeli : public DrawVehicle {
  public:
    typedef DrawVehicle Base;

    static Behavior *Construct(const BehaviorParams &params);

  protected:
    DrawHeli(const BehaviorParams &params);

    // Overrides: IModel
    void HidePart(const UCrc32 &name) override {}
    void ShowPart(const UCrc32 &name) override {}
    bool IsPartVisible(const UCrc32 &name) const override {
        return true;
    }

    // Overrides: IRenderable
    bool InView() const override {
        return this->mInView;
    }
    bool IsRenderable() const override {
        return CheckService(mRenderService) == Sim::CONNSTATUS_READY;
    }
    float DistanceToView() const override {
        return this->mDistanceToView;
    }
    void Reset() override {}
    void OnTaskSimulate(float dT) override {}

    // Overrides: IServiceable
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: IUnknown
    ~DrawHeli() override;

    // Overrides: ITaskable
    bool OnTask(HSIMTASK hTask, float dT) override;

    void OnService(RenderConn::Pkt_Heli_Service &pkt);

  private:
    HSIMSERVICE mRenderService;     // offset 0x88, size 0x4
    Sim::Effect mEffect;            // offset 0x8C, size 0x5C
    Attrib::Gen::effects mWash;     // offset 0xE8, size 0x14
    HSIMTASK mWashTask;             // offset 0xFC, size 0x4
    bool mInView;                   // offset 0x100, size 0x1
    float mDistanceToView;          // offset 0x104, size 0x4
    IAIHelicopter *mIAIHelicopter;  // offset 0x108, size 0x4
    IVehicle *mIVehicle;            // offset 0x10C, size 0x4
    ICollisionBody *mCollisionBody; // offset 0x110, size 0x4
};

BIND_BEHAVIOR_FACTORY(DrawHeli);

Behavior *DrawHeli::Construct(const BehaviorParams &params) {
    return new DrawHeli(params);
}

DrawHeli::DrawHeli(const BehaviorParams &params)
    : DrawVehicle(params),     //
      mRenderService(nullptr), //
      mEffect(0, nullptr),     //
      mWash(Attrib::StringToKey("heliwash"), 0, nullptr) {
    this->mInView = false;
    this->mDistanceToView = 0.0f;

    const char *model = GetVehicle()->GetVehicleAttributes().MODEL().GetString();
    if (model == nullptr) {
        model = "";
    }

    RenderConn::Pkt_Heli_Open pkt(model, this->GetOwner()->GetWorldID(), this->GetVehicle()->IsSpooled());
    this->mWashTask = this->AddTask(UCrc32("Physics"), 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(this->mWashTask, "HeliWash");
    this->mRenderService = this->OpenService(UCRC32_ECSTASY, &pkt);

    this->GetOwner()->QueryInterface(&this->mIVehicle);
    this->GetOwner()->QueryInterface(&this->mCollisionBody);
}

void DrawHeli::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mCollisionBody);
    }
    Behavior::OnBehaviorChange(mechanic);
}

DrawHeli::~DrawHeli() {
    this->RemoveTask(this->mWashTask);
    this->CloseService(this->mRenderService);
}

static const float Tweak_WashAltMin = 5.0f;
static const float Tweak_WashAltMax = 30.0f;

bool DrawHeli::OnTask(HSIMTASK hTask, float dT) {
    if (hTask == this->mWashTask) {
        if (!this->IsHidden() && this->mCollisionBody != nullptr) {
            const WWorldPos wpos = this->GetOwner()->GetWPos();
            if (wpos.OnValidFace()) {
                UMath::Vector3 position = this->GetVehicle()->GetPosition();
                float new_y = this->GetOwner()->GetWPos().HeightAtPoint(position);
                float altitude = position.y - new_y;
                position.y = new_y;

                UMath::Vector3 normal = UMath::Vector4To3(this->mCollisionBody->GetGroundNormal());
                float intensity = 1.0f - UMath::Ramp(altitude, Tweak_WashAltMin, Tweak_WashAltMax);
                if (intensity > 0.0f) {
                    UMath::Scale(normal, intensity);
                    this->mEffect.Pause(false);
                    this->mEffect.Set(this->mWash.GetConstCollection(), position, normal, nullptr, false, 0);
                } else {
                    this->mEffect.Pause(true);
                }
            } else {
                this->mEffect.Pause(true);
            }
        } else {
            this->mEffect.Stop();
        }

        return true;
    }

    return VehicleBehavior::OnTask(hTask, dT);
}

void DrawHeli::OnService(RenderConn::Pkt_Heli_Service &pkt) {
    this->mInView = pkt.InView();
    this->mDistanceToView = pkt.DistanceToView();
    if (this->mIVehicle != nullptr && this->mIVehicle->QueryInterface(&mIAIHelicopter)) {
        pkt.SetShadowScale(this->mIAIHelicopter->GetShadowScale());
        pkt.SetDustStormIntensity(this->mIAIHelicopter->GetDustStormIntensity());
    }
}

bool DrawHeli::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == this->mRenderService) {
        if (!this->IsHidden()) {
            this->OnService(*static_cast<RenderConn::Pkt_Heli_Service *>(pkt));
            return true;
        }
        this->mInView = false;
        return false;
    }
    return false;
}
