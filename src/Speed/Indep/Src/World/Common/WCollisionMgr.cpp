#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

unsigned short WCollisionMgr::fIterCount = 0;

inline void NearPtLine(const UMath::Vector3 &pt, const UMath::Vector3 &p0, float den, const UMath::Vector3 &diffVec, UMath::Vector3 &nearPt) {
    float u = ((pt.x - p0.x) * diffVec.x + (pt.y - p0.y) * diffVec.y + (pt.z - p0.z) * diffVec.z) * den;
    u = UMath::Min(u, 1.0f);
    u = UMath::Max(u, 0.0f);
    nearPt.x = u * diffVec.x + p0.x;
    nearPt.y = u * diffVec.y + p0.y;
    nearPt.z = u * diffVec.z + p0.z;
}

inline void NearPtLinePerSeg(const UMath::Vector3 &p0, const UMath::Vector3 &p1, float &invDen, UMath::Vector3 &diffVec) {
    UMath::Sub(p1, p0, diffVec);
    float ud = diffVec.x * diffVec.x + diffVec.y * diffVec.y + diffVec.z * diffVec.z;

    invDen = 1.0f / ud;
}

inline void NearPtLinePerSegXZ(const UMath::Vector3 &p0, const UMath::Vector3 &p1, float &invDen, UMath::Vector3 &diffVec) {
    UMath::Sub(p1, p0, diffVec);
    diffVec.y = 0.0f;
    float ud = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
    invDen = 0.0f;
    if (0.0f < ud) {
        invDen = 1.0f / ud;
    }
}

inline void NearPtLineXZ(const UMath::Vector3 &pt, const UMath::Vector3 &p0, float den, const UMath::Vector3 &diffVec, UMath::Vector3 &nearPt) {
    float u = ((pt.x - p0.x) * diffVec.x + (pt.z - p0.z) * diffVec.z) * den;
    u = UMath::Max(UMath::Min(u, 1.0f), 0.0f);
    nearPt.x = u * diffVec.x + p0.x;
    nearPt.z = u * diffVec.z + p0.z;
}

bool WCollisionMgr::FindFaceInTriStrip(const UMath::Vector3 &pt, const WCollisionStripSphere *sp, const WCollisionStrip *strip,
                                       WCollisionTri &retFace) {
    if (!this->StripPassesExclusion(*strip)) {
        return false;
    }

    const WCollisionPackedVert *packedFaceVert = strip->Verts();
    int numTris = strip->NumTris();
    WCollisionTri &face = retFace;
    strip->MakeFace(0, sp->fPos, face);

    if (strip->Flags() & 2) {
        for (int i = 0; i < numTris;) {
            const WSurface &surf = face.fSurface;
            if (!surf.HasFlag(8)) {
                // TODO HUH?
                if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&face.fPt0))) {
                    if (this->SurfacePassesExclusion(surf)) {
                        if (face.MinY() < pt.y + 1.0f) {
                            return true;
                        }
                    }
                }
            }
            ++i;
            if (i >= numTris)
                break;
            strip->MakeNextFace(i, sp->fPos, face);
        }
    } else {
        bool rightFlag = (strip->Flags() & 1) != 0;
        strip->MakeFace(0, sp->fPos, face);
        for (int i = 0; i < numTris;) {
            if (rightFlag) {
                const WSurface &surf = face.fSurface;
                if (!surf.HasFlag(8)) {
                    if (WWorldMath::InTriR(pt, reinterpret_cast<const UMath::Vector4 *>(&face.fPt0))) {
                        if (this->SurfacePassesExclusion(surf)) {
                            return true;
                        }
                    }
                }
            } else {
                const WSurface &surf = face.fSurface;
                if (!surf.HasFlag(8)) {
                    if (WWorldMath::InTriL(pt, reinterpret_cast<const UMath::Vector4 *>(&face.fPt0))) {
                        if (this->SurfacePassesExclusion(surf)) {
                            return true;
                        }
                    }
                }
            }
            ++i;
            rightFlag = !rightFlag;
            if (i >= numTris)
                break;
            strip->MakeNextFace(i, sp->fPos, face);
        }
    }

    return false;
}

static void CalcCollisionFaceNormal(UMath::Vector3 *norm, UMath::Vector4 *facePts) {
    UMath::Vector3 vecZ;
    UMath::Vector3 vecX;
    UMath::Vector3 normal;

    vecX.x = facePts[1].x - facePts[0].x;
    vecX.y = facePts[1].y - facePts[0].y;
    vecX.z = facePts[1].z - facePts[0].z;

    vecZ.x = facePts[0].x - facePts[2].x;
    vecZ.y = facePts[0].y - facePts[2].y;
    vecZ.z = facePts[0].z - facePts[2].z;
    v3crossprod(&vecX, &vecZ, &normal);
    if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
        norm->x = 0.0f;
        norm->z = 0.0f;
        norm->y = 1.0f;
    } else {
        v3unit(&normal, norm);
    }
    if (norm->y < 0.0f) {
        norm->y = -norm->y;
        norm->x = -norm->x;
        norm->z = -norm->z;
    }
    if (norm->y >= 0.9999f) {
        norm->y = 0.9999f;
    }
}

bool WCollisionMgr::FindFaceInTriStrip(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &pt, const WCollisionStripSphere *sp,
                                       const WCollisionStrip *strip, float &faceY, WCollisionTri &retFace) {
    if (!this->StripPassesExclusion(*strip)) {
        return false;
    }

    bool foundFace = false;
    int retFaceInd = 0;
    int numTris = strip->NumTris();
    WCollisionTri face;
    strip->MakeFace(0, sp->fPos, face);

    float bestFaceY = 1e38f;

    UMath::RotateTranslate(face.fPt0, vectorMat, face.fPt0);
    UMath::RotateTranslate(face.fPt1, vectorMat, face.fPt1);

    for (int i = 0; i < numTris;) {
        UMath::RotateTranslate(face.fPt2, vectorMat, face.fPt2);
        if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&face))) {
            UMath::Vector3 norm;
            CalcCollisionFaceNormal(&norm, reinterpret_cast<UMath::Vector4 *>(&face));
            faceY = pt.y - WWorldMath::GetPlaneY(norm, face.fPt2, pt);
            if (faceY < bestFaceY && -1.0f < faceY && this->SurfacePassesExclusion(face.fSurface)) {
                bestFaceY = faceY;
                retFaceInd = i;
                foundFace = true;
            }
        }
        ++i;
        if (i >= numTris)
            break;
        strip->MakeNextFace(i, sp->fPos, face);
    }

    if (foundFace) {
        strip->MakeFace(retFaceInd, sp->fPos, retFace);
    }
    faceY = bestFaceY;
    return foundFace;
}

