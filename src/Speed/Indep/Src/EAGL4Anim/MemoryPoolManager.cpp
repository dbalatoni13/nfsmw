#include "MemoryPoolManager.h"
#include "CompoundChannel.h"
#include "DeltaChan.h"
#include "FnAnimMemoryMap.h"
#include "FnCsisEventChannel.h"
#include "FnCycle.h"
#include "FnDeltaF1.h"
#include "FnDeltaF3.h"
#include "FnDeltaQ.h"
#include "FnDeltaQFast.h"
#include "FnDeltaSingleQ.h"
#include "FnEventBlender.h"
#include "FnGraft.h"
#include "FnPoseAnim.h"
#include "FnPoseBlender.h"
#include "FnPoseMirror.h"
#include "FnRawEventChannel.h"
#include "FnRawLinearChannel.h"
#include "FnRawPoseChannel.h"
#include "FnRunBlender.h"
#include "FnStatelessF3.h"
#include "FnStatelessQ.h"
#include "FnTurnBlender.h"
#include "PhaseChan.h"
#include "PoseAnim.h"
#include "RawPoseChannel.h"
#include "RawStateChan.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "StatelessF3.h"
#include "StatelessQ.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

unsigned int MemoryPoolManager::GetMemoryPoolUsageAux() {
    return gMemoryPoolFree - gMemoryPool;
}

unsigned int MemoryPoolManager::GetFreePoolSizeAux() {
    return gMemoryPoolSize - GetMemoryPoolUsage();
}

void MemoryPoolManager::AllocateIdxBlock(unsigned short idx) {
    unsigned int size = gFreeListSize[idx];

    *reinterpret_cast<char **>(gMemoryPoolFree) = gFreeList[idx];
    gFreeList[idx] = gMemoryPoolFree;
    gMemoryPoolFree += size;
}

void *MemoryPoolManager::NewBlockByIdxAux(unsigned short idx) {
    char *r = gFreeList[idx];
    if (!r) {
        AllocateIdxBlock(idx);
        r = gFreeList[idx];
    }
    gFreeList[idx] = *reinterpret_cast<char **>(r);
    return r;
}

void *MemoryPoolManager::NewBlockAux(size_t size) {
    unsigned char idx = static_cast<unsigned char>(size / 16);
    char *r = gSizeFreeList[idx];
    if (!r) {
        *reinterpret_cast<unsigned int *>(gMemoryPoolFree) = idx;
        r = gMemoryPoolFree + 4;
        gMemoryPoolFree += (idx + 1) * 0x10 + 4;
        return r;
    } else {
        gSizeFreeList[idx] = *reinterpret_cast<char **>(r);
        return r;
    }
}

void MemoryPoolManager::ResetPoolAux() {
    gMemoryPoolFree = gMemoryPool;
    int i;
    for (i = 0; i < 256; i++) {
        gSizeFreeList[i] = 0;
    }
    for (i = 0; i < 26; i++) {
        gFreeList[i] = 0;
    }
}

void MemoryPoolManager::InitAux(unsigned int poolSize) {
    gMemoryPoolSize = poolSize;
    gMemoryPool = (char *)EAGL4Internal::EAGL4Malloc(poolSize, "EAGL4Anim Memory Pool");
    gMaxIdx = 0;
    ResetPool();
}

void MemoryPoolManager::CleanupAux() {
    EAGL4Internal::EAGL4Free(gMemoryPool, gMemoryPoolSize);
}

// TODO do those constructors really take a second argument?
// TODO the problem here is probably in one or more constructors
FnAnim *MemoryPoolManager::NewFnAnimAux(AnimTypeId::Type type) {
    void *block = NewBlockByIdx(type);
    switch (type) {
        case AnimTypeId::ANIM_RAWPOSE:
            new (block) FnRawPoseChannel();
            break;
        case AnimTypeId::ANIM_RAWEVENT:
            new (block) FnRawEventChannel();
            break;
        case AnimTypeId::ANIM_RAWLINEAR:
            new (block) FnRawLinearChannel();
            break;
        case AnimTypeId::ANIM_CYCLE:
            new (block) FnCycle();
            break;
        case AnimTypeId::ANIM_EVENTBLENDER:
            new (block) FnEventBlender();
            break;
        case AnimTypeId::ANIM_GRAFT:
            new (block) FnGraft();
            break;
        case AnimTypeId::ANIM_POSEBLENDER:
            new (block) FnPoseBlender();
            break;
        case AnimTypeId::ANIM_POSEMIRROR:
            new (block) FnPoseMirror();
            break;
        case AnimTypeId::ANIM_RUNBLENDER:
            new (block) FnRunBlender();
            break;
        case AnimTypeId::ANIM_TURNBLENDER:
            new (block) FnTurnBlender();
            break;
        case AnimTypeId::ANIM_DELTALERP:
            new (block) FnDeltaLerpChan();
            break;
        case AnimTypeId::ANIM_DELTAQUAT:
            new (block) FnDeltaQuatChan();
            break;
        case AnimTypeId::ANIM_KEYLERP:
            new (block) FnKeyLerpChan();
            break;
        case AnimTypeId::ANIM_KEYQUAT:
            new (block) FnKeyQuatChan();
            break;
        case AnimTypeId::ANIM_PHASE:
            new (block) FnPhaseChan();
            break;
        case AnimTypeId::ANIM_COMPOUND:
            new (block) FnCompoundChannel();
            break;
        case AnimTypeId::ANIM_RAWSTATE:
            new (block) FnRawStateChan();
            break;
        case AnimTypeId::ANIM_DELTAQ:
            new (block) FnDeltaQ();
            break;
        case AnimTypeId::ANIM_DELTAQFAST:
            new (block) FnDeltaQFast();
            break;
        case AnimTypeId::ANIM_DELTASINGLEQ:
            new (block) FnDeltaSingleQ();
            break;
        case AnimTypeId::ANIM_DELTAF3:
            new (block) FnDeltaF3();
            break;
        case AnimTypeId::ANIM_DELTAF1:
            new (block) FnDeltaF1();
            break;
        case AnimTypeId::ANIM_CSISEVENT:
            new (block) FnCsisEventChannel();
            break;
        default:
            break;
    }
    return reinterpret_cast<FnAnim *>(block);
}

