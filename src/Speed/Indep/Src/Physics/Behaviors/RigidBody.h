#ifndef __RIGIDBODY_H
#define __RIGIDBODY_H 1

#include "Speed/Indep/Libs/Support/Utility/UDefs.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/SAP.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

#define ENABLE_UGLY_BOND_BODGES 1
#define SHOW_COLLISIONS 0

class IDynamicsEntity : public UTL::COM::IUnknown, public Dynamics::IEntity {
  public:
    DECL_INTERFACE(IDynamicsEntity);
};

#define IS_FRONT_LEVER(_a) ((_a > 1 && _a < 6) || (_a == 8))

class RBGrid;

// total size: 0x140
class RigidBody : public Behavior,
                  public IRigidBody,
                  public ICollisionBody,
                  public IDynamicsEntity,
                  protected WCollisionMgr::ICollisionHandler,
                  public bTNode<RigidBody>,
                  public CollisionGeometry::IBoundable,
                  public Debugable {

  public:
    class Volatile;

    enum State {
        STATE_NOMODEL = 2,
        STATE_SLEEPING = 1,
        STATE_AWAKE = 0,
    };

    // total size: 0x18
    class Mesh : public bTNode<Mesh> {
      public:
        USE_FASTALLOC(RigidBody::Mesh);

        enum Flags {
            DISABLED = 1,
            FREEABLE = 2,
        };

        Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);
        ~Mesh();

        const Attrib::Collection *GetMaterial() const {
            return this->mMaterial;
        }

        unsigned int GetNumVertices() const {
            return this->mNumVertices;
        }

        const UMath::Vector4 *GetOrtho() const {
            return this->mVerts;
        }

        UCrc32 GetName() const {
            return this->mName;
        }

        void Enable(bool enable);

        bool IsEnabled() const {
            return (this->mFlags & Mesh::DISABLED) == 0;
        }

      private:
        UMath::Vector4 *mVerts;              // offset 0x8, size 0x4
        const unsigned short mNumVertices;   // offset 0xC, size 0x2
        unsigned short mFlags;               // offset 0xE, size 0x2
        const Attrib::Collection *mMaterial; // offset 0x10, size 0x4
        UCrc32 mName;                        // offset 0x14, size 0x4
    };

    // total size: 0x10
    class MeshList : public bTList<Mesh> {
      public:
        bool Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);

        MeshList() : mSize(0), mVertCount(0) {}

      private:
        unsigned int mSize;      // offset 0x8, size 0x4
        unsigned int mVertCount; // offset 0xC, size 0x4
    };

    // total size: 0x4C
    class Primitive : public bTNode<Primitive> {
      public:
        enum Flags {
            VSWORLD = 1,
            VSOBJECTS = 2,
            VSGROUND = 4,
            DISABLED = 8,
            ONESIDED = 16,
        };

        void Enable(bool enable);
        void Prepare(const Volatile &data);
        bool SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const;

        USE_FASTALLOC(RigidBody::Primitive);

        Primitive(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, Dynamics::Collision::Geometry::Shape shape,
                  const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name)
            : mOrientation(orient), mDimension(dim), mShape(shape), mOffset(offset), mName(name), mPrevPosition(UMath::Vector3::kZero), mFlags(flags),
              mMaterial(material.GetConstCollection()) {}

        const UMath::Vector3 &GetDimension() const {
            return mDimension;
        }

        Dynamics::Collision::Geometry::Shape GetShape() const {
            return mShape;
        }

        const UMath::Vector3 &GetOffset() const {
            return mOffset;
        }

        const UCrc32 &GetName() const {
            return mName;
        }

        const unsigned int &GetFlags() const {
            return mFlags;
        }

        const Attrib::Collection *GetMaterial() const {
            return mMaterial;
        }

      private:
        const UMath::Vector4 mOrientation;                 // offset 0x8, size 0x10
        ALIGNVEC const UMath::Vector3 mDimension;          // offset 0x18, size 0xC
        const Dynamics::Collision::Geometry::Shape mShape; // offset 0x24, size 0x4
        ALIGNVEC const UMath::Vector3 mOffset;             // offset 0x28, size 0xC
        const UCrc32 mName;                                // offset 0x34, size 0x4
        ALIGNVEC UMath::Vector3 mPrevPosition;             // offset 0x38, size 0xC
        unsigned int mFlags;                               // offset 0x44, size 0x4
        const Attrib::Collection *mMaterial;               // offset 0x48, size 0x4
    };

    // total size: 0x10
    class PrimList : public bTList<Primitive> {
      public:
        PrimList() : mRadius(0.0f), mSize(0) {}

        float GetRadius() const {
            return mRadius;
        }

        unsigned int Size() const {
            return mSize;
        }

        void CalcRadius();
        bool Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, Dynamics::Collision::Geometry::Shape shape,
                    const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name);

      private:
        float mRadius;      // offset 0x8, size 0x4
        unsigned int mSize; // offset 0xC, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);
    static void Update(const float dT);
    static void PushSP(void *workspace);
    static void PopSP();
    static IRigidBody *Get(unsigned int index);

    static unsigned int GetCount() {
        return mCount;
    }

    static void InitRigidBodySystem();
    static void ShutdownRigidBodySystem();
    static void DebugSystem();

    //  Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

    void Detach();

    bool WasCollidingWithObject() const {}

    bool IsAwake() const {
        return mData->state == 0;
    }

    UMath::Matrix4 &GetInvWorldTensor() const {
        return const_cast<UMath::Matrix4 &>(mInvWorldTensor);
    }

    void UpdateCollider();

    // ICollisionBody
    void Damp(float amount) override;
    void SetAnimating(bool animating) override;

    bool IsModeling() const override {
        return mData->state != 2;
    }

    void DisableModeling() override;
    void EnableModeling() override;
    bool IsTriggering() const override;
    void DisableTriggering() override;
    void EnableTriggering() override;

    bool IsSleeping() const override {
        return mData->state == 1;
    }

    bool DistributeMass() override;
    void EnableCollisionGeometries(UCrc32 name, bool enable) override;

    const UMath::Vector3 &GetWorldMomentScale() const override {
        return UMath::Vector4To3(mSpecs->WORLD_MOMENT_SCALE());
    }

    const UMath::Vector3 &GetGroundMomentScale() const override {
        return UMath::Vector4To3(mSpecs->GROUND_MOMENT_SCALE());
    }

    void SetCenterOfGravity(const UMath::Vector3 &cg) override {
        mCOG = cg;
    }

    const UMath::Vector3 &GetCenterOfGravity() const override {
        return mCOG;
    }

    bool HasHadCollision() const override {
        return mData->GetStatus(Volatile::HAS_HAD_OBJECT_COLLISION | Volatile::HAS_HAD_WORLD_COLLISION);
    }

    bool HasHadWorldCollision() const override {
        return mData->GetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
    }

    bool HasHadObjectCollision() const override {
        return mData->GetStatus(Volatile::HAS_HAD_OBJECT_COLLISION);
    }

    bool IsAttachedToWorld() const override {
        return mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD);
    }

    void AttachedToWorld(bool b, float detachforce) override;

    bool IsAnchored() const override {
        return mData->GetStatus(Volatile::IS_ANCHORED);
    }

    void SetAnchored(bool b) override {
        if (b) {
            mData->SetStatus(Volatile::IS_ANCHORED);
        } else {
            mData->RemoveStatus(Volatile::IS_ANCHORED);
        }
    }

    void SetInertiaTensor(const UMath::Vector3 &moment) override;

    const UMath::Vector3 &GetInertiaTensor() const override {
        return mData->inertiaTensor;
    }

    float GetOrientToGround() const override;

    bool IsInGroundContact() const override {
        return mData->leversInContact != 0;
    }

    unsigned int GetNumContactPoints() const override {
        return mData->leversInContact;
    }

    const UMath::Vector4 &GetGroundNormal() const override {
        return mGroundNormal;
    }

    void SetForce(const UMath::Vector3 &v) override {
        mData->force = v;
    }

    const UMath::Vector3 &GetForce() const override {
        return mData->force;
    }

    const UMath::Vector3 &GetTorque() const override {
        return mData->torque;
    }

    void SetTorque(const UMath::Vector3 &v) override {
        mData->torque = v;
    }

    float GetGravity() const override {
        return mSpecs->GRAVITY();
    }

    const UMath::Vector3 &GetRightVector() const override {
        return UMath::ExtractAxis(mData->bodyMatrix, 0);
    }

    const UMath::Vector3 &GetUpVector() const override {
        return UMath::ExtractAxis(mData->bodyMatrix, 1);
    }

    const UMath::Vector3 &GetForwardVector() const override {
        return UMath::ExtractAxis(mData->bodyMatrix, 2);
    }

    const UMath::Matrix4 &GetMatrix4() const override {
        return mData->bodyMatrix;
    }

    // IEntity
    const UMath::Matrix4 &GetRotation() const override {
        return mData->bodyMatrix;
    }

    void SetRotation(const UMath::Matrix4 &mat) override {
        UMath::Vector4 q;
        UMath::Matrix4ToQuaternion(mat, q);
        SetOrientation(q);
        mData->bodyMatrix = mat;
    }

    const UMath::Vector3 &GetPrincipalInertia() const override {
        return mData->inertiaTensor;
    }

    bool IsImmobile() const override;

    // IBoundable
    const CollisionGeometry::Bounds *GetGeometryNode() const override {
        return mGeoms;
    }

    bool AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                               const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundflags) override;
    bool AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                          CollisionGeometry::BoundFlags flags, bool persistant) override;

    // IRigidBody
    ISimable *GetOwner() const override {
        return Behavior::GetOwner();
    }

    virtual void OnDebugDraw();

    bool IsSimple() const override {
        return false;
    }

    SimableType GetSimableType() const override {
        return mSimableType;
    }

    int GetIndex() const override {
        return mData->index;
    }

    float GetRadius() const override {
        return mData->radius;
    }

    float GetMass() const override {
        return mData->mass;
    }

    float GetOOMass() const override {
        return mData->oom;
    }

    const UMath::Vector3 &GetPosition() const override {
        return mData->position;
    }

    const UMath::Vector3 &GetLinearVelocity() const override {
        return mData->linearVel;
    }

    const UMath::Vector3 &GetAngularVelocity() const override {
        return mData->angularVel;
    }

    float GetSpeed() const override {
        return UMath::Length(mData->linearVel);
    }

    float GetSpeedXZ() const override {
        return UMath::Lengthxz(mData->linearVel);
    }

    void GetRightVector(UMath::Vector3 &vec) const override {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 0);
    }

    void GetUpVector(UMath::Vector3 &vec) const override {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 1);
    }

    void GetForwardVector(UMath::Vector3 &vec) const override {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 2);
    }

    void GetMatrix4(UMath::Matrix4 &mat) const override {
        UMath::Copy(mData->bodyMatrix, mat);
    }

    unsigned int GetTriggerFlags() const override;

    const WCollider *GetWCollider() const override {
        return mWCollider;
    }

    void SetPosition(const UMath::Vector3 &pos) override;
    void SetLinearVelocity(const UMath::Vector3 &vel) override;
    void SetAngularVelocity(const UMath::Vector3 &vel) override;
    void SetRadius(float radius) override;
    void SetMass(float newMass) override;
    void SetOrientation(const UMath::Matrix4 &orientMat) override;
    void SetOrientation(const UMath::Vector4 &newOrientation) override;

    void ModifyXPos(float offset) override {
        mData->position.x += offset;
    }

    void ModifyYPos(float offset) override {
        mData->position.y += offset;
    }

    void ModifyZPos(float offset) override {
        mData->position.z += offset;
    }

    void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) override;
    void ResolveTorque(const UMath::Vector3 &torque) override;
    void ResolveForce(const UMath::Vector3 &force) override;
    void ResolveTorque(const UMath::Vector3 &force, const UMath::Vector3 &p) override;
    void ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p) override;

    const UMath::Vector4 &GetOrientation() const override {
        return mData->orientation;
    }

    void GetDimension(UMath::Vector3 &dim) const override {
        dim = mDimension;
    }

    UMath::Vector3 GetDimension() const override {
        return mDimension;
    }

    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;
    void Accelerate(const UMath::Vector3 &a, float dT) override;
    void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const override;
    void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const override;
    void GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const override;
    void Debug() override;

    RigidBody(const BehaviorParams &bp, const RBComplexParams &params);
    ~RigidBody() override;

    // total size: 0xB0
    class Volatile {
      public:
        enum { MaxInstances = 64 };

        enum {
            IS_ATTACHED_TO_WORLD = 1 << 1,
            HAS_HAD_WORLD_COLLISION = 1 << 2,
            HAS_HAD_OBJECT_COLLISION = 1 << 3,
            IS_ANCHORED = 1 << 7,
        };
        Volatile();

        void SetStatus(unsigned int uAdd) {
            status |= uAdd;
        }

        void RemoveStatus(unsigned int uRemove) {
            status &= ~uRemove;
        }

        bool GetStatus(unsigned int uFind) const {
            return (status & uFind) != 0;
        }

        void Validate() const {}

        // TODO Quaternion typedef
        UMath::Vector4 orientation;                       // offset 0x0, size 0x10
        ALIGNVEC UMath::Vector3 position;                 // offset 0x10, size 0xC
        unsigned short status;                            // offset 0x1C, size 0x2
        unsigned short statusPrev;                        // offset 0x1E, size 0x2
        ALIGNVEC UMath::Vector3 linearVel;                // offset 0x20, size 0xC
        float mass;                                       // offset 0x2C, size 0x4
        ALIGNVEC UMath::Vector3 angularVel;               // offset 0x30, size 0xC
        float oom;                                        // offset 0x3C, size 0x4
        ALIGNVEC Dynamics::Inertia::Tensor inertiaTensor; // offset 0x40, size 0xC
        PS2ALIGN16 float unused1;                         // offset 0x4C, size 0x4
        ALIGNVEC UMath::Vector3 force;                    // offset 0x50, size 0xC
        char leversInContact;                             // offset 0x5C, size 0x1
        unsigned char state;                              // offset 0x5D, size 0x1
        unsigned char index;                              // offset 0x5E, size 0x1
        char unused2;                                     // offset 0x5F, size 0x1
        ALIGNVEC UMath::Vector3 torque;                   // offset 0x60, size 0xC
        float radius;                                     // offset 0x6C, size 0x4
        PS2ALIGN16 UMath::Matrix4 bodyMatrix;             // offset 0x70, size 0x40
    };

  protected:
    virtual void OnBeginFrame(float dT);
    virtual void OnEndFrame(float dT);
    virtual void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment);
    virtual void ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment);

    virtual bool DoPenetration(const RigidBody &other) {
        return true;
    }

    virtual bool ShouldSleep() const;
    virtual float ResolveWorldCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                        const Attrib::Collection *objSurface, const SimSurface &worldSurface, const UVector3 &worldVel);
    virtual bool CanCollideWith(const RigidBody &other) const;
    virtual bool CanCollideWithWorld() const;
    virtual bool CanCollideWithGround() const;
    virtual bool CanCollideWithObjects() const;

    // ICollisionHandler
    bool OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) override;

    bool AddCollisionSphere(float radius, const UMath::Vector3 &offset, const struct SimSurface &material, unsigned int flags,
                            const struct UCrc32 &name);
    bool AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                         unsigned int flags, const UCrc32 &name);

  private:
    void DoWorldCollisions(const float dT);
    void DoBarrierCollision(float dT);
    void DoObbCollision(float dT);
    void DoInstanceCollision(float dT);
    void DoInstanceCollision3d(float dT);
    void DoInstanceCollision2d(float dT);
    void DoIntegration(const float dT);
    void DoDrag();
    static void OnObjectOverlap(RigidBody &objA, RigidBody &objB, float dT);
    void CreateGeometries();
    void UpdateGrid(int &overlapx, int &overlapz);
    bool ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                  const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                  const SimSurface &obbsurface);
    void ResolveGroundCollision(const class CollisionPacket *bcp, const int numContacts);
    static bool ResolveObjectCollision(RigidBody &objA, RigidBody &objB, const Primitive &colliderA, const Primitive &colliderB,
                                       const UMath::Vector3 &collisionNormal, const UMath::Vector3 &collisionPoint, float overlap, bool APenetratesB);
    static bool Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB);
    static unsigned int AssignSlot();

    ScratchPtr<Volatile> mData;                           // offset 0x88, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs; // offset 0x8C, size 0x14
    UMath::Matrix4 mInvWorldTensor;                       // offset 0xA0, size 0x40
    UMath::Vector4 mGroundNormal;                         // offset 0xE0, size 0x10
    ALIGNVEC UMath::Vector3 mDimension;                   // offset 0xF0, size 0xC
    WCollider *mWCollider;                                // offset 0xFC, size 0x4
    ALIGNVEC UMath::Vector3 mCOG;                         // offset 0x100, size 0xC
    const CollisionGeometry::Bounds *mGeoms;              // offset 0x10C, size 0x4
    RBGrid *mGrid;                                        // offset 0x110, size 0x4
    unsigned int mCollisionMask;                          // offset 0x114, size 0x4
    SimableType mSimableType;                             // offset 0x118, size 0x4
    float mDetachForce;                                   // offset 0x11C, size 0x4
    PrimList mPrimitives;                                 // offset 0x120, size 0x10
    MeshList mMeshes;                                     // offset 0x130, size 0x10

    static RigidBody *mMaps[Volatile::MaxInstances];
    static unsigned int mCount;
    static bool mOnSP;
};

extern bTList<RigidBody> TheRigidBodies;

// total size: 0x6C
class RBGrid : public SAP::Grid<RigidBody> {
  public:
    static RBGrid *Add(unsigned int index, RigidBody &owner, const UMath::Vector3 &position, float radius);
    static void Remove(RBGrid *grid);

  private:
    RBGrid(RigidBody &owner, const UMath::Vector3 &position, float radius) : SAP::Grid<RigidBody>(owner, position, radius) {}
    ~RBGrid() {}
};

#endif
