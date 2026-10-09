#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/StrategyFlow.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

namespace Speech {

StrategyFlow::StrategyFlow()
    : mFlags(0),                                                                                                                                   //
      mLOSCount(0),                                                                                                                                //
      mFormationCount(0),                                                                                                                          //
      mFormationType(0),                                                                                                                           //
      mBackupType(0),                                                                                                                              //
      mT_requested(0),                                                                                                                             //
      mLastBackupType(0),                                                                                                                          //
      mMsgReqBackup(Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(this, &StrategyFlow::MessageReqBackup, "Request", 0)),         //
      mMsgBackupDenied(Hermes::Handler::Create<MReqBackup, StrategyFlow, StrategyFlow>(this, &StrategyFlow::MessageBackupDenied, "ReqDenied", 0)), //
      mMsgNotifyEventCompletion(Hermes::Handler::Create<MNotifySpeechStatus, StrategyFlow, StrategyFlow>(this, &StrategyFlow::MessageEventComplete,
                                                                                                         UCrc32(UCRC32_Gameplay), 0)) {}

StrategyFlow::~StrategyFlow() {
    if (this->mMsgReqBackup != nullptr) {
        Hermes::Handler::Destroy(this->mMsgReqBackup);
    }
    if (this->mMsgBackupDenied != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBackupDenied);
    }
    if (this->mMsgNotifyEventCompletion != nullptr) {
        Hermes::Handler::Destroy(this->mMsgNotifyEventCompletion);
    }
}

void StrategyFlow::Reset() {
    this->Terminal();
    this->SpeechFlow::Reset();
}

void StrategyFlow::Update() {
    int y;
    int x;

    if (SPEECHFLOW_DISPLAY) {
        SoundAI *ai = SoundAI::Get();
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

    switch (this->mState) {
        case kTransition:
            return;
        case kCullCheck:
            this->CullCheck();
            return;
        case kSoloCheck:
            this->SoloCheck();
            return;
        case kWaiting:
            this->Waiting();
            return;
        case kTerminal:
            this->Terminal();
            return;
        case kOutcome:
            this->Outcome();
            return;
        case kOutrun:
            this->Outrun();
            return;
        case kLost:
            this->Lost();
            return;
        case kReqBackup:
            this->ReqBackup();
            return;
        case kCallToPos:
            this->CallToPos();
            return;
        default:
            this->ChangeStateTo(kCullCheck);
            return;
    }
}

void StrategyFlow::CullCheck() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching) {
        this->ChangeStateTo(kSoloCheck);
    }
}

void StrategyFlow::SoloCheck() {
    SoundAI *ai = SoundAI::Get();
    ai->GetPursuit();

    this->mDistance[0] = ai->GetPursuitDistance();
    this->mSpeed[0] = ai->GetPlayerSpeed();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != SoundAI::kStrategyFlow)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetLeader() == nullptr) {
        return;
    }

    IPursuit *pursuit;
    if (!Manager::IsCopSpeechBusy()) {
        copList active;
        active.reserve(ai->GetActors().size());
        for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive()) {
                active.push_back(cop);
            }
        }

        if ((active.size() == 1) || (ai->NumCopsWithLOS() == 0)) {
            this->mBackupType = 32;
            this->ChangeStateTo(kReqBackup);
            return;
        }

        pursuit = ai->GetPursuit();
        this->mFormationType = pursuit->GetFormationType();
        if ((ai->NumCopsWithLOS() <= 1) || (ai->GetCopsInFormation().size() <= 1)) {
            if (ai->GetLeader()->IsHeli() && ai->GetHeli()->IsActive()) {
                ai->GetHeli()->SelfStrategy(1);
            } else if (ai->GetLeader()->IsActive()) {
                ai->GetLeader()->SelfStrategy(this->mFormationType);
            }
            this->mFlags |= SOLO;
        } else {
            if (ai->GetLeader()->IsActive()) {
                ai->GetLeader()->InitiateStrategy(this->mFormationType);
                this->mFlags &= ~SOLO;
            }
        }
        this->mDistance[0] = ai->GetPursuitDistance();
        this->ChangeStateTo(kWaiting);
    }
}

