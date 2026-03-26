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
    int *sig = GetNonInterpSig();
    float *frameData = GetFrame(frame);
    int *sigEnd = sig + mSigSize;

    if (!boneMask) {
        while (sig < sigEnd) {
            int numChannels = *sig++;
            float *nextOutputPose = outputPose + 12;

            for (int ichan = 0; ichan < numChannels; ichan++) {
                reinterpret_cast<void (*)(float *&, float *)>(*sig++)(frameData, outputPose + 4);
            }
            outputPose = nextOutputPose;
        }
    } else {
        for (unsigned int ibone = 0; sig < sigEnd; ibone++) {
            int numChannels = *sig++;

            if (boneMask->GetBone(ibone)) {
                for (int ichan = 0; ichan < numChannels; ichan++) {
                    reinterpret_cast<void (*)(float *&, float *)>(*sig++)(frameData, outputPose + 4);
                }
            } else {
                for (int ichan = 0; ichan < numChannels; ichan++) {
                    void (*func)(float *&, float *) = reinterpret_cast<void (*)(float *&, float *)>(*sig++);

                    if (func == EulF3 || func == TranF3) {
                        frameData += 3;
                    } else if (func == QuatF4) {
                        frameData += 4;
                    }
                }
            }

            outputPose += 12;
        }
    }
}

}; // namespace EAGL4Anim