inline void MakeWorldSpaceFace(WCollisionTri &worldFace, const WCollisionTri &localFace, const UMath::Matrix4 &invMat) {
    UTransform t(invMat);
#ifdef EA_BUILD_A124
    UMath::OrthoInverse(t.fTransform);
#else
    OrthoInverse(t.fTransform); // TODO why is this not inlined?
#endif
    worldFace.fSurface = localFace.fSurface;
    worldFace.fFlags = localFace.fFlags;
    UMath::RotateTranslate(localFace.fPt0, t.fTransform, worldFace.fPt0);
    UMath::RotateTranslate(localFace.fPt1, t.fTransform, worldFace.fPt1);
    UMath::RotateTranslate(localFace.fPt2, t.fTransform, worldFace.fPt2);
}

bool WCollisionMgr::FindFaceInCInst(const UMath::Vector3 &pt, const WCollisionInstance &cInst, WCollisionTri &retFace, float &retDist) {
    UMath::Vector3 tpt;
    UMath::Matrix4 invMat;

    cInst.MakeMatrix(invMat, true);
    UMath::RotateTranslate(pt, invMat, tpt);

    const WCollisionArticle *cArt = cInst.fCollisionArticle;

    if (cArt == nullptr || cArt->fNumStrips == 0 || tpt.x > cInst.fInvMatRow0Width.w || tpt.x < -cInst.fInvMatRow0Width.w ||
        tpt.z > cInst.fInvMatRow2Length.w || tpt.z < -cInst.fInvMatRow2Length.w) {
        return false;
    }

    WCollisionTri retVal;
    float leastYDist = 1e38f;
    const WCollisionStripSphere *sp = cArt->GetStripSphere(0);
    bool foundFace = false;

    for (int i = 0; i < cArt->fNumStrips; ++sp, ++i) {
        UMath::Vector3 diffVec;
        UMath::Sub(sp->fPos, tpt, diffVec);
        float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
        float radius = static_cast<float>(static_cast<int>(sp->fRadius)) * (1.0f / 16.0f);
        float radsSq = radius * radius;
        if (dSq < radsSq) {
            const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(reinterpret_cast<const char *>(cArt) + sp->Offset());
            WCollisionTri face;
            if (this->FindFaceInTriStrip(tpt, sp, strip, face)) {
                float dist;
                UMath::Vector3 norm;
                face.GetNormal(&norm);
                if (norm.y < 0.0f) {
                    norm.y = -norm.y;
                    norm.x = -norm.x;
                    norm.z = -norm.z;
                }
                if (norm.y >= 0.9999f) {
                    norm.y = 0.9999f;
                }
                dist = tpt.y - WWorldMath::GetPlaneY(norm, face.fPt0, tpt);

                bool minYBelowTestPoint = (face.MinY() - 0.5f) < tpt.y;

                if (cInst.IsYVecNotUp()) {
                    minYBelowTestPoint = (face.MinY() - 0.5f) > tpt.y;
                }

                if (minYBelowTestPoint && 0.0f < dist && dist < leastYDist) {
                    foundFace = true;
                    leastYDist = dist;
                    retVal = face;
                    retDist = dist;
                }
            }
        }
    }

    if (foundFace) {
        MakeWorldSpaceFace(retFace, retVal, invMat);
        return true;
    }

    return false;
}

// UNSOLVED stack issue
bool WCollisionMgr::FindFaceInCInst(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &endPt, const WCollisionInstance &cInst,
                                    WCollisionTri &retFace, float &retDist) {
    UMath::Matrix4 invMat;
    cInst.MakeMatrix(invMat, true);

    UTransform mat(invMat);
#ifdef EA_BUILD_A124
    UMath::OrthoInverse(mat.fTransform);
#else
    OrthoInverse(mat.fTransform);
#endif

    const UMath::Vector3 &startPt = *reinterpret_cast<const UMath::Vector3 *>(&vectorMat[3]);

    UTransform vecMatInv(vectorMat);
#ifdef EA_BUILD_A124
    UMath::OrthoInverse(vecMatInv.fTransform);
#else
    OrthoInverse(vecMatInv.fTransform);
#endif

    UMath::Matrix4 combinedMat;
    UMath::Mult(mat.fTransform, vecMatInv.fTransform, combinedMat);

    const WCollisionArticle *cArt = cInst.fCollisionArticle;
    if (cArt == nullptr) {
        return false;
    }

    WCollisionTri retVal;
    float leastYDist = 1e38f;

    UMath::Vector3 tp0;
    UMath::Vector3 tp1;

    UMath::RotateTranslate(startPt, invMat, tp0);

    UMath::RotateTranslate(endPt, invMat, tp1);
    const WCollisionStripSphere *sp = cArt->GetStripSphere(0);
    bool foundFace = false;

    float invDen;
    UMath::Vector3 npVec;
    NearPtLinePerSeg(tp0, tp1, invDen, npVec);

    for (int i = 0; i < cArt->fNumStrips; ++i, ++sp) {
        float radius = static_cast<float>(static_cast<int>(sp->fRadius)) * (1.0f / 16.0f);
        float radsSq = radius * radius;

        UMath::Vector3 diffVec;
        UMath::Vector3 nearPt;
        NearPtLine(sp->fPos, tp0, invDen, npVec, nearPt);
        UMath::Sub(sp->fPos, nearPt, diffVec);
        float dSq = diffVec.x * diffVec.x + diffVec.y * diffVec.y + diffVec.z * diffVec.z;

        if (dSq < radsSq) {
            const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(reinterpret_cast<const char *>(cArt) + sp->Offset());
            float faceY;
            WCollisionTri face;
            if (this->FindFaceInTriStrip(combinedMat, UMath::Vector3::kZero, sp, strip, faceY, face)) {
                if (0.0f < faceY && faceY < leastYDist) {
                    float dist = faceY;
                    leastYDist = dist;
                    retVal = face;
                    retDist = dist;
                    foundFace = true;
                }
            }
        }
    }

    if (foundFace) {
        MakeWorldSpaceFace(retFace, retVal, invMat);
        return true;
    }

    return false;
}

