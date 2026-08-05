#include "OnlineManager.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/Online/VoiceCore.hpp"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

namespace Online {
    bool IsInitialized();
    void SignalDriverFinish(SmartBitStream &payload_data);
}

extern float kMinBlendTime;
extern float kBlendMult;
extern float kMaxBlendTime;
extern float kMaxLatency;
extern int kSpamPhysics;
extern int kUseDriverAI;

static HSIMABLE__ *ExtrapolatedCar_kHandles[16];
static const unsigned int ExtrapolatedCar_CopTypes[8] = {
    0x82d1a1a3, 0x565398cc, 0xb25e4606, 0xf61dece9,
    0xe22ff3b2, 0xac9e142e, 0x8866eba4, 0x38299bbb};

extern "C" uint32 func_00422F70() {
    return TheOnlineManager.GetMasterTime();
}


ExtrapolatedCar::ExtrapolatedCar(Attrib::Key cartype) {
    mCarType = cartype;
    mCops = nullptr;
    mMutex = static_cast<NetworkMutex *>(gFastMem.Alloc(1, nullptr));
    mLast = mStateArray;
    mActive = true;
    mUpdateTime = 0;
    mTail = 0;
    mHead = 0;
    mCollisionTime = 0.0f;
    mRepositionCount = 0;
    mHasHeadset = false;
    mPaused = false;
    mUseDriverAI = false;
}

ExtrapolatedCar::~ExtrapolatedCar() {
    delete mMutex;
    if (mCops) {
        CopMap::iterator iter = mCops->begin();
        while (iter != mCops->end()) {
            if (iter->second) {
                delete iter->second;
            }
            ++iter;
        }
        mCops->clear();
        delete mCops;
    }
}

ExtrapolatedCar::CopMap::~CopMap() {}

OnlineRacer::~OnlineRacer() {}

void OnlineRacer::operator delete(void *ptr) {
    bFree(ptr);
}

ExtrapolatedCar::State::State() {
    mGear = 1;
    mBlendRate = -1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mSteering = 0.0f;
    mLinearVelocity.x = 0.0f;
    mLinearVelocity.y = 0.0f;
    mLinearVelocity.z = 0.0f;
    mGas = 0.0f;
    mLinearAcceleration.x = 0.0f;
    mLinearAcceleration.y = 0.0f;
    mLinearAcceleration.z = 0.0f;
    mBrake = 0.0f;
    mRotation.x = 0.0f;
    mRotation.y = 0.0f;
    mRotation.z = 0.0f;
    mRotation.w = 1.0f;
    mAngularVelocity.x = 0.0f;
    mAngularVelocity.y = 0.0f;
    mAngularVelocity.z = 0.0f;
    mAngularVelocity.w = 1.0f;
    mAngularAcceleration.x = 0.0f;
    mAngularAcceleration.y = 0.0f;
    mAngularAcceleration.z = 0.0f;
    mAngularAcceleration.w = 1.0f;
    mBlend = 1.0f;
    mHandBrake = 0.0f;
    mTime = 0.0f;
    mInFlight = false;
    mNOS = false;
}

float ExtrapolatedCar::State::SquaredDistanceTo(State &target) const {
    return VU0_v3distancesquare(mPosition, target.mPosition);
}

bool ExtrapolatedCar::State::IsBlending() const { return mBlend > 0.0f; }

bool ExtrapolatedCar::State::IsValidPosition() {
    WCollisionMgr collision_mgr(0, 3);
    float elevation;

    return collision_mgr.GetWorldHeightAtPointRigorous(mPosition, elevation, nullptr);
}

void ExtrapolatedCar::State::SetOnGround(IVehicle *vehicle) {
    UMath::Vector3 forwardvector;

    UMath::ExtractZAxis(mRotation, forwardvector);
    vehicle->SetVehicleOnGround(mPosition, forwardvector);
}

ISimable *ExtrapolatedCar::State::SpawnVehicle(Attrib::Key cartype) {
    UMath::Vector3 forwardvector;

    if (cartype == 0) {
        cartype = Attrib::StringToKey(SkipFEPlayer2Car);
    }
    UMath::ExtractZAxis(mRotation, forwardvector);
    return ISimable::CreateInstance(
        "PVehicle", VehicleParams(nullptr, DRIVER_REMOTE, cartype, forwardvector, mPosition, VPF_SNAP_TO_GROUND, nullptr, nullptr));
}

void ExtrapolatedCar::ExtractExtrapolatedPosition(UMath::Vector3 &position) const {
    position = mBlended.mPosition;
}

void ExtrapolatedCar::State::ExtractDirection(UMath::Vector3 &direction) const {
    UMath::ExtractZAxis(mRotation, direction);
}

