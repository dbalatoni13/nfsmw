#ifndef _UIMEMCARDINTERFACE
#define _UIMEMCARDINTERFACE

#include <types.h>

struct MemoryCardSetup {
    unsigned int mOp;
    const char* mFromScreen;
    const char* mToScreen;
    const char* mMemScreen;
    void (*mTermFunc)(void*);
    void* mTermFuncParam;
    unsigned int mLastMessage;
    unsigned int mPreviousCommand;
    unsigned int mPreviousPrompt;
    unsigned int mSuccessMsg;
    unsigned int mFailedMsg;
    unsigned int mLastController;
    bool mInBootFlow;

    MemoryCardSetup() { Clear(); }

    unsigned int GetCommand() {
        return mOp & 0xf0;
    }

    unsigned int GetCommand() const {
        return mOp & 0xf;
    }

    unsigned int GetMethod() const {
        return mOp & 0xf0;
    }

    unsigned int GetExtraOptions() const {
        return (mOp >> 8) & 0xf;
    }

    unsigned int GetPrompt() const {
        return (mOp >> 12) & 0xf;
    }

    void SetCommand(int command) {
        mOp = (mOp & ~0xf0) | (command & 0xf0);
    }

    void SetMethod(int method) {
        mOp = (mOp & ~0xf0) | (method & 0xf0);
    }

    void SetExtraOption(int eo) {
        mOp = (mOp & ~0xf00) | ((eo & 0xf) << 8);
    }

    void SetPrompt(int prompt) {
        mOp = (mOp & ~0xf000) | ((prompt & 0xf) << 12);
    }

    void ClearCommand() {
        mOp = mOp & ~0xf0;
    }

    void ClearMethod() {
        mOp = mOp & ~0xf0;
    }

    void ClearPrompt() {
        unsigned int p = GetPrompt();
        if (p != 0) {
            mPreviousPrompt = p;
            mOp = mOp & ~0xf000;
        }
    }

    bool IsSaving() const {
        unsigned int cmd = GetCommand();
        return cmd == 3 || cmd == 4;
    }

    void Clear();

    void SendTermMessage(unsigned int msg) {
        if (mTermFunc != nullptr) {
            mTermFunc(mTermFuncParam);
        }
    }

    void Complete(unsigned int msg) {
        mLastMessage = msg;
        SendTermMessage(msg);
        Clear();
    }
};

typedef MemoryCardSetup MemcardSetup;

extern MemoryCardSetup gMemcardSetup;

void MemcardEnter(const char *from, const char *to, unsigned int op,
                  void (*termFunc)(void *), void *termParam,
                  unsigned int successMsg, unsigned int failedMsg);
void MemcardExit(unsigned int msg);

#endif
