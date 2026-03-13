#include "Speed/Indep/Src/FEng/FEScript.h"

void FEInterpLinear(FEKeyTrack* pTrack, long tTime, void* pOutData);

void FEInterpLinear(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData) {
    FEKeyTrack* pTrack = pScript->pTracks + TrackNum;
    char* pData = reinterpret_cast<char*>(pOutData);

    FEInterpLinear(pTrack, tTime, pData + *(reinterpret_cast<char*>(pTrack) + 7) * 4);
}
