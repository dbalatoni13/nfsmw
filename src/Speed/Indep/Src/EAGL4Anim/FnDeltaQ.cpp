#include "FnDeltaQ.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

FnDeltaQ::FnDeltaQ()
    : mPrevKey(-1),            //
      mConstPhysical(nullptr), //
      mMinRanges(nullptr),     //
      mBins(nullptr),          //
      mBinSize(-1),            //
      mPrevQBlock(nullptr),    //
      mPrevQs(nullptr),        //
      mConstBoneIdxs(nullptr) {
    mType = AnimTypeId::ANIM_DELTAQ;
}

void FnDeltaQ::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQTMasked(currTime, nullptr, sqt);
}

bool FnDeltaQ::EvalWeights(float currTime, float *weights) {
    Eval(currTime, currTime, weights);
    return true;
}

bool FnDeltaQ::EvalVel2D(float currTime, float *vel) {
    Eval(currTime, currTime, vel);
    return true;
}

}; // namespace EAGL4Anim
