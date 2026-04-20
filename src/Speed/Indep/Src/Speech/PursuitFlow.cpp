#include "PursuitFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"

namespace Speech {

PursuitFlow::PursuitFlow()
    : mCauseofPursuit(kUnknown), //
      mFirstOnScene(0), //
      mReqRestart(false), //
      mAVSUnitRammedSaid(false), //
      mSpeaker(0), //
      mMsgNotifyEventCompletion(0) {}

PursuitFlow::~PursuitFlow() {}

void PursuitFlow::OnCopRemoved(EAXCop *cop) {
    if (mFirstOnScene && cop && (mFirstOnScene->GetSpeakerID() == cop->GetSpeakerID())) {
        mFirstOnScene = 0;
    }
}

void PursuitFlow::Update() {
    switch (mState) {
    case kTransition:
        return;
    case kCullCheck:
        CullCheck();
        return;
    case kCloseInCheck:
        CloseInCheck();
        return;
    case kPrimaryBranch:
        PrimaryBranch();
        return;
    case kSpotterBranch:
        SpotterBranch();
        return;
    case kScriptedBranch:
        ScriptedBranch();
        return;
    case kWaitForSpotter:
        SpotterWait();
        return;
    case kLostWhileSpotWait:
        LostWhileSpotterWait();
        return;
    case kPlayerStopped:
        PlayerStopped();
        return;
    case kBailout:
        Bailout();
        return;
    case kChangeTarget:
        ChangeTarget();
        return;
    case kTerminal:
        Terminal();
        return;
    default:
        ChangeStateTo(kCullCheck);
        return;
    }
}

void PursuitFlow::CullCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai && (ai->GetPursuitState() == SoundAI::kActive)) {
        ChangeStateTo(kCloseInCheck);
    }
}

void PursuitFlow::Reset() {
    mFirstOnScene = 0;
    ChangeStateTo(kTransition);
    mBusy = 0;
    mCauseofPursuit = kUnknown;
    mState = kCullCheck;
}

void PursuitFlow::Reacquire() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai && !ai->Is911Active()) {
        mCauseofPursuit = kReacquired;
        ChangeStateTo(kPrimaryBranch);
    }
}

bool PursuitFlow::RequiresRestart() {
    if (!mAVSUnitRammedSaid) {
        mBusy = 0;
        mReqRestart = true;
        return true;
    }
    return false;
}

void PursuitFlow::CloseInCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if ((ai->GetPursuitState() == SoundAI::kInactive) || ai->GetActors().empty()) {
        Reset();
        return;
    }

    if (!ai->GetPursuit()) {
        return;
    }

    if (ai->GetPursuitDuration() < ai->GetTune().PursuitInitDelay()) {
        return;
    }

    if (ai->GetRecentBlowby().distance < ai->GetTune().RangeForSpotterBranch()) {
        mCauseofPursuit = kSpotted;
    }

    if (ai->Is911Active()) {
        mCauseofPursuit = k911Reported;
    }

    if (ai->GetTimeLastNailedCop() < 10.0f) {
        mCauseofPursuit = kCopAssaulted;
    }

    if (mCauseofPursuit == kSpotted) {
        ChangeStateTo(kSpotterBranch);
    } else if ((mCauseofPursuit == k911Reported) || (mCauseofPursuit == kCopAssaulted) || (mCauseofPursuit == kCopAssaultedScripted) ||
               (mCauseofPursuit == kReacquired)) {
        ChangeStateTo(kPrimaryBranch);
    } else {
        ChangeStateTo(kScriptedBranch);
    }
}

void PursuitFlow::PrimaryBranch() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kPlayerStopped);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kChangeTarget);
        return;
    }

    if (mBusy) {
        if (!Manager::IsCopSpeechBusy()) {
            ChangeStateTo(kTerminal);
            mBusy = 0;
        }
        return;
    }

    EAXCop *leader = ai->FindClosestCop(false, false);
    if (!leader || !ai->MakeLeader(leader)) {
        ChangeStateTo(kCullCheck);
        return;
    }

    mFirstOnScene = leader;
    if (ai->GetPlayerStopTime() > ai->GetTune().MinTimeConsideredStopped()) {
        ChangeStateTo(kPlayerStopped);
    } else if (ai->NumPursuits() > 1) {
        leader->PrimaryEngage();
    }

    mBusy++;
}

