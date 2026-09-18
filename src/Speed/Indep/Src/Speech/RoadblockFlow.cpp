#include "RoadblockFlow.h"
#include "EAXCop.h"
#include "EAXDispatch.h"
#include "MiscSpeech.h"
#include "SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

extern const int SPEECHFLOW_DISPLAY;

namespace Speech {

RoadblockFlow::RoadblockFlow()
    : mT_setup(0), //
      mT_engaged(0), //
      mT_averted(0), //
      mT_reset(0), //
      mLoDist2RB(32767.0f), //
      mFlags(0), //
      mSpikeOffset(0), //
      mPertinentRB(0), //
      mNumBlocks(0), //
      mMsgReqHeliJoinRB(Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(
          this, &RoadblockFlow::MessageReqHeliJoinRB, "ReqHeliJoin", 0)), //
      mMsgRoadBlockDodged(Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(
          this, &RoadblockFlow::MessageRoadBlockDodged, "Dodged", 0)), //
      mMsgPosition(Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(
          this, &RoadblockFlow::MessagePositionUpdate, "Position", 0)) {
    mState = kWaiting;
}

RoadblockFlow::~RoadblockFlow() {
    if (mMsgReqHeliJoinRB) {
        Hermes::Handler::Destroy(mMsgReqHeliJoinRB);
    }
    if (mMsgRoadBlockDodged) {
        Hermes::Handler::Destroy(mMsgRoadBlockDodged);
    }
    if (mMsgPosition) {
        Hermes::Handler::Destroy(mMsgPosition);
    }
}

void RoadblockFlow::NailedSomethingInRB(unsigned int what) {
    if ((mFlags & (OUTCOMETIMERSET | AVERTED | ENGAGED)) == 0) {
        mT_engaged = WorldTimer;
        mFlags |= OUTCOMETIMERSET;
    }
    mFlags |= ENGAGED | what;
}

void RoadblockFlow::MessageRoadBlockDodged(const MReqRoadBlock &message) {
    if ((mFlags & (OUTCOMETIMERSET | AVERTED | ENGAGED)) == 0) {
        mT_averted = WorldTimer;
        mFlags |= OUTCOMETIMERSET;
    }
    mFlags |= AVERTED;
}

void RoadblockFlow::SyncRoadblock() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == 0) {
        return;
    }

    UMath::Vector3 pPos = UMath::Vector3::kZero;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IRoadBlock *rb = ai->GetRoadblock();

    if ((mFlags & RESET_PENDING) != 0) {
        float t_since_outcome = (WorldTimer - mT_reset).GetSeconds();
        if (mPertinentRB != rb && t_since_outcome >= ai->GetTune().RBPostOutcomeResetTime()) {
            mPertinentRB = rb;
            mFlags &= ~RESET_PENDING;

            if ((mFlags & SETUP) != 0) {
                mFlags |= REQ_SERVICE;
            }
            mFlags &= ~(SETUP |
                        LOS |
                        HELIJOINED |
                        AVERTED |
                        NAILED_SPIKES |
                        NAILED_OTHEROBJ |
                        NAILED_VEHICLE |
                        ENGAGED |
                        POSITIONED);

            ++mNumBlocks;
        }
    }

    if (rb != 0 && player != 0 && (mFlags & RESET_PENDING) == 0) {
        IPursuit *rbAI = rb->GetPursuit();
        pPos = player->GetPosition();
        mLoDist2RB = UMath::Distance(ai->GetRoadblock()->GetRoadBlockCentre(), pPos);

        if (((mFlags ^ SETUP) & SETUP) != 0) {
            mFlags |= SETUP;
            mFlags |= REQ_SERVICE;
            mT_setup = WorldTimer;
        }

        bool visual = false;
        const IRoadBlock::Vehicles &rb_cars = rb->GetVehicles();
        if (rb_cars.size() != 0) {
            for (IRoadBlock::Vehicles::const_iterator i = rb_cars.begin(); i != rb_cars.end(); ++i) {
                IVehicle *car = *i;
                IPursuitAI *car_ai;
                IRenderable *renderable;
                if (car->QueryInterface(&car_ai)) {
                    car->QueryInterface(&renderable);
                    if (car_ai->GetTimeSinceTargetSeen() <= 0.0f || (renderable != 0 && renderable->InView())) {
                        visual = true;
                        break;
                    }
                }
            }
        }

        // El objetivo salta al `else` TAMBIEN cuando LOS ya esta puesto
        // (`beq .L_802A1E74` y `bne .L_802A1E74` al MISMO bloque): es un solo
        // `if` con `&&`, no un `if` anidado.
        if (visual && (mFlags & LOS) == 0) {
            mFlags |= LOS;
            mFlags |= REQ_SERVICE;
            MMiscSound message(1);
            message.Send(UCrc32("RBLOS"));
        } else {
            mFlags &= ~LOS;
        }
    }

    if (((mFlags & RB_ENABLED) == 0 && ai->RoadblocksEnabled()) || ((mFlags & HELIRB_ENABLED) == 0 && ai->HeliRoadblocksEnabled()) || ((mFlags & SPIKES_ENABLED) == 0 && ai->SpikesEnabled()) || ((mFlags & RB_ENABLED) != 0 && !ai->RoadblocksEnabled()) || ((mFlags & HELIRB_ENABLED) != 0 && !ai->HeliRoadblocksEnabled()) || ((mFlags & SPIKES_ENABLED) != 0 && !ai->SpikesEnabled())) {
        Request();
    }
}

