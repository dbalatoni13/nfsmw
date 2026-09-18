#include "Speed/Indep/Src/Physics/Smackable.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Dynamics/Articulation.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/DamageZones.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x158
class RBSmackable : public RigidBody {
  public:
    static Behavior *Construct(const BehaviorParams &parms);

    RBSmackable(const BehaviorParams &parms, const RBComplexParams &rp);

    // Overrides: RigidBody
    bool ShouldSleep() const override;

    // Overrides: IUnknown
    ~RBSmackable() override;

    // Overrides: Behavior
    void OnTaskSimulate(float dT) override;

    // Overrides: RigidBody
    bool CanCollideWith(const RigidBody &other) const override;
    bool CanCollideWithGround() const override;
    bool CanCollideWithWorld() const override;

  private:
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs; // offset 0x140, size 0x14
    unsigned int mFrame;                                  // offset 0x154, size 0x4
};

unsigned int Smackable_RigidCount = 0;

static float GetDropTimer(const Attrib::Gen::smackable &attributes) {
    float dropout = attributes.DROPOUT(0);
    if (dropout <= 0.0f || attributes.DROPOUT(1) <= 0.0f)
        return 0.0f;
    return dropout;
}

bool Smackable::Simplify() {
    if (mCollisionBody == NULL) {
        return false;
    }
    if (mPersistant) {
        return false;
    }
    if (!Sim::CanSpawnSimpleRigidBody(GetPosition(), true)) {
        return false;
    }

    IRigidBody *irb = GetRigidBody();

    UMath::Vector3 position = irb->GetPosition();
    UMath::Vector3 velocity = irb->GetLinearVelocity();
    UMath::Vector3 angular = irb->GetAngularVelocity();
    float radius = irb->GetRadius();
    float mass = irb->GetMass();
    UMath::Matrix4 matrix = mCollisionBody->GetMatrix4();

    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"),
                 RBSimpleParams(position, velocity, angular, matrix, radius, mass));

    return true;
}

bool Smackable::TrySimplify() {
    for (SmackList::List::const_iterator iter = SmackList::GetList().begin(); iter != SmackList::GetList().end(); ++iter) {

        Smackable *smack = *iter;
        if (smack->Simplify()) {
            return true;
        }
    }
    return false;
}

ISimable *Smackable::Construct(Sim::Param params) {

    SmackableParams sp = params.Fetch<SmackableParams>(UCrc32(0xa6b47fac));

    if (sp.fScenery == NULL) {

        return NULL;
    }

    Attrib::Gen::smackable attributes(sp.fScenery->GetAttributes());

    if (!attributes.IsValid()) {
        return NULL;
    }

    bool persistant = false;

    IPlaceableScenery *iplaceable;
    if (sp.fScenery->QueryInterface(&iplaceable)) {
        persistant = true;
    }
    bool simple_physics = attributes.SimplePhysics() || sp.fSimplePhysics;

    if (!simple_physics && !persistant && Smackable_RigidCount > 19 && !TrySimplify()) {

        return NULL;
    }

    sp.fScenery->GetWorldID();

    const CollisionGeometry::Bounds *geoms = sp.fScenery->GetCollisionGeometry();

    if (geoms == NULL) {

        return NULL;
    }

    if (attributes.MASS() <= 0.000001f) {

        return NULL;
    }

    UMath::Matrix4 mat = sp.fMatrix;

    if (simple_physics) {

        if (!Sim::CanSpawnSimpleRigidBody(*(UMath::Vector3 *) &mat.v3, false)) {
            return NULL;
        }
    } else {

        if (!Sim::CanSpawnRigidBody(*(UMath::Vector3 *) &mat.v3, false)) {
            return NULL;
        }
    }

    if (Manager::Get() == NULL) {

        Manager *manager = new Manager(0.1f);
        if (manager == NULL) {
            return NULL;
        }
    }

    return new Smackable(mat, attributes, geoms, sp.fVirginSpawn, sp.fScenery, simple_physics, persistant);
}

