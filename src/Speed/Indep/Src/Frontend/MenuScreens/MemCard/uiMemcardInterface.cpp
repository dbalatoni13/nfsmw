#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

MemoryCardSetup gMemcardSetup;

// UNSOLVED
void MemcardEnter(const char *from, const char *to, uint32 op, MemCardOpType pTermFunc, void *pTermFuncParam, uint32 successMsg, uint32 failedMsg) {
    gMemcardSetup.mMemScreen = nullptr;
    gMemcardSetup.mOp = op;
    gMemcardSetup.mFromScreen = from;
    gMemcardSetup.mToScreen = to;
    gMemcardSetup.mTermFunc = pTermFunc;
    gMemcardSetup.mTermFuncParam = pTermFuncParam;
    gMemcardSetup.mSuccessMsg = successMsg;
    gMemcardSetup.mFailedMsg = failedMsg;
    MemoryCard::GetInstance()->ShowMessages(true);
    MemoryCard::GetInstance()->SetPlayerNum((op >> 17) & 1);
    if (TheGameFlowManager.IsInFrontend()) {
        gMemcardSetup.mMemScreen = "MC_Main_GC.fng";
    } else {
        gMemcardSetup.mMemScreen = "InGame_MC_Main_GC.fng";
    }
    switch (gMemcardSetup.GetMethod()) {
        case 2:
            cFEng::Get()->QueuePackageSwitch(gMemcardSetup.mMemScreen, 0, 0, false);
            break;
        case 1:
        case 3:
            cFEng::Get()->QueuePackagePush(gMemcardSetup.mMemScreen, 0, 0, false);
            break;
    }
    MemoryCard::GetInstance()->SetMemcardScreenShowing(true);
}

void MemcardExit(uint32 msg) {
    gMemcardSetup.mLastMessage = msg;
    if (!MemoryCard::GetInstance()->IsMemcardScreenInitialized()) {
        cFEng::Get()->QueueGameMessage(FEHashUpper("EXIT_COMPLETE"), gMemcardSetup.mMemScreen, 0xff);
    } else {
        cFEng::Get()->QueuePackageMessage(FEHashUpper("LEAVE_SCREEN"), gMemcardSetup.mMemScreen, nullptr);
    }
    MemoryCard::GetInstance()->SetMemcardScreenInitialized(false);
    MemoryCard::GetInstance()->SetMemcardScreenExiting(true);
}

uint32 MemcardGetCurrentUIOperation() {
    return gMemcardSetup.mOp & 0xf0;
}
