#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"

class CarPartDatabase {
public:
    int GetCarType(unsigned int hash);
};

extern CarPartDatabase CarPartDB;
extern "C" void *__builtin_vec_new(unsigned int size);
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

// total size: 0x44
struct CarWheelState {
    bVector2 mWheelSlip;          // offset 0x0
    float mWheelForceZ;           // offset 0x8
    float mPercentFsFkTransfer;   // offset 0xC
    int mWheelOnGround;           // offset 0x10
    SimSurface mTerrainType;      // offset 0x14
    SimSurface mPrevTerrainType;  // offset 0x28
    float mLoad;                  // offset 0x3C
    unsigned char mBlownState;    // offset 0x40
    unsigned char mPrevBlownState; // offset 0x41
};

// total size: 0x1C
struct CarEngineState {
    int mBoostFlag;
    int mNOSFlag;
    float mNOS;
    float mRPMPct;
    float mThrottle;
    float mBoost;
    int mBlownFlag;
};

// total size: 0x8
struct CarDrivelineState {
    int mGearShiftFlag;
    int mGear;
};

struct WorldRefView {
    unsigned int mWorldID;
    const bMatrix4 *mMatrix;
    const bVector3 *mVelocity;
    const bVector3 *mAcceleration;
};

typedef char CarWheelStateSizeCheck[(sizeof(CarWheelState) == 0x44) ? 1 : -1];
typedef char CarEngineStateSizeCheck[(sizeof(CarEngineState) == 0x1C) ? 1 : -1];
typedef char CarDrivelineStateSizeCheck[(sizeof(CarDrivelineState) == 0x8) ? 1 : -1];

template <typename T> inline T &CarStateField(EAX_CarState *state, int offset) {
    return *reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(state) + offset);
}

template <typename T> inline const T &CarStateField(const EAX_CarState *state, int offset) {
    return *reinterpret_cast<const T *>(reinterpret_cast<const unsigned char *>(state) + offset);
}

inline const bVector3 *GetWorldVelocity(const WorldConn::Reference &ref) {
    return reinterpret_cast<const WorldRefView *>(&ref)->mVelocity;
}

} // namespace

