#include "PursuitFlow.h"
#include "SoundAI.h"
#include "EAXDispatch.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"

extern int SPEECHFLOW_DISPLAY;

namespace Speech {

PursuitFlow::PursuitFlow()
    : mCauseofPursuit(kUnknown), //
      mFirstOnScene(0), //
      mReqRestart(false), //
      mAVSUnitRammedSaid(false), //
      mSpeaker(0), //
      mMsgNotifyEventCompletion(0) {
    mMsgNotifyEventCompletion = Hermes::Handler::Create<MNotifySpeechStatus, PursuitFlow, PursuitFlow>(
        this, &PursuitFlow::MessageEventComplete, UCrc32(0x20d60dbf), 0);
}

PursuitFlow::~PursuitFlow() {
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
        mMsgNotifyEventCompletion = 0;
    }
}

void PursuitFlow::OnCopRemoved(EAXCop *cop) {
    if (mFirstOnScene && cop && (mFirstOnScene->GetSpeakerID() == cop->GetSpeakerID())) {
        mFirstOnScene = 0;
    }
}

void PursuitFlow::Update() {
    int y;
    int x;

    if (SPEECHFLOW_DISPLAY != 0) {
        switch (mCauseofPursuit) {
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

        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if ((ai != nullptr) && (ai->GetPursuitState() == SoundAI::kActive)) {
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
    const copMap &cops = ai->GetActors();

    if ((ai->GetPursuitState() == SoundAI::kInactive) || (cops.size() == 0)) {
        Reset();
        return;
    }

    IPursuit *pursuit = ai->GetPursuit();
    if (!pursuit) {
        return;
    }

    if (ai->GetPursuitDuration() < ai->GetTune().PursuitInitDelay()) {
        return;
    }

    SoundAI::BlowByRecord &recent = ai->GetRecentBlowby();
    float lastblowby = (WorldTimer - recent.timestamp).GetSeconds();
    if ((recent.distance < ai->GetTune().RangeForSpotterBranch()) && (recent.speed >= ai->GetTune().SpeedDiffForBlowby()) &&
        (lastblowby < ai->GetTune().BlowbyInterval()) && (35.0f <= ai->GetPlayerSpeed()) && !ai->AreCopsAhead()) {
        mCauseofPursuit = kSpotted;
    }

    if (ai->Is911Active()) {
        mCauseofPursuit = k911Reported;
    }

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters()) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }
    if (pursuitRace) {
        mCauseofPursuit = kScripted;
    }

    bool officer_assaulted = ai->GetTimeLastNailedCop() < 10.0f;
    if (officer_assaulted && (mCauseofPursuit != kScripted)) {
        mCauseofPursuit = kCopAssaulted;
    }

    switch (mCauseofPursuit) {
    case kSpotted:
        ChangeStateTo(kSpotterBranch);
        break;
    case k911Reported:
        ChangeStateTo(kWaitForSpotter);
        break;
    case kCopAssaulted:
    case kCopAssaultedScripted:
    case kUnknown:
        ChangeStateTo(kPrimaryBranch);
        break;
    default:
        ChangeStateTo(kScriptedBranch);
        break;
    }
}

void PursuitFlow::PrimaryBranch() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kBailout);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kChangeTarget);
        return;
    }

    if (!mBusy) {
        EAXCop *newprim = ai->FindClosestCop(false, false);
        if (!newprim) {
            ChangeStateTo(kCullCheck);
            return;
        }

        if (!newprim->IsPrimary() && !ai->MakeLeader(newprim)) {
            ChangeStateTo(kCullCheck);
            return;
        }

        if (newprim->IsHeli()) {
            ChangeStateTo(kCullCheck);
            return;
        }

        if (mCauseofPursuit == kReacquired) {
            newprim->ReinitiatePursuit();
        } else {
            if (mReqRestart) {
                if (mSpeaker > 0) {
                    if (newprim->GetSpeakerID() != mSpeaker) {
                        EAXCop *oldprim = ai->GetCop(mSpeaker);
                        if (oldprim) {
                            newprim->SwapVoices(oldprim);
                        }
                    }
                }
                mReqRestart = false;
            }

            newprim->AttemptVehicleStop();
            mSpeaker = newprim->GetSpeakerID();

            if (!ai->GetLeader()) {
                ai->MakeLeader(newprim);
            }

            if (ai->GetDispatch()) {
                ai->GetDispatch()->GoAhead();
            }

            if ((mCauseofPursuit != kCopAssaulted) && (mCauseofPursuit != kCopAssaultedScripted)) {
                newprim->VehicleReport();
            }

            if (mCauseofPursuit == kCopAssaultedScripted) {
                ChangeStateTo(kTerminal);
                return;
            }

            if (ai->NumPursuits() > 1) {
                if (ai->NumPursuits() > 2) {
                    EAXCop *rp = ai->GetRandomActiveCop(1, false);
                    if (rp) {
                        if (rp->GetHandle() != newprim->GetHandle()) {
                            rp->DriverHistory();
                        } else {
                            ai->GetDispatch()->DriverHistory();
                        }
                    }
                } else {
                    newprim->SuspectConfirmed();
                }
                mCauseofPursuit = kReacquired;
            }
        }

        mBusy++;
        return;
    }

    if (ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) {
        if ((mCauseofPursuit != kCopAssaulted) && (mCauseofPursuit != kCopAssaultedScripted)) {
            mBusy = 0;
            ChangeStateTo(kPlayerStopped);
            return;
        }
    }

    if (!Manager::IsCopSpeechBusy()) {
        if (!mBusy) {
            return;
        }
        if (mReqRestart) {
            return;
        }
        ChangeStateTo(kTerminal);
        mBusy = 0;
    }
}

