#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Generated/Events/ECollision.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>
#include <types.h>

namespace Dynamics {
namespace Articulation {

void Resolve();

} // namespace Articulation
} // namespace Dynamics

static char RBGrid_Memory[6912]; // size: 0x1B00, address: 0x8048A1A8

template <>
void *ScratchPtr<RigidBody::Volatile>::mWorkSpace = nullptr;

template <>
RigidBody::Volatile *ScratchPtr<RigidBody::Volatile>::mPointer[64] = { nullptr };

template <>
RigidBody::Volatile ScratchPtr<RigidBody::Volatile>::mRAMBuffer[64];

template <>
SAP::Grid<RigidBody>::Node *SAP::Grid<RigidBody>::mRootX = nullptr;

template <>
SAP::Grid<RigidBody>::Node *SAP::Grid<RigidBody>::mRootZ = nullptr;

bTList<RigidBody> TheRigidBodies;

inline bool operator<(const CollisionPacket &a, const CollisionPacket &b) {
    return a.penetration > b.penetration;
}

template <> ScratchPtr<RigidBody::Volatile>::~ScratchPtr() {
    *mRef = nullptr;
}

IDynamicsEntity::~IDynamicsEntity() {}

struct SAPNodeAccess {
    SAPNodeAccess *mHead;
    SAPNodeAccess *mTail;
    float mPosition;
    float mSort;
    void *mAxis;
    SAPNodeAccess **mRoot;
};

struct SAPAxisAccess {
    SAPNodeAccess mMin;
    SAPNodeAccess mMax;
    void *mGrid;
};

struct RBGridAccess {
    SAPAxisAccess mX;
    SAPAxisAccess mZ;
    RigidBody *mOwner;
};

static inline void InitGridNode(SAPNodeAccess &node, void *axis, SAPNodeAccess **root, float position) {
    node.mHead = nullptr;
    node.mTail = nullptr;
    node.mPosition = position;
    node.mSort = position;
    node.mAxis = axis;
    node.mRoot = root;
}

static inline void InsertGridNode(SAPNodeAccess *&root, SAPNodeAccess *node) {
    SAPNodeAccess *tail = root;
    SAPNodeAccess *head = nullptr;

    while (tail && tail->mPosition < node->mPosition) {
        head = tail;
        tail = tail->mTail;
    }

    node->mHead = head;
    node->mTail = tail;
    if (head) {
        head->mTail = node;
    } else {
        root = node;
    }
    if (tail) {
        tail->mHead = node;
    }
}

template <typename T>
SAP::Grid<T>::~Grid() {
    struct NodeAccess {
        NodeAccess *mHead;
        NodeAccess *mTail;
        float mPosition;
        float mSort;
        void *mAxis;
        NodeAccess **mRoot;
    };

    struct AxisAccess {
        NodeAccess mMin;
        NodeAccess mMax;
        void *mGrid;
    };

    struct GridAccess {
        AxisAccess mX;
        AxisAccess mZ;
        void *mOwner;
    };

    GridAccess &grid = *reinterpret_cast<GridAccess *>(this);

    if (*grid.mZ.mMax.mRoot == &grid.mZ.mMax) {
        *grid.mZ.mMax.mRoot = grid.mZ.mMax.mTail;
    }
    if (grid.mZ.mMax.mHead) {
        grid.mZ.mMax.mHead->mTail = grid.mZ.mMax.mTail;
    }
    if (grid.mZ.mMax.mTail) {
        grid.mZ.mMax.mTail->mHead = grid.mZ.mMax.mHead;
    }
    grid.mZ.mMax.mTail = nullptr;
    grid.mZ.mMax.mHead = nullptr;

    if (*grid.mZ.mMin.mRoot == &grid.mZ.mMin) {
        *grid.mZ.mMin.mRoot = grid.mZ.mMin.mTail;
    }
    if (grid.mZ.mMin.mHead) {
        grid.mZ.mMin.mHead->mTail = grid.mZ.mMin.mTail;
    }
    if (grid.mZ.mMin.mTail) {
        grid.mZ.mMin.mTail->mHead = grid.mZ.mMin.mHead;
    }
    grid.mZ.mMin.mTail = nullptr;
    grid.mZ.mMin.mHead = nullptr;

    if (*grid.mX.mMax.mRoot == &grid.mX.mMax) {
        *grid.mX.mMax.mRoot = grid.mX.mMax.mTail;
    }
    if (grid.mX.mMax.mHead) {
        grid.mX.mMax.mHead->mTail = grid.mX.mMax.mTail;
    }
    if (grid.mX.mMax.mTail) {
        grid.mX.mMax.mTail->mHead = grid.mX.mMax.mHead;
    }
    grid.mX.mMax.mTail = nullptr;
    grid.mX.mMax.mHead = nullptr;

    if (*grid.mX.mMin.mRoot == &grid.mX.mMin) {
        *grid.mX.mMin.mRoot = grid.mX.mMin.mTail;
    }
    if (grid.mX.mMin.mHead) {
        grid.mX.mMin.mHead->mTail = grid.mX.mMin.mTail;
    }
    if (grid.mX.mMin.mTail) {
        grid.mX.mMin.mTail->mHead = grid.mX.mMin.mHead;
    }
    grid.mX.mMin.mTail = nullptr;
    grid.mX.mMin.mHead = nullptr;
}

// TODO clear the magic numbers in Volatile::state and status

Behavior *RigidBody::Construct(const BehaviorParams &params) {
    const RBComplexParams bp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RigidBody(params, bp);
}

RigidBody::Volatile::Volatile() {}

// UNSOLVED but functionally matching
RigidBody::Mesh::Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant)
    : mNumVertices(count), //
      mFlags(0) {
    mVerts = nullptr;
    mMaterial = material.GetConstCollection();
    mName = name;
    if (persistant) {
        mVerts = const_cast<UMath::Vector4 *>(verts);
    } else {
        mVerts = reinterpret_cast<UMath::Vector4 *>(gFastMem.Alloc(static_cast<unsigned short>(count) * sizeof(*mVerts), "RigidBody::Mesh::Verts"));
        bMemCpy(mVerts, verts, static_cast<unsigned short>(count) * sizeof(*mVerts));
        mFlags |= Mesh::FREEABLE;
    }
}

RigidBody::Mesh::~Mesh() {
    if (mFlags & Mesh::FREEABLE) {
        gFastMem.Free(mVerts, mNumVertices * sizeof(*mVerts), "RigidBody::Mesh::Verts");
    }
}

void RigidBody::Mesh::Enable(bool enable) {
    if (!enable) {
        mFlags |= Mesh::DISABLED;
    } else {
        mFlags &= ~Mesh::DISABLED;
    }
}

bool RigidBody::MeshList::Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant) {
    Mesh *mesh = new Mesh(material, verts, count, name, persistant);
    AddTail(mesh);
    mVertCount += count;
    mSize++;
    return true;
}

bool RigidBody::PrimList::Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material,
                                 Dynamics::Collision::Geometry::Shape shape, const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name) {
    Primitive *geom = new Primitive(dim, offset, material, shape, orient, flags, name);

    if (geom->GetShape() == Dynamics::Collision::Geometry::SPHERE) {
        AddHead(geom);
    } else {
        AddTail(geom);
    }

    float r = UMath::Length(dim) + UMath::Length(offset);
    mRadius = UMath::Max(mRadius, r);
    mSize++;
    return true;
}

void RigidBody::Primitive::Enable(bool enable) {
    if (!enable) {
        mFlags |= Primitive::DISABLED;
    } else {
        mFlags &= ~Primitive::DISABLED;
    }
}

// TODO why is the offset of bodyMatrix wrong on Xenon?
void RigidBody::Primitive::Prepare(const Volatile &data) {
    UMath::Rotate(GetOffset(), data.bodyMatrix, mPrevPosition);
    UMath::Add(mPrevPosition, data.position, mPrevPosition);
}

// UNSOLVED
bool RigidBody::Primitive::SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const {
    UMath::Vector3 position;
    UMath::Vector3 dP;

    UMath::Rotate(GetOffset(), data.bodyMatrix, position);
    UMath::Add(position, data.position, position);
    UMath::Sub(position, mPrevPosition, dP);

    if (this->mShape == Dynamics::Collision::Geometry::SPHERE) {
        obb.Set(UMath::Matrix4::kIdentity, position, GetDimension(), Dynamics::Collision::Geometry::SPHERE, dP);
    } else {
        UMath::Matrix4 mat;
        UMath::Vector4 rot;
        UMath::Mult(mOrientation, data.orientation, rot);
        UMath::QuaternionToMatrix4(rot, mat);
        obb.Set(mat, position, GetDimension(), mShape, dP);
    }
    return true;
}

void RigidBody::InitRigidBodySystem() {}

void RigidBody::ShutdownRigidBodySystem() {}

