#include "Speed/Indep/Src/Physics/Smackable.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
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
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __RBSmackable(
    "RBSmackable", RBSmackable::Construct);

unsigned int Smackable_RigidCount;
Attrib::StringKey Smackable::CYLINDER;
Attrib::StringKey Smackable::TUBE;
Attrib::StringKey Smackable::CONE;
Attrib::StringKey Smackable::SPHERE;
static float Smackable_ManagementRate = 0.125f;

IDisposable::~IDisposable() {}

Attrib::Key Attrib::Gen::smackable::ClassKey() {
    return 0xce70d7db;
}

HINTERFACE ISimpleBody::_IHandle() {
    return (HINTERFACE)_IHandle;
}

HINTERFACE IPlaceableScenery::_IHandle() {
    return (HINTERFACE)_IHandle;
}

IPlaceableScenery::~IPlaceableScenery() {}

HINTERFACE EventSequencer::IContext::_IHandle() {
    return (HINTERFACE)_IHandle;
}

EventSequencer::IContext::~IContext() {}

static float GetDropTimer(const Attrib::Gen::smackable &attributes) {
    float result;
    result = attributes.DROPOUT(0);
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
        const UMath::Vector3 &pos = static_cast<ISimable *>(this)->GetPosition();
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
    for (Smackable *const *iter = UTL::Collections::Listable< Smackable, 160 >::GetList().begin();
         iter != UTL::Collections::Listable< Smackable, 160 >::GetList().end(); ++iter) {
        Smackable *smack = *iter;
        if (smack->Simplify()) {
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
    if (geoms == nullptr) {
        return nullptr;
    }
    if (!(attributes.MASS() > 0.0f)) {
        return nullptr;
    }
    UMath::Matrix4 matrix = sp.fMatrix;
    if (simple_physics) {
        if (!Sim::CanSpawnSimpleRigidBody(UMath::Vector4To3(matrix.v3), false)) {
            return nullptr;
        }
    } else {
        if (!Sim::CanSpawnRigidBody(UMath::Vector4To3(matrix.v3), false)) {
            return nullptr;
        }
    }
    if (Manager::Get() == nullptr) {
        if (new Manager(Smackable_ManagementRate) == nullptr) {
            return nullptr;
        }
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
    , mSimplifyWeight(0.0f) //
    , mAge(0.0f) //
    , mLife(Smackable_ManagementRate) //
    , mDropTimer(0.0f) //
    , mDropOutTimerMax(GetDropTimer(attributes)) //
    , mOffWorldTimer(0.0f) //
    , mAutoSimplify(attributes.AUTO_SIMPLIFY()) //
    , mVirgin(virginspawn) //
    , mModel(scenery) //
    , mGeometry(geoms) //
    , mManageTask(nullptr) //
    , mDroppingOut(false) //
    , mPersistant(is_persistant) //
    , mCollisionBody(nullptr) //
    , mSimpleBody(nullptr) //
    , mLastImpactSpeed(UMath::Vector3::kZero) //
    , mRBSpecs(static_cast<ISimable *>(this), 0) //
    , mLastCollisionPosition(UMath::Vector4::kZero) //
{
    UMath::Vector3 dimension;
    geoms->GetHalfDimensions(dimension);
    dimension.x = UMath::Max(dimension.x, 0.025f);
    dimension.y = UMath::Max(dimension.y, 0.025f);
    dimension.z = UMath::Max(dimension.z, 0.025f);
    float radius = UMath::Length(dimension);
    float mass = attributes.MASS();
    Dynamics::Inertia::Box inertia(mass, dimension.x, dimension.y, dimension.z);
    UMath::Scale(inertia, 2.0f, inertia);
    UMath::Vector3 moment;
    if (attributes.MOMENT(moment)) {
        if (moment.x > 0.0f) {
            inertia.x *= moment.x;
        }
        if (moment.y > 0.0f) {
            inertia.y *= moment.y;
        }
        if (moment.z > 0.0f) {
            inertia.z *= moment.z;
        }
    }
    bool active = !virginspawn || mPersistant;
    UCrc32 smack_class;
    if (simple_physics) {
        RBSimpleParams rbp(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero,
                           UMath::Vector3::kZero, matrix, radius, mass);
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"), rbp);
    } else {
        RBComplexParams rbparams(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero,
                                 UMath::Vector3::kZero, matrix, mass, inertia, dimension, geoms,
                                 active, 0);
        if (mPersistant) {
            LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RigidBody"), rbparams);
        } else {
            LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RBSmackable"), rbparams);
        }
    }
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), UCrc32("EffectsSmackable"), Sim::Param());
    for (unsigned int i = 0; i < mAttributes.Num_BEHAVIORS(); ++i) {
        const Attrib::StringKey &key = mAttributes.BEHAVIORS(i);
        if (key.IsNotEmpty()) {
            LoadBehavior(UCrc32(key), UCrc32(key), Sim::Param());
        }
    }
    Attach(scenery);
    mManageTask = AddTask("Physics", 0.1f, 0.0f, Sim::TASK_FRAME_FIXED);
    CalcSimplificationWeight();
    if (mAttributes.EventSequencer().IsNotEmpty()) {
        Sim::Collision::AddListener(static_cast<Sim::Collision::IListener *>(this),
                                    GetInstanceHandle(), "Smackable");
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
    Sim::Collision::RemoveListener(static_cast<Sim::Collision::IListener *>(this));
}

bool Smackable::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fPosition = mLastCollisionPosition;
    return true;
}

template void UTL::Vector<ISimpleBody *, 16>::push_back(ISimpleBody *const &);
template BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs>::~BehaviorSpecsPtr();

unsigned int RenderConn::Pkt_Smackable_Open::Size() {
    return sizeof(*this);
}

unsigned int RenderConn::Pkt_Smackable_Open::Type() {
    return SType();
}

unsigned int RenderConn::Pkt_Smackable_Open::SType() {
    static UCrc32 hash = "Pkt_Smackable_Open";
    return hash.GetValue();
}

bool Smackable::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position,
                            float dT, IExplosion *explosion) {
    unsigned int targets = explosion->GetTargets();
    if ((targets & 1) == 0) {
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
        float deltaspeed = targetspeed - speed;
        UMath::Vector3 impactvel;
        UMath::Scale(normal, deltaspeed, impactvel);
        UMath::Vector3 force;
        UMath::Scale(impactvel, irb->GetMass() / dT, force);
        irb->ResolveForce(force, position);
    }
    EventSequencer::IEngine *sequencer = static_cast<ISimable *>(this)->GetEventSequencer();
    if (sequencer != nullptr) {
        sequencer->ProcessStimulus(0xab556d39, Sim::GetTime(), nullptr,
                                   EventSequencer::QUEUE_ALLOW);
        if (explosion->HasDamage()) {
            sequencer->ProcessStimulus(0xffcd8a63, Sim::GetTime(), nullptr,
                                       EventSequencer::QUEUE_ALLOW);
        }
    }
    if (!static_cast<ISimable *>(this)->GetCausality() && explosion->GetCausality()) {
        ICause *cause = ICause::FindInstance(explosion->GetCausality());
        if (cause != nullptr) {
            cause->OnCausedExplosion(explosion, static_cast<ISimable *>(this));
        }
    }
    return true;
}

void Smackable::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        if (static_cast<ISimable *>(this)->QueryInterface(&mCollisionBody)) {
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
        if (static_cast<ISimable *>(this)->QueryInterface(&mSimpleBody)) {
            mSimpleBody->ModifyFlags(0, 0x20b);
            ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
        }
    }
}

