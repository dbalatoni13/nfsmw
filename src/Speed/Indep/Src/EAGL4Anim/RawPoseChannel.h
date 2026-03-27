#ifndef EAGL4ANIM_RAWPOSECHANNEL_H
#define EAGL4ANIM_RAWPOSECHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimUtil.h"
#include "AnimMemoryMap.h"
#include "BoneMask.h"

namespace EAGL4Anim {

extern float qt0[7];

inline float DegToRad(float deg) {
    return deg * 0.017453294f;
}

inline void QuatF4(float *&data, float *output) {
    output[0] = *data++;
    output[1] = *data++;
    output[2] = *data++;
    output[3] = *data++;
}

inline void EulF3(float *&data, float *output) {
    float eulData[3];

    eulData[0] = DegToRad(*data++);
    eulData[1] = DegToRad(*data++);
    eulData[2] = DegToRad(*data++);
    EulToQuat(eulData, output);
}

inline void TranF3(float *&data, float *output) {
    output[4] = *data++;
    output[5] = *data++;
    output[6] = *data++;
}

inline void QuatBlendF4(float w, const float *d0, const float *d1, float *out) {
    FastQuatBlendF4(w, d0, d1, out);
}

inline void QuatF4Interp(float w, float *&data0, float *&data1, float *output) {
    QuatF4(data0, qt0);
    QuatF4(data1, output);
    QuatBlendF4(w, qt0, output, output);
}

inline void EulF3Interp(float w, float *&data0, float *&data1, float *output) {
    EulF3(data0, qt0);
    EulF3(data1, output);
    QuatBlendF4(w, qt0, output, output);
}

inline void TranF3Interp(float w, float *&data0, float *&data1, float *output) {
    TranF3(data0, qt0);
    TranF3(data1, output);
    output[4] = qt0[4] + w * (output[4] - qt0[4]);
    output[5] = qt0[5] + w * (output[5] - qt0[5]);
    output[6] = qt0[6] + w * (output[6] - qt0[6]);
}

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

    void SetSigSize(int s) {}

    int GetFrameSize() const {
        return mFrameSize;
    }

    void SetFrameSize(int s) {}

    int GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumFrames(int n) {}

    int *GetNonInterpSig() {
        return reinterpret_cast<int *>(&this[1]);
    }

    const int *GetNonInterpSig() const {
        return reinterpret_cast<const int *>(&this[1]);
    }

    int *GetInterpSig() {
        return GetNonInterpSig() + GetSigSize();
    }

    const int *GetInterpSig() const {
        return GetNonInterpSig() + GetSigSize();
    }

    float *GetAnimData() {
        return reinterpret_cast<float *>(GetInterpSig() + GetSigSize());
    }

    const float *GetAnimData() const {
        return reinterpret_cast<const float *>(GetInterpSig() + GetSigSize());
    }

    float *GetFrame(int i) {
        return &GetAnimData()[i * GetFrameSize()];
    }

    const float *GetFrame(int i) const {
        return &GetAnimData()[i * GetFrameSize()];
    }

    int GetSize() const {}

    static int ComputeSize(int sigSize, int poseSize, int numFrames) {}

    int GetNumBones() const {}

    void EvalInterpFrame(float t, int frame0, int frame1, float *outputPose, const BoneMask *boneMask) {
        int *s = GetInterpSig();
        float *d0 = GetFrame(frame0);
        float *d1 = GetFrame(frame1);
        int *end = s + mSigSize;

        if (!boneMask) {
            while (s < end) {
                int count = *s++;
                float *out = outputPose + 12;

                for (int j = 0; j < count; j++) {
                    reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++)(t, d0, d1, outputPose + 4);
                }
                outputPose = out;
            }
        } else {
            for (int i = 0; s < end; i++) {
                int count = *s++;

                if (boneMask->GetBone(i)) {
                    for (int j = 0; j < count; j++) {
                        reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++)(t, d0, d1, outputPose + 4);
                    }
                } else {
                    for (int j = 0; j < count; j++) {
                        void (*func)(float, float *&, float *&, float *) =
                            reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*s++);

                        if (func == EulF3Interp || func == TranF3Interp) {
                            d0 += 3;
                            d1 += 3;
                        } else if (func == QuatF4Interp) {
                            d0 += 4;
                            d1 += 4;
                        }
                    }
                }

                outputPose += 12;
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
