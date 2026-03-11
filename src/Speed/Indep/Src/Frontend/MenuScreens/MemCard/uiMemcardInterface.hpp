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

    void SetMethod(int method) {
        mOp = (mOp & ~0xf0) | ((method & 0xf) << 4);
    }

    void ClearMethod() {
        mOp = mOp & ~0xf0;
    }
};

extern MemcardSetup gMemcardSetup;

void MemcardEnter(const char *from, const char *to, unsigned int op,
                  void (*termFunc)(void *), void *termParam,
                  unsigned int successMsg, unsigned int failedMsg);

#endif
