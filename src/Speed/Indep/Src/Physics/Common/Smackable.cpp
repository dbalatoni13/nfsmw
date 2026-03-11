#include "Speed/Indep/Src/Physics/Smackable.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/HeirarchyModel.h"
#include "Speed/Indep/Src/Physics/PlaceableScenery.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/DamageZones.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

namespace CameraAI {
void AddAvoidable(IBody *body);
void RemoveAvoidable(IBody *body);
} // namespace CameraAI

const ModelHeirarchy *FindSceneryHeirarchyByName(unsigned int name);

namespace Sim {
bool CanSpawnRigidBody(const UMath::Vector3 &position, bool highPriority);
bool CanSpawnSimpleRigidBody(const UMath::Vector3 &position, bool highPriority);
} // namespace Sim

namespace DamageZone {
UCrc32 GetImpactStimulus(unsigned int level);
} // namespace DamageZone

Attrib::StringKey BEHAVIOR_MECHANIC_EFFECTS;

UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::Prototype _Smackable("Smackable",
                                                                       Smackable::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype _RBSmackable(
    "RBSmackable", RBSmackable::Construct);

unsigned int Smackable_RigidCount;
Attrib::StringKey Smackable::CYLINDER;
Attrib::StringKey Smackable::TUBE;
Attrib::StringKey Smackable::CONE;
Attrib::StringKey Smackable::SPHERE;
static float Smackable_ManagementRate = 0.125f;

static float GetDropTimer(const Attrib::Gen::smackable &attributes) {
    float result = attributes.DROPOUT(0);
    if (!(result > 0.0f)) {
        return 0.0f;
    }
    if (attributes.DROPOUT(1) > 0.0f) {
        return result;
    }
    return 0.0f;
}

bool Smackable::Simplify() {
    if (mCollisionBody != nullptr && !mPersistant) {
        const UMath::Vector3 &pos = static_cast< ISimable * >(this)->GetPosition();
        if (Sim::CanSpawnSimpleRigidBody(pos, true)) {
            IRigidBody *irb = GetRigidBody();
            UMath::Vector3 position = irb->GetPosition();
            UMath::Vector3 velocity = irb->GetLinearVelocity();
            UMath::Vector3 angular = irb->GetAngularVelocity();
            float radius = irb->GetRadius();
            float mass = irb->GetMass();
            UMath::Matrix4 matrix = mCollisionBody->GetMatrix4();
            RBSimpleParams rbp(position, velocity, angular, matrix, radius, mass);
            LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"), rbp);
            return true;
        }
    }
    return false;
}

bool Smackable::TrySimplify() {
    typedef UTL::Collections::Listable< Smackable, 160 > SmackList;
    for (Smackable *const *iter = SmackList::GetList().begin();
         iter != SmackList::GetList().end(); ++iter) {
        if ((*iter)->Simplify()) {
            return true;
        }
    }
    return false;
}

ISimable *Smackable::Construct(Sim::Param params) {
    const SmackableParams sp = params.Fetch< SmackableParams >(UCrc32(0xa6b47fac));
    if (sp.fScenery == nullptr) {
        return nullptr;
    }
    Attrib::Gen::smackable attributes(sp.fScenery->GetAttributes());
    if (!attributes.IsValid()) {
        return nullptr;
    }
    IPlaceableScenery *placeable = nullptr;
    bool is_persistant = false;
    if (sp.fScenery->QueryInterface(&placeable)) {
        is_persistant = true;
    }
    bool simple_physics = attributes.SimplePhysics() || sp.fSimplePhysics;
    if (!simple_physics && !is_persistant && Smackable_RigidCount >= 0x14 && !TrySimplify()) {
        return nullptr;
    }
    sp.fScenery->GetWorldID();
    const CollisionGeometry::Bounds *geoms = sp.fScenery->GetCollisionGeometry();
    if (geoms == nullptr || !(attributes.MASS() > 0.0f)) {
        return nullptr;
    }
    UMath::Matrix4 matrix = sp.fMatrix;
    bool canSpawn;
    if (simple_physics) {
        canSpawn = Sim::CanSpawnSimpleRigidBody(UMath::Vector4To3(matrix.v3), false);
    } else {
        canSpawn = Sim::CanSpawnRigidBody(UMath::Vector4To3(matrix.v3), false);
    }
    if (!canSpawn) {
        return nullptr;
    }
    if (!Manager::Exists()) {
        new Manager(Smackable_ManagementRate);
    }
    return new Smackable(matrix, attributes, geoms, sp.fVirginSpawn, sp.fScenery, simple_physics,
                         is_persistant);
}

Smackable::Smackable(const UMath::Matrix4 &matrix, const Attrib::Gen::smackable &attributes,
                     const CollisionGeometry::Bounds *geoms, bool virginspawn, IModel *scenery,
                     bool simple_physics, bool is_persistant)
    : PhysicsObject(attributes, SIMABLE_SMACKABLE, scenery->GetWorldID(), 10) //
    , IDisposable(this) //
    , IRenderable(this) //
    , IExplodeable(this) //
    , EventSequencer::IContext(this) //
    , mAttributes(attributes) //
    , mRBSpecs(static_cast< ISimable * >(this), 0) //
{
    mSimplifyWeight = 0.0f;
    mAge = 0.0f;
    mDropTimer = 0.0f;
    mLife = 0.125f;
    mDropOutTimerMax = GetDropTimer(attributes);
    mOffWorldTimer = 0.0f;
    mAutoSimplify = attributes.AUTO_SIMPLIFY();
    mVirgin = virginspawn;
    mPersistant = is_persistant;
    mLastImpactSpeed = UMath::Vector3::kZero;
    mModel = scenery;
    mGeometry = geoms;
    mManageTask = nullptr;
    mDroppingOut = false;
    mCollisionBody = nullptr;
    mSimpleBody = nullptr;
    UMath::Vector3 dimension;
    geoms->GetHalfDimensions(dimension);
    UMath::Scale(dimension, 2.0f, dimension);
    float mass = attributes.MASS();
    UMath::Vector3 moment = attributes.MOMENT();
    bool active = !virginspawn;
    if (simple_physics) {
        float radius = UMath::Max(dimension.x, UMath::Max(dimension.y, dimension.z));
        RBSimpleParams rbp(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero,
                           UMath::Vector3::kZero, matrix, radius, mass);
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"), rbp);
    } else {
        RBComplexParams rbparams(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero,
                                 UMath::Vector3::kZero, matrix, mass, moment, dimension, geoms,
                                 active, 0);
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RBSmackable"), rbparams);
    }
    for (unsigned int i = 0; i < attributes.Num_BEHAVIORS(); ++i) {
        const Attrib::StringKey &key = attributes.BEHAVIORS(i);
        if (key.IsValid()) {
            LoadBehavior(UCrc32(key), UCrc32(key), Sim::Param());
        }
    }
}

Smackable::~Smackable() {
    DetachAll();
    if (mManageTask != nullptr) {
        RemoveTask(mManageTask);
        mManageTask = nullptr;
    }
    if (mModel != nullptr) {
        Detach(mModel);
        mModel = nullptr;
    }
    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY));
    Sim::Collision::RemoveListener(static_cast< Sim::Collision::IListener * >(this));
}

