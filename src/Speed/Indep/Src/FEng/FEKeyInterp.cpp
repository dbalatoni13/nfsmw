#include "Speed/Indep/Src/FEng/FEScript.h"

struct FEObject;

void FEInterpLinear(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData);
void FEInterpNone(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData);

void FEKeyInterp(FEScript* pScript, unsigned char TrackNum, long tTime, FEObject* pOutObj) {
    unsigned char InterpType = *(reinterpret_cast<unsigned char*>(pScript->pTracks + TrackNum) + 2);
    void* pOutData = pOutObj->pData;

    if (InterpType == 2) {
        return;
    }

    if (InterpType > 2) {
        if (InterpType != 3) {
            return;
        }
    } else {
        if (InterpType == 0) {
            FEInterpNone(pScript, TrackNum, tTime, pOutData);
            return;
        }

        if (InterpType != 1) {
            return;
        }
    }

    FEInterpLinear(pScript, TrackNum, tTime, pOutData);
}

void FEInterpLinear(FEKeyTrack* pTrack, long tTime, void* pOutData);
void FEInterpNone(FEKeyTrack* pTrack, long tTime, void* pOutData);

void FEKeyInterp(FEKeyTrack* pTrack, long tTime, void* pOutData) {
    int InterpType = pTrack->InterpType;

    if (InterpType == 2) {
        return;
    }

    if (InterpType > 2) {
        if (InterpType != 3) {
            return;
        }
    } else {
        if (InterpType == 0) {
            FEInterpNone(pTrack, tTime, pOutData);
            return;
        }

        if (InterpType != 1) {
            return;
        }
    }

    FEInterpLinear(pTrack, tTime, pOutData);
}

void FEKeyInterpFast(FEKeyTrack* pTrack, long tTime, void* pOutData) {
    if (pTrack->InterpAction & 0x80) {
        return;
    }

    int InterpType = pTrack->InterpType;

    if (InterpType == 2) {
    } else if (InterpType > 2) {
        if (InterpType != 3) {
        } else {
            FEInterpLinear(pTrack, tTime, pOutData);
        }
    } else {
        if (InterpType == 0) {
            FEInterpNone(pTrack, tTime, pOutData);
        } else if (InterpType != 1) {
        } else {
            FEInterpLinear(pTrack, tTime, pOutData);
        }
    }

    if (pTrack->DeltaKeys.GetNumElements() == 0) {
        pTrack->InterpAction |= 0x80;
    }
}
