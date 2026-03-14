#include "Speed/Indep/Src/FEng/FEEvent.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FEEventList& FEEventList::operator=(FEEventList& Src) {
    SetCount(Src.Count);
    FEngMemCpy(pEvent, Src.pEvent, Count * sizeof(FEEvent));
    return *this;
}

void FEEventList::SetCount(long NewCount) {
    if (NewCount == Count) {
        return;
    }
    if (NewCount == 0) {
        if (pEvent) {
            delete pEvent;
        }
        Count = 0;
        pEvent = nullptr;
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
        Count = NewCount;
        pEvent = pNewList;
    }
}
