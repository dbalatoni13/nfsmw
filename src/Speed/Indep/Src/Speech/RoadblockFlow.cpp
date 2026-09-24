#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/RoadblockFlow.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

namespace Speech {

RoadblockFlow::RoadblockFlow()
    : mT_setup(0),           //
      mT_engaged(0),         //
      mT_averted(0),         //
      mT_reset(),            //
      mLoDist2RB(32767.0f),  //
      mFlags(0),             //
      mSpikeOffset(0),       //
      mPertinentRB(nullptr), //
      mNumBlocks(0),         //
      mMsgReqHeliJoinRB(
          Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(this, &RoadblockFlow::MessageReqHeliJoinRB, "ReqHeliJoin", 0)), //
      mMsgRoadBlockDodged(
          Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(this, &RoadblockFlow::MessageRoadBlockDodged, "Dodged", 0)), //
      mMsgPosition(Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(this, &RoadblockFlow::MessagePositionUpdate, "Position", 0)) {
    this->mState = kWaiting;
}

RoadblockFlow::~RoadblockFlow() {
    if (this->mMsgReqHeliJoinRB != nullptr) {
        Hermes::Handler::Destroy(this->mMsgReqHeliJoinRB);
    }
    if (this->mMsgRoadBlockDodged != nullptr) {
        Hermes::Handler::Destroy(this->mMsgRoadBlockDodged);
    }
    if (this->mMsgPosition != nullptr) {
        Hermes::Handler::Destroy(this->mMsgPosition);
    }
}

void RoadblockFlow::NailedSomethingInRB(unsigned int what) {
    if ((this->mFlags & (OUTCOMETIMERSET | AVERTED | ENGAGED)) == 0) {
        this->mT_engaged = WorldTimer;
        this->mFlags |= OUTCOMETIMERSET;
    }
    this->mFlags |= ENGAGED | what;
}

void RoadblockFlow::MessageRoadBlockDodged(const MReqRoadBlock &message) {
    if ((this->mFlags & (OUTCOMETIMERSET | AVERTED | ENGAGED)) == 0) {
        this->mT_averted = WorldTimer;
        this->mFlags |= OUTCOMETIMERSET;
    }
    this->mFlags |= AVERTED;
}

void RoadblockFlow::SyncRoadblock() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    UMath::Vector3 pPos = UMath::Vector3::kZero;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IRoadBlock *rb = ai->GetRoadblock();

    if ((this->mFlags & RESET_PENDING) != 0) {
        float t_since_outcome = (WorldTimer - this->mT_reset).GetSeconds();
        if (this->mPertinentRB != rb && t_since_outcome >= ai->GetTune().RBPostOutcomeResetTime()) {
            this->mPertinentRB = rb;
            this->mFlags &= ~RESET_PENDING;
            if ((this->mFlags & SETUP) != 0) {
                this->mFlags |= REQ_SERVICE;
            }
            this->mFlags &= ~(SETUP | LOS | HELIJOINED | AVERTED | NAILED_SPIKES | NAILED_OTHEROBJ | NAILED_VEHICLE | ENGAGED | POSITIONED);
            ++this->mNumBlocks;
        }
    }