bool Smackable::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fPosition = mLastCollisionPosition;
    return true;
}

bool Smackable::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position,
                            float dT, IExplosion *explosion) {
    if ((explosion->GetTargets() & 1) == 0) {
        return false;
    }
    IRigidBody *irb = GetRigidBody();
    float factor = mAttributes.ExplosionEffect();
    if (!(0.0f < factor)) {
        return false;
    }
    float targetspeed = explosion->GetExpansionSpeed() * factor;
    UMath::Vector3 point_velocity;
    irb->GetPointVelocity(position, point_velocity);
    float speed = UMath::Dot(point_velocity, normal);
    if (speed < targetspeed) {
        UMath::Vector3 impactvel;
        UMath::Scale(normal, targetspeed - speed, impactvel);
        UMath::Vector3 force;
        UMath::Scale(impactvel, irb->GetMass() / dT, force);
        irb->ResolveForce(force, position);
    }
    EventSequencer::IEngine *sequencer = static_cast< ISimable * >(this)->GetEventSequencer();
    if (sequencer != nullptr) {
        sequencer->ProcessStimulus(0xab556d39, Sim::GetTime(), nullptr,
                                   EventSequencer::QUEUE_ALLOW);
        if (explosion->GetCausality() != nullptr) {
            sequencer->ProcessStimulus(0xffcd8a63, Sim::GetTime(), nullptr,
                                       EventSequencer::QUEUE_ALLOW);
        }
    }
    if (!static_cast< ISimable * >(this)->GetCausality() && explosion->GetCausality()) {
        ICause *cause = ICause::FindInstance(explosion->GetCausality());
        if (cause != nullptr) {
            cause->OnCausedExplosion(explosion, static_cast< ISimable * >(this));
        }
    }
    return true;
}

void Smackable::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        if (static_cast< ISimable * >(this)->QueryInterface(&mCollisionBody)) {
            float detach = mAttributes.DETACH_FORCE();
            if (mVirgin && detach != 0.0f) {
                mCollisionBody->AttachedToWorld(true, UMath::Max(0.0f, detach));
            }
            const CollisionGeometry::Bounds *cog =
                mGeometry->GetChild(UCrc32(0x28b0bb8d));
            if (cog != nullptr) {
                UMath::Vector3 cog_position;
                cog->GetPosition(cog_position);
                mCollisionBody->SetCenterOfGravity(cog_position);
            } else {
                mCollisionBody->DistributeMass();
            }
        }
        if (static_cast< ISimable * >(this)->QueryInterface(&mSimpleBody)) {
            mSimpleBody->ModifyFlags(0, 0x20b);
            ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
        }
    }
}