void ExtrapolatedCar::State::Import(const ISimable *simable, float simtime) {
    const IRigidBody *irb;
    const IInput *input;
    const ITransmission *transmission;
    const ISuspension *suspension;
    const ICollisionBody *collision_body;
    Dynamics::Inertia::Tensor inertiaTensor;
    UMath::Matrix4 inverseWorldTensor;
    UMath::Vector3 angularAcceleration;
    GearID a;
    GearID amax;

    if (simable->QueryInterface(&input)) {
        const InputControls &controls = input->GetControls();
        mSteering = controls.fSteering;
        mNOS = controls.fNOS;
        mGas = controls.fGas;
        mBrake = controls.fBrake;
        mHandBrake = controls.fHandBrake;
    }

    if (!simable->QueryInterface(&transmission)) {
        mInFlight = false;
    } else {
        a = transmission->GetGear();
        amax = G_SIXTH;
        mGear = static_cast<uint8>(UMath::Clamp(a, G_REVERSE, amax));
        mInFlight = false;
    }

    if (simable->QueryInterface(&suspension)) {
        if (suspension->GetNumWheels() == 0 ||
            suspension->GetNumWheelsOnGround() < suspension->GetNumWheels()) {
            mInFlight = true;
        }
    }

    irb = simable->GetRigidBody();
    mPosition = irb->GetPosition();
    mLinearVelocity = irb->GetLinearVelocity();
    mRotation = irb->GetOrientation();
    UMath::Vector4To3(mAngularVelocity) = irb->GetAngularVelocity();

    if (simable->QueryInterface(&collision_body)) {
        if (UMath::LengthSquare(mLinearVelocity) > 1.0f) {
            UMath::Scale(collision_body->GetForce(), irb->GetOOMass(), mLinearAcceleration);
        } else {
            UMath::Clear(mLinearAcceleration);
        }

        inertiaTensor = collision_body->GetInertiaTensor();
        inertiaTensor.GetInverseWorldTensor(collision_body->GetMatrix4(), inverseWorldTensor);
        VU0_MATRIX3x4_vect3mult(collision_body->GetTorque(), inverseWorldTensor, angularAcceleration);
        UMath::Vector4To3(mAngularAcceleration) = angularAcceleration;
    }

    mTime = simtime;
    mBlend = 1.0f;
    mBlendRate = 1.0f / kMinBlendTime;
}

void ExtrapolatedCar::State::Export(SmartBitStream &data,
                                    ePosDataPriorityMask priority_mask,
                                    uint8 repositioncount) {
    UMath::Matrix4 matrix;

    mPriority = static_cast<uint8>(priority_mask);
    data.AddQuantizedFloat(mTime, TheOnlineManager.QuantFloatSimTime);
    data.AddQuantizedInt(repositioncount, TheOnlineManager.QuantInt2Bit);
    data.AddBool(mInFlight);

    if ((priority_mask & PDP_MASK_CRITICAL) != PDP_MASK_NONE) {
        if (!mInFlight || (priority_mask & PDP_MASK_INFLIGHT) != PDP_MASK_NONE) {
            data.AddQuantizedFloat(mAngularAcceleration.y,
                                   TheOnlineManager.AccelerationQuantizer);
        }
        data.AddQuantizedFloat(
            UMath::Clamp(mLinearAcceleration.x,
                         TheOnlineManager.AccelerationQuantizer.GetMinValue(),
                         TheOnlineManager.AccelerationQuantizer.GetMaxValue()),
            TheOnlineManager.AccelerationQuantizer);
        data.AddQuantizedFloat(
            UMath::Clamp(mLinearAcceleration.y,
                         TheOnlineManager.AccelerationQuantizer.GetMinValue(),
                         TheOnlineManager.AccelerationQuantizer.GetMaxValue()),
            TheOnlineManager.AccelerationQuantizer);
        data.AddQuantizedFloat(
            UMath::Clamp(mLinearAcceleration.z,
                         TheOnlineManager.AccelerationQuantizer.GetMinValue(),
                         TheOnlineManager.AccelerationQuantizer.GetMaxValue()),
            TheOnlineManager.AccelerationQuantizer);
    }

    if ((priority_mask & PDP_MASK_NORMAL) != PDP_MASK_NONE) {
        if (mInFlight) {
            data.AddQuantizedFloat(mAngularVelocity.x,
                                   TheOnlineManager.AVelocityQuantizer);
            data.AddQuantizedFloat(mAngularVelocity.z,
                                   TheOnlineManager.AVelocityQuantizer);
        }
        data.AddQuantizedFloat(mAngularVelocity.y,
                               TheOnlineManager.AVelocityQuantizer);
        data.AddQuantizedFloat(mLinearVelocity.x,
                               TheOnlineManager.VelocityQuantizer);
        data.AddQuantizedFloat(mLinearVelocity.y,
                               TheOnlineManager.VelocityQuantizer);
        data.AddQuantizedFloat(mLinearVelocity.z,
                               TheOnlineManager.VelocityQuantizer);
    }

    if ((priority_mask & PDP_MASK_LOW) != PDP_MASK_NONE) {
        if (!mInFlight && (priority_mask & PDP_MASK_INFLIGHT) == PDP_MASK_NONE) {
            UMath::QuaternionToMatrix4(mRotation, matrix);
            data.AddQuantizedFloat(VU0_Atan2(matrix.v2.z, -matrix.v2.x),
                                   TheOnlineManager.AngleQuantizer);
        } else {
            data.AddQuantizedFloat(mRotation.x, TheOnlineManager.MatrixQuantizer);
            data.AddQuantizedFloat(mRotation.y, TheOnlineManager.MatrixQuantizer);
            data.AddQuantizedFloat(mRotation.z, TheOnlineManager.MatrixQuantizer);
            data.AddQuantizedFloat(mRotation.w, TheOnlineManager.MatrixQuantizer);
        }
        data.AddQuantizedFloat(mPosition.x, TheOnlineManager.PositionQuantizerX);
        data.AddQuantizedFloat(mPosition.y, TheOnlineManager.PositionQuantizerY);
        data.AddQuantizedFloat(mPosition.z, TheOnlineManager.PositionQuantizerZ);
    }

    if ((priority_mask & (PDP_MASK_NORMAL | PDP_MASK_CRITICAL)) != PDP_MASK_NONE) {
        data.AddQuantizedFloat(mSteering, TheOnlineManager.MatrixQuantizer);
        data.AddQuantizedFloat(mHandBrake, TheOnlineManager.ControlQuantizer);
        data.AddQuantizedFloat(mGas, TheOnlineManager.ControlQuantizer);
        data.AddQuantizedFloat(mBrake, TheOnlineManager.ControlQuantizer);
        data.AddQuantizedInt(mGear, TheOnlineManager.QuantInt3Bit);
        data.AddBool(mNOS);
    }
}

