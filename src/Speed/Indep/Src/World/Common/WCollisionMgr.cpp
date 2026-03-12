#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

#include <algorithm>
#include <float.h>

void OrthoInverse(UMath::Matrix4 &m);

struct UTransform {
    UTransform() {}
    UTransform(const UMath::Matrix4 &m) : fTransform(m) {}
    ~UTransform() {}
    UMath::Matrix4 fTransform;
};

inline void WCollisionStrip::MakeFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const {
    const WCollisionPackedVert *v = Verts() + ind;
    retFace.fPt0.x = static_cast<float>(v->x) * (1.0f / 128.0f) + cp.x;
    retFace.fPt0.y = static_cast<float>(v->y) * (1.0f / 128.0f) + cp.y;
    retFace.fPt0.z = static_cast<float>(v->z) * (1.0f / 128.0f) + cp.z;
    retFace.fSurfaceRef = nullptr;
    v++;
    retFace.fPt1.x = static_cast<float>(v->x) * (1.0f / 128.0f) + cp.x;
    retFace.fPt1.y = static_cast<float>(v->y) * (1.0f / 128.0f) + cp.y;
    retFace.fPt1.z = static_cast<float>(v->z) * (1.0f / 128.0f) + cp.z;
    retFace.fFlags = 0;
    v++;
    retFace.fPt2.x = static_cast<float>(v->x) * (1.0f / 128.0f) + cp.x;
    retFace.fPt2.y = static_cast<float>(v->y) * (1.0f / 128.0f) + cp.y;
    retFace.fPt2.z = static_cast<float>(v->z) * (1.0f / 128.0f) + cp.z;
    retFace.fSurface = WSurface(v->surface);
}

