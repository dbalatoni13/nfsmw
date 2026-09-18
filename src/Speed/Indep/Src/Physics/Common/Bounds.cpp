#include "Speed/Indep/Src/Physics/Bounds.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Dynamics/Articulation.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace CollisionGeometry {

// total size: 0x1C
class BoundsPack : public bTNode<BoundsPack> {
  public:
    // total size: 0x8
    struct Pair {
        Pair() {}

        Pair(UCrc32 name, struct Collection *collection) : Name(name), Collection(collection) {}

        bool operator<(const Pair &rhs) const {
            return Name < rhs.Name;
        }

        UCrc32 Name;            // offset 0x0, size 0x4
        struct Collection *Collection; // offset 0x4, size 0x4 (con `struct`: EE-GCC no deja que el miembro cambie el significado del tipo)
    };

    // total size: 0x10
    struct Table : public UTL::Std::vector<Pair, _type_CollisionBoundsTable> {
        Collection *Find(UCrc32 name);
        void Add(Collection *collection);
    };

    USE_FASTALLOC(BoundsPack);

    BoundsPack(bChunk *chunk);

    Collection *Find(UCrc32 name) {
        return mTable.Find(name);
    }

    bChunk *GetHeader() {
        return mChunk;
    }

  private:
    bChunk *mChunk; // offset 0x8, size 0x4
    Table mTable;   // offset 0xC, size 0x10
};

inline void BoundsPack::Table::Add(Collection *collection) {
    Pair pair(collection->fNameHash, collection);
    insert(std::upper_bound(begin(), end(), pair), pair);
}

inline Collection *BoundsPack::Table::Find(UCrc32 name) {
    iterator pair = std::lower_bound(begin(), end(), Pair(name, nullptr));
    if (pair != end() && pair->Name == name) {
        return pair->Collection;
    }
    return nullptr;
}

// total size: 0x8
struct Collections : public bTList<BoundsPack> {
    Collection *Find(UCrc32 name);
    BoundsPack *Find(const bChunk *chunk);
};

} // namespace CollisionGeometry

static CollisionGeometry::Collections TheCollections;

namespace CollisionGeometry {

BoundsPack::BoundsPack(bChunk *pack) : mChunk(pack) {
    bChunk *chunk;
    int count = 0;
    bChunk *last_chunk = pack->GetLastChunk();

    for (chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        count++;
    }

    mTable.reserve(count);

    for (chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        BoundsHeader *pheader = (BoundsHeader *)chunk->GetAlignedData(16);

        UCrc32 name(pheader->fNameHash);
        if (!pheader->fIsResolved) {
            bPlatEndianSwap(&name);
        }

        if (mTable.Find(name) == nullptr) {
            Collection *collection = (Collection *)pheader;
            collection->Init();
            mTable.Add(collection);
        }
    }
}

BoundsPack *Collections::Find(const bChunk *chunk) {
    for (BoundsPack *pack = GetHead(); pack != EndOfList(); pack = pack->GetNext()) {
        if (pack->GetHeader() == chunk) {
            return pack;
        }
    }
    return nullptr;
}

Collection *Collections::Find(UCrc32 name) {
    for (BoundsPack *pack = GetHead(); pack != EndOfList(); pack = pack->GetNext()) {
        if (Collection *collection = pack->Find(name)) {
            return collection;
        }
    }
    return nullptr;
}

bool CreateJoint(IBoundable *ifemale, UCrc32 femalenode_name, IBoundable *imale, UCrc32 malenode_name, UMath::Vector3 *out_female,
                 UMath::Vector3 *out_male, unsigned int joint_flags) {
    const Bounds *female_node = ifemale->GetGeometryNode();
    const Bounds *male_node = imale->GetGeometryNode();

    if (female_node == NULL || male_node == NULL) {
        return false;
    }

    IDynamicsEntity *ide_female;
    if (!ifemale->QueryInterface(&ide_female)) {
        return false;
    }

    IDynamicsEntity *ide_male;
    if (!imale->QueryInterface(&ide_male)) {
        return false;
    }

    const Bounds *male_connector = NULL;
    for (unsigned int i = 0; i < male_node->fNumChildren; i++) {
        const Bounds *child = male_node->GetChild(i);

        if (child->fNameHash == malenode_name && (child->fFlags & kBounds_Joint_Male)) {

            male_connector = child;
            break;
        }
    }
    if (male_connector == NULL) {
        return false;
    }

    const Bounds *female_connector = NULL;
    for (unsigned int i = 0; i < female_node->fNumChildren; i++) {
        const Bounds *child = female_node->GetChild(i);

        if (child->fNameHash == femalenode_name && (child->fFlags & kBounds_Joint_Female)) {

            female_connector = child;
            break;
        }
    }
    if (female_connector == NULL) {
        return false;
    }

    UMath::Vector3 lever_male;
    UMath::Vector3 lever_female;

    male_connector->GetPosition(lever_male);
    female_connector->GetPosition(lever_female);

    if (out_female != NULL) {
        *out_female = lever_female;
    }
    if (out_male != NULL) {
        *out_male = lever_male;
    }

    Dynamics::Articulation::HJOINT hjoint = Dynamics::Articulation::Create(ide_female, lever_female, ide_male, lever_male,
                                                                           (Dynamics::Articulation::eJointFlags) joint_flags);
    if (hjoint == NULL) {
        return false;
    }

    static UMath::Matrix4 fix;

    fix.v0 = UMath::Vector4Make(1.0f, 0.0f, 0.0f, 0.0f);
    fix.v1 = UMath::Vector4Make(0.0f, 0.0f, -1.0f, 0.0f);
    fix.v2 = UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f);
    fix.v3 = UMath::Vector4Make(0.0f, 0.0f, 0.0f, 1.0f);

