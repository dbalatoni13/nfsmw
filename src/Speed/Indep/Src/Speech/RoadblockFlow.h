#ifndef SPEECH_ROADBLOCKFLOW_H
#define SPEECH_ROADBLOCKFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct IRoadBlock;
struct MNotifySpeechStatus;
struct MReqRoadBlock;

namespace Speech {

class RoadblockFlow : public SpeechFlow {
  public:
    enum State {
        kTransition = -1,
        kWaiting = 0,
        kTerminal = 999,
    };

    enum RBFlowState {
        SETUP = 1,
        LOS = 2,
        HELIJOINED = 4,
        AVERTED = 8,
        NAILED_SPIKES = 16,
        NAILED_OTHEROBJ = 32,
        NAILED_VEHICLE = 64,
        ENGAGED = 128,
        POSITIONED = 256,
        REQUESTED = 512,
        RB_ENABLED = 1024,
        HELIRB_ENABLED = 2048,
        SPIKES_ENABLED = 4096,
        CALLED_4_SPIKES = 8192,
        CALLED_4_NORMAL = 16384,
        CALLED_4_HELI = 32768,
        CALL_DENIED = 65536,
        RESET_PENDING = 131072,
        OUTCOMETIMERSET = 262144,
        REQ_SERVICE = 268435456,
    };

    RoadblockFlow();
    ~RoadblockFlow() override;
    void NailedSomethingInRB(unsigned int what);
    void MessageRoadBlockDodged(const MReqRoadBlock &message);
    void SyncRoadblock();
    void Update() override;
    void MessageReqHeliJoinRB(const MReqRoadBlock &message);
    void MessagePositionUpdate(const MReqRoadBlock &message);
    void MessageEventComplete(const MNotifySpeechStatus &message);
    void Request();
    void Setup();
    void Approach();
    void Effect();
    void Service();
    void Terminal();
    void Reset() override;
    bool IsTransitionable() override;

    bool HasNailedSpikes() const {
        return (mFlags & NAILED_SPIKES) != 0;
    }

  private:
    Timer mT_setup;                         // offset 0x10, size 0x4
    Timer mT_engaged;                       // offset 0x14, size 0x4
    Timer mT_averted;                       // offset 0x18, size 0x4
    Timer mT_reset;                         // offset 0x1C, size 0x4
    float mLoDist2RB;                       // offset 0x20, size 0x4
    unsigned int mFlags;                    // offset 0x24, size 0x4
    int mSpikeOffset;                       // offset 0x28, size 0x4
    IRoadBlock *mPertinentRB;               // offset 0x2C, size 0x4
    int mNumBlocks;                         // offset 0x30, size 0x4
    Hermes::HHANDLER mMsgReqHeliJoinRB;     // offset 0x34, size 0x4
    Hermes::HHANDLER mMsgRoadBlockDodged;   // offset 0x38, size 0x4
    Hermes::HHANDLER mMsgPosition;          // offset 0x3C, size 0x4
    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x40, size 0x4
};

} // namespace Speech


#endif