bool WCollisionMgr::GetWorldHeightAtPoint(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal) {
    WWorldPos temp(0.025f);
    temp.FindClosestFace(pt, true);
    if (temp.OnValidFace()) {
        UMath::Vector3 norm;
        temp.UNormal(&norm);
        height = WWorldMath::GetPlaneY(norm, UMath::Vector4To3(temp.FacePoint(0)), pt);
        if (normal != nullptr) {
            *normal = norm;
        }
        return true;
    } else {
        return false;
    }
}

bool WCollisionMgr::GetWorldHeightAtPointRigorous(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal) {
    if (!this->GetWorldHeightAtPoint(pt, height, normal)) {
        UMath::Vector4 seg[2];
        seg[1] = UMath::Vector4Make(pt, 1.0f);
        seg[0] = seg[1];
        seg[0].y -= 2.0f;
        seg[1].y += 1000.0f;

        WorldCollisionInfo cInfo;
        WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 1);

        if (cInfo.HitSomething()) {
            if (cInfo.fType == 1) {
                height = cInfo.fCollidePt.y;
                if (normal != nullptr) {
                    *normal = UMath::Vector4To3(cInfo.fNormal);
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

int WCollisionMgr::CheckHitWorld(const UMath::Vector4 *inputSeg, WorldCollisionInfo &cInfo, unsigned int primMask) {
    UMath::Vector4 seg[2];
    UMath::Vector4 segPosRad;
    int hitWorld;

    cInfo.fType = 0;

    UMath::Copy(inputSeg[0], seg[0]);
    UMath::Copy(inputSeg[1], seg[1]);

    {
        float len = UMath::Distancexyz(seg[0], seg[1]);
        if (len == 0.0f) {
            return 0;
        }

        if (len < 0.01f) {
            UMath::Vector4 diffVec;
            UMath::Subxyz(seg[1], seg[0], diffVec);
            UMath::Scalexyz(diffVec, 0.01f / len, diffVec);
            UMath::Addxyz(seg[0], diffVec, seg[1]);
        }

        hitWorld = 0;

        UMath::Add(UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]), UMath::Vector4To3(segPosRad));
        UMath::Scale(UMath::Vector4To3(segPosRad), 0.5f, UMath::Vector4To3(segPosRad));
        segPosRad.w = len * 0.5f;
    }

    {
        WCollisionInstanceCacheList instList;
        instList.reserve(0x40);

        this->fPrimitiveMask = primMask;
        GetInstanceList(instList, seg);
        this->fPrimitiveMask = 3;

        if (primMask & 2) {
            bool hitBarrier = GetBarrierNormal(instList, seg, cInfo);
            if (hitBarrier) {
                hitWorld = 2;
            }
        }

        if (hitWorld != 0) {
            UMath::Copy(cInfo.fCollidePt, seg[1]);
        }

        if (primMask & 1) {
            static WWorldPos wPos(0.025f);

            wPos.FindClosestFace(instList, UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]));

            if (wPos.OnValidFace()) {
                WorldCollisionInfo cInfoFace;

                cInfoFace.fType = 1;

                wPos.UNormal(&UMath::Vector4To3(cInfoFace.fNormal));
                cInfoFace.fNormal.w = 1.0f;

                float t;
                if (WWorldMath::IntersectSegPlane(*UMath::Vector4To3(&seg[0]), *UMath::Vector4To3(&seg[1]), UMath::Vector4To3(wPos.FacePoint(0)),
                                                  UMath::Vector4To3(cInfoFace.fNormal), *UMath::Vector4To3(&cInfoFace.fCollidePt), t)) {
                    cInfoFace.fCInst = nullptr;
                    cInfoFace.fAnimated = 0;

                    UMath::Vector4 hitVec;
                    UMath::Subxyz(seg[0], cInfoFace.fCollidePt, hitVec);

                    if (UMath::Dotxyz(cInfoFace.fNormal, hitVec) < 0.0f) {
                        UMath::Negatexyz(cInfoFace.fNormal);
                    }

                    if (hitWorld != 0) {
                        float dsq1 = UMath::DistanceSquare(UMath::Vector4To3(seg[0]), UMath::Vector4To3(cInfo.fCollidePt));
                        float dsq2 = UMath::DistanceSquare(UMath::Vector4To3(seg[0]), UMath::Vector4To3(cInfoFace.fCollidePt));
                        if (dsq2 < dsq1) {
                            cInfo = cInfoFace;
                            hitWorld = 1;
                        }
                    } else {
                        cInfo = cInfoFace;
                        hitWorld = 1;
                    }
                }
            }
        }

        if (hitWorld != 0) {
            UMath::Copy(cInfo.fCollidePt, seg[1]);
        }
    }

    return static_cast<int>(cInfo.HitSomething());
}

