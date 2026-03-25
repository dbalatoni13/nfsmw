#include "DrawVehicle.h"

#include "Speed/Indep/Src/FE/FECustomizationRecord.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

namespace RenderConn {

class Pkt_Car_Open : public Sim::Packet {
  public:
    Pkt_Car_Open(const char *modelname, WUID worldid, CarRenderUsage usage, const FECustomizationRecord *customizations, unsigned int physicskey,
                 bool spool_load)
        : mModelHash(bStringHash(modelname)), //
          mWorldID(worldid),                  //
          mUsage(usage),                      //
          mCustomizations(customizations),    //
          mPhysicsKey(physicskey),            //
          mSpoolLoad(spool_load) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "CarRenderConn";
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
    unsigned int mModelHash;
    WUID mWorldID;
    CarRenderUsage mUsage;
    const FECustomizationRecord *mCustomizations;
    unsigned int mPhysicsKey;
    bool mSpoolLoad;
};

class Pkt_Car_Service : public Sim::Packet {
  public:
    void HidePart(const UCrc32 &partname);

    bool InView() const {
        return mInView;
    }

    float DistanceToView() const {
        return mDistanceToView;
    }

    void SetFlashing(bool b) {
        mFlashing = b;
    }

    void SetBrokenLightState(VehicleFX::ID idx, bool on) {
        if (on) {
            mBrokenLights |= idx;
        } else {
            mBrokenLights &= ~idx;
        }
    }

    void SetLightState(VehicleFX::ID idx, bool on) {
        if (on) {
            mLights |= idx;
        } else {
            mLights &= ~idx;
        }
    }

    void SetExtraBodyRoll(float amount) {
        mExtraBodyRoll = amount;
    }

    void SetExtraBodyPitch(float amount) {
        mExtraBodyPitch = amount;
    }

    void SetDamage(const DamageZone::Info &dmg) {
        mDamageInfo = dmg;
    }

    void SetHealth(float amount) {
        mHealth = amount;
    }

    void SetSteering(eTireIdx idx, float angle) {
        mSteering[idx] = angle;
    }

    void SetTireBlowOut(eTireIdx idx) {
        mBlowOuts |= 1 << idx;
    }

    void SetWheelSlip(eTireIdx idx, float slip) {
        mTireSlip[idx] = slip;
    }

    void SetWheelSpeed(eTireIdx idx, float speed) {
        mWheelSpeed[idx] = speed;
    }

    void SetWheelCompression(eTireIdx idx, float compression) {
        mCompressions[idx] = compression;
    }

    void SetWheelSkid(eTireIdx idx, float skid) {
        mTireSkid[idx] = skid;
    }

    void SetWheelOnGround(eTireIdx idx, bool b) {
        if (b) {
            mGroundState |= 1 << idx;
        } else {
            mGroundState &= ~(1 << idx);
        }
    }

    void SetAnimatedPitch(float amount) {
        mAnimatedCarPitch = amount;
    }

    void SetAnimatedRoll(float amount) {
        mAnimatedCarRoll = amount;
    }

    void SetAnimatedShake(float amount) {
        mAnimatedCarShake = amount;
    }

    float mCompressions[4];
    float mWheelSpeed[4];
    float mTireSkid[4];
    float mTireSlip[4];
    float mSteering[2];
    int mGroundState;
    DamageZone::Info mDamageInfo;
    unsigned int mPartState[3];
    unsigned int mLights;
    unsigned int mBrokenLights;
    bool mInView;
    float mDistanceToView;
    bool mFlashing;
    bool mNos;
    bool mEngineBlown;
    ShiftStatus mShift;
    GearID mGear;
    float mEnginePower;
    float mEngineSpeed;
    float mExtraBodyRoll;
    float mExtraBodyPitch;
    int mBlowOuts;
    float mHealth;
    float mAnimatedCarPitch;
    float mAnimatedCarRoll;
    float mAnimatedCarShake;
};

} // namespace RenderConn

namespace VehicleFX {

struct Maps {
    ID id;
    Attrib::StringKey name;
    unsigned int marker;
};

const Maps *GetMaps();

} // namespace VehicleFX