void ExtrapolatedCar::State::Export(ISimable *simable) const {
    IRigidBody *irb;
    IVehicle *vehicle;
    ISuspension *suspension;
    IInput *input;
    ITransmission *transmission;
    bool stillinflight;
    UMath::Vector3 position;
    UMath::Vector3 velocity;
    UMath::Matrix4 matrix;
    UMath::Vector3 angularVelocity;

    stillinflight = false;
    irb = simable->GetRigidBody();
    if (!simable->QueryInterface(&vehicle) ||
        vehicle->GetPhysicsMode() == PHYSICS_MODE_EMULATED ||
        !simable->QueryInterface(&suspension) ||
        suspension->GetNumWheelsOnGround() == 0) {
        stillinflight = true;
    }

    if (mInFlight && stillinflight) {
        irb->SetPosition(mPosition);
        irb->SetOrientation(mRotation);
        irb->SetLinearVelocity(mLinearVelocity);
        irb->SetAngularVelocity(UMath::Vector4To3(mAngularVelocity));
    } else {
        position = mPosition;
        velocity = mLinearVelocity;

        if (vehicle) {
            float d;
            d = position.y - irb->GetPosition().y;
            if ((d > -10.0f) && (d < 10.0f)) {
                position.y = irb->GetPosition().y;
                velocity.y = irb->GetLinearVelocity().y;
            }
        }

        if (UMath::Abs(position.x - irb->GetPosition().x) >
            TheOnlineManager.PositionQuantizerX.GetMaxError()) {
            if (UMath::Abs(position.z - irb->GetPosition().z) >
                TheOnlineManager.PositionQuantizerZ.GetMaxError()) {
                irb->SetPosition(position);
            }
        }
        irb->SetLinearVelocity(velocity);

        UMath::QuaternionToMatrix4(mRotation, matrix);
        float result = VU0_Atan2(matrix.v2.z, -matrix.v2.x);
        irb->GetMatrix4(matrix);
        UMath::MultYRot(matrix, result - VU0_Atan2(matrix.v2.z, -matrix.v2.x), matrix);
        irb->SetOrientation(matrix);

        angularVelocity.x = irb->GetAngularVelocity().x;
        angularVelocity.y = mAngularVelocity.y;
        angularVelocity.z = irb->GetAngularVelocity().z;
        irb->SetAngularVelocity(angularVelocity);
    }

    if (simable->QueryInterface(&input)) {
        input->SetControlSteering(mSteering);
        input->SetControlGas(mGas);
        input->SetControlBrake(mBrake);
        input->SetControlHandBrake(mHandBrake);
        input->SetControlNOS(mNOS);
    }

    if (simable->QueryInterface(&transmission)) {
        transmission->Shift(static_cast<GearID>(mGear));
    }

#ifdef EA_PLATFORM_PLAYSTATION2
    if (kSpamPhysics) {
        UMath::QuaternionToEuler(irb->GetOrientation(), position);
    }
#endif
}

void ExtrapolatedCar::ExportStream(SmartBitStream &data,
                                   ePosDataPriorityMask priority_mask) {
    mLast = mStateArray + mHead;
    mLast->Export(data, priority_mask, mRepositionCount);
    data.AddBool(mHasHeadset);
    mSaved = *mLast;
    mUpdateTime = func_00422F70();

    if (mCops) {
        CopMap::iterator iter = mCops->begin();
        while (iter != mCops->end()) {
            int i;
            Attrib::Key type;
            i = 0;
            for (; i < 0xf; ++i) {
                if (iter->first == ExtrapolatedCar_kHandles[i]) {
                    break;
                }
            }

            if (i != 0x10) {
                data.AddQuantizedInt(i, TheOnlineManager.QuantInt4Bit);
                type = 0;
                if (iter->second->GetCarType() != ExtrapolatedCar_CopTypes[0]) {
                    while (static_cast<int>(++type) < 7 &&
                           iter->second->GetCarType() != ExtrapolatedCar_CopTypes[type]) {}
                }
                data.AddQuantizedInt(type, TheOnlineManager.QuantInt3Bit);
                iter->second->ExportStream(data, priority_mask);
            }
            ++iter;
        }
    }
}

