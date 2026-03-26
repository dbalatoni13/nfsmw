#!/usr/bin/env python3
"""Append function bodies to Smackable.cpp"""
filepath = 'src/Speed/Indep/Src/Physics/Common/Smackable.cpp'
functions = r'''
static float GetDropTimer(const Attrib::Gen::smackable &attributes) {
    float result = 0.0f;
    if (0.0f < attributes.DROPOUT(0)) {
        if (0.0f < attributes.DROPOUT(1)) {
            result = attributes.DROPOUT(0);
        }
    }
    return result;
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
    const SmackableParams &sp = params.Fetch< SmackableParams >(UCrc32(0xa6b47fac));
    IModel *scenery = sp.fScenery;
    if (scenery == nullptr) {
        return nullptr;
    }
    Attrib::Gen::smackable attributes(scenery->GetAttributes());
    if (attributes.GetCollection() == nullptr) {
        return nullptr;
    }
    IPlaceableScenery *placeable = nullptr;
    bool is_persistant = scenery->QueryInterface(&placeable);
    bool simple_physics = attributes.SimplePhysics() || sp.fSimplePhysics;
    if (!simple_physics && !is_persistant && Smackable_RigidCount >= 0x14 && !TrySimplify()) {
        return nullptr;
    }
    scenery->GetWorldID();
    const CollisionGeometry::Bounds *geoms = scenery->GetCollisionGeometry();
    if (geoms == nullptr || !(attributes.MASS() > 0.0f)) {
        return nullptr;
    }
    UMath::Matrix4 matrix = sp.fMatrix;
    bool canSpawn;
    if (!simple_physics) {
        canSpawn = Sim::CanSpawnRigidBody(Vector4To3(matrix.v3), false);
    } else {
        canSpawn = Sim::CanSpawnSimpleRigidBody(Vector4To3(matrix.v3), false);
    }
    if (!canSpawn) {
        return nullptr;
    }
    if (!Manager::Exists()) {
        new Manager(Smackable_ManagementRate);
    }
    return new Smackable(matrix, attributes, geoms, sp.fVirginSpawn, scenery, simple_physics,
                         is_persistant);
}

Smackable::Smackable(const UMath::Matrix4 &matrix, const Attrib::Gen::smackable &attributes,
                     const CollisionGeometry::Bounds *geoms, bool virginspawn, IModel *scenery,
                     bool simple_physics, bool is_persistant)
    : PhysicsObject(attributes.GetBase(), SIMABLE_SMACKABLE, scenery->GetWorldID(), 10) //
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
        RBSimpleParams rbp(Vector4To3(matrix.v3), UMath::Vector3::kZero, UMath::Vector3::kZero,
                           matrix, radius, mass);
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"), rbp);
    } else {
        RBComplexParams rbparams(Vector4To3(matrix.v3), UMath::Vector3::kZero,
                                 UMath::Vector3::kZero, matrix, mass, moment, dimension, geoms,
                                 active, 0);
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RBSmackable"), rbparams);
    }
    for (unsigned int i = 0; i < attributes.Num_BEHAVIORS(); ++i) {
        const Attrib::StringKey &key = attributes.BEHAVIORS(i);
        if (key.IsNotEmpty()) {
            LoadBehavior(UCrc32(key), UCrc32(key), Sim::Param());
        }
    }
}

Smackable::~Smackable() {}

bool Smackable::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fPosition = mLastCollisionPosition;
    return true;
}

bool Smackable::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position,
                            float dT, IExplosion *explosion) {
    if (!(explosion->GetTargets() & 1)) {
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
        float time = Sim::GetTime();
        sequencer->ProcessStimulus(0xab556d39, time, nullptr, EventSequencer::QUEUE_ALLOW);
        if (explosion->GetCausality() != nullptr) {
            sequencer->ProcessStimulus(0xffcd8a63, time, nullptr, EventSequencer::QUEUE_ALLOW);
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
        static_cast< ISimable * >(this)->QueryInterface(&mCollisionBody);
        if (mCollisionBody != nullptr) {
            float detach = mAttributes.DETACH_FORCE();
            if (mVirgin && detach != 0.0f) {
                mCollisionBody->AttachedToWorld(UMath::Max(0.0f, detach), true);
            }
            const CollisionGeometry::Bounds *cog =
                mGeometry->fCollection->GetChild(mGeometry, UCrc32(0x28b0bb8d));
            if (cog == nullptr) {
                mCollisionBody->DistributeMass();
            } else {
                UMath::Vector3 cog_position;
                cog->GetPosition(cog_position);
                mCollisionBody->SetCenterOfGravity(cog_position);
            }
        }
        static_cast< ISimable * >(this)->QueryInterface(&mSimpleBody);
        if (mSimpleBody != nullptr) {
            mSimpleBody->ModifyFlags(0, 0x20b);
            ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
        }
    }
}

void Smackable::DoImpactStimulus(unsigned int systemid, float intensity) {
    float externalTime = Sim::GetTime();
    EventSequencer::IEngine *iev = static_cast< ISimable * >(this)->GetEventSequencer();
    if (iev != nullptr) {
        EventSequencer::System *system = iev->FindSystem(systemid);
        if (system != nullptr) {
            float clamped = UMath::Min(intensity, 1.0f);
            if (clamped < 0.0f) {
                clamped = 0.0f;
            }
            unsigned int level = static_cast< unsigned int >(clamped * 6.0f);
            for (unsigned int i = 0; i <= level; ++i) {
                unsigned int stimulus;
                DamageZone::GetImpactStimulus(stimulus);
                system->ProcessStimulus(stimulus, externalTime,
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
        if (type == Sim::Collision::Info::WORLD) {
            DoImpactStimulus(0x7ebe81c0, speed / MPH2MPS(100.0f));
        } else if (static_cast< int >(type) < 3) {
            if (type == Sim::Collision::Info::OBJECT && iother != nullptr) {
                float intensity = acceleration / MPH2MPS(100.0f);
                DoImpactStimulus(0xd59062c8, intensity);
                if (iother->IsPlayer()) {
                    DoImpactStimulus(0x2f698829, intensity);
                }
                if (iother->GetSimableType() == SIMABLE_VEHICLE) {
                    DoImpactStimulus(0x80b88c1d, intensity);
                }
            }
        } else if (type == Sim::Collision::Info::GROUND) {
            DoImpactStimulus(0x2bf74e61, speed * 0.1f);
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
        ISimable *other = ISimable::FindInstance(cinfo.objB);
        OnImpact(cinfo.impulseA, speed,
                 static_cast< Sim::Collision::Info::CollisionType >(cinfo.type), other);
    } else if (cinfo.objB == myHandle) {
        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);
        mLastImpactSpeed = cinfo.objBVel;
        ISimable *other = ISimable::FindInstance(cinfo.objA);
        OnImpact(cinfo.impulseB, speed,
                 static_cast< Sim::Collision::Info::CollisionType >(cinfo.type), other);
    }
}

bool Smackable::InView() const {
    if (mModel == nullptr) { return false; }
    return mModel->InView();
}

bool Smackable::IsRenderable() const { return mModel != nullptr; }

HMODEL Smackable::GetModelHandle() const {
    if (mModel == nullptr) { return nullptr; }
    return mModel->GetInstanceHandle();
}

const IModel *Smackable::GetModel() const { return mModel; }
IModel *Smackable::GetModel() { return mModel; }

float Smackable::DistanceToView() {
    if (mModel == nullptr) { return 0.0f; }
    return mModel->DistanceToView();
}

void Smackable::Kill() {
    if (mManageTask != nullptr) { RemoveTask(mManageTask); mManageTask = nullptr; }
    if (mModel != nullptr && !mPersistant) { mModel->ReleaseModel(); mModel = nullptr; }
    if (mCollisionBody != nullptr) {
        mCollisionBody->DisableModeling();
        mCollisionBody->DisableTriggering();
    }
    if (mSimpleBody != nullptr) { mSimpleBody->ModifyFlags(0x308, 0); }
    PhysicsObject::Kill();
}

bool Smackable::Dropout() {
    if (mCollisionBody == nullptr) { return false; }
    if (mDropOutTimerMax <= 0.0f) { return false; }
    if (mCollisionBody->IsAttachedToWorld()) { return false; }
    mCollisionBody->DisableModeling();
    mCollisionBody->DisableTriggering();
    mDropTimer = mDropOutTimerMax;
    return true;
}

bool Smackable::ValidateWorld() {
    const UMath::Vector3 &pos = static_cast< ISimable * >(this)->GetPosition();
    WWorldPos &wpos = static_cast< ISimable * >(this)->GetWPos();
    wpos.FindClosestFace(pos, true);
    if (!wpos.OnValidFace()) { return false; }
    float height = wpos.HeightAtPoint(pos);
    IRigidBody *irb = GetRigidBody();
    float radius = irb->GetRadius();
    return height <= pos.y + radius;
}

bool Smackable::ShouldDie() {
    if (mDroppingOut) { return false; }
    if (mCollisionBody == nullptr) { return false; }
    if (mCollisionBody->IsSleeping()) {
        if (!mCollisionBody->HasHadObjectCollision()) { return true; }
    }
    if (mCollisionBody == nullptr) { return false; }
    return !mCollisionBody->IsModeling();
}

bool Smackable::CanRetrigger() const {
    if (mCollisionBody == nullptr) { return false; }
    const WWorldPos &wpos = const_cast< Smackable * >(this)->GetWPos();
    if (!wpos.OnValidFace()) { return false; }
    return mCollisionBody->IsSleeping();
}

void Smackable::ProcessDeath(float dT) {
    if (!ShouldDie()) { mLife = 0.125f; return; }
    if (mLife > 0.0f) { mLife -= dT; return; }
    if (Dropout()) { return; }
    if (mModel != nullptr) {
        if (CanRetrigger()) {
            if (!mVirgin || mCollisionBody == nullptr || !mCollisionBody->IsAttachedToWorld()) {
                ITriggerableModel *itrigger = nullptr;
                if (mModel->QueryInterface(&itrigger)) {
                    UMath::Matrix4 mat;
                    static_cast< ISimable * >(this)->GetTransform(mat);
                    itrigger->PlaceTrigger(mat, true);
                    static_cast< ISimable * >(this)->Detach(mModel);
                    mModel = nullptr;
                }
            } else {
                ISceneryModel *iscenery = nullptr;
                if (mModel->QueryInterface(&iscenery)) { iscenery->RestoreScene(); mModel = nullptr; }
            }
        } else {
            if (mModel != nullptr && !mPersistant) { mModel->ReleaseModel(); mModel = nullptr; }
        }
    }
    static_cast< ISimable * >(this)->Kill();
}

bool Smackable::ProcessDropout(float dT) {
    if (mDropOutTimerMax <= 0.0f || mDropTimer <= 0.0f) { return false; }
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

void Smackable::ProcessOffWorld(float dT) {
    if (mAttributes.ALLOW_OFF_WORLD()) { return; }
    if (ValidateWorld()) { mOffWorldTimer = 0.0f; }
    else {
        mOffWorldTimer += dT;
        if (mOffWorldTimer >= 1.0f) {
            if (mModel != nullptr && !mPersistant) { mModel->ReleaseModel(); mModel = nullptr; }
            static_cast< ISimable * >(this)->Kill();
        }
    }
}

bool Smackable::OnTask(HSIMTASK htask, float dT) {
    if (htask == mManageTask) { Manage(dT); return true; }
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
    if (mCollisionBody == nullptr || mPersistant) { mSimplifyWeight = -1.0f; }
    const UMath::Vector3 &pos = static_cast< ISimable * >(this)->GetPosition();
    float dist = Sim::DistanceToCamera(pos);
    IRigidBody *irb = GetRigidBody();
    float radius = irb->GetRadius();
    float baseweight = static_cast< float >(mAttributes.CAN_SIMPLIFY());
    if (!static_cast< IRenderable * >(this)->IsRenderable()) { baseweight += baseweight; }
    mSimplifyWeight = (dist + baseweight) / radius;
}

void Smackable::Manage(float dT) {
    ProcessDeath(dT);
    ProcessOffWorld(dT);
    CalcSimplificationWeight();
}

void Smackable::OnTaskSimulate(float dT) {
    mAge += dT;
    if (mCollisionBody != nullptr && mAutoSimplify > 0.0f && mAutoSimplify < mAge) {
        Simplify();
    }
    mDroppingOut = ProcessDropout(dT);
    IRigidBody *irb = GetRigidBody();
    if (mSimpleBody != nullptr) {
        UMath::Vector3 gravity;
        gravity.x = 0.0f;
        gravity.y = mRBSpecs.GRAVITY();
        gravity.z = 0.0f;
        UMath::Scale(gravity, irb->GetMass(), gravity);
        irb->ResolveForce(gravity);
    }
}

EventSequencer::IEngine *Smackable::GetEventSequencer() {
    return PhysicsObject::GetEventSequencer();
}

Smackable::Manager::Manager(float rate) : Sim::Object(0) {
    AddTask("Smackable", rate, 0.0f, Sim::TASK_FRAME_FIXED);
}

Smackable::Manager::~Manager() {}

bool Smackable::Manager::OnTask(HSIMTASK htask, float dT) {
    UTL::Collections::Listable< Smackable, 160 >::Sort(Smackable::SimplifySort);
    return Sim::Object::OnTask(htask, dT);
}

Behavior *RBSmackable::Construct(const BehaviorParams &parms) {
    const RBComplexParams &rp = parms.fparams.Fetch< RBComplexParams >(UCrc32(0xa6b47fac));
    return new RBSmackable(parms, rp);
}

RBSmackable::RBSmackable(const BehaviorParams &parms, const RBComplexParams &rp)
    : RigidBody(parms, rp) //
    , mSpecs(parms.fowner, 0) //
{
    mFrame = 0;
    Smackable_RigidCount++;
}

RBSmackable::~RBSmackable() { Smackable_RigidCount--; }

bool RBSmackable::ShouldSleep() const {
    if (Dynamics::Articulation::IsJoined(this)) { return false; }
    return RigidBody::ShouldSleep();
}

void RBSmackable::OnTaskSimulate(float dT) {
    RigidBody::OnTaskSimulate(dT);
    mFrame++;
}

bool RBSmackable::CanCollideWith(const RigidBody &other) const {
    if (Dynamics::Articulation::IsJoined(this, &other)) { return false; }
    return RigidBody::CanCollideWith(other);
}

bool RBSmackable::CanCollideWithGround() const {
    if (IsAttachedToWorld()) { return false; }
    return RigidBody::CanCollideWithGround();
}

bool RBSmackable::CanCollideWithWorld() const {
    if (IsAttachedToWorld()) { return false; }
    if (!HasHadCollision()) {
        float velSquare = UMath::LengthSquare(GetLinearVelocity());
        if (velSquare < 4.0f) {
            if ((mFrame & 3) != 0) { return false; }
        } else if (velSquare < 225.0f) {
            if ((mFrame & 1) != 0) { return false; }
        }
    }
    return RigidBody::CanCollideWithWorld();
}

SmackableAvoidable::SmackableAvoidable(HeirarchyModel *model)
    : AIAvoidable(model != nullptr ? static_cast< UTL::COM::IUnknown * >(static_cast< ITriggerableModel * >(model)) : nullptr) //
    , mModel(model) //
{
}

bool SmackableAvoidable::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    return mModel->OnUpdateAvoidable(pos, sweep);
}

bool Smackable::SimplifySort(Smackable *lhs, Smackable *rhs) {
    return lhs->mSimplifyWeight < rhs->mSimplifyWeight;
}

bool Smackable::IsRequired() const { return false; }
void Smackable::HidePart(const UCrc32 &name) {}
void Smackable::ShowPart(const UCrc32 &name) {}
bool Smackable::IsPartVisible(const UCrc32 &name) const { return false; }
'''
with open(filepath, 'a') as f:
    f.write(functions)
print("Done")
