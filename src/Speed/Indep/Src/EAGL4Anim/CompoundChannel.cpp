#include "CompoundChannel.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AttributeId.h"

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

bool FnCompoundChannel::EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        previousTime *= mFPS;
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalEvent(previousTime, currentTime, eventHandlers, extraData);
    }
    return handled;
}

bool FnCompoundChannel::EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalSQT(currentTime, sqt, boneMask);
    }
    return handled;
}

bool FnCompoundChannel::EvalPose(float currentTime, const PosePaletteBank *paletteBank, float *sqt) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalPose(currentTime, paletteBank, sqt);
    }
    return handled;
}

bool FnCompoundChannel::EvalWeights(float currentTime, float *weights) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalWeights(currentTime, weights);
    }
    return handled;
}

bool FnCompoundChannel::EvalVel2D(float currentTime, float *vel) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalVel2D(currentTime, vel);
    }
    return handled;
}

bool FnCompoundChannel::EvalState(float currentTime, State *state) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalState(currentTime, state);
    }
    return handled;
}

bool FnCompoundChannel::FindTime(const StateTest &test, float startTime, float &resultTime) {
    CompoundChannel *cchannel = GetCompoundChannel();

    if (mUseFPS) {
        startTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        if (mChannels[i]->FindTime(test, startTime, resultTime)) {
            if (mUseFPS) {
                resultTime /= mFPS;
            }
            return true;
        }
    }
    return false;
}

bool FnCompoundChannel::EvalPhase(float currentTime, PhaseValue &phase) {
    bool handled = false;
    CompoundChannel *cchannel = GetCompoundChannel();

    if (mUseFPS) {
        currentTime *= mFPS;
    }

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        handled |= mChannels[i]->EvalPhase(currentTime, phase);
    }
    return handled;
}

void CompoundChannel::InitAnimMemoryMap(AnimMemoryMap *anim) {
    CompoundChannel *cchannel = reinterpret_cast<CompoundChannel *>(anim);

    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        MemoryPoolManager::InitAnimMemoryMap(cchannel->mChannels[i]);
    }
}

const PhaseChan *FnCompoundChannel::GetPhaseChan() {
    CompoundChannel *cchannel = GetCompoundChannel();

    if (!mChannels) {
        InitSubChannels();
    }

    const PhaseChan *phaseChan;
    for (int i = cchannel->GetNumChannels() - 1; i >= 0; i--) {
        phaseChan = mChannels[i]->GetPhaseChan();
        if (phaseChan) {
            return phaseChan;
        }
    }
    return nullptr;
}

void FnCompoundChannel::UseFPS(bool u) {
    mUseFPS = u;
    if (mFPS != 0 || !u) {
        return;
    }
    GetAttribute(AttributeId(AttributeId::ID_FPS), mFPS);
}

}; // namespace EAGL4Anim