void ExtrapolatedCar::ImportStream(SmartBitStream &data,
                                   ePosDataPriorityMask priority_mask) {
    uint8 repositioncount;

    if (Sim::GetState() != Sim::STATE_ACTIVE) {
        return;
    }

    if (mLast->IsBlending()) {
        int c = mHead;
        int n = Next(c);
        if (n == mTail) {
            return;
        }
        mHead = n;
        mLast = mStateArray + c;
    }

    *mLast = mSaved;

    {
        float t = data.GetQuantizedFloat(TheOnlineManager.QuantFloatSimTime);
        mLast->Extrapolate(t);
        repositioncount = mLast->Import(t, data, priority_mask);
    }

    data.GetBool(mHasHeadset);
    if (repositioncount != mRepositionCount ||
        Sim::GetTime() - mLast->GetTime() > kMaxLatency) {
        mLast->Blend(mSaved, kMaxBlendTime);
        mRepositionCount = repositioncount;
    }

    mSaved = *mLast;
    mUpdateTime = func_00422F70();

    if (data.GetByteLengthRemaining() > 1) {
        if (!mCops) {
            mCops = new CopMap;
        }

        HSIMABLE handle = reinterpret_cast<HSIMABLE>(
            data.GetQuantizedInt(TheOnlineManager.QuantInt4Bit));
        Attrib::Key cartype = ExtrapolatedCar_CopTypes[
            data.GetQuantizedInt(TheOnlineManager.QuantInt3Bit)];
        std::pair<CopMap::iterator, bool> result;
        result = mCops->insert(CopMap::value_type(
            handle, static_cast<ExtrapolatedCar *>(nullptr)));
        if (result.second) {
            result.first->second = new ExtrapolatedCar(cartype);
        }
        result.first->second->ImportStream(data, priority_mask);
    }
}

void ExtrapolatedCar::ExportSimable(ISimable *simable) {
    State *state;
    float simtime;

    if (!simable) {
        return;
    }

    state = mStateArray + mHead;
    simtime = Sim::GetTime();
    state->Import(simable, simtime);
    mHasHeadset = VoiceCore::mInstance->IsHeadsetConnected();

    if (!simable->IsPlayer()) {
        return;
    }

    if (mCops) {
        CopMap::iterator nextiter = mCops->begin();
        CopMap::iterator copiter;
        while (nextiter != mCops->end()) {
            copiter = nextiter;
            ++nextiter;

            if (!ISimable::FindInstance(copiter->first)) {
                HSIMABLE handle = copiter->first;
                HSIMABLE__ **handleiter = ExtrapolatedCar_kHandles;
                while (handleiter < ExtrapolatedCar_kHandles + 0xf) {
                    if (handle == *handleiter) {
                        if (!kSpamPhysics) {
                            *handleiter = nullptr;
                        } else {
                            Attrib::Gen::pvehicle vehicle(copiter->second->GetCarType(), 0, nullptr);
                            *handleiter = nullptr;
                        }
                    }
                    ++handleiter;
                }

                delete copiter->second;
                mCops->erase(copiter);
            }
        }
    }

    for (IVehicle::List::const_iterator vehicleiter = IVehicle::GetList(VEHICLE_AICOPS).begin();
         vehicleiter != IVehicle::GetList(VEHICLE_AICOPS).end(); ++vehicleiter) {
        IVehicle *vehicle = *vehicleiter;
        if (vehicle->IsActive() && !vehicle->IsAnimating() && !vehicle->IsDestroyed()) {
            if (!mCops) {
                mCops = new CopMap;
            }

            ISimable *copsimable = vehicle->GetSimable();
            HSIMABLE handle = copsimable->GetInstanceHandle();
            std::pair<CopMap::iterator, bool> result;
            result = mCops->insert(CopMap::value_type(
                handle, static_cast<ExtrapolatedCar *>(nullptr)));
            if (result.second) {
                if (!ExtrapolatedCar_kHandles[0]) {
                    ExtrapolatedCar_kHandles[0] = handle;
                } else {
                    HSIMABLE__ **handleiter = ExtrapolatedCar_kHandles + 1;
                    while (handleiter < ExtrapolatedCar_kHandles + 0xf) {
                        if (!*handleiter) {
                            *handleiter = handle;
                            break;
                        }
                        ++handleiter;
                    }
                }
                result.first->second = new ExtrapolatedCar(vehicle->GetVehicleKey());
            }
            result.first->second->ExportSimable(copsimable);
        }
    }
}

