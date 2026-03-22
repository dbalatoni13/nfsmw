#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

#include <cstring>

void SoundPause(bool pause, eSNDPAUSE_REASON reason);
void SetSoundControlState(bool set, eSNDCTLSTATE state, const char* name);
void HideEverySingleHud();
int bStrCmp(const char* s1, const char* s2);
FEPackageRenderInfo* HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage* pkg);

Timer MessengerCreationTimer(0);
extern float RealTimeElapsed;

cFEng* cFEng::mInstance;

void cFEng::Init() {
    if (mInstance == nullptr) {
        mInstance = new cFEng();
        MessengerCreationTimer.ResetLow();
    }
}

cFEng::cFEng() {
    bWasPaused = false;
    mFEng = new FEngine();
    mFEng->SetInterface(cFEngGameInterface::pInstance);
    mFEng->SetNumJoyPads(2);
    mFEng->SetInitialState();
    mFEng->SetExecution(true);
}

void cFEng::PushErrorPackage(const char* pPackageName, int pArg, unsigned long ControlMask) {
    if (FEDatabase == nullptr) {
        if (cFEng::Get()->IsPackagePushed(pPackageName)) {
            return;
        }
        FEPackageManager::Get()->SetPackageDataArg(pPackageName, pArg);
        FEPackage* pPackage = mFEng->PushPackage(pPackageName, FE_PACKAGE_PRIORITY_ERROR, ControlMask);
        pPackage->SetErrorScreen(true);
        mFEng->ToggleErrorScreenMode(true);
        if (!FEManager::IsPaused() || bWasPaused) {
            bWasPaused = true;
            FEManager::RequestPauseSimulation(pPackageName);
            PauseAllSystems();
        }
    } else {
        FEPackageManager::Get()->SetPackageDataArg(pPackageName, pArg);
        FEPackage* pPackage = mFEng->PushPackage(pPackageName, FE_PACKAGE_PRIORITY_ERROR, ControlMask);
        pPackage->SetErrorScreen(true);
        mFEng->ToggleErrorScreenMode(true);
        if (!FEManager::IsPaused() || bWasPaused) {
            bWasPaused = true;
            FEManager::RequestPauseSimulation(pPackageName);
            PauseAllSystems();
        }
    }
}

void cFEng::PopErrorPackage() {
    if (FEDatabase == nullptr) {
        mFEng->QueuePackagePop();
        mFEng->ToggleErrorScreenMode(false);
        ServiceFengOnly();
        if (bWasPaused) {
            ResumeAllSystems(false);
            FEManager::RequestUnPauseSimulation(nullptr);
        }
    } else {
        mFEng->QueuePackagePop();
        mFEng->ToggleErrorScreenMode(false);
        ServiceFengOnly();
        if (bWasPaused) {
            ResumeAllSystems(true);
            if (FEManager::IsPaused()) {
                FEManager::RequestUnPauseSimulation(nullptr);
            }
        }
    }
}

void cFEng::PopErrorPackage(int port) {
    mFEng->QueuePackagePop();
    mFEng->ToggleErrorScreenMode(false);
    FEManager* feManager = FEManager::Get();
    if (port != -1) {
        feManager->ClearControllerError(port);
    }
    if (bWasPaused) {
        if (!feManager->WaitingForControllerError()) {
            ResumeAllSystems(true);
        }
        FEManager::RequestUnPauseSimulation(nullptr);
    }
}

void cFEng::PauseAllSystems() {
    if (UTL::Collections::Singleton<INIS>::Get()) {
        UTL::Collections::Singleton<INIS>::Get()->Pause();
    }
    SoundPause(true, static_cast<eSNDPAUSE_REASON>(-1));
    SetSoundControlState(true, SNDSTATE_ERROR, "PauseAllSystems");
}

void cFEng::ResumeAllSystems(bool flushActions) {
    SoundPause(false, static_cast<eSNDPAUSE_REASON>(-1));
    SetSoundControlState(false, SNDSTATE_ERROR, "PauseAllSystems");
    if (UTL::Collections::Singleton<INIS>::Get()) {
        UTL::Collections::Singleton<INIS>::Get()->UnPause();
    }
    if (flushActions) {
        cFEngJoyInput::Get()->FlushActions();
    }
}