void PursuitFlow::PlayerStopped() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kBailout);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kChangeTarget);
        return;
    }

    if (!mBusy) {
        if ((ai->GetTune().MinTimeConsideredStopped() < ai->GetPlayerStopTime()) && ai->GetLeader()) {
            ai->GetLeader()->AttemptVehicleStop();
            mBusy++;
            return;
        }
        ChangeStateTo(kTransition);
        mBusy = 0;
        return;
    }

    if ((ai->GetTune().MinTimeConsideredStopped() <= ai->GetPlayerStopTime()) && ai->GetLeader()) {
        ai->GetLeader()->AttemptVehicleStop();
        ChangeStateTo(kTerminal);
        mBusy = 0;
    }
}

void PursuitFlow::SpotterBranch() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    SoundAI::PursuitState pstate = ai->GetPursuitState();
    if ((pstate != SoundAI::kActive) && (pstate != SoundAI::kSearching)) {
        if (pstate == SoundAI::kInactive) {
            ChangeStateTo(kBailout);
        } else if (pstate == SoundAI::kOtherTarget) {
            ChangeStateTo(kChangeTarget);
        }
        return;
    }

    if (!mBusy) {
        EAXCop *anothercop = ai->FindClosestCop(false, ai->IsHighIntensity() || (ai->GetLastInfraction() == 0x40));
        if (!anothercop) {
            return;
        }

        mFirstOnScene = anothercop;
        if (ai->IsHighIntensity() || (ai->GetLastInfraction() == 0x40)) {
            anothercop->SpotterWanted();
        } else {
            anothercop->Spotter();
        }
        mBusy++;
        return;
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    if (ai->NumCopsWithLOS() < 1) {
        ChangeStateTo(kLostWhileSpotWait);
    } else {
        ChangeStateTo(kWaitForSpotter);
    }
    mBusy = 0;
}

void PursuitFlow::ScriptedBranch() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (!mBusy) {
        if (ai->GetLeader()) {
            ai->GetLeader()->SpotterReply();
        }
        mBusy++;
        return;
    }

    if (!Manager::IsCopSpeechBusy() && mBusy) {
        ChangeStateTo(kTerminal);
        mBusy = 0;
    }
}

void PursuitFlow::LostWhileSpotterWait() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (!mBusy) {
        if (ai->GetPursuitState() == SoundAI::kInactive) {
            ChangeStateTo(kBailout);
            return;
        }
        if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
            ChangeStateTo(kChangeTarget);
            return;
        }
        if (ai->NumCopsWithLOS() > 0) {
            ChangeStateTo(kWaitForSpotter);
        }
        return;
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    if (mBusy) {
        Reset();
        ChangeStateTo(kCullCheck);
        mBusy = 0;
    }
}

void PursuitFlow::SpotterWait() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (mBusy) {
        if (!Manager::IsCopSpeechBusy()) {
            ChangeStateTo(kTerminal);
            mBusy = 0;
        }
        return;
    }

    if (ai->GetPursuitState() != SoundAI::kActive) {
        ChangeStateTo(kLostWhileSpotWait);
        return;
    }

    EAXCop *cansee = ai->FindClosestCop(true, true);
    if (!cansee || !ai->MakeLeader(cansee)) {
        ChangeStateTo(kLostWhileSpotWait);
        return;
    }

    if (mCauseofPursuit == k911Reported) {
        cansee->SpotterReply();
    } else {
        cansee->Spotter();
    }

    mBusy++;
}

void PursuitFlow::Bailout() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    EAXCop *cop = ai->GetRandomActiveCop(0, false);
    if (cop) {
        cop->Bailout();
    }

    ChangeStateTo(kTransition);
    Reset();
}

void PursuitFlow::ChangeTarget() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    EAXCop *closest = ai->FindClosestCop(false, true);
    if (closest) {
        closest->FocusChange();
    }

    ChangeStateTo(kTransition);
    Reset();
}

void PursuitFlow::Terminal() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (!mBusy) {
        EAXCop *leader = ai->GetLeader();
        if ((ai->GetTune().MinTimeConsideredStopped() <= ai->GetPlayerSpeed()) && leader) {
            leader->PrimaryEngage();
        }
        if (!ai->IsHighIntensity() && leader) {
            leader->VehicleReport();
        }
        if (ai->GetTune().MinHavocForSuspectBehavior() <= ai->GetHavoc()) {
            EAXCop *cansee = ai->FindClosestCop(true, true);
            if (cansee) {
                cansee->SuspectBehavior();
            }
        }
        mBusy++;
        return;
    }

    if (!Manager::IsCopSpeechBusy() && mBusy) {
        ChangeStateTo(kTransition);
        mFirstOnScene = 0;
        mBusy = 0;
    }
}

bool PursuitFlow::IsTransitionable() {
    return mState == kTransition;
}

void PursuitFlow::MessageEventComplete(const MNotifySpeechStatus &message) {
    if (!message.GetEvent()) {
        return;
    }
    if (mReqRestart) {
        mReqRestart = false;
    }
}

} // namespace Speech
