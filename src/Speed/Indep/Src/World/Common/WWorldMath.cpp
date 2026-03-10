#include "Speed/Indep/Src/World/WWorldMath.h"

float WWorldMath::GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint) {
    if (normal.y == 0.0f) {
        return pointOnPlane.y;
    }
    return pointOnPlane.y - (normal.x * (testPoint.x - pointOnPlane.x) + normal.z * (testPoint.z - pointOnPlane.z)) / normal.y;
}