Smackable::Smackable(const UMath::Matrix4 &matrix, const Attrib::Gen::smackable &attributes, const CollisionGeometry::Bounds *geoms, bool virginspawn,
                     IModel *scenery, bool simple_physics, bool is_persistant)
    : PhysicsObject(attributes.GetBase(), SIMABLE_SMACKABLE, scenery->GetWorldID(), 10), //
      IDisposable(this),                                                                 //
      IRenderable(this),                                                                 //
      IExplodeable(this),                                                                //
      EventSequencer::IContext(this),                                                    //
      mAttributes(attributes),                                                           //
      mSimplifyWeight(0.0f),                                                             //
      mAge(0.0f),                                                                        //
      mLife(0.125f),                                                                     //
      mDropTimer(0.0f),                                                                  //
      mDropOutTimerMax(GetDropTimer(attributes)),                                        //
      mOffWorldTimer(0.0f),                                                              //
      mAutoSimplify(attributes.AUTO_SIMPLIFY()),                                         //
      mVirgin(virginspawn),                                                              //
      mModel(scenery),                                                                   //
      mGeometry(geoms),                                                                  //
      mManageTask(NULL),                                                                 //
      mDroppingOut(false),                                                               //
      mPersistant(is_persistant),                                                        //
      mCollisionBody(NULL),                                                              //
      mSimpleBody(NULL),                                                                 //
      mLastImpactSpeed(UMath::Vector3::kZero),                                           //
      mRBSpecs(this, 0),                                                                 //
      mLastCollisionPosition(UMath::Vector4::kZero) {
    UMath::Vector3 dimension;
    geoms->GetHalfDimensions(dimension);

    dimension.x = UMath::Max(dimension.x, 0.025f);
    dimension.y = UMath::Max(dimension.y, 0.025f);
    dimension.z = UMath::Max(dimension.z, 0.025f);

    float radius = UMath::Length(dimension);
    float mass = mAttributes.MASS();

    Dynamics::Inertia::Box inertia(mass, dimension.x + dimension.x, dimension.y + dimension.y, dimension.z + dimension.z);
    inertia *= 2.0f;

    UMath::Vector3 moment;

    if (mAttributes.MOMENT(moment)) {
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

    // CERRADA al 100 % (3120 B). r36c la cerro con CINCO piezas --cuatro de ellas
    // `asm` de cero bytes-- y la r65 LAS HA RETIRADO TODAS: la funcion casa con
    // UNA SOLA palabra de fuente legitima.
    //
    // LO QUE FALTABA ERA `Behavior *rbbehavior = NULL;` EN VEZ DE SIN INICIALIZAR.
    // Lo dice el DWARF del original, y hay que leer las DOS cosas a la vez:
    //   * el bloque anonimo del `else` del original tiene EXACTAMENTE dos locales,
    //     `rbbehavior // r11` y `rbparams // r1+0x60`. NO tiene collision_mask.
    //   * `li r11,0` es la INICIALIZACION de rbbehavior, y cse reutiliza ese mismo
    //     cero para el ultimo argumento de RBComplexParams.
    // O sea: la substitucion de cse que r36c se paso tres piezas intentando cortar
    // NO era el error -- era lo que hace el original. Solo cogia el registro
    // equivocado porque el registro correcto (el cero de rbbehavior) no existia.
    // Con la inicializacion puesta, el reparto entero sale solo y nuestro DWARF
    // pasa a coincidir local a local con el del original: smack_class r1+0x58 SIN
    // el asm de memoria, rbbehavior r11, rbparams r1+0x60, cero locales de sobra.
    //
    // RETIRADO en la r65 (secciones ALLOC identicas, comprobado por seccion):
    //   * `unsigned int collision_mask = 0;` -- local inventada; el DWARF del
    //     original no la tiene y le robaba r11 a rbbehavior.
    //   * __asm__("" : : "m"(smack_class))    (pieza 2)
    //   * __asm__("" : "+r"(simple_physics))  (pieza 3)
    //   * __asm__("" : : "r"(rbbehavior))     (pieza 5)
    // Se queda la pieza 1 (`if (simple_physics)` pelado), que es fuente normal, y
    // `UCrc32 smack_class;` sin usar, que el original TAMBIEN tiene (r1+0x58).
    // Medido: quitar collision_mask sin inicializar rbbehavior deja la funcion a
    // TRES instrucciones (el `li r11,0` y el `addi r9,r1,0x28` cruzados con el
    // `lwz r0,0x11c(r31)`), y quitarle encima la pieza 5 se lleva 4 B.
    //
    // HISTORIA (r27/r36c), que sigue valiendo como negativo de la forma ANTIGUA
    // --la que no inicializaba rbbehavior--. Medido con volcados RTL (banco: una
    // mini-TU que solo incluye este .cpp, 6,5 s por prueba):
    //
    // 1-3: cse extiende su bloque desde el `if` hasta la rama `else` --por
    // -fcse-follow-jumps (TAKEN) o por -fcse-skip-blocks (AROUND); harian falta
    // LAS DOS para cortarlo-- y `canon_reg` sustituye el `0` del ultimo argumento
    // de RBComplexParams por el primer registro de la clase del 0:
    //   con `== true`  -> el `0` de `UCrc32 smack_class` (pseudo 751, REG_EQUIV
    //                     const 0, 3 refs, live 46, pri 652 -> r29), que ademas
    //                     empuja `active` de r29 a r28;
    //   con `if (x)`   -> el propio parametro (pseudo 88 pasa de 2 refs/128 insns
    //                     a 3/588, pri 156 -> 51, y global_alloc ya no le da reg).
    // El objetivo no sustituye ninguno: sus `li r0,0` (UCrc.h:22) y `li r11,0`
    // son dos pseudos distintos de un solo uso que reload rematerializa. La pieza 2
    // es la llave y SOLO vale la forma de ENTRADA de memoria: `"=m"`, `"+m"` y
    // `: : "m"(x) : "memory"` dan BINARIO IDENTICO a la base. Lo que corta la
    // extension de bloque no es clobberar memoria: es declarar UNA LECTURA de esa
    // ranura, que impide que el pseudo de `smack_class` siga valiendo `const 0` en
    // la tabla de cse.
    //
    // 5: la dio `regmap.py`. Con las cuatro primeras puestas dice que al original
    // le queda `rbbehavior` en r11 y a nosotros en NINGUN registro (variable
    // muerta: flow nos la borra). Esa plaza de menos corria el par de direcciones
    // de las dos UCrc32 de la tercera LoadBehavior de (r27,r28) --el objetivo-- a
    // (r28,r29), y con ello el objetivo emitia un `mr r27,r30` que a nosotros nos
    // faltaba: 3116 B contra 3120. Mantener `rbbehavior` vivo con un asm de cero
    // bytes reproduce el reparto entero y los 4 B.
    //
    // Vedado y NEGATIVO en el camino (todo medido en el mismo banco):
    //   `!= false` (98,015); `smack_class` movido detras del if/else (faltan 4);
    //   `smack_class.GetValue()` como mascara (identico); `if(x){..} if(!x){..}`
    //   (98,015, con el derrame del marco 0x158); `if(x==true){..} if(x!=true){..}`
    //   y `switch(simple_physics)` (BINARIO IDENTICO: el front-end los canonicaliza
    //   al mismo if/else); `if(x==true){..} if(x==false){..}` (95,05);
    //   `if (mPersistant)` invertido (7 diffs); las dos UCrc32 de EFFECTS en
    //   locales (139 diffs); `Sim::Param` en local (69 diffs); el `mPersistant`
    //   como ternario dentro de la llamada (91 diffs); y --r36c-- mantener vivo
    //   `active` en vez de `rbbehavior` al final del else (12 diffs: mueve `active`
    //   de r29 a r28 y solo acierta el primero del par).
    if (simple_physics) {
        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("SimpleRigidBody"),
                     RBSimpleParams(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero, UMath::Vector3::kZero, matrix, radius, mass));
    } else {
        Behavior *rbbehavior = NULL;
        RBComplexParams rbparams(UMath::Vector4To3(matrix.v3), UMath::Vector3::kZero, UMath::Vector3::kZero, matrix, mass, inertia, dimension, geoms,
                                 active, 0);

        if (mPersistant) {
            rbbehavior = LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RigidBody"), rbparams);
        } else {
            rbbehavior = LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), UCrc32("RBSmackable"), rbparams);
        }

        LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), UCrc32("EffectsSmackable"), Sim::Param());
    }

    for (unsigned int i = 0; i < mAttributes.Num_BEHAVIORS(); i++) {
        const Attrib::StringKey &key = mAttributes.BEHAVIORS(i);

        if (key.IsNotEmpty()) {
            LoadBehavior(UCrc32(key), UCrc32(key), Sim::Param());
        }
    }

    Attach(scenery);
    mManageTask = AddTask(UCrc32("Physics"), 0.1f, 0.0f, (Sim::TaskMode) 0);

    CalcSimplificationWeight();

    if (mAttributes.EventSequencer().IsNotEmpty()) {
        Sim::Collision::AddListener(this, GetInstanceHandle(), "Smackable");
    }
}

