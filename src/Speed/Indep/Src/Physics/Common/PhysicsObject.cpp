#define ZPHYSICS_OUTLINE_IENTITY_HANDLE
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WorldConn.h"

#include <algorithm>

HINTERFACE Sim::IEntity::_IHandle() {
    return (HINTERFACE)_IHandle;
}

HINTERFACE IBody::_IHandle() {
    return (HINTERFACE)_IHandle;
}

IBody::~IBody() {}

unsigned int WorldConn::Pkt_Body_Open::Size() {
    return sizeof(*this);
}

unsigned int WorldConn::Pkt_Body_Open::Type() {
    return SType();
}

unsigned int WorldConn::Pkt_Body_Open::SType() {
    static UCrc32 hash = "Pkt_Body_Open";
    return hash.GetValue();
}

void PhysicsObject::Behaviors::Add(Behavior *beh) {
    int pri = beh->GetPriority();
    iterator iter;
    for (iter = begin(); iter != end(); ++iter) {
        if ((*iter)->GetPriority() > pri) {
            break;
        }
    }
    insert(iter, beh);
}

void PhysicsObject::Behaviors::Remove(Behavior *beh) {
    erase(_STL::remove(begin(), end(), beh));
}

PhysicsObject::PhysicsObject(const Attrib::Instance &attribs, SimableType objType, WUID wuid,
                             unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 3) //
    , ISimable(this) //
    , IBody(this) //
    , IAttachable(this) //
    , mAttributes(attribs.GetConstCollection(), 0, nullptr) //
{
    mWPos = new WWorldPos(0.025f);
    mObjType = objType;
    mOwner = nullptr;
    mRigidBody = nullptr;
    mEntity = nullptr;
    mPlayer = nullptr;
    mBodyService = nullptr;
    mWorldID = reinterpret_cast<unsigned int>(GetInstanceHandle()) | 0x1000000;
    if (wuid != 0) {
        mWorldID = wuid;
    }
    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mSimulateTask = AddTask(UCrc32(stringhash32("Physics")), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mSimulateTask, "Physics");
    Sim::Collision::AddParticipant(GetInstanceHandle());
}

PhysicsObject::PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType,
                             HSIMABLE owner, WUID wuid)
    : Sim::Object(13) //
    , ISimable(this) //
    , IBody(this) //
    , IAttachable(this) //
    , mAttributes(Attrib::FindCollectionWithDefault(Attrib::StringToKey(attributeClass),
                                                    Attrib::StringToKey(attribName)),
                  0, nullptr) //
{
    mWPos = new WWorldPos(0.025f);
    mObjType = objType;
    mOwner = owner;
    mRigidBody = nullptr;
    mEntity = nullptr;
    mPlayer = nullptr;
    mBodyService = nullptr;
    mWorldID = reinterpret_cast<unsigned int>(GetInstanceHandle()) | 0x1000000;
    if (wuid != 0) {
        mWorldID = wuid;
    }
    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mSimulateTask = AddTask(UCrc32(stringhash32("Physics")), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mSimulateTask, "Physics");
    Sim::Collision::AddParticipant(GetInstanceHandle());
}

PhysicsObject::~PhysicsObject() {
    ReleaseBehaviors();
    RemoveTask(mSimulateTask);
    DetachEntity();
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
    if (mBodyService) {
        CloseService(mBodyService);
        mBodyService = nullptr;
    }
    if (mWPos) {
        delete mWPos;
    }
    Sim::Collision::RemoveParticipant(GetInstanceHandle());
}

void PhysicsObject::GetTransform(UMath::Matrix4 &matrix) const {
    if (mRigidBody != nullptr) {
        mRigidBody->GetMatrix4(matrix);
        matrix.v3 = UMath::Vector4Make(mRigidBody->GetPosition(), 1.0f);
    } else {
        UMath::Copy(UMath::Matrix4::kIdentity, matrix);
    }
}

void PhysicsObject::GetLinearVelocity(UMath::Vector3 &velocity) const {
    if (mRigidBody != nullptr) {
        UMath::Copy(mRigidBody->GetLinearVelocity(), velocity);
    } else {
        UMath::Copy(UMath::Vector3::kZero, velocity);
    }
}

void PhysicsObject::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (mRigidBody != nullptr) {
        UMath::Copy(mRigidBody->GetAngularVelocity(), velocity);
    } else {
        UMath::Copy(UMath::Vector3::kZero, velocity);
    }
}

void PhysicsObject::GetDimension(UMath::Vector3 &dim) const {
    if (mRigidBody != nullptr) {
        mRigidBody->GetDimension(dim);
    } else {
        UMath::Copy(UMath::Vector3::kZero, dim);
    }
}

float PhysicsObject::GetCausalityTime() const {
    const IModel *model = GetModel();
    if (model != nullptr) {
        return model->GetCausalityTime();
    }
    return 0.0f;
}

