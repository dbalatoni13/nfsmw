#include "Speed/Indep/Src/World/WWorldPos.h"

bool WWorldPos::FindClosestFace(const WCollider *collider, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace) {
    if (collider != nullptr) {
        return FindClosestFace(collider->GetTriList(), ptRaw, quitIfOnSameFace);
    }
    return FindClosestFace(ptRaw, quitIfOnSameFace);
}

bool WWorldPos::FindClosestFace(const UMath::Vector3 &ptRaw, bool quitIfOnSameFace) {
    return FindClosestFaceInternal(static_cast<const WCollisionInstanceCacheList *>(nullptr), ptRaw, quitIfOnSameFace);
}

void WWorldPos::FindSurface(const WCollisionArticle &cArt) {
    fSurface = cArt.GetSurface(fFace.fSurface.Surface());
}
