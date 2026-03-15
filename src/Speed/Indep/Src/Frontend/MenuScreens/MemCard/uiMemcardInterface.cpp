#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

void MemoryCardSetup::Clear() {
    mPreviousPrompt = 0;
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
}

unsigned int MemcardGetCurrentUIOperation() {
    return gMemcardSetup.mOp & 0xf0;
}