RBGrid *RBGrid::Add(unsigned int index, RigidBody &owner, const UMath::Vector3 &position, float radius) {
    if (index > 0x3F) {
        return nullptr;
    }

    RBGridAccess *grid = reinterpret_cast<RBGridAccess *>(RBGrid_Memory + index * sizeof(RBGrid));
    if (!grid) {
        return nullptr;
    }

    const float minX = position.x - radius;
    const float maxX = position.x + radius;
    const float maxZ = position.z + radius;
    const float minZ = position.z - radius;

    SAPNodeAccess *&rootX = reinterpret_cast<SAPNodeAccess *&>(SAP::Grid<RigidBody>::mRootX);
    SAPNodeAccess *&rootZ = reinterpret_cast<SAPNodeAccess *&>(SAP::Grid<RigidBody>::mRootZ);

    grid->mX.mMin.mHead = nullptr;
    grid->mX.mMin.mTail = nullptr;
    grid->mX.mMin.mPosition = minX;
    grid->mX.mMin.mSort = minX;
    grid->mX.mMin.mAxis = &grid->mX;
    grid->mX.mMin.mRoot = &rootX;

    SAPNodeAccess *tail = rootX;
    if (!tail) {
        rootX = &grid->mX.mMin;
    } else {
        bool before = minX < tail->mPosition;
        bool equal = tail->mPosition == minX;
        SAPNodeAccess *head = nullptr;
        SAPNodeAccess *node = rootX;
        while (!(before || equal) && (tail = node->mTail, head = node, tail)) {
            before = grid->mX.mMin.mPosition < tail->mPosition;
            equal = tail->mPosition == grid->mX.mMin.mPosition;
            node = tail;
        }
        if (!head) {
            SAPNodeAccess *oldHead = rootX;
            if (rootX == &grid->mX.mMin) {
                rootX = grid->mX.mMin.mTail;
            }
            if (grid->mX.mMin.mHead) {
                grid->mX.mMin.mHead->mTail = grid->mX.mMin.mTail;
            }
            if (grid->mX.mMin.mTail) {
                grid->mX.mMin.mTail->mHead = grid->mX.mMin.mHead;
            }
            grid->mX.mMin.mHead = oldHead;
            grid->mX.mMin.mTail = nullptr;
            if (oldHead) {
                oldHead->mTail = &grid->mX.mMin;
            }
        } else {
            SAPNodeAccess *newTail = head->mTail;
            if (rootX == &grid->mX.mMin) {
                rootX = grid->mX.mMin.mTail;
            }
            if (grid->mX.mMin.mHead) {
                grid->mX.mMin.mHead->mTail = grid->mX.mMin.mTail;
            }
            if (grid->mX.mMin.mTail) {
                grid->mX.mMin.mTail->mHead = grid->mX.mMin.mHead;
            }
            grid->mX.mMin.mHead = head;
            grid->mX.mMin.mTail = nullptr;
            head->mTail = &grid->mX.mMin;
            if (newTail) {
                grid->mX.mMin.mTail = newTail;
                newTail->mHead = &grid->mX.mMin;
                if (grid->mX.mMin.mTail == rootX) {
                    rootX = &grid->mX.mMin;
                }
            }
        }
    }

    grid->mX.mMax.mHead = nullptr;
    grid->mX.mMax.mTail = nullptr;
    grid->mX.mMax.mPosition = maxX;
    grid->mX.mMax.mSort = maxX;
    grid->mX.mMax.mAxis = &grid->mX;
    grid->mX.mMax.mRoot = &rootX;
    grid->mX.mGrid = grid;

    tail = rootX;
    if (!tail) {
        rootX = &grid->mX.mMax;
    } else {
        bool before = maxX < tail->mPosition;
        bool equal = tail->mPosition == maxX;
        SAPNodeAccess *head = nullptr;
        SAPNodeAccess *node = rootX;
        while (!(before || equal) && (tail = node->mTail, head = node, tail)) {
            before = grid->mX.mMax.mPosition < tail->mPosition;
            equal = tail->mPosition == grid->mX.mMax.mPosition;
            node = tail;
        }
        if (!head) {
            SAPNodeAccess *oldHead = rootX;
            if (rootX == &grid->mX.mMax) {
                rootX = grid->mX.mMax.mTail;
            }
            if (grid->mX.mMax.mHead) {
                grid->mX.mMax.mHead->mTail = grid->mX.mMax.mTail;
            }
            if (grid->mX.mMax.mTail) {
                grid->mX.mMax.mTail->mHead = grid->mX.mMax.mHead;
            }
            grid->mX.mMax.mHead = oldHead;
            grid->mX.mMax.mTail = nullptr;
            if (oldHead) {
                oldHead->mTail = &grid->mX.mMax;
            }
        } else {
            SAPNodeAccess *newTail = head->mTail;
            if (rootX == &grid->mX.mMax) {
                rootX = grid->mX.mMax.mTail;
            }
            if (grid->mX.mMax.mHead) {
                grid->mX.mMax.mHead->mTail = grid->mX.mMax.mTail;
            }
            if (grid->mX.mMax.mTail) {
                grid->mX.mMax.mTail->mHead = grid->mX.mMax.mHead;
            }
            grid->mX.mMax.mHead = head;
            grid->mX.mMax.mTail = nullptr;
            head->mTail = &grid->mX.mMax;
            if (newTail) {
                grid->mX.mMax.mTail = newTail;
                newTail->mHead = &grid->mX.mMax;
                if (grid->mX.mMax.mTail == rootX) {
                    rootX = &grid->mX.mMax;
                }
            }
        }
    }

    grid->mZ.mMin.mHead = nullptr;
    grid->mZ.mMin.mTail = nullptr;
    grid->mZ.mMin.mPosition = minZ;
    grid->mZ.mMin.mSort = minZ;
    grid->mZ.mMin.mAxis = &grid->mZ;
    grid->mZ.mMin.mRoot = &rootZ;

    tail = rootZ;
    if (!tail) {
        rootZ = &grid->mZ.mMin;
    } else {
        bool before = minZ < tail->mPosition;
        bool equal = tail->mPosition == minZ;
        SAPNodeAccess *head = nullptr;
        SAPNodeAccess *node = rootZ;
        while (!(before || equal) && (tail = node->mTail, head = node, tail)) {
            before = grid->mZ.mMin.mPosition < tail->mPosition;
            equal = tail->mPosition == grid->mZ.mMin.mPosition;
            node = tail;
        }
        if (!head) {
            SAPNodeAccess *oldHead = rootZ;
            if (rootZ == &grid->mZ.mMin) {
                rootZ = grid->mZ.mMin.mTail;
            }
            if (grid->mZ.mMin.mHead) {
                grid->mZ.mMin.mHead->mTail = grid->mZ.mMin.mTail;
            }
            if (grid->mZ.mMin.mTail) {
                grid->mZ.mMin.mTail->mHead = grid->mZ.mMin.mHead;
            }
            grid->mZ.mMin.mHead = oldHead;
            grid->mZ.mMin.mTail = nullptr;
            if (oldHead) {
                oldHead->mTail = &grid->mZ.mMin;
            }
        } else {
            SAPNodeAccess *newTail = head->mTail;
            if (rootZ == &grid->mZ.mMin) {
                rootZ = grid->mZ.mMin.mTail;
            }
            if (grid->mZ.mMin.mHead) {
                grid->mZ.mMin.mHead->mTail = grid->mZ.mMin.mTail;
            }
            if (grid->mZ.mMin.mTail) {
                grid->mZ.mMin.mTail->mHead = grid->mZ.mMin.mHead;
            }
            grid->mZ.mMin.mHead = head;
            grid->mZ.mMin.mTail = nullptr;
            head->mTail = &grid->mZ.mMin;
            if (newTail) {
                grid->mZ.mMin.mTail = newTail;
                newTail->mHead = &grid->mZ.mMin;
                if (grid->mZ.mMin.mTail == rootZ) {
                    rootZ = &grid->mZ.mMin;
                }
            }
        }
    }

    grid->mZ.mMax.mHead = nullptr;
    grid->mZ.mMax.mTail = nullptr;
    grid->mZ.mMax.mPosition = maxZ;
    grid->mZ.mMax.mSort = maxZ;
    grid->mZ.mMax.mAxis = &grid->mZ;
    grid->mZ.mMax.mRoot = &rootZ;
    grid->mZ.mGrid = grid;

    tail = rootZ;
    if (!tail) {
        rootZ = &grid->mZ.mMax;
    } else {
        bool before = maxZ < tail->mPosition;
        bool equal = tail->mPosition == maxZ;
        SAPNodeAccess *head = nullptr;
        SAPNodeAccess *node = rootZ;
        while (!(before || equal) && (tail = node->mTail, head = node, tail)) {
            before = grid->mZ.mMax.mPosition < tail->mPosition;
            equal = tail->mPosition == grid->mZ.mMax.mPosition;
            node = tail;
        }
        if (!head) {
            SAPNodeAccess *oldHead = rootZ;
            if (rootZ == &grid->mZ.mMax) {
                rootZ = grid->mZ.mMax.mTail;
            }
            if (grid->mZ.mMax.mHead) {
                grid->mZ.mMax.mHead->mTail = grid->mZ.mMax.mTail;
            }
            if (grid->mZ.mMax.mTail) {
                grid->mZ.mMax.mTail->mHead = grid->mZ.mMax.mHead;
            }
            grid->mZ.mMax.mHead = oldHead;
            grid->mZ.mMax.mTail = nullptr;
            if (oldHead) {
                oldHead->mTail = &grid->mZ.mMax;
            }
        } else {
            SAPNodeAccess *newTail = head->mTail;
            if (rootZ == &grid->mZ.mMax) {
                rootZ = grid->mZ.mMax.mTail;
            }
            if (grid->mZ.mMax.mHead) {
                grid->mZ.mMax.mHead->mTail = grid->mZ.mMax.mTail;
            }
            if (grid->mZ.mMax.mTail) {
                grid->mZ.mMax.mTail->mHead = grid->mZ.mMax.mHead;
            }
            grid->mZ.mMax.mHead = head;
            grid->mZ.mMax.mTail = nullptr;
            head->mTail = &grid->mZ.mMax;
            if (newTail) {
                grid->mZ.mMax.mTail = newTail;
                newTail->mHead = &grid->mZ.mMax;
                if (grid->mZ.mMax.mTail == rootZ) {
                    rootZ = &grid->mZ.mMax;
                }
            }
        }
    }

    grid->mOwner = &owner;
    return reinterpret_cast<RBGrid *>(grid);
}

