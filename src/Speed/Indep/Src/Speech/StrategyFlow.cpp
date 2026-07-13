#include "StrategyFlow.h"
#include "EAXAirSupport.h"
#include "EAXCop.h"
#include "EAXDispatch.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"
#include <algorithm>

extern "C" void SwarmingReply__10MiscSpeech();
extern "C" void SwarmingReplyFollow__10MiscSpeech();

namespace Speech {

StrategyFlow::StrategyFlow()
    : mFlags(0), //
      mLOSCount(0), //
      mFormationCount(0), //
      mFormationType(0), //
      mBackupType(0), //
      mT_requested(0), //
      mLastBackupType(0), //
      mMsgReqBackup(Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(
          this, &StrategyFlow::MessageReqBackup, "Request", 0)), //
      mMsgBackupDenied(Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(
          this, &StrategyFlow::MessageBackupDenied, "ReqDenied", 0)), //
      mMsgNotifyEventCompletion(Hermes::Handler::Create<MNotifySpeechStatus, StrategyFlow, StrategyFlow>(
          this, &StrategyFlow::MessageEventComplete, UCrc32(0x20d60dbf), 0)) {}

StrategyFlow::~StrategyFlow() {
    if (mMsgReqBackup) {
        Hermes::Handler::Destroy(mMsgReqBackup);
    }
    if (mMsgBackupDenied) {
        Hermes::Handler::Destroy(mMsgBackupDenied);
    }
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
    }
}

void StrategyFlow::Reset() {
    Terminal();
    SpeechFlow::Reset();
}

void StrategyFlow::Update() {
    int y;
    int x;

    if (SPEECHFLOW_DISPLAY != 0) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai->GetPursuitState() == SoundAI::kActive) {
            switch (ai->GetPursuit()->GetFormationType()) {
            case PIT:
                y = 0;
                break;
            case BOX_IN:
                y = 1;
                break;
            case ROLLING_BLOCK:
                y = 2;
                break;
            case FOLLOW:
            case STAGGER_FOLLOW:
                y = 3;
                break;
            case HELI_PURSUIT:
                y = 4;
                break;
            case HERD:
                y = 5;
                break;
            default:
                y = 6;
                break;
            }
        }
    }

    switch (mState) {
    case kTransition:
        return;
    case kCullCheck:
        CullCheck();
        return;
    case kSoloCheck:
        SoloCheck();
        return;
    case kWaiting:
        Waiting();
        return;
    case kTerminal:
        Terminal();
        return;
    case kOutcome:
        Outcome();
        return;
    case kOutrun:
        Outrun();
        return;
    case kLost:
        Lost();
        return;
    case kReqBackup:
        ReqBackup();
        return;
    case kCallToPos:
        CallToPos();
        return;
    default:
        ChangeStateTo(kCullCheck);
        return;
    }
}

void StrategyFlow::CullCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching) {
        ChangeStateTo(kSoloCheck);
    }
}

void StrategyFlow::SoloCheck() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    IPursuit *pursuit;

    mDistance[0] = ai->GetPursuitDistance();
    mSpeed[0] = ai->GetPlayerSpeed();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != SoundAI::kStrategyFlow)) {
        ChangeStateTo(kTerminal);
        return;
    }

    if (!ai->GetLeader()) {
        return;
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    {
        copList active;
        active.reserve(ai->GetActors().size());
        {
            copMap::const_iterator iter = ai->GetActors().begin();
            while (iter != ai->GetActors().end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive()) {
                    active.push_back(cop);
                }
                ++iter;
            }
        }

        if ((active.size() == 1) || (ai->NumCopsWithLOS() == 0)) {
            mBackupType = 32;
            ChangeStateTo(kReqBackup);
            return;
        }

        pursuit = ai->GetPursuit();
        mFormationType = pursuit->GetFormationType();
        if ((ai->NumCopsWithLOS() <= 1) || (ai->GetCopsInFormation().size() <= 1)) {
            if (ai->GetLeader()->IsHeli()) {
                if (ai->GetHeli()->IsActive()) {
                    ai->GetHeli()->SelfStrategy(1);
                }
            } else if (ai->GetLeader()->IsActive()) {
                ai->GetLeader()->SelfStrategy(mFormationType);
            }
            mFlags |= SOLO;
        } else {
            if (ai->GetLeader()->IsActive()) {
                ai->GetLeader()->InitiateStrategy(mFormationType);
            }
            mFlags &= ~SOLO;
        }
        mDistance[0] = ai->GetPursuitDistance();
        ChangeStateTo(kWaiting);
    }
}