void ExtrapolatedCar::ImportSimable(ISimable *simable, float t, float simtime) {
    int n;

    if (!simable) {
        return;
    }

    if (mCollisionTime == 0.0f) {
        ICollisionBody *collision_body;
        if (simable->QueryInterface(&collision_body) && collision_body->HasHadCollision()) {
            mCollisionTime = simtime;
        }
    }

    n = mTail;
    State &state = mStateArray[n];
    bool colliding = false;
    if (!mPaused && mCollisionTime > 0.0f) {
        if ((state.mPriority & 4) == 0 || state.mTime <= mCollisionTime) {
            colliding = true;
        }
    }

    if (colliding) {
        state.Import(simable, mCollisionTime);
    } else {
        state.Extrapolate(simtime);
        mCollisionTime = 0.0f;
    }

    mBlended = state;

    n = Next(n);
    while (n != mHead) {
        state = mStateArray[n];
        if (state.mTime <= mCollisionTime) {
            mTail = n;
        } else {
            state.Extrapolate(simtime);
            if (state.Blend(mBlended, t)) {
                mTail = n;
            }
        }
        n = Next(n);
    }

    bool needReset = false;
    IVehicle *vehicle;
    if (!simable->QueryInterface(&vehicle)) {
        vehicle = nullptr;
    }
    if (!vehicle) {
        goto import_cops;
    }

    if (Sim::GetTime() - mSaved.GetTime() < kMaxLatency) {
        if (vehicle->GetPhysicsMode() == PHYSICS_MODE_SIMULATED) {
            if (vehicle->IsOffWorld()) {
                mActive = false;
            }
        } else {
            mActive = true;
        }
    } else {
        mHasHeadset = false;
        mActive = false;
    }

    if (kUseDriverAI != mUseDriverAI) {
        IHumanAI *human_ai;
        if (simable->QueryInterface(&human_ai)) {
            human_ai->SetAiControl(kUseDriverAI);
        }
        mUseDriverAI = kUseDriverAI;
        needReset = kUseDriverAI == 0;
    }

    if (!mActive) {
        if (vehicle->GetPhysicsMode() == PHYSICS_MODE_SIMULATED) {
            if (kSpamPhysics) {
                simable->QueryInterface(&vehicle);
            }
            vehicle->SetPhysicsMode(PHYSICS_MODE_EMULATED);
            needReset = false;
        }
    } else if (vehicle->GetPhysicsMode() == PHYSICS_MODE_EMULATED) {
        if (mBlended.IsValidPosition()) {
            vehicle->SetPhysicsMode(PHYSICS_MODE_SIMULATED);
            needReset = true;
        } else if (mPaused) {
            needReset = true;
        }
    } else if (mPaused) {
        needReset = true;
    }

    if (needReset) {
        mBlended.SetOnGround(vehicle);
    }
    if (!mUseDriverAI) {
        mBlended.Export(simable);
    }

import_cops:
    if (!mCops) {
        return;
    }

    CopMap::iterator copiter = mCops->begin();
    while (copiter != mCops->end()) {
        CopMap::iterator nextiter = copiter;
        ++nextiter;

        n = static_cast<int>(reinterpret_cast<uintptr_t>(copiter->first));
        ISimable *cop_simable = nullptr;
        if (!ExtrapolatedCar_kHandles[n]) {
            cop_simable = copiter->second->mLast->SpawnVehicle(copiter->second->mCarType);
            if (cop_simable) {
                if (kSpamPhysics) {
                    cop_simable->QueryInterface(&vehicle);
                }
                ExtrapolatedCar_kHandles[n] = cop_simable->GetInstanceHandle();
            }
        } else {
            cop_simable = ISimable::FindInstance(ExtrapolatedCar_kHandles[n]);
            if (!cop_simable) {
                cop_simable = copiter->second->mLast->SpawnVehicle(copiter->second->mCarType);
                ExtrapolatedCar_kHandles[n] = cop_simable->GetInstanceHandle();
            }
        }

        if (cop_simable) {
            copiter->second->ImportSimable(cop_simable, t, simtime);
            if (copiter->second->mSaved.GetTime() < mSaved.GetTime()) {
                if (kSpamPhysics) {
                    cop_simable->QueryInterface(&vehicle);
                }
                cop_simable->Kill();
                ExtrapolatedCar_kHandles[n] = nullptr;
                delete copiter->second;
                mCops->erase(copiter);
            }
        }

        copiter = nextiter;
    }
}

