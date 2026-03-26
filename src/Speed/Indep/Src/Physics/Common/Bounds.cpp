#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

inline void bPlatEndianSwap(UCrc32 *c) {
    unsigned int val = c->GetValue();
    ::bPlatEndianSwap(&val);
    *c = UCrc32(val);
}

inline void bPlatEndianSwap(UMath::Vector4 *v) {
    ::bPlatEndianSwap(&v->x);
    ::bPlatEndianSwap(&v->y);
    ::bPlatEndianSwap(&v->z);
    ::bPlatEndianSwap(&v->w);
}

void OrthoInverse(UMath::Matrix4 &m);

namespace CollisionGeometry {

static Collections TheCollections;
static UMath::Matrix4 fix;

BoundsPack::BoundsPack(bChunk *pack) : mChunk(pack) {
    bChunk *last_chunk = reinterpret_cast< bChunk * >(reinterpret_cast< char * >(pack) + pack->Size + 8);
    int count = 0;
    for (bChunk *chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        count = count + 1;
    }
    mTable.reserve(count);

    for (bChunk *chunk = pack->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
        BoundsHeader *pheader = reinterpret_cast< BoundsHeader * >(chunk->GetAlignedData(16));
        UCrc32 name(pheader->fNameHash);
        ::bPlatEndianSwap(&name);

        Collection *collection = mTable.Find(UCrc32(name));
        if (collection == nullptr) {
            collection = reinterpret_cast< Collection * >(pheader);
            collection->Init();
            mTable.Add(collection);
        }
    }
}

inline Collection *BoundsPack::Table::Find(UCrc32 name) {
    Pair *iter = _STL::lower_bound(begin(), end(), Pair(name, nullptr));
    if (iter != end() && iter->Name == name) {
        return iter->Collection;
    }
    return nullptr;
}

BoundsPack *Collections::Find(const bChunk *header) {
    for (BoundsPack *pack = GetHead(); pack != EndOfList(); pack = pack->GetNext()) {
        if (pack->GetHeader() == header) {
            return pack;
        }
    }
    return nullptr;
}

const Collection *Collections::Find(UCrc32 name) {
    for (BoundsPack *pack = GetHead(); pack != EndOfList(); pack = pack->GetNext()) {
        const Collection *collection = pack->Find(UCrc32(name));
        if (collection != nullptr) {
            return collection;
        }
    }
    return nullptr;
}

const Bounds *Collection::GetRoot() const {
    if (fNumBounds > 0) {
        return GetBounds();
    }
    return nullptr;
}

const Bounds *Collection::GetChild(const Bounds *parent, UCrc32 name) const {
    if (parent->fChildIndex >= 0) {
        for (int idx = 0; idx < static_cast< int >(parent->fNumChildren); idx++) {
            const Bounds *bnds = &GetBounds()[parent->fChildIndex + idx];
            if (bnds->fNameHash == name) {
                return bnds;
            }
        }
    }
    return nullptr;
}

const Bounds *Collection::GetChild(const Bounds *parent, unsigned int idx) const {
    if (idx < parent->fNumChildren && parent->fChildIndex >= 0) {
        return &GetBounds()[parent->fChildIndex + idx];
    }
    return nullptr;
}

const PCloud *Collection::GetPointCloud(const Bounds *parent) const {
    const PCloudHeader *pcheader = GetPCHeader();
    int numPClouds = pcheader->fNumPClouds;
    if (numPClouds > 0 && parent->fPCloudIndex >= 0) {
        const PCloud *pcloud = GetPCloud();
        for (int i = 0; i < numPClouds; i++) {
            if (i == static_cast< int >(parent->fPCloudIndex)) {
                return pcloud;
            }
            pcloud = reinterpret_cast< const PCloud * >(pcloud->fPList + pcloud->fNumVerts);
        }
    }
    return nullptr;
}

const Bounds *Collection::GetBounds(UCrc32 hash_name) const {
    if (hash_name != UCrc32::kNull) {
        for (int i = 0; i < fNumBounds; i++) {
            if (GetBounds()[i].fNameHash == hash_name) {
                return &GetBounds()[i];
            }
        }
    }
    return nullptr;
}

void Collection::Init() {
    if (fIsResolved == 0) {
        ::bPlatEndianSwap(&fNameHash);
        ::bPlatEndianSwap(&fNumBounds);
        if (fIsResolved == 0) {
            int i;
            PCloud *pcloud;
            for (i = 0; i < fNumBounds; i++) {
                Bounds &bounds = GetBounds()[i];
                ::bPlatEndianSwap(&bounds.fPosition.x);
                ::bPlatEndianSwap(&bounds.fPosition.y);
                ::bPlatEndianSwap(&bounds.fPosition.z);
                ::bPlatEndianSwap(&bounds.fHalfDimensions.x);
                ::bPlatEndianSwap(&bounds.fHalfDimensions.y);
                ::bPlatEndianSwap(&bounds.fHalfDimensions.z);
                ::bPlatEndianSwap(&bounds.fOrientation.x);
                ::bPlatEndianSwap(&bounds.fOrientation.y);
                ::bPlatEndianSwap(&bounds.fOrientation.z);
                ::bPlatEndianSwap(&bounds.fOrientation.w);
                ::bPlatEndianSwap(&bounds.fPivot.x);
                ::bPlatEndianSwap(&bounds.fPivot.y);
                ::bPlatEndianSwap(&bounds.fPivot.z);
                ::bPlatEndianSwap(&bounds.fChildIndex);
                ::bPlatEndianSwap(&bounds.fRadius);
                ::bPlatEndianSwap(&bounds.fFlags);
                ::bPlatEndianSwap(&bounds.fNameHash);
                ::bPlatEndianSwap(&bounds.fSurface);
            }
            ::bPlatEndianSwap(&GetPCHeader()->fNumPClouds);
            pcloud = GetPCloud();
            i = 0;
            while (i < GetPCHeader()->fNumPClouds) {
                ::bPlatEndianSwap(&pcloud->fNumVerts);
                i = i + 1;
                pcloud->fPList = reinterpret_cast< UMath::Vector4 * >(pcloud + 1);
                for (int j = 0; j < pcloud->fNumVerts; j++) {
                    ::bPlatEndianSwap(&pcloud->fPList[j]);
                }
                pcloud = reinterpret_cast< PCloud * >(pcloud->fPList + pcloud->fNumVerts);
            }
            fIsResolved = 1;
        }
    } else {
        PCloud *pcloud = GetPCloud();
        for (int i = 0; i < GetPCHeader()->fNumPClouds; i++) {
            pcloud->fPList = reinterpret_cast< UMath::Vector4 * >(pcloud + 1);
            pcloud = reinterpret_cast< PCloud * >(pcloud->fPList + pcloud->fNumVerts);
        }
    }

    for (int i = 0; i < fNumBounds; i++) {
        GetBounds()[i].fCollection = this;
    }
}

bool Collection::AddTo(IBoundable *irbc, const Bounds *root, const SimSurface &defsurface, bool parsechildren) const {
    bool added = false;
    if (root != nullptr) {
        if (parsechildren && root->fNumChildren != 0) {
            for (unsigned int i = 0; i < root->fNumChildren; i++) {
                const Bounds *geom = GetChild(root, i);
                if (AddNode(irbc, geom, defsurface, true)) {
                    added = true;
                }
            }
        }
        if (!added) {
            if (AddNode(irbc, root, defsurface, false)) {
                added = true;
            }
        }
    }
    return added;
}

bool Collection::AddNode(IBoundable *iboundable, const Bounds *geom, const SimSurface &defsurface, bool ischild) const {
    bool result = false;
    UMath::Vector3 offset;
    UMath::Vector3 dim;
    UMath::Vector4 orientation;
    UMath::Matrix4 invmat;

    geom->GetHalfDimensions(dim);
    geom->GetPosition(offset);
    geom->GetOrientation(orientation);

    invmat = UMath::Matrix4::kIdentity;
    SimSurface surface(defsurface);
    unsigned short flags = geom->fFlags;

    if (geom->fSurface.GetValue() != 0) {
        const Attrib::Collection *surfcol = SimSurface::Lookup(geom->fSurface);
        SimSurface found(surfcol);
        surface.Change(found.GetConstCollection());
        if (surface.GetConstCollection() == SimSurface::kNull.GetConstCollection()) {
            surface.Change(defsurface.GetConstCollection());
        }
    }

    if (ischild && (flags & kBounds_Internal)) {
        result = false;
    } else {
        if (!ischild) {
            UMath::QuaternionToMatrix4(orientation, invmat);
            invmat[3].x = offset.x;
            invmat[3].y = offset.y;
            invmat[3].z = offset.z;
            invmat[3].w = 1.0f;
            OrthoInverse(invmat);
            offset = UMath::Vector3::kZero;
            orientation = UMath::Vector4::kIdentity;
        }

        if (flags & (kBounds_PrimVsWorld | kBounds_PrimVsObjects | kBounds_PrimVsGround)) {
            if (iboundable->AddCollisionPrimitive(
                    geom->fNameHash, dim, geom->fRadius, offset, surface, orientation,
                    static_cast< BoundFlags >(flags))) {
                result = true;
            }
        }

        const PCloud *pcloud;
        if ((flags & kBounds_MeshVsGround) &&
            (pcloud = GetPointCloud(geom)) != nullptr &&
            pcloud->fNumVerts > 0) {
            if (ischild) {
                iboundable->AddCollisionMesh(
                    geom->fNameHash, pcloud->fPList, pcloud->fNumVerts, surface,
                    static_cast< BoundFlags >(flags), true);
            } else {
                UMath::Vector4 tmp[16];
                int i = 0;
                do {
                    UMath::Vector4 in = pcloud->fPList[i];
                    UMath::RotateTranslate(in, invmat, tmp[i]);
                    i++;
                } while (i < pcloud->fNumVerts);
                iboundable->AddCollisionMesh(
                    geom->fNameHash, tmp, pcloud->fNumVerts, surface,
                    static_cast< BoundFlags >(flags), false);
            }
            result = true;
        }
    }

    return result;
}

const Collection *Lookup(UCrc32 object_name_hash) {
    return TheCollections.Find(UCrc32(object_name_hash));
}

bool CreateJoint(IBoundable *ifemale, UCrc32 femalenode_name, IBoundable *imale, UCrc32 malenode_name,
                 UMath::Vector3 *out_female, UMath::Vector3 *out_male, unsigned int joint_flags) {
    const Bounds *female_node = ifemale->GetGeometryNode();
    const Bounds *male_node = imale->GetGeometryNode();

    if (female_node == nullptr || male_node == nullptr) {
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

    const Bounds *male_connector = nullptr;
    for (unsigned int i = 0; i < male_node->fNumChildren; i++) {
        const Bounds *child = male_node->GetChild(i);
        if (child->fNameHash == malenode_name && (child->fFlags & kBounds_Joint_Male)) {
            male_connector = child;
            break;
        }
        male_connector = nullptr;
    }

    if (male_connector == nullptr) {
        return false;
    }

    const Bounds *female_connector = nullptr;
    for (unsigned int i = 0; i < female_node->fNumChildren; i++) {
        const Bounds *child = female_node->GetChild(i);
        if (child->fNameHash == femalenode_name && (child->fFlags & kBounds_Joint_Female)) {
            female_connector = child;
            break;
        }
        female_connector = nullptr;
    }

    if (female_connector == nullptr) {
        return false;
    }

    UMath::Vector3 lever_male;
    UMath::Vector3 lever_female;
    male_connector->GetPosition(lever_male);
    female_connector->GetPosition(lever_female);

    if (out_female != nullptr) {
        *out_female = lever_female;
    }
    if (out_male != nullptr) {
        *out_male = lever_male;
    }

    Dynamics::Articulation::HJOINT__ *hjoint = Dynamics::Articulation::Create(
        ide_female, lever_female, ide_male, lever_male,
        static_cast< Dynamics::Articulation::eJointFlags >(joint_flags));

    if (hjoint == nullptr) {
        return false;
    }

    fix[0][0] = 1.0f;
    fix[0][1] = 0.0f;
    fix[0][2] = 0.0f;
    fix[0][3] = 0.0f;
    fix[1][0] = 0.0f;
    fix[1][1] = -1.0f;
    fix[1][2] = 0.0f;
    fix[1][3] = 0.0f;
    fix[2][0] = 0.0f;
    fix[2][1] = 0.0f;
    fix[2][2] = 1.0f;
    fix[2][3] = 0.0f;
    fix[3][0] = 0.0f;
    fix[3][1] = 0.0f;
    fix[3][2] = 0.0f;
    fix[3][3] = 1.0f;

    for (unsigned int i = 0; i < female_connector->fNumChildren; i++) {
        const Bounds *constraint = female_connector->GetChild(i);
        if ((constraint->fFlags & (kBounds_Constraint_Conical | kBounds_Constraint_Prismatic)) == 0) {
            continue;
        }

        const Bounds *post = nullptr;
        for (unsigned int j = 0; j < male_connector->fNumChildren; j++) {
            post = male_connector->GetChild(j);
            if (!(post->fFlags & kBounds_Male_Post)) {
                continue;
            }
            if (post->fNameHash == constraint->fNameHash) {
                break;
            }
        }

        if (post == nullptr) {
            continue;
        }

        UMath::Vector3 constraint_dim;
        UMath::Vector4 q;
        UMath::Matrix4 constraint_mat;
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
            UMath::Vector3 post_vec;
            post_vec.x = post_mat[2][0];
            post_vec.y = post_mat[2][1];
            post_vec.z = post_mat[2][2];
            UMath::Scale(post_vec, post_dim.y + post_dim.y);
            float theta = UMath::Atan2d(constraint_dim.x, constraint_dim.y + constraint_dim.y);
            Dynamics::Articulation::Constrain(hjoint, ide_female, constraint_mat, theta * 360.0f, theta * 360.0f, post_vec, Dynamics::Articulation::CONICAL);
        } else if (constraint->fFlags & kBounds_Constraint_Prismatic) {
            UMath::Vector3 post_vec;
            post_vec.x = post_mat[2][0];
            post_vec.y = post_mat[2][1];
            post_vec.z = post_mat[2][2];
            UMath::Scale(post_vec, post_dim.y + post_dim.y);
            float theta = UMath::Atan2d(constraint_dim.x, constraint_dim.y + constraint_dim.y);
            Dynamics::Articulation::Constrain(hjoint, ide_female, constraint_mat, theta * 360.0f, theta * 360.0f, post_vec, Dynamics::Articulation::PRISMATIC);
        }
    }

    return true;
}

}; // namespace CollisionGeometry

int LoaderBounds(bChunk *chunk) {
    if (chunk->GetID() != 0x8003b900) {
        return 0;
    }
    CollisionGeometry::TheCollections.AddHead(new CollisionGeometry::BoundsPack(chunk));
    return 1;
}

int UnloaderBounds(bChunk *chunk) {
    if (chunk->GetID() != 0x8003b900) {
        return 0;
    }
    CollisionGeometry::BoundsPack *pack = CollisionGeometry::TheCollections.Find(chunk);
    if (pack != nullptr) {
        CollisionGeometry::TheCollections.Remove(pack);
        delete pack;
    }
    return 1;
}