bool WCollisionMgr::GetWorldNormal(const WCollisionInstanceCacheList *instList, const WCollisionBarrierList *barrierList, const UMath::Vector4 *seg,
                                   WorldCollisionInfo &cInfo) {
    WorldCollisionInfo cInfoFaces;
    WorldCollisionInfo cInfoBarrier;

    if (barrierList != nullptr) {
        this->GetBarrierNormal(*barrierList, seg, cInfoBarrier);
    }

    if (instList != nullptr) {
        static WWorldPos wPos(0.025f);

        wPos.FindClosestFace(*instList, UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]));

        if (wPos.OnValidFace()) {
            float t;

            wPos.UNormal(&UMath::Vector4To3(cInfoFaces.fNormal));
            cInfoFaces.fNormal.w = 1.0f;

            if (WWorldMath::IntersectSegPlane(*UMath::Vector4To3(&seg[0]), *UMath::Vector4To3(&seg[1]), UMath::Vector4To3(wPos.FacePoint(0)),
                                              UMath::Vector4To3(cInfoFaces.fNormal), *UMath::Vector4To3(&cInfoFaces.fCollidePt), t)) {
                cInfoFaces.fType = 1;
                cInfoFaces.fCInst = nullptr;
                cInfoFaces.fAnimated = 0;

                UMath::Vector4 hitVec;
                UMath::Subxyz(seg[0], cInfoFaces.fCollidePt, hitVec);

                if (UMath::Dotxyz(cInfoFaces.fNormal, hitVec) < 0.0f) {
                    UMath::Negatexyz(cInfoFaces.fNormal);
                }
            }
        }
    }

    cInfo.fType = 0;
    this->ClosestCollisionInfo(seg, cInfoFaces, cInfoBarrier, cInfo);
    return cInfo.HitSomething();
}

void WCollisionMgr::ClosestCollisionInfo(const UMath::Vector4 *seg, const WorldCollisionInfo &c1, const WorldCollisionInfo &c2,
                                         WorldCollisionInfo &result) {
    if (!c1.HitSomething() && !c2.HitSomething()) {
        return;
    }
    float distSqC1 = 9.9e31f;
    float distSqC2 = 9.9e31f;
    if (c1.HitSomething()) {
        distSqC1 = UMath::DistanceSquare(UMath::Vector4To3(*seg), UMath::Vector4To3(c1.fCollidePt));
    }
    if (c2.HitSomething()) {
        distSqC2 = UMath::DistanceSquare(UMath::Vector4To3(*seg), UMath::Vector4To3(c2.fCollidePt));
    }
    if (distSqC1 < distSqC2) {
        result = c1;
    } else {
        result = c2;
    }
}

void WCollisionMgr::GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector3 &inPt,
                                        float radius,
#ifdef EA_BUILD_A124
                                        bool cullStrips,
#endif
                                        bool cylinderTest) {
    UMath::Vector3 pt;
    const WGrid &grid = WGrid::Get();
    pt = inPt;
#ifndef EA_BUILD_A124
    ++fIterCount;
#endif

    for (const unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *node = grid.fNodes[*iter];
        if (node != nullptr) {
            const unsigned int *instIndPtr;
            WGridNode::iterator eIter(node, WGrid_kInstance);
            while ((instIndPtr = eIter.GetIndPtr()) != nullptr) {
                unsigned int instInd = *instIndPtr;
                const WCollisionInstance *cInst = WCollisionAssets::Get().Instance(instInd);

                if ((cInst != nullptr) && (cInst->fGroupNumber == 0 || IsSceneryGroupEnabled(cInst->fGroupNumber)) && (cInst->fCollisionArticle != nullptr) &&
                    this->InstancePassesExclusion(*cInst) && cInst->fIterStamp != fIterCount) {
                    const_cast<WCollisionInstance *>(cInst)->fIterStamp = fIterCount;

                    UMath::Vector3 instPos;
                    cInst->CalcPosition(instPos);

                    float instRadius;
                    if (cInst->NeedsCrossProduct()) {
                        instRadius = cInst->CalcSphericalRadius();
                    } else {
                        instRadius = cInst->fInvPosRadius.w;
                    }

                    float radSum = radius + instRadius;
                    if (UMath::DistanceSquarexz(instPos, pt) < radSum * radSum) {
                        instList.push_back(cInst);
                    }
                }
            }
        }
    }
}

void WCollisionMgr::GetInstanceList(WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, float radius,
#ifdef EA_BUILD_A124
                                    bool cullStrips,
#endif
                                    bool cylinderTest) {
    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(0x40);
    WGrid::Get().FindNodes(pt, radius, nodeInds);
    this->GetInstanceListGuts(nodeInds, instList, pt, radius,
#ifdef EA_BUILD_A124
                              cullStrips,
#endif
                              cylinderTest);
}

// UNSOLVED wmin, wmax, regswaps
void WCollisionMgr::GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector4 *seg) {
    const WGrid &grid = WGrid::Get();

#ifndef EA_BUILD_A124
    ++fIterCount;
#endif

    float invDen;
    UMath::Vector3 npVec;
    NearPtLinePerSegXZ(UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]), invDen, npVec);
    float minSegY = WWorldMath::wmin(seg[1].y, seg[0].y);
    float maxSegY = WWorldMath::wmax(seg[1].y, seg[0].y);

    for (const unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *node = grid.fNodes[*iter];
        if (node != nullptr) {
            WGridNode::iterator eIter(node, WGrid_kInstance);
            const unsigned int *instIndPtr;
            while ((instIndPtr = eIter.GetIndPtr()) != nullptr) {
                unsigned int instInd = *instIndPtr;
                const WCollisionInstance *cInst = WCollisionAssets::Get().Instance(instInd);

                if ((cInst != nullptr) && (cInst->fGroupNumber == 0 || IsSceneryGroupEnabled(cInst->fGroupNumber)) && (cInst->fCollisionArticle != nullptr) &&
                    this->InstancePassesExclusion(*cInst) && cInst->fIterStamp != fIterCount) {
                    float instRad = cInst->fInvPosRadius.w;
                    float instRadSq = instRad * instRad;
                    const_cast<WCollisionInstance *>(cInst)->fIterStamp = fIterCount;

                    UMath::Vector3 diffVec;
                    UMath::Vector3 nearPt;
                    UMath::Matrix4 invMat;

                    cInst->MakeMatrix(invMat, true);
#ifdef EA_BUILD_A124
                    UMath::OrthoInverse(invMat);
#else
                    OrthoInverse(invMat);
#endif

                    const UMath::Vector3 &instPos = *UMath::Vector4To3(&invMat[3]);

                    NearPtLineXZ(instPos, UMath::Vector4To3(seg[0]), invDen, npVec, nearPt);

                    UMath::Sub(instPos, nearPt, diffVec);

                    float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;

                    if (dSq < instRadSq) {
                        if (cInst->NeedsCrossProduct()) {
                            instList.push_back(cInst);
                        } else {
                            float instTopY = instPos.y + cInst->fHeight;
                            float instBotY = instPos.y - cInst->fHeight;
                            if (minSegY < instTopY && maxSegY > instBotY) {
                                instList.push_back(cInst);
                            }
                        }
                    }
                }
            }
        }
    }
}

