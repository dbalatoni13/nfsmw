#include "RawPoseChannel.h"

#include <stdio.h>

namespace EAGL4Anim {

void QuatF4(float *&data, float *output);
void EulF3(float *&data, float *output);
void TranF3(float *&data, float *output);
void QuatF4Interp(float w, float *&data0, float *&data1, float *output);
void EulF3Interp(float w, float *&data0, float *&data1, float *output);
void TranF3Interp(float w, float *&data0, float *&data1, float *output);

void RawPoseChannel::InitAnimMemoryMap(AnimMemoryMap *anim) {
    RawPoseChannel *rawPoseChannel = reinterpret_cast<RawPoseChannel *>(anim);
    int numSigs = rawPoseChannel->mSigSize;
    int *sig = rawPoseChannel->GetNonInterpSig();

    int isig = 0;
    while (isig < numSigs) {
        int numChannels = *sig++;
        isig++;

        for (int ichan = 0; ichan < numChannels; ichan++) {
            switch (*sig) {
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

    sig = rawPoseChannel->GetInterpSig();
    isig = 0;
    while (isig < numSigs) {
        int numChannels = *sig++;
        isig++;

        for (int ichan = 0; ichan < numChannels; ichan++) {
            switch (*sig) {
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
                int *sig = GetInterpSig();
                int *sigEnd = sig + mSigSize;
                float *data0 = GetFrame(frame);
                float *data1 = GetFrame(frame + 1);

                if (!boneMask) {
                    while (sig < sigEnd) {
                        int numChannels = *sig++;
                        float *nextOutputPose = outputPose + 12;

                        for (int ichan = 0; ichan < numChannels; ichan++) {
                            reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*sig++)(t, data0, data1,
                                                                                                  outputPose + 4);
                        }
                        outputPose = nextOutputPose;
                    }
                } else {
                    for (unsigned int ibone = 0; sig < sigEnd; ibone++) {
                        int numChannels = *sig++;

                        if (boneMask->GetBone(ibone)) {
                            for (int ichan = 0; ichan < numChannels; ichan++) {
                                reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*sig++)(
                                    t, data0, data1, outputPose + 4);
                            }
                        } else {
                            for (int ichan = 0; ichan < numChannels; ichan++) {
                                void (*func)(float, float *&, float *&, float *) =
                                    reinterpret_cast<void (*)(float, float *&, float *&, float *)>(*sig++);

                                if (func == EulF3Interp || func == TranF3Interp) {
                                    data0 += 3;
                                    data1 += 3;
                                } else if (func == QuatF4Interp) {
                                    data0 += 4;
                                    data1 += 4;
                                }
                            }
                        }

                        outputPose += 12;
                    }
                }
            } else {
                EvalFrame(frame, outputPose, boneMask);
            }
        }
    }
}

void RawPoseChannel::EvalFrame(int frame, float *outputPose, const BoneMask *boneMask) {
    int *sig = GetNonInterpSig();
    int *sigEnd = sig + mSigSize;
    float *frameData = GetFrame(frame);

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
