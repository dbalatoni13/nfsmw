#ifndef PHYSICS_BEHAVIORS_RIGIDBODY_H
#define PHYSICS_BEHAVIORS_RIGIDBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/SAP.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

// total size: 0x38
struct RBComplexParams : public Sim::Param {
    RBComplexParams(const RBComplexParams &_ctor_arg)
        : Sim::Param(_ctor_arg), finitPos(_ctor_arg.finitPos), finitVel(_ctor_arg.finitVel), finitAngVel(_ctor_arg.finitAngVel),
          finitMat(_ctor_arg.finitMat), finitMass(_ctor_arg.finitMass), finitMoment(_ctor_arg.finitMoment), fdimension(_ctor_arg.fdimension),
          factive(_ctor_arg.factive), fgeoms(_ctor_arg.fgeoms), fCollisionMask(_ctor_arg.fCollisionMask) {}

    RBComplexParams(const UMath::Vector3 &initPos, const UMath::Vector3 &initVel, const UMath::Vector3 &initAngVel, const UMath::Matrix4 &initMat,
                    float initMass, const UMath::Vector3 &initMoment, const UMath::Vector3 &dimension, const CollisionGeometry::Bounds *geoms,
                    bool active, unsigned int collision_mask)
        : Sim::Param(TypeName(), static_cast<RBComplexParams *>(nullptr)), finitPos(initPos), finitVel(initVel), finitAngVel(initAngVel),
          finitMat(initMat), finitMass(initMass), finitMoment(initMoment), fdimension(dimension), factive(active), fgeoms(geoms),
          fCollisionMask(collision_mask) {}

    static UCrc32 TypeName() {
        static UCrc32 value = UCrc32("RBComplexParams");
        return value;
    }

