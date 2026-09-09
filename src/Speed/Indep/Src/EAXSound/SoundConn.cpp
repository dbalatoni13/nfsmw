#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/Sim/SimServer.h"

IMPLEMENT_FACTORY(Sound::AudioEvent); // Decl: 11

IMPLEMENT_LISTABLE(EAX_CarState);
IMPLEMENT_LISTABLE(EAX_HeliState);

void SoundConn::InitServices() {}

void SoundConn::RestoreServices() {}

// Decl: 34
BIND_SIM_CONN(CarSoundConn);
// Decl: 35
IMPLEMENT_LISTABLE(CarSoundConn);

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

CarSoundConn::CarSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data), //
      mConnected(false),     //
      mState(nullptr),       //
      mTarget(0) {
    SoundConn::Pkt_Car_Open *oc = static_cast<SoundConn::Pkt_Car_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    this->mTarget.Set(oc->mWorldID);

    uint32 namehash = bStringHash(att->MODEL(0).GetString());
    CarType ct = CarPartDB.GetCarType(namehash);

    Sound::Context ctx = oc->mCarContext;
    this->mState = new ("EAX_CarState", 0) EAX_CarState(att.GetConstCollection(), ctx, oc->mWorldID, oc->mHandle);

    if (!oc->mSpoolLoad) {
        new ECommitAudioAssets();
    }
}

CarSoundConn::~CarSoundConn() {
    this->mTarget.Set(0);
    if (g_pEAXSound != nullptr) {
        g_pEAXSound->DestroyEAXCar(this->mState);
    }
    if (this->mState != nullptr) {
        delete this->mState;
        this->mState = nullptr;
    }
}

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (this->mConnected && this->mState != nullptr && this->mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr) {
        return;
    }

    if (!this->mTarget.IsValid()) {
        return;
    }

    if (!this->mConnected) {
        return;
    }

    if (this->mState == nullptr) {
        return;
    }

    if (!this->mState->mAssetsLoaded) {
        return;
    }

    SoundConn::Pkt_Car_Service data(this->mState->mVisualRPM);
    this->mState->mMatrix = *this->mTarget.GetMatrix();

    if (!this->Service(&data)) {
        this->mState->mSimUpdating = false;
        this->mState->mVel0 = this->mState->mVel1 = this->mState->mAccel = bVector3(0.0f, 0.0f, 0.0f);
        this->mState->mFWSpeed = 0.0f;
        this->mState->mWheel[0].Reset();
        this->mState->mWheel[1].Reset();
        this->mState->mWheel[2].Reset();
        this->mState->mWheel[3].Reset();
        this->mState->mEngine.Reset();
        return;
    }

    this->mState->mSimUpdating = true;
    this->mState->mVel1 = this->mState->mVel0;
    this->mState->mVel0 = *this->mTarget.GetVelocity();
    bSub(&this->mState->mAccel, &this->mState->mVel0, &this->mState->mVel1);
    bScale(&this->mState->mAccel, &this->mState->mAccel, 1.0f / dT);

    this->mState->mEngine.mRPMPct = data.mRPMPercent;
    this->mState->mEngine.mNOSFlag = static_cast<int>(data.mNOSFlag);
    this->mState->mEngine.mNOS = data.mNOSCapacity;
    this->mState->mEngine.mThrottle = data.mThrottlePercent;
    this->mState->mEngine.mBlownFlag = data.mEngineBlown;
    this->mState->mBrake = data.mBrakePercent;
    this->mState->mEBrake = data.mEBrakePercent;
    this->mState->mSteering = bRadToAng(data.mSteering);
    this->mState->mSirenState = data.mSirenState;
    this->mState->mHotPursuit = data.mHotPursuit;
    this->mState->mOversteer = data.mOversteer;
    this->mState->mUndersteer = data.mUndersteer;
    this->mState->mSlipAngle = -data.mSlipAngle;
    this->mState->mHealth = data.mHealth;

    this->mState->mFWSpeed = bLength(this->mTarget.GetVelocity());

    this->mState->mTrailerID = data.mTrailer;
    this->mState->mTimeSinceSeen = data.mTimeSinceSeen;
    this->mState->mDesiredSpeed = data.mDesiredSpeed;
    this->mState->mControlSource = data.mControlSource;

    for (int i = 0; i < 4; i++) {
        this->mState->mWheel[i].mWheelOnGround = data.mWheelOnGround[i] ? 1 : 0;
        this->mState->mWheel[i].mWheelSlip = data.mWheelSlip[i];
        this->mState->mWheel[i].mPercentFsFkTransfer = 1.0f - data.mTractionPct[i];
        this->mState->mWheel[i].mPrevTerrainType = this->mState->mWheel[i].mTerrainType;
        this->mState->mWheel[i].mTerrainType = data.mWheelTerrain[i];
        this->mState->mWheel[i].mLoad = data.mWheelLoad[i];
        this->mState->mWheel[i].mPrevBlownState = this->mState->mWheel[i].mBlownState;
        this->mState->mWheel[i].mBlownState = data.mBlownTires[i];
        this->mState->mWheel[i].mWheelForceZ = data.mWheelZforce[i];
    }

    if (this->mState->mDriveline.mGear != data.mGear) {
        this->mState->mDriveline.mGear = static_cast<Gear>(data.mGear);
        this->mState->mDriveline.mGearShiftFlag = 1;
    } else {
        this->mState->mDriveline.mGearShiftFlag = 0;
    }
}