void StrategyFlow::CallToPos() {
    SoundAI *ai = SoundAI::Get();
    IPursuit *pursuit = ai->GetPursuit();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != SoundAI::kStrategyFlow)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetLeader() == nullptr && ai->GetPursuit() == nullptr) {
        return;
    }

    if (ai->GetLeader()->IsHeli()) {
        this->ChangeStateTo(kWaiting);
        return;
    }

    if (!ai->GetLeader()->IsActive()) {
        return;
    }

    for (copMap::const_iterator ci = ai->GetActors().begin(); ci != ai->GetActors().end(); ++ci) {
        if (ci->cop->GetInFormation() && ci->cop->IsActive() && !ci->cop->GetInPosition()) {
            if (Manager::HasBeenSaid(kSPCH1_EventID_CallToPosition)) {
                ai->GetLeader()->CallToPositionReminder();
            } else {
                ai->GetLeader()->CallToPosition(ci->cop);
            }

            if ((Manager::GetLastEventID() == kSPCH1_EventID_CallToPosition) || (Manager::GetLastEventID() == kSPCH1_EventID_CallToPositionRem)) {
                if (!Manager::IsQueued(kSPCH1_EventID_Acknowledge, 4)) {
                    ci->cop->Ack();
                }
            }
        }
    }

    this->ChangeStateTo(kWaiting);
}

void StrategyFlow::ReqBackup() {
    SoundAI *ai = SoundAI::Get();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (ai->GetFocus() != 2)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    IPursuit *pursuit = ai->GetPursuit();
    if (pursuit == nullptr) {
        return;
    }

    if (!Manager::IsCopSpeechBusy() && !this->mBusy) {
        copList active;
        active.reserve(ai->GetActors().size());

        for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive()) {
                active.push_back(cop);
            }
        }

        if (active.empty()) {
            return;
        }

        if (active.size() > 1) {
            std::sort(active.begin(), active.end());
        }

        EAXCop *caller = active.front();
        if (caller != nullptr) {
            bool eta_valid = false;
            if (pursuit->GetBackupETA() > 10.0f) {
                if (pursuit->GetBackupETA() < 200.0f) {
                    eta_valid = true;
                }
            }

            if ((Manager::GetHistory().GetCount(kSPCH1_EventID_InitialCallForBU) == 0) && !ai->IsHighIntensity()) {
                caller->InitialCallForBackup();
                if (eta_valid && (bRandom(1.0f) > 0.5f) && ((this->mFlags & BUDENIED) == 0)) {
                    ai->GetDispatch()->BackupETA();
                } else {
                    ai->GetDispatch()->BackupReply(caller, (this->mFlags & BUDENIED) == 0, this->mBackupType);
                    if ((this->mFlags & BUDENIED) != 0) {
                        caller->NegativeBackupReply();
                    }
                }
                this->mBusy++;
                return;
            } else if ((Manager::GetHistory().GetCount(kSPCH1_EventID_CallForBU) == 0) || (ai->GetNumActiveCopCars() > 1)) {
                caller->CallForBackup(this->mBackupType);
                if (eta_valid && (bRandom(1.0f) > 0.5f) && ((this->mFlags & BUDENIED) == 0)) {
                    ai->GetDispatch()->BackupETA();
                } else {
                    ai->GetDispatch()->BackupReply(caller, (this->mFlags & BUDENIED) == 0, this->mBackupType);
                    if ((this->mFlags & BUDENIED) != 0) {
                        caller->NegativeBackupReply();
                    }
                }
                this->mBusy++;
                return;
            } else if ((Manager::GetHistory().GetCount(kSPCH1_EventID_BUReminder) == 0) && !ai->IsHighIntensity()) {
                float t_since_req = (WorldTimer - this->mT_requested).GetSeconds();
                if ((t_since_req >= ai->GetTune().BURemindTime()) && (this->mLastBackupType == this->mBackupType)) {
                    caller->BackupReminder(this->mBackupType);
                } else {
                    caller->CallForBackup(this->mBackupType);
                }
                if (eta_valid && (bRandom(1.0f) > 0.5f) && ((this->mFlags & BUDENIED) == 0)) {
                    ai->GetDispatch()->BackupETA();
                } else {
                    ai->GetDispatch()->BackupUpdate(caller, (this->mFlags & BUDENIED) == 0);
                }
                this->mBusy++;
                return;
            } else {
                caller->CallForSwarming();
                if ((this->mFlags & BUDENIED) == 0) {
                    ai->GetDispatch()->BackupETA();
                    if (ai->GetHeli() != nullptr && (bRandom(1.0f) > 0.5f)) {
                        ai->GetHeli()->Swarming();
                    } else {
                        MiscSpeech::SwarmingReply();
                    }
                    MiscSpeech::SwarmingReplyFollow();
                } else {
                    ai->GetDispatch()->BackupUpdate(caller, (this->mFlags & BUDENIED) == 0);
                }
                this->mBusy++;
                return;
            }
        }
    }

    if (this->mBusy && !Manager::IsCopSpeechBusy()) {
        this->mBusy = 0;
        this->ChangeStateTo(this->mLastState);
    }
}

