#include "Speed/Indep/Src/FEng/FEKeyTrack.h"

FEKeyNode* FEKeyTrack::GetKeyAt(long tTime) {
    if (tTime > -1) {
        FEKeyNode* pKey = GetFirstDeltaKey();
        if (pKey) {
            FEKeyNode* pPrev;
            do {
                pPrev = pKey;
                if (!pPrev->GetNext()) {
                    return pPrev;
                }
                pKey = pPrev->GetNext();
            } while (pPrev->tTime < tTime);
            return pPrev;
        }
    }
    return GetBaseKey();
}

FEKeyNode* FEKeyTrack::GetDeltaKeyAt(long tTime) {
    FEKeyNode* pKey = GetFirstDeltaKey();
    FEKeyNode* pPrev;
    if (!pKey) {
        pPrev = nullptr;
    } else {
        do {
            pPrev = pKey;
            if (!pPrev->GetNext()) {
                return pPrev;
            }
            pKey = pPrev->GetNext();
        } while (pPrev->tTime < tTime);
    }
    return pPrev;
}
