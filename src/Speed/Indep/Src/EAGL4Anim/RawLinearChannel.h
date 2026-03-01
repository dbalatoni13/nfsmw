#ifndef EAGL4ANIM_RAWLINEARCHANNEL_H
#define EAGL4ANIM_RAWLINEARCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x8
class RawLinearChannel : public AnimMemoryMap {
  public:
    void SetNumDOFs(int n) {
        mNumDOFs = n;
    }

    int GetNumDOFs() const {
        return mNumDOFs;
    }

    int GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumFrames(int n) {
        mNumFrames = n;
    }

    unsigned short *GetDOFIndex() {}

    const unsigned short *GetDOFIndex() const {}

    int GetDOFIndexSize() const {}

    float *GetAnimData() {}

    const float *GetAnimData() const {}

    float *GetFrame(int i) {}

    const float *GetFrame(int i) const {}

    int GetSize() const {}

    static int ComputeSize(int numDOFs, int numFrames) {}

    void EvalInterpFrame(float t, int frame0, int frame1, float *output) {}

    void Eval(float frameTime, float *output, bool interp) {}

  private:
    void EvalFrame(int frame, float *output) {}

    unsigned short mNumDOFs;   // offset 0x4, size 0x2
    unsigned short mNumFrames; // offset 0x6, size 0x2
};

}; // namespace EAGL4Anim

#endif
