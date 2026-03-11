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

void PhysicsObject::Behaviors::Add(Behavior *beh) {
    int pri = beh->GetPriority();
    iterator iter;
    for (iter = begin(); iter != end(); ++iter) {
        if (pri < (*iter)->GetPriority()) {
            break;
        }
    }
    insert(iter, beh);
}

void PhysicsObject::Behaviors::Remove(Behavior *beh) {
    erase(_STL::remove(begin(), end(), beh), end());
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
    : Sim::Object(3) //
    , ISimable(this) //
    , IBody(this) //
    , IAttachable(this) //
    , mAttributes(Attrib::FindCollection(stringhash32(attributeClass), stringhash32(attribName)), 0,
                  nullptr) //
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
        velocity = mRigidBody->GetLinearVelocity();
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void PhysicsObject::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (mRigidBody != nullptr) {
        velocity = mRigidBody->GetAngularVelocity();
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void PhysicsObject::GetDimension(UMath::Vector3 &dim) const {
    if (mRigidBody != nullptr) {
        mRigidBody->GetDimension(dim);
    } else {
        dim = UMath::Vector3::kZero;
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

bool PhysicsObject::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == mBodyService) {
        WorldConn::Pkt_Body_Service *pbs = static_cast<WorldConn::Pkt_Body_Service *>(pkt);
        UMath::Matrix4 mat;
        mRigidBody->GetMatrix4(mat);
        UMath::Copy(mRigidBody->GetPosition(), UMath::ExtractAxis(mat, 3));
        pbs->SetMatrix(mat);
        pbs->SetVelocity(mRigidBody->GetLinearVelocity());
        return true;
    }
    return Sim::Object::OnService(hCon, pkt);
}

bool PhysicsObject::OnTask(HSIMTASK htask, float dT) {
    if (htask == mSimulateTask) {
        if (!IsDirty()) {
            OnTaskSimulate(dT);
            mBehaviors.Simulate(dT);
        }
        return true;
    }
    return false;
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
            ISimable *newOwner = ISimable::FindInstance(owner->GetOwnerHandle());
            if (owner == newOwner) {
                return false;
            }
            owner = newOwner;
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
            irb->GetMatrix4(mat);
            UMath::Copy(irb->GetPosition(), UMath::ExtractAxis(mat, 3));
            WorldConn::Pkt_Body_Open pkt(mWorldID, mat);
            mBodyService = OpenService(UCrc32(0x998c21c0), &pkt);
        }
    }
    for (Behaviors::const_iterator iter = mBehaviors.begin(); iter != mBehaviors.end(); ++iter) {
        (*iter)->OnBehaviorChange(mechanic);
    }
}

bool PhysicsObject::IsBehaviorActive(const UCrc32 &mechanic) const {
    unsigned int key = mechanic.GetValue();
    Mechanics::const_iterator iter = mMechanics.find(key);
    if (iter._M_node == mMechanics.end()._M_node) {
        return false;
    }
    if (iter->second == nullptr) {
        return false;
    }
    return !iter->second->IsPaused();
}

void PhysicsObject::PauseBehavior(const UCrc32 &mechanic, bool pause) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator iter = mMechanics.find(key);
    if (iter._M_node == mMechanics.end()._M_node) {
        return;
    }
    Behavior *beh = mMechanics[key];
    if (beh != nullptr) {
        beh->Pause(pause);
    }
}

bool PhysicsObject::ResetBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator iter = mMechanics.find(key);
    if (iter._M_node == mMechanics.end()._M_node) {
        goto ret_false;
    }
    {
        Behavior *beh = mMechanics[key];
        if (beh != nullptr) {
            beh->Reset();
            return true;
        }
    }
ret_false:
    return false;
}

void PhysicsObject::ReleaseBehaviors() {
    for (Mechanics::iterator iter = mMechanics.begin(); iter != mMechanics.end(); iter++) {
        Behavior *beh = iter->second;
        if (beh != nullptr) {
            UCrc32 mechanic(iter->first);
            mBehaviors.Remove(beh);
            RemoveElement(beh);
            iter->second = nullptr;
            OnBehaviorChange(mechanic);
        }
    }
}

