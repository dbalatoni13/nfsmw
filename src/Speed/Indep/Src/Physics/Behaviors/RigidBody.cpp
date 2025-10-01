#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bTList<RigidBody> TheRigidBodies;

Behavior *RigidBody::Construct(const BehaviorParams &params) {
    const RBComplexParams bp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RigidBody(params, bp);
}

RigidBody::Volatile::Volatile() {}

// UNSOLVED but functionally matching
RigidBody::Mesh::Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant)
    : mVerts(nullptr), mNumVertices(count), mFlags(0), mMaterial(material.GetConstCollection()), mName(UCrc32(name)) {
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
    Mesh *mesh = new Mesh(material, verts, count, UCrc32(name), persistant);
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

void RigidBody::Primitive::Prepare(const Volatile &data) {
    UMath::Rotate(GetOffset(), data.bodyMatrix, mPrevPosition);
    UMath::Add(mPrevPosition, data.position, mPrevPosition);
}

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

void RBGrid::Remove(RBGrid *grid) {
    if (grid) {
        grid->~RBGrid();
    }
}

// UNSOLVED
RigidBody::~RigidBody() {
    if (mGrid) {
        RBGrid::Remove(mGrid);
    }
    mGrid = nullptr;
    mCount--;
    mMaps[mData->index] = nullptr;
    if (mWCollider) {
        WCollider::Destroy(mWCollider);
    }
    mWCollider = nullptr;
    mMeshes.DeleteAllElements();
    mPrimitives.DeleteAllElements();
    Dynamics::Articulation::Release(this);
    TheRigidBodies.Remove(this);
    // TODO come back to this after we decomped Listable<>Unlist or the inlines it calls
}

void RigidBody::Reset() {
    mWCollider->Clear();
    UpdateCollider();
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
        mData->SetStatus(2);
    } else {
        mData->RemoveStatus(2);
    }
    mDetachForce = detachforce;
}

void RigidBody::Detach() {
    if (mData->GetStatus(2)) {
        mData->RemoveStatus(2);
        EventSequencer::IEngine *engine = GetOwner()->GetEventSequencer();
        if (engine) {
            engine->ProcessStimulus(0xc91b90d9, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        }
    }
}

void RigidBody::SetInertiaTensor(const UMath::Vector3 &moment) {
    mData->inertiaTensor = moment;
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
    return mMeshes.Create(material, verts, count, UCrc32(name), persistant);
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
}
