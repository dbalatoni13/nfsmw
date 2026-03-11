#include "Speed/Indep/Src/World/WCollisionTri.h"

void WCollisionTriList::clear_all() {
    WCollisionTriBlock **i = begin();
    while (i != end()) {
        delete *i;
        i++;
    }
    clear();
    mCurrBlock = nullptr;
}