void Smackable::DoImpactStimulus(unsigned int systemid, float intensity) {
    float time = Sim::GetTime();
    EventSequencer::IEngine *iev = static_cast< ISimable * >(this)->GetEventSequencer();
    if (iev != nullptr) {
        EventSequencer::System *system = iev->FindSystem(systemid);
        if (system != nullptr) {
            intensity = UMath::Clamp(intensity, 0.0f, 1.0f);
            unsigned int level = static_cast< unsigned int >(intensity * 6.0f);
            for (unsigned int i = 0; i < level + 1; i++) {
                UCrc32 stimulus = DamageZone::GetImpactStimulus(i);
                system->ProcessStimulus(stimulus.GetValue(), time,
                                        static_cast< EventSequencer::IContext * >(this),
                                        EventSequencer::QUEUE_ALLOW);
            }
        }
    }
}

void Smackable::OnImpact(float acceleration, float speed,
                         Sim::Collision::Info::CollisionType type, ISimable *iother) {
    Sim::GetTime();
    EventSequencer::IEngine *iev = static_cast< ISimable * >(this)->GetEventSequencer();
    if (iev != nullptr) {
        switch (type) {
        case Sim::Collision::Info::OBJECT:
            if (iother != nullptr) {
                float intensity = acceleration / MPH2MPS(100.0f);
                DoImpactStimulus(0xd59062c8, intensity);
                if (iother->IsPlayer()) {
                    DoImpactStimulus(0x2f698829, intensity);
                }
                if (iother->GetSimableType() == SIMABLE_VEHICLE) {
                    DoImpactStimulus(0x80b88c1d, intensity);
                }
            }
            break;
        case Sim::Collision::Info::GROUND:
            DoImpactStimulus(0x2bf74e61, speed * 0.1f);
            break;
        case Sim::Collision::Info::WORLD:
            DoImpactStimulus(0x7ebe81c0, speed / MPH2MPS(100.0f));
            break;
        default:
            break;
        }
    }
}

void Smackable::OnCollision(const Sim::Collision::Info &cinfo) {
    EventSequencer::IEngine *iev = static_cast< ISimable * >(this)->GetEventSequencer();
    if (iev == nullptr) {
        return;
    }
    float speed = UMath::Length(cinfo.closingVel);
    if (speed < 0.0f) {
        return;
    }
    mLastCollisionPosition = UMath::Vector4Make(cinfo.position, 0.0f);
    HSIMABLE myHandle = static_cast< ISimable * >(this)->GetInstanceHandle();
    if (cinfo.objA == myHandle) {
        UMath::Vector3 normal = cinfo.normal;
        mLastImpactSpeed = cinfo.objAVel;
        float impact_acceleration = cinfo.impulseA;
        Sim::Collision::Info::CollisionType collisionType =
            static_cast< Sim::Collision::Info::CollisionType >(cinfo.type);
        ISimable *other = ISimable::FindInstance(cinfo.objB);
        OnImpact(impact_acceleration, speed, collisionType, other);
    } else if (cinfo.objB == myHandle) {
        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);
        mLastImpactSpeed = cinfo.objBVel;
        float impact_acceleration = cinfo.impulseB;
        Sim::Collision::Info::CollisionType collisionType =
            static_cast< Sim::Collision::Info::CollisionType >(cinfo.type);
        ISimable *other = ISimable::FindInstance(cinfo.objA);
        OnImpact(impact_acceleration, speed, collisionType, other);
    }
}

bool Smackable::InView() const {
    if (mModel != nullptr) {
        return mModel->InView();
    }
    return false;
}

bool Smackable::IsRenderable() const { return mModel != nullptr; }

HMODEL Smackable::GetModelHandle() const {
    HMODEL result = nullptr;
    if (mModel != nullptr) {
        result = mModel->GetInstanceHandle();
    }
    return result;
}

const IModel *Smackable::GetModel() const { return mModel; }
IModel *Smackable::GetModel() { return mModel; }

float Smackable::DistanceToView() const {
    if (mModel != nullptr) {
        return mModel->DistanceToView();
    }
    return 0.0f;
}

void Smackable::Kill() {
    if (mManageTask != nullptr) {
        RemoveTask(mManageTask);
        mManageTask = nullptr;
    }
    if (mModel != nullptr && !mPersistant) {
        mModel->ReleaseModel();
        mModel = nullptr;
    }
    if (mCollisionBody != nullptr) {
        mCollisionBody->DisableModeling();
        mCollisionBody->DisableTriggering();
    }
    if (mSimpleBody != nullptr) {
        mSimpleBody->ModifyFlags(0x308, 0);
    }
    PhysicsObject::Kill();
}

