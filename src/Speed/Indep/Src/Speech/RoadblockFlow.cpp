#include "RoadblockFlow.h"
#include "EAXCop.h"
#include "EAXDispatch.h"
#include "SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

namespace MiscSpeech {
void RBAverted();
void RBEngaged(bool spikes_hit);
void RBPosition(int pos);
void RBWarning();
}

namespace Speech {

RoadblockFlow::RoadblockFlow()
    : mT_setup(0), //
      mT_engaged(0), //
      mT_averted(0), //
      mT_reset(0), //
      mLoDist2RB(0.0f), //
      mFlags(0), //
      mSpikeOffset(0), //
      mPertinentRB(0), //
      mNumBlocks(0), //
      mMsgReqHeliJoinRB(0), //
      mMsgRoadBlockDodged(0), //
      mMsgPosition(0), //
      mMsgNotifyEventCompletion(0) {
    mState = kWaiting;
    mLastState = kTransition;

    mMsgReqHeliJoinRB =
        Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(this, &RoadblockFlow::MessageReqHeliJoinRB, "ReqHeliJoinRB", 0);
    mMsgRoadBlockDodged = Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(
        this, &RoadblockFlow::MessageRoadBlockDodged, "RoadBlockDodged", 0);
    mMsgPosition = Hermes::Handler::Create<MReqRoadBlock, RoadblockFlow, RoadblockFlow>(
        this, &RoadblockFlow::MessagePositionUpdate, "PositionUpdate", 0);
    mMsgNotifyEventCompletion = Hermes::Handler::Create<MNotifySpeechStatus, RoadblockFlow, RoadblockFlow>(
        this, &RoadblockFlow::MessageEventComplete, UCrc32(0x20d60dbf), 0);
}

RoadblockFlow::~RoadblockFlow() {
    if (mMsgReqHeliJoinRB) {
        Hermes::Handler::Destroy(mMsgReqHeliJoinRB);
        mMsgReqHeliJoinRB = 0;
    }
    if (mMsgRoadBlockDodged) {
        Hermes::Handler::Destroy(mMsgRoadBlockDodged);
        mMsgRoadBlockDodged = 0;
    }
    if (mMsgPosition) {
        Hermes::Handler::Destroy(mMsgPosition);
        mMsgPosition = 0;
    }
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
        mMsgNotifyEventCompletion = 0;
    }
}

void RoadblockFlow::NailedSomethingInRB(unsigned int what) {
    mFlags |= what;
    mFlags |= REQ_SERVICE;
}

void RoadblockFlow::MessageRoadBlockDodged(const MReqRoadBlock &) {
    mFlags |= AVERTED;
    mT_averted = WorldTimer;
}

