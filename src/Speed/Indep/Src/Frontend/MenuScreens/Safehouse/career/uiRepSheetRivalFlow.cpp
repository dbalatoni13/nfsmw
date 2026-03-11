#include "uiRepSheetRivalFlow.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

void MemcardEnter();

extern unsigned int iCurrentViewBin;

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
    mStage++;
    switch (mStage) {
    case 0:
        cFEng::Get()->QueuePackageSwitch("IG_BL_CHALLENGE", 1, 0, false);
        break;
    case 1:
        cFEng::Get()->QueuePackageSwitch("IG_BL_MARKSELECT", 1, 0, false);
        break;
    case 2: {
        unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        if (bin == 8 || bin == 12) {
            cFEng::Get()->QueuePackageSwitch("IG_BL_REGIONUNLOCK", 1, 0, false);
        } else {
            Next();
        }
        break;
    }
    case 3:
        if (MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
            MemcardEnter();
        } else {
            Next();
        }
        break;
    case 4:
        cFEng::Get()->QueuePackageSwitch("IG_BL_BIO", 2, 0, false);
        break;
    case 5: {
        char buf[64];
        bSNPrintf(buf, 64, "FMV_BL%d", iCurrentViewBin);
        cFEng::Get()->QueuePackageSwitch("FE_MOVIE_PLAYER", 1, 0, false);
        break;
    }
    case 6: {
        unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        if (bin != 16) {
            iCurrentViewBin = bin;
            cFEng::Get()->QueuePackageSwitch("IG_BL_BIO", 3, 0, false);
        } else {
            FEDatabase->ClearGameMode(static_cast<eFEGameModes>(0x20000));
            mStage = -1;
        }
        break;
    }
    case 7:
        FEDatabase->ClearGameMode(static_cast<eFEGameModes>(0x20000));
        mStage = -1;
        break;
    default:
        mStage = -1;
        break;
    }
}
