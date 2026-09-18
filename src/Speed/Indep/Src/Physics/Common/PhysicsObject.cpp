#include "Speed/Indep/Src/Physics/PhysicsObject.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WorldConn.h"

void PhysicsObject::Behaviors::Add(Behavior *beh) {
    int priority = beh->GetPriority();

    for (iterator i = begin(); i != end(); i++) {
        if ((*i)->GetPriority() > priority) {
            insert(i, beh);
            return;
        }
    }

    push_back(beh);
}

void PhysicsObject::Behaviors::Remove(Behavior *beh) {
    erase(std::remove(begin(), end(), beh));
}

PhysicsObject::PhysicsObject(const Attrib::Instance &attribs, SimableType objType, WUID wuid, unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 3),
      ISimable(this),
      IBody(this),
      IAttachable(this),
      mWPos(new WWorldPos(0.025f)),
      mObjType(objType),
      mOwner(NULL),
      mAttributes(attribs.GetConstCollection(), 0, NULL),
      mRigidBody(NULL),
      mEntity(NULL),
      mPlayer(NULL),
      mBodyService(NULL),
      mWorldID((WUID)GetInstanceHandle() | WUID_SIMINSTANCE),
      mAttachments(new Sim::Attachments(this)) {
    if (wuid) {
        mWorldID = wuid;
    }

    mSimulateTask = AddTask(UCrc32("Physics"), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);

    Sim::ProfileTask(mSimulateTask, "Physics");

    Sim::Collision::AddParticipant(GetInstanceHandle());
}

PhysicsObject::PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType, HSIMABLE owner, WUID wuid)
    : Sim::Object(13),
      ISimable(this),
      IBody(this),
      IAttachable(this),
      mWPos(new WWorldPos(0.025f)),
      mObjType(objType),
      mOwner(owner),
      mAttributes(Attrib::FindCollectionWithDefault(Attrib::StringToKey(attributeClass), Attrib::StringToKey(attribName)), 0, NULL),
      mRigidBody(NULL),
      mEntity(NULL),
      mPlayer(NULL),
      mBodyService(NULL),
      mWorldID((WUID)GetInstanceHandle() | WUID_SIMINSTANCE),
      mAttachments(new Sim::Attachments(this)) {
    if (wuid) {
        mWorldID = wuid;
    }

    mSimulateTask = AddTask(UCrc32("Physics"), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);

    Sim::Collision::AddParticipant(GetInstanceHandle());
}

PhysicsObject::~PhysicsObject() {
    ReleaseBehaviors();

    RemoveTask(mSimulateTask);

    DetachEntity();

    if (mAttachments) {
        delete mAttachments;
        mAttachments = NULL;
    }

    if (mBodyService) {
        CloseService(mBodyService);
        mBodyService = NULL;
    }

    if (mWPos) {
        delete mWPos;
    }

    Sim::Collision::RemoveParticipant(GetInstanceHandle());
}

void PhysicsObject::GetTransform(UMath::Matrix4 &matrix) const {
    if (mRigidBody) {
        mRigidBody->GetMatrix4(matrix);
        matrix.v3 = UMath::Vector4Make(mRigidBody->GetPosition(), 1.0f);
    } else {
        UMath::Copy(UMath::Matrix4::kIdentity, matrix);
    }
}

