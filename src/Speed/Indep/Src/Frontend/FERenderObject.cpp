#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"

bVector4 V4Mult(const bVector4 &v, float d) {
    return bVector4(v.x * d, v.y * d, v.z * d, v.w * d);
}

void FERenderObject::SetTransform(bMatrix4 *pMatrix) {
    bMemCpy(&mstTransform, pMatrix, sizeof(bMatrix4));
}