bool Smackable::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fPosition = mLastCollisionPosition;
    return true;
}

bool Smackable::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) {
    unsigned int targets = explosion->GetTargets();
    if ((targets & 1) == 0) {
        return false;
    }

    IRigidBody *irb = GetRigidBody();
    float factor = mAttributes.ExplosionEffect();

    if (factor <= 0.0f) {
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

    EventSequencer::IEngine *engine = GetEventSequencer();
    if (engine) {

        engine->ProcessStimulus(UCRC32_EXPLOSION, Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);
        if (explosion->HasDamage()) {
            engine->ProcessStimulus(UCRC32_EXPLOSION_DAMAGE, Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);
        }
    }

    if (GetCausality() == NULL && explosion->GetCausality()) {

        ICause *icause = ICause::FindInstance(explosion->GetCausality());

        if (icause) {

            icause->OnCausedExplosion(explosion, this);
        }
    }

    return true;
}

void Smackable::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);

    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {

        if (this->ISimable::QueryInterface(&mCollisionBody)) {

            float detach = mAttributes.DETACH_FORCE();
            if (mVirgin && detach != 0.0f) {

                float force = UMath::Max(detach, 0.0f);
                mCollisionBody->AttachedToWorld(true, force);
            }

            {
                const CollisionGeometry::Bounds *cog = mGeometry->GetChild(UCrc32(UCRC32_COG));
                if (cog) {

                    UMath::Vector3 cogpos;
                    cog->GetPosition(cogpos);
                    mCollisionBody->SetCenterOfGravity(cogpos);
                } else {

                    mCollisionBody->DistributeMass();
                }
            }
        }

        if (this->ISimable::QueryInterface(&mSimpleBody)) {

            mSimpleBody->ModifyFlags(0, 523);

            ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
        }
    }
}

