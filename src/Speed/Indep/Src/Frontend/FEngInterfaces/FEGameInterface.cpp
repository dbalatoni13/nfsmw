#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/cFEng.h"

cFEngGameInterface* cFEngGameInterface::pInstance;

cFEngGameInterface::cFEngGameInterface() {
    RenderThisPackage = true;
    iGameMode = 0;
}

cFEngGameInterface::~cFEngGameInterface() {
}

void cFEngGameInterface::GetMouseInfo(FEMouseInfo&) {}


bool cFEngGameInterface::DoesPointTouchObject(float xPos, float yPos, FEObject* pButton) {
    return false;
}
