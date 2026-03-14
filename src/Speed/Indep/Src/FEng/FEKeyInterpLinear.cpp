#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FETypes.h"

void FEInterpLinear(FEKeyTrack* pTrack, long tTime, void* pOutData);

void FEInterpLinear(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData) {
    FEKeyTrack* pTrack = pScript->pTracks + TrackNum;
    char* pData = reinterpret_cast<char*>(pOutData);

    FEInterpLinear(pTrack, tTime, pData + *(reinterpret_cast<char*>(pTrack) + 7) * 4);
}

void FELerpInteger(int n1, int n2, float t, long* pOffset, long* pDest) {
    *pDest = *pOffset + n1 + static_cast<int>(static_cast<float>(n2 - n1) * t + 0.5f);
}

void FELerpFloat(float n1, float n2, float t, float* pOffset, float* pDest) {
    *pDest = *pOffset + n1 + (n2 - n1) * t;
}

void FELerpVector2(FEVector2& v1, FEVector2& v2, float t, FEVector2* pOffset, FEVector2* pDest) {
    pDest->x = pOffset->x + v1.x + (v2.x - v1.x) * t;
    pDest->y = pOffset->y + v1.y + (v2.y - v1.y) * t;
}

void FELerpVector3(FEVector3& v1, FEVector3& v2, float t, FEVector3* pOffset, FEVector3* pDest) {
    pDest->x = pOffset->x + v1.x + (v2.x - v1.x) * t;
    pDest->y = pOffset->y + v1.y + (v2.y - v1.y) * t;
    pDest->z = pOffset->z + v1.z + (v2.z - v1.z) * t;
}
