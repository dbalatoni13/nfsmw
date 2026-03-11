#include "./STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

cSTICH_PlayBack::cSTICH_PlayBack() {
    mSampleRefSlotPool = bNewSlotPool(32, 129, "SampleWrapper SlotPool", AudioMemoryPool);
    mSampleRefSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    mSampleRefSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);

    mStitchSlotPool = bNewSlotPool(108, 45, "Stitch SlotPool", AudioMemoryPool);
    mStitchSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    mStitchSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
}

cSTICH_PlayBack::~cSTICH_PlayBack() {
    if (mSampleRefSlotPool) {
        bDeleteSlotPool(mSampleRefSlotPool);
    }
    if (mStitchSlotPool) {
        bDeleteSlotPool(mStitchSlotPool);
    }
}

void cSTICH_PlayBack::QueueSampleRequest(struct SampleQueueItem &samplereq) {
    STICH_TYPE type = static_cast<STICH_TYPE>(samplereq.pStitch->GetData().eStichType);
    GetQueueList(type).push_back(samplereq);
}

void cSTICH_PlayBack::RemoveFromList(struct SampleQueueItem sampleitem) {
    STICH_TYPE type = static_cast<STICH_TYPE>(sampleitem.pStitch->GetData().eStichType);
    UTL::FixedVector<SampleQueueItem, 43, 16> &list = GetQueueList(type);
    for (SampleQueueItem *iter = list.begin(); iter != list.end(); ++iter) {
        SampleQueueItem compareto = *iter;
        if (compareto == sampleitem) {
            GetQueueList(type).erase(iter);
            break;
        }
    }
    sampleitem.pSample->Destroy();
}

// UNSOLVED
int cSTICH_PlayBack::Prune(STICH_TYPE type, int priority, int num_to_clear) {
    if (num_to_clear == 0)
        return 0;
    else {
        gFastMem.Alloc(16, nullptr);
    }
}

bool cSTICH_PlayBack::AddStich(STICH_TYPE StichType, SND_Stich &NewStichData) {
    StichList[StichType].AddTail(&NewStichData);
    return true;
}

bPList<SND_Stich> &cSTICH_PlayBack::GetStichList(STICH_TYPE StichType) {
    return StichList[StichType];
}

SND_Stich &cSTICH_PlayBack::GetStich(STICH_TYPE StichType, int Index) {
    return *reinterpret_cast<SND_Stich *>(GetStichList(StichType).GetNode(Index)->GetpObject());
}

void cSTICH_PlayBack::Update(float t) {}

void cSTICH_PlayBack::DestroyAllStichs(void) {}
