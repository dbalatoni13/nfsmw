#ifndef EAGL4ANIM_FNANIM_H
#define EAGL4ANIM_FNANIM_H

#include "AnimTypeId.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttributeId.h"

#include <types.h>

namespace EAGL4Anim {

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

    // virtual unsigned short GetTargetCheckSum() const {}

    virtual void UseFPS(bool u) {}

    virtual void Eval(float previousTime, float currentTime, float *dofs) {}

    // virtual bool GetLength(float &timeLength) const {}

    // virtual bool FindMatchTime(const struct MatchPhaseInput &input, float &time) const {}

    // virtual bool EvalSQT(float currentTime, float *sqt, const struct BoneMask *boneMask) {}

    // virtual bool EvalPhase(float currentTime, struct PhaseValue &phase) {}

    // virtual bool EvalVel2D(float currentTime, float *velocity) {}

    // virtual bool EvalEvent(float previousTime, float currentTime, struct EventHandler **eventHandlers, void *extraData) {}

    // virtual bool EvalWeights(float currentTime, float *weights) {}

    // virtual bool EvalState(float currentTime, struct State *s) {}

    // virtual bool EvalPose(float currentTime, const struct PosePaletteBank *paletteBank, float *sqt) {}

    // virtual bool FindTime(const struct StateTest &test, float startTime, float &resultTime) {}

    // virtual const struct PhaseChan *GetPhaseChan() {}

    // bool GetAttribute(AttributeId id, float &result) const {}

    // bool GetAttribute(AttributeId id, unsigned short &result) const {}

    // bool GetAttribute(AttributeId id, short &result) const {}

    // bool GetAttribute(AttributeId id, int &result) const {}

    // bool GetAttribute(AttributeId id, unsigned int &result) const {}

    // bool GetAttribute(AttributeId id, char &result) const {}

    // bool GetAttribute(AttributeId id, unsigned char &result) const {}

    // bool GetAttribute(AttributeId id, char *&result) const {}

    // bool GetAttribute(AttributeId id, void *&result) const {}

    struct AnimStat *mStat; // offset 0x4, size 0x4

  protected:
    static bool gReverseDeltaSumEnabled; // size: 0x1, address: 0x80417118
    AnimTypeId::Type mType;              // offset 0x8, size 0x4
};

}; // namespace EAGL4Anim

#endif
