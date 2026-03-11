#ifndef WORLD_WCOLLISION_H
#define WORLD_WCOLLISION_H

#include <cstring>
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct WSurface : CollisionSurface {
    static void InitSystem();

    WSurface() {
        fSurface = 0;
        fFlags = 0;
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

    bool HasFlag(unsigned char flag) const {
        return (fFlags & flag) != 0;
    }
};

struct WCollisionBarrier;

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

    inline bool IsDynamic() const {
        return (fFlags & 2) != 0;
    }

    float CalcSphericalRadius() const;
    void CalcPosition(UMath::Vector3 &pos) const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;
};

#endif