Smackable::~Smackable() {
    DetachAll();

    if (mManageTask) {

        RemoveTask(mManageTask);
        mManageTask = NULL;
    }

    if (mModel) {

        Detach(mModel);
        mModel = NULL;
    }

    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY));

    Sim::Collision::RemoveListener(this);

}

void Smackable::DoImpactStimulus(unsigned int systemid, float intensity) {
    float time = Sim::GetTime();
    EventSequencer::IEngine *engine = GetEventSequencer();
    if (engine) {

        EventSequencer::System *system = engine->FindSystem(systemid);
        if (system) {

            unsigned int levels = (unsigned int) (UMath::Clamp(intensity, 0.0f, 1.0f) * 6.0f);

            for (unsigned int i = 0; i < levels + 1; i++) {

                system->ProcessStimulus(DamageZone::GetImpactStimulus(i).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
            }
        }
    }
}

void Smackable::OnImpact(float acceleration, float speed, Sim::Collision::Info::CollisionType type, ISimable *iother) {
    float time = Sim::GetTime();
    EventSequencer::IEngine *engine = GetEventSequencer();
    if (engine == NULL) {
        return;
    }

    switch (type) {

    case Sim::Collision::Info::OBJECT:
        if (iother) {

            float intensity = acceleration / MPH2MPS(100.0f);
            DoImpactStimulus(UCRC32_OBJECT_COLLISION, intensity);
            if (iother->IsPlayer()) {

                DoImpactStimulus(UCRC32_PLAYER_COLLISION, intensity);
            }
            if (iother->GetSimableType() == SIMABLE_VEHICLE) {

                DoImpactStimulus(UCRC32_VEHICLE_COLLISION, intensity);
            }
        }
        break;
    case Sim::Collision::Info::GROUND:
        DoImpactStimulus(UCRC32_GROUND_COLLISION, speed * 0.1f);
        break;
    case Sim::Collision::Info::WORLD:
        DoImpactStimulus(UCRC32_WORLD_COLLISION, speed / MPH2MPS(100.0f));
        break;

    }
}

void Smackable::OnCollision(const Sim::Collision::Info &cinfo) {
    EventSequencer::IEngine *engine = GetEventSequencer();
    if (engine == NULL) {
        return;
    }

    float speed = UMath::Length(cinfo.closingVel);

    if (speed < 1.0f) {
        return;
    }

    mLastCollisionPosition = UMath::Vector4Make(cinfo.position, 1.0f);

    if (cinfo.objA == GetInstanceHandle()) {

        UMath::Vector3 normal = cinfo.normal;
        mLastImpactSpeed = cinfo.objAVel;
        float impulse = cinfo.impulseA;

        OnImpact(impulse, speed, cinfo.Type(), ISimable::FindInstance(cinfo.objB));

    } else if (cinfo.objB == GetInstanceHandle()) {

        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);

        mLastImpactSpeed = cinfo.objBVel;
        float impulse = cinfo.impulseB;

        OnImpact(impulse, speed, cinfo.Type(), ISimable::FindInstance(cinfo.objA));
    }
}

