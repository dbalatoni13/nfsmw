#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/WWorldPos.h"

#include <float.h>

static void CalcCollisionFaceNormal(UMath::Vector3 *norm, UMath::Vector4 *facePts) {
    UMath::Vector3 vecX;
    UMath::Vector3 vecZ;
    UMath::Vector3 normal;
    vecX.x = facePts[1].x - facePts[0].x;
    vecX.y = facePts[1].y - facePts[0].y;
    vecZ.z = facePts[0].z - facePts[2].z;
    vecX.z = facePts[1].z - facePts[0].z;
    vecZ.x = facePts[0].x - facePts[2].x;
    vecZ.y = facePts[0].y - facePts[2].y;
    UMath::Cross(vecX, vecZ, normal);
    if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
        norm->z = 0.0f;
        norm->x = 0.0f;
        norm->y = 1.0f;
    } else {
        VU0_v3unit(normal, *norm);
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

        if (!cInfo.HitSomething() || cInfo.fType != 1) {
            return false;
        }
        height = cInfo.fCollidePt.y;
        if (normal != nullptr) {
            *normal = UMath::Vector4To3(cInfo.fNormal);
        }
    }
    return true;
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
