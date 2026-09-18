#ifndef EAGL4ANIM_RAWLINEARCHANNEL_H
#define EAGL4ANIM_RAWLINEARCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"

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
        return reinterpret_cast<unsigned short *>(this) + 4;
    }

    const unsigned short *GetDOFIndex() const {
        return reinterpret_cast<const unsigned short *>(this) + 4;
    }

    int GetDOFIndexSize() const {
        return (mNumDOFs + 1) * 2;
    }

    float *GetAnimData() {
        return reinterpret_cast<float *>(GetDOFIndex()) + static_cast<unsigned int>(mNumDOFs + 1) / 2;
    }

    const float *GetAnimData() const {
        return reinterpret_cast<const float *>(GetDOFIndex()) + static_cast<unsigned int>(mNumDOFs + 1) / 2;
    }

    float *GetFrame(int i) {
        return GetAnimData() + i * mNumDOFs;
    }

    const float *GetFrame(int i) const {
        return GetAnimData() + i * mNumDOFs;
    }

    int GetSize() const {
        return ComputeSize(mNumDOFs, mNumFrames);
    }

    static int ComputeSize(int numDOFs, int numFrames) {
        return 0x8 + (numDOFs + 1) * 2 + numDOFs * numFrames * 4;
    }

    void EvalFrame(int frame, float *output) {
        unsigned short *dofIdx = GetDOFIndex();
        float *frameData = GetFrame(frame);

        int i = 0;
        if (i < GetNumDOFs()) {
            do {
                output[dofIdx[i]] = frameData[i];
                i++;
            } while (i < GetNumDOFs());
        }
    }

    void EvalInterpFrame(float t, int frame0, int frame1, float *output) {
        unsigned short *dofIdx = GetDOFIndex();
        float *frameData0 = GetFrame(frame0);
        float *frameData1 = GetFrame(frame1);

        int i = 0;
        if (i < GetNumDOFs()) {
            do {
                output[dofIdx[i]] = t * (frameData1[i] - frameData0[i]) + frameData0[i];
                i++;
            } while (i < GetNumDOFs());
        }
    }

    void Eval(float frameTime, float *output, int interp) {

        int frame = FloatToInt(frameTime);

        if (frame < 0) {

            EvalFrame(0, output);

        } else if (frame >= GetNumFrames() - 1) {

            EvalFrame(GetNumFrames() - 1, output);

        } else {

            float t = frameTime - frame;

            if (t != 0.0f && interp) {

                EvalInterpFrame(t, frame, frame + 1, output);

            } else {

                EvalFrame(frame, output);
            }
        }
    }

  private:
    unsigned short mNumDOFs;   // offset 0x4, size 0x2
    unsigned short mNumFrames; // offset 0x6, size 0x2
};

}; // namespace EAGL4Anim

#endif
