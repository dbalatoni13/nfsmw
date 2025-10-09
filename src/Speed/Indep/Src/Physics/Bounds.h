#ifndef PHYSICS_BOUNDS_H
#define PHYSICS_BOUNDS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

namespace CollisionGeometry {

enum BoundFlags {
    kBounds_Joint_Invert = 8192,
    kBounds_Male_Post = 4096,
    kBounds_Joint_Male = 2048,
    kBounds_Joint_Female = 1024,
    kBounds_Constraint_Prismatic = 512,
    kBounds_Constraint_Conical = 256,
    kBounds_Sphere = 128,
    kBounds_Box = 64,
    kBounds_Internal = 32,
    kBounds_MeshVsGround = 16,
    kBounds_PrimVsGround = 8,
    kBounds_PrimVsObjects = 4,
    kBounds_PrimVsWorld = 2,
    kBounds_Disabled = 1,
};

struct _V3c {
    // total size: 0x6
    short x; // offset 0x0, size 0x2
    short y; // offset 0x2, size 0x2
    short z; // offset 0x4, size 0x2
};

struct _Q4c {
    // total size: 0x8
    short x; // offset 0x0, size 0x2
    short y; // offset 0x2, size 0x2
    short z; // offset 0x4, size 0x2
    short w; // offset 0x6, size 0x2
};

struct BoundsHeader {
    // total size: 0x10
    UCrc32 fNameHash; // offset 0x0, size 0x4
    int fNumBounds;   // offset 0x4, size 0x4
    int fIsResolved;  // offset 0x8, size 0x4
    int fPad;         // offset 0xC, size 0x4
};

struct Bounds;
class IBoundable;

struct Collection : public BoundsHeader {
    // total size: 0x10

    bool AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const;
};

struct Bounds {
    // total size: 0x30
    _Q4c fOrientation;          // offset 0x0, size 0x8
    _V3c fPosition;             // offset 0x8, size 0x6
    unsigned short fFlags;      // offset 0xE, size 0x2
    _V3c fHalfDimensions;       // offset 0x10, size 0x6
    unsigned char fNumChildren; // offset 0x16, size 0x1
    char fPCloudIndex;          // offset 0x17, size 0x1
    _V3c fPivot;                // offset 0x18, size 0x6
    short fChildIndex;          // offset 0x1E, size 0x2
    float fRadius;              // offset 0x20, size 0x4
    UCrc32 fSurface;            // offset 0x24, size 0x4
    UCrc32 fNameHash;           // offset 0x28, size 0x4
    Collection *fCollection;    // offset 0x2C, size 0x4
};

class IBoundable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IBoundable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IBoundable() {}

    virtual const CollisionGeometry::Bounds *GetGeometryNode() const;
    virtual bool AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                                       const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundFlags);
    virtual bool AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const SimSurface &material,
                                  CollisionGeometry::BoundFlags flags, bool persistant);
};

const Attrib::Collection *Lookup(UCrc32 object_name_hash);
bool CreateJoint(IBoundable *ifemale, struct UCrc32 femalenode_name, IBoundable *imale, UCrc32 malenode_name, UMath::Vector3 *out_female,
                 UMath::Vector3 *out_male, unsigned int joint_flags);

}; // namespace CollisionGeometry

#endif