void StrategyFlow::Waiting() {
    SoundAI *ai = SoundAI::Get();
    IPursuit *pursuit = ai->GetPursuit();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetLeader() == nullptr) {
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

    for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        }
    }

    if (active.size() == 1) {
        this->mFlags |= SOLO;
    } else if (active.empty()) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if ((this->mFormationType != ai->GetPursuit()->GetFormationType()) && (ai->GetPursuit()->GetFormationType() != FOLLOW) &&
        (ai->GetPursuit()->GetFormationType() != STAGGER_FOLLOW) && (active.size() > 1)) {
        ai->GetLeader()->StrategyReset(true);
        this->ChangeStateTo(kSoloCheck);
        return;
    }

    if (pursuit->IsCollapseActive() || pursuit->IsFinisherActive()) {
        switch (pursuit->GetFormationType()) {
            case PIT:
                if (active.size() < 2) {
                    break;
                }
                if (bRandom(1.0f) > 0.5f) {
                    EAXCop *closest = ai->FindClosestCop(false, false);
                    if (closest != nullptr) {
                        closest->IntentToRam();
                    }
                } else {
                    ai->GetLeader()->StrategyExecute();
                }
                this->ChangeStateTo(kOutcome);
                break;
            case HELI_PURSUIT:
                if (ai->GetHeli() != nullptr) {
                    ai->GetHeli()->SelfStrategy(1);
                }
                break;
            case BOX_IN:
            case ROLLING_BLOCK:
                if (active.size() < 2) {
                    break;
                }
                if (bRandom(1.0f) > 0.5f) {
                    ai->GetLeader()->StrategyExecute();
                } else {
                    this->CallToPos();
                }
                this->ChangeStateTo(kOutcome);
                break;
            case FOLLOW:
            case HERD:
            case STAGGER_FOLLOW:
            default: {
                EAXCop *closest = ai->FindClosestCop(false, false);
                if (closest != nullptr) {
                    closest->Bullhorn();
                }
                break;
            }
        }
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kActive) {
        bool contact_strategy = false;
        if ((pursuit->GetFormationType() == PIT) || (pursuit->GetFormationType() == BOX_IN) || (pursuit->GetFormationType() == ROLLING_BLOCK) ||
            (pursuit->GetFormationType() == HERD)) {
            contact_strategy = true;
        }
        if (contact_strategy && (ai->GetCopsInFormation().size() == 0) && (active.size() > 1)) {
            this->ChangeStateTo(kCallToPos);
            return;
        }

        for (copMap::const_iterator ci = ai->GetActors().begin(); ci != ai->GetActors().end(); ++ci) {
            if (ci->cop->GetInFormation()) {
                ci->cop->Bullhorn();
            }
        }

        if (ai->GetPursuitDistance() > ai->GetTune().SuspectOutrunRange()) {
            this->ChangeStateTo(kOutrun);
            return;
        }
    }

    if ((ai->GetCopsInFormation().size() < this->mFormationCount) || (pursuit->GetEvadeLevel() > 0.25f) || (active.size() == 1)) {
        this->mFormationCount = ai->GetCopsInFormation().size();
        this->mBackupType = 32; // TODO magic
        this->ChangeStateTo(kReqBackup);
        return;
    }
    this->mFormationCount = ai->GetCopsInFormation().size();
}

void StrategyFlow::Outrun() {
    SoundAI *ai = SoundAI::Get();

    if ((ai->GetPursuitState() != SoundAI::kSearching) && (ai->GetPursuitState() != SoundAI::kActive)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetLeader() == nullptr) {
        return;
    }

    if (!Manager::IsCopSpeechBusy()) {
        copList losList;
        losList.reserve(ai->GetActors().size());

        for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive() && cop->HasLOS()) {
                losList.push_back(cop);
            }
        }

        if (losList.size() > 1) {
            this->ChangeStateTo(kWaiting);
            return;
        }

        ai->GetTune().PursuitInactivityTimer(0);
        float t_permalost = ai->GetTune().TimeConsideredLostNoLOS();
        float t_comment = ai->GetTune().NoLOSCommentaryTime();

        if ((losList.size() == 1) && !ai->AreCopsAhead() && (ai->GetPursuitDistance() >= ai->GetTune().SuspectOutrunRange())) {
            EAXCop *outrunee = losList.front();
            outrunee->SuspectOutrun();
            this->ChangeStateTo(kWaiting);
        } else if (losList.empty() && (ai->GetPerpLostTime() >= t_comment)) {
            copList closeList;
            closeList.reserve(ai->GetActors().size());

            for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive()) {
                    closeList.push_back(cop);
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

                    if (ai->GetHeli() != nullptr && ai->GetHeli()->HasLOS() && ai->GetHeli()->IsActive()) {
                        ai->GetHeli()->Spotter();
                        ai->GetHeli()->LocationReport();
                    }
                }
            }

            this->ChangeStateTo(kLost);
        } else {
            this->ChangeStateTo(kWaiting);
        }
    }
}

