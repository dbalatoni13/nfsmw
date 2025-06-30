#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/mtx44_ext.h"

bMatrix4 eMathIdentityMatrix;
bMatrix4 eMathZeroMatrix;

void eMathInit(void) {
    bMatrix4 *identity = eGetIdentityMatrix();
    bMatrix4 *zero = eGetZeroMatrix();
    MTX44Identity(*reinterpret_cast<Mtx44 *>(identity));
    bMemSet(zero, 0, sizeof(*zero));
}

void eCopyMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bCopy(dest, src);
}

void eMulMatrix(bMatrix4 *ab, bMatrix4 *a, bMatrix4 *b) {
    MTX44Concat(*reinterpret_cast<Mtx44 *>(a), *reinterpret_cast<Mtx44 *>(b), *reinterpret_cast<Mtx44 *>(ab));
}
