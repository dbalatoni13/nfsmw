#include "MusicFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"

namespace Speech {

MusicFlow::MusicFlow()
    : mStartDelay(false), //
      mStartEvent(0), //
      mTimer(0), //
      mBoostTimer(0), //
      mT_currPiece(0), //
      mElapsed(0.0f), //
      mIntensity(0.0f), //
      mAvgNumCopsInForm(0.0f), //
      mAvgNumCopsLOS(0.0f), //
      mAvgPlayerSpeed(0.0f), //
      mAvgPursuitDist(0.0f), //
      mCurrentPart(-1), //
      mRestrained(true), //
      mTopSpeed(0.0f), //
      mTimeInPiece(0.0f), //
      mRequestedSwap(false), //
      mX360UserTunes(false), //
      mMsgNewPart(0), //
      mMsgInitFlow(0), //
      mMsgTerminate(0), //
      mMsgDone(0), //
      mMsgX360UserTunes(0) {
    mMsgNewPart = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageNewPart, "PartUpdate", 0);
    mMsgInitFlow = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageInitFlow, "Init", 0);
    mMsgTerminate = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageTerminate, "Terminate", 0);
    mMsgDone = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageDone, "InteractiveDone", 0);
    mMsgX360UserTunes = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageX360UserTunes, "X360UserTunes", 0);
    mState = kTransition;
    mBusy = 0;
}

MusicFlow::~MusicFlow() {
    if (mMsgNewPart) {
        Hermes::Handler::Destroy(mMsgNewPart);
        mMsgNewPart = 0;
    }
    if (mMsgInitFlow) {
        Hermes::Handler::Destroy(mMsgInitFlow);
        mMsgInitFlow = 0;
    }
    if (mMsgTerminate) {
        Hermes::Handler::Destroy(mMsgTerminate);
        mMsgTerminate = 0;
    }
    if (mMsgDone) {
        Hermes::Handler::Destroy(mMsgDone);
        mMsgDone = 0;
    }
    if (mMsgX360UserTunes) {
        Hermes::Handler::Destroy(mMsgX360UserTunes);
        mMsgX360UserTunes = 0;
    }
}

void MusicFlow::MessageNewPart(const MNotifyMusicFlow &message) {
    mCurrentPart = message.GetPart();
    mStartDelay = false;
    mT_currPiece = WorldTimer;
}

void MusicFlow::MessageInitFlow(const MNotifyMusicFlow &message) {
    mStartEvent = message.GetPart();
    mStartDelay = true;
    mTimer = WorldTimer;
}

void MusicFlow::MessageTerminate(const MNotifyMusicFlow &) {
    ChangeStateTo(kTerminal);
}

void MusicFlow::MessageDone(const MNotifyMusicFlow &) {
    mRequestedSwap = false;
    mBusy = 0;
}

void MusicFlow::MessageX360UserTunes(const MNotifyMusicFlow &message) {
    mX360UserTunes = message.GetPart() != 0;
    mRequestedSwap = mX360UserTunes;
}

void MusicFlow::Reacquire() {
    mElapsed = 0.0f;
    mIntensity = 0.0f;
    mTopSpeed = 0.0f;
    mTimeInPiece = 0.0f;
    mRequestedSwap = false;
    mStartDelay = false;
    mT_currPiece = WorldTimer;
    if (mState == kTransition) {
        ChangeStateTo(kNeutral);
    }
}

void MusicFlow::Update() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai) {
        mTimeInPiece = (WorldTimer - mT_currPiece).GetSeconds();
        mAvgNumCopsInForm = static_cast<float>(ai->GetCopsInFormation().size());
        mAvgNumCopsLOS = static_cast<float>(ai->NumCopsWithLOS());
        mAvgPlayerSpeed = ai->GetPlayerSpeed();
        mAvgPursuitDist = ai->GetPursuitDistance();
        if (mAvgPlayerSpeed > mTopSpeed) {
            mTopSpeed = mAvgPlayerSpeed;
        }
        mElapsed += (WorldTimer - mTimer).GetSeconds();
        mTimer = WorldTimer;
    }

    switch (mState) {
    case kTransition:
        return;
    case kWaiting:
        Waiting();
        return;
    case kNeutral:
        Neutral();
        return;
    case kLose:
        Lose();
        return;
    case kWin:
        Win();
        return;
    case kElude:
        Elude();
        return;
    case kTerminal:
        Terminal();
        return;
    default:
        ChangeStateTo(kWaiting);
        return;
    }
}

float MusicFlow::UpdateIntensity(float adj) {
    mIntensity += adj;
    if (mIntensity < static_cast<float>(kLow)) {
        mIntensity = static_cast<float>(kLow);
    }
    if (mIntensity > static_cast<float>(kHigh)) {
        mIntensity = static_cast<float>(kHigh);
    }
    return mIntensity;
}

void MusicFlow::Waiting() {
    if (mStartDelay) {
        if ((WorldTimer - mTimer).GetSeconds() > 1.0f) {
            mStartDelay = false;
            ChangeStateTo(kNeutral);
        }
        return;
    }
    if (!Manager::IsCopSpeechBusy()) {
        ChangeStateTo(kNeutral);
    }
}

void MusicFlow::Neutral() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kWaiting);
        return;
    }
    if (ai->GetPursuitState() == SoundAI::kSearching) {
        ChangeStateTo(kElude);
        return;
    }
    if (mAvgPlayerSpeed < 6.0f) {
        ChangeStateTo(kLose);
        return;
    }
    if (mRequestedSwap) {
        ChangeStateTo(kWin);
    }
}

void MusicFlow::Lose() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        ChangeStateTo(kNeutral);
    } else if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kWaiting);
    }
}

void MusicFlow::RequestSwap() {
    mRequestedSwap = true;
}

void MusicFlow::Win() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        ChangeStateTo(kNeutral);
        return;
    }

    if (!Manager::IsCopSpeechBusy()) {
        ChangeStateTo(kWaiting);
    }
}

void MusicFlow::Elude() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        ChangeStateTo(kNeutral);
        return;
    }
    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kWin);
    }
}

void MusicFlow::Terminal() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kTransition);
}

bool MusicFlow::IsTransitionable() {
    return mState == kTransition;
}

void MusicFlow::ChangeStateTo(int new_state) {
    SpeechFlow::ChangeStateTo(new_state);
    if (new_state == kTransition) {
        mBusy = 0;
        mRequestedSwap = false;
    }
}

void MusicFlow::Reset() {
    mStartDelay = false;
    mStartEvent = 0;
    mTimer = Timer(0);
    mBoostTimer = Timer(0);
    mT_currPiece = Timer(0);
    mElapsed = 0.0f;
    mIntensity = 0.0f;
    mAvgNumCopsInForm = 0.0f;
    mAvgNumCopsLOS = 0.0f;
    mAvgPlayerSpeed = 0.0f;
    mAvgPursuitDist = 0.0f;
    mCurrentPart = -1;
    mRestrained = true;
    mTopSpeed = 0.0f;
    mTimeInPiece = 0.0f;
    mRequestedSwap = false;
    mX360UserTunes = false;
    mBusy = 0;
    mState = kTransition;
    mLastState = kTransition;
}

} // namespace Speech