void PhysicsObject::SetCausality(HCAUSE from, float time) {
    IModel *model = GetModel();
    if (model != nullptr) {
        model->SetCausality(from, time);
    }
}

HCAUSE PhysicsObject::GetCausality() const {
    const IModel *model = GetModel();
    if (model != nullptr) {
        return model->GetCausality();
    }
    return nullptr;
}

void PhysicsObject::OnAttached(IAttachable *pOther) {
    mBehaviors.OnAttach(pOther);
}

void PhysicsObject::OnDetached(IAttachable *pOther) {
    mBehaviors.OnDetach(pOther);
}

bool PhysicsObject::OnService(HSIMSERVICE hConn, Sim::Packet *pkt) {
    if (hConn == mBodyService) {
        WorldConn::Pkt_Body_Service *pbs = static_cast<WorldConn::Pkt_Body_Service *>(pkt);
        UMath::Matrix4 mat;
        mRigidBody->GetMatrix4(mat);
        UMath::Copy(mRigidBody->GetPosition(), UMath::ExtractAxis(mat, 3));
        pbs->SetMatrix(mat);
        pbs->SetVelocity(mRigidBody->GetLinearVelocity());
        return true;
    }
    return Sim::Object::OnService(hConn, pkt);
}

bool PhysicsObject::OnTask(HSIMTASK htask, float dT) {
    if (htask == mSimulateTask) {
        if (!IsDirty()) {
            OnTaskSimulate(dT);
            mBehaviors.Simulate(dT);
        }
        return true;
    }
    return Sim::Object::OnTask(htask, dT);
}

void PhysicsObject::Kill() {
    ReleaseGC();
    if (mBodyService) {
        CloseService(mBodyService);
        mBodyService = nullptr;
    }
}

void PhysicsObject::SetOwnerObject(ISimable *pOwner) {
    if (pOwner != nullptr) {
        mOwner = pOwner->GetInstanceHandle();
    } else {
        mOwner = nullptr;
    }
}

bool PhysicsObject::IsOwnedByPlayer() const {
    ISimable *owner = ISimable::FindInstance(mOwner);
    if (owner != nullptr) {
        do {
            if (owner->IsPlayer()) {
                return true;
            }
            if (owner->GetOwnerHandle() == nullptr) {
                return false;
            }
            if (owner->GetOwnerHandle() == owner->GetInstanceHandle()) {
                return false;
            }
            ISimable *next = ISimable::FindInstance(owner->GetOwnerHandle());
            if (owner == next) {
                return false;
            }
            owner = next;
        } while (owner != nullptr);
    }
    return false;
}

bool PhysicsObject::IsOwnedBy(ISimable *queriedOwner) const {
    if (queriedOwner != nullptr) {
        HSIMABLE qSig = queriedOwner->GetInstanceHandle();
        ISimable *potentialOwner = ISimable::FindInstance(mOwner);
        if (potentialOwner != nullptr) {
            do {
                if (potentialOwner->GetInstanceHandle() == qSig) {
                    return true;
                }
                HSIMABLE ownerHandle = potentialOwner->GetOwnerHandle();
                if (ownerHandle == nullptr) {
                    return false;
                }
                ISimable *newOwner = ISimable::FindInstance(ownerHandle);
                if (potentialOwner == newOwner) {
                    return false;
                }
                potentialOwner = newOwner;
            } while (potentialOwner != nullptr);
        }
    }
    return false;
}

void PhysicsObject::DebugObject() {
    GetRigidBody()->Debug();
}

void PhysicsObject::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        IRigidBody *irb = nullptr;
        static_cast<ISimable *>(this)->QueryInterface(&irb);
        mRigidBody = irb;
        if (mBodyService == nullptr && irb != nullptr) {
            UMath::Matrix4 mat;
            mRigidBody->GetMatrix4(mat);
            UMath::Copy(mRigidBody->GetPosition(), UMath::ExtractAxis(mat, 3));
            WorldConn::Pkt_Body_Open pkt(mWorldID, mat);
            mBodyService = OpenService(UCrc32(0x998c21c0), &pkt);
        }
    }
    mBehaviors.Changed(mechanic);
}

bool PhysicsObject::IsBehaviorActive(const UCrc32 &mechanic) const {
    unsigned int key = mechanic.GetValue();
    Mechanics::const_iterator iter = mMechanics.find(key);
    if (iter == mMechanics.end()) {
        return false;
    }
    Behavior *beh = (*iter).second;
    if (beh == nullptr) {
        return false;
    }
    return !beh->IsPaused();
}

void PhysicsObject::PauseBehavior(const UCrc32 &mechanic, bool pause) {
    unsigned int key = mechanic.GetValue();
    if (mMechanics.find(key) == mMechanics.end()) {
        return;
    }
    Behavior *beh = mMechanics[key];
    if (beh != nullptr) {
        beh->Pause(pause);
    }
}

