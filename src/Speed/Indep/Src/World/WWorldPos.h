#ifndef WORLD_WWORLDPOS_H
#define WORLD_WWORLDPOS_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"

// total size: 0x3C
class WWorldPos {
  public:
    USE_FASTALLOC(WWorldPos);

    WWorldPos(float yOffset) {
        fYOffset = yOffset;
        fFaceValid = 0;
        fMissCount = 0;
        fUsageCount = 0;
        fFace.fPt0 = UMath::Vector3::kZero;
        fFace.fPt1 = UMath::Vector3::kZero;
        fFace.fPt2 = UMath::Vector3::kZero;
        fSurface = nullptr;
    }

    ~WWorldPos() {}

    bool Update(const UMath::Vector3 &pos, UMath::Vector4 &dest, bool usecache, const WCollider *collider, bool keep_valid);

    // bool OffEdge() const {}

    bool OnValidFace() const {
        return fFaceValid;
    }

    void ForceFaceValidity() {
        fFaceValid = 1;
    }

    // const WSurface &Surface() const {}

    bool FindClosestFace(const WCollider *collider, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    bool FindClosestFace(const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    // this one is supposed to be private
    bool FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, const UMath::Vector3 &endPt);

    void SetTolerance(float liftAmount) {
        fYOffset = liftAmount;
    }

    void UNormal(UMath::Vector3 *norm) const {
        if (fFaceValid) {
            fFace.GetNormal(norm);
            if (norm->y < 0.0f) {
                norm->y = -norm->y;
                norm->x = -norm->x;
                norm->z = -norm->z;
            }
            if (0.9999f <= norm->y) {
                norm->y = 0.9999f;
            }
        } else {
            norm->z = 0.0f;
            norm->x = 0.0f;
            norm->y = 1.0f;
        }
    }

    void UNormal(UMath::Vector4 *norm) const {
        UNormal(&UMath::Vector4To3(*norm));
        norm->w = 0.0f;
    }

    float HeightAtPoint(const UMath::Vector3 &pt) const;

    const UMath::Vector4 &FacePoint(int ptInd) const {
        return reinterpret_cast<const UMath::Vector4 *>(&fFace)[ptInd];
    }

    void MakeFaceAtPoint(const UMath::Vector3 &inPoint);

    const Attrib::Collection *GetSurface() const {
        return fSurface;
    }

  private:
    void FindSurface(const WCollisionArticle &cArt);
    bool FindClosestFaceInternal(const WCollisionInstanceCacheList *instList, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    bool FindClosestFaceInternal(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt);
    bool FindClosestFace(const WCollisionTriList &triList, const UMath::Vector3 &ipt, bool quitIfOnSameFace);

    // bool FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, bool quitIfOnSameFace) {}

    WCollisionTri fFace;           // offset 0x0, size 0x30
    unsigned int fFaceValid : 1;   // offset 0x30, size 0x4
    unsigned int fMissCount : 15;  // offset 0x30, size 0x4
    unsigned int fUsageCount : 16; // offset 0x30, size 0x4
#ifdef EA_BUILD_A124
    UCrc32 fSurfaceHash;
#endif
    float fYOffset;                     // offset 0x34, size 0x4
    const Attrib::Collection *fSurface; // offset 0x38, size 0x4
};

#endif
