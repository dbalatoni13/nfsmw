#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

class CarPartDatabase {
public:
    int GetCarType(unsigned int hash);
};

extern CarPartDatabase CarPartDB;
extern "C" void *__builtin_vec_new(unsigned int size);
extern "C" void __builtin_delete(void *ptr);
extern int btestprint;

namespace {

// total size: 0x18
struct Pkt_Car_Open : public Sim::Packet {
    const Attrib::Collection *m_VehicleSpec; // offset 0x4
    WUID mWorldID;                           // offset 0x8
    Sound::Context mCarContext;              // offset 0xC
    bool mSpoolLoad;                         // offset 0x10
    HSIMABLE__ *mHandle;                     // offset 0x14
};

// total size: 0x10
struct Pkt_Heli_Open : public Sim::Packet {
    const Attrib::Collection *m_VehicleSpec; // offset 0x4
    WUID mWorldID;                           // offset 0x8
};

} // namespace

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

CarSoundConn::CarSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mConnected(false) //
    , mState(nullptr) //
    , mTarget(0) {
    Pkt_Car_Open *oc = static_cast<Pkt_Car_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    mTarget.Set(oc->mWorldID);

    const void *modelData = att.GetAttributePointer(0x9047C9E0, 0);
    if (modelData == nullptr) {
        modelData = Attrib::DefaultDataArea(0x10);
    }

    const char *modelName = *static_cast<const char *const *>(
        static_cast<const void *>(static_cast<const char *>(modelData) + 0xC));
    if (modelName == nullptr) {
        modelName = "";
    }
    CarPartDB.GetCarType(bStringHash(modelName));

    mState = static_cast<EAX_CarState *>(__builtin_vec_new(0x248));
    new (mState) EAX_CarState(att.GetConstCollection(), oc->mCarContext, oc->mWorldID, oc->mHandle);

    if (!oc->mSpoolLoad) {
        new ECommitAudioAssets();
    }
}

namespace SoundConn {

template <>
UTL::Collections::Listable<EAX_CarState, 10>::List UTL::Collections::Listable<EAX_CarState, 10>::_mTable =
    UTL::Collections::Listable<EAX_CarState, 10>::List();
template <>
UTL::Collections::Listable<EAX_HeliState, 10>::List UTL::Collections::Listable<EAX_HeliState, 10>::_mTable =
    UTL::Collections::Listable<EAX_HeliState, 10>::List();
template <>
UTL::Collections::Listable<CarSoundConn, 10>::List UTL::Collections::Listable<CarSoundConn, 10>::_mTable =
    UTL::Collections::Listable<CarSoundConn, 10>::List();
template <>
UTL::Collections::Listable<HeliSoundConn, 10>::List UTL::Collections::Listable<HeliSoundConn, 10>::_mTable =
    UTL::Collections::Listable<HeliSoundConn, 10>::List();

void InitServices() {}
void RestoreServices() {}

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

Pkt_Car_Service::~Pkt_Car_Service() {}

UCrc32 Pkt_Car_Service::ConnectionClass() {
    static UCrc32 hash = "CarSoundConn";
    return hash;
}

unsigned int Pkt_Car_Service::Size() {
    return sizeof(*this);
}

unsigned int Pkt_Car_Service::Type() {
    return SType();
}

unsigned int Pkt_Car_Service::SType() {
    static UCrc32 hash = "Pkt_Car_Service";
    return hash.GetValue();
}

UCrc32 Pkt_Heli_Service::ConnectionClass() {
    static UCrc32 hash = "HeliSoundConn";
    return hash;
}

unsigned int Pkt_Heli_Service::Size() {
    return sizeof(*this);
}

unsigned int Pkt_Heli_Service::Type() {
    return SType();
}

unsigned int Pkt_Heli_Service::SType() {
    static UCrc32 hash = "Pkt_Heli_Service";
    return hash.GetValue();
}

} // namespace SoundConn

template <>
UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype *
    UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype::mHead = nullptr;

UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype _CarSoundConn(
    "CarSoundConn", CarSoundConn::Construct);
UTL::COM::Factory<Sim::ConnectionData const &, Sim::Connection, UCrc32>::Prototype _HeliSoundConn(
    "HeliSoundConn", HeliSoundConn::Construct);

