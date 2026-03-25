#include "DrawVehicle.h"

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

namespace RenderConn {

class Pkt_Heli_Open : public Sim::Packet {
  public:
    Pkt_Heli_Open(const char *modelname, WUID worldid, bool spool_load)
        : mModelHash(bStringHash(modelname)), //
          mWorldID(worldid),                  //
          mSpoolLoad(spool_load) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "HeliRenderConn";
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
    unsigned int mModelHash; // offset 0x4, size 0x4
    WUID mWorldID;           // offset 0x8, size 0x4
    bool mSpoolLoad;         // offset 0xC, size 0x1
};

class Pkt_Heli_Service : public Sim::Packet {
  public:
    bool mInView;           // offset 0x4, size 0x1
    float mDistanceToView;  // offset 0x8, size 0x4
    float mShadowScale;     // offset 0xC, size 0x4
    float mDustStorm;       // offset 0x10, size 0x4
};

} // namespace RenderConn

static inline const Attrib::Collection *GetHeliWashCollection() {
    unsigned int collectionKey = Attrib::StringToKey("heliwash");
    return Attrib::FindCollection(Attrib::Gen::effects::ClassKey(), collectionKey);
}

Behavior *DrawHeli::Construct(const BehaviorParams &params) {
    return new DrawHeli(params);
}

DrawHeli::DrawHeli(const BehaviorParams &params)
    : DrawVehicle(params), //
      mRenderService(nullptr), //
      mEffect(0, nullptr), //
      mWash(GetHeliWashCollection(), 0, nullptr), //
      mInView(false), //
      mDistanceToView(0.0f), //
      mIAIHelicopter(nullptr) {
    mWash.SetDefaultLayout(0x10);

    const char *model = GetVehicle()->GetVehicleAttributes().MODEL().GetString();
    if (!model) {
        model = "";
    }

    RenderConn::Pkt_Heli_Open pkt(model, GetOwner()->GetWorldID(), GetVehicle()->IsSpooled());
    mWashTask = AddTask(UCrc32(stringhash32("Physics")), 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mWashTask, "HeliWash");
    mRenderService = OpenService(UCrc32(0x804c146e), &pkt);

    GetOwner()->QueryInterface(&mIVehicle);
    GetOwner()->QueryInterface(&mCollisionBody);
}

void DrawHeli::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
    }
}

DrawHeli::~DrawHeli() {
    RemoveTask(mWashTask);
    CloseService(mRenderService);
}

void DrawHeli::OnService(RenderConn::Pkt_Heli_Service &pkt) {
    mInView = pkt.mInView;
    mDistanceToView = pkt.mDistanceToView;
    if (mIVehicle) {
        if (mIVehicle->QueryInterface(&mIAIHelicopter)) {
            pkt.mShadowScale = mIAIHelicopter->GetShadowScale();
            pkt.mDustStorm = mIAIHelicopter->GetDustStormIntensity();
        }
    }
}

bool DrawHeli::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == mRenderService) {
        if (!static_cast<IModel *>(this)->IsHidden()) {
            OnService(*static_cast<RenderConn::Pkt_Heli_Service *>(pkt));
            return true;
        }
        mInView = false;
        return false;
    }
    return false;
}

void DrawHeli::HidePart(const UCrc32 &name) {}

void DrawHeli::ShowPart(const UCrc32 &name) {}

bool DrawHeli::IsPartVisible(const UCrc32 &name) const {
    return true;
}

bool DrawHeli::InView() const {
    return mInView;
}

bool DrawHeli::IsRenderable() const {
    return CheckService(mRenderService) == Sim::CONNSTATUS_READY;
}

float DrawHeli::DistanceToView() const {
    return mDistanceToView;
}

void DrawHeli::Reset() {}

void DrawHeli::OnTaskSimulate(float dT) {}

bool DrawHeli::OnTask(HSIMTASK hTask, float dT) {
    if (hTask == mWashTask) {
        if (!static_cast<IModel *>(this)->IsHidden() && mCollisionBody) {
            WWorldPos wpos = GetOwner()->GetWPos();
            if (wpos.OnValidFace()) {
                UMath::Vector3 position = GetVehicle()->GetPosition();
                float altitude = position.y;
                position.y = wpos.HeightAtPoint(position);
                altitude -= position.y;

                UMath::Vector3 normal = UMath::Vector4To3(mCollisionBody->GetGroundNormal());
                float intensity = 1.0f - UMath::Ramp(altitude, 5.0f, 30.0f);
                if (intensity > 0.0f) {
                    UMath::Scale(normal, intensity);
                    mEffect.Pause(false);
                    mEffect.Set(mWash.GetConstCollection(), position, normal, nullptr, false, 0);
                    return true;
                }
            }

            mEffect.Pause(true);
        } else {
            mEffect.Stop();
        }

        return true;
    }
    return false;
}