void RoadblockFlow::SyncRoadblock() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == 0) {
        return;
    }

    UMath::Vector3 pPos = UMath::Vector3::kZero;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IRoadBlock *roadblock = ai->GetRoadblock();
    unsigned int flags = mFlags;

    if ((flags & RESET_PENDING) != 0) {
        float reset_time = (WorldTimer - mT_reset).GetSeconds();
        if (mPertinentRB != roadblock && reset_time >= ai->GetTune().RBPostOutcomeResetTime()) {
            mPertinentRB = roadblock;
            mFlags = flags & ~RESET_PENDING;
            if ((flags & SETUP) != 0) {
                mFlags |= REQ_SERVICE;
            }
            mFlags &= ~(SETUP | LOS | HELIJOINED | AVERTED | NAILED_SPIKES | NAILED_OTHEROBJ | NAILED_VEHICLE | ENGAGED | POSITIONED);
            ++mNumBlocks;
        }
    }

    if (roadblock != 0 && player != 0 && (mFlags & RESET_PENDING) == 0) {
        roadblock->GetPursuit();
        pPos = player->GetPosition();
        {
            UMath::Vector3 rbToPlayer;
            UMath::Sub(ai->GetRoadblock()->GetRoadBlockCentre(), pPos, rbToPlayer);
            mLoDist2RB = UMath::Length(rbToPlayer);
        }

        flags = mFlags;
        if (((flags ^ SETUP) & SETUP) != 0) {
            flags |= SETUP;
            mFlags = flags | REQ_SERVICE;
            mT_setup = WorldTimer;
        }

        bool has_los = false;
        const IRoadBlock::Vehicles &vehicles = roadblock->GetVehicles();
        if (vehicles.size() != 0) {
            for (IRoadBlock::Vehicles::const_iterator i = vehicles.begin(); i != vehicles.end(); ++i) {
                IVehicle *car = *i;
                IPursuitAI *car_ai;
                IRenderable *renderable;
                if (car->QueryInterface(&car_ai)) {
                    car->QueryInterface(&renderable);
                    if (car_ai->GetTimeSinceTargetSeen() <= 0.0f || (renderable != 0 && renderable->InView())) {
                        has_los = true;
                        break;
                    }
                }
            }
        }

        if (has_los) {
            if ((mFlags & LOS) == 0) {
                flags = mFlags | LOS;
                mFlags = flags | REQ_SERVICE;
                MMiscSound message(1);
                message.Send(UCrc32("RBLOS"));
            }
        } else {
            mFlags &= ~LOS;
        }
    }

    flags = mFlags;
    if ((flags & RB_ENABLED) == 0 && ai->RoadblocksEnabled()) {
        Request();
    } else if ((flags & HELIRB_ENABLED) == 0 && ai->HeliRoadblocksEnabled()) {
        Request();
    } else if ((flags & SPIKES_ENABLED) == 0 && ai->SpikesEnabled()) {
        Request();
    } else if ((flags & RB_ENABLED) != 0 && !ai->RoadblocksEnabled()) {
        Request();
    } else if ((flags & HELIRB_ENABLED) != 0 && !ai->HeliRoadblocksEnabled()) {
        Request();
    } else if ((flags & SPIKES_ENABLED) != 0 && !ai->SpikesEnabled()) {
        Request();
    }
}

void RoadblockFlow::Update() {
    if (mState == kTransition) {
        return;
    }

    if (mState == kTerminal) {
        Terminal();
        return;
    }

    SyncRoadblock();
}

void RoadblockFlow::MessageReqHeliJoinRB(const MReqRoadBlock &) {
    mFlags |= HELIJOINED;
}

void RoadblockFlow::MessagePositionUpdate(const MReqRoadBlock &) {
    mFlags |= POSITIONED;
}

void RoadblockFlow::MessageEventComplete(const MNotifySpeechStatus &) {
    mFlags &= ~REQ_SERVICE;
}

void RoadblockFlow::Request() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != 0) {
        bool roadblocks_enabled = false;
        if (ai->RoadblocksEnabled()) {
            roadblocks_enabled = true;
        }

        if (!roadblocks_enabled && (mFlags & (REQUESTED | RB_ENABLED)) == 0) {
            if (Manager::GetHistory().GetCount(static_cast<SPCHType_1_EventID>(0x4c)) == 0 &&
                !Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x4c), 4) &&
                !Manager::IsCopSpeechPlaying(static_cast<SPCHType_1_EventID>(0x4c)) && ai->GetLeader() != 0) {
                ai->GetLeader()->CallForRB();
                ai->GetDispatch()->RBReply(ai->GetLeader(), 0, 0);
                ai->GetLeader()->NegRBReply();
            }
        } else {
            bool request_roadblock = false;
            if (ai->RoadblocksEnabled()) {
                request_roadblock = true;
            }

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

            bool roadblocks_enabled = false;
            if (ai->RoadblocksEnabled()) {
                roadblocks_enabled = true;
            }

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
            } else {
                MiscSpeech::RBWarning();
            }
        }
    }

    mFlags &= ~REQ_SERVICE;
}

void RoadblockFlow::Approach() {
    mFlags |= POSITIONED;
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
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    mFlags &= ~REQ_SERVICE;
    mFlags |= RESET_PENDING;
    mT_reset = WorldTimer;
}

void RoadblockFlow::Terminal() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kTransition);
}

void RoadblockFlow::Reset() {
    mFlags = 0;
    mSpikeOffset = 0;
    mPertinentRB = 0;
    mNumBlocks = 0;
    mLoDist2RB = 0.0f;
    mBusy = 0;
    mState = kWaiting;
    mLastState = kTransition;
}

bool RoadblockFlow::IsTransitionable() {
    return mState == kTransition;
}

} // namespace Speech
