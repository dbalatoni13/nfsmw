#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"

#include <float.h>

void OrthoInverse(UMath::Matrix4 &m);

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
    u = UMath::Min(u, 1.0f);
    u = UMath::Max(0.0f, u);
    nearPt.x = u * diffVec.x + p0.x;
    nearPt.z = u * diffVec.z + p0.z;
}

void WCollisionMgr::GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector3 &inPt, float radius,
                                        bool cylinderTest) {
    UMath::Vector3 pt;
    const WGrid &grid = WGrid::Get();
    pt = inPt;
    ++fIterCount;

    for (const unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *node = grid.fNodes[*iter];
        if (node != nullptr) {
            WGridNode::iterator eIter(node, WGrid_kInstance);
            const unsigned int *instIndPtr;
            while ((instIndPtr = eIter.GetIndPtr()) != nullptr) {
                unsigned int instInd = *instIndPtr;
                const WCollisionInstance *cInst = WCollisionAssets::Get().Instance(instInd);

                if (cInst != nullptr &&
                    (cInst->fGroupNumber == 0 || IsSceneryGroupEnabled(cInst->fGroupNumber)) &&
                    cInst->fCollisionArticle != nullptr &&
                    InstancePassesExclusion(*cInst) &&
                    cInst->fIterStamp != fIterCount) {
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

void WCollisionMgr::GetInstanceList(WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, float radius, bool cylinderTest) {
    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(0x40);
    WGrid::Get().FindNodes(pt, radius, nodeInds);
    GetInstanceListGuts(nodeInds, instList, pt, radius, cylinderTest);
}

void WCollisionMgr::GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector4 *seg) {
    const WGrid &grid = WGrid::Get();
    float invDen;
    UMath::Vector3 npVec;

    ++fIterCount;
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

                if (cInst != nullptr &&
                    (cInst->fGroupNumber == 0 || IsSceneryGroupEnabled(cInst->fGroupNumber)) &&
                    cInst->fCollisionArticle != nullptr &&
                    InstancePassesExclusion(*cInst) &&
                    cInst->fIterStamp != fIterCount) {
                    float instRad = cInst->fInvPosRadius.w;
                    const_cast<WCollisionInstance *>(cInst)->fIterStamp = fIterCount;

                    UMath::Matrix4 invMat;
                    cInst->MakeMatrix(invMat, true);
                    OrthoInverse(invMat);

                    const UMath::Vector3 &instPos = UMath::Vector4To3(invMat.v3);

                    UMath::Vector3 nearPt;
                    NearPtLineXZ(instPos, UMath::Vector4To3(seg[0]), invDen, npVec, nearPt);

                    UMath::Vector3 diffVec;
                    UMath::Sub(instPos, nearPt, diffVec);

                    float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
                    float instRadSq = instRad * instRad;

                    if (dSq < instRadSq) {
                        if (cInst->NeedsCrossProduct()) {
                            instList.push_back(cInst);
                        } else {
                            float instTopY = instPos.y + cInst->fHeight;
                            float instBotY = instPos.y - cInst->fHeight;
                            if (instTopY > minSegY && instBotY < maxSegY) {
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
    GetInstanceListGuts(nodeInds, instList, seg);
}

static void CalcCollisionFaceNormal(UMath::Vector3 *norm, UMath::Vector4 *facePts) {
    UMath::Vector3 vecZ;
    UMath::Vector3 vecX;
    UMath::Vector3 normal;
    vecX.x = facePts[1].x - facePts[0].x;
    vecX.y = facePts[1].y - facePts[0].y;
    vecZ.z = facePts[0].z - facePts[2].z;
    vecX.z = facePts[1].z - facePts[0].z;
    vecZ.x = facePts[0].x - facePts[2].x;
    vecZ.y = facePts[0].y - facePts[2].y;
    UMath::Cross(vecX, vecZ, normal);
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
    if (norm->y >= 1.0f) {
        norm->y = 1.0f;
    }
}

bool WCollisionMgr::GetWorldHeightAtPointRigorous(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal) {
    if (!GetWorldHeightAtPoint(pt, height, normal)) {
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

    float len = UMath::Distancexyz(seg[0], seg[1]);
    if (len == 0.0f) {
        return 0;
    }

    if (len < 0.5f) {
        UMath::Vector4 diffVec;
        UMath::Subxyz(seg[1], seg[0], diffVec);
        UMath::Scalexyz(diffVec, 0.5f / len, diffVec);
        UMath::Addxyz(seg[0], diffVec, seg[1]);
    }

    hitWorld = 0;

    UMath::Add(UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]), UMath::Vector4To3(segPosRad));
    UMath::Scale(UMath::Vector4To3(segPosRad), 0.5f, UMath::Vector4To3(segPosRad));
    segPosRad.w = len * 0.5f;

    WCollisionInstanceCacheList instList;
    instList.reserve(0x40);

    fPrimitiveMask = primMask;
    GetInstanceList(instList, seg);
    fPrimitiveMask = 3;

    if ((primMask & 2) != 0) {
        if (GetBarrierNormal(instList, seg, cInfo)) {
            hitWorld = 2;
        }
    }

    if (hitWorld != 0) {
        UMath::Copy(cInfo.fCollidePt, seg[1]);
    }

    if ((primMask & 1) != 0) {
        static WWorldPos wPos(2.0f);

        wPos.FindClosestFace(instList, UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]));

        if (wPos.OnValidFace()) {
            WorldCollisionInfo cInfoFace;
            float t;

            cInfoFace.fType = 1;

            wPos.UNormal(&UMath::Vector4To3(cInfoFace.fNormal));
            cInfoFace.fNormal.w = 1.0f;

            if (WWorldMath::IntersectSegPlane(
                    UMath::Vector4To3(seg[0]),
                    UMath::Vector4To3(seg[1]),
                    UMath::Vector4To3(wPos.FacePoint(0)),
                    UMath::Vector4To3(cInfoFace.fNormal),
                    UMath::Vector4To3(cInfoFace.fCollidePt),
                    t)) {
                cInfoFace.fAnimated = 0;
                cInfoFace.fCInst = nullptr;

                UMath::Vector4 hitVec;
                UMath::Subxyz(seg[0], cInfoFace.fCollidePt, hitVec);
                float dot = UMath::Dotxyz(cInfoFace.fNormal, hitVec);

                if (dot < 0.0f) {
                    cInfoFace.fNormal.x = -cInfoFace.fNormal.x;
                    cInfoFace.fNormal.y = -cInfoFace.fNormal.y;
                    cInfoFace.fNormal.z = -cInfoFace.fNormal.z;
                }

                if (hitWorld == 0) {
                    cInfo = cInfoFace;
                } else {
                    float dsq1 = UMath::DistanceSquare(UMath::Vector4To3(seg[0]), UMath::Vector4To3(cInfo.fCollidePt));
                    float dsq2 = UMath::DistanceSquare(UMath::Vector4To3(seg[0]), UMath::Vector4To3(cInfoFace.fCollidePt));
                    if (dsq1 <= dsq2) {
                        goto done;
                    }
                    cInfo = cInfoFace;
                }
                hitWorld = 1;
            }
        }
    }

done:
    if (hitWorld != 0) {
        UMath::Copy(cInfo.fCollidePt, seg[1]);
    }

    return cInfo.HitSomething() ? 1 : 0;
}

bool WCollisionMgr::GetWorldHeightAtPoint(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal) {
    WWorldPos temp(2.0f);
    temp.FindClosestFace(pt, true);
    if (temp.OnValidFace()) {
        UMath::Vector3 norm;
        temp.UNormal(&norm);
        height = WWorldMath::GetPlaneY(norm, UMath::Vector4To3(temp.FacePoint(0)), pt);
        if (normal != nullptr) {
            *normal = norm;
        }
        return true;
    }
    return false;
}

void WCollisionMgr::ClosestCollisionInfo(const UMath::Vector4 *seg, const WorldCollisionInfo &c1, const WorldCollisionInfo &c2, WorldCollisionInfo &result) {
    if (c1.HitSomething() || c2.HitSomething()) {
        float distSqC1 = FLT_MAX;
        float distSqC2 = FLT_MAX;
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
}

void WCollisionMgr::GetObjectListGuts(const NodeIndexList &nodeInds, WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius) {
    const WGrid &grid = WGrid::Get();

    for (const unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *node = grid.fNodes[*iter];
        if (node != nullptr) {
            WGridNode::iterator eIter(node, WGrid_kObject);
            const unsigned int *objIndPtr;
            while ((objIndPtr = eIter.GetIndPtr()) != nullptr) {
                const WCollisionObject *cObj = WCollisionAssets::Get().Object(*objIndPtr);
                if (cObj != nullptr) {
                    UMath::Vector3 cp = UMath::Vector4To3(cObj->fPosRadius);
                    float distSq = UMath::DistanceSquarexz(cp, pt);
                    float totalRadius = radius + cObj->fPosRadius.w;
                    if (distSq < totalRadius * totalRadius && cObj->fType == 0) {
                        obbObjectList.push_back(cObj);
                    }
                }
            }
        }
    }
}

void WCollisionMgr::GetObjectList(WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius) {
    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(0x40);
    WGrid::Get().FindNodes(pt, radius, nodeInds);
    obbObjectList.reserve(0x10);
    GetObjectListGuts(nodeInds, obbObjectList, pt, radius);
}

void WCollisionMgr::BuildGeomFromWorldObb(const WCollisionObject &object, float dt, Dynamics::Collision::Geometry &geom, UMath::Vector3 &vel,
                                          WSurface &surface) {
    surface = object.GetWSurface();

    UMath::Vector3 pos = UMath::Vector3Make(object.fPosRadius.x, object.fPosRadius.y, object.fPosRadius.z);
    UMath::Vector3 objPos = pos;
    UMath::Matrix4 objMat;

    object.MakeMatrix(objMat, false);

    vel.x = 0.0f;
    vel.z = 0.0f;
    vel.y = 0.0f;
    UMath::ScaleAdd(UMath::Vector4To3(objMat.v1), object.fDimensions.y, objPos, pos);

    UMath::Vector3 dP = UMath::Vector3Make(object.fDimensions.x, object.fDimensions.y, object.fDimensions.z);
    UMath::Vector3 dim = dP;
    UMath::Scale(vel, dt, dP);

    geom.Set(objMat, pos, dim, Dynamics::Collision::Geometry::BOX, dP);
}

bool WCollisionMgr::GetClosestIntersectingBarrier(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo) {
    cInfo.fType = 0;
    float closestDistSq = FLT_MAX;
    const WCollisionBarrierListEntry *ret = nullptr;

    for (const WCollisionBarrierListEntry *bIter = barrierList.begin(); bIter != barrierList.end(); ++bIter) {
        const WCollisionBarrier *barrier = &bIter->fB;
        if (!SurfacePassesExclusion(barrier->GetWSurface())) {
            continue;
        }
        UMath::Vector4 intersectionPt;
        if (WWorldMath::SegmentIntersect(testSegment, barrier->GetPts(), &intersectionPt)) {
            float yBot = barrier->YBot();
            float yTop = barrier->YTop();
            float yMin = yBot;
            if (yTop < yBot) {
                yMin = yTop;
            }
            if (yMin < intersectionPt.y) {
                float yMax = yBot;
                if (yBot < yTop) {
                    yMax = yTop;
                }
                if (intersectionPt.y < yMax) {
                    float distSq = UMath::DistanceSquare(UMath::Vector4To3(intersectionPt), UMath::Vector4To3(*testSegment));
                    if (distSq < closestDistSq) {
                        cInfo.fCollidePt = intersectionPt;
                        ret = bIter;
                        closestDistSq = distSq;
                    }
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

bool WCollisionMgr::GetWorldNormal(const WCollisionInstanceCacheList *instList, const WCollisionBarrierList *barrierList, const UMath::Vector4 *seg,
                                   WorldCollisionInfo &cInfo) {
    WorldCollisionInfo cInfoFaces;
    WorldCollisionInfo cInfoBarrier;

    if (barrierList != nullptr) {
        GetBarrierNormal(*barrierList, seg, cInfoBarrier);
    }

    if (instList != nullptr) {
        static WWorldPos wPos(2.0f);

        wPos.FindClosestFace(*instList, UMath::Vector4To3(seg[0]), UMath::Vector4To3(seg[1]));

        if (wPos.OnValidFace()) {
            float t;

            wPos.UNormal(&UMath::Vector4To3(cInfoFaces.fNormal));
            cInfoFaces.fNormal.w = 1.0f;

            if (WWorldMath::IntersectSegPlane(
                    UMath::Vector4To3(seg[0]),
                    UMath::Vector4To3(seg[1]),
                    UMath::Vector4To3(wPos.FacePoint(0)),
                    UMath::Vector4To3(cInfoFaces.fNormal),
                    UMath::Vector4To3(cInfoFaces.fCollidePt),
                    t)) {
                cInfoFaces.fType = 1;
                cInfoFaces.fAnimated = 0;
                cInfoFaces.fCInst = nullptr;

                UMath::Vector4 hitVec;
                UMath::Subxyz(seg[0], cInfoFaces.fCollidePt, hitVec);
                float dot = UMath::Dotxyz(cInfoFaces.fNormal, hitVec);

                if (dot < 0.0f) {
                    UMath::Negatexyz(cInfoFaces.fNormal);
                }
            }
        }
    }

    cInfo.fType = 0;
    ClosestCollisionInfo(seg, cInfoFaces, cInfoBarrier, cInfo);
    return cInfo.HitSomething();
}

bool WCollisionMgr::GetBarrierNormal(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo) {
    cInfo.fType = 0;
    if (GetClosestIntersectingBarrier(barrierList, testSegment, cInfo)) {
        cInfo.fBle.fB.GetNormal(UMath::Vector4To3(cInfo.fNormal));
        cInfo.fNormal.w = 0.0f;
        cInfo.fAnimated = 0;
        cInfo.fCInst = nullptr;
        UMath::Vector3 testVec;
        UMath::Sub(UMath::Vector4To3(*testSegment), UMath::Vector4To3(cInfo.fCollidePt), testVec);
        if (cInfo.fNormal.x * testVec.x + cInfo.fNormal.z * testVec.z < 0.0f) {
            cInfo.fNormal.x = -cInfo.fNormal.x;
            cInfo.fNormal.z = -cInfo.fNormal.z;
        }
        cInfo.fType = 2;
    }
    return cInfo.HitSomething();
}

bool WCollisionMgr::GetBarrierNormal(const WCollisionInstanceCacheList &instList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo) {
    const WCollisionBarrier *closestBarrier = nullptr;
    const WCollisionInstance *closestBarrierInst = nullptr;
    UMath::Vector4 closestIntersectionPt;
    float closestDistSq = FLT_MAX;

    for (const WCollisionInstance *const *iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        const WCollisionInstance &cInst = **iIter;
        const WCollisionArticle *cArt = cInst.fCollisionArticle;
        if (cArt != nullptr && cArt->fNumEdges != 0) {
            UMath::Matrix4 invMat;
            UMath::Vector4 tseg[2];
            cInst.MakeMatrix(invMat, true);
            UMath::RotateTranslate(UMath::Vector4To3(testSegment[0]), invMat, UMath::Vector4To3(tseg[0]));
            UMath::RotateTranslate(UMath::Vector4To3(testSegment[1]), invMat, UMath::Vector4To3(tseg[1]));

            const WCollisionBarrier *barrier = cArt->GetBarrier(0);
            for (int i = 0; i < cArt->fNumEdges; ++i) {
                if (!SurfacePassesExclusion(barrier->GetWSurface())) {
                    barrier = barrier->Next();
                    continue;
                }
                UMath::Vector4 intersectionPt;
                if (WWorldMath::SegmentIntersect(tseg, barrier->GetPts(), &intersectionPt)) {
                    float yTop = barrier->YTop();
                    float yBot = barrier->YBot();
                    float yMin = yTop;
                    if (yBot < yTop) {
                        yMin = yBot;
                    }
                    if (yMin < intersectionPt.y) {
                        if (yTop < yBot) {
                            yTop = yBot;
                        }
                        if (intersectionPt.y < yTop) {
                            float distSq = UMath::DistanceSquare(
                                UMath::Vector4To3(intersectionPt),
                                UMath::Vector4To3(tseg[0]));
                            if (distSq < closestDistSq) {
                                UMath::Copy(intersectionPt, closestIntersectionPt);
                                closestBarrierInst = &cInst;
                                closestBarrier = barrier;
                                closestDistSq = distSq;
                            }
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
        cInfo.fAnimated = closestBarrierInst->IsDynamic();

        UMath::Matrix4 invMat;
        closestBarrierInst->MakeMatrix(invMat, true);
        OrthoInverse(invMat);

        WCollisionBarrier b;
        UMath::RotateTranslate(UMath::Vector4To3(*closestBarrier->GetPt(0)), invMat, UMath::Vector4To3(b.fPts[0]));
        UMath::RotateTranslate(UMath::Vector4To3(*closestBarrier->GetPt(1)), invMat, UMath::Vector4To3(b.fPts[1]));
        b.fPts[0].w = closestBarrier->fPts[0].w;
        b.fPts[1].w = closestBarrier->fPts[1].w;

        const WCollisionArticle *cArt = closestBarrierInst->fCollisionArticle;
        const Attrib::Collection *surfaceHash = cArt->GetSurface(closestBarrier->GetWSurface().Surface());

        WCollisionBarrierListEntry ble(b, surfaceHash, closestDistSq);
        cInfo.fBle = ble;

        UMath::RotateTranslate(UMath::Vector4To3(closestIntersectionPt), invMat, UMath::Vector4To3(cInfo.fCollidePt));
        cInfo.fCollidePt.w = 0.0f;

        cInfo.fBle.fB.GetNormal(UMath::Vector4To3(cInfo.fNormal));
        cInfo.fNormal.y = 0.0f;
        cInfo.fNormal.w = 0.0f;

        UMath::Vector3 testVec;
        UMath::Sub(UMath::Vector4To3(*testSegment), UMath::Vector4To3(cInfo.fCollidePt), testVec);
        if (cInfo.fNormal.x * testVec.x + cInfo.fNormal.z * testVec.z < 0.0f) {
            cInfo.fNormal.x = -cInfo.fNormal.x;
            cInfo.fNormal.z = -cInfo.fNormal.z;
        }
    }
    return cInfo.HitSomething();
}
