#include "eagl4runtimetransform.h"

#include "Speed/Indep/bWare/Inc/bMath.hpp"

void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *first, const EAGL4::Transform *second) {
    EAGL4::Transform newMtx;

    bMulMatrix(reinterpret_cast<bMatrix4 *>(&newMtx), reinterpret_cast<const bMatrix4 *>(first),
               reinterpret_cast<const bMatrix4 *>(second));
    bCopy(reinterpret_cast<bMatrix4 *>(result), reinterpret_cast<const bMatrix4 *>(&newMtx));
}