void RBGrid::Remove(RBGrid *grid) {
    if (grid) {
        grid->~RBGrid();
    }
}

RigidBody::RigidBody(const BehaviorParams &bp, const RBComplexParams &params)
    : Behavior(bp, 0),                  //
      IRigidBody(bp.fowner),            //
      ICollisionBody(bp.fowner),        //
      IDynamicsEntity(bp.fowner),       //
      IBoundable(bp.fowner),            //
      mData(),                          //
      mSpecs(this, 0),                  //
      mWCollider(nullptr),              //
      mCOG(UMath::Vector3::kZero),      //
      mGeoms(params.fgeoms),            //
      mGrid(nullptr),                   //
      mCollisionMask(params.fCollisionMask), //
      mSimableType(GetOwner()->GetSimableType()), //
      mDetachForce(0.0f) {
    unsigned int world_id;

    UMath::Copy(UMath::Matrix4::kIdentity, mInvWorldTensor);
    mGroundNormal.x = 0.0f;
    mGroundNormal.y = 1.0f;
    mGroundNormal.z = 0.0f;
    mGroundNormal.w = 0.0f;
    {
        float dimension = params.fdimension.x;
        if (dimension < 0.1f) {
            dimension = 0.1f;
        }
        mDimension.x = dimension;
    }
    {
        float dimension = params.fdimension.y;
        if (dimension < 0.1f) {
            dimension = 0.1f;
        }
        mDimension.y = dimension;
    }
    {
        float dimension = params.fdimension.z;
        if (dimension < 0.1f) {
            dimension = 0.1f;
        }
        mDimension.z = dimension;
    }

    TheRigidBodies.AddTail(this);
    MakeDebugable(DBG_RIGIDBODY);

    mData->force = UMath::Vector3::kZero;
    mData->torque = UMath::Vector3::kZero;
    mData->index = AssignSlot();
    mData->mass = params.finitMass;
    mData->status = 0;
    mData->statusPrev = 0;
    mData->position = params.finitPos;
    mData->linearVel = params.factive ? params.finitVel : UMath::Vector3::kZero;
    mData->angularVel = params.factive ? params.finitAngVel : UMath::Vector3::kZero;
    UMath::Copy(UMath::Matrix4::kIdentity, mData->bodyMatrix);
    mData->inertiaTensor = params.finitMoment;
    mData->leversInContact = 0;
    mData->oom = 1.0f / mData->mass;
    mData->state = 0;
    mData->radius = UMath::Length(params.fdimension);
    SetOrientation(params.finitMat);
    mGrid = RBGrid::Add(mData->index, *this, params.finitPos, UMath::Length(params.fdimension));

    mCOG = UMath::Vector4To3(mSpecs->CG());
    if (!params.factive) {
        mData->state = 1;
    }

    if (UMath::LengthSquare(UMath::Vector4To3(mSpecs->DRAG())) > 0.0f) {
        mData->status |= 0x20;
    } else {
        mData->status &= 0xFFDF;
    }

    if (UMath::LengthSquare(UMath::Vector4To3(mSpecs->DRAG_ANGULAR())) > 0.0f) {
        mData->status |= 0x800;
    } else {
        mData->status &= 0xF7FF;
    }

    if (mSimableType == SIMABLE_VEHICLE) {
        mData->status |= 0x40;
    }

    world_id = GetOwner()->GetWorldID();
    mWCollider = WCollider::Create(world_id, WCollider::kColliderShape_Cylinder, 0x1C, mCollisionMask);
    CreateGeometries();
    mCount++;
    mMaps[mData->index] = this;
    mData->status |= 0x200;
}

RigidBody::~RigidBody() {
    if (mGrid) {
        RBGrid::Remove(mGrid);
        mGrid = nullptr;
    }
    mMaps[mData->index] = nullptr;
    mCount--;
    if (mWCollider) {
        WCollider::Destroy(mWCollider);
    }
    mWCollider = nullptr;
    mMeshes.DeleteAllElements();
    mPrimitives.DeleteAllElements();
    Dynamics::Articulation::Release(this);
    TheRigidBodies.Remove(this);
}

void RigidBody::Reset() {
    mWCollider->Clear();
    UpdateCollider();
}

void RigidBody::SetOrientation(const UMath::Matrix4 &orientMat) {
    Volatile &data = *mData;
    UMath::Matrix4ToQuaternion(orientMat, data.orientation);
    UMath::Copy(orientMat, data.bodyMatrix);
    UMath::Set(data.bodyMatrix, 3, UMath::Vector4::kIdentity);
    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, mInvWorldTensor);
}

void RigidBody::SetOrientation(const UMath::Vector4 &newOrientation) {
    Volatile &data = *mData;
    UMath::Copy(newOrientation, data.orientation);
    UMath::QuaternionToMatrix4(data.orientation, data.bodyMatrix);
    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, mInvWorldTensor);
}

bool RigidBody::DistributeMass() {
    Volatile &data = *mData;
    UMath::Clear(mCOG);

    float total_volume = 0.0f;
    for (Primitive *primitive = mPrimitives.GetHead(); primitive != mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetFlags() & Primitive::VSOBJECTS) {
            const UMath::Vector3 &dimension = primitive->GetDimension();
            float volume = dimension.x * dimension.y * dimension.z * 8.0f;
            UMath::ScaleAdd(primitive->GetOffset(), volume, mCOG, mCOG);
            total_volume += volume;
        }
    }
    if (total_volume > 0.0f) {
        UMath::Scale(mCOG, 1.0f / total_volume, mCOG);
        return true;
    }
    return false;
}

void RigidBody::EnableCollisionGeometries(UCrc32 name, bool enable) {
    if (name == UCrc32::kNull) {
        return;
    }
    for (Primitive *primitive = mPrimitives.GetHead(); primitive != mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetName() == name) {
            primitive->Enable(enable);
        }
    }
    for (Mesh *mesh = mMeshes.GetHead(); mesh != mMeshes.EndOfList(); mesh = mesh->GetNext()) {
        if (mesh->GetName() == name) {
            mesh->Enable(enable);
        }
    }
}

void RigidBody::SetPosition(const UMath::Vector3 &pos) {
    mData->position = pos;
}

void RigidBody::SetLinearVelocity(const UMath::Vector3 &vel) {
    mData->linearVel = vel;
}

void RigidBody::SetAngularVelocity(const UMath::Vector3 &vel) {
    mData->angularVel = vel;
}

void RigidBody::SetRadius(float radius) {
    mData->radius = radius;
}

void RigidBody::AttachedToWorld(bool b, float detachforce) {
    if (b) {
        mData->SetStatus(Volatile::IS_ATTACHED_TO_WORLD);
    } else {
        mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    }
    mDetachForce = detachforce;
}

void RigidBody::Detach() {
    if (mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
        EventSequencer::IEngine *engine = GetOwner()->GetEventSequencer();
        if (engine) {
            engine->ProcessStimulus(0xc91b90d9, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        }
    }
}

void RigidBody::SetInertiaTensor(const UMath::Vector3 &moment) {
    mData->inertiaTensor = moment;
}

void RigidBody::DoIntegration(const float dT) {
    Volatile &data = *mData;

    if (data.state == STATE_AWAKE) {
        UMath::Vector3 cg0;
        UMath::Rotate(mCOG, data.bodyMatrix, cg0);

        UMath::ScaleAdd(data.force, dT * data.oom, data.linearVel, data.linearVel);
        UMath::Scale(data.torque, dT, data.torque);
        UMath::ScaleAdd(data.linearVel, dT, data.position, data.position);
        UMath::Rotate(data.torque, mInvWorldTensor, data.torque);
        UMath::Add(data.angularVel, data.torque, data.angularVel);

        data.linearVel.x = UMath::Clamp(data.linearVel.x, -300.0f, 300.0f);
        data.linearVel.y = UMath::Clamp(data.linearVel.y, -300.0f, 300.0f);
        data.linearVel.z = UMath::Clamp(data.linearVel.z, -300.0f, 300.0f);
        data.angularVel.x = UMath::Clamp(data.angularVel.x, -30.0f, 30.0f);
        data.angularVel.y = UMath::Clamp(data.angularVel.y, -30.0f, 30.0f);
        data.angularVel.z = UMath::Clamp(data.angularVel.z, -30.0f, 30.0f);

        if (UMath::LengthSquare(data.angularVel) != 0.0f) {
            UMath::Vector3 angularStep;
            UMath::Vector4 deltaOrientation;

            UMath::Scale(data.angularVel, dT, angularStep);
            deltaOrientation.x =
                angularStep.x * data.orientation.w + (angularStep.y * data.orientation.z - angularStep.z * data.orientation.y);
            deltaOrientation.y =
                angularStep.y * data.orientation.w + (angularStep.z * data.orientation.x - angularStep.x * data.orientation.z);
            deltaOrientation.z =
                angularStep.z * data.orientation.w + (angularStep.x * data.orientation.y - angularStep.y * data.orientation.x);
            deltaOrientation.w =
                -(angularStep.x * data.orientation.x + angularStep.y * data.orientation.y + angularStep.z * data.orientation.z);

            UMath::ScaleAdd(deltaOrientation, 0.5f, data.orientation, data.orientation);

            float orientationLength =
                UMath::Sqrt(UMath::Dotxyz(data.orientation, data.orientation) + data.orientation.w * data.orientation.w);
            if (orientationLength != 0.0f) {
                UMath::Scale(data.orientation, 1.0f / orientationLength, data.orientation);
            }

            UMath::QuaternionToMatrix4(data.orientation, data.bodyMatrix);
            data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, mInvWorldTensor);

            UMath::Vector3 cg1;
            UMath::Vector3 dcg;
            UMath::Rotate(mCOG, data.bodyMatrix, cg1);
            UMath::Sub(cg0, cg1, dcg);
            UMath::Add(data.position, dcg, data.position);
        }
    } else {
        UMath::Clear(data.linearVel);
        UMath::Clear(data.angularVel);
    }
}

