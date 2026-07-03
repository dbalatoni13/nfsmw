#ifndef WORLD_WCOLLISION_H
#define WORLD_WCOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WCollisionTri.h"
#include "WSurfaceTypes.h"

// total size: 0x8
struct WCollisionPackedVert {
    short x;                  // offset 0x0, size 0x2
    short y;                  // offset 0x2, size 0x2
    short z;                  // offset 0x4, size 0x2
    CollisionSurface surface; // offset 0x6, size 0x2
};

struct WCollisionBarrier;

// total size: 0x10
class WCollisionStripSphere {
  public:
    unsigned int Offset() const {
        return static_cast<unsigned int>(fOffset) + 0x10;
    }

    UMath::Vector3 fPos;    // offset 0x0, size 0xC
    unsigned short fRadius; // offset 0xC, size 0x2

  private:
    unsigned short fOffset; // offset 0xE, size 0x2
};

// total size: 0x1
struct WCollisionStrip {
    enum Flags {
        kUpFacing = 1,
        kFacingUnknown = 2,
    };

    const WCollisionPackedVert *Verts() const {
        return reinterpret_cast<const WCollisionPackedVert *>(this);
    }

    WCollisionPackedVert *Verts() {
        return reinterpret_cast<WCollisionPackedVert *>(this);
    }

    unsigned int NumVerts() const {
        return *reinterpret_cast<const unsigned short *>(&Verts()[0].surface);
    }

    unsigned int NumTris() const {
        return NumVerts() - 2;
    }

    unsigned int Flags() const {
        return *reinterpret_cast<const unsigned short *>(&Verts()[1].surface);
    }

    inline void MakeFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const {
        const WCollisionPackedVert *v = &Verts()[ind];
        retFace.fPt0.x = static_cast<float>(v->x) / 128.0f + cp.x;
        retFace.fPt0.y = static_cast<float>(v->y) / 128.0f + cp.y;
        retFace.fPt0.z = static_cast<float>(v->z) / 128.0f + cp.z;
        retFace.fSurfaceRef = nullptr;
        v++;
        retFace.fPt1.x = static_cast<float>(v->x) / 128.0f + cp.x;
        retFace.fPt1.y = static_cast<float>(v->y) / 128.0f + cp.y;
        retFace.fPt1.z = static_cast<float>(v->z) / 128.0f + cp.z;
        retFace.fFlags = 0;
        v++;
        retFace.fPt2.x = static_cast<float>(v->x) / 128.0f + cp.x;
        retFace.fPt2.y = static_cast<float>(v->y) / 128.0f + cp.y;
        retFace.fPt2.z = static_cast<float>(v->z) / 128.0f + cp.z;
        retFace.fSurface = WSurface(v->surface);
    }

    inline void MakeNextFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const {
        const WCollisionPackedVert *v = Verts() + ind + 2;
        retFace.fPt0 = retFace.fPt1;
        retFace.fPt1 = retFace.fPt2;
        retFace.fPt2.x = static_cast<float>(v->x) * (1.0f / 128.0f) + cp.x;
        retFace.fPt2.y = static_cast<float>(v->y) * (1.0f / 128.0f) + cp.y;
        retFace.fPt2.z = static_cast<float>(v->z) * (1.0f / 128.0f) + cp.z;
        retFace.fSurface = WSurface(v->surface);
    }
};

// total size: 0x10
struct WCollisionArticle {
    void Resolve();

    const Attrib::Collection *GetSurface(unsigned int ind) const {
        const char *dataStart = reinterpret_cast<const char *>(&this[1]);
        // TODO 64 bit
        unsigned int ref = reinterpret_cast<const unsigned int *>(dataStart + fStripsSize + fEdgesSize)[ind];
        return reinterpret_cast<const Attrib::Collection *>(ref);
    }

    const WCollisionBarrier *GetBarrier(unsigned int ind) const {
        const char *dataStart = reinterpret_cast<const char *>(&this[1]) + fStripsSize;
        return reinterpret_cast<const WCollisionBarrier *>(dataStart + ind * 0x10); // sizeof(WCollisionBarrier)
    }

    const WCollisionStripSphere *GetStripSphere(unsigned int ind) const {
        const char *dataStart = reinterpret_cast<const char *>(&this[1]);
        const WCollisionStripSphere *stripSp = reinterpret_cast<const WCollisionStripSphere *>(dataStart);
        return &stripSp[ind];
    }

    unsigned short fNumStrips;         // offset 0x0, size 0x2
    unsigned short fStripsSize;        // offset 0x2, size 0x2
    unsigned short fNumEdges;          // offset 0x4, size 0x2
    unsigned short fEdgesSize;         // offset 0x6, size 0x2
    unsigned char fResolvedFlag;       // offset 0x8, size 0x1
    unsigned char fNumSurfaces;        // offset 0x9, size 0x1
    unsigned short fSurfacesSize;      // offset 0xA, size 0x2
    unsigned short fIntermediatObjInd; // offset 0xC, size 0x2
    short fFlags;                      // offset 0xE, size 0x2
};