bool Smackable::Dropout() {
    if (mCollisionBody == nullptr) {
        return false;
    }
    if (mDropOutTimerMax <= 0.0f) {
        return false;
    }
    if (!mCollisionBody->IsAttachedToWorld()) {
        mCollisionBody->DisableModeling();
        mCollisionBody->DisableTriggering();
        mDropTimer = mDropOutTimerMax;
        return true;
    }
    return false;
}

bool Smackable::ValidateWorld() {
    const UMath::Vector3 &pos = static_cast< ISimable * >(this)->GetPosition();
    WWorldPos &wpos = static_cast< ISimable * >(this)->GetWPos();
    wpos.FindClosestFace(pos, true);
    if (!wpos.OnValidFace()) {
        goto fail;
    }
    {
        float height = wpos.HeightAtPoint(pos);
        IRigidBody *irb = GetRigidBody();
        float radius = irb->GetRadius();
        if (height <= pos.y + radius) {
            goto success;
        }
    }
fail:
    return false;
success:
    return true;
}

bool Smackable::ShouldDie() {
    if (mDroppingOut) {
        return false;
    }
    if (mCollisionBody == nullptr) {
        return false;
    }
    if (mCollisionBody->IsSleeping()) {
        if (!mCollisionBody->HasHadObjectCollision()) {
            return true;
        }
    }
    if (mCollisionBody == nullptr) {
        goto shouldnt_die;
    }
    if (mCollisionBody->IsModeling()) {
        goto shouldnt_die;
    }
    return true;
shouldnt_die:
    return false;
}

bool Smackable::CanRetrigger() const {
    if (mCollisionBody == nullptr) {
        return false;
    }
    const WWorldPos &wpos = GetWPos();
    if (!wpos.OnValidFace()) {
        return false;
    }
    if (mCollisionBody->IsSleeping()) {
        return true;
    }
    return false;
}

void Smackable::ProcessDeath(float dT) {
    if (ShouldDie()) {
        if (mLife > 0.0f) {
            mLife -= dT;
            return;
        }
        if (Dropout()) {
            return;
        }
        if (mModel != nullptr) {
            if (CanRetrigger()) {
                if (mVirgin && mCollisionBody != nullptr &&
                    mCollisionBody->IsAttachedToWorld()) {
                    ISceneryModel *iscenery = nullptr;
                    if (mModel->QueryInterface(&iscenery)) {
                        iscenery->RestoreScene();
                        mModel = nullptr;
                    }
                } else {
                    ITriggerableModel *itrigger = nullptr;
                    if (mModel->QueryInterface(&itrigger)) {
                        UMath::Matrix4 mat;
                        static_cast< ISimable * >(this)->GetTransform(mat);
                        itrigger->PlaceTrigger(mat, true);
                        static_cast< ISimable * >(this)->Detach(mModel);
                        mModel = nullptr;
                    }
                }
            } else if (mModel != nullptr && !mPersistant) {
                mModel->ReleaseModel();
                mModel = nullptr;
            }
        }
        static_cast< ISimable * >(this)->Kill();
        return;
    }
    mLife = 0.125f;
}

bool Smackable::ProcessDropout(float dT) {
    if (mDropOutTimerMax > 0.0f && mDropTimer > 0.0f) {
        IRigidBody *irb = GetRigidBody();
        mDropTimer -= dT;
        float dropspeed = mAttributes.DROPOUT(1);
        irb->ModifyYPos(-dropspeed * dT);
        if (mDropTimer <= 0.0f) {
            static_cast< ISimable * >(this)->Kill();
            mDropTimer = 0.0f;
        }
        return true;
    }
    return false;
}

void Smackable::ProcessOffWorld(float dT) {
    if (mAttributes.ALLOW_OFF_WORLD()) {
        return;
    }
    if (!ValidateWorld()) {
        mOffWorldTimer += dT;
        if (mOffWorldTimer >= 1.0f) {
            if (mModel != nullptr && !mPersistant) {
                mModel->ReleaseModel();
                mModel = nullptr;
            }
            static_cast< ISimable * >(this)->Kill();
        }
    } else {
        mOffWorldTimer = 0.0f;
    }
}

bool Smackable::OnTask(HSIMTASK htask, float dT) {
    if (htask == mManageTask) {
        Manage(dT);
        return true;
    }
    return PhysicsObject::OnTask(htask, dT);
}

void Smackable::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mModel)) {
        mModel = nullptr;
        if (!UTL::Collections::GarbageNode< PhysicsObject, 160 >::IsDirty()) {
            static_cast< ISimable * >(this)->Kill();
        }
    }
    PhysicsObject::OnDetached(pOther);
}

void Smackable::CalcSimplificationWeight() {
    if (mCollisionBody == nullptr || mPersistant) {
        mSimplifyWeight = -1.0f;
    }
    const UMath::Vector3 &pos = static_cast< ISimable * >(this)->GetPosition();
    float dist = Sim::DistanceToCamera(pos);
    IRigidBody *irb = GetRigidBody();
    float radius = irb->GetRadius();
    float baseweight = static_cast< float >(mAttributes.CAN_SIMPLIFY());
    if (!static_cast< IRenderable * >(this)->IsRenderable()) {
        baseweight += baseweight;
    }
    mSimplifyWeight = (dist + baseweight) / radius;
}

