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