void StrategyFlow::CallToPos() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    IPursuit *pursuit = ai->GetPursuit();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != SoundAI::kStrategyFlow)) {
        ChangeStateTo(kTerminal);
        return;
    }

    EAXCop *leader = ai->GetLeader();
    if (!leader && !pursuit) {
        return;
    }

    if (leader->IsHeli()) {
        ChangeStateTo(kWaiting);
        return;
    }

    if (!ai->GetLeader()->IsActive()) {
        return;
    }

    {
        copMap::const_iterator ci = ai->GetActors().begin();
        while (ci != ai->GetActors().end()) {
            if (ci->cop->GetInFormation() && ci->cop->IsActive() && !ci->cop->GetInPosition()) {
                if (Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0x54))) {
                    ai->GetLeader()->CallToPositionReminder();
                } else {
                    ai->GetLeader()->CallToPosition(ci->cop);
                }

                if ((Manager::GetLastEventID() == static_cast<SPCHType_1_EventID>(0x54)) ||
                    (Manager::GetLastEventID() == static_cast<SPCHType_1_EventID>(0x56))) {
                    if (!Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x39), 4)) {
                        ci->cop->Ack();
                    }
                }
            }
            ++ci;
        }
    }

    ChangeStateTo(kWaiting);
}

void StrategyFlow::ReqBackup() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != 2)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    IPursuit *pursuit = ai->GetPursuit();
    if (!pursuit) {
        return;
    }

    if (!Manager::IsCopSpeechBusy()) {
        if (!mBusy) {
            copList active;
            EAXCop *caller;
            active.reserve(ai->GetActors().size());

            copMap::const_iterator iter = ai->GetActors().begin();
            while (iter != ai->GetActors().end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive()) {
                    active.push_back(cop);
                }
                ++iter;
            }

            if (active.empty()) {
                return;
            } else {
                if (active.size() > 1) {
                    std::sort(active.begin(), active.end());
                }

                caller = active.front();
                if (caller) {
                    bool eta_valid = false;
                    if (pursuit->GetBackupETA() > 10.0f) {
                        if (pursuit->GetBackupETA() < 200.0f) {
                            eta_valid = true;
                        }
                    }

                    if ((Manager::mGlobalHistory.GetCount(static_cast<SPCHType_1_EventID>(0xa1)) == 0) && !ai->IsHighIntensity()) {
                        caller->InitialCallForBackup();
                        if (!eta_valid || (bRandom(1.0f) <= 0.5f) || ((this->mFlags & BUDENIED) != 0)) {
                            goto initial_backup_reply;
                        }

                    backup_eta:
                        ai->GetDispatch()->BackupETA();
                        goto backup_done;

                    initial_backup_reply: {
                            unsigned int flags = this->mFlags;
                            ai->GetDispatch()->BackupReply(caller, ((flags ^ BUDENIED) >> 1) & 1, this->mBackupType);
                            if ((flags & BUDENIED) != 0) {
                                caller->NegativeBackupReply();
                            }
                            goto backup_done;
                    }
                    } else if ((Manager::mGlobalHistory.GetCount(static_cast<SPCHType_1_EventID>(0x43)) == 0) ||
                               (ai->GetNumActiveCopCars() > 1)) {
                        caller->CallForBackup(this->mBackupType);
                        if (eta_valid && (bRandom(1.0f) > 0.5f) && ((this->mFlags & BUDENIED) == 0)) {
                            goto backup_eta;
                        } else {
                            ai->GetDispatch()->BackupReply(caller, ((this->mFlags ^ BUDENIED) >> 1) & 1, this->mBackupType);
                            if ((this->mFlags & BUDENIED) != 0) {
                                caller->NegativeBackupReply();
                            }
                        }
                    } else if ((Manager::mGlobalHistory.GetCount(static_cast<SPCHType_1_EventID>(0x49)) == 0) && !ai->IsHighIntensity()) {
                        float t_since_req = (WorldTimer - this->mT_requested).GetSeconds();
                        if ((t_since_req >= ai->GetTune().BURemindTime()) && (this->mLastBackupType == this->mBackupType)) {
                            caller->BackupReminder(this->mBackupType);
                        } else {
                            caller->CallForBackup(this->mBackupType);
                        }
                        if (eta_valid && (bRandom(1.0f) > 0.5f) && ((this->mFlags & BUDENIED) == 0)) {
                            goto backup_eta;
                        } else {
                            ai->GetDispatch()->BackupUpdate(caller, ((this->mFlags ^ BUDENIED) >> 1) & 1);
                        }
                    } else {
                        caller->CallForSwarming();
                        if ((this->mFlags & BUDENIED) == 0) {
                            ai->GetDispatch()->BackupETA();
                            if (ai->GetHeli() && (bRandom(1.0f) > 0.5f)) {
                                ai->GetHeli()->Swarming();
                            } else {
                                SwarmingReply__10MiscSpeech();
                            }
                            SwarmingReplyFollow__10MiscSpeech();
                        } else {
                            ai->GetDispatch()->BackupUpdate(caller, ((this->mFlags ^ BUDENIED) >> 1) & 1);
                        }
                    }

                backup_done:
                    this->mBusy++;
                    return;
                }
            }
        }
    }

    if (this->mBusy) {
        if (!Manager::IsCopSpeechBusy()) {
            this->mBusy = 0;
            this->ChangeStateTo(this->mLastState);
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

    {
        copMap::const_iterator iter = ai->GetActors().begin();
        while (iter != ai->GetActors().end()) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive()) {
                active.push_back(cop);
            }
            ++iter;
        }
    }

    if (active.size() == 1) {
        this->mFlags |= SOLO;
    } else if (active.empty()) {
        this->ChangeStateTo(kTerminal);
        goto cleanup;
    }

    if ((this->mFormationType != ai->GetPursuit()->GetFormationType()) && (ai->GetPursuit()->GetFormationType() != FOLLOW) &&
        (ai->GetPursuit()->GetFormationType() != STAGGER_FOLLOW) &&
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
                EAXCop *closest = ai->FindClosestCop(false, false);
                if (closest) {
                    closest->IntentToRam();
                }
            } else {
                ai->GetLeader()->StrategyExecute();
            }
            this->ChangeStateTo(kOutcome);
            goto cleanup;
        case HELI_PURSUIT:
            if (ai->GetHeli()) {
                ai->GetHeli()->SelfStrategy(1);
            }
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
            EAXCop *closest = ai->FindClosestCop(false, false);
            if (closest) {
                closest->Bullhorn();
            }
            goto cleanup;
        }
        }
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        bool contact_strategy = false;
        if ((pursuit->GetFormationType() == PIT) || (pursuit->GetFormationType() == BOX_IN) || (pursuit->GetFormationType() == ROLLING_BLOCK) ||
            (pursuit->GetFormationType() == HERD)) {
            contact_strategy = true;
        }
        if (contact_strategy && (ai->GetCopsInFormation().size() == 0) && (active.size() > 1)) {
            this->ChangeStateTo(kCallToPos);
            goto cleanup;
        }

        {
            copMap::const_iterator ci = ai->GetActors().begin();
            while (ci != ai->GetActors().end()) {
                if (ci->cop->GetInFormation()) {
                    ci->cop->Bullhorn();
                }
                ++ci;
            }
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

    {
        copList losList;
        float t_permalost;
        float t_comment;
        losList.reserve(ai->GetActors().size());

        {
            copMap::const_iterator iter = ai->GetActors().begin();
            while (iter != ai->GetActors().end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive() && cop->HasLOS()) {
                    losList.push_back(cop);
                }
                ++iter;
            }
        }

        if (losList.size() > 1) {
            this->ChangeStateTo(kWaiting);
            return;
        }

        ai->GetTune().PursuitInactivityTimer(0);
        t_permalost = ai->GetTune().TimeConsideredLostNoLOS();
        t_comment = ai->GetTune().NoLOSCommentaryTime();

        if ((losList.size() == 1) && !ai->AreCopsAhead() && (ai->GetPursuitDistance() >= ai->GetTune().SuspectOutrunRange())) {
            EAXCop *outrunee = losList.front();
            outrunee->SuspectOutrun();
            this->ChangeStateTo(kWaiting);
            goto cleanup;
        }

        if (losList.empty()) {
            if (ai->GetPerpLostTime() >= t_comment) {
                copList closeList;
                closeList.reserve(ai->GetActors().size());

                {
                    copMap::const_iterator iter = ai->GetActors().begin();
                    while (iter != ai->GetActors().end()) {
                        EAXCop *cop = iter->cop;
                        if (cop->IsActive()) {
                            closeList.push_back(cop);
                        }
                        ++iter;
                    }
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

                        if (ai->GetHeli() && ai->GetHeli()->HasLOS() && ai->GetHeli()->IsActive()) {
                            ai->GetHeli()->Spotter();
                            ai->GetHeli()->LocationReport();
                        }
                    }
                }

                this->ChangeStateTo(kLost);
                goto cleanup;
            }
        }

        this->ChangeStateTo(kWaiting);
    cleanup:;
    }
}