void Smackable::DoImpactStimulus(unsigned int systemid, float intensity) {
    float time;
    EventSequencer::IEngine *iev;
    EventSequencer::System *system;
    unsigned int level;
    time = Sim::GetTime();
    iev = static_cast<ISimable *>(this)->GetEventSequencer();
    if (iev != nullptr) {
        system = iev->FindSystem(systemid);
        if (system != nullptr) {
            intensity = UMath::Clamp(intensity, 0.0f, 1.0f);
            level = static_cast<unsigned int>(intensity * 6.0f);
            for (unsigned int i = 0; i < level + 1; i++) {
                system->ProcessStimulus(DamageZone::GetImpactStimulus(i).GetValue(), time,
                                        static_cast<EventSequencer::IContext *>(this),
                                        EventSequencer::QUEUE_ALLOW);
            }
        }
    }
}

void Smackable::OnImpact(float acceleration, float speed,
                         Sim::Collision::Info::CollisionType type, ISimable *iother) {
    float time;
    EventSequencer::IEngine *iev;
    EventSequencer::System *system;
    float intensity;
    time = Sim::GetTime();
    iev = static_cast<ISimable *>(this)->GetEventSequencer();
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
    EventSequencer::IEngine *iev = static_cast<ISimable *>(this)->GetEventSequencer();
    if (iev == nullptr) {
        return;
    }
    float speed = UMath::Length(cinfo.closingVel);
    if (speed < 0.0f) {
        return;
    }
    mLastCollisionPosition = UMath::Vector4Make(cinfo.position, 0.0f);
    if (cinfo.objA == static_cast<ISimable *>(this)->GetInstanceHandle()) {
        UMath::Vector3 normal = cinfo.normal;
        mLastImpactSpeed = cinfo.objAVel;
        float impact_acceleration = cinfo.impulseA;
        OnImpact(impact_acceleration, speed, cinfo.Type(), ISimable::FindInstance(cinfo.objB));
    } else if (cinfo.objB == static_cast<ISimable *>(this)->GetInstanceHandle()) {
        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);
        mLastImpactSpeed = cinfo.objBVel;
        float impact_acceleration = cinfo.impulseB;
        OnImpact(impact_acceleration, speed, cinfo.Type(), ISimable::FindInstance(cinfo.objA));
    }
}