void WCollisionMgr::GetInstanceList(WCollisionInstanceCacheList &instList, const UMath::Vector4 *seg) {
    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(0x40);
    WGrid::Get().FindNodes(seg, nodeInds);
    this->GetInstanceListGuts(nodeInds, instList, seg);
}

void WCollisionMgr::GetObjectListGuts(const NodeIndexList &nodeInds, WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius) {
    const WGrid &grid = WGrid::Get();

    for (const unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *node = grid.fNodes[*iter];
        if (node != nullptr) {
            const unsigned int *objIndPtr;
            WGridNode::iterator eIter(node, WGrid_kObject);
            while ((objIndPtr = eIter.GetIndPtr()) != nullptr) {
                unsigned int objInd = *objIndPtr;
                const WCollisionObject *cObj = WCollisionAssets::Get().Object(objInd);
                if (cObj == nullptr) {
                    continue;
                }
                UMath::Vector3 cp = UMath::Vector4To3(cObj->fPosRadius);
                if (UMath::DistanceSquarexz(cp, pt) < ((radius + cObj->fPosRadius.w) * (radius + cObj->fPosRadius.w)) && cObj->fType == 0) {
                    obbObjectList.push_back(cObj);
                }
            }
        }
    }
}

void WCollisionMgr::GetObjectList(WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius) {
    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(64);
    WGrid::Get().FindNodes(pt, radius, nodeInds);
    obbObjectList.reserve(16);
    this->GetObjectListGuts(nodeInds, obbObjectList, pt, radius);
}

void WCollisionMgr::BuildGeomFromWorldObb(const WCollisionObject &object, float dt, Dynamics::Collision::Geometry &geom, UMath::Vector3 &vel,
                                          WSurface &surface) {
    surface = object.GetWSurface();

    UMath::Vector3 objPos = UMath::Vector3Make(object.fPosRadius.x, object.fPosRadius.y, object.fPosRadius.z);
    UMath::Matrix4 objMat;
    UMath::Vector3 pos;

    object.MakeMatrix(objMat, false);

    vel.x = vel.y = vel.z = 0.0f;
    UMath::ScaleAdd(*reinterpret_cast<UMath::Vector3 *>(&objMat.v1), object.fDimensions.y, objPos, pos);

    UMath::Vector3 dim = UMath::Vector3Make(object.fDimensions.x, object.fDimensions.y, object.fDimensions.z);
    UMath::Vector3 dP;
    UMath::Scale(vel, dt, dP);

    geom.Set(objMat, pos, dim, Dynamics::Collision::Geometry::BOX, dP);
}

bool WCollisionMgr::Collide(Dynamics::Collision::Geometry *geom, const WCollisionBarrierList *barrierList, ICollisionHandler *results, void *userdata,
                            bool force_single_sided) {
    bool hit = false;
    if ((barrierList != nullptr) && !barrierList->empty()) {

        const WCollisionBarrierList &barriers = *barrierList;
        UMath::Matrix4 mat = UMath::Matrix4::kIdentity;

        for (const WCollisionBarrierListEntry *iter = barriers.begin(); iter != barriers.end(); ++iter) {
            const WCollisionBarrierListEntry &ble = *iter;
            if (!this->SurfacePassesExclusion(ble.fB.GetWSurface())) {
                continue;
            }

            UMath::Vector4 bcp;
            ble.fB.GetCenter(bcp);

            UMath::Vector3 &vR = *reinterpret_cast<UMath::Vector3 *>(&mat[0][0]);
            UMath::Vector3 &vU = *reinterpret_cast<UMath::Vector3 *>(&mat[1][0]);
            UMath::Vector3 &vF = *reinterpret_cast<UMath::Vector3 *>(&mat[2][0]);

            const WCollisionBarrier *barrier = &ble.fB;

            barrier->GetNormal(vF);

            float w = barrier->GetWidth();
            float h = barrier->GetHeight();

            UMath::Vector3 bdim = {};
            bdim.x = w * 0.5f;
            bdim.y = h * 0.5f;

            vU = UMath::Vector3Make(0.0f, 1.0f, 0.0f);
            UMath::Cross(vU, vF, vR);

            Dynamics::Collision::Geometry bgeom(mat, UVector3(bcp), bdim, Dynamics::Collision::Geometry::BOX, UMath::Vector3::kZero);

            if (!Dynamics::Collision::Geometry::FindIntersection(geom, &bgeom, geom)) {
                continue;
            }

            if (force_single_sided || !barrier->GetWSurface().HasFlag(0x10)) {
                if (UMath::Dot(vF, geom->GetCollisionNormal()) <= 0.0f) {
                    continue;
                }
            }

            hit = true;
            if (results != nullptr) {
                float penetration = -geom->GetOverlap();

                WorldCollisionInfo cInfo;
                cInfo.fNormal = UMath::Vector4Make(geom->GetCollisionNormal(), penetration);
                cInfo.fType = 2;
                cInfo.fCInst = nullptr;
                cInfo.fBle = ble;
                cInfo.fCollidePt = UMath::Vector4Make(geom->GetCollisionPoint(), penetration);

                if (cInfo.fCInst != nullptr && cInfo.fCInst->IsDynamic()) {
                    cInfo.fAnimated = 1;
                }

                UMath::Vector3 dP;
                UMath::Scale(geom->GetCollisionNormal(), -geom->GetOverlap(), dP);

                UMath::Vector3 cPoint;
                cPoint = geom->GetCollisionPoint();

                if (geom->PenetratesOther()) {
                    UMath::Add(cPoint, dP, cPoint);
                }

                if (results->OnWCollide(cInfo, *reinterpret_cast<bVector3 *>(&cPoint), userdata)) {
                    geom->Move(dP);
                }
            } else {
                return true;
            }
        }
    }

    return hit;
}

