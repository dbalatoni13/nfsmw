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