void RigidBody::SetMass(float newMass) {
    Volatile &data = *mData;
    float ratio = data.mass;
    if ((ratio != newMass) && (newMass > 0.0f)) {
        data.mass = newMass;
        data.oom = 1.0f / newMass;
        UMath::Scale(data.inertiaTensor, newMass / ratio, data.inertiaTensor);
    }
}

bool RigidBody::AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                                      const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundflags) {
    bool result = false;
    unsigned int flags = 0;
    if (boundflags & CollisionGeometry::kBounds_Disabled) {
        flags = 8;
    }
    if ((boundflags &
         (CollisionGeometry::kBounds_PrimVsGround | CollisionGeometry::kBounds_PrimVsObjects | CollisionGeometry::kBounds_PrimVsWorld))) {
        if (boundflags & CollisionGeometry::kBounds_PrimVsGround) {
            flags |= 4;
        }
        if (boundflags & CollisionGeometry::kBounds_PrimVsWorld) {
            flags |= 1;
        }
        if (boundflags & CollisionGeometry::kBounds_PrimVsObjects) {
            flags |= 2;
        }
        if (boundflags & CollisionGeometry::kBounds_Box) {
            if (AddCollisionBox(dim, offset, material, orient, flags, name)) {
                result = true;
            }
        } else if (boundflags & CollisionGeometry::kBounds_Sphere) {
            if (AddCollisionSphere(radius, offset, material, flags, name)) {
                result = true;
            }
        }
    }
    return result;
}

bool RigidBody::AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                                 CollisionGeometry::BoundFlags flags, bool persistant) {
    return mMeshes.Create(material, verts, count, name, persistant);
}

void RigidBody::CreateGeometries() {
    SimSurface material = SimSurface(mSpecs.BASE_MATERIAL());
    if (mGeoms && mGeoms->fCollection && mGeoms->fCollection->AddTo(this, mGeoms, material, true)) {
        return;
    }
    if (mSpecs.DEFAULT_COL_BOX() == Attrib::StringKey("SPHERE")) {
        AddCollisionSphere(UMath::Max(mDimension.x, UMath::Max(mDimension.y, mDimension.z)), UMath::Vector3::kZero, material, 3, UCrc32::kNull);
    } else {
        AddCollisionBox(mDimension, UMath::Vector3::kZero, material, UMath::Vector4::kIdentity, 7, UCrc32::kNull);
    }
}

void RigidBody::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    Volatile &data = *mData;
    data.state = 0;
    SetOrientation(orientMat);
    data.RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    UMath::Clear(data.linearVel);
    UMath::Clear(data.angularVel);
    data.position = initPos;
    data.leversInContact = 0;
    if (mSimableType == SIMABLE_VEHICLE) {
        data.SetStatus(0x40);
    } else {
        data.RemoveStatus(0x40);
    }
    mData->statusPrev = mData->status;
    mWCollider->Clear();
    UpdateCollider();
    mData->Validate();
}

float RigidBody::GetOrientToGround() const {
    const Volatile &data = *mData;

    return data.bodyMatrix.v1.x * mGroundNormal.x + data.bodyMatrix.v1.y * mGroundNormal.y + data.bodyMatrix.v1.z * mGroundNormal.z;
}

void RigidBody::GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const {
    const Volatile &data = *mData;
    UMath::Vector3 worldArm;
    UMath::Vector3 cg;

    UMath::Rotate(mCOG, data.bodyMatrix, cg);
    UMath::Add(cg, data.position, cg);
    UMath::Sub(position, cg, worldArm);
    UMath::Cross(data.angularVel, worldArm, velocity);
    UMath::Add(velocity, data.linearVel, velocity);
}

void RigidBody::OnEndFrame(float dT) {
    Volatile &data = *mData;
    if (data.state == 0) {
        UpdateCollider();
        data.RemoveStatus(Volatile::HAS_HAD_WORLD_COLLISION);
        if (!mWCollider->IsEmpty()) {
            DoWorldCollisions(dT);
        }
        DoDrag();
        data.Validate();
        if (ShouldSleep() != 0) {
            data.state = 1;
        }
    } else {
        Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, true, nullptr, true);
    }
    data.RemoveStatus(0x400);
}

void RigidBody::DisableTriggering() {
    mData->SetStatus(1);
}

bool RigidBody::IsTriggering() const {
    return !mData->GetStatus(1);
}

void RigidBody::EnableTriggering() {
    mData->RemoveStatus(1);
}

void RigidBody::DisableModeling() {
    mData->state = 2;
}

void RigidBody::EnableModeling() {
    if (mData->state == 2) {
        Volatile &data = *mData;
        data.Validate();
        data.state = 0;
        Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, false, nullptr, true);
    }
}

void RigidBody::OnBeginFrame(float dT) {
    Volatile &data = *mData;
    data.SetStatus(0x400);
    if (data.state != 2) {
        for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
            collider->Prepare(data);
        }
        DoIntegration(dT);
    }
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    if (data.state == 0) {
        data.statusPrev = data.status;
        if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
            data.force.y += mSpecs.GRAVITY() * data.mass;
        }
    }
    data.Validate();
}

struct Drag : public UVector3 {
    Drag(const UVector3 &vel, const UVector3 &C, const UVector3 &A, const float q) {
        float speed = vel.Magnitude();
        UMath::Scale(vel, A, *this);
        UMath::Scale(*this, C, *this);
        UMath::Scale(*this, -0.5f * speed * q, *this);
    }
};

void RigidBody::DoDrag() {
    Volatile &data = *mData;
    if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        if (data.GetStatus(0x800)) {
            UMath::Vector3 avel = data.angularVel;
            ConvertWorldToLocal(avel, false);
            UMath::Vector3 cross_section;
            cross_section.x = mDimension.x * 4.0f * UMath::Sqrt(mDimension.y * mDimension.y + mDimension.z * mDimension.z);
            cross_section.y = mDimension.y * 4.0f * UMath::Sqrt(mDimension.x * mDimension.x + mDimension.z * mDimension.z);
            cross_section.z = mDimension.z * 4.0f * UMath::Sqrt(mDimension.x * mDimension.x + mDimension.y * mDimension.y);

            Drag drag((UVector3(avel)), UVector3(UMath::Vector4To3(mSpecs.DRAG_ANGULAR())), UVector3(cross_section), 1.225f);
            ConvertLocalToWorld(drag, false);
            ResolveTorque(drag);
        }
        if (data.GetStatus(0x20)) {
            UMath::Vector3 vel = data.linearVel;
            UMath::Vector3 area;
            UMath::Scale(mDimension, mDimension, area);
            UMath::Scale(area, 4.0f, area);
            ConvertWorldToLocal(vel, false);

            Drag drag((UVector3(vel)), UVector3(UMath::Vector4To3(mSpecs.DRAG())), UVector3(area), 1.225f);
            ConvertLocalToWorld(drag, false);
            ResolveForce(drag);
        }
    }
}

void RigidBody::ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const {
    UMath::Rotate(val, mData->bodyMatrix, val);
    if (translate) {
        UMath::Add(val, mData->position, val);
    }
}

void RigidBody::ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const {
    UMath::Matrix4 InvBodyMatrix;

    if (translate) {
        UMath::Sub(val, mData->position, val);
    }
    UMath::Transpose(mData->bodyMatrix, InvBodyMatrix);
    UMath::Rotate(val, InvBodyMatrix, val);
}

void RigidBody::Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(force, data.force, data.force);
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &torque) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveForce(const UMath::Vector3 &force) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(force, data.force, data.force);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &force, const UMath::Vector3 &p) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();

        UMath::Vector3 cg;
        UMath::Vector3 torque;
        UMath::Vector3 r;
        UMath::Rotate(this->mCOG, data.bodyMatrix, cg);
        UMath::Add(cg, data.position, cg);
        UMath::Sub(p, cg, r);
        UMath::Cross(r, force, torque);
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();

        UMath::Vector3 cg;
        UMath::Vector3 torque;
        UMath::Vector3 r;
        UMath::Rotate(this->mCOG, data.bodyMatrix, cg);
        UMath::Add(cg, data.position, cg);
        UMath::Sub(p, cg, r);
        UMath::Cross(r, force, torque);
        UMath::Add(torque, data.torque, data.torque);
        UMath::Add(force, data.force, data.force);
    }
}

void RigidBody::Debug() {}

void RigidBody::SetAnimating(bool animating) {
    if (animating) {
        mData->SetStatus(0x100);
    } else {
        mData->RemoveStatus(0x100);
    }
}

