#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

bool FEngTestForIntersection(float xPos, float yPos, FEObject* obj);

cFEngGameInterface* cFEngGameInterface::pInstance;

cFEngGameInterface::cFEngGameInterface() {
    RenderThisPackage = true;
    iGameMode = 0;
}

cFEngGameInterface::~cFEngGameInterface() {
}

void cFEngGameInterface::GenerateRenderContext(unsigned short uContext, FEObject* pObject) {
    cFEngRender::mInstance->GenerateRenderContext(uContext, pObject);
}

void cFEngGameInterface::EndPackageRendering(FEPackage* pPackage) {
    cFEngRender::mInstance->PackageFinished(pPackage);
}

unsigned long cFEngGameInterface::GetJoyPadMask(unsigned char feng_pad_index) {
    return cFEngJoyInput::mInstance->GetJoyPadMask(feng_pad_index);
}

void cFEngGameInterface::GetMouseInfo(FEMouseInfo&) {}

void cFEngGameInterface::OutputWarning(const char*, FEng_WarningLevel) {}

bool cFEngGameInterface::DoesPointTouchObject(float xPos, float yPos, FEObject* pButton) {
    return FEngTestForIntersection(xPos, yPos, pButton);
}