    for (unsigned int i = 0; i < female_connector->fNumChildren; i++) {

        const Bounds *constraint = female_connector->GetChild(i);
        if ((constraint->fFlags & (kBounds_Constraint_Conical | kBounds_Constraint_Prismatic)) == 0) {
            continue;
        }

        const Bounds *post = NULL;
        for (unsigned int j = 0; j < male_connector->fNumChildren; j++) {
            const Bounds *malechild = male_connector->GetChild(j);

            if ((malechild->fFlags & kBounds_Male_Post) && malechild->fNameHash == constraint->fNameHash) {

                post = malechild;
                break;
            }
        }

        if (post == NULL) {
            continue;
        }

        UMath::Vector4 q;
        UMath::Matrix4 constraint_mat;
        UMath::Vector3 constraint_dim;
        UMath::Vector4 constraint_orientation;
        UMath::Vector4 female_connector_orientation;

        constraint->GetHalfDimensions(constraint_dim);
        constraint->GetOrientation(constraint_orientation);
        female_connector->GetOrientation(female_connector_orientation);
        UMath::Mult(female_connector_orientation, constraint_orientation, q);
        UMath::QuaternionToMatrix4(q, constraint_mat);
        UMath::Mult(fix, constraint_mat, constraint_mat);

        UMath::Matrix4 post_mat;
        UMath::Vector3 post_dim;
        UMath::Vector4 post_orientation;
        UMath::Vector4 male_connector_orientation;

        post->GetHalfDimensions(post_dim);
        post->GetOrientation(post_orientation);
        male_connector->GetOrientation(male_connector_orientation);
        UMath::Mult(male_connector_orientation, post_orientation, q);
        UMath::QuaternionToMatrix4(q, post_mat);
        UMath::Mult(fix, post_mat, post_mat);

        if (constraint->fFlags & kBounds_Constraint_Conical) {

            UMath::Vector3 post = UMath::Vector4To3(post_mat.v2);
            UMath::Scale(post, post_dim.y + post_dim.y);

            float angle = UMath::Atan2d(constraint_dim.x, constraint_dim.y + constraint_dim.y);

            Dynamics::Articulation::Constrain(hjoint, ide_female, constraint_mat, angle, angle, post, Dynamics::Articulation::CONICAL);
        } else if (constraint->fFlags & kBounds_Constraint_Prismatic) {

            UMath::Vector3 post = UMath::Vector4To3(post_mat.v2);
            UMath::Scale(post, post_dim.y + post_dim.y);

            float angle = UMath::Atan2d(constraint_dim.x, constraint_dim.y + constraint_dim.y);

            Dynamics::Articulation::Constrain(hjoint, ide_female, constraint_mat, angle, angle, post, Dynamics::Articulation::PRISMATIC);
        }
    }

