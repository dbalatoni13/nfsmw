#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/PursuitFlow.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

namespace Speech {

PursuitFlow::PursuitFlow()
    : mCauseofPursuit(kUnknown), //
      mFirstOnScene(nullptr),    //
      mReqRestart(false),        //
      mAVSUnitRammedSaid(false), //
      mSpeaker(0),               //
      mMsgNotifyEventCompletion(Hermes::Handler::Create<MNotifySpeechStatus, PursuitFlow, PursuitFlow>(this, &PursuitFlow::MessageEventComplete,
                                                                                                       UCrc32(UCRC32_Gameplay), 0)) {}

PursuitFlow::~PursuitFlow() {
    if (this->mMsgNotifyEventCompletion != nullptr) {
        Hermes::Handler::Destroy(this->mMsgNotifyEventCompletion);
    }
}

void PursuitFlow::OnCopRemoved(EAXCop *cop) {
    if (this->mFirstOnScene != nullptr && (this->mFirstOnScene->GetHandle() == cop->GetHandle())) {
        this->mFirstOnScene = nullptr;
    }
}

void PursuitFlow::Update() {
    int y;
    int x;

    if (SPEECHFLOW_DISPLAY) {
        switch (this->mCauseofPursuit) {
            case k911Reported:
                x = 0;
                break;
            case kCopAssaulted:
                x = 1;
                break;
            case kSpotted:
                x = 2;
                break;
            case kReacquired:
                x = 3;
                break;
            case kScripted:
                x = 4;
                break;
            default:
                x = 5;
                break;
        }

        SoundAI *ai = SoundAI::Get();
        if (ai != nullptr && ai->GetPursuitState() == SoundAI::kActive) {
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
        case kCloseInCheck:
            this->CloseInCheck();
            return;
        case kPrimaryBranch:
            this->PrimaryBranch();
            return;
        case kSpotterBranch:
            this->SpotterBranch();
            return;
        case kScriptedBranch:
            this->ScriptedBranch();
            return;
        case kWaitForSpotter:
            this->SpotterWait();
            return;
        case kLostWhileSpotWait:
            this->LostWhileSpotterWait();
            return;
        case kPlayerStopped:
            this->PlayerStopped();
            return;
        case kBailout:
            this->Bailout();
            return;
        case kChangeTarget:
            this->ChangeTarget();
            return;
        case kTerminal:
            this->Terminal();
            return;
        default:
            this->ChangeStateTo(kCullCheck);
            return;
    }
}

void PursuitFlow::CullCheck() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() == SoundAI::kActive) {
        this->ChangeStateTo(kCloseInCheck);
    }
}

void PursuitFlow::Reset() {
    this->mFirstOnScene = nullptr;
    this->SpeechFlow::Reset();
    this->mState = kCullCheck;
    this->mBusy = 0;
    this->mCauseofPursuit = kUnknown;
}

void PursuitFlow::Reacquire() {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr && !ai->Is911Active()) {
        this->mCauseofPursuit = kReacquired;
        this->ChangeStateTo(kPrimaryBranch);
    }
}

bool PursuitFlow::RequiresRestart() {
    if (Manager::IsCopSpeechPlaying(kSPCH1_EventID_AttmptVehStp)) {
        Module *cop_speech = Manager::GetSpeechModule(1);
        if (cop_speech != nullptr) {
            GameSpeech *gamespeech = static_cast<GameSpeech *>(cop_speech);
            ScheduledSpeechEvent *avs = gamespeech->GetCurrentEvent();
            if (avs != nullptr) {
                Csis::Setup_AttmptVehStpStruct *data = static_cast<Csis::Setup_AttmptVehStpStruct *>(avs->GetData(nullptr));
                if (data != nullptr && data->pursuit_type == Csis::Type_pursuit_type_Unit_Rammed) {
                    this->mCauseofPursuit = kCopAssaulted;
                    this->mAVSUnitRammedSaid = true;
                }
            }
        }
    }

    if (this->mAVSUnitRammedSaid) {
        return false;
    }

    this->mReqRestart = true;
    this->mBusy = 0;
    return true;
}