    if (rb != nullptr && player != nullptr && (this->mFlags & RESET_PENDING) == 0) {
        IPursuit *rbAI = rb->GetPursuit();
        pPos = player->GetPosition();
        this->mLoDist2RB = UMath::Distance(ai->GetRoadblock()->GetRoadBlockCentre(), pPos);

        if (!(this->mFlags & SETUP)) {
            this->mFlags |= SETUP;
            this->mFlags |= REQ_SERVICE;
            this->mT_setup = WorldTimer;
        }

        bool visual = false;
        const IRoadBlock::Vehicles &rb_cars = rb->GetVehicles();
        if (rb_cars.size() != 0) {
            for (IRoadBlock::Vehicles::const_iterator i = rb_cars.begin(); i != rb_cars.end(); ++i) {
                IPursuitAI *car_ai;
                IRenderable *renderable;
                IVehicle *car = *i;
                if (car->QueryInterface(&car_ai)) {
                    car->QueryInterface(&renderable);
                    if (car_ai->GetTimeSinceTargetSeen() <= 0.0f || (renderable != nullptr && renderable->InView())) {
                        visual = true;
                        break;
                    }
                }
            }
        }

        if (visual && (this->mFlags & LOS) == 0) {
            this->mFlags |= LOS;
            this->mFlags |= REQ_SERVICE;
            MMiscSound(1).Send(UCrc32("RBLOS"));
        } else {
            this->mFlags &= ~LOS;
        }
    }

    if (((this->mFlags & RB_ENABLED) == 0 && ai->RoadblocksEnabled()) || ((this->mFlags & HELIRB_ENABLED) == 0 && ai->HeliRoadblocksEnabled()) ||
        ((this->mFlags & SPIKES_ENABLED) == 0 && ai->SpikesEnabled()) || ((this->mFlags & RB_ENABLED) != 0 && !ai->RoadblocksEnabled()) ||
        ((this->mFlags & HELIRB_ENABLED) != 0 && !ai->HeliRoadblocksEnabled()) || ((this->mFlags & SPIKES_ENABLED) != 0 && !ai->SpikesEnabled())) {
        this->Request();
    }
}

void RoadblockFlow::Update() {
    this->SyncRoadblock();
    int y;
    int x;
    if (SPEECHFLOW_DISPLAY) {
        unsigned int color;
        if ((this->mFlags & 3) == 1) {
            color = 0xFFFFFF00;
        } else if ((this->mFlags & 3) == 3) {
            color = 0xFFFF3F3F;
        } else {
            color = 0x5FFF3F3F;
        }
    }
    this->Service();
}

void RoadblockFlow::MessageReqHeliJoinRB(const MReqRoadBlock &message) {
    SoundAI *ai = SoundAI::Get();
    this->mFlags |= HELIJOINED;
    if (ai->GetHeli() != nullptr && ai->GetPursuitState() == SoundAI::kActive) {
        ai->GetHeli()->JoinRB();
    }
}

void RoadblockFlow::MessagePositionUpdate(const MReqRoadBlock &message) {
    this->mFlags |= POSITIONED;
    this->mFlags |= REQ_SERVICE;
    this->mSpikeOffset = message.GetData();
}

void RoadblockFlow::MessageEventComplete(const MNotifySpeechStatus &) {
    this->mFlags &= ~REQ_SERVICE;
}

void RoadblockFlow::Request() {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        if (!ai->RoadblocksEnabled() && (this->mFlags & (REQUESTED | RB_ENABLED)) == 0) {
            if (Manager::GetHistory().GetCount(kSPCH1_EventID_CallForRB) == 0 && !Manager::IsQueued(kSPCH1_EventID_CallForRB, 4) &&
                !Manager::IsCopSpeechPlaying(kSPCH1_EventID_CallForRB) && ai->GetLeader() != nullptr) {
                ai->GetLeader()->CallForRB();
                ai->GetDispatch()->RBReply(ai->GetLeader(), 0, 0);
                ai->GetLeader()->NegRBReply();
            }
        } else {
            if (((ai->RoadblocksEnabled() && (this->mFlags & RB_ENABLED) == 0) ||
                 (ai->SpikesEnabled() && (this->mFlags & (RB_ENABLED | SPIKES_ENABLED | CALLED_4_SPIKES)) == RB_ENABLED)) &&
                ai->GetLeader() != nullptr) {
                if (Manager::GetHistory().GetCount(kSPCH1_EventID_CallForRB) == 0) {
                    ai->GetLeader()->CallForRB();
                } else {
                    ai->GetLeader()->RBReminder();
                }
                ai->GetDispatch()->RBReply(ai->GetLeader(), 1, 0);
                this->mFlags |= RB_ENABLED;
                if (ai->SpikesEnabled()) {
                    this->mFlags |= SPIKES_ENABLED;
                }
            }

            if (ai->RoadblocksEnabled()) {
                if ((this->mFlags & RB_ENABLED) != 0 && ai->HeliRoadblocksEnabled() && (this->mFlags & HELIRB_ENABLED) == 0 &&
                    (this->mFlags & (CALLED_4_SPIKES | CALLED_4_NORMAL)) != 0 && (this->mFlags & REQUESTED) != 0 && ai->GetLeader() != nullptr) {
                    ai->GetLeader()->CallForBackup(8);
                    ai->GetLeader();
                    ai->GetDispatch()->RBReply(ai->GetLeader(), 0, 4);
                    this->mFlags |= HELIRB_ENABLED;
                }
            }
        }
    }
}

