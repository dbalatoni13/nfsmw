#include "DrawVehicle.h"

#include "Speed/Indep/Src/FE/FECustomizationRecord.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Sim/SimConn.h"

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
        return UCrc32(0x804c146e);
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
    void HidePart(const UCrc32 &partname) {}

    bool InView() const {
        return mInView;
    }

    float DistanceToView() const {
        return mDistanceToView;
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

namespace {

struct VehicleLightInfo {
    VehicleFX::ID Light;
};

static const VehicleLightInfo kVehicleLights[] = {
    {VehicleFX::LIGHT_LHEAD},
    {VehicleFX::LIGHT_RHEAD},
    {VehicleFX::LIGHT_CHEAD},
    {VehicleFX::LIGHT_LBRAKE},
    {VehicleFX::LIGHT_RBRAKE},
    {VehicleFX::LIGHT_CBRAKE},
    {VehicleFX::LIGHT_LREVERSE},
    {VehicleFX::LIGHT_RREVERSE},
    {VehicleFX::LIGHT_LRSIGNAL},
    {VehicleFX::LIGHT_RRSIGNAL},
    {VehicleFX::LIGHT_LFSIGNAL},
    {VehicleFX::LIGHT_RFSIGNAL},
    {VehicleFX::LIGHT_COPRED},
    {VehicleFX::LIGHT_COPBLUE},
    {VehicleFX::LIGHT_COPWHITE},
};

static float AbsFloat(float value) {
    return value < 0.0f ? -value : value;
}

static unsigned int GetPacketWheelIndex(unsigned int wheel) {
    if (wheel < 2) {
        return wheel & 1;
    }
    return (wheel & 1) ? 2 : 3;
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
    mInView = pkt.mInView;
    mDistanceToView = pkt.mDistanceToView;

    if (mRBVehicle) {
        eInvulnerablitiy invulnerability = mRBVehicle->GetInvulnerability();
        if (invulnerability > INVULNERABLE_NONE && invulnerability < INVULNERABLE_FROM_CONTROL_SWITCH) {
            pkt.mFlashing = true;
        }
    }

    pkt.mLights = 0;
    pkt.mBrokenLights = 0;
    if (mVehicle) {
        for (unsigned int i = 0; i < sizeof(kVehicleLights) / sizeof(kVehicleLights[0]); ++i) {
            VehicleFX::ID light = kVehicleLights[i].Light;
            if (mVehicleDamage && mVehicleDamage->IsLightDamaged(light)) {
                pkt.mBrokenLights |= light;
            } else if (mVehicle->IsGlareOn(light)) {
                pkt.mLights |= light;
            }
        }
    }

    IDynamicsEntity *dynamicsEntity = nullptr;
    GetOwner()->QueryInterface(&dynamicsEntity);
    if (dynamicsEntity && Dynamics::Articulation::IsJoined(dynamicsEntity)) {
        pkt.mExtraBodyRoll = 0.0f;
        pkt.mExtraBodyPitch = 0.0f;
    } else if (mSuspension) {
        float renderMotion = mSuspension->GetRenderMotion();
        pkt.mExtraBodyRoll = renderMotion;
        pkt.mExtraBodyPitch = renderMotion;
    }

    if (mDamage) {
        pkt.mDamageInfo = mDamage->GetZoneDamage();
        pkt.mHealth = mDamage->GetHealth();
    }

    for (Parts::const_iterator iter = mParts.begin(); iter != mParts.end(); ++iter) {
        pkt.HidePart(iter->first);
    }

    pkt.mGroundState = 0;
    pkt.mBlowOuts = 0;
    if (mSuspension) {
        for (unsigned int wheel = 0; wheel < mSuspension->GetNumWheels(); ++wheel) {
            unsigned int packetWheel = GetPacketWheelIndex(wheel);
            float steering = mSuspension->GetWheelSteer(wheel) * 6.2831855f;
            if (steering > 3.1415927f) {
                steering = 3.1415927f;
            } else if (steering < -3.1415927f) {
                steering = -3.1415927f;
            }
            if (wheel < 2) {
                pkt.mSteering[packetWheel] = -steering;
            }

            bool onGround = mSuspension->IsWheelOnGround(wheel);
            float skid = 0.0f;
            float slip = 0.0f;
            if (onGround) {
                skid = AbsFloat(mSuspension->GetWheelSkid(wheel));
                float absSlip = AbsFloat(mSuspension->GetWheelSlip(wheel));
                float tolerated = AbsFloat(mSuspension->GetToleratedSlip(wheel));
                if (absSlip > tolerated) {
                    slip = absSlip - tolerated;
                }
                pkt.mGroundState |= 1 << packetWheel;
            } else {
                pkt.mGroundState &= ~(1 << packetWheel);
            }

            pkt.mCompressions[packetWheel] =
                mSuspension->GetCompression(wheel) - mSuspension->GetRideHeight(wheel) + mSuspension->GetWheelRadius(wheel);
            pkt.mWheelSpeed[packetWheel] = mSuspension->GetWheelAngularVelocity(wheel) * mSuspension->GetWheelRadius(wheel);
            pkt.mTireSkid[packetWheel] = skid;
            pkt.mTireSlip[packetWheel] = slip;

            if (mSpikeDamage && mSpikeDamage->GetTireDamage(wheel) == TIRE_DAMAGE_BLOWN) {
                pkt.mBlowOuts |= 1 << packetWheel;
            }
        }
    }

    if (mNIS) {
        pkt.mAnimatedCarPitch = mNIS->GetAnimPitch();
        pkt.mAnimatedCarRoll = mNIS->GetAnimRoll();
        pkt.mAnimatedCarShake = mNIS->GetAnimShake();
    }
}
