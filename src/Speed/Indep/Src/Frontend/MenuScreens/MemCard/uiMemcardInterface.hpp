#ifndef _UIMEMCARDINTERFACE
#define _UIMEMCARDINTERFACE

#include <types.h>

struct MemcardSetup {
    int mOp;
    const char* mFromScreen;
    const char* mToScreen;
    const char* mMemScreen;
    void* mTermFunc;
    int mTermFuncParam;
    int mLastMessage;
    int mPreviousCommand;
    int mPreviousPrompt;
    unsigned int mSuccessMsg;
    unsigned int mFailedMsg;
    int mLastController;
    bool mInBootFlow;

    int GetMethod() const {
        return (mOp >> 4) & 0xf;
    }

    int GetCommand() const {
        return mOp & 0xf;
    }
};

extern MemcardSetup gMemcardSetup;

#endif
