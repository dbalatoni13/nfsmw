#include "Speed/Indep/Src/FEng/FEEvent.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "types.h"

void FEEventList::operator=(FEEventList& Src) {
    SetCount(Src.GetCount());
    FEngMemCpy(pEvent, Src.pEvent, Count * sizeof(FEEvent));
}

void FEEventList::SetCount(long NewCount) {
    if (NewCount == Count) {
        return;
    }
    if (NewCount == 0) {
        if (pEvent) {
            delete pEvent;
        }
        pEvent = nullptr;
        Count = 0;
    } else {
        FEEvent* pNewList = static_cast<FEEvent*>(FEngMalloc(NewCount * sizeof(FEEvent), nullptr, 0));
        if (NewCount < Count) {
            FEngMemCpy(pNewList, pEvent, NewCount * sizeof(FEEvent));
        } else {
            FEngMemCpy(pNewList, pEvent, Count * sizeof(FEEvent));
            FEngMemSet(reinterpret_cast<char*>(pNewList) + Count * sizeof(FEEvent), 0, (NewCount - Count) * sizeof(FEEvent));
        }
        if (pEvent) {
            delete[] pEvent;
        }
        pEvent = pNewList;
        Count = NewCount;
    }
}
