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

    unsigned short *GetDOFIndex() {
        return reinterpret_cast<unsigned short *>(&this[1]);
    }

    const unsigned short *GetDOFIndex() const {
        return reinterpret_cast<const unsigned short *>(&this[1]);
    }

    int GetDOFIndexSize() const {
        return (mNumDOFs + 1) & ~1;
    }

    float *GetAnimData() {
        return reinterpret_cast<float *>(GetDOFIndex() + GetDOFIndexSize());
    }

    const float *GetAnimData() const {
        return reinterpret_cast<const float *>(GetDOFIndex() + GetDOFIndexSize());
    }

    float *GetFrame(int i) {
        return &GetAnimData()[i * mNumDOFs];
    }

    const float *GetFrame(int i) const {
        return &GetAnimData()[i * mNumDOFs];
    }

    int GetSize() const {
        return ComputeSize(mNumDOFs, mNumFrames);
    }

    static int ComputeSize(int numDOFs, int numFrames) {
        return sizeof(RawLinearChannel) + (((numDOFs + 1) & ~1) * sizeof(unsigned short)) + numFrames * numDOFs * sizeof(float);
    }

    void EvalInterpFrame(float t, int frame0, int frame1, float *output) {
        const unsigned short *dofIndex = GetDOFIndex();
        const float *frameData0 = GetFrame(frame0);
        const float *frameData1 = GetFrame(frame1);

        for (int i = 0; i < mNumDOFs; i++) {
            float value0 = frameData0[i];
            output[dofIndex[i]] = t * (frameData1[i] - value0) + value0;
        }
    }

  private:
    void EvalFrame(int frame, float *output) {
        const unsigned short *dofIndex = GetDOFIndex();
        const float *frameData = GetFrame(frame);

        for (int i = 0; i < mNumDOFs; i++) {
            output[dofIndex[i]] = frameData[i];
        }
    }

  public:
    void Eval(float frameTime, float *output, bool interp) {
        int frame = static_cast<int>(frameTime);

        if (frame < 0) {
            EvalFrame(0, output);
        } else {
            int lastFrame = mNumFrames - 1;

            if (frame < lastFrame) {
                float t = frameTime - static_cast<float>(frame);

                if (t != 0.0f && interp) {
                    EvalInterpFrame(t, frame, frame + 1, output);
                } else {
                    EvalFrame(frame, output);
                }
            } else {
                EvalFrame(lastFrame, output);
            }
        }
    }

    unsigned short mNumDOFs;   // offset 0x4, size 0x2
    unsigned short mNumFrames; // offset 0x6, size 0x2
};

}; // namespace EAGL4Anim

#endif
