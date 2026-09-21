#ifndef __MUSFLOW_H
#define __MUSFLOW_H 1 // Decl: 15

#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

// total size: 0x68
// Decl: 30
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

    static const float kChaseDist; // size: 0x4, address: 0xFFFFFFFF, Decl: 97 // TODO

    MusicFlow(); // Decl: 100

    // Overrides: SpeechFlow
    ~MusicFlow() override;
    void Update() override;
    bool IsTransitionable() override;
    void Reset() override;
    void ChangeStateTo(int new_state) override;

    void Reacquire();

  private:
    MusicFlow(const MusicFlow &);
    const MusicFlow &operator=(const MusicFlow &);

    float UpdateIntensity(float adj);

    void Neutral();

    void Lose();

    void Win();

    void Elude();

    void Terminal();

    void Waiting();

    void MessageNewPart(const MNotifyMusicFlow &message);

    void MessageInitFlow(const MNotifyMusicFlow &message);

    void MessageTerminate(const MNotifyMusicFlow &message);

    void MessageDone(const MNotifyMusicFlow &message);

    void MessageX360UserTunes(const MNotifyMusicFlow &message);

    void RequestSwap();

  private:
    bool mStartDelay;   // offset 0x10, size 0x1
    int mStartEvent;    // offset 0x14, size 0x4, Decl: 126
    Timer mTimer;       // offset 0x18, size 0x4, Decl: 127
    Timer mBoostTimer;  // offset 0x1C, size 0x4, Decl: 128
    Timer mT_currPiece; // offset 0x20, size 0x4, Decl: 129

    float mElapsed;          // offset 0x24, size 0x4, Decl: 131
    float mIntensity;        // offset 0x28, size 0x4, Decl: 132
    float mAvgNumCopsInForm; // offset 0x2C, size 0x4, Decl: 133
    float mAvgNumCopsLOS;    // offset 0x30, size 0x4, Decl: 134
    float mAvgPlayerSpeed;   // offset 0x34, size 0x4, Decl: 135
    float mAvgPursuitDist;   // offset 0x38, size 0x4, Decl: 136
    int mCurrentPart;        // offset 0x3C, size 0x4, Decl: 137
    bool mRestrained;        // offset 0x40, size 0x1
    float mTopSpeed;         // offset 0x44, size 0x4, Decl: 138
    float mTimeInPiece;      // offset 0x48, size 0x4, Decl: 139
    bool mRequestedSwap;     // offset 0x4C, size 0x1
    bool mX360UserTunes;     // offset 0x50, size 0x1

    Hermes::HHANDLER mMsgNewPart; // offset 0x54, size 0x4, Decl: 144

    Hermes::HHANDLER mMsgInitFlow; // offset 0x58, size 0x4, Decl: 147

    Hermes::HHANDLER mMsgTerminate; // offset 0x5C, size 0x4, Decl: 150

    Hermes::HHANDLER mMsgDone; // offset 0x60, size 0x4, Decl: 153

    Hermes::HHANDLER mMsgX360UserTunes; // offset 0x64, size 0x4, Decl: 156
};

}; // namespace Speech

#endif
