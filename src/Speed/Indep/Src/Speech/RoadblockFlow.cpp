#include "RoadblockFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

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
    mFlags |= REQUESTED;
    mT_setup = WorldTimer;
}

void RoadblockFlow::Setup() {
    mFlags |= SETUP;
    mT_engaged = WorldTimer;
}

void RoadblockFlow::Approach() {
    mFlags |= POSITIONED;
}

void RoadblockFlow::Effect() {
    mFlags |= ENGAGED;
    if (HasNailedSpikes()) {
        mFlags |= NAILED_SPIKES;
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