// UNSOLVED for loop and stack issues
bool WCollisionMgr::Collide(Dynamics::Collision::Geometry *geom, const WCollisionInstanceCacheList *instanceList, ICollisionHandler *results,
                            void *userdata) {
    bool hit = false;

    if ((instanceList != nullptr) && !instanceList->empty()) {
        unsigned int i, j;
        static const UMath::Vector4 offsets[4][2] = {{{-1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f, 1.0f}},
                                                     {{-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}},
                                                     {{1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f, 1.0f}},
                                                     {{1.0f, -1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}}};
        unsigned int num2check;
        UMath::Vector4 arms[4][2];
        UMath::Vector4 dim = UMath::Vector4Make(geom->GetDimension(), 1.0f);
        UMath::Vector4 cp = UMath::Vector4Make(geom->GetPosition(), 1.0f);

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 2; j++) {
                UMath::Vector4 tmp;
                UMath::Vector4 tmp2;

                tmp.w = 1.0f;
                UMath::Scalexyz(offsets[i][j], dim, tmp);
                UMath::Rotate(tmp, geom->GetOrientation(), tmp2);
                tmp2.w = 1.0f;
                UMath::Add(tmp2, cp, arms[i][j]);
                arms[i][j].w = 1.0f;
            }
        }

        UMath::Vector4 seg[2];
        UMath::Vector4 delta = UMath::Vector4::kIdentity;

        for (i = 0; i < 4; i++) {
            UMath::Addxyz(arms[i][0], delta, seg[0]);
            UMath::Addxyz(arms[i][1], delta, seg[1]);

            WorldCollisionInfo cInfo;

            if (this->GetWorldNormal(instanceList, nullptr, seg, cInfo)) {
                if (results != nullptr) {
                    UMath::Vector4 penVec;
                    UMath::Vector4 cPoint;
                    float penetration;

                    if (UMath::DistanceSquarexyz(seg[0], cInfo.fCollidePt) < UMath::DistanceSquarexyz(seg[1], cInfo.fCollidePt)) {
                        cPoint = seg[0];
                    } else {
                        cPoint = seg[1];
                    }

                    UMath::Subxyz(cPoint, cInfo.fCollidePt, penVec);
                    penVec.w = 0.0f;
                    cInfo.fNormal.w = 1.0f;
                    penetration = -UMath::Dotxyz(penVec, cInfo.fNormal);
                    if (penetration < 0.0f) {
                        UMath::Negatexyz(cInfo.fNormal);
                        penetration = -penetration;
                    }

                    cPoint.w = penetration;
                    cInfo.fNormal.w = penetration;

                    if (results->OnWCollide(cInfo, *reinterpret_cast<bVector3 *>(&cPoint), userdata)) {
                        UMath::ScaleAddxyz(cInfo.fNormal, cInfo.fNormal.w, delta, delta);
                    }
                } else {
                    return true;
                }
                hit = true;
            }
        }
    }

    return hit;
}

bool WCollisionMgr::GetClosestIntersectingBarrier(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment,
                                                  WorldCollisionInfo &cInfo) {
    cInfo.fType = 0;
    const WCollisionBarrierListEntry *ret = nullptr;
    float closestDistSq = 1e38f;

    for (const WCollisionBarrierListEntry *bIter = barrierList.begin(); bIter != barrierList.end(); ++bIter) {
        const WCollisionBarrier *barrier = &bIter->fB;
        if (this->SurfacePassesExclusion(barrier->GetWSurface())) {
            UMath::Vector4 intersectionPt;
            if (WWorldMath::SegmentIntersect(testSegment, barrier->GetPts(), &intersectionPt) && intersectionPt.y > barrier->YBot() &&
                intersectionPt.y < barrier->YTop()) {
                float distSq = UMath::DistanceSquare(UMath::Vector4To3(intersectionPt), UMath::Vector4To3(*testSegment));
                if (distSq < closestDistSq) {
                    cInfo.fCollidePt = intersectionPt;
                    ret = bIter;
                    closestDistSq = distSq;
                }
            }
        }
    }

    if (ret != nullptr) {
        cInfo.fBle = *ret;
        cInfo.fType = 2;
    }
    return cInfo.HitSomething();
}

