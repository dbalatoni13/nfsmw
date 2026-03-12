#ifndef WORLD_WCOLLISION_H
#define WORLD_WCOLLISION_H

#include <cstring>
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct WCollisionPackedVert {
    short x;
    short y;
    short z;
    CollisionSurface surface;
};

struct WSurface : CollisionSurface {
    static void InitSystem();

    WSurface() {
        fSurface = 0;
        fFlags = 0;
    }
    WSurface(const CollisionSurface &surface) {
        fSurface = surface.fSurface;
        fFlags = surface.fFlags;
    }
    WSurface(unsigned char surface, unsigned char flags) {
        fSurface = surface;
        fFlags = flags;
    }

    unsigned int Surface() const {
        return fSurface;
    }

    unsigned char &FlagsRef() {
        return fFlags;
    }

    unsigned char Flags() const {
        return fFlags;
    }

    bool HasFlag(unsigned char flag) const {
        return (fFlags & flag) != 0;
    }
};

struct WCollisionBarrier;
struct WCollisionTri;

struct WCollisionStripSphere {
    // total size: 0x10
    unsigned int Offset() const {
        return static_cast<unsigned int>(fOffset) + 0x10;
    }

    UMath::Vector3 fPos;         // offset 0x0, size 0xC
    unsigned short fRadius;      // offset 0xC, size 0x2
    unsigned short fOffset;      // offset 0xE, size 0x2
};

struct WCollisionStrip {
    // total size: 0x1
    const WCollisionPackedVert *Verts() const {
        return reinterpret_cast<const WCollisionPackedVert *>(this);
    }

    unsigned int NumVerts() const {
        const WCollisionPackedVert *v = Verts();
        return *reinterpret_cast<const unsigned short *>(&v[0].surface);
    }

    unsigned int NumTris() const {
        return NumVerts() - 2;
    }

    unsigned int Flags() const {
        return *reinterpret_cast<const unsigned short *>(&Verts()[1].surface);
    }

    void MakeFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const;
    void MakeNextFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const;
};

struct WCollisionArticle {
    // total size: 0x10
    void Resolve();

    const Attrib::Collection *GetSurface(unsigned int ind) const {
        unsigned int ref = fStripsSize + 0x10;
        const char *dataStart = reinterpret_cast<const char *>(this) + ref + fEdgesSize;
        return reinterpret_cast<const Attrib::Collection *>(
            *reinterpret_cast<const unsigned int *>(dataStart + ind * 4));
    }

    inline const WCollisionBarrier *GetBarrier(unsigned int ind) const;

    const WCollisionStripSphere *GetStripSphere(unsigned int ind) const {
        const char *dataStart = reinterpret_cast<const char *>(this) + 0x10;
        return reinterpret_cast<const WCollisionStripSphere *>(dataStart + ind * sizeof(WCollisionStripSphere));
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
        return *reinterpret_cast<const WSurface *>(
            reinterpret_cast<const char *>(this) + 0xC);
    }

    const WCollisionBarrier *Next() const {
        return this + 1;
    }

    const UMath::Vector4 *GetPts() const {
        return fPts;
    }

    const UMath::Vector4 *GetPt(int ptInd) const {
        return &fPts[ptInd];
    }

    const float YBot() const {
        return fPts[0].y;
    }

    const float YTop() const {
        return fPts[1].y;
    }

    float GetInvXZLength() const {
        return fPts[1].w;
    }

    void GetNormal(UMath::Vector3 &norm) const {
        float invLen = GetInvXZLength();
        norm.x = (fPts[1].z - fPts[0].z) * invLen;
        norm.y = 0.0f;
        norm.z = (fPts[0].x - fPts[1].x) * invLen;
    }

    void GetCenter(UMath::Vector4 &cp) const {
        cp.x = (fPts[0].x + fPts[1].x) * 0.5f;
        cp.y = (fPts[0].y + fPts[1].y) * 0.5f;
        cp.z = (fPts[0].z + fPts[1].z) * 0.5f;
    }

    float GetWidth() const {
        float rz = fPts[0].z - fPts[1].z;
        float rx = fPts[0].x - fPts[1].x;
        return UMath::Sqrt(rx * rx + rz * rz);
    }

    float GetHeight() const {
        return UMath::Abs(fPts[0].y - fPts[1].y);
    }

    float DistSq(const UMath::Vector3 &pt) const {
        float invLen = GetInvXZLength();
        float z1 = fPts[0].z;
        float z2 = fPts[1].z;
        float pz = pt.z;
        float x1 = fPts[0].x;
        float x2 = fPts[1].x;
        float px = pt.x;
        float u = ((pz - z1) * (z2 - z1) + (px - x1) * (x2 - x1)) * invLen * invLen;
        float nearZ;
        float nearX;
        if (u < 0.0f) {
            nearZ = z1 - pz;
            nearX = x1 - px;
        } else if (u > 1.0f) {
            nearZ = z2 - pz;
            nearX = x2 - px;
        } else {
            nearZ = u * (z2 - z1) + z1 - pz;
            nearX = u * (x2 - x1) + x1 - px;
        }
        return nearX * nearX + nearZ * nearZ;
    }

    UMath::Vector4 fPts[2]; // offset 0x0, size 0x20
};

inline const WCollisionBarrier *WCollisionArticle::GetBarrier(unsigned int ind) const {
    const char *dataStart = reinterpret_cast<const char *>(this) + (fStripsSize + 0x10);
    return reinterpret_cast<const WCollisionBarrier *>(dataStart + ind * 0x20);
}

// total size: 0x28
struct WCollisionBarrierListEntry {
    WCollisionBarrier fB;                  // offset 0x0, size 0x20
    const Attrib::Collection *fSurfaceRef; // offset 0x20, size 0x4
    float fDistanceToSq;                   // offset 0x24, size 0x4

    WCollisionBarrierListEntry()
        : fB(),                 //
          fSurfaceRef(nullptr), //
          fDistanceToSq(0.0f) {}

    WCollisionBarrierListEntry(const WCollisionBarrier &b, const Attrib::Collection *surfHash, float disttosq)
        : fB(b),                   //
          fSurfaceRef(surfHash),   //
          fDistanceToSq(disttosq) {}

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

    bool IsDynamic() const { return (fFlags & 1) != 0; }

    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;
};

extern signed char SceneryGroupEnabledTable[];

inline int IsSceneryGroupEnabled(int group_number) {
    return SceneryGroupEnabledTable[group_number];
}

struct WCollisionInstance : public CollisionInstance {
    // total size: 0x40

    inline bool NeedsCrossProduct() const {
        return (fFlags & 3) != 0;
    }

    inline bool IsYVecNotUp() const {
        return (fFlags & 1) != 0;
    }

    inline bool IsDynamic() const {
        return (fFlags & 2) != 0;
    }

    float CalcSphericalRadius() const;
    void CalcPosition(UMath::Vector3 &pos) const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;
};

#endif
