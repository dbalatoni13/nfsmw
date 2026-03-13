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