// UNSOLVED, regswaps
void RigidBody::ResolveGroundCollision(const CollisionPacket *bcp, const int numContacts) {
    Volatile &data = *mData;
    bool first = true;
    Dynamics::Collision::Friction obj_friction;
    obj_friction.mUs = mSpecs.GROUND_FRICTION(0);
    obj_friction.mUk = UMath::Min(mSpecs.GROUND_FRICTION(1), mSpecs.GROUND_FRICTION(0));
    Dynamics::Collision::Plane plane;

    for (int loop = 0; loop < numContacts; ++loop, ++bcp) {
        Dynamics::Collision::Moment mo(this);
        SimSurface surface(bcp->surface);
        surface.DebugOverride();
        UMath::Add(bcp->lever, data.position, plane.point);
        plane.friction = obj_friction;
        plane.friction *= surface.GROUND_FRICTION();
        plane.normal = bcp->normal;

        UMath::Vector3 e;
        UMath::Dot(plane.normal, data.bodyMatrix, e);
        UMath::Scale(e, UMath::Vector4To3(mSpecs.GROUND_ELASTICITY()), e);
        mo.SetElasticity(UMath::Length(e));
        mo.SetInertiaScale(UMath::Vector4To3(mSpecs.GROUND_MOMENT_SCALE()));
        if (mo.React(plane, 16)) {
            if (first) {
                first = false;
                COLLISION_INFO collisionInfo;
                float force = mo.GetForce().Magnitude();
                collisionInfo.normal = plane.normal;
                collisionInfo.position = plane.point;
                collisionInfo.objA = GetOwner()->GetInstanceHandle();
                collisionInfo.objAVel = data.linearVel;
                collisionInfo.objAsurface = bcp->bodysurface;
                collisionInfo.objBsurface = surface.GetConstCollection();
                collisionInfo.type = COLLISION_INFO::GROUND;
                collisionInfo.sliding = mo.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
                collisionInfo.slidingVel = mo.GetSlidingVelocity();
                collisionInfo.closingVel = mo.GetClosingVelocity();
                collisionInfo.force = force;
                collisionInfo.impulseA = force * mo.GetOOMass();
                collisionInfo.impulseB = 0.0f;
                collisionInfo.armA = bcp->arm;
                new ECollision(collisionInfo);
            }
            data.angularVel = mo.GetAngularVelocity();
            data.linearVel = mo.GetLinearVelocity();
            data.Validate();
            break;
        }
    }
}

bool RigidBody::CanCollideWith(const RigidBody &other) const {
    if (mData->GetStatus(0x100) && other.mData->GetStatus(0x100)) {
        return false;
    }
    return true;
}

bool RigidBody::CanCollideWithGround() const {
    if (mSpecs.NO_GROUND_COLLISIONS() || !IsAwake()) {
        return false;
    }
    return true;
}

void RigidBody::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    myMoment.SetInertiaScale(UMath::Vector4To3(mSpecs.WORLD_MOMENT_SCALE()));
}

bool RigidBody::IsImmobile() const {
    if (mSpecs.IMMOBILE_OBJECT_COLLISIONS() || mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        return true;
    }
    return false;
}

void RigidBody::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    Volatile &data = *mData;
    if (!other.IsImmobile()) {
        if (mSpecs.IMMOBILE_OBJECT_COLLISIONS()) {
            myMoment.MakeImmobile(true, 0.0f);
        } else {
            if (data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
                myMoment.MakeImmobile(true, mDetachForce);
            }
        }
    }
    myMoment.SetInertiaScale(UMath::Vector4To3(mSpecs.OBJ_MOMENT_SCALE()));
    if (mSpecs.OBJ_MOMENT_SCALE().w > 0.0f) {
        myMoment.SetMass(myMoment.GetMass() * mSpecs.OBJ_MOMENT_SCALE().w);
    }
    if (data.GetStatus(Volatile::IS_ANCHORED)) {
        myMoment.SetFixedCG(true);
    }
}

bool RigidBody::Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB) {
    if (overlap <= FLOAT_EPSILON) {
        return false;
    }
    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    UMath::Vector3 deltaA;
    UMath::Vector3 deltaB;

    if (objBImmobile != objAImmobile) {
        float moveB;
        float moveA;
        if (objBImmobile) {
            moveB = 0.0f;
            moveA = 1.0f;
        } else {
            moveA = 0.0f;
            moveB = 1.0f;
        }
        UMath::Scale(normal, moveA * overlap, deltaA);
        UMath::Scale(normal, moveB * -overlap, deltaB);
    } else {
        if (objAImmobile)
            return false;
        float normal_speedA = -UMath::Dot(dataA.linearVel, normal);
        float normal_speedB = UMath::Dot(dataB.linearVel, normal);
        float closingspeed = normal_speedA + normal_speedB;
        if ((UMath::Abs(closingspeed) > FLOAT_EPSILON) && (objA.mSimableType == objB.mSimableType) && (objA.mSimableType == SIMABLE_SMACKABLE)) {
            float t = overlap / closingspeed;
            float xA = normal_speedA * t;
            float xB = normal_speedB * t;
            float moveA;
            float moveB;
            if (closingspeed > 0.0f) {
                moveA = xA / overlap;
                moveB = xB / overlap;
            } else {
                moveA = xB / overlap;
                moveB = xA / overlap;
            }
            if (moveA > 1.0f) {
                moveA = 1.0f;
                moveB = 0.0f;
            } else if (moveB > 1.0f) {
                moveB = 1.0f;
                moveA = 0.0f;
            }
            UMath::Scale(normal, moveA * overlap, deltaA);
            UMath::Scale(normal, moveB * -overlap, deltaB);
        } else {
            float moveB = dataA.mass / (dataA.mass + dataB.mass);
            float moveA = 1.0f - moveB;
            UMath::Scale(normal, moveA * overlap, deltaA);
            UMath::Scale(normal, moveB * -overlap, deltaB);
        }
    }
    UMath::Add(dataA.position, deltaA, dataA.position);
    UMath::Add(dataB.position, deltaB, dataB.position);
    if (APenetratesB) {
        UMath::Add(point, deltaA, point);
    } else {
        UMath::Add(point, deltaB, point);
    }
    return true;
}

bool RigidBody::ResolveObjectCollision(RigidBody &objA, RigidBody &objB, const Primitive &colliderA, const Primitive &colliderB,
                                       const UMath::Vector3 &collisionNormal, const UMath::Vector3 &collisionPoint, float overlap,
                                       bool APenetratesB) {
    bool result = false;
    const Attrib::Gen::rigidbodyspecs &specsA = *objA.mSpecs;
    const Attrib::Gen::rigidbodyspecs &specsB = *objB.mSpecs;
    bool shouldresolve = true;

    if (!objA.DoPenetration(objB)) {
        shouldresolve = false;
    }

    if (!objB.DoPenetration(objA)) {
        shouldresolve = false;
    }

    if (!shouldresolve) {
        return result;
    }

    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    Dynamics::Collision::Moment moA(&objA);
    Dynamics::Collision::Moment moB(&objB);
    dataA.SetStatus(8);
    dataB.SetStatus(8);

    const float Ks = UMath::Sqrt(specsA.OBJ_FRICTION(0) * specsB.OBJ_FRICTION(0));
    const float Kd = UMath::Sqrt(specsA.OBJ_FRICTION(1) * specsB.OBJ_FRICTION(1));

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(Ks, Kd), Ks);
    plane.normal = collisionNormal;
    plane.point = collisionPoint;

    UMath::Vector3 eA;
    UMath::Dot(plane.normal, dataA.bodyMatrix, eA);
    UMath::Scale(eA, UMath::Vector4To3(specsA.OBJ_ELASTICITY()), eA);
    moA.SetElasticity(UMath::Length(eA));

    UMath::Vector3 eB;
    UMath::Dot(plane.normal, dataB.bodyMatrix, eB);
    UMath::Scale(eB, UMath::Vector4To3(specsB.OBJ_ELASTICITY()), eB);
    moB.SetElasticity(UMath::Length(eB));

    objA.ModifyCollision(objB, plane, moA);
    objB.ModifyCollision(objA, plane, moB);

    if (moA.IsImmobile() && moB.IsImmobile()) {
        if (moA.GetBreakingForce() > 0.0f) {
            moA.MakeImmobile(false, 0.0f);
        }
        if (moB.GetBreakingForce() > 0.0f) {
            moB.MakeImmobile(false, 0.0f);
        }
    }

    if (!RigidBody::Separate(objA, moA.IsImmobile(), objB, moB.IsImmobile(), plane.normal, plane.point, overlap, APenetratesB)) {
        return false;
    }

    moA.SetPosition(dataA.position);
    moB.SetPosition(dataB.position);

    if (moA.React(moB, plane, 0x20)) {
        COLLISION_INFO collisionInfo;
        float force = moA.GetForce().Magnitude();

        collisionInfo.type = 1;
        collisionInfo.normal = collisionNormal;
        collisionInfo.position = collisionPoint;
        collisionInfo.sliding =
            moA.GetFrictionState() == Dynamics::Collision::Friction::Dynamic || moB.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
        collisionInfo.slidingVel = moA.GetSlidingVelocity();
        collisionInfo.closingVel = moA.GetClosingVelocity();
        collisionInfo.force = force;
        collisionInfo.objAVel = dataA.linearVel;
        collisionInfo.objBVel = dataB.linearVel;

        if (!moA.IsImmobile()) {
            dataA.angularVel = moA.GetAngularVelocity();
            dataA.linearVel = moA.GetLinearVelocity();
            dataA.state = 0;
            dataA.SetStatus(0x40);
            collisionInfo.impulseA = force * moA.GetOOMass();
        } else {
            collisionInfo.objAImmobile = true;
            collisionInfo.impulseA = force * moB.GetOOMass();
        }

        if (!moB.IsImmobile()) {
            dataB.angularVel = moB.GetAngularVelocity();
            dataB.linearVel = moB.GetLinearVelocity();
            dataB.state = 0;
            dataB.SetStatus(0x40);
            collisionInfo.impulseB = force * moB.GetOOMass();
        } else {
            collisionInfo.objBImmobile = true;
            collisionInfo.impulseB = force * moA.GetOOMass();
        }

        UMath::Matrix4 aInvMat;
        UMath::Matrix4 bInvMat;

        UMath::Transpose(dataA.bodyMatrix, aInvMat);
        UMath::Transpose(dataB.bodyMatrix, bInvMat);

        collisionInfo.objA = objA.GetOwner()->GetInstanceHandle();
        collisionInfo.objAsurface = colliderA.GetMaterial();
        UMath::Sub(collisionPoint, dataA.position, collisionInfo.armA);
        UMath::Rotate(collisionInfo.armA, aInvMat, collisionInfo.armA);

        collisionInfo.objB = objB.GetOwner()->GetInstanceHandle();
        collisionInfo.objBsurface = colliderB.GetMaterial();
        UMath::Sub(collisionPoint, dataB.position, collisionInfo.armB);
        UMath::Rotate(collisionInfo.armB, bInvMat, collisionInfo.armB);

        collisionInfo.objADetached = !collisionInfo.objAImmobile && dataA.GetStatus(Volatile::IS_ATTACHED_TO_WORLD);
        collisionInfo.objBDetached = !collisionInfo.objBImmobile && dataB.GetStatus(Volatile::IS_ATTACHED_TO_WORLD);

        new ECollision(collisionInfo);

        if (collisionInfo.objADetached) {
            objA.Detach();
        }
        if (collisionInfo.objBDetached) {
            objB.Detach();
        }
        result = true;
    }

    return result;
}

