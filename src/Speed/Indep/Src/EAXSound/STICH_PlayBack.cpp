#include "./STICH_Playback.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <algorithm>

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
    for (SampleQueueItem *iter = mQueuedSampleList[type].begin();
         iter != mQueuedSampleList[type].end(); ++iter) {
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
    if (num_to_clear == 0) {
        return 0;
    }

    int num_pruned = 0;
    UTL::Std::list<SampleQueueItem, _type_list> destroyed;

    for (SampleQueueItem *iter = GetQueueList(type).begin(); iter != GetQueueList(type).end();
         ++iter) {
        SampleQueueItem sampleitem = *iter;
        if (static_cast<int>(sampleitem.pSample->GetData().Priority) <= priority) {
            destroyed.push_back(sampleitem);
            num_pruned++;
        }
        if (num_to_clear == num_pruned) {
            break;
        }
    }

    std::for_each(destroyed.begin(), destroyed.end(), RemoveFromList);

    return num_pruned;
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

void cSTICH_PlayBack::Update(float t) {
    for (int n = 0; n < MAX_NUM_STICH_TYPE; n++) {
        while (mQueuedSampleList[n].size() != 0) {
            SampleQueueItem samplereq = mQueuedSampleList[n][mQueuedSampleList[n].size() - 1];
            mQueuedSampleList[n].pop_back();
            samplereq.pSample->Play(&samplereq.pStitch->SndParams);
        }
    }
}

void cSTICH_PlayBack::DestroyAllStichs(void) {
    for (int i = 0; i < MAX_NUM_STICH_TYPE; i++) {
        GetQueueList(static_cast<STICH_TYPE>(i)).clear();
        while (!StichList[i].IsEmpty()) {
            StichList[i].RemoveHead();
        }
    }
}

char *GetStichTypeName(STICH_TYPE CurType) {
    return "";
}

void KillSample(cSampleWarpper *sampleref) {
    sampleref->Destroy();
}

cSampleWarpper::cSampleWarpper(SND_SampleRef &NewRef) {
    SampleRefData = &NewRef;
    m_eIsPlaying = 0;
    AEMS_ActiveSampleWsh = nullptr;
    AEMS_ActiveSampleCol = nullptr;
    AEMS_ActiveSampleStatic = nullptr;
}

void cSampleWarpper::Initialize() {
    m_eIsPlaying = 1;
}
