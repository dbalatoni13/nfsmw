#ifndef __RBFLOW_H
#define __RBFLOW_H 1 // Decl: 15

#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

// total size: 0x44
// Decl: 29
struct RoadblockFlow : public SpeechFlow {
    enum State {
        kTransition = -1,
        kWaiting = 0,
        kTerminal = 999,
    };
    enum RBFlowState {
        SETUP = 1 << 0,
        LOS = 1 << 1,
        HELIJOINED = 1 << 2,
        AVERTED = 1 << 3,
        NAILED_SPIKES = 1 << 4,
        NAILED_OTHEROBJ = 1 << 5,
        NAILED_VEHICLE = 1 << 6,
        ENGAGED = 1 << 7,
        POSITIONED = 1 << 8,
        REQUESTED = 1 << 9,
        RB_ENABLED = 1 << 10,
        HELIRB_ENABLED = 1 << 11,
        SPIKES_ENABLED = 1 << 12,
        CALLED_4_SPIKES = 1 << 13,
        CALLED_4_NORMAL = 1 << 14,
        CALLED_4_HELI = 1 << 15,
        CALL_DENIED = 1 << 16,
        RESET_PENDING = 1 << 17,
        OUTCOMETIMERSET = 1 << 18,
        REQ_SERVICE = 1 << 28,
    };

    RoadblockFlow(); // Decl: 74

    ~RoadblockFlow() override;

    // Overrides: SpeechFlow
    void Update() override;
    bool IsTransitionable() override;
    void Reset() override;

    void NailedSomethingInRB(unsigned int what);

    // Decl: 39
    bool HasNailedSpikes() {
        return (this->mFlags & NAILED_SPIKES) != 0;
    }

  private:
    RoadblockFlow(const RoadblockFlow &);
    const RoadblockFlow &operator=(const RoadblockFlow &);

    void Request();

    void Setup();

    void Approach();

    void Effect();

    void Terminal();

    void Service();

    void SyncRoadblock();

    void MessageReqHeliJoinRB(const MReqRoadBlock &message);

    void MessageRoadBlockDodged(const MReqRoadBlock &message);

    void MessagePositionUpdate(const MReqRoadBlock &message);

    void MessageEventComplete(const MNotifySpeechStatus &message);

    Timer mT_setup;           // offset 0x10, size 0x4, Decl: 87
    Timer mT_engaged;         // offset 0x14, size 0x4, Decl: 88
    Timer mT_averted;         // offset 0x18, size 0x4, Decl: 89
    Timer mT_reset;           // offset 0x1C, size 0x4, Decl: 90
    float mLoDist2RB;         // offset 0x20, size 0x4, Decl: 91
    unsigned int mFlags;      // offset 0x24, size 0x4, Decl: 92
    int mSpikeOffset;         // offset 0x28, size 0x4, Decl: 93
    IRoadBlock *mPertinentRB; // offset 0x2C, size 0x4, Decl: 94
    int mNumBlocks;           // offset 0x30, size 0x4, Decl: 95

    Hermes::HHANDLER mMsgReqHeliJoinRB; // offset 0x34, size 0x4, Decl: 97

    Hermes::HHANDLER mMsgRoadBlockDodged; // offset 0x38, size 0x4, Decl: 100

    Hermes::HHANDLER mMsgPosition; // offset 0x3C, size 0x4, Decl: 103

    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x40, size 0x4, Decl: 106
};

}; // namespace Speech

#endif
