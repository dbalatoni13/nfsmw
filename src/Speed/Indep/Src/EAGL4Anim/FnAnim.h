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
    FnAnim() : mStat(nullptr) {}

    // Overrides: FnAnimSuper
    ~FnAnim() override {}

    // AnimTypeId::Type GetType() const {}

    static bool IsReverseDeltaSumEnabled() {
        return gReverseDeltaSumEnabled;
    }

    virtual unsigned short GetTargetCheckSum() const {
        return 0;
    }

    virtual void UseFPS(bool u) {}

    virtual void Eval(float previousTime, float currentTime, float *dofs) {}

    virtual bool GetLength(float &timeLength) const {
        return false;
    }

    virtual bool FindMatchTime(const MatchPhaseInput &input, float &time) const {
        return false;
    }

    virtual bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) {
        return false;
    }

    virtual bool EvalPhase(float currentTime, PhaseValue &phase) {
        return false;
    }

    virtual bool EvalVel2D(float currentTime, float *velocity) {
        return false;
    }

    virtual bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) {
        return false;
    }

    virtual bool EvalWeights(float currentTime, float *weights) {
        return false;
    }

    virtual bool EvalState(float currentTime, State *s) {
        return false;
    }

    virtual bool EvalPose(float currentTime, const PosePaletteBank *paletteBank, float *sqt) {
        return false;
    }

    virtual bool FindTime(const StateTest &test, float startTime, float &resultTime) {
        return false;
    }

    virtual const PhaseChan *GetPhaseChan() {
        return nullptr;
    }

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
