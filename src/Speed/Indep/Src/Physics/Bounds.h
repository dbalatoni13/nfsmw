#ifndef BOUNDS_H
#define BOUNDS_H

#define COLLISION_GEOM_VECTOR_PRESSICION 1000.f
#define COLLISION_GEOM_QUAT_PRECISION 32767.f

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

namespace CollisionGeometry {

enum BoundFlags {
    kBounds_Disabled = 1,
    kBounds_PrimVsWorld = 1 << 1,
    kBounds_PrimVsObjects = 1 << 2,
    kBounds_PrimVsGround = 1 << 3,
    kBounds_MeshVsGround = 1 << 4,
    kBounds_Internal = 1 << 5,
    kBounds_Box = 1 << 6,
    kBounds_Sphere = 1 << 7,
    kBounds_Constraint_Conical = 1 << 8,
    kBounds_Constraint_Prismatic = 1 << 9,
    kBounds_Joint_Female = 1 << 10,
    kBounds_Joint_Male = 1 << 11,
    kBounds_Male_Post = 1 << 12,
    kBounds_Joint_Invert = 1 << 13,
};

// total size: 0x6
struct _V3c {
    void Decompress(UMath::Vector3 &to) const {
        to.x = static_cast<float>(x) / COLLISION_GEOM_VECTOR_PRESSICION;
        to.y = static_cast<float>(y) / COLLISION_GEOM_VECTOR_PRESSICION;
        to.z = static_cast<float>(z) / COLLISION_GEOM_VECTOR_PRESSICION;
    }

  private:
    int16 x; // offset 0x0, size 0x2
    int16 y; // offset 0x2, size 0x2
    int16 z; // offset 0x4, size 0x2
};

// total size: 0x8
struct _Q4c {
    void Decompress(struct UMath::Vector4 &to) const {
        to.x = static_cast<float>(this->x) / COLLISION_GEOM_QUAT_PRECISION;
        to.y = static_cast<float>(this->y) / COLLISION_GEOM_QUAT_PRECISION;
        to.z = static_cast<float>(this->z) / COLLISION_GEOM_QUAT_PRECISION;
        to.w = static_cast<float>(this->w) / COLLISION_GEOM_QUAT_PRECISION;
    }

  private:
    int16 x; // offset 0x0, size 0x2
    int16 y; // offset 0x2, size 0x2
    int16 z; // offset 0x4, size 0x2
    int16 w; // offset 0x6, size 0x2
};

// total size: 0x10
struct BoundsHeader {
    UCrc32 fNameHash; // offset 0x0, size 0x4
    int fNumBounds;   // offset 0x4, size 0x4
    int fIsResolved;  // offset 0x8, size 0x4
    int fPad;         // offset 0xC, size 0x4
};

class Collection;

// total size: 0x30
struct Bounds {
    _Q4c fOrientation;       // offset 0x0, size 0x8
    _V3c fPosition;          // offset 0x8, size 0x6
    uint16 fFlags;           // offset 0xE, size 0x2
    _V3c fHalfDimensions;    // offset 0x10, size 0x6
    uint8 fNumChildren;      // offset 0x16, size 0x1
    int8 fPCloudIndex;       // offset 0x17, size 0x1
    _V3c fPivot;             // offset 0x18, size 0x6
    int16 fChildIndex;       // offset 0x1E, size 0x2
    float fRadius;           // offset 0x20, size 0x4
    UCrc32 fSurface;         // offset 0x24, size 0x4
    UCrc32 fNameHash;        // offset 0x28, size 0x4
    Collection *fCollection; // offset 0x2C, size 0x4

    void GetOrientation(UMath::Vector4 &to) const {
        this->fOrientation.Decompress(to);
    }

    void GetPivot(UMath::Vector3 &to) const {
        this->fPivot.Decompress(to);
    }
};

// total size: 0x10
struct PCloudHeader {
    int fNumPClouds; // offset 0x0, size 0x4
    int fPad[3];     // offset 0x4, size 0xC
};

// total size: 0x10
struct PCloud {
    int fNumVerts;          // offset 0x0, size 0x4
    int fPad[2];            // offset 0x4, size 0x8
    UMath::Vector4 *fPList; // offset 0xC, size 0x4
};

class IBoundable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IBoundable);

    virtual const CollisionGeometry::Bounds *GetGeometryNode() const;
    virtual bool AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                                       const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundFlags);
    virtual bool AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const SimSurface &material,
                                  CollisionGeometry::BoundFlags flags, bool persistant);
};

// total size: 0x10
struct Collection : public BoundsHeader {
    Bounds *const GetRoot() const;
    Bounds *const GetBounds(UCrc32 hash_name) const;
    bool AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const;
};

const Collection *Lookup(UCrc32 object_name_hash);
bool CreateJoint(IBoundable *ifemale, struct UCrc32 femalenode_name, IBoundable *imale, UCrc32 malenode_name, UMath::Vector3 *out_female,
                 UMath::Vector3 *out_male, unsigned int joint_flags);

}; // namespace CollisionGeometry

#endif
