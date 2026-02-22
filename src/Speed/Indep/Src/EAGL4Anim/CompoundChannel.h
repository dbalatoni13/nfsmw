#ifndef EAGL4ANIM_COMPOUNDCHANNEL_H
#define EAGL4ANIM_COMPOUNDCHANNEL_H

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "BoneMask.h"
#include "FnAnimMemoryMap.h"
#include "PhaseChan.h"
#include "PosePalette.h"

namespace EAGL4Anim {

// total size: 0x10
struct CompoundChannel : public AnimMemoryMap {
    void SetNumFrames(unsigned short n) {
        mNumChannels = n;
    }

    unsigned short GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumChannels(unsigned short n) {
        mNumChannels = n;
    }

    unsigned short GetNumChannels() const {
        return mNumChannels;
    }

    AnimMemoryMap **GetChannels() {
        return mChannels;
    }

    const AnimMemoryMap *const *GetChannels() const {
        return mChannels;
    }

    int GetSize() const {}

    static int ComputeSize(int numChannels) {}

    const AttributeBlock *GetAttributeBlock() const {
        return mAttributeBlock;
    }

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    AttributeBlock *mAttributeBlock; // offset 0x4, size 0x4
    unsigned short mNumChannels;     // offset 0x8, size 0x2
    unsigned short mNumFrames;       // offset 0xA, size 0x2
    AnimMemoryMap *mChannels[1];     // offset 0xC, size 0x4
};

// total size: 0x1C
class FnCompoundChannel : public FnAnimMemoryMap {
  public:
    FnCompoundChannel() {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    // Overrides: FnAnimSuper
    ~FnCompoundChannel() override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *dofs) override {}

    // Overrides: FnAnim
    bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) override;

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) override;

    // Overrides: FnAnim
    bool EvalPose(float currentTime, const PosePaletteBank *paletteBank, float *sqt) override;

    // Overrides: FnAnim
    bool EvalWeights(float currentTime, float *weights) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currentTime, float *vel) override;

    // Overrides: FnAnim
    bool EvalState(float currentTime, State *state) override;

    // Overrides: FnAnim
    bool FindTime(const StateTest &test, float startTime, float &resultTime) override;

    // Overrides: FnAnim
    bool EvalPhase(float currentTime, PhaseValue &phase) override;

    // Overrides: FnAnim
    const PhaseChan *GetPhaseChan() override;

    // Overrides: FnAnim
    void UseFPS(bool u) override;

    unsigned char GetFPS() const;

    // Overrides: FnAnim
    unsigned short GetTargetCheckSum() const override {}

    CompoundChannel *GetCompoundChannel() {}

    const CompoundChannel *GetCompoundChannel() const {}

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {}

    // Overrides: FnAnim
    const AttributeBlock *GetAttributes() const override {}

  protected:
    void InitSubChannels() {}

  private:
    FnAnim **mChannels; // offset 0x10, size 0x4
    bool mUseFPS;       // offset 0x14, size 0x1
    unsigned char mFPS; // offset 0x18, size 0x1
};

}; // namespace EAGL4Anim

#endif