void Smackable::Manage(float dT) {
    ProcessDeath(dT);
    ProcessOffWorld(dT);
    CalcSimplificationWeight();
}

void Smackable::OnTaskSimulate(float dT) {
    mAge += dT;
    if (mCollisionBody != nullptr && mAutoSimplify > 0.0f && mAge > mAutoSimplify) {
        Simplify();
    }
    mDroppingOut = ProcessDropout(dT);
    IRigidBody *irb = GetRigidBody();
    if (mSimpleBody != nullptr) {
        UMath::Vector3 gravity = UMath::Vector3Make(0.0f, mRBSpecs.GRAVITY(), 0.0f);
        UMath::Scale(gravity, irb->GetMass(), gravity);
        irb->ResolveForce(gravity);
    }
}

EventSequencer::IEngine *Smackable::GetEventSequencer() {
    if (mModel != nullptr) {
        return mModel->GetEventSequencer();
    }
    return nullptr;
}

Smackable::Manager::Manager(float rate) : Sim::Activity(0) {
    mManageTask = AddTask("Smackable", rate, 0.0f, Sim::TASK_FRAME_FIXED);
}

Smackable::Manager::~Manager() {
    RemoveTask(mManageTask);
}

bool Smackable::Manager::OnTask(HSIMTASK htask, float dT) {
    if (htask == mManageTask) {
        UTL::Collections::Listable< Smackable, 160 >::Sort(Smackable::SimplifySort);
        if (Smackable_RigidCount > 0xa) {
            TrySimplify();
        }
        return true;
    }
    return false;
}

Behavior *RBSmackable::Construct(const BehaviorParams &parms) {
    RBComplexParams rp = parms.fparams.Fetch< RBComplexParams >(UCrc32(0xa6b47fac));
    return new RBSmackable(parms, rp);
}

RBSmackable::RBSmackable(const BehaviorParams &parms, const RBComplexParams &rp)
    : RigidBody(parms, rp) //
    , mSpecs(this, 0) //
{
    mFrame = 0;
    Smackable_RigidCount++;
}

RBSmackable::~RBSmackable() { Smackable_RigidCount--; }

bool RBSmackable::ShouldSleep() const {
    if (Dynamics::Articulation::IsJoined(this)) {
        return false;
    }
    return RigidBody::ShouldSleep();
}

void RBSmackable::OnTaskSimulate(float dT) {
    RigidBody::OnTaskSimulate(dT);
    mFrame++;
}

bool RBSmackable::CanCollideWith(const RigidBody &other) const {
    if (Dynamics::Articulation::IsJoined(this, &other)) {
        return false;
    }
    return RigidBody::CanCollideWith(other);
}

bool RBSmackable::CanCollideWithGround() const {
    if (IsAttachedToWorld()) {
        return false;
    }
    return RigidBody::CanCollideWithGround();
}

bool RBSmackable::CanCollideWithWorld() const {
    if (IsAttachedToWorld()) {
        return false;
    }
    if (!HasHadCollision()) {
        float velSquare = UMath::LengthSquare(GetLinearVelocity());
        if (velSquare < 4.0f) {
            if ((mFrame & 3) != 0) {
                return false;
            }
        } else if (velSquare < 225.0f) {
            if ((mFrame & 1) != 0) {
                return false;
            }
        }
    }
    return RigidBody::CanCollideWithWorld();
}

HeirarchyModel::HeirarchyModel(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry,
                               UCrc32 nodename, HeirarchyModel *parent,
                               const Attrib::Collection *attribs, const ModelHeirarchy *heirarchy,
                               unsigned int child_index, bool visible)
    : Sim::Model(parent != nullptr ? static_cast< IModel * >(parent) : nullptr, geometry,
                 nodename, 6) //
    , IBody(this) //
    , ITriggerableModel(this) //
    , Attrib::Gen::smackable(attribs, 0, nullptr) //
{
    mTriggerAvoid = UMath::Vector4::kZero;
    mHeirarchy = const_cast< ModelHeirarchy * >(heirarchy);
    mHeirarchyNode = static_cast< unsigned short >(child_index);
    mRenderMesh = rendermesh;
    mOffScreenTimer = 10.0f;
    mChildVisibility = 0xFFFFFFFF;
    mAvoidable = nullptr;
    mTrigger = nullptr;
    mFlags = 0;
    Attrib::Gen::smackable smackable(attribs, 0, nullptr);
    if (visible) {
        RenderConn::Pkt_Smackable_Open pkt(mRenderMesh, GetWorldID(), GetCollisionGeometry(),
                                           mHeirarchy, mHeirarchyNode);
        BeginDraw(UCrc32(0x804c146e), &pkt);
    }
    if (smackable.AI_AVOIDABLE()) {
        if (mAvoidable == nullptr) {
            mAvoidable = new SmackableAvoidable(this);
        }
    }
    if (smackable.CAMERA_AVOIDABLE()) {
        SetCameraAvoidable(true);
    }
}

