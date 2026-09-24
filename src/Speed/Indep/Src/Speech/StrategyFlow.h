#ifndef __STRATEGYFLOW_H
#define __STRATEGYFLOW_H 1 // Decl: 15

#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

// total size: 0x48
// Decl: 28
struct StrategyFlow : public SpeechFlow {
    enum State {
        kTransition = -1,
        kCullCheck = 0,
        kSoloCheck = 1,
        kOutrun = 2,
        kLost = 3,
        kReqBackup = 4,
        kCallToPos = 5,
        kWaiting = 333,
        kTerminal = 999,
        kOutcome = 1000,
    };
    enum StrategyFlowFlags {
        SOLO = 1,
        BUDENIED = 2,
        REQUESTABLE = 4,
    };

    StrategyFlow(); // Decl: 61
    ~StrategyFlow() override;

    // Overrides: SpeechFlow
    void Update() override;
    bool IsTransitionable() override;
    void Reset() override;

  private:
    virtual void CullCheck();

    virtual void SoloCheck();

    virtual void Waiting();

    virtual void Terminal();

    virtual void Outcome();

    virtual void Outrun();

    virtual void Lost();

    virtual void ReqBackup();

    virtual void CallToPos();

    void MessageReqBackup(const MReqBackup &message);

    void MessageBackupDenied(const MReqBackup &message);

    void MessageEventComplete(const MNotifySpeechStatus &message);

    unsigned int mFlags; // offset 0x10, size 0x4, Decl: 75
    float mDistance[2];  // offset 0x14, size 0x8, Decl: 76
    float mSpeed[2];     // offset 0x1C, size 0x8, Decl: 77
    int mLOSCount;       // offset 0x24, size 0x4, Decl: 78
    int mFormationCount; // offset 0x28, size 0x4, Decl: 79
    int mFormationType;  // offset 0x2C, size 0x4, Decl: 80
    int mBackupType;     // offset 0x30, size 0x4, Decl: 81
    Timer mT_requested;  // offset 0x34, size 0x4, Decl: 82
    int mLastBackupType; // offset 0x38, size 0x4, Decl: 83

    Hermes::HHANDLER mMsgReqBackup; // offset 0x3C, size 0x4, Decl: 85

    Hermes::HHANDLER mMsgBackupDenied; // offset 0x40, size 0x4, Decl: 88

    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x44, size 0x4, Decl: 91
};

}; // namespace Speech

#endif