// total size: 0x20
struct WCollisionBarrier {
    const WSurface &GetWSurface() const {
        // TODO why 0xC?
        return *reinterpret_cast<const WSurface *>(reinterpret_cast<const char *>(this) + 0xC);
    }

    const WCollisionBarrier *Next() const {
        return &this[1];
    }

    const UMath::Vector4 *GetPts() const {
        return fPts;
    }

    const UMath::Vector4 *GetPt(int ptInd) const {
        return &fPts[ptInd];
    }

    const float YBot() const {
        return fPts[0].y < fPts[1].y ? fPts[0].y : fPts[1].y;
    }

    const float YTop() const {
        return fPts[1].y < fPts[0].y ? fPts[0].y : fPts[1].y;
    }

    float GetInvXZLength() const {
        return fPts[1].w;
    }

    void GetNormal(UMath::Vector3 &norm) const {
        float invLen = GetInvXZLength();
        norm.x = (fPts[1].z - fPts[0].z) * invLen;
        norm.y = 0.0f;
        norm.z = -(fPts[1].x - fPts[0].x) * invLen;
    }

    void GetCenter(UMath::Vector4 &cp) const {
        cp.x = (fPts[0].x + fPts[1].x) * 0.5f;
        cp.y = (fPts[0].y + fPts[1].y) * 0.5f;
        cp.z = (fPts[0].z + fPts[1].z) * 0.5f;
    }

    float GetWidth() const {
        float rx = fPts[0].x - fPts[1].x;
        float rz = fPts[0].z - fPts[1].z;
        return UMath::Sqrt(rx * rx + rz * rz);
    }

    float GetHeight() const {
        return UMath::Abs(fPts[0].y - fPts[1].y);
    }

    float DistSq(const UMath::Vector3 &pt) const {
        float x1 = fPts[0].x;
        float z1 = fPts[0].z;
        float x2 = fPts[1].x;
        float z2 = fPts[1].z;
        float px = pt.x;
        float pz = pt.z;
        float u = (px - x1) * (x2 - x1) + (pz - z1) * (z2 - z1);
        float invLen = GetInvXZLength();
        u *= invLen * invLen;

        if (u < 0.0f) {
            return (x1 - px) * (x1 - px) + (z1 - pz) * (z1 - pz);
        } else if (u > 1.0f) {
            return (x2 - px) * (x2 - px) + (z2 - pz) * (z2 - pz);
        } else {
            float nearX = u * (x2 - x1) + x1 - px;
            float nearZ = u * (z2 - z1) + z1 - pz;
            return nearX * nearX + nearZ * nearZ;
        }
    }

    UMath::Vector4 fPts[2]; // offset 0x0, size 0x20
};

// total size: 0x28
struct WCollisionBarrierListEntry {
    WCollisionBarrier fB; // offset 0x0, size 0x20
#ifdef EA_BUILD_A124
    UCrc32 fSurfaceHash;
#else
    const Attrib::Collection *fSurfaceRef; // offset 0x20, size 0x4
#endif
    float fDistanceToSq; // offset 0x24, size 0x4

    WCollisionBarrierListEntry()
        : fB(), //
#ifdef EA_BUILD_A124
          fSurfaceHash(), //
#else
          fSurfaceRef(nullptr), //
#endif
          fDistanceToSq(0.0f) {
    }

    WCollisionBarrierListEntry(const WCollisionBarrier &b, const Attrib::Collection *surfHash, float disttosq)
        : fB(b), //
#ifdef EA_BUILD_A124
          fSurfaceHash(), // TODO
#else
          fSurfaceRef(surfHash), //
#endif
          fDistanceToSq(disttosq) {
    }

    bool operator<(const WCollisionBarrierListEntry &rhs) const {
        return fDistanceToSq < rhs.fDistanceToSq;
    }
};

struct WCollisionObject : public CollisionObject {
    // total size: 0x70
    enum Types {
        kBox = 0,
        kCylinder = 1,
    };

    const WSurface GetWSurface() const {
        return WSurface(fSurface.fSurface, fSurface.fFlags);
    }

    bool IsDynamic() const {
        return (fFlags & 1) != 0;
    }

    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;
};

// total size: 0x40
struct WCollisionInstance : public CollisionInstance {
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;
    void CalcPosition(UMath::Vector3 &pos) const;
    const char *GetName() const;

    bool NeedsCrossProduct() const {
        return (fFlags & 3) != 0;
    }

    float CalcSphericalRadius() const;

    bool IsYVecNotUp() const {
        return (fFlags & 1) != 0;
    }

    bool IsDynamic() const {
        return (fFlags & 2) != 0;
    }
};

#endif