namespace {

static inline bool IsRight(unsigned int i) {
    return (i & 1) != 0;
}

} // namespace

Behavior *DrawTraffic::Construct(const BehaviorParams &params) {
    return new DrawTraffic(params);
}

DrawTraffic::DrawTraffic(const BehaviorParams &params)
    : DrawCar(params, CarRenderUsage_AITraffic) {}

DrawPerformanceCar::DrawPerformanceCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawCar(params, usage) {
    GetOwner()->QueryInterface(&mTransmission);
    GetOwner()->QueryInterface(&mEngine);
}

DrawCar::DrawCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawVehicle(params), //
      mRenderService(nullptr), //
      mSuspension(nullptr), //
      mDamage(nullptr), //
      mVehicle(nullptr), //
      mVehicleDamage(nullptr), //
      mRBVehicle(nullptr), //
      mNIS(nullptr), //
      mSpikeDamage(nullptr), //
      mParts(), //
      mInView(false), //
      mDistanceToView(0.0f), //
      mHidden(false) {
    GetOwner()->QueryInterface(&mVehicle);
    GetOwner()->QueryInterface(&mSuspension);
    GetOwner()->QueryInterface(&mDamage);
    GetOwner()->QueryInterface(&mSpikeDamage);
    GetOwner()->QueryInterface(&mVehicleDamage);
    GetOwner()->QueryInterface(&mRBVehicle);
    GetOwner()->QueryInterface(&mNIS);

    if (mSuspension && mVehicle) {
        const char *modelName = mVehicle->GetVehicleName();
        if (!modelName) {
            modelName = "";
        }

        RenderConn::Pkt_Car_Open pkt(modelName,
                                     GetOwner()->GetWorldID(),
                                     usage,
                                     mVehicle->GetCustomizations(),
                                     mVehicle->GetVehicleKey(),
                                     mVehicle->IsSpooled());
        mRenderService = OpenService(UCrc32(0x804c146e), &pkt);
    }
}

void DrawPerformanceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mTransmission);
        GetOwner()->QueryInterface(&mEngine);
    }
    DrawCar::OnBehaviorChange(mechanic);
}

void DrawPerformanceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    DrawCar::OnService(pkt);
    if (mEngine && mTransmission) {
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x80) = mTransmission->GetGear();
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x7C) = mTransmission->GetShiftStatus();
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x74) = mEngine->IsNOSEngaged();

        if (mTransmission->GetGear() < 2 || GetVehicle()->GetDriverClass() != DRIVER_HUMAN) {
            float rpm_ratio = 0.0f;
            float rev = mEngine->GetRPM();
            float amin = mEngine->GetMinRPM();
            float amax = mEngine->GetMaxRPM();
            float arange = amax - amin;
            if (arange > 1e-06f) {
                float result = (rev - amin) / arange;
                rpm_ratio = 1.0f;
                if (result < 1.0f) {
                    rpm_ratio = result;
                }
                if (rpm_ratio < 0.0f) {
                    rpm_ratio = 0.0f;
                }
            }

            float vib = 0.0f;
            {
                float rev = mEngine->GetHorsePower();
                Hp min_power = mEngine->GetMinHorsePower();
                Hp max_power = mEngine->GetMaxHorsePower();
                float arange = max_power - min_power;
                if (arange > 1e-06f) {
                    float result = (rev - min_power) / arange;
                    vib = 1.0f;
                    if (result < 1.0f) {
                        vib = result;
                    }
                    if (vib < 0.0f) {
                        vib = 0.0f;
                    }
                }
            }

            *reinterpret_cast<float *>(reinterpret_cast<char *>(&pkt) + 0x88) = rpm_ratio;
            *reinterpret_cast<float *>(reinterpret_cast<char *>(&pkt) + 0x84) = vib;
        }
    }
}

Behavior *DrawNISCar::Construct(const BehaviorParams &params) {
    return new DrawNISCar(params);
}

DrawNISCar::DrawNISCar(const BehaviorParams &params)
    : DrawPerformanceCar(params, carRenderUsage_NISCar) {}

Behavior *DrawCopCar::Construct(const BehaviorParams &params) {
    return new DrawCopCar(params);
}

