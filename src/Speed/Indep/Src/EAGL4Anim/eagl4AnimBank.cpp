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

// TODO do this when we figure out how all inlines work
// FnAnimMemoryMap *AnimBank::NewFnAnim(AnimMemoryMap *animMem) {
//     switch (animMem->GetType().GetType()) {
//         case AnimTypeId::ANIM_STATELESSQ:
//             StatelessQ *statelessQ;
//             FnStatelessQ *fnStatelessQ;
//             break;
//         case AnimTypeId::ANIM_STATELESSF3:
//             StatelessF3 *statelessF3;
//             FnStatelessF3 *fnStatelessF3;
//             break;
//         case AnimTypeId::ANIM_POSEANIM:
//             PoseAnim *poseAnim;
//             FnPoseAnim *fnPoseAnim;
//             break;
//         case AnimTypeId::ANIM_COMPOUND:
//             void *block;
//             FnCompoundChannel *fnComp;
//             break;

//         default:
//             return nullptr;
//     }
// }

}; // namespace EAGL4Anim
