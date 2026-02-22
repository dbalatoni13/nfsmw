#include "CompoundChannel.h"
#include "MemoryPoolManager.h"

namespace EAGL4Anim {

FnCompoundChannel::~FnCompoundChannel() {
    if (mChannels) {
        CompoundChannel *prevCompoundChannel = reinterpret_cast<CompoundChannel *>(mpAnim);
        for (int i = prevCompoundChannel->GetNumChannels() - 1; i >= 0; i--) {
            MemoryPoolManager::DeleteFnAnim(mChannels[i]);
        }
        MemoryPoolManager::DeleteBlock(mChannels);
    }
}

void FnCompoundChannel::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
    if (mChannels) {
        CompoundChannel *prevCompoundChannel = reinterpret_cast<CompoundChannel *>(mpAnim);
        for (int i = prevCompoundChannel->GetNumChannels() - 1; i >= 0; i--) {
            MemoryPoolManager::DeleteFnAnim(mChannels[i]);
        }
        MemoryPoolManager::DeleteBlock(mChannels);
    }
    mChannels = nullptr;
}

bool FnCompoundChannel::EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) {}

}; // namespace EAGL4Anim
