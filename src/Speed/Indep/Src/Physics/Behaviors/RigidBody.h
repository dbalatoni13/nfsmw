#ifndef PHYSICS_BEHAVIORS_RIGIDBODY_H
#define PHYSICS_BEHAVIORS_RIGIDBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/SAP.h"
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

struct RBComplexParams : public Sim::Param {
    // total size: 0x38
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
};

class RBGrid;

class RigidBody : public Behavior,
                  public IRigidBody,
                  public ICollisionBody,
                  public IDynamicsEntity,
                  protected WCollisionMgr::ICollisionHandler,
                  public bTNode<RigidBody>,
                  public CollisionGeometry::IBoundable,
                  public Debugable {

  public:
    class Volatile {
      public:
        Volatile();

        bool GetStatus(unsigned int uFind) {
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

    class Mesh : public bTNode<Mesh> {
      public:
        enum Flags {
            FREEABLE = 2,
            DISABLED = 1,
        };

        void *operator new(std::size_t size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, std::size_t size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);
        ~Mesh();
        void Enable(bool enable);

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

        void Enable(bool enable);
        void Prepare(const Volatile &data);
        bool SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const;

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
        PrimList() {
            // TODO
        }

        bool Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, Dynamics::Collision::Geometry::Shape shape,
                    const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name);

      private:
        // total size: 0x10
        float mRadius;      // offset 0x8, size 0x4
        unsigned int mSize; // offset 0xC, size 0x4
    };

    class MeshList : public bTList<Mesh> {
      public:
        MeshList() {
            // TODO
        }

        bool Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant);

      private:
        // total size: 0x10
        unsigned int mSize;      // offset 0x8, size 0x4
        unsigned int mVertCount; // offset 0xC, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);
    static bool Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB);
    static bool ResolveObjectCollision(RigidBody &objA, RigidBody &objB, const Primitive &colliderA, const Primitive &colliderB,
                                       const UMath::Vector3 &collisionNormal, const UMath::Vector3 &collisionPoint, float overlap, bool APenetratesB);
    static void OnObjectOverlap(RigidBody &objA, RigidBody &objB, float dT);

    RigidBody(const BehaviorParams &bp, const RBComplexParams &params);
    void InitRigidBodySystem();
    void ShutdownRigidBodySystem();
    void Detach();
    void CreateGeometries();

    bool ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, Sim::Collision::Info *collisionInfo,
                                  const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                  const SimSurface &obbsurface);
    void UpdateCollider();
    void DoWorldCollisions(const float dT);
    void DoBarrierCollision(float dT);
    void DoInstanceCollision(float dT);
    void DoInstanceCollision3d(float dT);
    void DoObbCollision(float dT);
    bool AddCollisionSphere(float radius, const UMath::Vector3 &offset, const struct SimSurface &material, unsigned int flags,
                            const struct UCrc32 &name);
    bool AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                         unsigned int flags, const UCrc32 &name);

    // Overrides
    virtual ~RigidBody();

    //  Behavior
    virtual void Reset();

    // ICollisionBody
    virtual bool DistributeMass();
    virtual void EnableCollisionGeometries(UCrc32 name, bool enable);
    virtual void AttachedToWorld(bool b, float detachforce);
    virtual void SetInertiaTensor(const UMath::Vector3 &moment);

    // IRigidBody
    virtual void SetPosition(const UMath::Vector3 &pos);
    virtual void SetLinearVelocity(const UMath::Vector3 &vel);
    virtual void SetAngularVelocity(const UMath::Vector3 &vel);
    virtual void SetRadius(float radius);
    virtual void SetMass(float newMass);
    virtual void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos);

    // IBoundable
    virtual bool AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                                       const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundflags);
    virtual bool AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                                  CollisionGeometry::BoundFlags flags, bool persistant);

    // ISimable
    virtual ISimable *GetOwner() const {
        return IRigidBody::GetOwner();
    }

  private:
    static RigidBody *mMaps[128];
    static std::size_t mCount;
    static bool mOnSP;

    // total size: 0x140
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
