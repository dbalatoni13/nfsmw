#include "Speed/Indep/Src/FEng/fengine.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned long FEngine::SysGUID;

FEngine::FEngine()
    : bExecuting(true) //
    , bMouseActive(false) //
    , bLoadObjectNames(false) //
    , bLoadScriptNames(false) //
    , pJoyPad(nullptr) //
    , FastRep(0) //
    , FastRepCache(0) //
    , PadHoldRegistered(0) //
    , WrapMode(kFBW_Wrap) //
    , NumJoyPads(0) //
    , pInterface(nullptr) //
    , CurrentPackageRecordIndex(0) //
    , NextButtonRecordIndex(0) //
    , bErrorScreenMode(false) //
    , bRenderedRecently(false) //
    , bDebugMessages(false) //
{
    bExecuting = true;
    bMouseActive = false;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
    FEngMemSet(HeldButtons, 0, sizeof(HeldButtons));
    Sorter.Zero();
    FEngMemSet(RecordedPackageNames, 0, sizeof(RecordedPackageNames));
    NextButtonRecordIndex = 0;
    FEngMemSet(RecordedPackageButtons, 0, sizeof(RecordedPackageButtons));
    TypeLib.Startup();
}

void FEngine::SetNumJoyPads(unsigned char Count) {
    if (pJoyPad) {
        delete[] pJoyPad;
    }
    if (Count) {
        pJoyPad = new FEJoyPad[Count];
    }
    NumJoyPads = Count;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
}

void FEngine::SetExecution(bool bProcessEverything) {
    FEPackage* pPack = PackList.GetFirstPackage();
    bExecuting = bProcessEverything;
    while (pPack) {
        pPack->bExecuting = bExecuting;
        pPack = pPack->GetNext();
    }
}

void FEngine::SetInitialState() {
    PackageInitStateCB cb;
    ForAllObjects(cb);
    unsigned char i = 0;
    while (i < NumJoyPads) {
        pJoyPad[i].Reset();
        i++;
    }
    if (bMouseActive) {
        Mouse.Reset();
    }
}

FEPackage* FEngine::FindIdlePackage(const char* pName) const {
    return static_cast<FEPackage*>(IdleList.FindNode(pName));
}

FEPackage* FEngine::FindPackageWithControl() {
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (pPack->Controllers) {
            return pPack;
        }
        pPack = pPack->GetNext();
    }
    return nullptr;
}

bool FEngine::ForAllObjects(FEObjectCallback& Callback) {
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (!pPack->ForAllObjects(Callback)) {
            return false;
        }
        pPack = pPack->GetNext();
    }
    return true;
}
