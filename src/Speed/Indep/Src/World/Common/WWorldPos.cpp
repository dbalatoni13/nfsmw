#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
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

bool WWorldPos::FindClosestFaceInternal(const WCollisionInstanceCacheList *instList, const UMath::Vector3 &ptRaw, bool quitIfOnSameFace) {
    fUsageCount++;
    UMath::Vector3 pt = ptRaw;
    pt.y += fYOffset;

    bool faceChanged = false;
    if (fFaceValid) {
        if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&fFace))) {
            faceChanged = true;
        }
    }

    if (faceChanged && quitIfOnSameFace) {
        return !faceChanged;
    }

    if (instList != nullptr) {
        FindClosestFaceInternal(*instList, pt);
    } else {
        WCollisionInstanceCacheList localList;
        localList.reserve(16);
        WCollisionMgr collMgr(0, 3);
        collMgr.GetInstanceList(localList, pt, 0.0f, true);
        FindClosestFaceInternal(localList, pt);
    }
    return true;
}

bool WWorldPos::FindClosestFaceInternal(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt) {
    fFaceValid = 0;
    bool matComputed = false;
    bool found = false;
    float bestDist = 100000.0f;
    UMath::Matrix4 segMat;
    UMath::Vector3 startPt;
    UMath::Vector3 endPt;

    for (const WCollisionInstance *const *it = instList.begin(); it != instList.end(); ++it) {
        WCollisionTri tri;
        tri.fSurface.fSurface = 0;
        const WCollisionInstance *inst = *it;
        float dist;

        if (!inst->NeedsCrossProduct()) {
            WCollisionMgr collMgr(0, 3);
            if (collMgr.FindFaceInCInst(pt, *inst, tri, dist)) {
                found = true;
                if (dist < bestDist) {
                    fFaceValid = 1;
                    fFace = tri;
                    FindSurface(*inst->fCollisionArticle);
                    bestDist = dist;
                }
            }
        } else {
            if (!matComputed) {
                startPt.x = pt.x;
                startPt.z = pt.z;
                matComputed = true;
                endPt.y = pt.y - 100.0f;
                startPt.y = pt.y + fYOffset;
                endPt.x = startPt.x;
                endPt.z = startPt.z;
                WWorldMath::MakeSegSpaceMatrix(startPt, endPt, segMat);
            }
            WCollisionMgr collMgr2(0, 3);
            if (collMgr2.FindFaceInCInst(segMat, endPt, *inst, tri, dist)) {
                found = true;
                dist = dist - fYOffset;
                if (dist < bestDist) {
                    fFaceValid = 1;
                    fFace = tri;
                    FindSurface(*inst->fCollisionArticle);
                    bestDist = dist;
                }
            }
        }
    }

    return found;
}

bool WWorldPos::FindClosestFace(const WCollisionTriList &triList, const UMath::Vector3 &ipt, bool quitIfOnSameFace) {
    bool foundFace = false;
    bool onSameFace = false;
    fUsageCount++;
    float bestDist = 100000.0f;
    UMath::Vector3 pt;
    pt.x = ipt.x;
    pt.z = ipt.z;
    pt.y = ipt.y;

    if (fFaceValid) {
        onSameFace = WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&fFace));
    }

    if (onSameFace && quitIfOnSameFace) {
        return false;
    }

    fFaceValid = 0;
    pt.y = pt.y + fYOffset;

    for (WCollisionTriBlock *const *bIter = triList.begin(); bIter != triList.end(); ++bIter) {
        if (foundFace && !(fFace.fSurface.Surface() & 4)) break;

        const WCollisionTriBlock &triBlock = **bIter;
        for (const WCollisionTri *iIter = triBlock.begin(); iIter != triBlock.end(); ++iIter) {
            const WCollisionTri &tri = *iIter;

            if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&tri))) {
                UMath::Vector3 norm;
                tri.GetNormal(&norm);

                if (norm.y < 0.0f) {
                    norm.y = -norm.y;
                    norm.x = -norm.x;
                    norm.z = -norm.z;
                }
                if (0.9999f <= norm.y) {
                    norm.y = 0.9999f;
                }

                float y = WWorldMath::GetPlaneY(norm, tri.fPt0, pt);
                float dist = pt.y - y;
                if (dist < bestDist && -100000.0f < dist) {
                    fFaceValid = 1;
                    fFace = tri;
                    foundFace = true;
                    fSurface = reinterpret_cast<const Attrib::Collection *>(tri.fSurfaceRef);
                    bestDist = dist;
                    if (!(fFace.fSurface.Surface() & 4)) break;
                }
            }
        }
    }

    return false;
}

bool WWorldPos::FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, const UMath::Vector3 &endPt) {
    fUsageCount++;
    fFaceValid = 0;

    UMath::Matrix4 segMat;
    if (WWorldMath::MakeSegSpaceMatrix(pt, endPt, segMat)) {
        float bestDist = 100000.0f;
        fFaceValid = 0;

        for (const WCollisionInstance *const *it = instList.begin(); it != instList.end(); ++it) {
            WCollisionTri tri;
            tri.fSurface.fSurface = 0;
            float dist;
            WCollisionMgr collMgr(0, 3);
            if (collMgr.FindFaceInCInst(segMat, endPt, **it, tri, dist)) {
                if (dist < bestDist) {
                    fFaceValid = 1;
                    fFace = tri;
                    FindSurface(*(*it)->fCollisionArticle);
                    bestDist = dist;
                }
            }
        }
    }

    return false;
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
