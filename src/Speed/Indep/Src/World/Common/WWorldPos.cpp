#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

const WSurface WSurface::kNull;

void WSurface::InitSystem() {}

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

    bool faceChanged = fFaceValid && WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&fFace.fPt0));

    if (!faceChanged || !quitIfOnSameFace) {
        if (instList != nullptr) {
            FindClosestFaceInternal(*instList, pt);
        } else {
            WCollisionInstanceCacheList internalInstList;
            internalInstList.reserve(16);
            WCollisionMgr(0, 3).GetInstanceList(internalInstList, pt, 0.0f,
#ifdef EA_BUILD_A124
                                                false,
#endif
                                                true);
            FindClosestFaceInternal(internalInstList, pt);
        }
        return true;
    }

    bool onSameFace = !faceChanged;
    return onSameFace;
}

bool WWorldPos::FindClosestFaceInternal(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt) {
    fFaceValid = 0;
    UMath::Matrix4 mat;
    UMath::Vector3 startPt;
    UMath::Vector3 endPt;
    bool matValid = false;
    bool foundFace = false;
    float bestDist = 1e38f;

    for (WCollisionInstanceCacheList::const_iterator iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        WCollisionTri face;
        float dist;
        const WCollisionInstance &cInst = **iIter;

        if (!cInst.NeedsCrossProduct()) {
            if (WCollisionMgr(0, 3).FindFaceInCInst(pt, cInst, face, dist)) {
                foundFace = true;
                if (dist < bestDist) {
                    fFaceValid = 1;
                    fFace = face;
                    FindSurface(*cInst.fCollisionArticle);
                    bestDist = dist;
                }
            }
        } else {
            if (!matValid) {
                endPt = startPt = pt;
                startPt.y += fYOffset;
                endPt.y = pt.y - 750.0f;
                WWorldMath::MakeSegSpaceMatrix(startPt, endPt, mat);
                matValid = true;
            }
            if (WCollisionMgr(0, 3).FindFaceInCInst(mat, endPt, **iIter, face, dist)) {
                foundFace = true;
                dist -= fYOffset;
                if (dist < bestDist) {
                    const WCollisionInstance &cInst = **iIter;
                    fFaceValid = 1;
                    fFace = face;
                    FindSurface(*cInst.fCollisionArticle);
                    bestDist = dist;
                }
            }
        }
    }

    return foundFace;
}

bool WWorldPos::FindClosestFace(const WCollisionTriList &triList, const UMath::Vector3 &ipt, bool quitIfOnSameFace) {
    bool foundFace = false;
    bool onSameFace = false;
    fUsageCount++;

    UMath::Vector3 pt = ipt;
    if (fFaceValid) {
        onSameFace = WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&fFace.fPt0));
    }
    float bestDist = 1e38f;

    if (!onSameFace || !quitIfOnSameFace) {
        fFaceValid = 0;
        pt.y += fYOffset;

        for (WCollisionTriList::const_iterator bIter = triList.begin(); bIter != triList.end(); ++bIter) {
            if (foundFace && !fFace.fSurface.HasFlag(4))
                break;

            const WCollisionTriBlock &triBlock = **bIter;
            for (WCollisionTriBlock::const_iterator iIter = triBlock.begin(); iIter != triBlock.end(); ++iIter) {
                const WCollisionTri &tri = *iIter;

                if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&tri.fPt0))) {
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

                    float dist = pt.y - WWorldMath::GetPlaneY(norm, tri.fPt0, pt);
                    if (dist < bestDist && -1.0f < dist) {
                        fFaceValid = 1;
                        fFace = tri;
                        foundFace = true;
                        fSurface = reinterpret_cast<const Attrib::Collection *>(tri.fSurfaceRef);
                        bestDist = dist;
                        if (!fFace.fSurface.HasFlag(4))
                            break;
                    }
                }
            }
        }
    }

    return !onSameFace;
}

bool WWorldPos::FindClosestFace(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, const UMath::Vector3 &endPt) {
    fUsageCount++;
    fFaceValid = 0;

    UMath::Matrix4 mat;
    if (!WWorldMath::MakeSegSpaceMatrix(pt, endPt, mat)) {
        return false;
    }
    float bestDist = 1e38f;
    fFaceValid = 0;

    for (const WCollisionInstance *const *iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        WCollisionTri face;
        float dist;
        if (WCollisionMgr(0, 3).FindFaceInCInst(mat, endPt, **iIter, face, dist)) {
            if (dist < bestDist) {
                const WCollisionInstance &cInst = **iIter;
                fFaceValid = 1;
                fFace = face;
                FindSurface(*cInst.fCollisionArticle);
                bestDist = dist;
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
