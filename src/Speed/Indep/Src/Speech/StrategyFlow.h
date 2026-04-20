#ifndef SPEECH_STRATEGYFLOW_H
#define SPEECH_STRATEGYFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct MNotifySpeechStatus;
struct MReqBackup;

namespace Speech {

class StrategyFlow : public SpeechFlow {
  public:
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

    StrategyFlow();
    ~StrategyFlow() override;
    void Reset() override;
    void Update() override;
    bool IsTransitionable() override;

    void CullCheck();
    void SoloCheck();
    void CallToPos();
    void ReqBackup();
    void Waiting();
    void Outrun();
    void Lost();
    void Terminal();
    void Outcome();
    void MessageReqBackup(const MReqBackup &message);
    void MessageBackupDenied(const MReqBackup &message);
    void MessageEventComplete(const MNotifySpeechStatus &message);

  private:
    unsigned int mFlags;                    // offset 0x10, size 0x4
    float mDistance[2];                     // offset 0x14, size 0x8
    float mSpeed[2];                        // offset 0x1C, size 0x8
    int mLOSCount;                          // offset 0x24, size 0x4
    int mFormationCount;                    // offset 0x28, size 0x4
    int mFormationType;                     // offset 0x2C, size 0x4
    int mBackupType;                        // offset 0x30, size 0x4
    Timer mT_requested;                     // offset 0x34, size 0x4
    int mLastBackupType;                    // offset 0x38, size 0x4
    Hermes::HHANDLER mMsgReqBackup;         // offset 0x3C, size 0x4
    Hermes::HHANDLER mMsgBackupDenied;      // offset 0x40, size 0x4
    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x44, size 0x4
};

} // namespace Speech

#endif
