#include "Speed/Indep/Src/FEng/FEKeyInterpolators.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEInterpNone(FEScript *pScript, u8 TrackNum, i32 tTime, void *pOutData) {
    FEKeyTrack *pTrack = pScript->pTracks + TrackNum;
    u8 *pOutDataPtr = reinterpret_cast<u8 *>(pOutData);

    u32 OutOffset = pTrack->LongOffset * 4;

    FEInterpNone(pTrack, tTime, pOutDataPtr + OutOffset);
}

void FEInterpNone(FEKeyTrack *pTrack, i32 tTime, void *pOutDataPtr) {
    u32 KeySize = pTrack->ParamSize + 4;
    FEKeyNode *pKey;
    FEKeyNode *pPrevKey;

    if (tTime <= pTrack->Length) {
        pKey = pTrack->GetKeyAt(tTime);
        pPrevKey = static_cast<FEKeyNode *>(pKey->GetPrev());
        if ((pPrevKey != nullptr) && pKey->tTime > tTime) {
            // TODO: There's only one call to getkeydata but two accesses to it. Maybe there's a better pattern here.
            FEngMemCpy(pOutDataPtr, &pPrevKey->GetKeyData()->Val, KeySize - 4);
        } else {
            FEngMemCpy(pOutDataPtr, &pKey->Val, KeySize - 4);
        }
        return;
    }

    pKey = pTrack->GetKeyAt(pTrack->Length - tTime);
    pPrevKey = static_cast<FEKeyNode *>(pKey->GetNext());
    if ((pPrevKey != nullptr) && pKey->tTime < tTime) {
        FEngMemCpy(pOutDataPtr, &pPrevKey->GetKeyData()->Val, KeySize - 4);
    } else {
        FEngMemCpy(pOutDataPtr, &pKey->Val, KeySize - 4);
    }
}