bool RigidBody::ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                         const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                         const SimSurface &obbsurface) {
    const float otherMass = 1000000.0f;
    Volatile &data = *mData;
    Dynamics::Collision::Moment moA(this);
    UMath::Vector3 inertia =
        Dynamics::Inertia::Box(1000000.0f, 2 * otherGeom->GetDimension().x, 2 * otherGeom->GetDimension().y, 2 * otherGeom->GetDimension().z);
    UMath::Vector3 cg = {};
    UMath::Vector3 angularVel = {};
    Dynamics::Collision::Moment moB(otherGeom->GetOrientation(), otherMass, inertia, cg, linearVel, angularVel, otherGeom->GetPosition());

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(mSpecs.OBJ_FRICTION(0), mSpecs.OBJ_FRICTION(1)), mSpecs.OBJ_FRICTION(0));
    plane.normal = cn;
    plane.point = UVector3(cp.x, cp.y, cp.z);
    plane.friction *= obbsurface.WORLD_FRICTION();

    UMath::Vector3 e;
    UMath::Dot(plane.normal, data.bodyMatrix, e);
    UMath::Scale(e, UMath::Vector4To3(mSpecs.WALL_ELASTICITY()), e);
    moA.SetElasticity(UMath::Length(e));

    ModifyCollision(obbsurface, plane, moA);

    moA.MakeImmobile(false, 0.0f);
    moB.MakeImmobile(true, 0.0f);

    if (moA.React(moB, plane, 0x20)) {
        float force = moA.GetForce().Magnitude();

        collisionInfo->objAVel = data.linearVel;
        collisionInfo->objBVel = linearVel;
        collisionInfo->objAsurface = rbsurface.GetConstCollection();
        collisionInfo->objA = GetOwner()->GetInstanceHandle();
        collisionInfo->type = 2;
        collisionInfo->objBsurface = obbsurface.GetConstCollection();
        collisionInfo->normal = cn;
        collisionInfo->position = cp;
        collisionInfo->sliding = moA.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
        collisionInfo->slidingVel = moA.GetSlidingVelocity();
        collisionInfo->closingVel = moA.GetClosingVelocity();
        collisionInfo->force = force;
        collisionInfo->impulseA = force * moA.GetOOMass();
        collisionInfo->impulseB = 0.0f;

        UMath::Sub(cp, data.position, collisionInfo->armA);
        ConvertWorldToLocal(collisionInfo->armA, 0);

        data.angularVel = moA.GetAngularVelocity();
        data.linearVel = moA.GetLinearVelocity();
        data.state = 0;
        data.Validate();
        return true;
    }
    return false;
}

float RigidBody::ResolveWorldCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                       const Attrib::Collection *objSurface, const SimSurface &worldSurface, const UVector3 &worldVel) {
    Volatile &data = *mData;
    const Attrib::Gen::rigidbodyspecs &specs = *mSpecs;
    Dynamics::Collision::Moment mo(this);

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(specs.WALL_FRICTION(1), specs.WALL_FRICTION(0)), specs.WALL_FRICTION(0));
    plane.normal = cn;
    plane.point = cp;
    plane.friction *= worldSurface.WORLD_FRICTION();

    UMath::Vector3 e;
    UMath::Dot(plane.normal, data.bodyMatrix, e);
    UMath::Scale(e, UMath::Vector4To3(specs.WALL_ELASTICITY()), e);
    mo.SetElasticity(UMath::Length(e));
    ModifyCollision(worldSurface, plane, mo);
    mo.SetLinearVelocity(mo.GetLinearVelocity() - worldVel);
    if (mo.React(plane, 0x10)) {
        float force = mo.GetForce().Magnitude();
        if (collisionInfo) {
            collisionInfo->normal = cn;
            collisionInfo->position = cp;
            collisionInfo->objA = GetOwner()->GetInstanceHandle();
            collisionInfo->objAVel = data.linearVel;
            collisionInfo->objAsurface = objSurface;
            collisionInfo->type = 2;
            collisionInfo->objBsurface = worldSurface.GetConstCollection();
            collisionInfo->sliding = mo.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
            collisionInfo->slidingVel = mo.GetSlidingVelocity();
            collisionInfo->closingVel = mo.GetClosingVelocity();
            collisionInfo->force = force;
            collisionInfo->impulseA = force * mo.GetOOMass();
            collisionInfo->impulseB = 0.0f;
            UMath::Sub(cp, data.position, collisionInfo->armA);
            ConvertWorldToLocal(collisionInfo->armA, 0);
        }
        data.angularVel = mo.GetAngularVelocity();
        data.linearVel = mo.GetLinearVelocity() + worldVel;
        data.Validate();
        return force;
    }
    return 0.0f;
}

void RigidBody::OnObjectOverlap(RigidBody &objA, RigidBody &objB, float dT) {
    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    float comp = objA.mPrimitives.GetRadius() + objB.mPrimitives.GetRadius();
    comp = UMath::Max(1.0f, comp * comp);

    if (UMath::DistanceSquare(dataA.position, dataB.position) > comp) {
        return;
    }
    if (!objA.CanCollideWith(objB) || !objB.CanCollideWith(objA)) {
        return;
    }
    HSIMABLE hA = objA.GetOwner()->GetInstanceHandle();
    HSIMABLE hB = objB.GetOwner()->GetInstanceHandle();
    Dynamics::Collision::Geometry geomA;
    Dynamics::Collision::Geometry geomB;
    for (Primitive *iter1 = objA.mPrimitives.GetHead(); iter1 != objA.mPrimitives.EndOfList(); iter1 = iter1->GetNext()) {
        Primitive &colliderA = *iter1;
        if (!(colliderA.GetFlags() & 2) || (colliderA.GetFlags() & 8)) {
            continue;
        }
        if (!colliderA.SetCollision(dataA, geomA)) {
            continue;
        }
        for (Primitive *iter2 = objB.mPrimitives.GetHead(); iter2 != objB.mPrimitives.EndOfList(); iter2 = iter2->GetNext()) {
            Primitive &colliderB = *iter2;
            if (!(colliderB.GetFlags() & 2) || (colliderB.GetFlags() & 8)) {
                continue;
            }
            if (colliderB.SetCollision(dataB, geomB) && Dynamics::Collision::Geometry::FindIntersection(&geomA, &geomB, &geomA)) {
                const UMath::Vector3 &collisionNormal = geomA.GetCollisionNormal();
                const UMath::Vector3 &collisionPoint = geomA.GetCollisionPoint();
                float overlap = -geomA.GetOverlap();
                ResolveObjectCollision(objA, objB, colliderA, colliderB, collisionNormal, collisionPoint, overlap, geomA.PenetratesOther());
            }
        }
    }
}

void RigidBody::UpdateCollider() {
    Volatile &data = *mData;
    float radius = UMath::Max(mPrimitives.GetRadius(), 3.0f);
    mWCollider->Refresh(data.position, radius, true);
    Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, true, mWCollider, true);
}

// UNSOLVED but functionally matching
bool RigidBody::CanCollideWithWorld() const {
    const Volatile &data = *mData;

    if (data.GetStatus(0x40) && !mSpecs.NO_WORLD_COLLISIONS() && !IsSleeping()) {
        if (mPrimitives.Size() == 0) {
            return false;
        }
        return true;
    }
    return false;
}

bool RigidBody::OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) {
    Volatile &data = *mData;
    data.SetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
    SimSurface surface(cInfo.fBle.fSurfaceRef);
    surface.DebugOverride();
    UVector3 wV;
    if (cInfo.fNormal.w > 0.0f) {
        UVector3 dp(cInfo.fNormal);
        dp *= cInfo.fNormal.w;
        UMath::Add(data.position, dp, data.position);
    }
    COLLISION_INFO collisionInfo;

    if (ResolveWorldCollision(UVector3(cInfo.fNormal), cPoint, &collisionInfo, reinterpret_cast<Primitive *>(userdata)->GetMaterial(), surface, wV) >
        0.0f) {
        new ECollision(collisionInfo);
    }
    return true;
}

void RigidBody::DoWorldCollisions(const float dT) {
    if (CanCollideWithWorld() != 0) {
        DoObbCollision(dT);
        DoBarrierCollision(dT);
    }
    if (CanCollideWithGround() && !mWCollider->GetInstanceList().empty()) {
        DoInstanceCollision(dT);
    }
}