void RoadblockFlow::Update() {
    int y;
    int x;

    SyncRoadblock();

    // El bloque de depuracion no emite codigo (todo muerto), pero SI parte la
    // funcion en dos bloques basicos: por eso `this` lo reparte global_alloc y
    // cae en r31 (local_alloc nunca asigna r31 y daria r30 + stmw).
    y = SPEECHFLOW_DISPLAY;
    if (y != 0) {
        unsigned int color = 0xffffffff;
        y = 0;
        x = 0;
    }

    Service();
}

void RoadblockFlow::MessageReqHeliJoinRB(const MReqRoadBlock &message) {
    SoundAI *ai = SoundAI::Get();
    mFlags |= HELIJOINED;
    if (ai->GetHeli() != nullptr && ai->GetPursuitState() == SoundAI::kActive) {
        ai->GetHeli()->JoinRB();
    }
}

void RoadblockFlow::MessagePositionUpdate(const MReqRoadBlock &message) {
    mFlags |= POSITIONED;
    mFlags |= REQ_SERVICE;
    mSpikeOffset = message.GetData();
}

void RoadblockFlow::MessageEventComplete(const MNotifySpeechStatus &) {
    mFlags &= ~REQ_SERVICE;
}

void RoadblockFlow::Request() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != 0) {
        bool roadblocks_enabled = ai->RoadblocksEnabled();

        if (!roadblocks_enabled && (mFlags & (REQUESTED | RB_ENABLED)) == 0) {
            if (Manager::GetHistory().GetCount(static_cast<SPCHType_1_EventID>(0x4c)) == 0 &&
                !Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x4c), 4) &&
                !Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x4c)) && ai->GetLeader() != 0) {
                ai->GetLeader()->CallForRB();
                ai->GetDispatch()->RBReply(ai->GetLeader(), 0, 0);
                ai->GetLeader()->NegRBReply();
            }
        } else {
            bool request_roadblock = ai->RoadblocksEnabled();

            if (((request_roadblock && (mFlags & RB_ENABLED) == 0) ||
                 (ai->SpikesEnabled() && (mFlags & (RB_ENABLED | SPIKES_ENABLED | CALLED_4_SPIKES)) == RB_ENABLED)) &&
                ai->GetLeader() != 0) {
                if (Manager::GetHistory().GetCount(static_cast<SPCHType_1_EventID>(0x4c)) == 0) {
                    ai->GetLeader()->CallForRB();
                } else {
                    ai->GetLeader()->RBReminder();
                }
                ai->GetDispatch()->RBReply(ai->GetLeader(), 1, 0);

                unsigned int flags = mFlags;
                mFlags = flags | RB_ENABLED;
                if (ai->SpikesEnabled()) {
                    mFlags = flags | (RB_ENABLED | SPIKES_ENABLED);
                }
            }

            bool roadblocks_enabled = ai->RoadblocksEnabled();

            if (roadblocks_enabled) {
                unsigned int flags = mFlags;
                if ((flags & RB_ENABLED) != 0 && ai->HeliRoadblocksEnabled() && (flags & HELIRB_ENABLED) == 0 &&
                    (flags & (CALLED_4_SPIKES | CALLED_4_NORMAL)) != 0 && (flags & REQUESTED) != 0 && ai->GetLeader() != 0) {
                    ai->GetLeader()->CallForBackup(8);
                    ai->GetDispatch()->RBReply(ai->GetLeader(), 0, 4);
                    mFlags |= HELIRB_ENABLED;
                }
            }
        }
    }
}

void RoadblockFlow::Setup() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (((mFlags ^ SETUP) & SETUP) != 0) {
        EAXCop *primary = ai->GetRandomCop(1);
        EAXCop *secondary = ai->GetRandomCop(2);

        if (mNumBlocks >= 2 && secondary != 0) {
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
    } else {
        if ((mFlags & POSITIONED) != 0) {
            bool pos_comment = true;
            if (ai->GetRoadblock() != 0) {
                pos_comment = ai->GetRoadblock()->GetNumSpikeStrips() <= 1;
            }

            if (pos_comment) {
                MiscSpeech::RBPosition(mSpikeOffset);
            } else {
                EAXCop *cop_in_rb = ai->GetCopInRB();
                if (cop_in_rb == 0) {
                    cop_in_rb = ai->GetRandomCop(2);
                }
                if (cop_in_rb != 0) {
                    cop_in_rb->RBApproach();
                } else {
                    MiscSpeech::RBWarning();
                }
            }
        } else {
            bool should_interrupt_dispatch = false;
            if (Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x4d)) ||
                Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x4e))) {
                should_interrupt_dispatch = true;
            }
            if (bRandom(1.0f) > 0.5f && !should_interrupt_dispatch) {
                EAXCop *primary = ai->GetRandomActiveCop(1, false);
                if (primary == 0) {
                    return;
                }
                ai->GetDispatch()->RBUpdate(primary, 1);
                primary->PursuitApproaching();
                mFlags &= ~REQ_SERVICE;
                return;
            } else {
                MiscSpeech::RBWarning();
                mFlags &= ~REQ_SERVICE;
                return;
            }
        }
    }

    mFlags &= ~REQ_SERVICE;
}

