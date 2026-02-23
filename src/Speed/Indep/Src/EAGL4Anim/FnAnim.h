#ifndef EAGL4ANIM_FNANIM_H
#define EAGL4ANIM_FNANIM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimStat.h"
#include "AnimTypeId.h"
#include "Attribute.h"
#include "AttributeId.h"
#include "BoneMask.h"
#include "EventHandler.h"

#include <types.h>

namespace EAGL4Anim {

class MatchPhaseInput;
class PhaseChan;
class PhaseValue;
class PosePaletteBank;

struct State {};

struct StateTest {
    virtual bool Pass(const State *) const;
};

// total size: 0x4
struct FnAnimSuper {
    FnAnimSuper() {}

    virtual ~FnAnimSuper() {}
};

// total size: 0xC
class FnAnim : FnAnimSuper {
  public:
    FnAnim() {}

    // Overrides: FnAnimSuper
    ~FnAnim() override {}

    // AnimTypeId::Type GetType() const {}

    virtual unsigned short GetTargetCheckSum() const {}

    virtual void UseFPS(bool u) {}

    virtual void Eval(float previousTime, float currentTime, float *dofs) {}

    virtual bool GetLength(float &timeLength) const {}

    virtual bool FindMatchTime(const MatchPhaseInput &input, float &time) const {}

    virtual bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) {}

    virtual bool EvalPhase(float currentTime, PhaseValue &phase) {}

    virtual bool EvalVel2D(float currentTime, float *velocity) {}

    virtual bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) {}

    virtual bool EvalWeights(float currentTime, float *weights) {}

    virtual bool EvalState(float currentTime, State *s) {}

    virtual bool EvalPose(float currentTime, const PosePaletteBank *paletteBank, float *sqt) {}

    virtual bool FindTime(const StateTest &test, float startTime, float &resultTime) {}

    virtual const PhaseChan *GetPhaseChan() {}

    virtual const AttributeBlock *GetAttributes() const;

    template <typename T> bool GetAttribute(AttributeId id, T &result) const {
        const AttributeBlock *attribBlock = GetAttributes();
        if (attribBlock) {
            return attribBlock->GetAttribute(id, result);
        } else {
            return false;
        }
    }

    AnimStat *mStat; // offset 0x4, size 0x4

  protected:
    static bool gReverseDeltaSumEnabled; // size: 0x1, address: 0x80417118
    AnimTypeId::Type mType;              // offset 0x8, size 0x4
};

}; // namespace EAGL4Anim

#endif
