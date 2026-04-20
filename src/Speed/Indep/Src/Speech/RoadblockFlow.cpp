#include "RoadblockFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"

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
}

RoadblockFlow::~RoadblockFlow() {}

void RoadblockFlow::NailedSomethingInRB(unsigned int what) {
    mFlags |= what;
    mFlags |= REQ_SERVICE;
}

void RoadblockFlow::MessageRoadBlockDodged(const MReqRoadBlock &) {
    mFlags |= AVERTED;
    mT_averted = WorldTimer;
}

void RoadblockFlow::SyncRoadblock() {
    if ((mFlags & REQUESTED) == 0) {
        Request();
        return;
    }

    if ((mFlags & SETUP) == 0) {
        Setup();
        return;
    }

    if ((mFlags & POSITIONED) == 0) {
        Approach();
        return;
    }

    if ((mFlags & ENGAGED) == 0) {
        Effect();
        return;
    }

    if ((mFlags & REQ_SERVICE) != 0) {
        Service();
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