void PhysicsObject::ReleaseBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator iter = mMechanics.find(key);
    if (iter._M_node == mMechanics.end()._M_node) {
        return;
    }
    Behavior *beh = mMechanics[key];
    if (beh != nullptr) {
        mBehaviors.Remove(beh);
        RemoveElement(beh);
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

    UCrc32 sig(behavior);
    if (sig == UCrc32::kNull) {
        return nullptr;
    }

    unsigned int key = mechanic.GetValue();
    BehaviorParams bp = {params, this, behavior, mechanic};
    beh = Behavior::CreateInstance(sig, bp);
    if (beh != nullptr) {
        AddElement(beh);
        mBehaviors.Add(beh);
        mMechanics[key] = beh;
        OnBehaviorChange(mechanic);
    }
    return beh;
}

bool PhysicsObject::Attach(UTL::COM::IUnknown *object) {
    if (mAttachments != nullptr) {
        if (!UTL::COM::ComparePtr(mEntity, object)) {
            Sim::IEntity *e = nullptr;
            object->QueryInterface(&e);
            if (e != nullptr) {
                if (mEntity != nullptr) {
                    Detach(mEntity);
                    mPlayer = nullptr;
                    mEntity = nullptr;
                }
                mEntity = e;
                IPlayer *p = nullptr;
                e->QueryInterface(&p);
                mPlayer = p;
            }
            return mAttachments->Attach(object);
        }
    }
    return false;
}

void PhysicsObject::DetachAll() {
    static_cast<ISimable *>(this)->DetachEntity();
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
}

bool PhysicsObject::Detach(UTL::COM::IUnknown *object) {
    if (!mAttachments) {
        return false;
    }
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

EventSequencer::IEngine *PhysicsObject::GetEventSequencer() {
    return nullptr;
}

Sim::IEntity *PhysicsObject::GetEntity() const {
    return mEntity;
}

bool PhysicsObject::IsPlayer() const {
    return mPlayer != nullptr;
}

IPlayer *PhysicsObject::GetPlayer() const {
    return mPlayer;
}

SimableType PhysicsObject::GetSimableType() const {
    return mObjType;
}

const Attrib::Instance &PhysicsObject::GetAttributes() const {
    return mAttributes;
}

bool PhysicsObject::IsRigidBodySimple() const {
    if (mRigidBody) {
        return mRigidBody->IsSimple();
    }
    return false;
}

bool PhysicsObject::IsRigidBodyComplex() const {
    return !IsRigidBodySimple();
}

WWorldPos &PhysicsObject::GetWPos() {
    return *mWPos;
}

const WWorldPos &PhysicsObject::GetWPos() const {
    return *mWPos;
}

HSIMABLE PhysicsObject::GetOwnerHandle() const {
    return mOwner;
}

ISimable *PhysicsObject::GetOwner() const {
    return ISimable::FindInstance(mOwner);
}

IRigidBody *PhysicsObject::GetRigidBody() {
    return mRigidBody;
}

const UMath::Vector3 &PhysicsObject::GetPosition() const {
    if (mRigidBody == nullptr) {
        return UMath::Vector3::kZero;
    }
    return mRigidBody->GetPosition();
}

unsigned int PhysicsObject::GetWorldID() const {
    return mWorldID;
}

bool PhysicsObject::IsAttached(const UTL::COM::IUnknown *pOther) const {
    if (mAttachments) {
        return mAttachments->IsAttached(pOther);
    }
    return false;
}

const UTL::Std::list<IAttachable *, _type_IAttachableList> *PhysicsObject::GetAttachments() const {
    if (mAttachments == nullptr) {
        return nullptr;
    }
    return &mAttachments->GetList();
}

void PhysicsObject::Behaviors::Reset() {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->Reset();
    }
}