void StrategyFlow::Lost() {
    SoundAI *ai = SoundAI::Get();
    if ((ai->GetPursuitState() != SoundAI::kInactive) || (ai->GetPursuitState() != SoundAI::kInactive) ||
        (ai->GetPursuitState() != SoundAI::kInactive)) {
        this->ChangeStateTo(kTerminal);
    }
}

void StrategyFlow::Terminal() {
    this->mBusy = 0;
    this->ChangeStateTo(kTransition);
    this->mDistance[0] = this->mDistance[1] = 0.0f;
    this->mSpeed[0] = this->mSpeed[1] = 0.0f;
    this->mLOSCount = 0;
    this->mFormationCount = 0;
    this->mBackupType = 0;
    this->mFormationType = 0;
    this->mFlags = 0;
    this->mT_requested = Timer(0);
}

bool StrategyFlow::IsTransitionable() {
    if (this->mState == kTransition || this->mState == kWaiting) {
        return true;
    }
    return false;
}

void StrategyFlow::Outcome() {
    SoundAI *ai = SoundAI::Get();
    IPursuit *pursuit = ai->GetPursuit();
    if (pursuit == nullptr) {
        return;
    }
    if (ai->GetLeader() == nullptr) {
        return;
    }
    if (Manager::IsCopSpeechBusy()) {
        return;
    } else {
        this->mDistance[1] = ai->GetPursuitDistance();
        float delta = this->mDistance[1] - this->mDistance[0];
        this->mSpeed[1] = ai->GetPlayerSpeed();
        float deltav = this->mSpeed[1] - this->mSpeed[0];
        if ((delta > 0.0f) || (deltav > 0.0f)) {
            if ((ai->GetPursuitDistance() > 50.0f) || (ai->NumCopsWithLOS() == 0) || !ai->AreCopsAhead()) {
                EAXCop *spokesperson;
                if (ai->GetLeader()->IsHeli()) {
                    spokesperson = ai->FindClosestCop(false, false);
                } else {
                    spokesperson = ai->GetLeader();
                }
                spokesperson->AnticipateFail();

            } else {
                ai->GetLeader()->StrategyReset(this->mFormationType != ai->GetPursuit()->GetFormationType());
            }
        } else if (!(this->mFlags & SOLO)) {
            ai->GetLeader()->AnticipateSuccess();
        }
    }
    this->ChangeStateTo(kWaiting);
}

void StrategyFlow::MessageReqBackup(const MReqBackup &message) {
    this->mBackupType = message.GetBackupType();
    this->mFlags &= ~BUDENIED;
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() == SoundAI::kActive && ai->GetFocus() == 2) {
        this->ChangeStateTo(kReqBackup);
    }
}

void StrategyFlow::MessageBackupDenied(const MReqBackup &message) {
    this->mBackupType = message.GetBackupType();
    this->mFlags |= BUDENIED;
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() == SoundAI::kActive && ai->GetFocus() == 2) {
        this->ChangeStateTo(kReqBackup);
    }
}

void StrategyFlow::MessageEventComplete(const MNotifySpeechStatus &message) {
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech != nullptr) {
        switch (speech->ID) {
            case kSPCH1_EventID_CallForBU: {
                Csis::Backup_CallForBUStruct *data = static_cast<Csis::Backup_CallForBUStruct *>(speech->GetData(nullptr));
                if (data != nullptr) {
                    this->mLastBackupType = data->backup_type;
                    this->mT_requested = WorldTimer;
                }
                break;
            }
            case kSPCH1_EventID_BUReminder: {
                Csis::Backup_BUReminderStruct *data = static_cast<Csis::Backup_BUReminderStruct *>(speech->GetData(nullptr));
                if (data != nullptr) {
                    this->mLastBackupType = data->backup_type;
                    this->mT_requested = WorldTimer;
                }
                break;
            }
            default:
                break;
        }
    }
}

}; // namespace Speech
