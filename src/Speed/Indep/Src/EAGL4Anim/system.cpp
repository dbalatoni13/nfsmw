#include "eagl4runtimetransform.h"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "dolphin/mtx44_ext.h"

static void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *A, const EAGL4::Transform *B) {
    EAGL4::Transform newMtx;

    bMulMatrix(reinterpret_cast<bMatrix4 *>(&newMtx), reinterpret_cast<const bMatrix4 *>(A), reinterpret_cast<const bMatrix4 *>(B));
    bCopy(reinterpret_cast<bMatrix4 *>(result), reinterpret_cast<const bMatrix4 *>(&newMtx));
}
