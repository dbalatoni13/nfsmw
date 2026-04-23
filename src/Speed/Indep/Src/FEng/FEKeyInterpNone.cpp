#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEInterpNone(FEKeyTrack* pTrack, long tTime, void* pOutDataPtr) {
    unsigned long KeySize = pTrack->ParamSize + 4;
    FEKeyNode* pKey;
    FEKeyNode* pPrevKey;

    if (tTime <= pTrack->Length) {
        pKey = pTrack->GetKeyAt(tTime);
        pPrevKey = static_cast<FEKeyNode*>(pKey->GetPrev());
        if (pPrevKey && pKey->tTime > tTime) {
            FEngMemCpy(pOutDataPtr, &pPrevKey->Val, KeySize - 4);
        } else {
            FEngMemCpy(pOutDataPtr, &pKey->Val, KeySize - 4);
        }
        return;
    }

    pKey = pTrack->GetKeyAt(pTrack->Length - tTime);
    pPrevKey = static_cast<FEKeyNode*>(pKey->GetNext());
    if (pPrevKey && pKey->tTime < tTime) {
        FEngMemCpy(pOutDataPtr, &pPrevKey->Val, KeySize - 4);
    } else {
        FEngMemCpy(pOutDataPtr, &pKey->Val, KeySize - 4);
    }
}

void FEInterpNone(FEScript* pScript, unsigned char TrackNum, long tTime, void* pOutData) {
    FEKeyTrack* pTrack = pScript->pTracks + TrackNum;
    char* pData = reinterpret_cast<char*>(pOutData);

    FEInterpNone(pTrack, tTime, pData + *(reinterpret_cast<char*>(pTrack) + 7) * 4);
}
