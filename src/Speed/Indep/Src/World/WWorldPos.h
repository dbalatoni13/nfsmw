#ifndef WORLD_WWORLDPOS_H
#define WORLD_WWORLDPOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"

// total size: 0x3C
class WWorldPos {
  public:
    void MakeFaceAtPoint(const UMath::Vector3 &inPoint);
    bool FindClosestFace(const WCollider *collider, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    bool FindClosestFace(const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    bool FindClosestFaceInternal(const WCollisionInstanceCacheList *instList, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace);
    bool FindClosestFaceInternal(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt);
    bool FindClosestFace(const WCollisionTriList &triList, const UMath::Vector3 &ipt, bool quitIfOnSameFace);
    bool FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, const UMath::Vector3 &endPt);
    bool Update(const UMath::Vector3 &pos, UMath::Vector4 &dest, bool usecache, const WCollider *collider, bool keep_valid);
    float HeightAtPoint(const UMath::Vector3 &pt) const;
    void FindSurface(const WCollisionArticle &cArt);

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    WWorldPos(float yOffset) {
        this->fFaceValid = 0;
        this->fMissCount = 0;
        this->fUsageCount = 0;
        this->fYOffset = yOffset;
        this->fSurface = nullptr;
    }

    ~WWorldPos() {}

    // bool OffEdge() const {}

    // bool OnValidFace() const {}

    void ForceFaceValidity() {}

    // const WSurface &Surface() const {}

    void SetTolerance(float liftAmount) {
        fYOffset = liftAmount;
    }

    void UNormal(UMath::Vector3 *norm) const {}

    void UNormal(UMath::Vector4 *norm) const {}

    // const UMath::Vector4 &FacePoint(int ptInd) const {}

    const Attrib::Collection *GetSurface() const {
        return fSurface;
    }

    // bool FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, bool quitIfOnSameFace) {}

  private:
    WCollisionTri fFace;                // offset 0x0, size 0x30
    unsigned int fFaceValid : 1;        // offset 0x30, size 0x4
    unsigned int fMissCount : 15;       // offset 0x30, size 0x4
    unsigned int fUsageCount : 16;      // offset 0x30, size 0x4
    float fYOffset;                     // offset 0x34, size 0x4
    const Attrib::Collection *fSurface; // offset 0x38, size 0x4
};

#endif
