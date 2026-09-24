#ifndef __UIMEMCARDINTERFACE_HPP__
#define __UIMEMCARDINTERFACE_HPP__

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include <types.h>

typedef void MemCardOpType(void *);

enum MemcardFlowOperator {
    MCF_Modal = 0x1,
    MCF_Switch = 0x2,
    MCF_ModalSwitch = 0x3,
    MCF_None = 0x4,
    MCO_LoadList = 0x10,
    MCO_BootList = 0x20,
    MCO_DeleteList = 0x30,
    MCO_SaveCreate = 0x40,
    MCO_Save = 0x50,
    MCO_CreateNew = 0x60,
    MCO_CarLotLoad = 0x70,
    MCO_CheckCard = 0x80,
    MCO_AutoSaveError = 0x90,
    MCO_EnableAutoSave = 0xA0,
    MCO_AutoSave = 0xB0,
    MCO_Unused3 = 0xC0,
    MCO_AutoSaveOverwrite = 0xD0,
    MCO_ShowTRCMessage = 0xE0,
    MCO_AutoLoad = 0xF0,
    MCE_SendTermMessage = 0x100,
    MCE_PromptForSave = 0x200,
    MCE_SendCustomizedMessage = 0x400,
    MCE_ProfileOnly = 0x800,
    MCE_PostOnDestroy = 0x1000,
    MCE_SkipAutoSave = 0x2000,
    MCE_SendInitComplete = 0x4000,
    MCE_Online = 0x8000,
    MCE_ChallengeSeries = 0x10000,
    MCE_Player2 = 0x20000,
    MCE_PromptSaveOptions = 0x40000,
    MCE_NewCareer = 0x80000,
    MCE_PromptSaveStable = 0x200000,
    MCE_RivalFlow = 0x400000,
    MCE_Unused2 = 0x800000,
    MCP_Create = 0x1000000,
    MCP_AutoSaveOverwrite = 0x2000000,
    MCP_DismissMe = 0x3000000,
    MCP_ConfirmSave = 0x4000000,
    MCP_ConfirmDestoryCreate = 0x5000000,
    MCP_ConfirmDestoryLoad = 0x6000000,
    MCP_OK = 0x7000000,
    MCP_AutoSaveWarning = 0x8000000,
    MCP_AutoSaveWarning2 = 0x9000000,
    MCP_ConfirmAutoSave = 0xA000000,
    MCP_ConfirmAutoSaveEnableFailed = 0xB000000,
    MCP_EnableAutoSave = 0xC000000,
    MCP_CorruptProfile = 0xD000000,
    MCP_CardRemoved = 0xE000000,
    MCP_ConfirmSignIn = 0xF000000,
};

enum MemCardFileFlag {
    MCFF_OK = 0,
    MCFF_DeleteOnly = 1,
    MCFF_Corrupted = 2,
};

enum MCSaveFlow {
    MCSF_PromptForDestoryCreate = 1,
    MCSF_PromptForCreate = 2,
    MCSF_PromptForKeyboard = 3,
    MCSF_PromptForSave = 4,
    MCSF_PromptForAutoSave = 5,
    MCSF_PromptForConfirm = 6,
    MCSF_DoAutoSave = 7,
    MCSF_DoSave = 8,
    MCSF_PromptForExceedingLimit = 9,
    MCSF_AutoSaveWarning = 10,
    MCSF_AutoSaveWarning2 = 11,
    MCSF_DisableAutoSave = 12,
    MCSF_CheckCard = 13,
};

struct MemoryCardSetup {
    uint32 mOp;
    const char *mFromScreen;
    const char *mToScreen;
    const char *mMemScreen;
    MemCardOpType *mTermFunc;
    void *mTermFuncParam;
    uint32 mLastMessage;
    uint32 mPreviousCommand;
    uint32 mPreviousPrompt;
    uint32 mSuccessMsg;
    uint32 mFailedMsg;
    uint32 mLastController;
    bool mInBootFlow;

    MemoryCardSetup() {
        Clear();
    }

    uint32 GetCommand() {
        return mOp & 0xf0;
    }

    uint32 GetMethod() {
        return mOp & 0xf;
    }

    uint32 GetExtraOptions() {
        return (mOp & 0xffff00);
    }

    uint32 GetPrompt() {
        return mOp & 0xf000000;
    }

    void SetCommand(int command) {
        mOp |= (command & 0xf0);
    }

    void SetMethod(int method) {
        mOp |= (method & 0xf);
    }

    void SetExtraOption(int eo) {
        mOp |= (eo & 0xf000);
    }

    void SetPrompt(int prompt) {
        mOp |= (prompt & 0xf000000);
    }

    void ClearCommand() {
        mPreviousCommand = GetCommand();
        mOp = mOp & ~0xf0;
    }

    void ClearMethod() {
        mOp = mOp & ~0xf;
    }

    void ClearPrompt() {
        mPreviousPrompt = GetPrompt();
        mOp = mOp & ~0xf000000;
    }

    bool IsSaving() {
        uint32 cmd = GetCommand();
        return cmd == MCF_ModalSwitch || cmd == MCF_None;
    }

    void Clear() {
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
    };

    void SendTermMessage(uint32 msg) {
        if (mOp & MCE_SendTermMessage) {
            cFEng::Get()->QueuePackageMessage(msg, mToScreen, nullptr);
        }
        if (mOp & MCE_SendCustomizedMessage) {
            uint32 m = msg == 0x461a18ee ? mSuccessMsg : mFailedMsg;
            cFEng::Get()->QueueGameMessage(m, mToScreen, 0xff);
        }
    }

    void Complete(uint32 msg) {
        if (mTermFunc != nullptr) {
            mTermFunc(mTermFuncParam);
        }
        Clear();
        mLastMessage = msg;
    }
};

typedef MemoryCardSetup MemcardSetup;

extern MemoryCardSetup gMemcardSetup;

void MemcardEnter(const char *from, const char *to, uint32 op, MemCardOpType *pTermFunc, void *pTermFuncParam, uint32 successMsg, uint32 failedMsg);
void MemcardExit(uint32 msg);
uint32 MemcardGetCurrentUIOperation();

#endif
