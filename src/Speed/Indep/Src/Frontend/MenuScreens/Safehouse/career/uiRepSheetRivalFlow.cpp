#include "uiRepSheetRivalFlow.hpp"

#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

uiRepSheetRivalFlow *uiRepSheetRivalFlow::mInstance = nullptr;

static const char *ScreenNames[] = {
    "SafeHouseRivalChallenge.fng", "SafeHouseMarkers.fng", "SafeHouseRegionUnlock.fng", "MC_Main_GC.fng", "SafeHouseRivalBio.fng",
};

void uiRepSheetRivalFlow::Init() {
    mInstance = new ("uiRepSheetRivalFlow", 0) uiRepSheetRivalFlow();
}

uiRepSheetRivalFlow *uiRepSheetRivalFlow::Get() {
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
    mStage++;

    if (mStage == 5) {
        char buf[64];
        bSNPrintf(buf, 64, "blacklist_%02d", FEDatabase->GetCareerSettings()->GetCurrentBin());
        FEAnyMovieScreen::SetMovieName(buf);
        cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, false);
    } else if (mStage == 6) {
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 15) {
            FEDatabase->ClearGameMode(eFE_GAME_MODE_POST_RIVAL);
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
            CarViewer::ShowAllCars();
            FEDatabase->GetCareerSettings()->SetHasDoneCareerIntro();
            cFEng::Get()->QueuePackagePop(-1);
            cFEng::Get()->QueuePackagePush("SafeHouseReputationOverview.fng", 0, 0, false);
            mStage = -1;
        } else if (!FEDatabase->GetCareerSettings()->HasRapSheet() && FEDatabase->GetCareerSettings()->GetCurrentBin() == 13) {
            mStage--;
            FEDatabase->GetCareerSettings()->SetHasRapSheet();
            FEAnyMovieScreen::SetMovieName("storyfmv_rap30");
            cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, false);
        } else {
            RaceStarter::StartCareerFreeRoam();
        }
    } else if (mStage == 7) {
        MFlowReadyForOutro().Post(0x20d60dbf);
        new ERaceSheetOn(0);
        FEDatabase->ClearGameMode(eFE_GAME_MODE_POST_RIVAL);
        mStage = -1;
    } else if (mStage == 2) {
        int cur_stage = FEDatabase->GetCareerSettings()->GetCurrentBin() + 1;
        if (cur_stage == 9 || cur_stage == 13) {
            cFEng::Get()->QueuePackageSwitch(ScreenNames[2], 0, 0, false);
            return;
        }
        Next();
    } else if (mStage == 3) {
        if (MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
            MemcardEnter(nullptr, ScreenNames[mStage + 1], 0x4000b2, nullptr, nullptr, 0, 0);
            return;
        }
        Next();
    } else {
        cFEng::Get()->QueuePackageSwitch(ScreenNames[mStage], 0, 0, false);
    }
}