void PursuitFlow::CloseInCheck() {
    SoundAI *ai = SoundAI::Get();
    const copMap &cops = ai->GetActors();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (cops.size() < 1)) {
        this->Reset();
        return;
    }

    IPursuit *pursuit = ai->GetPursuit();
    if (pursuit == nullptr) {
        return;
    }

    if (ai->GetPursuitDuration() < ai->GetTune().PursuitInitDelay()) {
        return;
    }

    BlowByRecord &recent = ai->GetRecentBlowby();
    float lastblowby = (WorldTimer - recent.timestamp).GetSeconds();
    if ((recent.distance < ai->GetTune().RangeForSpotterBranch()) && (recent.speed >= ai->GetTune().SpeedDiffForBlowby()) &&
        (lastblowby < ai->GetTune().BlowbyInterval()) && (ai->GetPlayerSpeed() >= 35.0f) && !ai->AreCopsAhead()) {
        this->mCauseofPursuit = kSpotted;
    }

    if (ai->Is911Active()) {
        this->mCauseofPursuit = k911Reported;
    }

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }
    if (pursuitRace) {
        this->mCauseofPursuit = kScripted;
    }

    bool officer_assaulted = ai->GetTimeLastNailedCop() < 10.0f;
    if (officer_assaulted && (this->mCauseofPursuit != kScripted)) {
        this->mCauseofPursuit = kCopAssaulted;
    }

    switch (this->mCauseofPursuit) {
        case kSpotted:
            this->ChangeStateTo(kSpotterBranch);
            break;
        case kCopAssaulted:
        case kCopAssaultedScripted:
        case kUnknown:
            this->ChangeStateTo(kPrimaryBranch);
            break;
        case k911Reported:
            this->ChangeStateTo(kWaitForSpotter);
            break;
        case kScripted:
        default:
            this->ChangeStateTo(kScriptedBranch);
            break;
    }
}

void PursuitFlow::PrimaryBranch() {
    SoundAI *ai = SoundAI::Get();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kBailout);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        this->ChangeStateTo(kChangeTarget);
        return;
    }

    if (!this->mBusy) {
        EAXCop *leader = ai->FindClosestCop(false, false);
        if (leader == nullptr) {
            this->ChangeStateTo(kCullCheck);
            return;
        }

        if (!leader->IsPrimary()) {
            bool converted = ai->MakeLeader(leader);
            if (!converted) {
                this->ChangeStateTo(kCullCheck);
                return;
            }
        }

        if (leader->IsHeli()) {
            this->ChangeStateTo(kCullCheck);
            return;
        }

        if (this->mCauseofPursuit == kReacquired) {
            leader->ReinitiatePursuit();
        } else {
            if (this->mReqRestart) {
                if (this->mSpeaker > 0) {
                    if (leader->GetSpeakerID() != this->mSpeaker) {
                        EAXCop *orig_spkr = ai->GetCop(this->mSpeaker);
                        if (orig_spkr != nullptr) {
                            leader->SwapVoices(orig_spkr);
                        }
                    }
                }
                this->mReqRestart = false;
            }

            leader->AttemptVehicleStop();
            this->mSpeaker = leader->GetSpeakerID();

            if (ai->GetLeader() == nullptr) {
                ai->MakeLeader(leader);
            }

            if (ai->GetDispatch() != nullptr) {
                ai->GetDispatch()->GoAhead();
            }

            if ((this->mCauseofPursuit != kCopAssaulted) && (this->mCauseofPursuit != kCopAssaultedScripted)) {
                leader->VehicleReport();
            }

            if (this->mCauseofPursuit == kCopAssaultedScripted) {
                this->ChangeStateTo(kTerminal);
                return;
            }

            if (ai->NumPursuits() > 1) {
                if (ai->NumPursuits() > 2) {
                    EAXCop *rp = ai->GetRandomActiveCop(1, false);
                    if (rp != nullptr) {
                        if (rp->GetHandle() != leader->GetHandle()) {
                            rp->DriverHistory();
                        } else {
                            ai->GetDispatch()->DriverHistory();
                        }
                    }
                } else {
                    leader->SuspectConfirmed();
                }
                this->mCauseofPursuit = kReacquired;
            }
        }

        this->mBusy++;
        return;
    }

    if (ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) {
        if ((this->mCauseofPursuit != kCopAssaulted) && (this->mCauseofPursuit != kCopAssaultedScripted)) {
            this->mBusy = 0;
            this->ChangeStateTo(kPlayerStopped);
            return;
        }
    }

    if (!Manager::IsCopSpeechBusy()) {
        if (!this->mBusy) {
            return;
        }
        if (this->mReqRestart) {
            return;
        }
        this->ChangeStateTo(kTerminal);
        this->mBusy = 0;
    }
}

