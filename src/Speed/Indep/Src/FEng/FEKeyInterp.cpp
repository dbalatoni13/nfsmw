#include "Speed/Indep/Src/FEng/FEKeyInterpolators.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEScript.h"

void FEKeyInterp(FEScript *pScript, u8 TrackNum, i32 tTime, FEObject *pOutObj) {
    FEKeyTrack *pTrack = &pScript->pTracks[TrackNum];

    switch (pTrack->InterpType) {
        case 0:
            FEInterpNone(pScript, TrackNum, tTime, pOutObj->pData);
            break;
        case 1:
        case 3:
            FEInterpLinear(pScript, TrackNum, tTime, pOutObj->pData);
            break;
        case 2:
        case 4:
        default:
            break;
    }
}

void FEKeyInterp(FEKeyTrack *pTrack, i32 tTime, void *pOutData) {
    switch (pTrack->InterpType) {
        case 0:
            FEInterpNone(pTrack, tTime, pOutData);
            break;
        case 1:
        case 3:
            FEInterpLinear(pTrack, tTime, pOutData);
            break;
        case 2:
        case 4:
        default:
            break;
    }
}

void FEKeyInterpFast(FEKeyTrack *pTrack, i32 tTime, void *pOutData) {
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
        case 2:
        case 4:
        default:
            break;
    }

    if (pTrack->DeltaKeys.GetNumElements() == 0) {
        pTrack->InterpAction |= 0x80;
    }
}
