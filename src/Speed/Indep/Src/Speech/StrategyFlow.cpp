#include "StrategyFlow.h"
#include "EAXAirSupport.h"
#include "EAXCop.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"
#include <algorithm>

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

    mMsgReqBackup = Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(this, &StrategyFlow::MessageReqBackup, "ReqBackup", 0);
    mMsgBackupDenied =
        Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(this, &StrategyFlow::MessageBackupDenied, "BackupDenied", 0);
    mMsgNotifyEventCompletion = Hermes::Handler::Create<MNotifySpeechStatus, StrategyFlow, StrategyFlow>(
        this, &StrategyFlow::MessageEventComplete, UCrc32(0x20d60dbf), 0);
}

StrategyFlow::~StrategyFlow() {
    if (mMsgReqBackup) {
        Hermes::Handler::Destroy(mMsgReqBackup);
        mMsgReqBackup = 0;
    }
    if (mMsgBackupDenied) {
        Hermes::Handler::Destroy(mMsgBackupDenied);
        mMsgBackupDenied = 0;
    }
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
        mMsgNotifyEventCompletion = 0;
    }
}

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
    IPursuit *pursuit = ai->GetPursuit();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (!ai->GetLeader()) {
        return;
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kSearching) {
        this->ChangeStateTo(kOutrun);
        return;
    }

    copList active;
    active.reserve(ai->GetActors().size());

    copMap::const_iterator iter = ai->GetActors().begin();
    while (iter != ai->GetActors().end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        }
        ++iter;
    }

    if (active.size() == 1) {
        this->mFlags |= SOLO;
    } else if (active.empty()) {
        this->ChangeStateTo(kTerminal);
        goto cleanup;
    }

    if ((this->mFormationType != pursuit->GetFormationType()) && (pursuit->GetFormationType() != FOLLOW) && (pursuit->GetFormationType() != STAGGER_FOLLOW) &&
        (active.size() > 1)) {
        ai->GetLeader()->StrategyReset(true);
        this->ChangeStateTo(kSoloCheck);
        goto cleanup;
    }

    if (pursuit->IsCollapseActive() || pursuit->IsFinisherActive()) {
        switch (pursuit->GetFormationType()) {
        case PIT:
            if (active.size() < 2) {
                goto cleanup;
            }
            if (bRandom(1.0f) > 0.5f) {
                EAXCop *cop = ai->FindClosestCop(false, false);
                if (cop) {
                    cop->IntentToRam();
                }
            } else {
                ai->GetLeader()->StrategyExecute();
            }
            this->ChangeStateTo(kOutcome);
            goto cleanup;
        case BOX_IN:
        case ROLLING_BLOCK:
            if (active.size() < 2) {
                goto cleanup;
            }
            if (bRandom(1.0f) > 0.5f) {
                ai->GetLeader()->StrategyExecute();
            } else {
                this->CallToPos();
            }
            this->ChangeStateTo(kOutcome);
            goto cleanup;
        default: {
            EAXCop *cop = ai->FindClosestCop(false, false);
            if (cop) {
                cop->Bullhorn();
            }
            goto cleanup;
        }
        case HELI_PURSUIT:
            if (ai->GetHeli()) {
                ai->GetHeli()->SelfStrategy(1);
            }
            goto cleanup;
        }
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        bool contact_strategy = false;
        if ((pursuit->GetFormationType() == PIT) || (pursuit->GetFormationType() == BOX_IN) || (pursuit->GetFormationType() == ROLLING_BLOCK) ||
            (pursuit->GetFormationType() == HERD)) {
            contact_strategy = true;
        }
        if (contact_strategy && ai->GetCopsInFormation().empty() && (active.size() > 1)) {
            this->ChangeStateTo(kCallToPos);
            goto cleanup;
        }

        copMap::const_iterator ci = ai->GetActors().begin();
        while (ci != ai->GetActors().end()) {
            if (ci->cop->GetInFormation()) {
                ci->cop->Bullhorn();
            }
            ++ci;
        }

        if (ai->GetPursuitDistance() > ai->GetTune().SuspectOutrunRange()) {
            this->ChangeStateTo(kOutrun);
            goto cleanup;
        }
    }

    if (ai->GetCopsInFormation().size() >= static_cast<unsigned int>(this->mFormationCount)) {
        if ((pursuit->GetEvadeLevel() <= 0.25f) && (active.size() != 1)) {
            goto no_backup;
        }
    }

    this->mBackupType = 0x20;
    this->mFormationCount = static_cast<int>(ai->GetCopsInFormation().size());
    this->ChangeStateTo(kReqBackup);
cleanup:;
    return;
no_backup:
    this->mFormationCount = static_cast<int>(ai->GetCopsInFormation().size());
}

void StrategyFlow::Outrun() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if ((ai->GetPursuitState() != SoundAI::kSearching) && (ai->GetPursuitState() != SoundAI::kActive)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (!ai->GetLeader()) {
        return;
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    copList losList;
    losList.reserve(ai->GetActors().size());

    copMap::const_iterator iter = ai->GetActors().begin();
    while (iter != ai->GetActors().end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && cop->HasLOS()) {
            losList.push_back(cop);
        }
        ++iter;
    }

    if (losList.size() > 1) {
        this->ChangeStateTo(kWaiting);
        return;
    }

    ai->GetTune().PursuitInactivityTimer(0);
    float t_comment = ai->GetTune().NoLOSCommentaryTime();

    if ((losList.size() == 1) && !ai->AreCopsAhead() && (ai->GetPursuitDistance() >= ai->GetTune().SuspectOutrunRange())) {
        EAXCop *outrunee = losList.front();
        outrunee->SuspectOutrun();
        this->ChangeStateTo(kWaiting);
        return;
    }

    if (losList.empty()) {
        if (ai->GetPerpLostTime() >= t_comment) {
            copList closeList;
            closeList.reserve(ai->GetActors().size());

            iter = ai->GetActors().begin();
            while (iter != ai->GetActors().end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive()) {
                    closeList.push_back(cop);
                }
                ++iter;
            }

            if (!closeList.empty()) {
                if (closeList.size() > 1) {
                    std::sort(closeList.begin(), closeList.end());
                }

                EAXCop *closest = closeList.front();
                if (closest->IsHeli()) {
                    ai->GetHeli()->LostVisual();
                } else {
                    closest->LostVisual();

                    EAXAirSupport *heli = ai->GetHeli();
                    if (heli && heli->HasLOS() && heli->IsActive()) {
                        heli->Spotter();
                        heli->LocationReport();
                    }
                }

            }

            this->ChangeStateTo(kLost);
            return;
        }

    }

    this->ChangeStateTo(kWaiting);
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
