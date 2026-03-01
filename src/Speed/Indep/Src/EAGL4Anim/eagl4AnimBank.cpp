#include "eagl4AnimBank.h"
#include "CompoundChannel.h"
#include "FnPoseAnim.h"
#include "FnStatelessF3.h"
#include "FnStatelessQ.h"
#include "MemoryPoolManager.h"
#include "PoseAnim.h"
#include "StatelessF3.h"
#include "StatelessQ.h"
#include <cstring>

namespace EAGL4Anim {

void AnimBank::Constructor(void *ptr, EAGL4::DynamicLoader *loader, const char *pSymbolName) {
    AnimBank *bank = reinterpret_cast<AnimBank *>(ptr);
    for (int i = bank->GetNumAnims() - 1; i >= 0; i--) {
        MemoryPoolManager::InitAnimMemoryMap(bank->GetAnim(i));
    }
}

void AnimBank::Destructor(void *ptr) {}

int AnimBank::GetAnimIndex(const char *name) {
    int r;
    int m;
    int s = 0;
    int e = GetNumAnims() - 1;

    while (s <= e) {
        m = (s + e) >> 1;
        r = strcmp(name, mNames[m]);

        if (r > 0) {
            s = m + 1;
        } else if (r < 0) {
            e = m - 1;
        } else {
            return m;
        }
    }

    return -1;
}

FnAnimMemoryMap *AnimBank::NewFnAnim(int i) const {
    return NewFnAnim(mAnims[i]);
}

FnAnimMemoryMap *AnimBank::NewFnAnim(AnimMemoryMap *animMem) {
    StatelessQ *statelessQ;
    FnStatelessQ *fnStatelessQ;

    StatelessF3 *statelessF3;
    FnStatelessF3 *fnStatelessF3;

    PoseAnim *poseAnim;
    FnPoseAnim *fnPoseAnim;

    void *block;
    FnCompoundChannel *fnComp;

    switch (animMem->GetType().GetType()) {
        case AnimTypeId::ANIM_STATELESSQ:
            statelessQ = reinterpret_cast<StatelessQ *>(animMem);
            fnStatelessQ = reinterpret_cast<FnStatelessQ *>(statelessQ->GetFnLocation());
            fnStatelessQ->SetAnimMemoryMap(animMem);
            return fnStatelessQ;
        case AnimTypeId::ANIM_STATELESSF3:
            statelessF3 = reinterpret_cast<StatelessF3 *>(animMem);
            fnStatelessF3 = reinterpret_cast<FnStatelessF3 *>(statelessF3->GetFnLocation());
            fnStatelessF3->SetAnimMemoryMap(animMem);
            return fnStatelessF3;
        case AnimTypeId::ANIM_POSEANIM:
            poseAnim = reinterpret_cast<PoseAnim *>(animMem);
            fnPoseAnim = reinterpret_cast<FnPoseAnim *>(poseAnim->GetFnLocation());
            fnPoseAnim->SetAnimMemoryMap(animMem);
            return fnPoseAnim;
        case AnimTypeId::ANIM_COMPOUND:
            block = MemoryPoolManager::NewBlockByIdx(AnimTypeId::ANIM_COMPOUND);
            fnComp = new (block) FnCompoundChannel();
            fnComp->SetAnimMemoryMap(animMem);
            return fnComp;
        default:
            return nullptr;
    }
}

}; // namespace EAGL4Anim