bool Smackable::InView() const {
    if (mModel != nullptr) {
        return mModel->InView();
    }
    return false;
}

bool Smackable::IsRenderable() const { return mModel != nullptr; }

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
    const UMath::Vector3 &position = static_cast<ISimable *>(this)->GetPosition();
    WWorldPos &wpos = static_cast<ISimable *>(this)->GetWPos();
    wpos.FindClosestFace(position, true);
    if (!wpos.OnValidFace()) {
        goto fail;
    }
    {
        float height = wpos.HeightAtPoint(position);
        IRigidBody *irb = GetRigidBody();
        float radius = irb->GetRadius();
        if (height <= position.y + radius) {
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
    if (!GetWPos().OnValidFace()) {
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
                        static_cast<ISimable *>(this)->GetTransform(mat);
                        itrigger->PlaceTrigger(mat, true);
                        static_cast<ISimable *>(this)->Detach(mModel);
                        mModel = nullptr;
                    }
                }
            } else if (mModel != nullptr && !mPersistant) {
                mModel->ReleaseModel();
                mModel = nullptr;
            }
        }
        static_cast<ISimable *>(this)->Kill();
        return;
    }
    mLife = 0.125f;
}

bool Smackable::ProcessDropout(float dT) {
    if (mDropOutTimerMax > 0.0f && mDropTimer > 0.0f) {
        IRigidBody &rb = *GetRigidBody();
        mDropTimer -= dT;
        const float dropspeed = mAttributes.DROPOUT(1);
        rb.ModifyYPos(-dropspeed * dT);
        if (mDropTimer <= 0.0f) {
            static_cast<ISimable *>(this)->Kill();
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
            static_cast<ISimable *>(this)->Kill();
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
            static_cast<ISimable *>(this)->Kill();
        }
    }
    PhysicsObject::OnDetached(pOther);
}

void Smackable::CalcSimplificationWeight() {
    if (mCollisionBody == nullptr || mPersistant) {
        mSimplifyWeight = -1.0f;
    }
    float dist = Sim::DistanceToCamera(static_cast<ISimable *>(this)->GetPosition());
    float radius = GetRigidBody()->GetRadius();
    float baseweight = static_cast<float>(mAttributes.CAN_SIMPLIFY());
    if (!static_cast<IRenderable *>(this)->InView()) {
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
        UMath::Scale(gravity, irb->GetMass());
        irb->ResolveForce(gravity);
    }
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
    return Sim::Object::OnTask(htask, dT);
}

