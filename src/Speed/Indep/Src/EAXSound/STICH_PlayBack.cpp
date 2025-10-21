#include "./STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

cSTICH_PlayBack::cSTICH_PlayBack() {
    mSampleRefSlotPool = bNewSlotPool(32, 129, "SampleWrapper SlotPool", AudioMemoryPool);
    // compiler complains if i try to |=
    mSampleRefSlotPool->Flags = SlotPoolFlags(mSampleRefSlotPool->Flags | SLOTPOOL_FLAG_OVERFLOW_IF_FULL | SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    mStitchSlotPool = bNewSlotPool(108, 45, "Stitch SlotPool", AudioMemoryPool);
    mStitchSlotPool->Flags = SlotPoolFlags(mStitchSlotPool->Flags | SLOTPOOL_FLAG_OVERFLOW_IF_FULL | SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
}

cSTICH_PlayBack::~cSTICH_PlayBack() {
    if (mSampleRefSlotPool) {
        bDeleteSlotPool(mSampleRefSlotPool);
    }
    if (mStitchSlotPool) {
        bDeleteSlotPool(mStitchSlotPool);
    }
}

void cSTICH_PlayBack::QueueSampleRequest(struct SampleQueueItem &samplereq) {}

void cSTICH_PlayBack::RemoveFromList(struct SampleQueueItem sampleitem) {}

int cSTICH_PlayBack::Prune(STICH_TYPE type, int priority, int num_to_clear) {
    if (num_to_clear == 0)
        return 0;
    else {
        gFastMem.Alloc(16, nullptr);
    }
}

bool cSTICH_PlayBack::AddStich(STICH_TYPE StichType, SND_Stich &NewStichData) {
    GetStichList(StichType).AddTail(&NewStichData);
    return true;
}

SND_Stich &cSTICH_PlayBack::GetStich(STICH_TYPE StichType, int Index) {
    bPNode *node = GetStichList(StichType).GetNode(Index);
    return *reinterpret_cast<SND_Stich *>(node->Object); // i hate this too
}

void cSTICH_PlayBack::Update(float t) {}

void cSTICH_PlayBack::DestroyAllStichs(void) {}
