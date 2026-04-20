#include "StrategyFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"

namespace Speech {

StrategyFlow::StrategyFlow()
    : mFlags(0), //
      mDistance(), //
      mSpeed(), //
      mLOSCount(0), //
      mFormationCount(0), //
      mFormationType(0), //
      mBackupType(0), //
      mT_requested(0), //
      mLastBackupType(0), //
      mMsgReqBackup(0), //
      mMsgBackupDenied(0), //
      mMsgNotifyEventCompletion(0) {
    mDistance[0] = 0.0f;
    mDistance[1] = 0.0f;
    mSpeed[0] = 0.0f;
    mSpeed[1] = 0.0f;
    mState = kCullCheck;
    mLastState = kTransition;
}

StrategyFlow::~StrategyFlow() {}

void StrategyFlow::Reset() {
    mFlags = 0;
    mDistance[0] = 0.0f;
    mDistance[1] = 0.0f;
    mSpeed[0] = 0.0f;
    mSpeed[1] = 0.0f;
    mLOSCount = 0;
    mFormationCount = 0;
    mFormationType = 0;
    mBackupType = 0;
    mLastBackupType = 0;
    mBusy = 0;
    ChangeStateTo(kCullCheck);
}

void StrategyFlow::Update() {
    switch (mState) {
    case kTransition:
        return;
    case kCullCheck:
        CullCheck();
        return;
    case kSoloCheck:
        SoloCheck();
        return;
    case kReqBackup:
        ReqBackup();
        return;
    case kCallToPos:
        CallToPos();
        return;
    case kWaiting:
        Waiting();
        return;
    case kOutrun:
        Outrun();
        return;
    case kLost:
        Lost();
        return;
    case kTerminal:
        Terminal();
        return;
    case kOutcome:
        Outcome();
        return;
    default:
        ChangeStateTo(kCullCheck);
        return;
    }
}

void StrategyFlow::CullCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kLost);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kTerminal);
        return;
    }

    ChangeStateTo(kSoloCheck);
}

void StrategyFlow::SoloCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    mLOSCount = ai->NumCopsWithLOS();
    mFormationCount = static_cast<int>(ai->GetCopsInFormation().size());
    mDistance[0] = mDistance[1];
    mDistance[1] = ai->GetPursuitDistance();
    mSpeed[0] = mSpeed[1];
    mSpeed[1] = ai->GetPlayerSpeed();

    if ((mLOSCount <= 0) || (mFormationCount <= 1)) {
        ChangeStateTo(kReqBackup);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kSearching) {
        ChangeStateTo(kOutrun);
        return;
    }

    ChangeStateTo(kWaiting);
}

void StrategyFlow::CallToPos() {
    if (!mBusy) {
        mBusy = 1;
        mT_requested = WorldTimer;
        return;
    }

    if (!Manager::IsCopSpeechBusy()) {
        mBusy = 0;
        ChangeStateTo(kWaiting);
    }
}

void StrategyFlow::ReqBackup() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (!mBusy) {
        mBackupType = ai->GetFocus();
        mLastBackupType = mBackupType;
        mFlags |= REQUESTABLE;
        mBusy = 1;
        mT_requested = WorldTimer;
        return;
    }

    if (!Manager::IsCopSpeechBusy()) {
        mBusy = 0;
        if ((mFlags & BUDENIED) != 0) {
            ChangeStateTo(kTerminal);
        } else {
            ChangeStateTo(kCallToPos);
        }
    }
}

void StrategyFlow::Waiting() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kLost);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kTerminal);
        return;
    }

    if ((mFlags & REQUESTABLE) == 0) {
        ChangeStateTo(kReqBackup);
        return;
    }

    if ((WorldTimer - mT_requested).GetSeconds() > 3.0f) {
        ChangeStateTo(kOutcome);
    }
}

void StrategyFlow::Outrun() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        ChangeStateTo(kSoloCheck);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kLost);
    }
}

void StrategyFlow::Lost() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kTransition);
}

void StrategyFlow::Terminal() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kTransition);
}

bool StrategyFlow::IsTransitionable() {
    return mState == kTransition;
}

void StrategyFlow::Outcome() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kWaiting);
}

void StrategyFlow::MessageReqBackup(const MReqBackup &) {
    mFlags |= REQUESTABLE;
    mT_requested = WorldTimer;
}

void StrategyFlow::MessageBackupDenied(const MReqBackup &) {
    mFlags |= BUDENIED;
}

void StrategyFlow::MessageEventComplete(const MNotifySpeechStatus &) {
    mBusy = 0;
}

} // namespace Speech