void HeirarchyModel::SetCameraAvoidable(bool b) {
    bool is_avoidable = mFlags & 1;
    if (static_cast< unsigned short >(b) != is_avoidable) {
        if (b) {
            if (!CAMERA_AVOIDABLE()) {
                return;
            }
            CameraAI::AddAvoidable(static_cast< IBody * >(this));
            mFlags = mFlags | 1;
        } else {
            CameraAI::RemoveAvoidable(static_cast< IBody * >(this));
            mFlags = mFlags & 0xFFFE;
        }
    }
}

bool HeirarchyModel::OnRemoveOffScreen(float dT) {
    if (0.0f < mOffScreenTimer) {
        IModel *model = static_cast< IModel * >(this);
        if (!model->InView()) {
            float timer = mOffScreenTimer - dT;
            mOffScreenTimer = timer;
            if (timer < 0.0f) {
                mOffScreenTimer = 0.0f;
                return true;
            }
        } else {
            mOffScreenTimer = KILL_OFF_SCREEN();
        }
    }
    return false;
}

void HeirarchyModel::OnProcessFrame(float dT) {
    if (OnRemoveOffScreen(dT)) {
        ReleaseModel();
    }
}

void HeirarchyModel::HidePart(const UCrc32 &nodename) {
    int index = FindHeirarchyChild(nodename);
    if (index > -1) {
        mChildVisibility = mChildVisibility & ~(1 << index);
    }
}

void HeirarchyModel::ShowPart(const UCrc32 &nodename) {
    int index = FindHeirarchyChild(nodename);
    if (index > -1) {
        mChildVisibility = mChildVisibility | (1 << index);
    }
}

bool HeirarchyModel::IsPartVisible(const UCrc32 &nodename) const {
    int index = FindHeirarchyChild(nodename);
    if (index < 0) {
        return false;
    }
    if ((mChildVisibility & (1 << index)) == 0) {
        return false;
    }
    return true;
}

int HeirarchyModel::FindHeirarchyChild(const UCrc32 &nodename) const {
    if (mHeirarchy == nullptr) {
        return -1;
    }
    const ModelHeirarchy::Node *nodes = mHeirarchy->GetNodes();
    const ModelHeirarchy::Node &node = nodes[mHeirarchyNode];
    unsigned int numChildren = node.mNumChildren;
    int childindex = -1;
    for (unsigned int i = 0; i < numChildren; ++i) {
        int idx = node.mFirstChild + i;
        if (nodes[idx].mNameHash == nodename.GetValue()) {
            childindex = idx;
            break;
        }
    }
    return childindex;
}

IModel *HeirarchyModel::SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) {
    if (mHeirarchy != nullptr && !IsDirty()) {
        if (UTL::Collections::Listable< IModel, 434 >::Count() > 434u) {
            return nullptr;
        }
        int childindex = FindHeirarchyChild(rendernode);
        if (childindex > -1) {
            const CollisionGeometry::Bounds *geom = GetCollisionGeometry();
            const CollisionGeometry::Bounds *bounds =
                geom->GetChild(collisionnode);
            if (bounds != nullptr) {
                const Attrib::Collection *attribs =
                    SmokeableSpawner::FindAttributes(attributes);
                if (attribs != nullptr) {
                    const ModelHeirarchy::Node *nodes = mHeirarchy->GetNodes();
                    eModel *emodel =
                        reinterpret_cast< eModel * >(nodes[childindex].mModel);
                    if (emodel != nullptr) {
                        bHash32 meshname(emodel->GetNameHash());
                        HeirarchyModel *child = new HeirarchyModel(
                            meshname, bounds, rendernode, this, attribs, mHeirarchy,
                            childindex, true);
                        IModel *result = nullptr;
                        if (child != nullptr) {
                            result = static_cast< IModel * >(child);
                        }
                        return result;
                    }
                }
            }
        }
    }
    return nullptr;
}

HeirarchyModel::~HeirarchyModel() {
    EndSimulation();
    RemoveTrigger();
    ReleaseChildModels();
    if (mAvoidable != nullptr) {
        delete mAvoidable;
        mAvoidable = nullptr;
    }
    SetCameraAvoidable(false);
}

void HeirarchyModel::OnBeginDraw() {
    StartSequencer(UCrc32(EventSequencer()));
    mOffScreenTimer = KILL_OFF_SCREEN();
}

void HeirarchyModel::OnEndDraw() {
    mOffScreenTimer = 0.0f;
    if ((mFlags & 1) != 0) {
        CameraAI::RemoveAvoidable(static_cast< IBody * >(this));
        mFlags = mFlags & 0xFFFE;
    }
}