void cFEng::QueuePackagePush(const char* pPackageName, int pArg, unsigned long ControlMask, bool pSuppressSimPause) {
    PrintLoadedPackages();
    if (TheGameFlowManager.IsInGame() && !pSuppressSimPause && !FEManager::IsPaused()) {
        FEManager::RequestPauseSimulation(pPackageName);
        HideEverySingleHud();
        bool push_bkg = IsPackagePushed("InGameBackground.fng");
        if (!push_bkg) {
            mFEng->QueuePackagePush("InGameBackground.fng", 0);
        }
    }
    FEPackageManager::Get()->SetPackageDataArg(pPackageName, pArg);
    mFEng->QueuePackagePush(pPackageName, ControlMask);
    if (!cFEngJoyInput::Get()->IsJoyEnabled(kJP_NumPorts)) {
        cFEngJoyInput::Get()->JoyEnable(kJP_NumPorts, true);
    }
}

void cFEng::QueuePackagePop(int numPackagesToPop) {
    const int numPackagesPushed = mFEng->GetNumPackagesBelowPriority(FE_PACKAGE_PRIORITY_FIFTH_CLOSEST);
    if (numPackagesToPop < 1 || numPackagesToPop > numPackagesPushed) {
        numPackagesToPop = numPackagesPushed;
    }
    PrintLoadedPackages();
    for (int i = 0; i < numPackagesToPop; i++) {
        mFEng->QueuePackagePop();
    }
    if (mFEng->GetNumPackagesBelowPriority(FE_PACKAGE_PRIORITY_FIFTH_CLOSEST) < 1) {
        if (TheGameFlowManager.IsInGame() && FEManager::IsPaused()) {
            FEManager::RequestUnPauseSimulation(nullptr);
        }
        if (cFEngJoyInput::Get()->IsJoyEnabled(kJP_NumPorts)) {
            cFEngJoyInput::Get()->JoyDisable(kJP_NumPorts, true);
        }
    }
}

void cFEng::QueuePackageSwitch(const char* pPackageName, int pArg, unsigned long ControlMask, bool pSuppressSimPause) {
    PrintLoadedPackages();
    if (TheGameFlowManager.IsInGame() && !pSuppressSimPause && !FEManager::IsPaused()) {
        FEManager::RequestPauseSimulation(pPackageName);
        HideEverySingleHud();
        bool push_bkg = IsPackagePushed("InGameBackground.fng");
        if (!push_bkg) {
            mFEng->QueuePackagePush("InGameBackground.fng", 0);
        }
    }
    FEPackageManager::Get()->SetPackageDataArg(pPackageName, pArg);
    mFEng->QueuePackageSwitch(pPackageName, ControlMask);
    if (cFEngJoyInput::Get() && !cFEngJoyInput::Get()->IsJoyEnabled(kJP_NumPorts)) {
        cFEngJoyInput::Get()->JoyEnable(kJP_NumPorts, true);
    }
}

void cFEng::PushNoControlPackage(const char* pPackageName, FE_PACKAGE_PRIORITY pPriority) {
    mFEng->PushPackage(pPackageName, pPriority, 0);
}

void cFEng::PopNoControlPackage(const char* pPackageName) {
    FEPackage* packageToPop = FEPackageManager::Get()->FindPackage(pPackageName);
    mFEng->UnloadPackage(packageToPop);
}

void cFEng::Service() {
    mFEng->Update(static_cast<long>(RealTimeElapsed * 1000.0f),
                  IsGameFlowInGame() && WorldTimeElapsed > 0.0f);
}

void cFEng::ServiceFengOnly() {
    mFEng->Update(0, 0);
}

void cFEng::DrawForeground() {
    mFEng->Render();
}

FEPackage* cFEng::FindPackageWithControl() {
    FEPackageList* packageList = mFEng->GetPackageList();
    if (packageList != nullptr) {
        for (FEPackage* package = packageList->GetLastPackage(); package != nullptr; package = package->GetPrev()) {
            if (package->GetControlMask() != 0) {
                return package;
            }
        }
    }
    return nullptr;
}