    const UMath::Vector3 &finitPos;          // offset 0x10, size 0x4
    const UMath::Vector3 &finitVel;          // offset 0x14, size 0x4
    const UMath::Vector3 &finitAngVel;       // offset 0x18, size 0x4
    const UMath::Matrix4 &finitMat;          // offset 0x1C, size 0x4
    float finitMass;                         // offset 0x20, size 0x4
    const UMath::Vector3 &finitMoment;       // offset 0x24, size 0x4
    const UMath::Vector3 &fdimension;        // offset 0x28, size 0x4
    bool factive;                            // offset 0x2C, size 0x1
    const CollisionGeometry::Bounds *fgeoms; // offset 0x30, size 0x4
    unsigned int fCollisionMask;             // offset 0x34, size 0x4
};

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
    class Mesh : public bTNode<Mesh> {
      public:
        enum Flags {
            FREEABLE = 2,
            DISABLED = 1,
        };

        Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);
        ~Mesh();
        void Enable(bool enable);

        void *operator new(std::size_t size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, std::size_t size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        UCrc32 GetName() const {
            return UCrc32(mName);
        }

      private:
        // total size: 0x18
        UMath::Vector4 *mVerts;              // offset 0x8, size 0x4
        const unsigned short mNumVertices;   // offset 0xC, size 0x2
        unsigned short mFlags;               // offset 0xE, size 0x2
        const Attrib::Collection *mMaterial; // offset 0x10, size 0x4
        UCrc32 mName;                        // offset 0x14, size 0x4
    };

    class Primitive : public bTNode<Primitive> {
      public:
        enum Flags {
            ONESIDED = 16,
            DISABLED = 8,
            VSGROUND = 4,
            VSOBJECTS = 2,
            VSWORLD = 1,
        };

        void Enable(bool enable);
        void Prepare(const Volatile &data);
        bool SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const;

        void *operator new(std::size_t size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, std::size_t size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        Primitive(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, Dynamics::Collision::Geometry::Shape shape,
                  const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name)
            : mOrientation(orient), mDimension(dim), mShape(shape), mOffset(offset), mName(UCrc32(name)), mPrevPosition(UMath::Vector3::kZero),
              mFlags(flags), mMaterial(material.GetConstCollection()) {}

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
        // total size: 0x4C
        const UMath::Vector4 mOrientation;                 // offset 0x8, size 0x10
        const UMath::Vector3 mDimension;                   // offset 0x18, size 0xC
        const Dynamics::Collision::Geometry::Shape mShape; // offset 0x24, size 0x4
        const UMath::Vector3 mOffset;                      // offset 0x28, size 0xC
        const UCrc32 mName;                                // offset 0x34, size 0x4
        UMath::Vector3 mPrevPosition;                      // offset 0x38, size 0xC
        unsigned int mFlags;                               // offset 0x44, size 0x4
        const Attrib::Collection *mMaterial;               // offset 0x48, size 0x4
    };

    enum State {
        STATE_NOMODEL = 2,
        STATE_SLEEPING = 1,
        STATE_AWAKE = 0,
    };

    class PrimList : public bTList<Primitive> {
      public:
        bool Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, Dynamics::Collision::Geometry::Shape shape,
                    const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name);

        PrimList() {
            // TODO
        }

        float GetRadius() const {
            return mRadius;
        }

        unsigned int Size() const {
            return mSize;
        }

      private:
        // total size: 0x10
        float mRadius;      // offset 0x8, size 0x4
        unsigned int mSize; // offset 0xC, size 0x4
    };

    class MeshList : public bTList<Mesh> {
      public:
        bool Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);

        MeshList() {
            // TODO
        }

      private:
        // total size: 0x10
        unsigned int mSize;      // offset 0x8, size 0x4
        unsigned int mVertCount; // offset 0xC, size 0x4
    };

    // Static functions
    static Behavior *Construct(const BehaviorParams &params);
    static bool Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB);
    static bool ResolveObjectCollision(RigidBody &objA, RigidBody &objB, const Primitive &colliderA, const Primitive &colliderB,
                                       const UMath::Vector3 &collisionNormal, const UMath::Vector3 &collisionPoint, float overlap, bool APenetratesB);
    static void OnObjectOverlap(RigidBody &objA, RigidBody &objB, float dT);
    static void PushSP(void *workspace);
    static void PopSP();
    static IRigidBody *Get(unsigned int index);
    static unsigned int AssignSlot();

    // Methods
    RigidBody(const BehaviorParams &bp, const RBComplexParams &params);
    void InitRigidBodySystem();
    void ShutdownRigidBodySystem();
    void Detach();
    void CreateGeometries();

    bool ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                  const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                  const SimSurface &obbsurface);
    void UpdateCollider();
    void DoWorldCollisions(const float dT);
    void DoBarrierCollision(float dT);
    void DoInstanceCollision(float dT);
    void DoInstanceCollision2d(float dT);
    void DoInstanceCollision3d(float dT);
    void DoObbCollision(float dT);
    bool AddCollisionSphere(float radius, const UMath::Vector3 &offset, const struct SimSurface &material, unsigned int flags,
                            const struct UCrc32 &name);
    bool AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                         unsigned int flags, const UCrc32 &name);
    void DoDrag();
    void DoIntegration(const float dT);
    void ResolveGroundCollision(const CollisionPacket *bcp, const int numContacts);
    void UpdateGrid(int &overlapx, int &overlapz);

    // Overrides
    override virtual ~RigidBody();

    //  Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);

    // ICollisionBody
    override virtual bool DistributeMass();
    override virtual void EnableCollisionGeometries(UCrc32 name, bool enable);
    override virtual void AttachedToWorld(bool b, float detachforce);
    override virtual void SetInertiaTensor(const UMath::Vector3 &moment);
    override virtual float GetOrientToGround() const;
    override virtual void DisableTriggering();
    override virtual bool IsTriggering() const;
    override virtual void EnableTriggering();
    override virtual void DisableModeling();
    override virtual void EnableModeling();
    override virtual void SetAnimating(bool animating);
    override virtual void Damp(float amount);

    // IRigidBody
    override virtual void SetPosition(const UMath::Vector3 &pos);
    override virtual void SetLinearVelocity(const UMath::Vector3 &vel);
    override virtual void SetAngularVelocity(const UMath::Vector3 &vel);
    override virtual void SetRadius(float radius);
    override virtual void SetMass(float newMass);
    override virtual void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos);
    override virtual void SetOrientation(const UMath::Matrix4 &orientMat);
    override virtual void GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const;
    override virtual void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const;
    override virtual void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const;
    override virtual void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque);
    override virtual void ResolveTorque(const UMath::Vector3 &torque);
    override virtual void ResolveForce(const UMath::Vector3 &force);
    override virtual void ResolveTorque(const UMath::Vector3 &force, const UMath::Vector3 &p);
    override virtual void ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p);
    override virtual void Debug();
    override virtual void Accelerate(const UMath::Vector3 &a, float dT);
    override virtual unsigned int GetTriggerFlags() const;

    // IBoundable
    override virtual bool AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset,
                                                const SimSurface &material, const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundflags);
    override virtual bool AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                                           CollisionGeometry::BoundFlags flags, bool persistant);

    // IEntity
    override virtual bool IsImmobile() const;

    // ICollisionHandler
    override bool OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata);

    // Virtual methods
    virtual void OnDebugDraw();
    virtual void OnBeginFrame(float dT);
    virtual void OnEndFrame(float dT);
    virtual void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment);
    virtual void ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment);

    // Inline virtuals
    // ICollisionBody
    override virtual bool IsModeling() const {
        return mData->state != 2;
    }

    override virtual bool IsSleeping() const {
        return mData->state == 1;
    }

    override virtual const UMath::Vector3 &GetWorldMomentScale() const {
        return UMath::Vector4To3(mSpecs->WORLD_MOMENT_SCALE());
    }

    override virtual const UMath::Vector3 &GetGroundMomentScale() const {
        return UMath::Vector4To3(mSpecs->GROUND_MOMENT_SCALE());
    }

    override virtual void SetCenterOfGravity(const UMath::Vector3 &cg) {
        mCOG = cg;
    }

    override virtual const UMath::Vector3 &GetCenterOfGravity() const {
        return mCOG;
    }

    override virtual bool HasHadCollision() const {
        return mData->GetStatus(Volatile::HAS_HAD_OBJECT_COLLISION | Volatile::HAS_HAD_WORLD_COLLISION);
    }

    override virtual bool HasHadWorldCollision() const {
        return mData->GetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
    }

    override virtual bool HasHadObjectCollision() const {
        return mData->GetStatus(Volatile::HAS_HAD_OBJECT_COLLISION);
    }

    override virtual bool IsAttachedToWorld() const {
        return mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD);
    }

    override virtual bool IsAnchored() const {
        return mData->GetStatus(Volatile::IS_ANCHORED);
    }

    override virtual void SetAnchored(bool b) {
        if (b) {
            mData->SetStatus(Volatile::IS_ANCHORED);
        } else {
            mData->RemoveStatus(Volatile::IS_ANCHORED);
        }
    }

    override virtual const UMath::Vector3 &GetInertiaTensor() const {
        return mData->inertiaTensor;
    }

    override virtual bool IsInGroundContact() const {
        return mData->leversInContact != 0;
    }

    override virtual unsigned int GetNumContactPoints() const {
        return mData->leversInContact;
    }

    override virtual const UMath::Vector4 &GetGroundNormal() const {
        return mGroundNormal;
    }

    override virtual void SetForce(const UMath::Vector3 &v) {
        mData->force = v;
    }

    override virtual const UMath::Vector3 &GetForce() const {
        return mData->force;
    }

    override virtual const UMath::Vector3 &GetTorque() const {
        return mData->torque;
    }

    override virtual void SetTorque(const UMath::Vector3 &v) {
        mData->torque = v;
    }

    override virtual float GetGravity() const {
        return mSpecs->GRAVITY();
    }

    override virtual const UMath::Vector3 &GetRightVector() const {
        return UMath::ExtractAxis(mData->bodyMatrix, 0);
    }

    override virtual const UMath::Vector3 &GetUpVector() const {
        return UMath::ExtractAxis(mData->bodyMatrix, 1);
    }

    override virtual const UMath::Vector3 &GetForwardVector() const {
        return UMath::ExtractAxis(mData->bodyMatrix, 2);
    }

    override virtual const UMath::Matrix4 &GetMatrix4() const {
        return mData->bodyMatrix;
    }

    // IEntity
    override virtual const UMath::Matrix4 &GetRotation() const {
        return mData->bodyMatrix;
    }

    override virtual void SetRotation(const UMath::Matrix4 &mat) {
        UMath::Vector4 q;
        // TODO Matrix4ToQuaternion
    }

    override virtual const UMath::Vector3 &GetPrincipalInertia() const {
        return mData->inertiaTensor;
    }

    // IBoundable
    override virtual const CollisionGeometry::Bounds *GetGeometryNode() const {
        return mGeoms;
    }

    // IRigidBody
    override virtual ISimable *GetOwner() const {
        return Behavior::GetOwner();
    }

    override virtual bool IsSimple() const {
        return false;
    }

    override virtual enum SimableType GetSimableType() const {
        return mSimableType;
    }

    override virtual int GetIndex() const {
        return mData->index;
    }

    override virtual float GetRadius() const {
        return mData->radius;
    }

    override virtual float GetMass() const {
        return mData->mass;
    }

    override virtual float GetOOMass() const {
        return mData->oom;
    }

    override virtual const UMath::Vector3 &GetPosition() const {
        return mData->position;
    }

    override virtual const UMath::Vector3 &GetLinearVelocity() const {
        return mData->linearVel;
    }

    override virtual const UMath::Vector3 &GetAngularVelocity() const {
        return mData->angularVel;
    }

    override virtual float GetSpeed() const {
        return UMath::Length(mData->linearVel);
    }

    override virtual float GetSpeedXZ() const {
        return UMath::Lengthxz(mData->linearVel);
    }

    override virtual void GetRightVector(UMath::Vector3 &vec) const {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 0);
    }

    override virtual void GetUpVector(UMath::Vector3 &vec) const {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 1);
    }

    override virtual void GetForwardVector(UMath::Vector3 &vec) const {
        vec = UMath::ExtractAxis(mData->bodyMatrix, 2);
    }

    override virtual void GetMatrix4(UMath::Matrix4 &mat) const {
        UMath::Copy(mData->bodyMatrix, mat);
    }

    override virtual const struct WCollider *GetWCollider() const {
        return mWCollider;
    }

    override virtual void ModifyXPos(float offset) {
        mData->position.x += offset;
    }

    override virtual void ModifyYPos(float offset) {
        mData->position.y += offset;
    }

    override virtual void ModifyZPos(float offset) {
        mData->position.z += offset;
    }

    override virtual const UMath::Vector4 &GetOrientation() const {
        return mData->orientation;
    }

    override virtual void GetDimension(UMath::Vector3 &dim) const {
        dim = mDimension;
    }

    override virtual UMath::Vector3 GetDimension() const {
        return mDimension;
    }

    // Rest of the own virtuals
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

    // Inline functions
    bool IsAwake() const {
        return mData->state == 0;
    }

    // declared here, so GetStatus doesn't inline in the overrides
    class Volatile {
      public:
        enum StatusFlags {
            IS_ATTACHED_TO_WORLD = 2,
            HAS_HAD_WORLD_COLLISION = 4,
            HAS_HAD_OBJECT_COLLISION = 8,
            IS_ANCHORED = 0x80,
        };
        // TODO create enums for state
        Volatile();

        bool GetStatus(unsigned int uFind) const {
            return status & uFind;
        }

        void SetStatus(unsigned int uAdd) {
            status |= uAdd;
        }

        void RemoveStatus(unsigned int uRemove) {
            status &= ~uRemove;
        }

        void Validate() {}

        // total size: 0xB0
        UMath::Vector4 orientation;              // offset 0x0, size 0x10
        UMath::Vector3 position;                 // offset 0x10, size 0xC
        unsigned short status;                   // offset 0x1C, size 0x2
        unsigned short statusPrev;               // offset 0x1E, size 0x2
        UMath::Vector3 linearVel;                // offset 0x20, size 0xC
        float mass;                              // offset 0x2C, size 0x4
        UMath::Vector3 angularVel;               // offset 0x30, size 0xC
        float oom;                               // offset 0x3C, size 0x4
        Dynamics::Inertia::Tensor inertiaTensor; // offset 0x40, size 0xC
        float unused1;                           // offset 0x4C, size 0x4
        UMath::Vector3 force;                    // offset 0x50, size 0xC
        char leversInContact;                    // offset 0x5C, size 0x1
        unsigned char state;                     // offset 0x5D, size 0x1
        unsigned char index;                     // offset 0x5E, size 0x1
        char unused2;                            // offset 0x5F, size 0x1
        UMath::Vector3 torque;                   // offset 0x60, size 0xC
        float radius;                            // offset 0x6C, size 0x4
        UMath::Matrix4 bodyMatrix;               // offset 0x70, size 0x40
    };

  private:
    static RigidBody *mMaps[128];
    static std::size_t mCount;
    static bool mOnSP;

    ScratchPtr<RigidBody::Volatile> mData;                // offset 0x88, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs; // offset 0x8C, size 0x14
    UMath::Matrix4 mInvWorldTensor;                       // offset 0xA0, size 0x40
    UMath::Vector4 mGroundNormal;                         // offset 0xE0, size 0x10
    UMath::Vector3 mDimension;                            // offset 0xF0, size 0xC
    WCollider *mWCollider;                                // offset 0xFC, size 0x4
    UMath::Vector3 mCOG;                                  // offset 0x100, size 0xC
    const CollisionGeometry::Bounds *mGeoms;              // offset 0x10C, size 0x4
    RBGrid *mGrid;                                        // offset 0x110, size 0x4
    unsigned int mCollisionMask;                          // offset 0x114, size 0x4
    SimableType mSimableType;                             // offset 0x118, size 0x4
    float mDetachForce;                                   // offset 0x11C, size 0x4
    PrimList mPrimitives;                                 // offset 0x120, size 0x10
    MeshList mMeshes;                                     // offset 0x130, size 0x10
};

class RBGrid : public SAP::Grid<RigidBody> {
    // total size: 0x6C
  public:
    static RBGrid *Add(unsigned int index, RigidBody &owner, const UMath::Vector3 &position, float radius);
    static void Remove(RBGrid *grid);

  private:
    RBGrid(RigidBody &owner, const UMath::Vector3 &position, float radius);
    ~RBGrid() {}
};

#endif