void HeirarchyModel::GetTransform(UMath::Matrix4 &matrix) const {
    IModel *model = const_cast< IModel * >(static_cast< const IModel * >(this));
    ISimable *simable = model->GetSimable();
    if (simable != nullptr) {
        ISimable *sim = model->GetSimable();
        sim->GetTransform(matrix);
    } else {
        if (mTrigger != nullptr) {
            mTrigger->GetObjectMatrix(matrix);
        } else {
            UMath::Copy(UMath::Matrix4::kIdentity, matrix);
        }
    }
}

void HeirarchyModel::GetAngularVelocity(UMath::Vector3 &velocity) const {
    IModel *model = const_cast< IModel * >(static_cast< const IModel * >(this));
    ISimable *simable = model->GetSimable();
    if (simable != nullptr) {
        ISimable *sim = model->GetSimable();
        IRigidBody *irb = sim->GetRigidBody();
        const UMath::Vector3 &av = irb->GetAngularVelocity();
        velocity = av;
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void HeirarchyModel::RemoveTrigger() {
    if (mTrigger != nullptr) {
        delete mTrigger;
        mTrigger = nullptr;
    }
}

void HeirarchyModel::DisableTrigger() {
    if (mTrigger != nullptr) {
        mTrigger->Disable();
    }
}

void HeirarchyModel::SetTrigger(const UMath::Matrix4 &matrix, bool virgin) {
    UMath::Vector3 dim;
    GetCollisionGeometry()->GetHalfDimensions(dim);
    if (mTrigger == nullptr) {
        mTrigger = new SmackableTrigger(GetInstanceHandle(), virgin, matrix, dim, 0);
    } else {
        mTrigger->Move(matrix, dim, virgin);
        mTrigger->Enable();
    }
    mTriggerAvoid = matrix.v3;
    float zz = dim.y * matrix.v1.z;
    float zx = dim.y * matrix.v1.x;
    zz += dim.x * matrix.v0.z;
    zx += dim.x * matrix.v0.x;
    zz += dim.z * matrix.v2.z;
    zx += dim.z * matrix.v2.x;
    mTriggerAvoid.w = UMath::Sqrt(zx * zx + zz * zz);
}

bool HeirarchyModel::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    if (mTrigger != nullptr && mTrigger->IsEnabled()) {
        if (0.0f < mTriggerAvoid.w) {
            pos = *reinterpret_cast< const UMath::Vector3 * >(&mTriggerAvoid);
            sweep = mTriggerAvoid.w;
            return true;
        }
    } else {
        IModel *model = static_cast< IModel * >(this);
        ISimable *simable = model->GetSimable();
        if (simable != nullptr) {
            ISimable *sim = model->GetSimable();
            IRigidBody *irb = sim->GetRigidBody();
            if (irb != nullptr) {
                sweep = irb->GetRadius();
                const UMath::Vector3 &position = irb->GetPosition();
                pos = position;
                return true;
            }
        }
    }
    return false;
}

void HeirarchyModel::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    SetTrigger(matrix, false);
    if (!enable) {
        DisableTrigger();
    }
}

void HeirarchyModel::OnEndSimulation() {
    if (mAvoidable != nullptr) {
        mAvoidable->SetAvoidableObject(static_cast< IBody * >(this));
    }
}

void HeirarchyModel::OnBeginSimulation() {
    DisableTrigger();
    if (mAvoidable != nullptr) {
        IModel *model = static_cast< IModel * >(this);
        mAvoidable->SetAvoidableObject(model->GetSimable());
    }
    IModel *model = static_cast< IModel * >(this);
    RenderConn::Pkt_Smackable_Open pkt(mRenderMesh, model->GetWorldID(),
                                       model->GetCollisionGeometry(), mHeirarchy,
                                       mHeirarchyNode);
    BeginDraw(UCrc32(0x804c146e), &pkt);
}

bool HeirarchyModel::OnDraw(Sim::Packet *service) {
    RenderConn::Pkt_Smackable_Service *pss =
        static_cast< RenderConn::Pkt_Smackable_Service * >(service);
    UpdateVisibility(pss->mVisible, pss->mDistanceToView);
    pss->mChildVisibility = mChildVisibility;
    return true;
}

PlaceableScenery::PlaceableScenery(bHash32 rendermesh,
                                   const CollisionGeometry::Bounds *geometry,
                                   const Attrib::Collection *attribs,
                                   const ModelHeirarchy *heirarchy)
    : HeirarchyModel(rendermesh, geometry, UCrc32(0x9756df79), nullptr, attribs, heirarchy, 0,
                     false) //
    , IPlaceableScenery(this) //
{
}

void PlaceableScenery::ReleaseModel() {
    static_cast< IPlaceableScenery * >(this)->Destroy();
}

