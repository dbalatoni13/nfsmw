#include "Speed/Indep/Src/FEng/FEScript.h"

struct FEObject;

void FEInterpLinear(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData);
void FEInterpNone(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData);

void FEKeyInterp(FEScript* pScript, unsigned char TrackNum, long tTime, FEObject* pOutObj) {
    FEKeyTrack* pTrack = pScript->pTracks + TrackNum;

    switch (pTrack->InterpType) {
    case 0:
        FEInterpNone(pScript, TrackNum, tTime, pOutObj->pData);
        break;
    case 1:
    case 3:
        FEInterpLinear(pScript, TrackNum, tTime, pOutObj->pData);
        break;
    default:
        break;
    }
}

void FEInterpLinear(FEKeyTrack* pTrack, long tTime, void* pOutData);
void FEInterpNone(FEKeyTrack* pTrack, long tTime, void* pOutData);

void FEKeyInterp(FEKeyTrack* pTrack, long tTime, void* pOutData) {
    switch (pTrack->InterpType) {
    case 0:
        FEInterpNone(pTrack, tTime, pOutData);
        break;
    case 1:
    case 3:
        FEInterpLinear(pTrack, tTime, pOutData);
        break;
    default:
        break;
    }
}

void FEKeyInterpFast(FEKeyTrack* pTrack, long tTime, void* pOutData) {
    if (pTrack->InterpAction & 0x80) {
        return;
    }

    switch (pTrack->InterpType) {
    case 0:
        FEInterpNone(pTrack, tTime, pOutData);
        break;
    case 1:
    case 3:
        FEInterpLinear(pTrack, tTime, pOutData);
        break;
    default:
        break;
    }

    if (pTrack->DeltaKeys.GetNumElements() == 0) {
        pTrack->InterpAction |= 0x80;
    }
}