FEPackage* cFEng::FindPackageAtBase() {
    FEPackageList* packageList = mFEng->GetPackageList();
    if (packageList != nullptr) {
        return packageList->GetFirstPackage();
    }
    return nullptr;
}

FEPackage* cFEng::FindPackageActive(const char* pPackageName) {
    FEPackageList* packageList = mFEng->GetPackageList();
    return packageList->FindPackage(pPackageName);
}

FEPackage* cFEng::FindPackageIdle(const char* pPackageName) {
    return mFEng->FindIdlePackage(pPackageName);
}

FEPackage* cFEng::FindPackage(const char* pPackageName) {
    if (pPackageName != nullptr && strlen(pPackageName) != 0) {
        if (!FEPackageData::IsInScreenConstructor()) {
            FEPackage* package = FindPackageActive(pPackageName);
            if (package != nullptr) {
                return package;
            }
            package = FindPackageIdle(pPackageName);
            if (package != nullptr) {
                return package;
            }
        } else {
            return FEPackageManager::Get()->FindPackage(pPackageName);
        }
    }
    return nullptr;
}

bool cFEng::IsPackagePushed(const char* pPackageName) {
    FEPackage* package;
    if (FEPackageData::IsInScreenConstructor()) {
        package = FEPackageManager::Get()->FindPackage(pPackageName);
    } else {
        package = FindPackageActive(pPackageName);
    }
    return package != nullptr;
}

bool cFEng::IsPackageInControl(const char* pPackageName) {
    FEPackage* packageWithCtrl = FindPackageWithControl();
    if (packageWithCtrl != nullptr) {
        return bStrCmp(pPackageName, packageWithCtrl->GetName()) == 0;
    }
    return false;
}

void cFEng::PrintLoadedPackages() {}

void cFEng::QueueMessage(unsigned int pMessage, const char* pPackageName, FEObject* to, unsigned int controlMask) {
    if (pPackageName != nullptr) {
        FEPackage* package = FindPackage(pPackageName);
        if (package != nullptr) {
            mFEng->QueueMessage(pMessage, nullptr, package, to, controlMask);
        }
    } else {
        FEPackageList* pkg_list = mFEng->GetPackageList();
        if (pkg_list != nullptr) {
            for (FEPackage* pkg = pkg_list->GetFirstPackage(); pkg != nullptr; pkg = pkg->GetNext()) {
                mFEng->QueueMessage(pMessage, nullptr, pkg, to, 0);
            }
        }
    }
}

void cFEng::QueueGameMessage(unsigned int pMessage, const char* pPackageName, unsigned int controlMask) {
    QueueMessage(pMessage, pPackageName, reinterpret_cast<FEObject*>(-1), controlMask);
}

void cFEng::QueueGameMessagePkg(unsigned int pMessage, FEPackage* topkg) {
    mFEng->QueueMessage(pMessage, nullptr, topkg, reinterpret_cast<FEObject*>(-1), 0);
}

void cFEng::QueuePackageMessage(unsigned int pMessage, const char* pPackageName, FEObject* obj) {
    if (obj != nullptr) {
        QueueMessage(pMessage, pPackageName, obj, 0xFF);
    } else {
        QueueMessage(pMessage, pPackageName, reinterpret_cast<FEObject*>(-4), 0xFF);
    }
}

void cFEng::QueueSoundMessage(unsigned int pMessage, const char* pPackageName) {
    FEPackage* package = FindPackage(pPackageName);
    if (package != nullptr) {
        mFEng->QueueMessage(pMessage, nullptr, package, reinterpret_cast<FEObject*>(-5), 0);
    }
}

void cFEng::MakeLoadedPackagesDirty() {
    FEPackageList* pkg_list = mFEng->GetPackageList();
    if (pkg_list != nullptr) {
        ObjectDirtySetter dirt;
        for (FEPackage* pkg = pkg_list->GetFirstPackage(); pkg != nullptr; pkg = pkg->GetNext()) {
            dirt.pRenderInfo = HACK_FEPkgMgr_GetPackageRenderInfo(pkg);
            pkg->ForAllObjects(dirt);
        }
    }
}