bool Smackable::InView() const {
    if (mModel) {
        return mModel->InView();
    }
    return false;
}

bool Smackable::IsRenderable() const {
    if (mModel != NULL) {
        return true;
    }
    return false;
}

float Smackable::DistanceToView() const {
    if (mModel) {
        return mModel->DistanceToView();
    }
    return 0.0f;
}

void Smackable::Kill() {
    if (mManageTask) {
        RemoveTask(mManageTask);
        mManageTask = NULL;
    }

    if (mModel && !mPersistant) {
        mModel->ReleaseModel();
        mModel = NULL;
    }

    if (mCollisionBody) {
        mCollisionBody->DisableModeling();
        mCollisionBody->DisableTriggering();
    }

    if (mSimpleBody) {
        mSimpleBody->ModifyFlags(0x308, 0);
    }

    PhysicsObject::Kill();
}

bool Smackable::Dropout() {
    if (mCollisionBody == NULL ||

        mDropOutTimerMax <= 0.0f ||

        mCollisionBody->IsAttachedToWorld()) {
        return false;
    }

    mCollisionBody->DisableModeling();
    mCollisionBody->DisableTriggering();

    mDropTimer = mDropOutTimerMax;

    return true;
}

bool Smackable::ValidateWorld() {
    const UMath::Vector3 &pos = GetPosition();
    WWorldPos &wpos = GetWPos();

    wpos.FindClosestFace(pos, true);
    if (!wpos.OnValidFace() || wpos.HeightAtPoint(pos) > pos.y + GetRigidBody()->GetRadius()) {

        return false;
    }
    return true;
}

bool Smackable::ShouldDie() {
    if (mDroppingOut) {
        return false;
    }

    if (mCollisionBody && mCollisionBody->IsSleeping() && !mCollisionBody->HasHadObjectCollision()) {
        return true;
    }

    if (mCollisionBody && !mCollisionBody->IsModeling()) {
        return true;
    }

    return false;
}

bool Smackable::CanRetrigger() const {
    if (mCollisionBody == NULL) {
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

        } else if (!Dropout()) {

            if (mModel && CanRetrigger()) {

                if (mVirgin && mCollisionBody && mCollisionBody->IsAttachedToWorld()) {

                    ISceneryModel *iscenery;
                    if (mModel->QueryInterface(&iscenery)) {

                        iscenery->RestoreScene();

                        mModel = NULL;
                    }
                } else {

                    ITriggerableModel *itrigger;
                    if (mModel->QueryInterface(&itrigger)) {

                        UMath::Matrix4 matrix;
                        GetTransform(matrix);
                        itrigger->PlaceTrigger(matrix, true);

                        Detach(mModel);
                        mModel = NULL;
                    }
                }
            } else {

                if (mModel && !mPersistant) {

                    mModel->ReleaseModel();
                    mModel = NULL;
                }
            }
            Kill();
        }
    } else {
        mLife = 0.125f;
    }
}

bool Smackable::ProcessDropout(float dT) {
    if (mDropOutTimerMax > 0.0f && mDropTimer > 0.0f) {

        IRigidBody *irb = GetRigidBody();

        mDropTimer -= dT;

        float drop = mAttributes.DROPOUT(1);

        irb->ModifyYPos(-drop * dT);

        if (mDropTimer <= 0.0f) {

            Kill();
            mDropTimer = 0.0f;
        }
        return true;
    }
    return false;
}

void Smackable::ProcessOffWorld(float dT) {
    if (!mAttributes.ALLOW_OFF_WORLD()) {

        if (!ValidateWorld()) {

            mOffWorldTimer += dT;

            if (mOffWorldTimer >= 1.0f) {

                if (mModel && !mPersistant) {

                    mModel->ReleaseModel();
                    mModel = NULL;
                }

                Kill();
            }
        } else {
            mOffWorldTimer = 0.0f;
        }
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

        mModel = NULL;
        if (!IsDirty()) {
            Kill();
        }
    }
    PhysicsObject::OnDetached(pOther);
}

