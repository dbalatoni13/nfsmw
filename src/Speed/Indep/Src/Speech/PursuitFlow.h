#ifndef __PURSUITFLOW_H
#define __PURSUITFLOW_H 1 // Decl: 15

#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

// total size: 0x28
// Decl: 26
struct PursuitFlow : public SpeechFlow {
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

    PursuitFlow(); // Decl: 69
    ~PursuitFlow() override;

    // Overrides: SpeechFlow
    void Update() override;
    void OnCopRemoved(EAXCop *cop) override;
    void Reset() override;
    bool IsTransitionable() override;

    void Reacquire();

    bool RequiresRestart();

    // Decl: 65
    PursuitCause GetPursuitCause() {
        return this->mCauseofPursuit;
    }
    // Decl: 66
    void SetPursuitCause(PursuitCause cause) {
        this->mCauseofPursuit = cause;
    }

  private:
    PursuitFlow(const PursuitFlow &);
    const PursuitFlow &operator=(const PursuitFlow &);

    void CullCheck();

    void CloseInCheck();

    void PrimaryBranch();

    void SpotterBranch();

    void ScriptedBranch();

    void PlayerStopped();

    void SpotterWait();

    void LostWhileSpotterWait();

    void Bailout();

    void ChangeTarget();

    void Terminal();

    void MessageEventComplete(const MNotifySpeechStatus &message);

    PursuitCause mCauseofPursuit; // offset 0x10, size 0x4, Decl: 85
    EAXCop *mFirstOnScene;        // offset 0x14, size 0x4, Decl: 86
    bool mReqRestart;             // offset 0x18, size 0x1, Decl: 87
    bool mAVSUnitRammedSaid;      // offset 0x1C, size 0x1, Decl: 88
    short mSpeaker;               // offset 0x20, size 0x2, Decl: 89

    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x24, size 0x4, Decl: 91
};

}; // namespace Speech

#endif