void ExtrapolatedCar::State::Extrapolate(float simtime) {
    float amin;
    float amax;
    UMath::Vector4 temp;
    UMath::Matrix4 matrix;

    if (mBlendRate < 0.0f) {
        amin = UMath::Min(kBlendMult * (simtime - mTime), kMaxBlendTime);
        amax = UMath::Max(kMinBlendTime, amin);
        mBlendRate = 1.0f / amax;
    }

    if (mTime != simtime) {
        do {
            float dt = UMath::Clamp(simtime - mTime, -0.016666668f, 0.016666668f);

            UMath::ScaleAdd(mLinearAcceleration, dt, mLinearVelocity, mLinearVelocity);
            UMath::ScaleAdd(mLinearVelocity, dt, mPosition, mPosition);
            if (!mInFlight) {
                UMath::ScaleAdd(mAngularAcceleration, dt, mAngularVelocity, mAngularVelocity);
            }
            UMath::Mult(mAngularVelocity, mRotation, temp);
            UMath::ScaleAdd(temp, dt * 0.5f, mRotation, mRotation);
            UMath::Normalize(mRotation);

            UMath::SetYRot(matrix, mAngularVelocity.y * dt * UMath::OOTWOPI);
            UMath::Rotate(mLinearAcceleration, matrix, mLinearAcceleration);

            mTime += dt;
        } while (mTime != simtime);
    }
}

bool ExtrapolatedCar::State::Blend(State &blended, float t) {
    float s;

    if (mBlendRate < 0.0f) {
        mBlendRate = 1.0f / kMinBlendTime;
    }
    mBlend = t * mBlendRate + mBlend;

    if (mBlend < 1.0f) {
        s = 1.0f - mBlend * mBlend * (3.0f - (mBlend + mBlend));

        UMath::Sub(blended.mPosition, mPosition, blended.mPosition);
        UMath::ScaleAdd(blended.mPosition, s, mPosition, blended.mPosition);
        UMath::Sub(blended.mLinearVelocity, mLinearVelocity, blended.mLinearVelocity);
        UMath::ScaleAdd(blended.mLinearVelocity, s, mLinearVelocity, blended.mLinearVelocity);
#ifdef EA_PLATFORM_PLAYSTATION2
        UMath::Slerp(mRotation, blended.mRotation, s, blended.mRotation);
#endif
        blended.mAngularVelocity.x -= mAngularVelocity.x;
        blended.mAngularVelocity.y -= mAngularVelocity.y;
        blended.mAngularVelocity.z -= mAngularVelocity.z;
        blended.mAngularVelocity.w -= mAngularVelocity.w;
        blended.mAngularVelocity.x = blended.mAngularVelocity.x * s + mAngularVelocity.x;
        blended.mAngularVelocity.y = blended.mAngularVelocity.y * s + mAngularVelocity.y;
        blended.mAngularVelocity.z = blended.mAngularVelocity.z * s + mAngularVelocity.z;
        blended.mAngularVelocity.w = blended.mAngularVelocity.w * s + mAngularVelocity.w;

        blended.mSteering = (blended.mSteering - mSteering) * s + mSteering;
        blended.mGas = (blended.mGas - mGas) * s + mGas;
        blended.mBrake = (blended.mBrake - mBrake) * s + mBrake;
        blended.mHandBrake = (blended.mHandBrake - mHandBrake) * s + mHandBrake;
        blended.mGear = mGear;
        *reinterpret_cast<uint32 *>(&blended.mNOS) =
            *reinterpret_cast<const uint32 *>(&mNOS);
        return false;
    }

    blended = *this;
    return true;
}