Behavior *RBSmackable::Construct(const BehaviorParams &parms) {
    const RBComplexParams rp = parms.fparams.Fetch< RBComplexParams >(UCrc32(0xa6b47fac));
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
    : Sim::Model(parent != nullptr ? static_cast<IModel *>(parent) : nullptr, geometry,
                 nodename, 6) //
    , IBody(this) //
    , ITriggerableModel(this) //
    , Attrib::Gen::smackable(attribs, 0, nullptr) //
{
    mTriggerAvoid = UMath::Vector4::kZero;
    mHeirarchy = const_cast<ModelHeirarchy *>(heirarchy);
    mHeirarchyNode = static_cast<unsigned short>(child_index);
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
    if (static_cast<unsigned short>(b) != is_avoidable) {
        if (b) {
            if (!CAMERA_AVOIDABLE()) {
                return;
            }
            CameraAI::AddAvoidable(static_cast<IBody *>(this));
            mFlags = mFlags | 1;
        } else {
            CameraAI::RemoveAvoidable(static_cast<IBody *>(this));
            mFlags = mFlags & 0xFFFE;
        }
    }
}

bool HeirarchyModel::OnRemoveOffScreen(float dT) {
    if (0.0f < mOffScreenTimer) {
        if (!static_cast<IModel *>(this)->InView()) {
            mOffScreenTimer = mOffScreenTimer - dT;
            if (mOffScreenTimer < 0.0f) {
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
                        reinterpret_cast<eModel *>(nodes[childindex].mModel);
                    if (emodel != nullptr) {
                        bHash32 meshname(emodel->GetNameHash());
                        HeirarchyModel *child = new HeirarchyModel(
                            meshname, bounds, rendernode, this, attribs, mHeirarchy,
                            childindex, true);
                        IModel *result = nullptr;
                        if (child != nullptr) {
                            result = static_cast<IModel *>(child);
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
        CameraAI::RemoveAvoidable(static_cast<IBody *>(this));
        mFlags = mFlags & 0xFFFE;
    }
}

void HeirarchyModel::GetTransform(UMath::Matrix4 &matrix) const {
    if (static_cast<const IModel *>(this)->GetSimable()) {
        static_cast<const IModel *>(this)->GetSimable()->GetTransform(matrix);
    } else if (mTrigger != nullptr) {
        mTrigger->GetObjectMatrix(matrix);
    } else {
        matrix = UMath::Matrix4::kIdentity;
    }
}

void HeirarchyModel::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (static_cast<const IModel *>(this)->GetSimable()) {
        velocity = static_cast<const IModel *>(this)->GetSimable()->GetRigidBody()->GetAngularVelocity();
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

void HeirarchyModel::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    SetTrigger(matrix, false);
    if (!enable) {
        DisableTrigger();
    }
}

void HeirarchyModel::OnEndSimulation() {
    if (mAvoidable != nullptr) {
        mAvoidable->SetRefrence(static_cast<IBody *>(this));
    }
}

void HeirarchyModel::OnBeginSimulation() {
    DisableTrigger();
    if (mAvoidable != nullptr) {
        mAvoidable->SetRefrence(static_cast<IModel *>(this)->GetSimable());
    }
    RenderConn::Pkt_Smackable_Open pkt(mRenderMesh,
                                       static_cast<IModel *>(this)->GetWorldID(),
                                       static_cast<IModel *>(this)->GetCollisionGeometry(),
                                       mHeirarchy,
                                       mHeirarchyNode);
    BeginDraw(UCrc32(0x804c146e), &pkt);
}

bool HeirarchyModel::OnDraw(Sim::Packet *service) {
    RenderConn::Pkt_Smackable_Service *pss =
        static_cast<RenderConn::Pkt_Smackable_Service *>(service);
    UpdateVisibility(pss->IsVisible(), pss->DistanceToView());
    pss->SetChildVisibility(mChildVisibility);
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
    static_cast<IPlaceableScenery *>(this)->Destroy();
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
        eModel model;
        model.Init(render_name.GetValue());
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
    ReleaseChildModels();
    StopEffects();
    EndDraw();
    EndSimulation();
    ReleaseSequencer();
    DisableTrigger();
}

bool PlaceableScenery::Place(const UMath::Matrix4 &transform, bool snap_to_ground) {
    static_cast<IPlaceableScenery *>(this)->Destroy();
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
    SmackableParams sp(mat, true, static_cast<IModel *>(this), false);
    ISimable *physics = UTL::COM::Factory< Sim::Param, ISimable, UCrc32 >::CreateInstance(
        UCrc32("Smackable"), sp);
    if (physics == nullptr) {
        static_cast<IPlaceableScenery *>(this)->Destroy();
        return false;
    }
    return true;
}

SmackableAvoidable::SmackableAvoidable(HeirarchyModel *model)
    : AIAvoidable(
          mModel != nullptr
              ? static_cast<UTL::COM::IUnknown *>(static_cast<IModel *>(mModel))
              : nullptr) //
    , mModel(model) //
{
    SetAvoidableObject(mModel != nullptr
                           ? static_cast<UTL::COM::IUnknown *>(static_cast<IBody *>(mModel))
                           : nullptr);
}

bool SmackableAvoidable::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    return mModel->OnUpdateAvoidable(pos, sweep);
}

inline bool Smackable::SimplifySort(const Smackable *lhs, const Smackable *rhs) {
    if (lhs->mSimplifyWeight > rhs->mSimplifyWeight) {
        return true;
    }
    if (lhs->mSimplifyWeight < rhs->mSimplifyWeight) {
        return false;
    }
    return lhs->GetInstanceHandle() < rhs->GetInstanceHandle();
}

IPlaceableScenery *IPlaceableScenery::CreateInstance(const char *name, unsigned int attributes) {
    return PlaceableScenery::Construct(name, attributes);
}

PlaceableScenery::~PlaceableScenery() {}