void PhysicsObject::GetLinearVelocity(UMath::Vector3 &velocity) const {
    if (mRigidBody) {
        velocity = mRigidBody->GetLinearVelocity();
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void PhysicsObject::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (mRigidBody) {
        velocity = mRigidBody->GetAngularVelocity();
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void PhysicsObject::GetDimension(UMath::Vector3 &dim) const {
    if (mRigidBody) {
        mRigidBody->GetDimension(dim);
    } else {
        dim = UMath::Vector3::kZero;
    }
}

float PhysicsObject::GetCausalityTime() const {
    const IModel *model = GetModel();
    if (model) {
        return model->GetCausalityTime();
    }
    return 0.0f;
}

void PhysicsObject::SetCausality(HCAUSE from, float time) {
    IModel *model = GetModel();
    if (model) {
        model->SetCausality(from, time);
    }
}

HCAUSE PhysicsObject::GetCausality() const {
    const IModel *model = GetModel();
    if (model) {
        return model->GetCausality();
    }
    return NULL;
}

void PhysicsObject::OnAttached(IAttachable *pOther) {
    mBehaviors.OnOwnerAttached(pOther);
}

void PhysicsObject::OnDetached(IAttachable *pOther) {
    mBehaviors.OnOwnerDetached(pOther);
}

bool PhysicsObject::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == mBodyService) {
        WorldConn::Pkt_Body_Service *bodypkt = (WorldConn::Pkt_Body_Service *)pkt;

        UMath::Matrix4 matrix;

        mRigidBody->GetMatrix4(matrix);
        *(UMath::Vector3 *)&matrix.v3 = mRigidBody->GetPosition();

        bodypkt->SetMatrix(matrix);
        bodypkt->SetVelocity(mRigidBody->GetLinearVelocity());

        return true;
    }

    return false;
}

bool PhysicsObject::OnTask(HSIMTASK htask, float dT) {
    if (htask == mSimulateTask) {
        if (!IsDirty()) {
            OnTaskSimulate(dT);
            mBehaviors.OnTaskSimulate(dT);
        }

        return true;
    }

    return false;
}

void PhysicsObject::Kill() {
    ReleaseGC();

    if (mBodyService) {
        CloseService(mBodyService);
        mBodyService = NULL;
    }
}

void PhysicsObject::SetOwnerObject(ISimable *pOwner) {
    if (pOwner) {
        mOwner = pOwner->GetInstanceHandle();
    } else {
        mOwner = NULL;
    }
}

bool PhysicsObject::IsOwnedByPlayer() const {
    ISimable *owner = ISimable::FindInstance(mOwner);

    while (owner) {
        if (owner->IsPlayer()) {
            return true;
        }

        if (owner->GetOwnerHandle() == NULL || owner->GetOwnerHandle() == owner->GetInstanceHandle()) {
            return false;
        }

        ISimable *next = ISimable::FindInstance(owner->GetOwnerHandle());

        if (owner == next) {
            return false;
        }

        owner = next;
    }

    return false;
}

bool PhysicsObject::IsOwnedBy(ISimable *queriedOwner) const {
    if (queriedOwner) {
        HSIMABLE qSig = queriedOwner->GetInstanceHandle();

        if (mOwner) {
            ISimable *potentialOwner = ISimable::FindInstance(mOwner);

            while (potentialOwner) {
                if (potentialOwner->GetInstanceHandle() == qSig) {
                    return true;
                }

                ISimable *nextInChain = ISimable::FindInstance(potentialOwner->GetOwnerHandle());

                if (potentialOwner == nextInChain) {
                    return false;
                }

                potentialOwner = nextInChain;
            }
        }
    }

    return false;
}

void PhysicsObject::DebugObject() {
    GetRigidBody()->Debug();
}

void PhysicsObject::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->ISimable::QueryInterface(&mRigidBody);

        if (mBodyService == NULL && mRigidBody) {
            UMath::Matrix4 matrix;

            mRigidBody->GetMatrix4(matrix);
            *(UMath::Vector3 *)&matrix.v3 = mRigidBody->GetPosition();

            WorldConn::Pkt_Body_Open open(mWorldID, matrix);

            mBodyService = OpenService(UCrc32(0x998C21C0), &open);
        }
    }

    mBehaviors.OnBehaviorChange(mechanic);
}

bool PhysicsObject::IsBehaviorActive(const UCrc32 &mechanic) const {
    unsigned int key = mechanic.GetValue();
    Mechanics::const_iterator i = mMechanics.find(key);

    if (i == mMechanics.end()) {
        return false;
    }

    const Behavior *beh = (*i).second;

    if (beh == NULL) {
        return false;
    }

    return beh->IsPaused() == false;
}