void RoadblockFlow::Setup() {
    SoundAI *ai = SoundAI::Get();
    if (!(this->mFlags & SETUP)) {
        EAXCop *primary = ai->GetRandomCop(1);
        EAXCop *secondary = ai->GetRandomCop(2);

        if (this->mNumBlocks >= 2 && secondary != nullptr) {
            if (bRandom(1.0f) > 0.5f) {
                secondary->CallForSubRB();
            } else {
                ai->GetDispatch()->SubRBReply();
            }
        } else {
            ai->GetDispatch()->RBUpdate(primary, 0);
            if (!primary->IsHeli()) {
                primary->NegRBReply();
            }
        }
        this->mFlags &= ~REQ_SERVICE;
    } else {
        if ((this->mFlags & POSITIONED) != 0) {
            bool pos_comment = true;
            if (ai->GetRoadblock() != nullptr) {
                pos_comment = ai->GetRoadblock()->GetNumSpikeStrips() <= 1;
            }

            if (pos_comment) {
                MiscSpeech::RBPosition(this->mSpikeOffset);
            } else {
                EAXCop *cop_in_rb = ai->GetCopInRB();
                if (cop_in_rb == nullptr) {
                    cop_in_rb = ai->GetRandomCop(2);
                }
                if (cop_in_rb != nullptr) {
                    cop_in_rb->RBApproach();
                } else {
                    MiscSpeech::RBWarning();
                }
            }
            this->mFlags &= ~REQ_SERVICE;
        } else {
            bool should_interrupt_dispatch = false;
            if (Manager::IsCopSpeechPlaying(kSPCH1_EventID_DispRBReply) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_DispRBUpdate)) {
                should_interrupt_dispatch = true;
            }
            if (bRandom(1.0f) > 0.5f && !should_interrupt_dispatch) {
                EAXCop *primary = ai->GetRandomActiveCop(1, false);
                if (primary == nullptr) {
                    return;
                }
                ai->GetDispatch()->RBUpdate(primary, 1);
                primary->PursuitApproaching();
                this->mFlags &= ~REQ_SERVICE;
            } else {
                MiscSpeech::RBWarning();
                this->mFlags &= ~REQ_SERVICE;
            }
        }
    }
}

void RoadblockFlow::Approach() {
    SoundAI *ai = SoundAI::Get();
    EAXCop *cop_LOS = ai->GetRandomCop(2);
    if (cop_LOS != nullptr) {
        cop_LOS->RBApproach();
        this->mFlags &= ~REQ_SERVICE;
    }
}