// total size: 0x28
// Decl: 294
class HeliSoundConn : public Sim::Connection, public UTL::Collections::Listable<HeliSoundConn, 10> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);
    HeliSoundConn(const Sim::ConnectionData &data);

    // Overrides: Connection
    ~HeliSoundConn() override;

    virtual void OnReceive(Sim::Packet *pkt) {}

    // Overrides: Connection
    void OnClose() override {
        delete this;
    }

    // Overrides: Connection
    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    EAX_HeliState *GetState() {
        return this->mState;
    }

    WUID GetWorldID() const {
        return this->mTarget.GetWorldID();
    }

    void UpdateState(float dT);

  private:
    EAX_HeliState *mState;        // offset 0x14, size 0x4
    WorldConn::Reference mTarget; // offset 0x18, size 0x10
};

// Decl: 318
BIND_SIM_CONN(HeliSoundConn);
// Decl: 319
IMPLEMENT_LISTABLE(HeliSoundConn);

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}

HeliSoundConn::HeliSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data), //
      mState(nullptr),       //
      mTarget(0) {
    SoundConn::Pkt_Heli_Open *oc = static_cast<SoundConn::Pkt_Heli_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    this->mTarget.Set(oc->mWorldID);

    uint32 namehash = bStringHash(att->MODEL(0).GetString());

    this->mState = new ("Aud: EAX_HeliState", 0) EAX_HeliState(att.GetConstCollection(), oc->mWorldID);
    g_pEAXSound->SpawnHelicopter(this->mState);
    this->mState->mSimUpdating = false;
}

HeliSoundConn::~HeliSoundConn() {
    this->mTarget.Set(0);
    if (g_pEAXSound != nullptr) {
        g_pEAXSound->DestroyEAXHeli(this->mState);
    }
    if (this->mState != nullptr) {
        delete this->mState;
        this->mState = nullptr;
    }
}

bool btestprint = false; // Decl: 373

void HeliSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr) {
        if (!btestprint) {
            btestprint = true;
        }
        return;
    }

    int validTarget = static_cast<int>(this->mTarget.IsValid());
    if (!validTarget) {
        if (!btestprint) {
            btestprint = true;
        }
        return;
    }

    SoundConn::Pkt_Heli_Service data;
    if (!this->Service(&data)) {
        this->mState->mSimUpdating = false;
        return;
    }

    float x, y, z;

    btestprint = false;
    this->mState->mSimUpdating = true;
    this->mState->mMatrix = *this->mTarget.GetMatrix();
    this->mState->mVel1 = this->mState->mVel0;
    this->mState->mVel0 = *this->mTarget.GetVelocity();
    bSub(&this->mState->mAccel, &this->mState->mVel0, &this->mState->mVel1);
    bScale(&this->mState->mAccel, &this->mState->mAccel, 1.0f / dT);
    this->mState->mFWSpeed = bLength(this->mTarget.GetVelocity());
}

void SoundConn::UpdateServices(float dT) {
    for (CarSoundConn *const *iter = CarSoundConn::GetList().begin(); iter != CarSoundConn::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }

    for (HeliSoundConn *const *iter = HeliSoundConn::GetList().begin(); iter != HeliSoundConn::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }
}