inline void WCollisionStrip::MakeNextFace(unsigned int ind, const UMath::Vector3 &cp, WCollisionTri &retFace) const {
    const WCollisionPackedVert *v = Verts() + ind + 2;
    retFace.fPt0 = retFace.fPt1;
    retFace.fPt1 = retFace.fPt2;
    retFace.fPt2.x = static_cast<float>(v->x) * (1.0f / 128.0f) + cp.x;
    retFace.fPt2.y = static_cast<float>(v->y) * (1.0f / 128.0f) + cp.y;
    retFace.fPt2.z = static_cast<float>(v->z) * (1.0f / 128.0f) + cp.z;
    retFace.fSurface = WSurface(v->surface);
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

inline void NearPtLine(const UMath::Vector3 &pt, const UMath::Vector3 &p0, float den, const UMath::Vector3 &diffVec, UMath::Vector3 &nearPt) {
    float u = ((pt.x - p0.x) * diffVec.x + (pt.y - p0.y) * diffVec.y + (pt.z - p0.z) * diffVec.z) * den;
    u = UMath::Min(u, 1.0f);
    u = UMath::Max(u, 0.0f);
    nearPt.x = u * diffVec.x + p0.x;
    nearPt.y = u * diffVec.y + p0.y;
    nearPt.z = u * diffVec.z + p0.z;
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

bool WCollisionMgr::Collide(Dynamics::Collision::Geometry *geom, const WCollisionBarrierList *barrierList, ICollisionHandler *results,
                            void *userdata, bool force_single_sided) {
    bool hit = false;
    if (!barrierList || barrierList->empty()) {
        return hit;
    }

    const WCollisionBarrierList &barriers = *barrierList;
    UMath::Matrix4 mat = UMath::Matrix4::kIdentity;

    for (const WCollisionBarrierListEntry *iter = barriers.begin(); iter != barriers.end(); ++iter) {
        if (!SurfacePassesExclusion(iter->fB.GetWSurface())) {
            continue;
        }

        UMath::Vector4 bcp;
        iter->fB.GetCenter(bcp);

        UMath::Vector3 &vR = UMath::Vector4To3(mat[0]);
        UMath::Vector3 &vU = UMath::Vector4To3(mat[1]);
        UMath::Vector3 &vF = UMath::Vector4To3(mat[2]);

        iter->fB.GetNormal(vF);

        float w = iter->fB.GetWidth();
        float h = iter->fB.GetHeight();

        UMath::Vector3 bdim;
        memset(&bdim, 0, sizeof(bdim));
        bdim.x = w * 0.5f;
        bdim.y = h * 0.5f;

        vU = UMath::Vector3Make(0.0f, 1.0f, 0.0f);

        UMath::Cross(vU, vF, vR);

        Dynamics::Collision::Geometry bgeom(mat, UVector3(bcp), bdim, Dynamics::Collision::Geometry::BOX, UMath::Vector3::kZero);

        if (!Dynamics::Collision::Geometry::FindIntersection(geom, &bgeom, geom)) {
            continue;
        }

        if (force_single_sided || !iter->fB.GetWSurface().HasFlag(0x10)) {
            if (UMath::Dot(vF, geom->GetCollisionNormal()) <= 0.0f) {
                continue;
            }
        }

        hit = true;

        if (!results) {
            return true;
        }

        float penetration = -geom->GetOverlap();

        WorldCollisionInfo cInfo;
        cInfo.fNormal = UMath::Vector4Make(geom->GetCollisionNormal(), penetration);
        cInfo.fBle = *iter;
        cInfo.fType = 2;
        cInfo.fCollidePt = UMath::Vector4Make(geom->GetCollisionPoint(), penetration);

        if (cInfo.fCInst && cInfo.fCInst->IsDynamic()) {
            cInfo.fAnimated = true;
        }

        UMath::Vector3 dP;
        UMath::Scale(geom->GetCollisionNormal(), -geom->GetOverlap(), dP);

        UMath::Vector3 cPoint;
        cPoint = geom->GetCollisionPoint();

        if (geom->PenetratesOther()) {
            UMath::Add(cPoint, dP, cPoint);
        }

        if (results->OnWCollide(cInfo, cPoint, userdata)) {
            geom->Move(dP);
        }
    }

    return hit;
}

bool WCollisionMgr::Collide(Dynamics::Collision::Geometry *geom, const WCollisionInstanceCacheList *instanceList, ICollisionHandler *results,
                             void *userdata) {
    bool hit = false;

    if (instanceList != nullptr && !instanceList->empty()) {
        unsigned int i;
        unsigned int j;
        static const UMath::Vector4 offsets[2][4] = {
            {{-1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}},
            {{1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}}};
        unsigned int num2check;
        UMath::Vector4 arms[2][4];
        UMath::Vector4 dim = UMath::Vector4Make(geom->GetDimension(), 1.0f);
        UMath::Vector4 cp = UMath::Vector4Make(geom->GetPosition(), 1.0f);
        UMath::Vector4 seg[2];
        UMath::Vector4 delta;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 2; j++) {
                seg[0].w = 1.0f;
                UMath::Scalexyz(offsets[0][i * 2 + j], dim, seg[0]);
                UMath::Rotate(seg[0], geom->GetOrientation(), seg[1]);
                seg[1].w = 1.0f;
                UMath::Add(seg[1], cp, arms[0][i * 2 + j]);
                arms[0][i * 2 + j].w = 1.0f;
            }
        }

        delta = UMath::Vector4::kIdentity;

        for (i = 0; i < 4; i++) {
            UMath::Addxyz(arms[0][i * 2], delta, seg[0]);
            UMath::Addxyz(arms[0][i * 2 + 1], delta, seg[1]);

            WorldCollisionInfo cInfo;

            if (GetWorldNormal(instanceList, nullptr, seg, cInfo)) {
                if (results == nullptr) {
                    return true;
                }

                UMath::Vector4 penVec;
                UMath::Vector4 cPoint;
                float penetration;

                float dist0 = UMath::DistanceSquarexyz(seg[0], cInfo.fCollidePt);
                float dist1 = UMath::DistanceSquarexyz(seg[1], cInfo.fCollidePt);

                if (dist0 < dist1) {
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

                cInfo.fNormal.w = penetration;
                cPoint.w = penetration;

                if (results->OnWCollide(cInfo, UMath::Vector4To3(cPoint), userdata)) {
                    UMath::ScaleAddxyz(cInfo.fNormal, cInfo.fNormal.w, delta, delta);
                }

                hit = true;
            }
        }
    }

    return hit;
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
            float y = barrier->YTop();
            float yBot = barrier->YBot();
            if (y > yBot) {
                y = yBot;
            }
            if (intersectionPt.y > y) {
                y = barrier->YTop();
                if (y < yBot) {
                    y = yBot;
                }
                if (intersectionPt.y < y) {
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
                    if (yTop > yBot) {
                        yMin = yBot;
                    }
                    if (intersectionPt.y > yMin) {
                        if (yTop < yBot) {
                            yTop = yBot;
                        }
                        if (intersectionPt.y < yTop) {
                            float distSq = UMath::DistanceSquare(
                                UMath::Vector4To3(intersectionPt),
                                UMath::Vector4To3(tseg[0]));
                            if (distSq < closestDistSq) {
                                UMath::Copy(intersectionPt, closestIntersectionPt);
                                closestBarrier = barrier;
                                closestBarrierInst = &cInst;
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

        {
            WCollisionBarrier b;
            UMath::RotateTranslate(UMath::Vector4To3(*closestBarrier->GetPt(0)), invMat, UMath::Vector4To3(b.fPts[0]));
            UMath::RotateTranslate(UMath::Vector4To3(*closestBarrier->GetPt(1)), invMat, UMath::Vector4To3(b.fPts[1]));
            b.fPts[0].w = closestBarrier->fPts[0].w;
            b.fPts[1].w = closestBarrier->fPts[1].w;

            const Attrib::Collection *surfaceHash = closestBarrierInst->fCollisionArticle->GetSurface(b.GetWSurface().Surface());

            WCollisionBarrierListEntry ble(b, surfaceHash, closestDistSq);
            cInfo.fBle = ble;
        }

        UMath::RotateTranslate(UMath::Vector4To3(closestIntersectionPt), invMat, UMath::Vector4To3(cInfo.fCollidePt));
        cInfo.fCollidePt.w = 0.0f;

        cInfo.fBle.fB.GetNormal(UMath::Vector4To3(cInfo.fNormal));
        cInfo.fNormal.y = 0.0f;
        cInfo.fNormal.w = 0.0f;

        {
            UMath::Vector3 testVec;
            UMath::Sub(UMath::Vector4To3(*testSegment), UMath::Vector4To3(cInfo.fCollidePt), testVec);
            if (cInfo.fNormal.x * testVec.x + cInfo.fNormal.z * testVec.z < 0.0f) {
                cInfo.fNormal.x = -cInfo.fNormal.x;
                cInfo.fNormal.z = -cInfo.fNormal.z;
            }
        }
    }
    return cInfo.HitSomething();
}

void WCollisionMgr::GetBarrierList(WCollisionBarrierList &barrierList, const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pos, float radius) {
    float radiusSq = radius * radius;
    barrierList.reserve(0x15);

    for (const WCollisionInstance *const *iIter = instList.begin(); iIter != instList.end(); ++iIter) {
        const WCollisionInstance &cInst = **iIter;

        if (!InstancePassesExclusion(cInst)) continue;

        const WCollisionArticle *cArt = cInst.fCollisionArticle;
        if (cArt == nullptr || cArt->fNumEdges == 0) continue;

        UMath::Vector3 tpt;
        UMath::Matrix4 invMat;
        UTransform t;
        bool tValid = false;

        cInst.MakeMatrix(invMat, true);
        UMath::RotateTranslate(pos, invMat, tpt);

        const WCollisionBarrier *barrier = cArt->GetBarrier(0);
        for (int i = 0; i < cArt->fNumEdges; ++i) {
            if (SurfacePassesExclusion(barrier->GetWSurface())) {
                float distsqr = barrier->DistSq(tpt);
                if (distsqr < radiusSq) {
                    float yBot = barrier->YBot();
                    float yTop = barrier->YTop();
                    float yMin = yBot;
                    if (yTop < yBot) {
                        yMin = yTop;
                    }
                    if (yMin < tpt.y + radius) {
                        float yMax = yBot;
                        if (yBot < yTop) {
                            yMax = yTop;
                        }
                        if (tpt.y - radius <= yMax) {
                            if (!tValid) {
                                t.fTransform = invMat;
                                t.fTransform[0][3] = 0.0f;
                                t.fTransform[1][3] = 0.0f;
                                t.fTransform[2][3] = 0.0f;
                                t.fTransform[3][3] = 1.0f;
                                OrthoInverse(t.fTransform);
                                tValid = true;
                            }

                            WCollisionBarrier wBarrier = *barrier;
                            UMath::RotateTranslate(UMath::Vector4To3(wBarrier.fPts[0]), t.fTransform, UMath::Vector4To3(wBarrier.fPts[0]));
                            UMath::RotateTranslate(UMath::Vector4To3(wBarrier.fPts[1]), t.fTransform, UMath::Vector4To3(wBarrier.fPts[1]));
                            wBarrier.fPts[0].w = barrier->fPts[0].w;
                            wBarrier.fPts[1].w = barrier->fPts[1].w;

                            const Attrib::Collection *collection = cArt->GetSurface(wBarrier.GetWSurface().Surface());
                            WCollisionBarrierListEntry ble(wBarrier, collection, distsqr);

                            WCollisionBarrierListEntry *it = std::upper_bound(barrierList.begin(), barrierList.end(), ble);
                            barrierList.insert(it, ble);
                        }
                    }
                }
            }
            barrier = barrier->Next();
        }
    }
}

bool WCollisionMgr::FindFaceInTriStrip(const UMath::Vector3 &pt, const WCollisionStripSphere *sp, const WCollisionStrip *strip,
                                       WCollisionTri &retFace) {
    if (!StripPassesExclusion(*strip)) {
        return false;
    }

    int numTris = strip->NumTris();
    strip->MakeFace(0, sp->fPos, retFace);

    if (strip->Flags() & 2) {
        for (int i = 0; i < numTris; ) {
            const WSurface &surf = retFace.fSurface;
            if (!surf.HasFlag(8)) {
                if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&retFace))) {
                    if (SurfacePassesExclusion(surf)) {
                        if (retFace.MinY() < pt.y + 1.0f) {
                            return true;
                        }
                    }
                }
            }
            ++i;
            if (i >= numTris) break;
            strip->MakeNextFace(i, sp->fPos, retFace);
        }
    } else {
        bool rightFlag = (strip->Flags() & 1) != 0;
        strip->MakeFace(0, sp->fPos, retFace);
        for (int i = 0; i < numTris; ) {
            if (rightFlag) {
                const WSurface &surf = retFace.fSurface;
                if (!surf.HasFlag(8)) {
                    if (WWorldMath::InTriR(pt, reinterpret_cast<const UMath::Vector4 *>(&retFace))) {
                        if (SurfacePassesExclusion(surf)) {
                            return true;
                        }
                    }
                }
            } else {
                const WSurface &surf = retFace.fSurface;
                if (!surf.HasFlag(8)) {
                    if (WWorldMath::InTriL(pt, reinterpret_cast<const UMath::Vector4 *>(&retFace))) {
                        if (SurfacePassesExclusion(surf)) {
                            return true;
                        }
                    }
                }
            }
            ++i;
            rightFlag = !rightFlag;
            if (i >= numTris) break;
            strip->MakeNextFace(i, sp->fPos, retFace);
        }
    }

    return false;
}

bool WCollisionMgr::FindFaceInTriStrip(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &pt,
                                        const WCollisionStripSphere *sp, const WCollisionStrip *strip,
                                        float &faceY, WCollisionTri &retFace) {
    if (!StripPassesExclusion(*strip)) {
        return false;
    }

    int numTris = strip->NumTris();
    bool foundFace = false;
    WCollisionTri face;
    strip->MakeFace(0, sp->fPos, face);

    int retFaceInd = 0;
    float bestFaceY = 1e38f;

    UMath::RotateTranslate(face.fPt0, vectorMat, face.fPt0);
    UMath::RotateTranslate(face.fPt1, vectorMat, face.fPt1);

    for (int i = 0; i < numTris; ) {
        UMath::RotateTranslate(face.fPt2, vectorMat, face.fPt2);
        if (WWorldMath::InTri(pt, reinterpret_cast<const UMath::Vector4 *>(&face))) {
            UMath::Vector3 norm;
            CalcCollisionFaceNormal(&norm, reinterpret_cast<UMath::Vector4 *>(&face));
            float y = pt.y - WWorldMath::GetPlaneY(norm, face.fPt2, pt);
            faceY = y;
            if (y < bestFaceY && -1.0f < y && SurfacePassesExclusion(face.fSurface)) {
                bestFaceY = y;
                retFaceInd = i;
                foundFace = true;
            }
        }
        ++i;
        if (i >= numTris) break;
        strip->MakeNextFace(i, sp->fPos, face);
    }

    if (foundFace) {
        strip->MakeFace(retFaceInd, sp->fPos, retFace);
    }
    faceY = bestFaceY;
    return foundFace;
}

extern "C" void v3sub(int num, const UMath::Vector3 *src, const UMath::Vector3 *vtosub, UMath::Vector3 *results);

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
        if (!(test.mMin.x > mMax.x)) {
            if (!(test.mMin.y > mMax.y)) {
                if (!(mMin.x > test.mMax.x)) {
                    return !(mMin.y > test.mMax.y);
                }
            }
        }
        return false;
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

            AABB regionAABB(pt, radius);

            const WCollisionStripSphere *sp = cArt->GetStripSphere(0);
            for (int i = 0; i < cArt->fNumStrips; ++i, ++sp) {
                UMath::Vector3 diffVec;
                v3sub(1, &sp->fPos, &tpt, &diffVec);

                float spRadius = static_cast<float>(sp->fRadius) * (1.0f / 16.0f) + radius;
                float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
                if (dSq < spRadius * spRadius) {
                    const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(
                        reinterpret_cast<const char *>(cArt) + sp->Offset());
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
                            float side;

                            side = PtDir(pt, face.fPt0, face.fPt1);
                            if (!(side * dir > 0.0f)) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt0, face.fPt1, nearPt);
                                if (!(side * dir > 0.0f)) {
                                    if (XZDistSq(pt, nearPt) >= radiusSq) {
                                        goto next_tri;
                                    }
                                }
                            }

                            side = PtDir(pt, face.fPt1, face.fPt2);
                            if (!(side * dir > 0.0f)) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt1, face.fPt2, nearPt);
                                if (!(side * dir > 0.0f)) {
                                    if (XZDistSq(pt, nearPt) >= radiusSq) {
                                        goto next_tri;
                                    }
                                }
                            }

                            side = PtDir(pt, face.fPt2, face.fPt0);
                            if (!(side * dir > 0.0f)) {
                                WWorldMath::NearestPointLine2D3(pt, face.fPt2, face.fPt0, nearPt);
                                if (!(side * dir > 0.0f)) {
                                    if (XZDistSq(pt, nearPt) >= radiusSq) {
                                        goto next_tri;
                                    }
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

inline void MakeWorldSpaceFace(WCollisionTri &worldFace, const WCollisionTri &localFace, const UMath::Matrix4 &invMat) {
    UTransform t(invMat);
    OrthoInverse(t.fTransform);
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

    if (cArt == nullptr || cArt->fNumStrips == 0 ||
        tpt.x > cInst.fInvMatRow0Width.w || tpt.x < -cInst.fInvMatRow0Width.w ||
        tpt.z > cInst.fInvMatRow2Length.w || tpt.z < -cInst.fInvMatRow2Length.w) {
        return false;
    }

    WCollisionTri retVal;
    bool foundFace;
    float leastYDist = 1e38f;
    foundFace = false;
    const WCollisionStripSphere *sp = cArt->GetStripSphere(0);

    for (int i = 0; i < cArt->fNumStrips; ++sp, ++i) {
        UMath::Vector3 diffVec;
        UMath::Sub(sp->fPos, tpt, diffVec);
        float radius = static_cast<float>(static_cast<int>(sp->fRadius)) * (1.0f / 16.0f);
        float dSq = diffVec.x * diffVec.x + diffVec.z * diffVec.z;
        float radsSq = radius * radius;
        if (dSq < radsSq) {
            const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(
                reinterpret_cast<const char *>(cArt) + sp->Offset());
            WCollisionTri face;
            if (FindFaceInTriStrip(tpt, sp, strip, face)) {
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
                float dist = tpt.y - WWorldMath::GetPlaneY(norm, face.fPt0, tpt);

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

bool WCollisionMgr::FindFaceInCInst(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &endPt,
                                    const WCollisionInstance &cInst, WCollisionTri &retFace, float &retDist) {
    UMath::Matrix4 invMat;
    cInst.MakeMatrix(invMat, true);

    UTransform mat(invMat);
    OrthoInverse(mat.fTransform);
    UTransform vecMatInv(vectorMat);
    OrthoInverse(vecMatInv.fTransform);

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

    const UMath::Vector3 &startPt = UMath::Vector4To3(vectorMat.v3);
    bool foundFace = false;
    UMath::RotateTranslate(startPt, invMat, tp0);

    const WCollisionStripSphere *sp = cArt->GetStripSphere(0);
    UMath::RotateTranslate(endPt, invMat, tp1);

    int i = 0;

    UMath::Vector3 npVec;
    UMath::Sub(tp1, tp0, npVec);
    float invDen = 1.0f / (npVec.x * npVec.x + npVec.y * npVec.y + npVec.z * npVec.z);

    for (; i < cArt->fNumStrips; ++i, ++sp) {
        float radius = static_cast<float>(static_cast<int>(sp->fRadius)) * (1.0f / 16.0f);
        float radsSq = radius * radius;

        UMath::Vector3 diffVec;
        UMath::Vector3 nearPt;
        NearPtLine(sp->fPos, tp0, invDen, npVec, nearPt);
        UMath::Sub(sp->fPos, nearPt, diffVec);
        float dSq = diffVec.x * diffVec.x + diffVec.y * diffVec.y + diffVec.z * diffVec.z;

        if (dSq < radsSq) {
            const WCollisionStrip *strip = reinterpret_cast<const WCollisionStrip *>(
                reinterpret_cast<const char *>(cArt) + sp->Offset());
            WCollisionTri face;
            float faceY;
            if (FindFaceInTriStrip(combinedMat, UMath::Vector3::kZero, sp, strip, faceY, face)) {
                if (0.0f < faceY && faceY < leastYDist) {
                    leastYDist = faceY;
                    retVal = face;
                    retDist = faceY;
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