CarSoundConn::~CarSoundConn() {
    mTarget.Set(0);
    if (g_pEAXSound != nullptr) {
        g_pEAXSound->DestroyEAXCar(mState);
    }
    if (mState != nullptr) {
        mState->~EAX_CarState();
        __builtin_delete(mState);
    }
    mState = nullptr;
}

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (mConnected && mState != nullptr && mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr) {
        return;
    }
    if (!mTarget.IsValid()) {
        return;
    }
    if (!mConnected) {
        return;
    }
    if (mState == nullptr) {
        return;
    }
    if (!mState->mAssetsLoaded) {
        return;
    }

    SoundConn::Pkt_Car_Service data(mState->mVisualRPM);
    mState->mMatrix = *mTarget.GetMatrix();

    if (!Service(&data)) {
        mState->mSimUpdating = false;
        mState->mAccel = bVector3(0.0f, 0.0f, 0.0f);
        mState->mVel1 = mState->mAccel;
        mState->mVel0 = mState->mVel1;
        mState->mFWSpeed = 0.0f;
        mState->mWheel[0].Reset();
        mState->mWheel[1].Reset();
        mState->mWheel[2].Reset();
        mState->mWheel[3].Reset();
        mState->mEngine.Reset();
        return;
    }

    mState->mSimUpdating = true;
    mState->mVel1 = mState->mVel0;
    mState->mVel0 = *mTarget.GetVelocity();
    bSub(&mState->mAccel, &mState->mVel0, &mState->mVel1);
    bScale(&mState->mAccel, &mState->mAccel, 1.0f / dT);

    mState->mEngine.mRPMPct = data.mRPMPercent;
    mState->mEngine.mNOSFlag = data.mNOSFlag;
    mState->mEngine.mNOS = data.mNOSCapacity;
    mState->mEngine.mThrottle = data.mThrottlePercent;
    mState->mEngine.mBlownFlag = data.mEngineBlown;
    mState->mBrake = data.mBrakePercent;
    mState->mEBrake = data.mEBrakePercent;
    mState->mSteering = bRadToAng(data.mSteering);
    mState->mSirenState = data.mSirenState;
    mState->mHotPursuit = data.mHotPursuit;
    mState->mOversteer = data.mOversteer;
    mState->mUndersteer = data.mUndersteer;
    mState->mSlipAngle = -data.mSlipAngle;
    mState->mHealth = data.mHealth;

    mState->mFWSpeed = bLength(mTarget.GetVelocity());

    mState->mTrailerID = data.mTrailer;
    mState->mTimeSinceSeen = data.mTimeSinceSeen;
    mState->mDesiredSpeed = data.mDesiredSpeed;
    mState->mControlSource = static_cast<Sound::ControlSource>(data.mControlSource);

    for (int i = 0; i < 4; ++i) {
        mState->mWheel[i].mWheelOnGround = data.mWheelOnGround[i] ? 1 : 0;
        mState->mWheel[i].mWheelSlip = data.mWheelSlip[i];
        mState->mWheel[i].mPercentFsFkTransfer = 1.0f - data.mTractionPct[i];
        mState->mWheel[i].mPrevTerrainType = mState->mWheel[i].mTerrainType;
        mState->mWheel[i].mTerrainType = data.mWheelTerrain[i];
        mState->mWheel[i].mLoad = data.mWheelLoad[i];
        mState->mWheel[i].mPrevBlownState = mState->mWheel[i].mBlownState;
        mState->mWheel[i].mBlownState = data.mBlownTires[i];
        mState->mWheel[i].mWheelForceZ = data.mWheelZforce[i];
    }

    if (mState->mDriveline.mGear != data.mGear) {
        mState->mDriveline.mGear = static_cast<Sound::Gear>(data.mGear);
        mState->mDriveline.mGearShiftFlag = 1;
    } else {
        mState->mDriveline.mGearShiftFlag = 0;
    }
}

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}

HeliSoundConn::HeliSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mState(nullptr) //
    , mTarget(0) {
    Pkt_Heli_Open *oc = static_cast<Pkt_Heli_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    unsigned int namehash;
    EAX_HeliState *state;
    mTarget.Set(oc->mWorldID);

    const void *modelData = att.GetAttributePointer(0x9047C9E0, 0);
    if (modelData == nullptr) {
        modelData = Attrib::DefaultDataArea(0x10);
    }

    const char *modelName = *static_cast<const char *const *>(
        static_cast<const void *>(static_cast<const char *>(modelData) + 0xC));
    if (modelName == nullptr) {
        modelName = "";
    }
    namehash = bStringHash(modelName);

    state = static_cast<EAX_HeliState *>(__builtin_vec_new(0xA0));
    new (state) EAX_HeliState(att.GetConstCollection(), oc->mWorldID);
    mState = state;
    g_pEAXSound->SpawnHelicopter(mState);
    mState->mSimUpdating = false;
}

HeliSoundConn::~HeliSoundConn() {
    mTarget.Set(0);
    if (g_pEAXSound != nullptr) {
        g_pEAXSound->DestroyEAXHeli(mState);
    }
    if (mState != nullptr) {
        delete mState;
        mState = nullptr;
    }
}

void HeliSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr) {
        if (!btestprint) {
            btestprint = 1;
        }
        return;
    }

    int validTarget = 1;
    if (!mTarget.IsValid()) {
        validTarget = 0;
    }

    if (!validTarget) {
        if (!btestprint) {
            btestprint = 1;
        }
        return;
    }

    SoundConn::Pkt_Heli_Service data;
    if (!Service(&data)) {
        mState->mSimUpdating = false;
        return;
    }

    btestprint = 0;
    mState->mSimUpdating = true;
    PSMTX44Copy((Mtx44)mTarget.GetMatrix(), (Mtx44)&mState->mMatrix);
    mState->mVel1 = mState->mVel0;
    mState->mVel0 = *mTarget.GetVelocity();
    bSub(&mState->mAccel, &mState->mVel0, &mState->mVel1);
    bScale(&mState->mAccel, &mState->mAccel, 1.0f / dT);
    mState->mFWSpeed = bLength(mTarget.GetVelocity());
}

unsigned int Sim::Packet::Compress(Sim::Packet *) const { return 0; }

unsigned int Sim::Packet::Decompress(Sim::Packet *) const { return 0; }

template class UTL::Collections::Listable<CarSoundConn, 10>::List;

template class UTL::Collections::Listable<HeliSoundConn, 10>::List;
