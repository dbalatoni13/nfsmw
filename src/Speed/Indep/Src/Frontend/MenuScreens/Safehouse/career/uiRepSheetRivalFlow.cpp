#include "uiRepSheetRivalFlow.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

void MemcardEnter(const char* from, const char* to, unsigned int op,
                  void (*pTermFunc)(void*), void* pTermFuncParam,
                  unsigned int successMsg, unsigned int failedMsg);

void RaceStarterStartCareerFreeRoam() asm("StartCareerFreeRoam__11RaceStarter");

void ShowAllCarsCarViewer() asm("ShowAllCars__9CarViewer");

static const char* ScreenNames[] = {
    "SafeHouseRivalChallenge.fng",
    "SafeHouseMarkers.fng",
    "SafeHouseRegionUnlock.fng",
    "MC_Main_GC.fng",
    "SafeHouseRivalBio.fng",
};

uiRepSheetRivalFlow* uiRepSheetRivalFlow::mInstance;

void uiRepSheetRivalFlow::Init() {
    mInstance = new uiRepSheetRivalFlow();
}

uiRepSheetRivalFlow* uiRepSheetRivalFlow::Get() {
    return mInstance;
}

uiRepSheetRivalFlow::uiRepSheetRivalFlow() {
    mStage = -1;
}

void uiRepSheetRivalFlow::StartFlow(int start_stage) {
    mStage = start_stage - 1;
    Next();
}

void uiRepSheetRivalFlow::Next() {
    int old_stage = mStage;
    mStage = old_stage + 1;

    if (mStage == 5) {
        char buf[64];
        bSNPrintf(buf, 64, "blacklist_%02d",
                  FEDatabase->GetCareerSettings()->GetCurrentBin());
        FEAnyMovieScreen::SetMovieName(buf);
        cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, false);
    } else if (mStage == 6) {
        unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        if (bin == 15) {
            FEDatabase->ClearGameMode(static_cast< eFEGameModes >(0x20000));
            FEDatabase->SetGameMode(static_cast< eFEGameModes >(1));
            ShowAllCarsCarViewer();
            FEDatabase->GetCareerSettings()->SetHasDoneCareerIntro();
            cFEng::Get()->QueuePackagePop(-1);
            cFEng::Get()->QueuePackagePush("SafeHouseReputationOverview.fng", 0, 0, false);
            mStage = -1;
        } else if (!FEDatabase->GetCareerSettings()->HasRapSheet() && bin == 13) {
            mStage = old_stage;
            FEDatabase->GetCareerSettings()->SetHasRapSheet();
            FEAnyMovieScreen::SetMovieName("blacklist_13");
            cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, false);
        } else {
            RaceStarterStartCareerFreeRoam();
        }
    } else if (mStage == 7) {
        UCrc32 kind(0x20d60dbf);
        MFlowReadyForOutro msg;
        msg.Post(kind);
        new ERaceSheetOn(0);
        FEDatabase->ClearGameMode(static_cast< eFEGameModes >(0x20000));
        mStage = -1;
    } else {
        if (mStage == 2) {
            unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            if (bin == 8 || bin == 12) {
                cFEng::Get()->QueuePackageSwitch(ScreenNames[2], 0, 0, false);
                return;
            }
        } else if (mStage == 3) {
            if (MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                MemcardEnter(nullptr, ScreenNames[mStage + 1], 0x4000b2, nullptr, nullptr, 0, 0);
                return;
            }
        } else {
            cFEng::Get()->QueuePackageSwitch(ScreenNames[mStage], 0, 0, false);
            return;
        }
        Next();
    }
}