void RoadblockFlow::Approach() {
    SoundAI *ai = SoundAI::Get();
    EAXCop *cop_LOS = ai->GetRandomCop(2);
    if (cop_LOS != nullptr) {
        cop_LOS->RBApproach();
        mFlags &= ~REQ_SERVICE;
    }
}

void RoadblockFlow::Effect() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == 0) {
        return;
    }

    if ((mFlags & OUTCOMETIMERSET) == 0) {
        return;
    }

    float t_engaged = (WorldTimer - mT_engaged).GetSeconds();
    float t_averted = (WorldTimer - mT_averted).GetSeconds();

    if ((mFlags & (AVERTED | ENGAGED)) == (AVERTED | ENGAGED)) {
        if (t_averted >= ai->GetTune().RBOutcomeTimer() && t_engaged >= ai->GetTune().RBOutcomeTimer()) {
            mFlags &= ~OUTCOMETIMERSET;
        }
    }

    if ((mFlags & (AVERTED | ENGAGED)) == AVERTED) {
        if (t_averted >= ai->GetTune().RBOutcomeTimer()) {
            mFlags &= ~OUTCOMETIMERSET;
        }
    }

    if ((mFlags & (AVERTED | ENGAGED)) == ENGAGED && t_engaged >= ai->GetTune().RBOutcomeTimer() && (mFlags & LOS) != 0) {
        EAXCop *cop = ai->GetRandomActiveCop(0, true);
        if (cop != 0) {
            cop->Arrest();
        }
    }

    if ((mFlags & OUTCOMETIMERSET) != 0) {
        return;
    }

    IRoadBlock *block = ai->GetRoadblock();
    if (block != 0) {
        EAXCop *secondary = ai->GetRandomCop(2);
        EAXCop *randcop = ai->GetRandomCop(0);

        if (block->GetNumCopsDamaged() > 0 || block->GetNumCopsDestroyed() > 0 || (mFlags & ENGAGED) != 0) {
            if (secondary != 0 && bRandom(1.0f) > 0.5f) {
                secondary->RBEngage((mFlags & NAILED_SPIKES) != 0);
            } else {
                MiscSpeech::RBEngaged((mFlags & NAILED_SPIKES) != 0);
            }
        } else {
            bool disp_response = false;
            if (block->GetNumSpikeStrips() > 0) {
                MiscSpeech::RBAverted();
            } else if (bRandom(1.0f) > 0.5f) {
                disp_response = true;
                ai->GetDispatch()->SubRBReply();
            } else if (secondary != 0 && bRandom(1.0f) > 0.5f) {
                secondary->RBAverted();
            } else {
                MiscSpeech::RBAverted();
            }

            if (!disp_response && randcop != 0 && randcop->IsPrimary()) {
                randcop->CallForSubRB();
            } else {
                ai->GetDispatch()->SubRBReply();
            }
        }

        mFlags &= ~REQ_SERVICE;
    }

    if ((mFlags & (AVERTED | ENGAGED)) != 0 && (mFlags & REQ_SERVICE) == 0) {
        mFlags |= RESET_PENDING;
        mT_reset = WorldTimer;
    }
}

void RoadblockFlow::Service() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if ((ai->GetFocus() != SoundAI::kStrategyFlow) && (ai->GetFocus() != SoundAI::kLost)) {
        ChangeStateTo(kTerminal);
        return;
    }

    if (((mFlags & REQUESTED) == 0) &&
        ((Manager::GetHistory().GetCount(kSPCH1_EventID_InitStrategy) > 0) ||
         (Manager::GetHistory().GetCount(kSPCH1_EventID_SelfStrategy) > 0))) {
        Request();
    }

    if ((mFlags & REQ_SERVICE) != 0) {
        if ((mFlags & (AVERTED | ENGAGED)) == 0) {
            if ((mFlags & LOS) != 0) {
                Approach();
            } else if ((mFlags & SETUP) != 0) {
                Setup();
            }
        }
    }

    if ((mFlags & OUTCOMETIMERSET) != 0) {
        Effect();
    }
}

void RoadblockFlow::Terminal() {
    mBusy = 0;
    mFlags = 0;
    mSpikeOffset = 0;
    mPertinentRB = nullptr;
    mLoDist2RB = 32767.0f;
}

void RoadblockFlow::Reset() {
    Terminal();
    SpeechFlow::Reset();
}

bool RoadblockFlow::IsTransitionable() {
    return true;
}

} // namespace Speech
