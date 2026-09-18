#ifndef SPEECH_MUSICFLOW_H
#define SPEECH_MUSICFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct MNotifyMusicFlow;

namespace Speech {

class MusicFlow : public SpeechFlow {
  public:
    enum State {
        kTransition = -1,
        kNeutral = 0,
        kLose = 1,
        kWin = 2,
        kElude = 3,
        kWaiting = 333,
        kTerminal = 999,
    };

    enum Intensity {
        kLow = 0,
        kMedium = 35,
        kHigh = 127,
    };

    MusicFlow();
    ~MusicFlow() override;
    void MessageNewPart(const MNotifyMusicFlow &message);
    void MessageInitFlow(const MNotifyMusicFlow &message);
    void MessageTerminate(const MNotifyMusicFlow &message);
    void MessageDone(const MNotifyMusicFlow &message);
    void MessageX360UserTunes(const MNotifyMusicFlow &message);
    void Reacquire();
    void Update() override;
    float UpdateIntensity(float adj);
    void Waiting();
    void Neutral();
    void Lose();
    void RequestSwap();
    void Win();
    void Elude();
    void Terminal();
    bool IsTransitionable() override;
    void ChangeStateTo(int new_state) override;
    void Reset() override;

  private:
    bool mStartDelay;                     // offset 0x10, size 0x1
    int mStartEvent;                      // offset 0x14, size 0x4
    Timer mTimer;                         // offset 0x18, size 0x4
    Timer mBoostTimer;                    // offset 0x1C, size 0x4
    Timer mT_currPiece;                   // offset 0x20, size 0x4
    float mElapsed;                       // offset 0x24, size 0x4
    float mIntensity;                     // offset 0x28, size 0x4
    float mAvgNumCopsInForm;              // offset 0x2C, size 0x4
    float mAvgNumCopsLOS;                 // offset 0x30, size 0x4
    float mAvgPlayerSpeed;                // offset 0x34, size 0x4
    float mAvgPursuitDist;                // offset 0x38, size 0x4
    int mCurrentPart;                     // offset 0x3C, size 0x4
    bool mRestrained;                     // offset 0x40, size 0x1
    float mTopSpeed;                      // offset 0x44, size 0x4
    float mTimeInPiece;                   // offset 0x48, size 0x4
    bool mRequestedSwap;                  // offset 0x4C, size 0x1
    bool mX360UserTunes;                  // offset 0x50, size 0x1
    Hermes::HHANDLER mMsgNewPart;         // offset 0x54, size 0x4
    Hermes::HHANDLER mMsgInitFlow;        // offset 0x58, size 0x4
    Hermes::HHANDLER mMsgTerminate;       // offset 0x5C, size 0x4
    Hermes::HHANDLER mMsgDone;            // offset 0x60, size 0x4
    Hermes::HHANDLER mMsgX360UserTunes;   // offset 0x64, size 0x4
};

} // namespace Speech

#endif