uint8 ExtrapolatedCar::State::Import(float time, SmartBitStream &data,
                                     ePosDataPriorityMask priority_mask) {
    UMath::Matrix4 matrix;
    float AccError;
    float VelError;
    int repositioncount;

    mPriority = static_cast<uint8>(priority_mask);
    mTime = time;
    repositioncount = data.GetQuantizedInt(TheOnlineManager.QuantInt2Bit);
    mInFlight = data.GetBool();

    if ((priority_mask & PDP_MASK_CRITICAL) != PDP_MASK_NONE) {
        mAngularAcceleration = UMath::Vector4::kZero;

        AccError = TheOnlineManager.AccelerationQuantizer.GetMaxError();
        AccError += AccError;
        if (!mInFlight || (priority_mask & PDP_MASK_INFLIGHT) != PDP_MASK_NONE) {
            mAngularAcceleration.y =
                data.GetQuantizedFloat(TheOnlineManager.AccelerationQuantizer);
            if (bAbs(mAngularAcceleration.y) < AccError) {
                mAngularAcceleration.y = 0.0f;
            }
        } else {
            mLinearAcceleration.z = 0.0f;
        }

        mLinearAcceleration.x = 0.0f;
        mLinearAcceleration.y = 0.0f;
        mLinearAcceleration.z = 0.0f;
        mLinearAcceleration.x =
            data.GetQuantizedFloat(TheOnlineManager.AccelerationQuantizer);
        if (bAbs(mLinearAcceleration.x) < AccError) {
            mLinearAcceleration.x = 0.0f;
        }
        mLinearAcceleration.y =
            data.GetQuantizedFloat(TheOnlineManager.AccelerationQuantizer);
        if (bAbs(mLinearAcceleration.y) < AccError) {
            mLinearAcceleration.y = 0.0f;
        }
        mLinearAcceleration.z =
            data.GetQuantizedFloat(TheOnlineManager.AccelerationQuantizer);
        if (bAbs(mLinearAcceleration.z) < AccError) {
            mLinearAcceleration.z = 0.0f;
        }
    }

    if ((priority_mask & PDP_MASK_NORMAL) != PDP_MASK_NONE) {
        mAngularVelocity = UMath::Vector4::kZero;

        if (mInFlight || (priority_mask & PDP_MASK_INFLIGHT) != PDP_MASK_NONE) {
            mAngularVelocity.x =
                data.GetQuantizedFloat(TheOnlineManager.AVelocityQuantizer);
            mAngularVelocity.z =
                data.GetQuantizedFloat(TheOnlineManager.AVelocityQuantizer);
        }

        mAngularVelocity.y = data.GetQuantizedFloat(TheOnlineManager.AVelocityQuantizer);
        VelError = TheOnlineManager.AVelocityQuantizer.GetMaxError();
        VelError += VelError;
        if (bAbs(mAngularVelocity.y) < VelError) {
            mAngularVelocity.y = 0.0f;
        }

        VelError = TheOnlineManager.VelocityQuantizer.GetMaxError();
        VelError += VelError;
        mLinearVelocity.x = data.GetQuantizedFloat(TheOnlineManager.VelocityQuantizer);
        if (bAbs(mLinearVelocity.x) < VelError) {
            mLinearVelocity.x = 0.0f;
        }
        mLinearVelocity.y = data.GetQuantizedFloat(TheOnlineManager.VelocityQuantizer);
        if (bAbs(mLinearVelocity.y) < VelError) {
            mLinearVelocity.y = 0.0f;
        }
        mLinearVelocity.z = data.GetQuantizedFloat(TheOnlineManager.VelocityQuantizer);
        if (bAbs(mLinearVelocity.z) < VelError) {
            mLinearVelocity.z = 0.0f;
        }
    }

    if ((priority_mask & PDP_MASK_LOW) != PDP_MASK_NONE) {
        if (!mInFlight && (priority_mask & PDP_MASK_INFLIGHT) == PDP_MASK_NONE) {
            float f = data.GetQuantizedFloat(TheOnlineManager.AngleQuantizer);
            float a = floorf((f - 0.25f) + 0.5f);
            UMath::SetYRot(matrix, (f - 0.25f) - a);
            UMath::Matrix4ToQuaternion(matrix, mRotation);
        } else {
            mRotation.x = data.GetQuantizedFloat(TheOnlineManager.MatrixQuantizer);
            mRotation.y = data.GetQuantizedFloat(TheOnlineManager.MatrixQuantizer);
            mRotation.z = data.GetQuantizedFloat(TheOnlineManager.MatrixQuantizer);
            mRotation.w = data.GetQuantizedFloat(TheOnlineManager.MatrixQuantizer);
        }

        mPosition.x = data.GetQuantizedFloat(TheOnlineManager.PositionQuantizerX);
        mPosition.y = data.GetQuantizedFloat(TheOnlineManager.PositionQuantizerY);
        mPosition.z = data.GetQuantizedFloat(TheOnlineManager.PositionQuantizerZ);
    }

    if ((priority_mask & (PDP_MASK_NORMAL | PDP_MASK_CRITICAL)) != PDP_MASK_NONE) {
        mSteering = data.GetQuantizedFloat(TheOnlineManager.MatrixQuantizer);
        mHandBrake = data.GetQuantizedFloat(TheOnlineManager.ControlQuantizer);
        mGas = data.GetQuantizedFloat(TheOnlineManager.ControlQuantizer);
        mBrake = data.GetQuantizedFloat(TheOnlineManager.ControlQuantizer);
        mGear = static_cast<uint8>(data.GetQuantizedInt(TheOnlineManager.QuantInt3Bit));
        mNOS = data.GetBool();
    }

    mBlend = 0.0f;
    mBlendRate = -1.0f;
    return static_cast<uint8>(repositioncount);
}

void ExtrapolatedCar::ExtractExtrapolatedDirection(UMath::Vector3 &direction) const {
    mBlended.ExtractDirection(direction);
}

bool ExtrapolatedCar::IsAbleToSee(ExtrapolatedCar &target) {
    float fVar1;

    fVar1 = mBlended.SquaredDistanceTo(target.mBlended);
    return fVar1 < 40000.0f;
}

OnlineRacer::OnlineRacer(int8 driver_number, bool is_server, const char *persona)
    : ExtrapolatedCar(0)
    , CarCustomization()
    , FinishedRaceStats() {
    DriverNumber = driver_number;
    bIsServer = is_server;
    DisconnectTime.SetTime(0.0f);
    RaceScore = 0;
    State = OPS_DISCONNECTED;
    bShouldRestart = false;
    PhysicsDataCRC = 0;
    BadnessReason = 0;
    SyncScoreMsgID = 0;
    BadnessCountdown.SetTime(0.0f);
    GraceCountdown.SetTime(0.0f);
    PlayerID = -1;
    Reputation = 100;
    LastSpamRealTime = -1.0f;
    EndRaceCountdown = -1.0f;
    ClearCheatInfo();
    DisconnectTime.SetTime(0.0f);
    bMemSet(&FinishedRaceStats, 0, 0xc0);
    if (SkipFE) {
        SetPersona("SkipFE");
    } else if (persona) {
        SetPersona(persona);
    } else {
        bMemSet(Persona, 0, 0x10);
    }
}

