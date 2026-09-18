#ifndef EAGL4ANIM_RAWPOSECHANNEL_H
#define EAGL4ANIM_RAWPOSECHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "BoneMask.h"

namespace EAGL4Anim {

void EulF3(float *&data, float *output);
void QuatF4(float *&data, float *output);
void TranF3(float *&data, float *output);
void EulF3Interp(float w, float *&data0, float *&data1, float *output);
void QuatF4Interp(float w, float *&data0, float *&data1, float *output);
void TranF3Interp(float w, float *&data0, float *&data1, float *output);

// total size: 0x10
class RawPoseChannel : public AnimMemoryMap {
  public:
    enum ChannelType {
        QUAT = 0,
        EUL = 1,
        TRAN = 2,
    };

    int GetSigSize() const {
        return mSigSize;
    }

    void SetSigSize(int s) {
        mSigSize = s;
    }

    int GetFrameSize() const {
        return mFrameSize;
    }

    void SetFrameSize(int s) {
        mFrameSize = s;
    }

    int GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumFrames(int n) {
        mNumFrames = n;
    }

    int *GetNonInterpSig() {
        return reinterpret_cast<int *>(this + 1);
    }

    const int *GetNonInterpSig() const {
        return reinterpret_cast<const int *>(this + 1);
    }

    int *GetInterpSig() {
        return GetNonInterpSig() + mSigSize;
    }

    const int *GetInterpSig() const {
        return GetNonInterpSig() + mSigSize;
    }

    float *GetAnimData() {
        return reinterpret_cast<float *>(GetInterpSig() + mSigSize);
    }

    const float *GetAnimData() const {
        return reinterpret_cast<const float *>(GetInterpSig() + mSigSize);
    }

    float *GetFrame(int i) {
        return &GetAnimData()[i * mFrameSize];
    }

    const float *GetFrame(int i) const {
        return &GetAnimData()[i * mFrameSize];
    }

    int GetSize() const {}

    static int ComputeSize(int sigSize, int poseSize, int numFrames) {}

    int GetNumBones() const {}

    void EvalInterpFrame(float t, int frame0, int frame1, float *outputPose, const BoneMask *boneMask) {
        int count;
        int *s;
        float *d0;
        float *d1;
        float *out;
        int *end;
        void (*func)(float w, float *&data0, float *&data1, float *output);

        s = GetInterpSig();
        d0 = GetFrame(frame0);
        d1 = GetFrame(frame1);
        out = outputPose;
        end = reinterpret_cast<int *>(GetAnimData());

        if (!boneMask) {

            while (s < end) {

                int j;

                count = *s++;

                for (j = 0; j < count; j++) {
                    func = reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++);
                    func(t, d0, d1, &out[4]);
                }

                out += 12;
            }
        } else {

            int j;
            int i = 0;

            while (s < end) {

                count = *s++;

                if (boneMask->GetBone(i)) {

                    for (j = 0; j < count; j++) {
                        func = reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++);
                        func(t, d0, d1, &out[4]);
                    }
                } else {

                    for (j = 0; j < count; j++) {

                        func = reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++);

                        if (func == EulF3Interp || func == TranF3Interp) {

                            d0 += 3;
                            d1 += 3;

                        } else if (func == QuatF4Interp) {

                            d0 += 4;
                            d1 += 4;
                        }
                    }
                }

                out += 12;
                i++;
            }
        }
    }

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    void UnInit();

    void Eval(float frameTime, float *outputPose, bool interp, const BoneMask *boneMask);

    void EvalFrame(int frame, float *outputPose, const BoneMask *boneMask);

  private:
    int mSigSize;   // offset 0x4, size 0x4
    int mFrameSize; // offset 0x8, size 0x4
    int mNumFrames; // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