// UNSOLVED
bool WCollisionMgr::GetBarrierNormal(const WCollisionInstanceCacheList &instList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo) {
    const WCollisionBarrier *closestBarrier = nullptr;
    const WCollisionInstance *closestBarrierInst = nullptr;
    UMath::Vector4 closestIntersectionPt;
    float closestDistSq = 1e38f;

    for (const WCollisionInstance *const *iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        const WCollisionInstance &cInst = **iIter;
        const WCollisionArticle *cArt = cInst.fCollisionArticle;
        if ((cArt != nullptr) && cArt->fNumEdges != 0) {
            UMath::Matrix4 invMat;
            UMath::Vector4 tseg[2];
            cInst.MakeMatrix(invMat, true);
            UMath::RotateTranslate(UMath::Vector4To3(testSegment[0]), invMat, UMath::Vector4To3(tseg[0]));
            UMath::RotateTranslate(UMath::Vector4To3(testSegment[1]), invMat, UMath::Vector4To3(tseg[1]));

            const WCollisionBarrier *barrier = cArt->GetBarrier(0);
            for (int i = 0; i < cArt->fNumEdges; ++i) {
                if (!this->SurfacePassesExclusion(barrier->GetWSurface())) {
                    barrier = barrier->Next();
                    continue;
                } else {
                    UMath::Vector4 intersectionPt;
                    if (WWorldMath::SegmentIntersect(tseg, barrier->GetPts(), &intersectionPt) && intersectionPt.y > barrier->YBot() &&
                        intersectionPt.y < barrier->YTop()) {
                        float distSq = UMath::DistanceSquare(UMath::Vector4To3(intersectionPt), UMath::Vector4To3(tseg[0]));
                        if (distSq < closestDistSq) {
                            UMath::Copy(intersectionPt, closestIntersectionPt);
                            closestBarrier = barrier;
                            closestBarrierInst = &cInst;
                            closestDistSq = distSq;
                        }
                    }
                }
                barrier = barrier->Next();
            }
        }
    }

    cInfo.fType = 0;
    if (closestBarrier != nullptr) {
        cInfo.fCInst = closestBarrierInst;
        cInfo.fType = 2;
        cInfo.fAnimated = static_cast<unsigned char>(closestBarrierInst->IsDynamic());

        UMath::Matrix4 invMat;
        closestBarrierInst->MakeMatrix(invMat, true);
#ifdef EA_BUILD_A124
        UMath::OrthoInverse(invMat);
#else
        OrthoInverse(invMat);
#endif

        {
            WCollisionBarrier b;
            UMath::RotateTranslate(UMath::Vector4To3(closestBarrier->fPts[0]), invMat, UMath::Vector4To3(b.fPts[0]));
            UMath::RotateTranslate(UMath::Vector4To3(closestBarrier->fPts[1]), invMat, UMath::Vector4To3(b.fPts[1]));
            b.fPts[0].w = closestBarrier->fPts[0].w;
            b.fPts[1].w = closestBarrier->fPts[1].w;

            const Attrib::Collection *surfaceHash = closestBarrierInst->fCollisionArticle->GetSurface(b.GetWSurface().Surface());

            cInfo.fBle = WCollisionBarrierListEntry(b, surfaceHash, closestDistSq);
        }

        UMath::RotateTranslate(UMath::Vector4To3(closestIntersectionPt), invMat, UMath::Vector4To3(cInfo.fCollidePt));

        cInfo.fCollidePt.w = 1.0f;
        cInfo.fBle.fB.GetNormal(UMath::Vector4To3(cInfo.fNormal));
        cInfo.fNormal.y = 0.0f;
        cInfo.fNormal.w = 0.0f;

        UMath::Vector3 testVec;
        UMath::Sub(UMath::Vector4To3(*testSegment), UMath::Vector4To3(cInfo.fCollidePt), testVec);
        float result = cInfo.fNormal.x * testVec.x + cInfo.fNormal.z * testVec.z;
        if (result < 0.0f) {
            cInfo.fNormal.x = -cInfo.fNormal.x;
            cInfo.fNormal.z = -cInfo.fNormal.z;
        }
    }
    return cInfo.HitSomething();
}

// UNSOLVED https://decomp.me/scratch/hGkzc
void WCollisionMgr::GetBarrierList(WCollisionBarrierList &barrierList, const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pos,
                                   float radius) {
    float radiusSq = radius * radius;
    barrierList.reserve(21);

    for (WCollisionInstanceCacheList::const_iterator iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        const WCollisionInstance &cInst = **iIter;

        if (!InstancePassesExclusion(cInst))
            continue;

        const WCollisionArticle *cArt = cInst.fCollisionArticle;
        if ((cArt != nullptr) && cArt->fNumEdges != 0) {
            UMath::Vector3 tpt;
            UMath::Matrix4 invMat;

            cInst.MakeMatrix(invMat, true);
            UMath::RotateTranslate(pos, invMat, tpt);

            UTransform t;
            bool tValid = false;

            const WCollisionBarrier *barrier = cArt->GetBarrier(0);
            for (int i = 0; i < cArt->fNumEdges; ++i) {
                if (!this->SurfacePassesExclusion(barrier->GetWSurface())) {
                    barrier = barrier->Next();
                    continue;
                }
                float distsqr = barrier->DistSq(tpt);
                if (distsqr < radiusSq && (tpt.y + radius > barrier->YBot()) && (tpt.y - radius <= barrier->YTop())) {
                    if (!tValid) {
                        t.fTransform = invMat;
                        t.fTransform[0][3] = 0.0f;
                        t.fTransform[1][3] = 0.0f;
                        t.fTransform[2][3] = 0.0f;
                        t.fTransform[3][3] = 1.0f;
#ifdef EA_BUILD_A124
                        UMath::OrthoInverse(t.fTransform);
#else
                        OrthoInverse(t.fTransform);
#endif
                        tValid = true;
                    }

                    WCollisionBarrier wBarrier = *barrier;
                    UMath::RotateTranslate(UMath::Vector4To3(wBarrier.fPts[0]), t.fTransform, UMath::Vector4To3(wBarrier.fPts[0]));
                    UMath::RotateTranslate(UMath::Vector4To3(wBarrier.fPts[1]), t.fTransform, UMath::Vector4To3(wBarrier.fPts[1]));

                    // TODO GetSurface is probably wrong
                    const Attrib::Collection *collection = cArt->GetSurface(wBarrier.GetWSurface().Surface());
                    WCollisionBarrierListEntry ble(wBarrier, collection, distsqr);

                    if (1) {
                        barrierList.insert(std::upper_bound(barrierList.begin(), barrierList.end(), ble), ble);
                    } else {
                        barrierList.push_back(ble);
                    }
                }
                barrier = barrier->Next();
            }
        }
    }
}

// STRIPPED
void WCollisionMgr::GetBarrierList(WCollisionBarrierList &barrierList, const UMath::Vector3 &pos, float radius) {}