void StrategyFlow::Lost() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() != SoundAI::kInactive) {
        ChangeStateTo(kTerminal);
    }
}

void StrategyFlow::Terminal() {
    mBusy = 0;
    ChangeStateTo(kTransition);
    mLOSCount = 0;
    mDistance[1] = 0.0f;
    mDistance[0] = 0.0f;
    mSpeed[1] = 0.0f;
    mSpeed[0] = 0.0f;
    mFormationCount = 0;
    mBackupType = 0;
    mFormationType = 0;
    mFlags = 0;
    mT_requested = Timer(0);
}

bool StrategyFlow::IsTransitionable() {
    if (mState == kTransition || mState == kWaiting) {
        return true;
    }
    return false;
}

void StrategyFlow::Outcome() {
    SoundAI *ai = SoundAI::Get();
    IPursuit *pursuit;
    if (!ai->GetPursuit()) {
        return;
    }
    if (!ai->GetLeader()) {
        return;
    }
    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    {
        mDistance[1] = ai->GetPursuitDistance();
        float delta = mDistance[1] - mDistance[0];
        mSpeed[1] = ai->GetPlayerSpeed();
        float deltav = mSpeed[1] - mSpeed[0];
        if ((delta > 0.0f) || (deltav > 0.0f)) {
            if ((ai->GetPursuitDistance() > 50.0f) || (ai->NumCopsWithLOS() == 0) || !ai->AreCopsAhead()) {
                {
                    EAXCop *spokesperson;
                    if (ai->GetLeader()->IsHeli()) {
                        spokesperson = ai->FindClosestCop(false, false);
                    } else {
                        spokesperson = ai->GetLeader();
                    }
                    spokesperson->AnticipateFail();
                }
            } else {
                ai->GetLeader()->StrategyReset(mFormationType != ai->GetPursuit()->GetFormationType());
            }
        } else if (((mFlags ^ SOLO) & SOLO) != 0) {
            ai->GetLeader()->AnticipateSuccess();
        }
    }
    ChangeStateTo(kWaiting);
}

