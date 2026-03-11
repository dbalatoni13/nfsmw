#include "Speed/Indep/Src/FEng/cFEng.h"

cFEng* cFEng::mInstance;

void cFEng::PrintLoadedPackages() {}

void cFEng::DrawForeground() {
    mFEng->Render();
}

FEPackage* cFEng::FindPackageAtBase() {
    FEPackageList* packageList = mFEng->GetPackageList();
    return packageList->GetFirstPackage();
}

FEPackage* cFEng::FindPackageActive(const char* pPackageName) {
    FEPackageList* packageList = mFEng->GetPackageList();
    return packageList->FindPackage(pPackageName);
}

FEPackage* cFEng::FindPackageIdle(const char* pPackageName) {
    return mFEng->FindIdlePackage(pPackageName);
}

void cFEng::ServiceFengOnly() {
    mFEng->Update(0, 0);
}

void cFEng::QueueGameMessage(unsigned int pMessage, const char* pPackageName, unsigned int controlMask) {
    QueueMessage(pMessage, pPackageName, nullptr, controlMask);
}