void PursuitFlow::PlayerStopped() {
    SoundAI *ai = SoundAI::Get();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kBailout);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        this->ChangeStateTo(kChangeTarget);
        return;
    }

    if (!this->mBusy) {
        if ((Manager::HasBeenSaid(kSPCH1_EventID_AttmptVehStp) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_AttmptVehStp) ||
             Manager::HasBeenSaid(kSPCH1_EventID_Spotter) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_Spotter) ||
             Manager::HasBeenSaid(kSPCH1_EventID_SpotterReply) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_SpotterReply) ||
             Manager::HasBeenSaid(kSPCH1_EventID_DispGoAhead) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_DispGoAhead)) &&
            ((1.0f < ai->GetPlayerStopTime()) || (ai->GetPlayerSpeed() < 10.0f)) && (ai->GetLeader() != nullptr)) {
            ai->GetLeader()->InterruptComposedLow();
            this->mBusy++;
        }
    } else if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && ai->GetLeader() != nullptr) {
        ai->GetLeader()->InterruptComposedLow();
        this->ChangeStateTo(kTerminal);
        this->mBusy = 0;
        return;
    }

    if (this->mBusy) {
        return;
    }

    this->ChangeStateTo(kTransition);
    this->mBusy = 0;
}

void PursuitFlow::SpotterBranch() {
    SoundAI *ai = SoundAI::Get();

    if (ai->GetPursuitState() == SoundAI::kActive) {
    } else if (ai->GetPursuitState() == SoundAI::kSearching) {
    } else if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kBailout);
        return;
    } else if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        this->ChangeStateTo(kChangeTarget);
        return;
    } else {
        return;
    }

    if (!this->mBusy) {
        if (ai->IsHighIntensity() || (ai->GetLastInfraction() == 0x40)) {
            EAXCop *cop = ai->FindClosestCop(false, true);
            if (cop == nullptr) {
                return;
            }
            this->mFirstOnScene = cop;
            cop->SpotterWanted();
        } else {
            EAXCop *cop = ai->FindClosestCop(false, false);
            if (cop == nullptr) {
                return;
            }
            this->mFirstOnScene = cop;
            cop->Spotter();
        }

        if (bRandom(1.0f) > 0.5f) {
            this->mFirstOnScene->VehicleReport();
        }

        if (bRandom(1.0f) > 0.5f) {
            ai->GetDispatch()->GoAhead();
        } else {
            ai->GetDispatch()->Ack();
        }

        EAXCop *anothercop = ai->FindClosestCop(false, true);
        if (this->mFirstOnScene != nullptr) {
            if (anothercop != nullptr && anothercop->GetSpeakerID() != this->mFirstOnScene->GetSpeakerID()) {
                anothercop->Reply911();
            }
        } else {
            if (anothercop != nullptr) {
                anothercop->Reply911();
            }
        }

        this->mBusy++;
        return;
    }

    if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && (this->mCauseofPursuit != kCopAssaulted) &&
        (this->mCauseofPursuit != kCopAssaultedScripted)) {
        this->mBusy = 0;
        this->ChangeStateTo(kPlayerStopped);
        return;
    }

    if (!Manager::IsCopSpeechBusy() && this->mBusy) {
        if (ai->NumCopsWithLOS() > 0) {
            this->ChangeStateTo(kWaitForSpotter);
        } else {
            if (this->mFirstOnScene != nullptr) {
                this->mFirstOnScene->LostVisual();
            }
            this->ChangeStateTo(kLostWhileSpotWait);
        }
        this->mBusy = 0;
    }
}

void PursuitFlow::ScriptedBranch() {
    SoundAI *ai = SoundAI::Get();
    if (!this->mBusy) {
        if (ai->IsHeadingValid()) {
            ai->GetDispatch()->PursuitEscalation();
        } else {
            ai->GetDispatch()->PursuitEscalationGeneric();
        }
        if ((ai->GetPlayerCarColor() > 0) && MiscSpeech::IsVehicleTypeOK()) {
            ai->GetDispatch()->VehicleDescription();
        }
        this->mBusy++;
    } else {
        if (!Manager::IsCopSpeechBusy() && this->mBusy) {
            this->ChangeStateTo(kTerminal);
            this->mBusy = 0;
        }
    }
}

void PursuitFlow::LostWhileSpotterWait() {
    SoundAI *ai = SoundAI::Get();
    if (!this->mBusy) {
        if (ai->GetPursuitState() == SoundAI::kInactive) {
            this->ChangeStateTo(kBailout);
            return;
        }
        if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
            this->ChangeStateTo(kChangeTarget);
            return;
        }
        if (ai->NumCopsWithLOS() > 0) {
            this->ChangeStateTo(kWaitForSpotter);
            this->mBusy = 0;
            return;
        }
    }

    if (Manager::IsCopSpeechBusy()) {
        return;
    }

    if (this->mBusy) {
        this->Reset();
        this->ChangeStateTo(kCullCheck);
        this->mBusy = 0;
    }
}

