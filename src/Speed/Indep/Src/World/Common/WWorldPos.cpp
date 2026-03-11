#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

bool WWorldPos::FindClosestFace(const WCollider *collider, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace) {
    if (collider != nullptr) {
        return FindClosestFace(collider->GetTriList(), ptRaw, quitIfOnSameFace);
    }
    return FindClosestFace(ptRaw, quitIfOnSameFace);
}

bool WWorldPos::FindClosestFace(const UMath::Vector3 &ptRaw, bool quitIfOnSameFace) {
    return FindClosestFaceInternal(static_cast<const WCollisionInstanceCacheList *>(nullptr), ptRaw, quitIfOnSameFace);
}

bool WWorldPos::Update(const UMath::Vector3 &pos, UMath::Vector4 &dest, bool usecache, const WCollider *collider, bool keep_valid) {
    bool was_valid = OnValidFace();
    bool recalcNormal = FindClosestFace(collider, pos, usecache);
    if (!OnValidFace()) {
        fSurface = SimSurface::kNull.GetConstCollection();
        if (keep_valid && was_valid) {
            ForceFaceValidity();
        } else {
            dest.w = -1.0f;
            return false;
        }
    }
    if (recalcNormal) {
        UNormal(&dest);
    }
    dest.w = (FacePoint(0).x - pos.x) * dest.x + (FacePoint(0).y - pos.y) * dest.y + (FacePoint(0).z - pos.z) * dest.z;
    return true;
}

float WWorldPos::HeightAtPoint(const UMath::Vector3 &pt) const {
    if (OnValidFace()) {
        UMath::Vector3 norm;
        UNormal(&norm);
        return WWorldMath::GetPlaneY(norm, UMath::Vector4To3(FacePoint(0)), pt);
    }
    return 0.0f;
}

void WWorldPos::FindSurface(const WCollisionArticle &cArt) {
    fSurface = cArt.GetSurface(fFace.fSurface.Surface());
}
