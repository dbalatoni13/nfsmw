#include "FnCycle.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

inline void FnCycle::Eval(float previousTime, float currentTime, float *dofs) {

    mpAnim->Eval(GetInRangeTime(previousTime), GetInRangeTime(currentTime), dofs);
}

inline bool FnCycle::EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) {

    return mpAnim->EvalEvent(GetInRangeTime(previousTime), GetInRangeTime(currentTime), eventHandlers, extraData);
}

inline bool FnCycle::EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) {

    return mpAnim->EvalSQT(GetInRangeTime(currentTime), sqt, boneMask);
}

inline bool FnCycle::EvalPhase(float currentTime, PhaseValue &phase) {

    return mpAnim->EvalPhase(GetInRangeTime(currentTime), phase);
}

}; // namespace EAGL4Anim