void PursuitFlow::SpotterWait() {
    SoundAI *ai = SoundAI::Get();

    if (!this->mBusy) {
        IPursuit *pursuit = ai->GetPursuit();
        if (ai->GetPursuitState() != SoundAI::kActive) {
            this->ChangeStateTo(kLostWhileSpotWait);
            return;
        }

        EAXCop *cansee = ai->FindClosestCop(true, true);
        if (cansee != nullptr && ai->MakeLeader(cansee) && (this->mCauseofPursuit != kScripted)) {
            if ((this->mCauseofPursuit == k911Reported) ||
                (this->mFirstOnScene != nullptr && (this->mFirstOnScene->GetSpeakerID() != cansee->GetSpeakerID()))) {
                if (ai->AreCopsAhead() || !cansee->IsPrimary()) {
                    cansee->Spotted();
                } else {
                    cansee->SpotterReply();
                }
            }

            if (this->mCauseofPursuit != k911Reported) {
                if ((bRandom(1.0f) > 0.5f) && ai->IsHeadingValid()) {
                    ai->GetDispatch()->PursuitEscalation();
                } else {
                    ai->GetDispatch()->PursuitEscalationGeneric();
                }
            }
        }

        this->mBusy++;
        return;
    }

    if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && (this->mCauseofPursuit != kCopAssaulted) &&
        (this->mCauseofPursuit != kCopAssaultedScripted)) {
        this->mBusy = 0;
        this->ChangeStateTo(kPlayerStopped);
        return;
    }

    if (!Manager::IsCopSpeechBusy() && this->mBusy) {
        this->ChangeStateTo(kTerminal);
        this->mBusy = 0;
    }
}

void PursuitFlow::Bailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    EAXCop *cop = ai->GetRandomActiveCop(0, false);
    if (cop != nullptr) {
        cop->LoBailout();
    }

    this->ChangeStateTo(kTransition);
    this->Reset();
}

void PursuitFlow::ChangeTarget() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    EAXCop *closest = ai->FindClosestCop(false, true);
    if (closest != nullptr) {
        closest->FocusChange();
    }

    ai->GetDispatch()->PursuitEscalation();
    this->ChangeStateTo(kTransition);
    this->Reset();
}

void PursuitFlow::Terminal() {
    SoundAI *ai = SoundAI::Get();

    if (!this->mBusy) {
        EAXCop *leader = ai->GetLeader();
        if ((ai->GetPlayerSpeed() >= 60.0f) && leader != nullptr && (this->mCauseofPursuit != kSpotted) && (this->mCauseofPursuit != k911Reported) &&
            (this->mCauseofPursuit != kScripted) && (this->mCauseofPursuit != kReacquired) && (this->mCauseofPursuit != kCopAssaultedScripted) &&
            (this->mCauseofPursuit != kCopAssaulted)) {
            leader->InitiatePursuit();
        }
        if (!ai->IsHighIntensity() && leader != nullptr && (this->mCauseofPursuit != kCopAssaulted) &&
            (this->mCauseofPursuit != kCopAssaultedScripted)) {
            leader->LocationReport();
        }
        if ((ai->GetHavoc() >= ai->GetTune().MinHavocForSuspectBehavior()) && (this->mCauseofPursuit != kCopAssaulted) &&
            (this->mCauseofPursuit != kCopAssaultedScripted)) {
            EAXCop *cansee = ai->FindClosestCop(true, true);
            if (cansee != nullptr) {
                cansee->SuspectBehavior();
            }
        }
        this->mBusy++;
        return;
    }

    if (!Manager::IsCopSpeechBusy() && this->mBusy) {
        this->ChangeStateTo(kTransition);
        this->mBusy = 0;
        this->mFirstOnScene = nullptr;
    }
}

bool PursuitFlow::IsTransitionable() {
    return this->mState == kTransition;
}

void PursuitFlow::MessageEventComplete(const MNotifySpeechStatus &message) {
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech == nullptr) {
        return;
    }

    switch (speech->ID) {
        case kSPCH1_EventID_AttmptVehStp: {
            Csis::Setup_AttmptVehStpStruct *data = static_cast<Csis::Setup_AttmptVehStpStruct *>(speech->GetData(nullptr));
            if ((data != nullptr) && (data->pursuit_type == Csis::Type_pursuit_type_Unit_Rammed)) {
                this->mAVSUnitRammedSaid = true;
            }
            break;
        }
        case kSPCH1_EventID_ReInitPursuit: {
            SoundAI *ai = SoundAI::Get();
            if ((ai != nullptr) && (ai->NumPursuits() > 2)) {
                if ((bRandom(1.0f) > 0.5f) && ai->IsHeadingValid()) {
                    ai->GetDispatch()->PursuitEscalation();
                } else {
                    ai->GetDispatch()->PursuitEscalationGeneric();
                }
            }
            break;
        }
        default:
            break;
    }
}

}; // namespace Speech