DrawCopCar::DrawCopCar(const BehaviorParams &params)
    : DrawPerformanceCar(params, CarRenderUsage_AICop) {}

Behavior *DrawRaceCar::Construct(const BehaviorParams &params) {
    IVehicle *vehicle = nullptr;
    static_cast<ISimable *>(params.fowner)->QueryInterface(&vehicle);
    if (!vehicle) {
        return nullptr;
    }

    DriverClass driverClass = vehicle->GetDriverClass();
    CarRenderUsage usage;
    if (driverClass != DRIVER_RACER) {
        if (driverClass < DRIVER_NONE) {
            if (driverClass == DRIVER_HUMAN) {
                usage = CarRenderUsage_Player;
            } else {
                usage = CarRenderUsage_AIRacer;
            }
        } else if (driverClass == DRIVER_REMOTE) {
            usage = CarRenderUsage_RemotePlayer;
        } else {
            usage = CarRenderUsage_AIRacer;
        }
    } else {
        usage = CarRenderUsage_AIRacer;
    }

    return new DrawRaceCar(params, usage);
}

DrawRaceCar::DrawRaceCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawPerformanceCar(params, usage) {
    GetOwner()->QueryInterface(&mEngineDamage);
}

void DrawRaceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mEngineDamage);
    }
    DrawPerformanceCar::OnBehaviorChange(mechanic);
}

void DrawRaceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    DrawPerformanceCar::OnService(pkt);
    if (mEngineDamage) {
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x78) = mEngineDamage->IsBlown();
    }
}

bool DrawCar::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == mRenderService) {
        if (!static_cast<IModel *>(this)->IsHidden()) {
            OnService(*static_cast<RenderConn::Pkt_Car_Service *>(pkt));
            return true;
        }
        mInView = false;
    }
    return false;
}

void DrawCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
        GetOwner()->QueryInterface(&mNIS);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRBVehicle);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mDamage);
        GetOwner()->QueryInterface(&mVehicleDamage);
        GetOwner()->QueryInterface(&mSpikeDamage);
    }
}

bool DrawCar::IsHidden() const {
    return mHidden || DrawVehicle::IsHidden() || mRenderService;
}

void DrawCar::HideModel() {
    DrawVehicle::HideModel();
    mHidden = true;
}

void DrawCar::ReleaseModel() {
    DrawVehicle::ReleaseModel();
    if (mRenderService) {
        CloseService(mRenderService);
        mRenderService = 0;
    }
}

void DrawCar::ReleaseChildModels() {
    DrawVehicle::ReleaseChildModels();
    if (!mParts.empty()) {
        mParts.clear();
    }
}

bool DrawCar::IsPartVisible(const UCrc32 &name) const {
    return mParts.find(name) == mParts.end();
}

void DrawCar::HidePart(const UCrc32 &name) {
    Parts::iterator iter = mParts.find(name);
    if (iter == mParts.end()) {
        Parts::iterator lower = mParts.lower_bound(name);
        if (lower == mParts.end() || name < lower->first) {
            lower = mParts.insert(lower, Parts::value_type(name, 0));
        }
        lower->second = 1;
    } else {
        iter->second = iter->second + 1;
    }
}

void DrawCar::ShowPart(const UCrc32 &name) {
    Parts::iterator iter = mParts.find(name);
    if (iter != mParts.end()) {
        iter->second = iter->second - 1;
        if (iter->second < 1) {
            mParts.erase(iter);
        }
    }
}

bool DrawCar::InView() const {
    return mInView;
}

bool DrawCar::IsRenderable() const {
    return CheckService(mRenderService) == Sim::CONNSTATUS_READY;
}

float DrawCar::DistanceToView() const {
    return mDistanceToView;
}

void DrawCar::Reset() {}

void DrawCar::OnTaskSimulate(float dT) {}

DrawCar::~DrawCar() {
    if (mRenderService) {
        CloseService(mRenderService);
        mRenderService = 0;
    }

    if (!mParts.empty()) {
        mParts.clear();
    }
}

void DrawCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    mInView = pkt.InView();
    mDistanceToView = pkt.DistanceToView();

    if (mRBVehicle) {
        eInvulnerablitiy invulnerability = mRBVehicle->GetInvulnerability();
        if (invulnerability > INVULNERABLE_NONE && invulnerability < INVULNERABLE_FROM_CONTROL_SWITCH) {
            pkt.SetFlashing(true);
        }
    }

    const VehicleFX::Maps *fx = VehicleFX::GetMaps();
    if (fx->id != VehicleFX::LIGHT_NONE) {
        do {
            if (fx->marker != 0) {
                if (mVehicleDamage && mVehicleDamage->IsLightDamaged(fx->id)) {
                    pkt.SetBrokenLightState(fx->id, true);
                } else {
                    pkt.SetLightState(fx->id, mVehicle->IsGlareOn(fx->id));
                }
            }
            ++fx;
        } while (fx->id != VehicleFX::LIGHT_NONE);
    }

    IDynamicsEntity *dynamicsEntity = nullptr;
    if (GetOwner()->QueryInterface(&dynamicsEntity) && Dynamics::Articulation::IsJoined(dynamicsEntity)) {
        pkt.SetExtraBodyRoll(0.0f);
        pkt.SetExtraBodyPitch(0.0f);
    } else if (mSuspension) {
        const float motion = mSuspension->GetRenderMotion();
        pkt.SetExtraBodyPitch(motion);
        pkt.SetExtraBodyRoll(motion);
    }

    if (mDamage) {
        pkt.SetDamage(mDamage->GetZoneDamage());
        pkt.SetHealth(mDamage->GetHealth());
    }

    for (Parts::const_iterator iter = mParts.begin(); iter != mParts.end(); iter++) {
        pkt.HidePart((*iter).first);
    }

    pkt.mGroundState = 0;
    pkt.mBlowOuts = 0;
    if (mSuspension) {
        IRigidBody *irb = GetOwner()->GetRigidBody();
        (void)irb;

        for (unsigned int i = 0; i < mSuspension->GetNumWheels(); ++i) {
            const UMath::Vector3 &vel = mSuspension->GetWheelVelocity(i);
            float pos_x = mSuspension->GetWheelLocalPos(i).x;
            float steer = UMath::Clamp(ANGLE2RAD(mSuspension->GetWheelSteer(i)), -3.1415927f, 3.1415927f);
            float compression = mSuspension->GetCompression(i) - mSuspension->GetRideHeight(i);
            float av = mSuspension->GetWheelAngularVelocity(i);
            float radius = mSuspension->GetWheelRadius(i);
            float slip = 0.0f;
            float skid = 0.0f;
            bool onground = false;
            eTireIdx idx;

            (void)vel;
            (void)pos_x;

            compression += radius;
            if (mSuspension->IsWheelOnGround(i)) {
                skid = UMath::Abs(mSuspension->GetWheelSkid(i));
                slip = UMath::Abs(mSuspension->GetWheelSlip(i));
                float tolerance = mSuspension->GetToleratedSlip(i);
                if (tolerance < slip) {
                    slip -= tolerance;
                } else {
                    slip = 0.0f;
                }
                onground = true;
            }

            if (IsFront(i)) {
                idx = IsRight(i) ? TIRE_FR : TIRE_FL;
                pkt.SetSteering(idx, -steer);
            } else {
                idx = IsRight(i) ? TIRE_RR : TIRE_RL;
            }

            if (mSpikeDamage && mSpikeDamage->GetTireDamage(i) == TIRE_DAMAGE_BLOWN) {
                pkt.SetTireBlowOut(idx);
            }

            pkt.SetWheelSlip(idx, slip);
            pkt.SetWheelSpeed(idx, av * radius);
            pkt.SetWheelCompression(idx, compression);
            pkt.SetWheelSkid(idx, skid);
            pkt.SetWheelOnGround(idx, onground);
        }
    }

    if (mNIS) {
        pkt.SetAnimatedPitch(mNIS->GetAnimPitch());
        pkt.SetAnimatedRoll(mNIS->GetAnimRoll());
        pkt.SetAnimatedShake(mNIS->GetAnimShake());
    }
}