void RigidBody::DoObbCollision(float dT) {
    if (mWCollider->fObbList.empty()) {
        return;
    }

    Volatile &data = *mData;
    WCollisionMgr mgr(mCollisionMask, 3);
    Dynamics::Collision::Geometry thisGeom;
    Dynamics::Collision::Geometry otherGeom;

    const WCollisionObject *const *obb = mWCollider->fObbList.begin();
    const WCollisionObject *const *obbEnd = mWCollider->fObbList.end();
    while (obb != obbEnd) {
        UMath::Vector3 otherVelocity = UMath::Vector3::kZero;
        WSurface otherSurface;
        const WCollisionObject *object = *obb++;

        mgr.BuildGeomFromWorldObb(*object, dT, otherGeom, otherVelocity, otherSurface);
        for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
            if (!(collider->GetFlags() & Primitive::VSWORLD) || (collider->GetFlags() & Primitive::DISABLED) ||
                !collider->SetCollision(data, thisGeom) || !Dynamics::Collision::Geometry::FindIntersection(&thisGeom, &otherGeom, &thisGeom)) {
                continue;
            }

            COLLISION_INFO collisionInfo;
            SimSurface bodySurface(collider->GetMaterial());

            data.SetStatus(Volatile::HAS_HAD_OBJECT_COLLISION);
            UMath::ScaleAdd(thisGeom.GetCollisionNormal(), -thisGeom.GetOverlap(), data.position, data.position);
            if (ResolveWorldOBBCollision(thisGeom.GetCollisionNormal(), thisGeom.GetCollisionPoint(), &collisionInfo, &otherGeom, otherVelocity,
                                         bodySurface, SimSurface::kNull)) {
                data.SetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
                new ECollision(collisionInfo);
            }
        }
    }
}

void RigidBody::DoBarrierCollision(float dT) {
    if (mWCollider->fBarrierList.empty()) {
        return;
    }

    const Volatile &data = *mData;
    const WCollisionBarrierList &barriers = mWCollider->fBarrierList;
    Dynamics::Collision::Geometry thisGeom;
    bool test_primitives = true;

    if (mPrimitives.Size() > 1) {
        UMath::Vector3 velocity;
        UMath::Vector3 dim;
        float radius = mPrimitives.GetRadius();

        UMath::Scale(data.linearVel, dT, velocity);
        dim.x = radius;
        dim.y = radius;
        dim.z = radius;
        thisGeom.Set(data.bodyMatrix, data.position, dim, Dynamics::Collision::Geometry::SPHERE, velocity);
        test_primitives = WCollisionMgr(mCollisionMask, 3).Collide(&thisGeom, &barriers, this, nullptr, false);
    }

    if (test_primitives) {
        for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
            if ((((collider->GetFlags() ^ 1) & 1) == 0) && !(collider->GetFlags() & 8) && collider->SetCollision(data, thisGeom)) {
                WCollisionMgr(mCollisionMask, 3).Collide(&thisGeom, &barriers, this, collider, (collider->GetFlags() & Primitive::ONESIDED) != 0);
            }
        }
    }
}

void RigidBody::DoInstanceCollision(float dT) {
    if (!mSpecs.INSTANCE_COLLISIONS_3D()) {
        DoInstanceCollision2d(dT);
    } else {
        DoInstanceCollision3d(dT);
    }
}

void RigidBody::DoInstanceCollision2d(float dT) {
    Volatile &data = *mData;
    CollisionPacket packets[16];
    unsigned int packetCount = 0;
    UMath::Vector4 deepestPenetration = UMath::Vector4::kZero;
    UMath::Vector3 worldCG;
    WWorldPos worldPos = GetOwner()->GetWPos();
    UMath::Vector3 upExtent;

    data.leversInContact = 0;
    UMath::Rotate(mCOG, data.bodyMatrix, worldCG);

    upExtent.x = data.bodyMatrix.v0.y * mDimension.x;
    upExtent.y = data.bodyMatrix.v1.y * mDimension.y;
    upExtent.z = data.bodyMatrix.v2.y * mDimension.z;

    const float speedXZ = UMath::Lengthxz(data.linearVel);
    const float projectedHeight = UMath::Length(upExtent);
    const float maxBodyY = data.position.y + projectedHeight;

    for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList() && packetCount < 16; collider = collider->GetNext()) {
        if (!(collider->GetFlags() & Primitive::VSGROUND) || (collider->GetFlags() & Primitive::DISABLED) ||
            collider->GetShape() != Dynamics::Collision::Geometry::BOX) {
            continue;
        }

        Dynamics::Collision::Geometry boxGeom;
        if (!collider->SetCollision(data, boxGeom)) {
            continue;
        }

        const UMath::Vector3 &dim = collider->GetDimension();
        for (int corner = 0; corner < 8 && packetCount < 16; ++corner) {
            UMath::Vector3 localCorner = UMath::Vector3::kZero;
            UMath::Vector3 lever;
            UMath::Vector3 worldPoint;
            UMath::Vector3 arm;
            UMath::Vector4 groundNormal = mGroundNormal;

            localCorner.x = (corner & 1) ? dim.x : -dim.x;
            localCorner.y = (corner & 2) ? dim.y : -dim.y;
            localCorner.z = (corner & 4) ? dim.z : -dim.z;

            UMath::Rotate(localCorner, boxGeom.GetOrientation(), lever);
            UMath::Add(lever, boxGeom.GetPosition(), worldPoint);
            UMath::Sub(worldPoint, data.position, lever);

            float fallDistance =
                (data.angularVel.x * (lever.z - worldCG.z) - (data.angularVel.z * (lever.x - worldCG.x) + data.linearVel.y)) * dT;
            fallDistance = UMath::Max(fallDistance, 0.0f);

            float tolerance = maxBodyY - worldPoint.y;
            const float maxTolerance = projectedHeight + speedXZ * dT + fallDistance;
            if (maxTolerance < tolerance) {
                tolerance = maxTolerance;
            }
            if (tolerance < 0.25f) {
                tolerance = 0.25f;
            }

            worldPos.SetTolerance(tolerance);
            if (worldPos.Update(worldPoint, groundNormal, true, mWCollider, true) && groundNormal.w > 1.0e-6f) {
                if (deepestPenetration.w < groundNormal.w) {
                    deepestPenetration = groundNormal;
                }

                arm = lever;
                ConvertWorldToLocal(arm, false);

                CollisionPacket &packet = packets[packetCount++];
                packet.lever = lever;
                packet.bodysurface = collider->GetMaterial();
                packet.normal = UMath::Vector4To3(groundNormal);
                packet.penetration = groundNormal.w;
                packet.arm = arm;
                packet.surface = worldPos.GetSurface();
            }
        }
    }

    for (Mesh *mesh = mMeshes.GetHead(); mesh != mMeshes.EndOfList() && packetCount < 16; mesh = mesh->GetNext()) {
        if (mesh->GetFlags() & Mesh::DISABLED) {
            continue;
        }

        const UMath::Vector4 *vertices = mesh->GetVerts();
        const unsigned int vertexCount = mesh->GetNumVertices();
        for (unsigned int i = 0; i < vertexCount && packetCount < 16; ++i) {
            UMath::Vector3 arm = UMath::Vector4To3(vertices[i]);
            UMath::Vector3 lever;
            UMath::Vector3 worldPoint;
            UMath::Vector4 groundNormal = mGroundNormal;

            UMath::Rotate(arm, data.bodyMatrix, lever);
            UMath::Add(lever, data.position, worldPoint);

            float fallDistance =
                (data.angularVel.x * (lever.z - worldCG.z) - (data.angularVel.z * (lever.x - worldCG.x) + data.linearVel.y)) * dT;
            fallDistance = UMath::Max(fallDistance, 0.0f);

            float tolerance = maxBodyY - worldPoint.y;
            const float maxTolerance = projectedHeight + speedXZ * dT + fallDistance;
            if (maxTolerance < tolerance) {
                tolerance = maxTolerance;
            }
            if (tolerance < 0.25f) {
                tolerance = 0.25f;
            }

            worldPos.SetTolerance(tolerance);
            if (worldPos.Update(worldPoint, groundNormal, true, mWCollider, true) && groundNormal.w > 1.0e-6f) {
                if (deepestPenetration.w < groundNormal.w) {
                    deepestPenetration = groundNormal;
                }

                CollisionPacket &packet = packets[packetCount++];
                packet.lever = lever;
                packet.bodysurface = mesh->GetMaterial();
                packet.normal = UMath::Vector4To3(groundNormal);
                packet.penetration = groundNormal.w;
                packet.arm = arm;
                packet.surface = worldPos.GetSurface();
            }
        }
    }

    if (packetCount != 0) {
        data.leversInContact = static_cast<char>(packetCount);
        if (packetCount > 1) {
            std::sort(packets, packets + packetCount);
        }

        ResolveGroundCollision(packets, packetCount);
        if (deepestPenetration.w > 0.0f) {
            UMath::ScaleAdd(UMath::Vector4To3(deepestPenetration), deepestPenetration.w, data.position, data.position);
        }
    }
}

void RigidBody::DoInstanceCollision3d(float dT) {
    Volatile &data = *mData;
    WCollisionMgr::WorldCollisionInfo cInfo;
    Dynamics::Collision::Geometry thisGeom;
    const struct WCollisionInstanceCacheList &instances = mWCollider->GetInstanceList();
    for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
        if ((((collider->GetFlags() ^ 1) & 1) == 0) && !(collider->GetFlags() & 8) && collider->SetCollision(data, thisGeom)) {
            WCollisionMgr(mCollisionMask, 3).Collide(&thisGeom, &instances, this, collider);
        }
    }
}

bool RigidBody::ShouldSleep() const {
    const Volatile &data = *mData;
    float sumVel = UMath::Length(data.linearVel) + (UMath::Length(data.angularVel) * data.radius);
    if (sumVel < mSpecs.SLEEP_VELOCITY() && GetNumContactPoints() > 2) {
        return true;
    }
    return false;
}

void RigidBody::Accelerate(const UMath::Vector3 &a, float dT) {
    Volatile &data = *mData;
    if (data.state == 0) {
        UMath::ScaleAdd(a, dT, data.linearVel, data.linearVel);
    }
}