void PursuitFlow::PlayerStopped() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kBailout);
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kOtherTarget) {
        ChangeStateTo(kChangeTarget);
        return;
    }

    if (!mBusy) {
        if ((Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0x9a)) ||
             Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x9a)) ||
             Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0x3a)) ||
             Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x3a)) ||
             Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0x3b)) ||
             Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x3b)) ||
             Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0xae)) ||
             Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0xae))) &&
            ((1.0f < ai->GetPlayerStopTime()) || (ai->GetPlayerSpeed() < 10.0f)) && ai->GetLeader()) {
            ai->GetLeader()->AttemptVehicleStop();
            mBusy++;
        }
    } else if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && ai->GetLeader()) {
        ai->GetLeader()->AttemptVehicleStop();
        ChangeStateTo(kTerminal);
        mBusy = 0;
        return;
    }

    if (mBusy) {
        return;
    }

    ChangeStateTo(kTransition);
    mBusy = 0;
}

void PursuitFlow::SpotterBranch() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    SoundAI::PursuitState pstate = ai->GetPursuitState();
    if (pstate == SoundAI::kActive) {
    } else if (pstate == SoundAI::kSearching) {
    } else if (pstate == SoundAI::kInactive) {
        ChangeStateTo(kBailout);
        return;
    } else if (pstate == SoundAI::kOtherTarget) {
        ChangeStateTo(kChangeTarget);
        return;
    } else {
        return;
    }

    if (!mBusy) {
        if (ai->IsHighIntensity() || (ai->GetLastInfraction() == 0x40)) {
            EAXCop *cop = ai->FindClosestCop(false, true);
            if (!cop) {
                return;
            }
            mFirstOnScene = cop;
            cop->SpotterWanted();
        } else {
            EAXCop *cop = ai->FindClosestCop(false, false);
            if (!cop) {
                return;
            }
            mFirstOnScene = cop;
            cop->Spotter();
        }

        if (bRandom(1.0f) > 0.5f) {
            mFirstOnScene->VehicleReport();
        }

        if (bRandom(1.0f) > 0.5f) {
            ai->GetDispatch()->GoAhead();
        } else {
            ai->GetDispatch()->Ack();
        }

        EAXCop *anothercop = ai->FindClosestCop(false, true);
        if (mFirstOnScene) {
            if (anothercop && anothercop->GetSpeakerID() != mFirstOnScene->GetSpeakerID()) {
                anothercop->Reply911();
            }
        } else {
            if (anothercop) {
                anothercop->Reply911();
            }
        }

        mBusy++;
        return;
    }

    if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && (mCauseofPursuit != kCopAssaulted) &&
        (mCauseofPursuit != kCopAssaultedScripted)) {
        mBusy = 0;
        ChangeStateTo(kPlayerStopped);
        return;
    }

    if (!Manager::IsCopSpeechBusy() && mBusy) {
        if (ai->NumCopsWithLOS() > 0) {
            ChangeStateTo(kWaitForSpotter);
        } else {
            if (mFirstOnScene) {
                mFirstOnScene->LostVisual();
            }
            ChangeStateTo(kLostWhileSpotWait);
        }
        mBusy = 0;
    }
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

    if (!mBusy) {
        if (ai->GetPursuitState() != SoundAI::kActive) {
            ChangeStateTo(kLostWhileSpotWait);
            return;
        }

        EAXCop *cansee = ai->FindClosestCop(true, true);
        if (cansee && ai->MakeLeader(cansee) && (mCauseofPursuit != kScripted)) {
            if (mCauseofPursuit == k911Reported) {
                if (ai->AreCopsAhead() || !cansee->IsPrimary()) {
                    cansee->Spotted();
                } else {
                    cansee->SpotterReply();
                }
            } else {
                if (mFirstOnScene) {
                    if (mFirstOnScene->GetSpeakerID() != cansee->GetSpeakerID()) {
                        if (ai->AreCopsAhead() || !cansee->IsPrimary()) {
                            cansee->Spotted();
                        } else {
                            cansee->SpotterReply();
                        }
                    }
                }
            }

            if (mCauseofPursuit != k911Reported) {
                if ((bRandom(1.0f) > 0.5f) && ai->IsHeadingValid()) {
                    ai->GetDispatch()->PursuitEscalation();
                } else {
                    ai->GetDispatch()->PursuitEscalationGeneric();
                }
            }
        }

        mBusy++;
        return;
    }

    if ((ai->GetPlayerStopTime() >= ai->GetTune().MinTimeConsideredStopped()) && (mCauseofPursuit != kCopAssaulted) &&
        (mCauseofPursuit != kCopAssaultedScripted)) {
        mBusy = 0;
        ChangeStateTo(kPlayerStopped);
        return;
    }

    if (!Manager::IsCopSpeechBusy() && mBusy) {
        ChangeStateTo(kTerminal);
        mBusy = 0;
    }
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
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech == 0) {
        return;
    }

    switch (speech->ID) {
    case kSPCH1_EventID_AttmptVehStp: {
        Csis::Setup_AttmptVehStpStruct *data = static_cast<Csis::Setup_AttmptVehStpStruct *>(speech->GetData(0));
        if ((data != 0) && (data->pursuit_type == Csis::Type_pursuit_type_Unit_Rammed)) {
            mAVSUnitRammedSaid = true;
        }
        break;
    }
    case kSPCH1_EventID_ReInitPursuit: {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if ((ai != 0) && (ai->NumPursuits() > 2)) {
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

} // namespace Speech
