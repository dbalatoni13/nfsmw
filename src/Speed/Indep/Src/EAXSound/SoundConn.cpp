#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

extern CarPartDatabase CarPartDB;

int btestprint = 0;
namespace SoundConn {

void InitServices() {}
void RestoreServices() {}

} // namespace SoundConn

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

CarSoundConn::CarSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mConnected(false) //
    , mState(nullptr) //
    , mTarget(0) {
    SoundConn::Pkt_Car_Open *oc = static_cast<SoundConn::Pkt_Car_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    this->mTarget.Set(oc->mWorldID);

    unsigned int namehash = bStringHash(att->MODEL(0).GetString());
    CarType ct = CarPartDB.GetCarType(namehash);
    Sound::Context ctx = oc->mCarContext;

    (void)ct;
    this->mState = new ("EAX_CarState", __LINE__) EAX_CarState(att.GetConstCollection(), ctx, oc->mWorldID, oc->mHandle);

    if (!oc->mSpoolLoad) {
        new ECommitAudioAssets();
    }
}

namespace SoundConn {

inline UCrc32 Pkt_Car_Service::ConnectionClass() {
    static UCrc32 hash = "CarSoundConn";
    return hash;
}

inline unsigned int Pkt_Car_Service::Size() {
    return sizeof(*this);
}

inline unsigned int Pkt_Car_Service::Type() {
    return SType();
}

inline unsigned int Pkt_Car_Service::SType() {
    static UCrc32 hash = "Pkt_Car_Service";
    return hash.GetValue();
}

inline UCrc32 Pkt_Heli_Service::ConnectionClass() {
    static UCrc32 hash = "HeliSoundConn";
    return hash;
}

inline unsigned int Pkt_Heli_Service::Size() {
    return sizeof(*this);
}

inline unsigned int Pkt_Heli_Service::Type() {
    return SType();
}

inline unsigned int Pkt_Heli_Service::SType() {
    static UCrc32 hash = "Pkt_Heli_Service";
    return hash.GetValue();
}

} // namespace SoundConn

template <>
UTL::Collections::Listable<EAX_CarState, 10>::List UTL::Collections::Listable<EAX_CarState, 10>::_mTable =
    UTL::Collections::Listable<EAX_CarState, 10>::List();
template <>
UTL::Collections::Listable<EAX_HeliState, 10>::List UTL::Collections::Listable<EAX_HeliState, 10>::_mTable =
    UTL::Collections::Listable<EAX_HeliState, 10>::List();

UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype _CarSoundConn(
    "CarSoundConn", CarSoundConn::Construct);

template <>
UTL::Collections::Listable<CarSoundConn, 10>::List UTL::Collections::Listable<CarSoundConn, 10>::_mTable =
    UTL::Collections::Listable<CarSoundConn, 10>::List();

UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype _HeliSoundConn(
    "HeliSoundConn", HeliSoundConn::Construct);

template <>
UTL::Collections::Listable<HeliSoundConn, 10>::List UTL::Collections::Listable<HeliSoundConn, 10>::_mTable =
    UTL::Collections::Listable<HeliSoundConn, 10>::List();

CarSoundConn::~CarSoundConn() {
    this->mTarget.Set(0);
    if (g_pEAXSound) {
        g_pEAXSound->DestroyEAXCar(this->mState);
    }
    if (this->mState) {
        delete this->mState;

        this->mState = nullptr;
    }
}

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (this->mConnected && this->mState && this->mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::UpdateState(float dT) {
    if (!g_pEAXSound) {
        return;
    }
    if (!this->mTarget.IsValid()) {
        return;
    }
    if (!this->mConnected) {
        return;
    }
    if (!this->mState) {
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
    this->mState->mEngine.mNOSFlag = data.mNOSFlag;
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
    this->mState->mControlSource = static_cast<Sound::ControlSource>(data.mControlSource);

    for (int i = 0; i < 4; ++i) {
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

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}

HeliSoundConn::HeliSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mState(nullptr) //
    , mTarget(0) {
    SoundConn::Pkt_Heli_Open *oc = static_cast<SoundConn::Pkt_Heli_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    this->mTarget.Set(oc->mWorldID);

    unsigned int namehash = bStringHash(att->MODEL(0).GetString());
    (void)namehash;

    this->mState = new ("Aud: EAX_HeliState", __LINE__) EAX_HeliState(att.GetConstCollection(), oc->mWorldID);
    g_pEAXSound->SpawnHelicopter(this->mState);
    this->mState->mSimUpdating = false;
}

HeliSoundConn::~HeliSoundConn() {
    this->mTarget.Set(0);
    if (g_pEAXSound) {
        g_pEAXSound->DestroyEAXHeli(this->mState);
    }
    if (this->mState) {
        delete this->mState;
        this->mState = nullptr;
    }
}

void HeliSoundConn::UpdateState(float dT) {
    if (!g_pEAXSound || !this->mTarget.IsValid()) {
        if (!btestprint) {
            btestprint = 1;
        }
        return;
    }

    SoundConn::Pkt_Heli_Service data;
    if (!this->Service(&data)) {
        this->mState->mSimUpdating = false;
        return;
    }

    btestprint = 0;
    this->mState->mSimUpdating = true;
    bCopy(&this->mState->mMatrix, this->mTarget.GetMatrix());
    this->mState->mVel1 = this->mState->mVel0;
    this->mState->mVel0 = *this->mTarget.GetVelocity();
    bSub(&this->mState->mAccel, &this->mState->mVel0, &this->mState->mVel1);
    bScale(&this->mState->mAccel, &this->mState->mAccel, 1.0f / dT);
    this->mState->mFWSpeed = bLength(this->mTarget.GetVelocity());
}

namespace SoundConn {

void UpdateServices(float dT) {
    typedef UTL::Collections::Listable<CarSoundConn, 10> CarList;
    for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }

    typedef UTL::Collections::Listable<HeliSoundConn, 10> HeliList;
    for (HeliSoundConn *const *iter = HeliList::GetList().begin(); iter != HeliList::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }
}

} // namespace SoundConn

inline unsigned int Sim::Packet::Compress(Sim::Packet *) const { return 0; }

inline unsigned int Sim::Packet::Decompress(Sim::Packet *) const { return 0; }

template class UTL::Collections::Listable<CarSoundConn, 10>::List;

template class UTL::Collections::Listable<HeliSoundConn, 10>::List;
