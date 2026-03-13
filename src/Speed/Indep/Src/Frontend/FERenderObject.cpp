#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"

void FERenderObject::SetTransform(bMatrix4 *pMatrix) {
    bMemCpy(&mstTransform, pMatrix, sizeof(bMatrix4));
}