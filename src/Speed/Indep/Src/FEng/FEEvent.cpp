#include "types.h"

#include "Speed/Indep/Src/FEng/FEEvent.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEEventList::operator=(FEEventList &Src) {
    SetCount(Src.GetCount());
    FEngMemCpy(pEvent, Src.pEvent, Count * sizeof(FEEvent));
}

// STRIPPED
// AUSENTE del objetivo: zFEng.o del original no trae este simbolo y ninguna de
// las 526 unidades lo referencia. El cuerpo vacio emitia un `blr` de 4 B que
// objdiff no compara y que sobraba en .text.
// void FEEventList::Insert(i32 Index) {}

// STRIPPED
// AUSENTE del objetivo: zFEng.o del original no trae este simbolo y ninguna de
// las 526 unidades lo referencia. El cuerpo vacio emitia un `blr` de 4 B que
// objdiff no compara y que sobraba en .text.
// i32 FEEventList::AddEvent(u32 EventID, u32 Target, u32 tTime) {}

// STRIPPED
// AUSENTE del objetivo: zFEng.o del original no trae este simbolo y ninguna de
// las 526 unidades lo referencia. El cuerpo vacio emitia un `blr` de 4 B que
// objdiff no compara y que sobraba en .text.
// i32 FEEventList::FindEvent(u32 EventID) {}

// STRIPPED
// AUSENTE del objetivo: zFEng.o del original no trae este simbolo y ninguna de
// las 526 unidades lo referencia. El cuerpo vacio emitia un `blr` de 4 B que
// objdiff no compara y que sobraba en .text.
// void FEEventList::Delete(i32 Index) {}

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
// AUSENTE del objetivo: zFEng.o del original no trae este simbolo y ninguna de
// las 526 unidades lo referencia. El cuerpo vacio emitia un `blr` de 4 B que
// objdiff no compara y que sobraba en .text.
// void FEEventList::SortEvents() {}