bool RigidBody::AddCollisionSphere(float radius, const UMath::Vector3 &offset, const SimSurface &material, unsigned int flags, const UCrc32 &name) {
    return mPrimitives.Create(UVector3(radius, radius, radius), offset, material, Dynamics::Collision::Geometry::SPHERE, UMath::Vector4::kIdentity,
                              flags | 0x10, name);
}

bool RigidBody::AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                                unsigned int flags, const UCrc32 &name) {
    return mPrimitives.Create(dim + mSpecs.COLLISION_BOX_PAD(), offset, material, Dynamics::Collision::Geometry::BOX, orient, flags, name);
}

bool RigidBody::CanCollideWithObjects() const {
    if (mPrimitives.Size() == 0) {
        return false;
    }
    if (mSpecs.NO_OBJ_COLLISIONS() != 0) {
        return false;
    }
    if (mData->state == 2) {
        return false;
    }
    return true;
}

void RigidBody::OnTaskSimulate(float dT) {}

void RigidBody::OnDebugDraw() {}

unsigned int RigidBody::GetTriggerFlags() const {
    if (mData->GetStatus(1) || IsSleeping()) {
        return 0;
    }
    return 0x40;
}

void RigidBody::PushSP(void *workspace) {
    mOnSP = 1;
    ScratchPtr<RigidBody::Volatile>::Push(workspace);
}

void RigidBody::PopSP() {
    mOnSP = 0;
    ScratchPtr<RigidBody::Volatile>::Pop();
}

void RigidBody::Update(const float dT) {
    int overlapX = 0;
    int overlapZ = 0;

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        body->OnBeginFrame(dT);
        body->UpdateGrid(overlapX, overlapZ);
    }

    for (SAPNodeAccess *nodeX = reinterpret_cast<SAPNodeAccess *>(SAP::Grid<RigidBody>::mRootX); nodeX;) {
        SAPNodeAccess *next = nodeX->mTail;
        const float position = nodeX->mPosition;
        nodeX->mSort = position;

        SAPNodeAccess *head = nullptr;
        for (SAPNodeAccess *node = nodeX->mHead; node && position < node->mSort; node = node->mHead) {
            head = node;
        }

        if (head) {
            SAPNodeAccess *newHead = head->mHead;
            if (*nodeX->mRoot == nodeX) {
                *nodeX->mRoot = nodeX->mTail;
            }
            if (nodeX->mHead) {
                nodeX->mHead->mTail = nodeX->mTail;
            }
            if (nodeX->mTail) {
                nodeX->mTail->mHead = nodeX->mHead;
            }
            nodeX->mTail = head;
            nodeX->mHead = newHead;
            if (newHead) {
                newHead->mTail = nodeX;
            }
            head->mHead = nodeX;
            if (nodeX->mTail == *nodeX->mRoot) {
                *nodeX->mRoot = nodeX;
            }
            nodeX = next;
            continue;
        }

        SAPNodeAccess *tail = nullptr;
        for (SAPNodeAccess *node = nodeX->mTail; node && node->mSort < position; node = node->mTail) {
            tail = node;
        }

        if (tail) {
            SAPNodeAccess *newTail = tail->mTail;
            if (*nodeX->mRoot == nodeX) {
                *nodeX->mRoot = nodeX->mTail;
            }
            if (nodeX->mHead) {
                nodeX->mHead->mTail = nodeX->mTail;
            }
            if (nodeX->mTail) {
                nodeX->mTail->mHead = nodeX->mHead;
            }
            nodeX->mHead = tail;
            tail->mTail = nodeX;
            if (newTail) {
                nodeX->mTail = newTail;
                newTail->mHead = nodeX;
                if (nodeX->mTail == *nodeX->mRoot) {
                    *nodeX->mRoot = nodeX;
                }
            } else {
                nodeX->mTail = nullptr;
            }
        }
        nodeX = next;
    }

    for (SAPNodeAccess *nodeZ = reinterpret_cast<SAPNodeAccess *>(SAP::Grid<RigidBody>::mRootZ); nodeZ;) {
        SAPNodeAccess *next = nodeZ->mTail;
        const float position = nodeZ->mPosition;
        nodeZ->mSort = position;

        SAPNodeAccess *head = nullptr;
        for (SAPNodeAccess *node = nodeZ->mHead; node && position < node->mSort; node = node->mHead) {
            head = node;
        }

        if (head) {
            SAPNodeAccess *newHead = head->mHead;
            if (*nodeZ->mRoot == nodeZ) {
                *nodeZ->mRoot = nodeZ->mTail;
            }
            if (nodeZ->mHead) {
                nodeZ->mHead->mTail = nodeZ->mTail;
            }
            if (nodeZ->mTail) {
                nodeZ->mTail->mHead = nodeZ->mHead;
            }
            nodeZ->mTail = head;
            nodeZ->mHead = newHead;
            if (newHead) {
                newHead->mTail = nodeZ;
            }
            head->mHead = nodeZ;
            if (nodeZ->mTail == *nodeZ->mRoot) {
                *nodeZ->mRoot = nodeZ;
            }
            nodeZ = next;
            continue;
        }

        SAPNodeAccess *tail = nullptr;
        for (SAPNodeAccess *node = nodeZ->mTail; node && node->mSort < position; node = node->mTail) {
            tail = node;
        }

        if (tail) {
            SAPNodeAccess *newTail = tail->mTail;
            if (*nodeZ->mRoot == nodeZ) {
                *nodeZ->mRoot = nodeZ->mTail;
            }
            if (nodeZ->mHead) {
                nodeZ->mHead->mTail = nodeZ->mTail;
            }
            if (nodeZ->mTail) {
                nodeZ->mTail->mHead = nodeZ->mHead;
            }
            nodeZ->mHead = tail;
            tail->mTail = nodeZ;
            if (newTail) {
                nodeZ->mTail = newTail;
                newTail->mHead = nodeZ;
                if (nodeZ->mTail == *nodeZ->mRoot) {
                    *nodeZ->mRoot = nodeZ;
                }
            } else {
                nodeZ->mTail = nullptr;
            }
        }
        nodeZ = next;
    }

    const bool useZ = overlapZ <= overlapX;
    SAPNodeAccess *root = reinterpret_cast<SAPNodeAccess *>(useZ ? SAP::Grid<RigidBody>::mRootZ : SAP::Grid<RigidBody>::mRootX);
    for (SAPNodeAccess *node = root; node; node = node->mTail) {
        if (node == reinterpret_cast<SAPNodeAccess *>(node->mAxis)) {
            SAPAxisAccess *axis = reinterpret_cast<SAPAxisAccess *>(node->mAxis);
            RBGridAccess *grid = reinterpret_cast<RBGridAccess *>(axis->mGrid);
            SAPAxisAccess *otherAxis = useZ ? &grid->mX : &grid->mZ;
            const float max = otherAxis->mMax.mPosition;
            const float min = otherAxis->mMin.mPosition;

            for (SAPNodeAccess *test = node->mTail; test != &axis->mMax; test = test->mTail) {
                if (test == reinterpret_cast<SAPNodeAccess *>(test->mAxis)) {
                    SAPAxisAccess *testAxis = reinterpret_cast<SAPAxisAccess *>(test->mAxis);
                    RBGridAccess *testGrid = reinterpret_cast<RBGridAccess *>(testAxis->mGrid);
                    SAPAxisAccess *testOtherAxis = useZ ? &testGrid->mX : &testGrid->mZ;
                    const float testMin = testOtherAxis->mMin.mPosition;
                    const float testMax = testOtherAxis->mMax.mPosition;

                    if (((min <= testMin) && (testMin <= max)) || ((min <= testMax) && (testMax <= max)) ||
                        ((testMin < min) && (max < testMax))) {
                        OnObjectOverlap(*grid->mOwner, *testGrid->mOwner, dT);
                    }
                }
            }
        }
    }

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        body->OnEndFrame(dT);
    }
    Dynamics::Articulation::Resolve();
}

void RigidBody::Damp(float amount) {
    Volatile &data = *mData;
    float scale = 1.0f - amount;
    UMath::Scale(data.linearVel, scale, data.linearVel);
    UMath::Scale(data.angularVel, scale, data.angularVel);
    UMath::Scale(data.force, scale, data.force);
    UMath::Scale(data.torque, scale, data.torque);
}

void RigidBody::UpdateGrid(int &overlapx, int &overlapz) {
    Volatile &data = *mData;
    data.RemoveStatus(Volatile::HAS_HAD_OBJECT_COLLISION | Volatile::HAS_HAD_WORLD_COLLISION);
    const bool collideable = CanCollideWithObjects();
    if (collideable && !mGrid) {
        mGrid = RBGrid::Add(data.index, *this, data.position, mPrimitives.GetRadius());
        return;
    }
    if (!collideable && mGrid) {
        RBGrid::Remove(mGrid);
        mGrid = nullptr;
        return;
    }
    if (!mGrid) {
        return;
    }
    mGrid->SetPosition(data.position, mPrimitives.GetRadius());

    if (mGrid->GetX().Overlaps()) {
        overlapx++;
    }
    if (mGrid->GetZ().Overlaps()) {
        overlapz++;
    }
}

IRigidBody *RigidBody::Get(unsigned int index) {
    if (index < RIGID_BODY_MAX) {
        return mMaps[index];
    }
    return nullptr;
}

unsigned int RigidBody::AssignSlot() {
    for (std::size_t i = 0; i < RIGID_BODY_MAX; ++i) {
        if (mMaps[i] == nullptr) {
            return i;
        }
    }
    return static_cast<unsigned int>(-1);
}
