#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEButtonMap::SetCount(unsigned long NewCount) {
    if (pList) {
        delete[] pList;
    }
    pList = nullptr;
    if (NewCount != 0) {
        pList = static_cast<FEObject**>(FEngMalloc(NewCount * 4, nullptr, 0));
    }
    Count = NewCount;
}