void RoadblockFlow::Effect() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    if ((this->mFlags & OUTCOMETIMERSET) == 0) {
        return;
    }

    float t_engaged = (WorldTimer - this->mT_engaged).GetSeconds();
    float t_averted = (WorldTimer - this->mT_averted).GetSeconds();

    if ((this->mFlags & (AVERTED | ENGAGED)) == (AVERTED | ENGAGED)) {
        if (t_averted >= ai->GetTune().RBOutcomeTimer() && t_engaged >= ai->GetTune().RBOutcomeTimer()) {
            this->mFlags &= ~OUTCOMETIMERSET;
        }
    }

    if ((this->mFlags & (AVERTED | ENGAGED)) == AVERTED) {
        if (t_averted >= ai->GetTune().RBOutcomeTimer()) {
            this->mFlags &= ~OUTCOMETIMERSET;
        }
    }

    if ((this->mFlags & (AVERTED | ENGAGED)) == ENGAGED && t_engaged >= ai->GetTune().RBOutcomeTimer() && (this->mFlags & LOS) != 0) {
        EAXCop *cop = ai->GetRandomActiveCop(0, true);
        if (cop != nullptr) {
            cop->Arrest();
        }
    }

    if ((this->mFlags & OUTCOMETIMERSET) != 0) {
        return;
    }

    IRoadBlock *block = ai->GetRoadblock();
    if (block != nullptr) {
        EAXCop *secondary = ai->GetRandomCop(2);
        EAXCop *randcop = ai->GetRandomCop(0);

        if (block->GetNumCopsDamaged() > 0 || block->GetNumCopsDestroyed() > 0 || (this->mFlags & ENGAGED) != 0) {
            if (secondary != nullptr && bRandom(1.0f) > 0.5f) {
                secondary->RBEngage((this->mFlags & NAILED_SPIKES) != 0);
            } else {
                MiscSpeech::RBEngaged((this->mFlags & NAILED_SPIKES) != 0);
            }
        } else {
            bool disp_response = false;
            if (block->GetNumSpikeStrips() > 0) {
                MiscSpeech::RBAverted();
            } else if (bRandom(1.0f) > 0.5f) {
                disp_response = true;
                ai->GetDispatch()->SubRBReply();
            } else if (secondary != nullptr && bRandom(1.0f) > 0.5f) {
                secondary->RBAverted();
            } else {
                MiscSpeech::RBAverted();
            }

            if (!disp_response && randcop != nullptr && randcop->IsPrimary()) {
                randcop->CallForSubRB();
            } else {
                ai->GetDispatch()->SubRBReply();
            }
        }

        this->mFlags &= ~REQ_SERVICE;
    }

    if ((this->mFlags & (AVERTED | ENGAGED)) != 0 && (this->mFlags & REQ_SERVICE) == 0) {
        this->mFlags |= RESET_PENDING;
        this->mT_reset = WorldTimer;
    }
}

void RoadblockFlow::Service() {
    SoundAI *ai = SoundAI::Get();
    if ((ai->GetFocus() != SoundAI::kStrategyFlow) && (ai->GetFocus() != SoundAI::kLost)) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (((this->mFlags & REQUESTED) == 0) &&
        ((Manager::GetHistory().GetCount(kSPCH1_EventID_InitStrategy) > 0) || (Manager::GetHistory().GetCount(kSPCH1_EventID_SelfStrategy) > 0))) {
        this->Request();
    }

    if ((this->mFlags & REQ_SERVICE) != 0) {
        if ((this->mFlags & (AVERTED | ENGAGED)) == 0) {
            if ((this->mFlags & LOS) != 0) {
                this->Approach();
            } else if ((this->mFlags & SETUP) != 0) {
                this->Setup();
            }
        }
    }

    if ((this->mFlags & OUTCOMETIMERSET) != 0) {
        this->Effect();
    }
}

void RoadblockFlow::Terminal() {
    this->mBusy = 0;
    this->mFlags = 0;
    this->mSpikeOffset = 0;
    this->mPertinentRB = nullptr;
    this->mLoDist2RB = 32767.0f;
}

void RoadblockFlow::Reset() {
    this->Terminal();
    this->SpeechFlow::Reset();
}

bool RoadblockFlow::IsTransitionable() {
    return true;
}

}; // namespace Speech