    return true;
}

Bounds *const Collection::GetRoot() const {
    if (fNumBounds > 0) {
        return (Bounds *)(this + 1);
    }
    return NULL;
}

const Bounds *Collection::GetChild(const Bounds *parent, UCrc32 name) const {
    if (parent->fChildIndex >= 0) {
        for (int i = 0; i < parent->fNumChildren; i++) {
            const Bounds *child = BoundsList() + (parent->fChildIndex + i);
            if (child->fNameHash == name) {
                return child;
            }
        }
    }
    return nullptr;
}

const Bounds *Collection::GetChild(const Bounds *parent, unsigned int idx) const {
    if (idx < parent->fNumChildren && parent->fChildIndex >= 0) {
        return BoundsList() + (parent->fChildIndex + idx);
    }
    return nullptr;
}

const PCloud *Collection::GetPointCloud(const Bounds *parent) const {
    if (GetPointClouds()->fNumPClouds > 0 && parent->fPCloudIndex >= 0) {
        const PCloud *pc = (const PCloud *)(GetPointClouds() + 1);
        for (int i = 0; i < GetPointClouds()->fNumPClouds; i++) {
            if (i == parent->fPCloudIndex) {
                return pc;
            }
            pc = (PCloud *)((char *)pc->fPList + pc->fNumVerts * 16);
        }
    }
    return nullptr;
}

Bounds *const Collection::GetBounds(UCrc32 hash_name) const {
    if (hash_name != UCrc32::kNull) {
        for (int i = 0; i < fNumBounds; i++) {
            if ((BoundsList() + i)->fNameHash == hash_name) {
                return BoundsList() + i;
            }
        }
    }
    return nullptr;
}

void Collection::Init() {
    if (!fIsResolved) {
        bPlatEndianSwap(&fNameHash);
        bPlatEndianSwap(&fNumBounds);
    }

    if (!fIsResolved) {
        int i;
        for (i = 0; i < fNumBounds; i++) {
            Bounds &bounds = BoundsList()[i];
            bounds.fPosition.EndianSwap();
            bounds.fHalfDimensions.EndianSwap();
            bounds.fOrientation.EndianSwap();
            bounds.fPivot.EndianSwap();
            bPlatEndianSwap(&bounds.fChildIndex);
            bPlatEndianSwap(&bounds.fRadius);
            bPlatEndianSwap(&bounds.fFlags);
            bPlatEndianSwap(&bounds.fNameHash);
            bPlatEndianSwap(&bounds.fSurface);
        }

        bEndianSwap32(&GetPointClouds()->fNumPClouds);

        PCloud *pc = (PCloud *)(GetPointClouds() + 1);
        for (i = 0; i < GetPointClouds()->fNumPClouds; i++) {
            bEndianSwap32(&pc->fNumVerts);
            pc->fPList = (UMath::Vector4 *)(pc + 1);
            for (int j = 0; j < pc->fNumVerts; j++) {
                UMath::Vector4 *vert = (UMath::Vector4 *)(j * sizeof(UMath::Vector4) + (unsigned int)pc->fPList);
                bEndianSwap32(&vert->x);
                bEndianSwap32(&vert->y);
                bEndianSwap32(&vert->z);
                bEndianSwap32(&vert->w);
            }
            pc = (PCloud *)((char *)pc->fPList + pc->fNumVerts * 16);
        }

        fIsResolved = true;
    } else {
        PCloud *pc = (PCloud *)(GetPointClouds() + 1);
        for (int i = 0; i < GetPointClouds()->fNumPClouds; i++) {
            pc->fPList = (UMath::Vector4 *)(pc + 1);
            pc = (PCloud *)((char *)pc->fPList + pc->fNumVerts * 16);
        }
    }

    for (int i = 0; i < fNumBounds; i++) {
        BoundsList()[i].fCollection = this;
    }
}

