#include "eagl4runtimetransform.h"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "dolphin/mtx44_ext.h"

static void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *A, const EAGL4::Transform *B) {
    EAGL4::Transform temp;
    const Mtx44 &src = *reinterpret_cast<const Mtx44 *>(&temp);
    Mtx44 &dst = *reinterpret_cast<Mtx44 *>(result);

    bMulMatrix(reinterpret_cast<bMatrix4 *>(&temp), reinterpret_cast<const bMatrix4 *>(A), reinterpret_cast<const bMatrix4 *>(B));
    PSMTX44Copy(src, dst);
}
