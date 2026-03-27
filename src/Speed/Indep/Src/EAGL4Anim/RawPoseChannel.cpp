#include "RawPoseChannel.h"

#include <stdio.h>

namespace EAGL4Anim {

float qt0[7];

void RawPoseChannel::InitAnimMemoryMap(AnimMemoryMap *anim) {
    RawPoseChannel *rawChan = reinterpret_cast<RawPoseChannel *>(anim);
    int i = 0;
    int j;
    int count;
    int *sig = rawChan->GetNonInterpSig();
    int sigSize = rawChan->GetSigSize();

    while (i < sigSize) {
        count = *sig++;
        i++;

        for (j = 0; j < count; j++) {
            int t = *sig;

            i++;
            switch (t) {
            case EUL:
                *sig = reinterpret_cast<int>(EulF3);
                break;
            case QUAT:
                *sig = reinterpret_cast<int>(QuatF4);
                break;
            case TRAN:
                *sig = reinterpret_cast<int>(TranF3);
                break;
            default:
                printf("Bad signature channel type\n");
                break;
            }
            sig++;
        }
    }

    sig = rawChan->GetInterpSig();
    i = 0;
    while (i < sigSize) {
        count = *sig++;
        i++;

        for (j = 0; j < count; j++) {
            int t = *sig;

            i++;
            switch (t) {
            case EUL:
                *sig = reinterpret_cast<int>(EulF3Interp);
                break;
            case QUAT:
                *sig = reinterpret_cast<int>(QuatF4Interp);
                break;
            case TRAN:
                *sig = reinterpret_cast<int>(TranF3Interp);
                break;
            default:
                printf("Bad signature channel type\n");
                break;
            }
            sig++;
        }
    }
}

void RawPoseChannel::Eval(float frameTime, float *outputPose, bool interp, const BoneMask *boneMask) {
    int frame = static_cast<int>(frameTime);

    if (frame < 0) {
        EvalFrame(0, outputPose, boneMask);
    } else {
        int lastFrame = mNumFrames - 1;

        if (frame >= lastFrame) {
            EvalFrame(lastFrame, outputPose, boneMask);
            } else {
                float t = frameTime - static_cast<float>(frame);

                if (t != 0.0f && interp) {
                    EvalInterpFrame(t, frame, frame + 1, outputPose, boneMask);
                } else {
                    EvalFrame(frame, outputPose, boneMask);
                }
        }
    }
}

void RawPoseChannel::EvalFrame(int frame, float *outputPose, const BoneMask *boneMask) {
    int count;
    int *s = GetNonInterpSig();
    float *d = GetFrame(frame);
    float *out = outputPose;
    int *end = s + GetSigSize();
    void (*func)(float *&, float *);
    int j;

    if (!boneMask) {
        while (s < end) {
            count = *s++;

            for (j = 0; j < count; j++) {
                func = reinterpret_cast<void (*)(float *&, float *)>(*s++);
                func(d, out + 4);
            }
            out += 12;
        }
    } else {
        for (int i = 0; s < end; i++) {
            count = *s++;

            if (boneMask->GetBone(i)) {
                for (j = 0; j < count; j++) {
                    func = reinterpret_cast<void (*)(float *&, float *)>(*s++);
                    func(d, out + 4);
                }
            } else {
                for (j = 0; j < count; j++) {
                    func = reinterpret_cast<void (*)(float *&, float *)>(*s++);
                    if (func == EulF3 || func == TranF3) {
                        d += 3;
                    } else if (func == QuatF4) {
                        d += 4;
                    }
                }
            }

            out += 12;
        }
    }
}

}; // namespace EAGL4Anim