bool Collection::AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const {
    bool result = false;

    if (root != nullptr) {
        if (parsechildren && root->fNumChildren != 0) {
            for (unsigned int i = 0; i < root->fNumChildren; i++) {
                if (AddNode(irbc, GetChild(root, i), defsurface, true)) {
                    result = true;
                }
            }
        }

        if (!result && AddNode(irbc, root, defsurface, false)) {
            result = true;
        }
    }

    return result;
}

bool Collection::AddNode(IBoundable *iboundable, const Bounds *geom, const SimSurface &defsurface, bool ischild) const {
    bool result = false;

    UMath::Vector3 offset;
    UMath::Vector3 dim;
    UMath::Vector4 orientation;

    geom->GetHalfDimensions(dim);
    geom->GetPosition(offset);
    geom->GetOrientation(orientation);

    UMath::Matrix4 invmat = UMath::Matrix4::kIdentity;
    SimSurface surface(defsurface);

    if (geom->fSurface.GetValue() != 0) {
        surface = SimSurface(SimSurface::Lookup(geom->fSurface));

        if (surface == SimSurface::kNull) {
            surface = defsurface;
        }
    }

    if (ischild == true && (geom->fFlags & kBounds_Internal) != 0) {
        return false;
    }

    if (!ischild) {
        UMath::QuaternionToMatrix4(orientation, invmat);
        invmat.v3 = UMath::Vector4Make(offset, 1.0f);

        OrthoInverse(invmat);

        offset = UMath::Vector3::kZero;
        orientation = UMath::Vector4::kIdentity;
    }

    if ((geom->fFlags & (kBounds_PrimVsWorld | kBounds_PrimVsObjects | kBounds_PrimVsGround)) != 0) {
        if (iboundable->AddCollisionPrimitive(geom->fNameHash, dim, geom->fRadius, offset, surface, orientation, (BoundFlags)geom->fFlags)) {
            result = true;
        }
    }

    if ((geom->fFlags & kBounds_MeshVsGround) != 0) {
        const PCloud *pcloud = GetPointCloud(geom);
        if (pcloud != nullptr && pcloud->fNumVerts > 0) {
            if (!ischild) {
                UMath::Vector4 tmp[16];
                for (int i = 0; i < pcloud->fNumVerts; i++) {
                    UMath::Vector4 in = pcloud->fPList[i];
                    in.w = 1.0f;
                    UMath::RotateTranslate(in, invmat, tmp[i]);
                }
                iboundable->AddCollisionMesh(geom->fNameHash, tmp, pcloud->fNumVerts, surface, (BoundFlags)geom->fFlags, false);
            } else {
                iboundable->AddCollisionMesh(geom->fNameHash, pcloud->fPList, pcloud->fNumVerts, surface, (BoundFlags)geom->fFlags, true);
            }

            result = true;
        }
    }

    return result;
}

const Collection *Lookup(UCrc32 object_name_hash) {
    return TheCollections.Find(object_name_hash);
}

}; // namespace CollisionGeometry

// .text:0x8021EA94 del original, scope:GLOBAL (no static): la tabla LoaderTable
// de ResourceLoader.cpp (zMisc) lo referencia por nombre desde otra unidad.
int LoaderBounds(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SPEED_BOUNDS_PACK) {
        return 0;
    }

    CollisionGeometry::BoundsPack *pack = new CollisionGeometry::BoundsPack(chunk);
    TheCollections.AddHead(pack);

    return 1;
}

// .text:0x8021EB18 del original, scope:GLOBAL (no static), igual que LoaderBounds.
int UnloaderBounds(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SPEED_BOUNDS_PACK) {
        return 0;
    }

    CollisionGeometry::BoundsPack *pack = TheCollections.Find(chunk);
    if (pack != nullptr) {
        pack->Remove();
        delete pack;
    }

    return 1;
}