PlaceableScenery *PlaceableScenery::Construct(const char *name, unsigned int attributes) {
    if (static_cast<unsigned int>(UTL::Collections::Listable< IModel, 434 >::Count()) > 434u) {
        return nullptr;
    }
    if (static_cast<unsigned int>(UTL::Collections::Countable< IPlaceableScenery >::Count()) > 12u) {
        return nullptr;
    }
    bHash32 render_name(bStringHashUpper(name));
    UCrc32 collision_name(name);
    bHash32 heirarchy_name(render_name);
    const ModelHeirarchy *heirarchy = FindSceneryHeirarchyByName(render_name.GetValue());
    const CollisionGeometry::Collection *collection = CollisionGeometry::Lookup(collision_name);
    if (collection == nullptr) {
        return nullptr;
    }
    const CollisionGeometry::Bounds *bounds = collection->GetRoot();
    if (bounds != nullptr) {
        eModel model(render_name.GetValue());
        if (model.GetSolid() == nullptr) {
            bHash32 fallback(0xc7395a8);
            render_name = fallback;
            heirarchy_name = fallback;
        }
        const Attrib::Collection *attribs =
            Attrib::FindCollection(Attrib::Gen::smackable::ClassKey(), attributes);
        Attrib::Gen::smackable smk_attribs(attribs, 0, nullptr);
        PlaceableScenery *result = new PlaceableScenery(heirarchy_name, bounds,
                                                        smk_attribs.GetConstCollection(),
                                                        heirarchy);
        return result;
    }
    return nullptr;
}

void PlaceableScenery::PickUp() {
    HideModel();
    StopEffects();
    EndDraw();
    EndSimulation();
    ReleaseSequencer();
    DisableTrigger();
}

bool PlaceableScenery::Place(const UMath::Matrix4 &transform, bool snap_to_ground) {
    static_cast< IPlaceableScenery * >(this)->Destroy();
    UMath::Matrix4 mat;
    UMath::Copy(transform, mat);
    if (snap_to_ground) {
        WCollisionMgr wcm(0, 3);
        float worldHeight = 1000.0f;
        if (!wcm.GetWorldHeightAtPointRigorous(UMath::Vector4To3(mat.v3), worldHeight, nullptr)) {
            return false;
        }
        UMath::Vector3 dim;
        GetDimension(dim);
        mat.v3.y = worldHeight + dim.y;
    }
    PlaceTrigger(mat, false);
    SmackableParams sp(mat, true, static_cast< IModel * >(this), false);
    ISimable *physics = UTL::COM::Factory< Sim::Param, ISimable, UCrc32 >::CreateInstance(
        UCrc32("Smackable"), sp);
    if (physics == nullptr) {
        static_cast< IPlaceableScenery * >(this)->Destroy();
        return false;
    }
    return true;
}

SmackableAvoidable::SmackableAvoidable(HeirarchyModel *model)
    : AIAvoidable(
          mModel != nullptr
              ? static_cast< UTL::COM::IUnknown * >(static_cast< IModel * >(mModel))
              : nullptr) //
    , mModel(model) //
{
    SetAvoidableObject(model != nullptr
                           ? static_cast< UTL::COM::IUnknown * >(static_cast< IBody * >(model))
                           : nullptr);
}

bool SmackableAvoidable::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    return mModel->OnUpdateAvoidable(pos, sweep);
}

bool Smackable::SimplifySort(const Smackable *lhs, const Smackable *rhs) {
    if (lhs->mSimplifyWeight > rhs->mSimplifyWeight) {
        return true;
    }
    if (lhs->mSimplifyWeight < rhs->mSimplifyWeight) {
        return false;
    }
    return lhs->GetInstanceHandle() < rhs->GetInstanceHandle();
}

bool Smackable::IsRequired() const { return false; }
void Smackable::HidePart(const UCrc32 &name) {}
void Smackable::ShowPart(const UCrc32 &name) {}
bool Smackable::IsPartVisible(const UCrc32 &name) const { return true; }

IPlaceableScenery *IPlaceableScenery::CreateInstance(const char *name, unsigned int attributes) {
    return PlaceableScenery::Construct(name, attributes);
}

PlaceableScenery::~PlaceableScenery() {}

void HeirarchyModel::GetDimension(UMath::Vector3 &dim) const {
    GetCollisionGeometry()->GetHalfDimensions(dim);
}

const Attrib::Instance &HeirarchyModel::GetAttributes() const {
    return *static_cast< const Attrib::Instance * >(
        static_cast< const Attrib::Gen::smackable * >(this));
}

unsigned int HeirarchyModel::GetWorldID() const {
    return Sim::Model::GetWorldID();
}

void HeirarchyModel::GetLinearVelocity(UMath::Vector3 &velocity) const {
    Sim::Model::GetLinearVelocity(velocity);
}

void PlaceableScenery::Destroy() {
    Sim::Model::ReleaseModel();
}

bool PlaceableScenery::OnRemoveOffScreen(float time) {
    return false;
}