FnAnimMemoryMap *MemoryPoolManager::NewFnAnimAux(AnimMemoryMap *anim) {
    FnAnimMemoryMap *fnMemMap;

    switch (anim->GetType().GetType()) {
        case AnimTypeId::ANIM_STATELESSQ: {
            StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(anim);
            FnStatelessQ *fnStatelessQ = reinterpret_cast<FnStatelessQ *>(statelessQ->GetFnLocation());
            fnMemMap = fnStatelessQ;
            break;
        }
        case AnimTypeId::ANIM_STATELESSF3: {
            StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(anim);
            FnStatelessF3 *fnStatelessF3 = reinterpret_cast<FnStatelessF3 *>(statelessF3->GetFnLocation());
            fnMemMap = fnStatelessF3;
            break;
        }
        case AnimTypeId::ANIM_POSEANIM: {
            PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(anim);
            FnPoseAnim *fnPoseAnim = reinterpret_cast<FnPoseAnim *>(poseAnim->GetFnLocation());
            fnMemMap = fnPoseAnim;
            break;
        }
        default:
            fnMemMap = (FnAnimMemoryMap *)NewFnAnim(anim->GetType().GetType());
            break;
    }
    fnMemMap->SetAnimMemoryMap(anim);
    return fnMemMap;
}

void MemoryPoolManager::InitAnimMemoryMapAux(AnimMemoryMap *memMap) {
    switch (memMap->GetType().GetType()) {
        case AnimTypeId::ANIM_RAWPOSE:
            RawPoseChannel::InitAnimMemoryMap(memMap);
            break;
        case AnimTypeId::ANIM_RAWEVENT:
            break;
        case AnimTypeId::ANIM_RAWLINEAR:
            break;
        case AnimTypeId::ANIM_CYCLE:
            break;
        // case AnimTypeId::ANIM_EVENTBLENDER:
        //     break;
        // case AnimTypeId::ANIM_GRAFT:
        //     break;
        // case AnimTypeId::ANIM_POSEBLENDER:
        //     break;
        // case AnimTypeId::ANIM_POSEMIRROR:
        //     break;
        // case AnimTypeId::ANIM_RUNBLENDER:
        //     break;
        // case AnimTypeId::ANIM_TURNBLENDER:
        //     break;
        // case AnimTypeId::ANIM_DELTALERP:
        //     break;
        // case AnimTypeId::ANIM_DELTAQUAT:
        //     break;
        // case AnimTypeId::ANIM_KEYLERP:
        //     break;
        // case AnimTypeId::ANIM_KEYQUAT:
        //     break;
        // case AnimTypeId::ANIM_PHASE:
        //     break;
        case AnimTypeId::ANIM_COMPOUND:
            CompoundChannel::InitAnimMemoryMap(memMap);
            break;
        // case AnimTypeId::ANIM_RAWSTATE:
        //     break;
        // case AnimTypeId::ANIM_DELTAQ:
        //     break;
        // case AnimTypeId::ANIM_DELTAQFAST:
        //     break;
        // case AnimTypeId::ANIM_DELTASINGLEQ:
        //     break;
        // case AnimTypeId::ANIM_DELTAF3:
        //     break;
        // case AnimTypeId::ANIM_DELTAF1:
        //     break;
        case AnimTypeId::ANIM_STATELESSQ:
            StatelessQ::InitAnimMemoryMap(memMap);
            break;
        case AnimTypeId::ANIM_STATELESSF3:
            StatelessF3::InitAnimMemoryMap(memMap);
            break;
        // case AnimTypeId::ANIM_CSISEVENT:
        //     break;
        case AnimTypeId::ANIM_POSEANIM:
            PoseAnim::InitAnimMemoryMap(memMap);
            break;
        default:
            break;
    }
}

void MemoryPoolManager::DeleteBlockByIdxAux(unsigned short idx, void *ptr) {
    *reinterpret_cast<char **>(ptr) = gFreeList[idx];
    gFreeList[idx] = reinterpret_cast<char *>(ptr);
}

void MemoryPoolManager::DeleteBlockAux(void *ptr) {
    unsigned int idx = reinterpret_cast<int *>(ptr)[-1];

    *reinterpret_cast<char **>(ptr) = gSizeFreeList[idx];
    gSizeFreeList[idx] = reinterpret_cast<char *>(ptr);
}

void MemoryPoolManager::DeleteFnAnimAux(FnAnim *fnAnim) {
    if (fnAnim->GetType() == AnimTypeId::ANIM_STATELESSQ) {
        return;
    }
    if (fnAnim->GetType() == AnimTypeId::ANIM_STATELESSF3) {
        return;
    }
    if (fnAnim->GetType() == AnimTypeId::ANIM_POSEANIM) {
        return;
    }
    fnAnim->~FnAnim();
    DeleteBlockByIdx(fnAnim->GetType(), fnAnim);
}

}; // namespace EAGL4Anim