bool PhysicsObject::ResetBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    Behavior *beh;
    if (mMechanics.find(key) == mMechanics.end()) {
    } else {
        beh = mMechanics[key];
        if (beh != nullptr) {
            beh->Reset();
            return true;
        }
    }
    return false;
}

void PhysicsObject::ReleaseBehaviors() {
    for (Mechanics::iterator iter = mMechanics.begin(); iter != mMechanics.end(); iter++) {
        Behavior *beh = (*iter).second;
        if (beh != nullptr) {
            UCrc32 mechanic((*iter).first);
            mBehaviors.Remove(beh);
            DestroyElement(*beh);
            (*iter).second = nullptr;
            OnBehaviorChange(mechanic);
        }
    }
}

void PhysicsObject::ReleaseBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    if (mMechanics.find(key) == mMechanics.end()) {
        return;
    }
    Behavior *beh = mMechanics[key];
    if (beh != nullptr) {
        mBehaviors.Remove(beh);
        DestroyElement(*beh);
        mMechanics[key] = nullptr;
        OnBehaviorChange(mechanic);
    }
}

Behavior *PhysicsObject::FindBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    if (mMechanics.find(key) != mMechanics.end()) {
        Behavior *beh = mMechanics[key];
        if (beh != nullptr) {
            return beh;
        }
    }
    return nullptr;
}

Behavior *PhysicsObject::LoadBehavior(const UCrc32 &mechanic, const UCrc32 &behavior,
                                      Sim::Param params) {
    if (IsDirty()) {
        return nullptr;
    }
    Behavior *beh = FindBehavior(mechanic);
    if (beh != nullptr && beh->GetSignature() == behavior) {
        return beh;
    }
    ReleaseBehavior(mechanic);

    if (behavior == UCrc32::kNull) {
        return nullptr;
    }

    unsigned int key = mechanic.GetValue();
    beh = BuildElement(behavior, BehaviorParams(params, this, mechanic, behavior));
    if (beh != nullptr) {
        mMechanics[key] = beh;
        mBehaviors.Add(beh);
        OnBehaviorChange(mechanic);
        return beh;
    }
    return nullptr;
}

bool PhysicsObject::Attach(UTL::COM::IUnknown *object) {
    if (mAttachments == nullptr) {
        return false;
    }
    UTL::COM::ValidatePtr(object);
    if (UTL::COM::ComparePtr(mEntity, object)) {
        return false;
    }
    Sim::IEntity *ientity = reinterpret_cast<Sim::IEntity *>(
        (*reinterpret_cast<UTL::COM::Object **>(object))->_mInterfaces.Find((HINTERFACE)Sim::IEntity::_IHandle)
    );
    bool hasIEntity = ientity != nullptr;
    if (hasIEntity) {
        if (mEntity != nullptr) {
            Detach(mEntity);
            mEntity = nullptr;
            mPlayer = nullptr;
        }
        mEntity = ientity;
        ientity->QueryInterface(&mPlayer);
    }
    return mAttachments->Attach(object);
}

void PhysicsObject::DetachAll() {
    DetachEntity();
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
}

bool PhysicsObject::Detach(UTL::COM::IUnknown *object) {
    if (!mAttachments) {
        return false;
    }
    UTL::COM::ValidatePtr(object);
    if (UTL::COM::ComparePtr(mEntity, object)) {
        mEntity = nullptr;
        mPlayer = nullptr;
    }
    return mAttachments->Detach(object);
}

void PhysicsObject::DetachEntity() {
    if (mEntity != nullptr) {
        Detach(mEntity);
        mPlayer = nullptr;
        mEntity = nullptr;
    }
}

void PhysicsObject::AttachEntity(Sim::IEntity *e) {
    if (e == nullptr) {
        DetachEntity();
    } else {
        Attach(e);
    }
}

void PhysicsObject::ProcessStimulus(unsigned int stimulus) {
    if (GetEventSequencer() != nullptr) {
        GetEventSequencer()->ProcessStimulus(stimulus, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
    }
}

void PhysicsObject::Reset() {
    mBehaviors.Reset();
}

WWorldPos &PhysicsObject::GetWPos() {
    return *mWPos;
}

const WWorldPos &PhysicsObject::GetWPos() const {
    return *mWPos;
}

IRigidBody *PhysicsObject::GetRigidBody() {
    return mRigidBody;
}


void PhysicsObject::Behaviors::Reset() {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->Reset();
    }
}

template void UTL::Vector<Sim::IEntity *, 16>::reserve(UTL::Vector<Sim::IEntity *, 16>::size_type);
template void UTL::Vector<IPlayer *, 16>::reserve(UTL::Vector<IPlayer *, 16>::size_type);
template void UTL::Vector<IModel *, 16>::push_back(IModel *const &);
template void UTL::Vector<IRigidBody *, 16>::push_back(IRigidBody *const &);
template const Attrib::RefSpec &Attrib::Attribute::Get<Attrib::RefSpec>(unsigned int) const;