void Smackable::CalcSimplificationWeight() {
    if (mCollisionBody == NULL || mPersistant) {
        mSimplifyWeight = -1.0f;
    }

    float distance = Sim::DistanceToCamera(GetPosition());
    float radius = GetRigidBody()->GetRadius();
    float cost = (float) mAttributes.CAN_SIMPLIFY();

    if (!InView()) {
        cost += cost;
    }

    mSimplifyWeight = (distance + cost) / radius;
}

void Smackable::Manage(float dT) {
    ProcessDeath(dT);
    ProcessOffWorld(dT);

    CalcSimplificationWeight();
}

void Smackable::OnTaskSimulate(float dT) {

    mAge += dT;

    if (mCollisionBody && mAutoSimplify > 0.0f && mAge > mAutoSimplify) {

        Simplify();
    }

    mDroppingOut = ProcessDropout(dT);

    IRigidBody *irb = GetRigidBody();

    if (mSimpleBody) {

        UMath::Vector3 gravity = UMath::Vector3Make(0.0f, mRBSpecs->GRAVITY(), 0.0f);
        UMath::Scale(gravity, irb->GetMass(), gravity);
        irb->ResolveForce(gravity);
    }
}

IMPLEMENT_SINGLETON(Smackable::Manager);

Smackable::Manager::Manager(float rate) : Sim::Activity(0) {
    mSortTask = AddTask(UCrc32("WorldUpdate"), rate, 0.0f, (Sim::TaskMode) 0);
}

Smackable::Manager::~Manager() {
    RemoveTask(mSortTask);
}

bool Smackable::Manager::OnTask(HSIMTASK htask, float dT) {
    if (htask == mSortTask) {

        SmackList::Sort(Smackable::SimplifySort);

        if (Smackable_RigidCount > 10) {

            TrySimplify();
        }

        return true;
    }

    return false;
}

Behavior *RBSmackable::Construct(const BehaviorParams &parms) {
    const RBComplexParams rp = parms.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac));
    return new RBSmackable(parms, rp);
}

RBSmackable::RBSmackable(const BehaviorParams &parms, const RBComplexParams &rp) : RigidBody(parms, rp), mSpecs(this, 0), mFrame(0) {
    Smackable_RigidCount++;
}

bool RBSmackable::ShouldSleep() const {
    if (Dynamics::Articulation::IsJoined(this))
        return false;

    return RigidBody::ShouldSleep();
}

RBSmackable::~RBSmackable() {
    Smackable_RigidCount--;
}

void RBSmackable::OnTaskSimulate(float dT) {
    RigidBody::OnTaskSimulate(dT);
    mFrame++;
}

bool RBSmackable::CanCollideWith(const RigidBody &other) const {
    if (Dynamics::Articulation::IsJoined(this, &other))
        return false;
    return RigidBody::CanCollideWith(other);
}

bool RBSmackable::CanCollideWithGround() const {
    if (IsAttachedToWorld())
        return false;

    return RigidBody::CanCollideWithGround();
}

bool RBSmackable::CanCollideWithWorld() const {
    if (IsAttachedToWorld()) {
        return false;
    }
    if (!HasHadCollision()) {

        float velSquare = UMath::LengthSquare(GetLinearVelocity());
        if (velSquare < 4.0f) {

            if (mFrame & 3) {
                return false;
            }
        } else if (velSquare < 225.0f) {

            if (mFrame & 1) {
                return false;
            }
        }
    }
    return RigidBody::CanCollideWithWorld();
}

const Attrib::StringKey Smackable::CYLINDER("CYLINDER");
const Attrib::StringKey Smackable::TUBE("TUBE");
const Attrib::StringKey Smackable::CONE("CONE");
const Attrib::StringKey Smackable::SPHERE("SPHERE");

template <> UTL::Collections::Listable<Smackable, 160>::List UTL::Collections::Listable<Smackable, 160>::_mTable = UTL::Collections::Listable<Smackable, 160>::List();

BIND_PHYSICS_FACTORY(Smackable);
BIND_BEHAVIOR_FACTORY(RBSmackable);