namespace SoundConn {

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

Pkt_Car_Service::Pkt_Car_Service(float audible_rpm)
    : mRPMPercent(0.0f) //
    , mThrottlePercent(0.0f) //
    , mBrakePercent(0.0f) //
    , mEBrakePercent(0.0f) //
    , mSteering(0.0f) //
    , mGear(1) //
    , mSirenState(-1) //
    , mHotPursuit(false) //
    , mOversteer(0.0f) //
    , mUndersteer(0.0f) //
    , mSlipAngle(0.0f) //
    , mHealth(1.0f) //
    , mAudibleRPMPct(audible_rpm) //
    , mEngineBlown(0) //
    , mNOSFlag(false) //
    , mNOSCapacity(0.0f) //
    , mTrailer(0) //
    , mTimeSinceSeen(0.0f) //
    , mDesiredSpeed(0.0f) //
    , mControlSource(0) {
    const Attrib::Collection *const nullSpec = SimSurface::kNull.GetConstCollection();
    for (int i = 0; i < 4; ++i) {
        mTractionPct[i] = 1.0f;
        if (nullSpec != nullptr) {
            mWheelTerrain[i].Change(nullSpec);
        }
        mWheelSlip[i].x = 0.0f;
        mWheelSlip[i].y = 0.0f;
        mWheelLoad[i] = 0.0f;
        mWheelZforce[i] = 0.0f;
        mWheelOnGround[i] = false;
        mBlownTires[i] = 0;
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

    const char *modelName = *reinterpret_cast<const char *const *>(reinterpret_cast<const char *>(modelData) + 0xC);
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

CarSoundConn::~CarSoundConn() {
    mTarget.Set(0);
    if (g_pEAXSound != nullptr && mState != nullptr) {
        g_pEAXSound->DestroyEAXCar(mState);
    }
    mState = nullptr;
}

HeliSoundConn::HeliSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mState(nullptr) //
    , mTarget(0) {
    Pkt_Heli_Open *oc = static_cast<Pkt_Heli_Open *>(data.pkt);
    Attrib::Instance att(oc->m_VehicleSpec, 0, nullptr);
    mTarget.Set(oc->mWorldID);
    mState = new EAX_HeliState(att.GetConstCollection(), oc->mWorldID);
    if (g_pEAXSound != nullptr && mState != nullptr) {
        g_pEAXSound->SpawnHelicopter(mState);
        mState->mSimUpdating = false;
    }
}

HeliSoundConn::~HeliSoundConn() {
    mTarget.Set(0);
    if (g_pEAXSound != nullptr && mState != nullptr) {
        g_pEAXSound->DestroyEAXHeli(mState);
    }
    delete mState;
    mState = nullptr;
}

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (mConnected && mState != nullptr && mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::OnReceive(Sim::Packet *) {}

void CarSoundConn::OnClose() {
    mConnected = false;
    mTarget.Set(0);
    mState = nullptr;
}

void HeliSoundConn::OnReceive(Sim::Packet *) {}

void HeliSoundConn::OnClose() {
    mTarget.Set(0);
    mState = nullptr;
}

Sim::ConnStatus HeliSoundConn::OnStatusCheck() {
    return Sim::CONNSTATUS_READY;
}

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}

void CarSoundConn::SetAssetsLoaded(CarSoundConn *conn) {
    if (conn != nullptr && conn->mState != nullptr) {
        conn->mState->mAssetsLoaded = true;
    }
}

void CarSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr || mTarget.GetMatrix() == nullptr || !mConnected || mState == nullptr ||
        !CarStateField<bool>(mState, 0x218)) {
        return;
    }

    bMatrix4 &matrix = CarStateField<bMatrix4>(mState, 0x14);
    bVector3 &vel0 = CarStateField<bVector3>(mState, 0x54);
    bVector3 &vel1 = CarStateField<bVector3>(mState, 0x68);
    bVector3 &accel = CarStateField<bVector3>(mState, 0x7C);
    float &brake = CarStateField<float>(mState, 0x78);
    float &eBrake = CarStateField<float>(mState, 0x8C);
    float &fwSpeedState = CarStateField<float>(mState, 0x90);
    float &health = CarStateField<float>(mState, 0x98);
    CarWheelState *const wheels = reinterpret_cast<CarWheelState *>(reinterpret_cast<unsigned char *>(mState) + 0xA8);
    unsigned short &steering = CarStateField<unsigned short>(mState, 0x1B8);
    CarEngineState &engine = CarStateField<CarEngineState>(mState, 0x1BC);
    CarDrivelineState &driveline = CarStateField<CarDrivelineState>(mState, 0x1D8);
    int &siren = CarStateField<int>(mState, 0x1E0);
    unsigned int &hotPursuitWord = CarStateField<unsigned int>(mState, 0x1E4);
    bool &simUpdating = CarStateField<bool>(mState, 0x214);
    unsigned int &trailerID = CarStateField<unsigned int>(mState, 0x224);
    float &oversteer = CarStateField<float>(mState, 0x228);
    float &understeer = CarStateField<float>(mState, 0x22C);
    float &slipAngle = CarStateField<float>(mState, 0x230);
    float &visualRPM = CarStateField<float>(mState, 0x234);
    float &timeSinceSeen = CarStateField<float>(mState, 0x238);
    float &desiredSpeed = CarStateField<float>(mState, 0x240);
    int &controlSource = CarStateField<int>(mState, 0x244);

    SoundConn::Pkt_Car_Service data(visualRPM);
    PSMTX44Copy((Mtx44)mTarget.GetMatrix(), (Mtx44)&matrix);

    if (!Service(&data)) {
        simUpdating = false;
        accel.x = 0.0f;
        accel.y = 0.0f;
        accel.z = 0.0f;
        vel1 = accel;
        vel0 = vel1;
        fwSpeedState = 0.0f;

        const Attrib::Collection *const nullSpec = SimSurface::kNull.GetConstCollection();
        for (int i = 0; i < 4; ++i) {
            CarWheelState &wheel = wheels[i];
            wheel.mWheelOnGround = 1;
            wheel.mWheelForceZ = 0.0f;
            wheel.mWheelSlip.x = 0.0f;
            wheel.mWheelSlip.y = 0.0f;
            if (nullSpec != nullptr) {
                wheel.mTerrainType.Change(nullSpec);
            }
            wheel.mPrevBlownState = 0;
            wheel.mPercentFsFkTransfer = 0.0f;
            wheel.mLoad = 0.0f;
            wheel.mBlownState = 0;
        }

        engine.mBlownFlag = 0;
        engine.mBoost = 0.0f;
        engine.mBoostFlag = 0;
        engine.mNOSFlag = 0;
        engine.mNOS = 0.0f;
        engine.mRPMPct = 0.0f;
        engine.mThrottle = 0.0f;
        return;
    }

    simUpdating = true;
    vel1 = vel0;
    vel0 = *GetWorldVelocity(mTarget);

    const float invDT = 1.0f / dT;
    accel.x = (vel0.x - vel1.x) * invDT;
    accel.y = (vel0.y - vel1.y) * invDT;
    accel.z = (vel0.z - vel1.z) * invDT;

    engine.mRPMPct = data.mRPMPercent;
    engine.mNOSFlag = data.mNOSFlag;
    engine.mNOS = data.mNOSCapacity;
    engine.mThrottle = data.mThrottlePercent;
    engine.mBlownFlag = data.mEngineBlown;
    brake = data.mBrakePercent;
    eBrake = data.mEBrakePercent;
    steering = static_cast<unsigned short>(static_cast<int>(data.mSteering * 10430.378f));
    siren = data.mSirenState;
    hotPursuitWord = *reinterpret_cast<const unsigned int *>(&data.mHotPursuit);
    oversteer = data.mOversteer;
    understeer = data.mUndersteer;
    slipAngle = -data.mSlipAngle;
    health = data.mHealth;

    const bVector3 *velocity = GetWorldVelocity(mTarget);
    const float speedSq = velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z;
    float fwSpeed = 0.0f;
    if (speedSq > 0.0f) {
        fwSpeed = bSqrt(speedSq);
    }
    fwSpeedState = fwSpeed;

    trailerID = data.mTrailer;
    timeSinceSeen = data.mTimeSinceSeen;
    desiredSpeed = data.mDesiredSpeed;
    controlSource = data.mControlSource;

    for (int i = 0; i < 4; ++i) {
        CarWheelState &wheel = wheels[i];
        wheel.mWheelOnGround = data.mWheelOnGround[i] ? 1 : 0;
        wheel.mWheelSlip = data.mWheelSlip[i];
        wheel.mPercentFsFkTransfer = 1.0f - data.mTractionPct[i];
        wheel.mPrevTerrainType = wheel.mTerrainType;
        wheel.mTerrainType = data.mWheelTerrain[i];
        wheel.mLoad = data.mWheelLoad[i];
        wheel.mPrevBlownState = wheel.mBlownState;
        wheel.mBlownState = data.mBlownTires[i];
        wheel.mWheelForceZ = data.mWheelZforce[i];
    }

    const bool gearChanged = driveline.mGear != data.mGear;
    if (gearChanged) {
        driveline.mGear = data.mGear;
    }
    driveline.mGearShiftFlag = gearChanged;
}

void HeliSoundConn::UpdateState(float dT) {
    if (g_pEAXSound == nullptr) {
        if (!btestprint) {
            btestprint = 1;
        }
        return;
    }

    int validTarget = 1;
    if (mTarget.GetMatrix() == nullptr) {
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
    mState->mVel0 = *GetWorldVelocity(mTarget);

    const float invDT = 1.0f / dT;
    mState->mAccel.x = (mState->mVel0.x - mState->mVel1.x) * invDT;
    mState->mAccel.y = (mState->mVel0.y - mState->mVel1.y) * invDT;
    mState->mAccel.z = (mState->mVel0.z - mState->mVel1.z) * invDT;

    const bVector3 *velocity = GetWorldVelocity(mTarget);
    const float speedSq = velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z;
    float fwSpeed = 0.0f;
    if (speedSq > 0.0f) {
        fwSpeed = bSqrt(speedSq);
    }
    mState->mFWSpeed = fwSpeed;
}

template class UTL::Collections::Listable<CarSoundConn, 10>::List;

template class UTL::Collections::Listable<HeliSoundConn, 10>::List;
