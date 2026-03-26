#ifndef PHYSICS_BOUNDS_H
#define PHYSICS_BOUNDS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

DECLARE_CONTAINER_TYPE(CollisionBoundsTable);

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
    short x;
    short y;
    short z;

    void Decompress(UMath::Vector3 &to) const {
        to.x = static_cast< float >(x) * (1.0f / 128.0f);
        to.y = static_cast< float >(y) * (1.0f / 128.0f);
        to.z = static_cast< float >(z) * (1.0f / 128.0f);
    }
};

struct _Q4c {
    short x;
    short y;
    short z;
    short w;

    void Decompress(UMath::Vector4 &to) const {
        to.x = static_cast< float >(x) * (1.0f / 16384.0f);
        to.y = static_cast< float >(y) * (1.0f / 16384.0f);
        to.z = static_cast< float >(z) * (1.0f / 16384.0f);
        to.w = static_cast< float >(w) * (1.0f / 16384.0f);
    }
};

struct PCloudHeader {
    int fNumPClouds;
    int fPad[3];
};

struct PCloud {
    int fNumVerts;
    int fPad1;
    int fPad2;
    UMath::Vector4 *fPList;
};

struct BoundsHeader {
    UCrc32 fNameHash;
    int fNumBounds;
    int fIsResolved;
    int fPad;
};

struct Bounds;
class IBoundable;

struct Collection : public BoundsHeader {
    Bounds *GetBounds() { return reinterpret_cast< Bounds * >(this + 1); }
    const Bounds *GetBounds() const { return reinterpret_cast< const Bounds * >(this + 1); }
    PCloudHeader *GetPCHeader();
    const PCloudHeader *GetPCHeader() const;
    PCloud *GetPCloud();
    const PCloud *GetPCloud() const;

    const Bounds *GetRoot() const;
    const Bounds *GetChild(const Bounds *parent, UCrc32 name) const;
    const Bounds *GetChild(const Bounds *parent, unsigned int idx) const;
    const PCloud *GetPointCloud(const Bounds *parent) const;
    const Bounds *GetBounds(UCrc32 hash_name) const;
    void Init();
    bool AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const;
    bool AddNode(IBoundable *iboundable, const Bounds *geom, const SimSurface &defsurface, bool ischild) const;
};

struct Bounds {
    _Q4c fOrientation;
    _V3c fPosition;
    unsigned short fFlags;
    _V3c fHalfDimensions;
    unsigned char fNumChildren;
    char fPCloudIndex;
    _V3c fPivot;
    short fChildIndex;
    float fRadius;
    UCrc32 fSurface;
    UCrc32 fNameHash;
    Collection *fCollection;

    void GetPivot(UMath::Vector3 &to) const { fPivot.Decompress(to); }
    void GetPosition(UMath::Vector3 &to) const { fPosition.Decompress(to); }
    void GetHalfDimensions(UMath::Vector3 &to) const { fHalfDimensions.Decompress(to); }
    void GetOrientation(UMath::Vector4 &to) const { fOrientation.Decompress(to); }

    const Bounds *GetChild(unsigned int idx) const {
        return fCollection->GetChild(this, idx);
    }
    const Bounds *GetChild(UCrc32 name) const {
        return fCollection->GetChild(this, name);
    }
};

inline PCloudHeader *Collection::GetPCHeader() {
    return reinterpret_cast< PCloudHeader * >(&GetBounds()[fNumBounds]);
}
inline const PCloudHeader *Collection::GetPCHeader() const {
    return reinterpret_cast< const PCloudHeader * >(&GetBounds()[fNumBounds]);
}
inline PCloud *Collection::GetPCloud() {
    return reinterpret_cast< PCloud * >(GetPCHeader() + 1);
}
inline const PCloud *Collection::GetPCloud() const {
    return reinterpret_cast< const PCloud * >(GetPCHeader() + 1);
}

class BoundsPack : public bTNode< BoundsPack > {
    struct Pair {
        UCrc32 Name;
        Collection *Collection;

        Pair(UCrc32 name, struct Collection *collection) : Name(name), Collection(collection) {}

        bool operator<(const Pair &rhs) const {
            return Name < rhs.Name;
        }
    };

    class Table : public _STL::vector< Pair, UTL::Std::Allocator< Pair, _type_CollisionBoundsTable > > {
      public:
        void Add(Collection *collection) {
            Pair pair(collection->fNameHash, collection);
            iterator pos = _STL::upper_bound(begin(), end(), pair);
            insert(pos, pair);
        }
        Collection *Find(UCrc32 name);
    };

    const bChunk *mChunk;
    Table mTable;

  public:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }

    BoundsPack(bChunk *pack);

    const bChunk *GetHeader() const { return mChunk; }

    const Collection *Find(UCrc32 name) {
        return mTable.Find(UCrc32(name));
    }
};

struct Collections : public bTList< BoundsPack > {
    ~Collections() {}
    BoundsPack *Find(const bChunk *header);
    const Collection *Find(UCrc32 name);
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

const Collection *Lookup(UCrc32 object_name_hash);
bool CreateJoint(IBoundable *ifemale, UCrc32 femalenode_name, IBoundable *imale, UCrc32 malenode_name, UMath::Vector3 *out_female,
                 UMath::Vector3 *out_male, unsigned int joint_flags);

}; // namespace CollisionGeometry

#endif
