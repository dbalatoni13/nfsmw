#include "types.h"

#include "Speed/Indep/Src/FEng/FEEvent.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEEventList::operator=(FEEventList &Src) {
    SetCount(Src.GetCount());
    FEngMemCpy(pEvent, Src.pEvent, Count * sizeof(FEEvent));
}

// STRIPPED
void FEEventList::Insert(i32 Index) {}

// STRIPPED
i32 FEEventList::AddEvent(u32 EventID, u32 Target, u32 tTime) {}

// STRIPPED
i32 FEEventList::FindEvent(u32 EventID) {}

// STRIPPED
void FEEventList::Delete(i32 Index) {}

void FEEventList::SetCount(i32 NewCount) {
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
        FEEvent *pNewList = FNEW FEEvent[NewCount];
        if (NewCount < Count) {
            FEngMemCpy(pNewList, pEvent, NewCount * sizeof(FEEvent));
        } else {
            FEngMemCpy(pNewList, pEvent, Count * sizeof(FEEvent));
            FEngMemSet(&pNewList[Count], 0, (NewCount - Count) * sizeof(FEEvent));
        }
        if (pEvent) {
            delete[] pEvent;
        }
        pEvent = pNewList;
        Count = NewCount;
    }
}

// STRIPPED
void FEEventList::SortEvents() {}