void OnlineRacer::SetPersona(const char *persona) {
    bMemCpy(Persona, persona, 0x10);
}

void OnlineRacer::SetRaceScore(int score) {
    if (RaceScore != score) {
        RaceScore = score;
    }
}

bool OnlineRacer::IsFinishedRacing() {
    if (State == OPS_LOST_CONNECTION || State == OPS_QUIT ||
        State == OPS_DISCONNECTED || State == OPS_DISCERROR || State == OPS_FINISHED) {
        return true;
    }
    return TheOnlineManager.GetState() > OLS_RACING && State < OPS_RACING;
}

void OnlineRacer::ChangeState(eOnlineRacerState new_state) {
    if ((new_state != State) && (State = new_state, new_state == OPS_RACING)) {
        BadnessCountdown.SetTime(20.0f);
        GraceCountdown.SetTime(6.0f);
        BadnessReason = 0;
    }
}

void OnlineRacer::DriverDisconnect(eOnlineRacerState new_state, int finish_reason) {
    bool connected = false;
    if (State == OPS_LOST_CONNECTION || State == OPS_QUIT || State == OPS_DISCONNECTED) {
    } else {
        connected = State != OPS_DISCERROR;
    }

    if (connected) {
        ChangeState(new_state);
        FinishedRaceStats.FinishReason = finish_reason;
    } else {
        FinishedRaceStats.FinishReason = finish_reason;
    }
}

void OnlineRacer::ClearCheatInfo() {
    int i;
    for (i = 15; i > -1; i--) {
        CheatTally[i] = 0;
    }
}

uint8 OnlineRacer::GetCheatScore() { return 0; }

uint32 OnlineRacer::GetDataCRC(bool recalc) { return PhysicsDataCRC; }

float OnlineRacer::GetEndRaceCountdown() { return EndRaceCountdown; }

void OnlineRacer::UpdateEndRaceStats() {}

void OnlineRacer::Finish(int nRank, bool bBlinkBlinkPoof, int raceFinishReason) {
    SmartBitStream data;
    data.AddByte(DriverNumber);
    data.AddByte(static_cast<uint8>(nRank));
    uint8 value = 0;
    if (bBlinkBlinkPoof || raceFinishReason == 0xb) {
        value = 1;
    }
    data.AddByte(value);
    data.AddByte(static_cast<uint8>(raceFinishReason));
    data.AddRawData(reinterpret_cast<const char *>(CheatTally), 0x20);
    UpdateEndRaceStats();
    data.AddRawData(reinterpret_cast<const char *>(&FinishedRaceStats), 0xc0);
    if (Online::IsInitialized()) {
        Online::SignalDriverFinish(data);
    }
    ChangeState(OPS_FINISHED);
}

void OnlineRacer::SignalFinish(SmartBitStream &data) {
    volatile uint8 rank;
    volatile uint8 blinkPoof;
    volatile uint8 finishReason;
    uint16 remote_cheat_tally[16];

    {
        uint32 v = 0;
        data.GetBits(v, 8);
        rank = v;
    }
    {
        uint32 v = 0;
        data.GetBits(v, 8);
        blinkPoof = v;
    }
    {
        uint32 v = 0;
        data.GetBits(v, 8);
        finishReason = v;
    }
    data.GetRawData(reinterpret_cast<char *>(remote_cheat_tally), 0x20);
    data.GetRawData(reinterpret_cast<char *>(&FinishedRaceStats), 0xc0);

    uint16 *cheat_tally = CheatTally;
    uint16 *buffer = remote_cheat_tally;
    int i;
    for (i = 15; i >= 0; --i) {
        *cheat_tally = bMax(*cheat_tally, *buffer);
        ++cheat_tally;
        ++buffer;
    }
    ChangeState(OPS_FINISHED);
}

void OnlineRacer::UpdateLocal(float t) {
    if (GRaceStatus::Exists()) {
        BadnessReason = 0;
        EndRaceCountdown = -1.0f;
        IPlayer *p = IPlayer::First(PLAYER_LOCAL);
        GRacerInfo *localRacerInfo = GRaceStatus::Get().GetRacerInfo(p->GetSimable());

        if (!TheOnlineManager.AreAllPlayersFinishedRacing() &&
            TheOnlineManager.TimeupStartTime.IsSet()) {
            EndRaceCountdown = TheOnlineManager.TimeupLength -
                static_cast<float>(WorldTimer.GetPackedTime() -
                                   TheOnlineManager.TimeupStartTime.GetPackedTime()) * 0.00025f;
            if (EndRaceCountdown <= 0.0f && !localRacerInfo->mFinishedRacing) {
                TheOnlineManager.RaceTimeup = true;
                GRaceStatus::Get().SkipToEndOfRaceForRacer(
                    IPlayer::First(PLAYER_LOCAL)->GetSimable(), localRacerInfo->mIndex, 3600.0f);
            }
        }
    }
}