void StrategyFlow::MessageReqBackup(const MReqBackup &message) {
    mBackupType = message.GetBackupType();
    SoundAI *ai = SoundAI::Get();
    mFlags &= ~BUDENIED;
    if (ai->GetPursuitState() == SoundAI::kActive && ai->GetFocus() == 2) {
        ChangeStateTo(kReqBackup);
    }
}

void StrategyFlow::MessageBackupDenied(const MReqBackup &message) {
    mBackupType = message.GetBackupType();
    SoundAI *ai = SoundAI::Get();
    mFlags |= BUDENIED;
    if (ai->GetPursuitState() == SoundAI::kActive && ai->GetFocus() == 2) {
        ChangeStateTo(kReqBackup);
    }
}

void StrategyFlow::MessageEventComplete(const MNotifySpeechStatus &message) {
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech != nullptr) {
        switch (speech->ID) {
        case static_cast<SPCHType_1_EventID>(67): {
            Csis::Backup_CallForBUStruct *data = static_cast<Csis::Backup_CallForBUStruct *>(speech->GetData(nullptr));
            if (data != nullptr) {
                mLastBackupType = data->backup_type;
                mT_requested = WorldTimer;
            }
            break;
        }
        case static_cast<SPCHType_1_EventID>(73): {
            Csis::Backup_BUReminderStruct *data = static_cast<Csis::Backup_BUReminderStruct *>(speech->GetData(nullptr));
            if (data != nullptr) {
                mLastBackupType = data->backup_type;
                mT_requested = WorldTimer;
            }
            break;
        }
        default:
            break;
        }
    }
}

} // namespace Speech
