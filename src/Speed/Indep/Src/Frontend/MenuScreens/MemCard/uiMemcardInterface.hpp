#ifndef __UIMEMCARDINTERFACE_HPP__
#define __UIMEMCARDINTERFACE_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

typedef void MemCardOpType(void *);

typedef enum {
    MCF_Modal = 1,
    MCF_Switch = 2,
    MCF_ModalSwitch = 3,
    MCF_None = 4,
    MCO_LoadList = 16,
    MCO_BootList = 32,
    MCO_DeleteList = 48,
    MCO_SaveCreate = 64,
    MCO_Save = 80,
    MCO_CreateNew = 96,
    MCO_CarLotLoad = 112,
    MCO_CheckCard = 128,
    MCO_AutoSaveError = 144,
    MCO_EnableAutoSave = 160,
    MCO_AutoSave = 176,
    MCO_Unused3 = 192,
    MCO_AutoSaveOverwrite = 208,
    MCO_ShowTRCMessage = 224,
    MCE_SendTermMessage = 256,
    MCE_PromptForSave = 512,
    MCE_SendCustomizedMessage = 1024,
    MCE_ProfileOnly = 2048,
    MCE_PostOnDestroy = 4096,
    MCE_SkipAutoSave = 8192,
    MCE_SendInitComplete = 16384,
    MCE_Online = 32768,
    MCE_ChallengeSeries = 65536,
    MCE_Player2 = 131072,
    MCE_PromptSaveOptions = 262144,
    MCE_NewCareer = 524288,
    MCE_PromptSaveStable = 2097152,
    MCE_RivalFlow = 4194304,
    MCE_Unused2 = 8388608,
    MCP_Create = 16777216,
    MCP_AutoSaveOverwrite = 33554432,
    MCP_DismissMe = 50331648,
    MCP_ConfirmSave = 67108864,
    MCP_ConfirmDestoryCreate = 83886080,
    MCP_ConfirmDestoryLoad = 100663296,
    MCP_OK = 117440512,
    MCP_AutoSaveWarning = 134217728,
    MCP_AutoSaveWarning2 = 150994944,
    MCP_ConfirmAutoSave = 167772160,
    MCP_ConfirmAutoSaveEnableFailed = 184549376,
    MCP_EnableAutoSave = 201326592,
    MCP_CorruptProfile = 218103808,
    MCP_CardRemoved = 234881024
} MemcardFlowOperator;

typedef enum { MCFF_OK = 0, MCFF_DeleteOnly = 1, MCFF_Corrupted = 2 } MemCardFileFlag;

typedef enum {
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
    MCSF_CheckCard = 13
} MCSaveFlow;

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

    uint32 GetCommand() { return mOp & 0xf0; }
    uint32 GetMethod() { return mOp & 0xf; }
    uint32 GetExtraOptions() { return mOp & 0xffff00; }
    uint32 GetPrompt() { return mOp & 0xf000000; }
    void SetExtraOption(int eo) { mOp = (mOp & ~0xffff00) | (eo & 0xffff00); }
    void SetPrompt(int prompt) { mOp |= prompt & 0xf000000; }
    void SetMethod(int method) { mOp = (mOp & ~0xf) | (method & 0xf); }
    void SetCommand(int command) { mOp |= command & 0xf0; }
    void ClearPrompt() {

        mPreviousPrompt = GetPrompt();
        mOp &= ~0xf000000;
    }
    void ClearMethod() { mOp &= ~0xf; }
    void ClearCommand() {

        mPreviousCommand = GetCommand();
        mOp &= ~0xf0;
    }

    bool IsSaving() {
        uint32 cmd = GetCommand();
        return cmd == 3 || cmd == 4;
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
        unsigned int m = mLastMessage;

        if ((mOp & MCE_SendTermMessage) != 0) {

            cFEng::Get()->QueuePackageMessage(m, mToScreen, nullptr);
        }

        if ((mOp & MCE_SendCustomizedMessage) != 0) {

            unsigned int hash = m == 0x461A18EE ? mSuccessMsg : mFailedMsg;

            cFEng::Get()->QueueGameMessage(hash, mToScreen, 0xff);
        }
    }

    // `Complete` salva mLastMessage, limpia la estructura entera y lo repone:
    // el ultimo store a +0x18 lleva el valor ANTIGUO, no el argumento.
    void Complete(uint32 msg) {
        unsigned int m = mLastMessage;

        if (mTermFunc != nullptr) {
            mTermFunc(mTermFuncParam);
        }

        Clear();
        mLastMessage = m;
    }
};

typedef MemoryCardSetup MemcardSetup;

extern MemoryCardSetup gMemcardSetup;

void MemcardEnter(const char *from, const char *to, uint32 op, MemCardOpType *pTermFunc, void *termParam, uint32 successMsg, uint32 failedMsg);
void MemcardExit(uint32 msg);
uint32 MemcardGetCurrentUIOperation();

#endif
