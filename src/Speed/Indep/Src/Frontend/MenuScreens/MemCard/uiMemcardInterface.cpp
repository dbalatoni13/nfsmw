#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

MemoryCardSetup gMemcardSetup;

void MemoryCardSetup::Clear() {
    mOp = 0;
    mMemScreen = nullptr;
    mToScreen = nullptr;
    mFromScreen = nullptr;
    mTermFunc = nullptr;
    mTermFuncParam = nullptr;
    mLastMessage = 0;
    mSuccessMsg = 0;
    mFailedMsg = 0;
    mInBootFlow = false;
    mPreviousCommand = 0;
    mPreviousPrompt = 0;
}

unsigned int MemcardGetCurrentUIOperation() {
    return gMemcardSetup.mOp & 0xf0;
}

// ===== MemcardEnter / MemcardExit =====

void MemcardEnter(const char *from, const char *to, uint32 op, MemCardOpType termFunc, void *termParam, uint32 successMsg, uint32 failedMsg) {
    gMemcardSetup.mOp = op;
    gMemcardSetup.mFromScreen = from;
    gMemcardSetup.mToScreen = to;
    gMemcardSetup.mTermFunc = termFunc;
    gMemcardSetup.mTermFuncParam = termParam;
    gMemcardSetup.mSuccessMsg = successMsg;
    gMemcardSetup.mFailedMsg = failedMsg;
    gMemcardSetup.mMemScreen = nullptr;
    MemoryCard::GetInstance()->ShowMessages(true);
    MemoryCard::GetInstance()->SetPlayerNum((op >> 17) & 1);
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        gMemcardSetup.mMemScreen = "MC_Main_GC.fng";
    } else {
        gMemcardSetup.mMemScreen = "InGame_MC_Main_GC.fng";
    }
    int cmd = gMemcardSetup.mOp & 0xf;
    switch (cmd) {
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

void MemcardExit(unsigned int msg) {
    gMemcardSetup.mLastMessage = msg;
    if (!MemoryCard::GetInstance()->m_bInitialized) {
        cFEng *feng = cFEng::Get();
        unsigned long hash = FEHashUpper("EXIT_COMPLETE");
        feng->QueueGameMessage(hash, gMemcardSetup.mMemScreen, 0xff);
    } else {
        cFEng *feng = cFEng::Get();
        unsigned long hash = FEHashUpper("LEAVE_SCREEN");
        feng->QueuePackageMessage(hash, gMemcardSetup.mMemScreen, nullptr);
    }
    MemoryCard::GetInstance()->m_bInitialized = false;
    MemoryCard::GetInstance()->SetMemcardScreenExiting(true);
}