bool WCollisionMgr::GetBarrierNormal(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo) {
    cInfo.fType = 0;
    if (this->GetClosestIntersectingBarrier(barrierList, testSegment, cInfo)) {
        cInfo.fBle.fB.GetNormal(UMath::Vector4To3(cInfo.fNormal));
        cInfo.fNormal.w = 0.0f;
        cInfo.fCInst = nullptr;
        cInfo.fAnimated = 0;
        UMath::Vector3 testVec;
        UMath::Sub(UMath::Vector4To3(*testSegment), UMath::Vector4To3(cInfo.fCollidePt), testVec);

        float result = cInfo.fNormal.x * testVec.x + cInfo.fNormal.z * testVec.z;
        if (result < 0.0f) {
            cInfo.fNormal.x = -cInfo.fNormal.x;
            cInfo.fNormal.z = -cInfo.fNormal.z;
        }
        cInfo.fType = 2;
    }
    return cInfo.HitSomething();
}

struct AABB {
    bVector2 mMin;
    bVector2 mMax;

    AABB(const UMath::Vector3 &pt, float radius) {
        mMin.x = pt.x - radius;
        mMin.y = pt.z - radius;
        mMax.x = pt.x + radius;
        mMax.y = pt.z + radius;
    }

    AABB(const UMath::Vector3 &pt1, const UMath::Vector3 &pt2, const UMath::Vector3 &pt3) {
        mMin.x = bMin(pt3.x, bMin(pt1.x, pt2.x));
        mMin.y = bMin(pt3.z, bMin(pt1.z, pt2.z));
        mMax.x = bMax(pt3.x, bMax(pt1.x, pt2.x));
        mMax.y = bMax(pt3.z, bMax(pt1.z, pt2.z));
    }

    bool Overlap(const AABB &test) {
        if ((test.mMin.x > mMax.x) || (test.mMin.y > mMax.y) || (test.mMax.x < mMin.x)) {
            return false;
        } else {
            return test.mMax.y >= mMin.y;
        }
    }
};

inline float PTDir(const UMath::Vector3 &vert, const UMath::Vector3 &p0, const UMath::Vector3 &p1) {
    float x0 = vert.x - p0.x;
    float z0 = vert.z - p0.z;
    float x1 = p1.x - p0.x;
    float z1 = p1.z - p0.z;
    return x1 * z0 - x0 * z1;
}

inline float PtDir(const UMath::Vector3 &p1, const UMath::Vector3 &p2, const UMath::Vector3 &p3) {
    return (p2.x - p3.x) * (p1.z - p3.z) - (p1.x - p3.x) * (p2.z - p3.z);
}

inline float XZDistSq(const UMath::Vector3 &p0, const UMath::Vector3 &p1) {
    return (p0.x - p1.x) * (p0.x - p1.x) + (p0.z - p1.z) * (p0.z - p1.z);
}

// UNSOLVED goto. I tried breaking out from a do while (0), but it messed up the dwarf
void WCollisionMgr::GetTriList(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, float radius, WCollisionTriList &triList) {
    float radiusSq = radius * radius;

    for (const WCollisionInstance *const *iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        const WCollisionInstance &cInst = **iIter;
        const WCollisionArticle *cArt = cInst.fCollisionArticle;
        if (cArt != nullptr) {
            UMath::Vector3 ipt;
            UMath::Vector3 tpt;
            UMath::Matrix4 invMat;

            ipt = pt;
            cInst.MakeMatrix(invMat, true);
            UMath::RotateTranslate(ipt, invMat, tpt);

            const WCollisionStripSphere *sp = cArt->GetStripSphere(0);
            AABB regionAABB(pt, radius);

            for (int i = 0; i < cArt->fNumStrips; i++, sp++) {
                UMath::Vector3 diffVec;
                v3sub(1, &sp->fPos, &tpt, &diffVec);

                float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
                float spRadius = static_cast<float>(static_cast<int>(sp->fRadius)) / 16.0f;
                float tempRadSum = spRadius + radius;
                if (dSq < tempRadSum * tempRadSum) {
                    const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(reinterpret_cast<const char *>(cArt) + sp->Offset());
                    int numTris = strip->NumTris();
                    WCollisionTri face;
                    UMath::Vector3 off;

                    UMath::Sub(sp->fPos, UMath::Vector4To3(invMat.v3), off);

                    strip->MakeFace(0, off, face);

                    for (int i = 0; i < numTris; ++i) {
                        AABB faceAABB(face.fPt1, face.fPt2, face.fPt0);
                        if (faceAABB.Overlap(regionAABB)) {
                            UMath::Vector3 nearPt;
                            float dir = PTDir(face.fPt1, face.fPt0, face.fPt2);
                            float side = PtDir(pt, face.fPt0, face.fPt1);
                            if (side * dir <= 0.0f) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt0, face.fPt1, nearPt);
                                if ((side * dir <= 0.0f) && (XZDistSq(pt, nearPt) >= radiusSq)) {
                                    goto next_tri;
                                }
                            }

                            side = PtDir(pt, face.fPt1, face.fPt2);
                            if (side * dir <= 0.0f) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt1, face.fPt2, nearPt);
                                if ((side * dir <= 0.0f) && (XZDistSq(pt, nearPt) >= radiusSq)) {
                                    goto next_tri;
                                }
                            }

                            side = PtDir(pt, face.fPt2, face.fPt0);
                            if (side * dir <= 0.0f) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt2, face.fPt0, nearPt);
                                if ((side * dir <= 0.0f) && (XZDistSq(pt, nearPt) >= radiusSq)) {
                                    goto next_tri;
                                }
                            }

                            face.fSurfaceRef = reinterpret_cast<const SimSurface *>(cArt->GetSurface(face.fSurface.Surface()));
                            triList.add_tri(face);
                        }
                    next_tri:
                        if (i + 1 < numTris) {
                            strip->MakeNextFace(i + 1, off, face);
                        }
                    }
                }
            }
        }
    }
}
