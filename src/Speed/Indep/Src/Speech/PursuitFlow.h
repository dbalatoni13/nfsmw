#ifndef SPEECH_PURSUITFLOW_H
#define SPEECH_PURSUITFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct MNotifySpeechStatus;

namespace Speech {

class PursuitFlow : public SpeechFlow {
  public:
    enum PursuitCause {
        k911Reported = 0,
        kCopAssaulted = 1,
        kSpotted = 2,
        kReacquired = 3,
        kScripted = 4,
        kCopAssaultedScripted = 5,
        kUnknown = 6,
        kMAX_CAUSE_OF_PURSUIT = 7,
    };

    enum State {
        kTransition = -1,
        kCullCheck = 0,
        kCloseInCheck = 1,
        kPrimaryBranch = 2,
        kSpotterBranch = 3,
        kScriptedBranch = 4,
        kWaitForSpotter = 5,
        kLostWhileSpotWait = 6,
        kPlayerStopped = 7,
        kBailout = 8,
        kChangeTarget = 9,
        kTerminal = 999,
    };

    PursuitFlow();
    ~PursuitFlow() override;

    void Update() override;
    void OnCopRemoved(EAXCop *cop) override;
    void Reset() override;
    bool IsTransitionable() override;

    void CullCheck();
    void Reacquire();
    bool RequiresRestart();
    void CloseInCheck();
    void PrimaryBranch();
    void PlayerStopped();
    void SpotterBranch();
    void ScriptedBranch();
    void LostWhileSpotterWait();
    void SpotterWait();
    void Bailout();
    void ChangeTarget();
    void Terminal();
    void MessageEventComplete(const MNotifySpeechStatus &message);

    PursuitCause GetPursuitCause() const {
        return mCauseofPursuit;
    }

    void SetPursuitCause(PursuitCause cause) {
        mCauseofPursuit = cause;
    }

  private:
    PursuitCause mCauseofPursuit;       // offset 0x10, size 0x4
    EAXCop *mFirstOnScene;              // offset 0x14, size 0x4
    bool mReqRestart;                   // offset 0x18, size 0x1
    bool mAVSUnitRammedSaid;            // offset 0x1C, size 0x1
    short mSpeaker;                     // offset 0x20, size 0x2
    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x24, size 0x4
};

} // namespace Speech


#endif
