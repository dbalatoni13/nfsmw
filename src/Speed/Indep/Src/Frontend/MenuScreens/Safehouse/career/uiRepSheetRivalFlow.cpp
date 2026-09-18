#include "uiRepSheetRivalFlow.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Event.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"



uiRepSheetRivalFlow *uiRepSheetRivalFlow::mInstance = 0;

const char *ScreenNames[8] = {
    "SafeHouseRivalChallenge.fng",
    "SafeHouseMarkers.fng",
    "SafeHouseRegionUnlock.fng",
    "MC_Main_GC.fng",
    "SafeHouseRivalBio.fng",
    "",
    "",
    "SafeHouseReputationOverview.fng",
};


void uiRepSheetRivalFlow::Init() {
    mInstance = new ("uiRepSheetRivalFlow", 0) uiRepSheetRivalFlow;
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
    ++mStage;
    if (mStage == BIO_MOVIE) {
        char buf[64];
        bSNPrintf(buf, 0x40, "blacklist_%02d", FEDatabase->GetCareerSettings()->GetCurrentBin());
        FEAnyMovieScreen::SetMovieName(buf);
        cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, 0);
    } else if (mStage == BACK_TO_FREE_ROAM) {
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 15) {
            FEDatabase->ClearGameMode(eFE_GAME_MODE_POST_RIVAL);
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
            CarViewer::ShowAllCars();
            FEDatabase->GetCareerSettings()->SetHasDoneCareerIntro();
            cFEng::Get()->QueuePackagePop(-1);
            cFEng::Get()->QueuePackagePush("SafeHouseReputationOverview.fng", 0, 0, 0);
            mStage = -1;
        } else if (!FEDatabase->GetCareerSettings()->HasRapSheet() && FEDatabase->GetCareerSettings()->GetCurrentBin() == 13) {
            mStage--;
            FEDatabase->GetCareerSettings()->SetHasRapSheet();
            FEAnyMovieScreen::SetMovieName("storyfmv_rap30");
            cFEng::Get()->QueuePackageSwitch(FEAnyMovieScreen::GetFEngPackageName(), 0, 0, 0);
        } else {
            RaceStarter::StartCareerFreeRoam();
        }
    } else if (mStage == IN_GAME_BLACKLIST) {
        MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
        new ERaceSheetOn(0);
        FEDatabase->ClearGameMode(eFE_GAME_MODE_POST_RIVAL);
        mStage = -1;
    } else if (mStage == REGION_UNLOCK) {
        int cur_stage = FEDatabase->GetCareerSettings()->GetCurrentBin() + 1;
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 8 || cur_stage == 13) {
            cFEng::Get()->QueuePackageSwitch(ScreenNames[2], 0, 0, 0);
        } else {
            Next();
        }
    } else if (mStage == SAVE_FLOW) {
        if (MemoryCard::s_pThis->ShouldDoAutoSave(false)) {
            MemcardEnter(0, ScreenNames[mStage + 1], 0x4000B2, 0, 0, 0, 0);
        } else {
            Next();
        }
    } else {
        cFEng::Get()->QueuePackageSwitch(ScreenNames[mStage], 0, 0, 0);
    }
}