void PhysicsObject::PauseBehavior(const UCrc32 &mechanic, bool pause) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator i = mMechanics.find(key);

    if (i == mMechanics.end()) {
        return;
    }

    Behavior *beh = mMechanics[key];

    if (beh == NULL) {
        return;
    }

    beh->Pause(pause);
}

bool PhysicsObject::ResetBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator i = mMechanics.find(key);

    if (i == mMechanics.end()) {
        return false;
    }

    Behavior *beh = mMechanics[key];

    if (beh == NULL) {
        return false;
    }

    beh->Reset();
    return true;
}

void PhysicsObject::ReleaseBehaviors() {
    for (Mechanics::iterator i = mMechanics.begin(); i != mMechanics.end(); i++) {
        Behavior *beh = (*i).second;

        if (beh) {
            UCrc32 mechanic = (*i).first;

            mBehaviors.Remove(beh);
            Destroy(beh);
            (*i).second = NULL;
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

    if (beh) {
        mBehaviors.Remove(beh);
        Destroy(beh);

        mMechanics[key] = NULL;
        OnBehaviorChange(mechanic);
    }
}

Behavior *PhysicsObject::FindBehavior(const UCrc32 &mechanic) {
    unsigned int key = mechanic.GetValue();
    Mechanics::iterator i = mMechanics.find(key);

    if (i != mMechanics.end()) {
        Behavior *beh = mMechanics[key];

        if (beh) {
            return beh;
        }
    }

    return NULL;
}

Behavior *PhysicsObject::LoadBehavior(const UCrc32 &mechanic, const UCrc32 &behavior, Sim::Param params) {
    if (IsDirty()) {
        return NULL;
    }

    Behavior *current = FindBehavior(mechanic);

    if (current && current->GetSignature() == behavior) {
        return current;
    }

    ReleaseBehavior(mechanic);

    if (behavior == UCrc32::kNull) {
        return NULL;
    }

    unsigned int key = mechanic.GetValue();
    Behavior *beh = Create(behavior, BehaviorParams(params, this, behavior, mechanic));

    if (beh) {
        mMechanics[key] = beh;

        mBehaviors.Add(beh);

        OnBehaviorChange(mechanic);

        return beh;
    }

    return NULL;
}

bool PhysicsObject::Attach(UTL::COM::IUnknown *object) {
    if (mAttachments == NULL) {
        return false;
    }

    if (UTL::COM::ComparePtr(mEntity, object)) {
        return false;
    }

    Sim::IEntity *ientity;

    if (object->QueryInterface(&ientity)) {
        if (mEntity) {
            Detach(mEntity);
            mEntity = NULL;
            mPlayer = NULL;
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
        mAttachments = NULL;
    }
}

bool PhysicsObject::Detach(UTL::COM::IUnknown *object) {
    if (mAttachments == NULL) {
        return false;
    }

    if (UTL::COM::ComparePtr(mEntity, object)) {
        mEntity = NULL;
        mPlayer = NULL;
    }

    return mAttachments->Detach(object);
}

void PhysicsObject::DetachEntity() {
    if (mEntity) {
        Detach(mEntity);
        mPlayer = NULL;
        mEntity = NULL;
    }
}

void PhysicsObject::AttachEntity(Sim::IEntity *e) {
    if (e == NULL) {
        DetachEntity();
    } else {
        Attach(e);
    }
}

void PhysicsObject::ProcessStimulus(unsigned int stimulus) {
    if (GetEventSequencer()) {
        GetEventSequencer()->ProcessStimulus(stimulus, Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);
    }
}

template <> UTL::Collections::GarbageNode<PhysicsObject, 160>::Collector UTL::Collections::GarbageNode<PhysicsObject, 160>::_mCollector = UTL::Collections::GarbageNode<PhysicsObject, 160>::Collector();
