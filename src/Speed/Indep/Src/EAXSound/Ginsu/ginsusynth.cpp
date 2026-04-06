#include "Speed/Indep/Src/EAXSound/Ginsu/ginsu.h"
#include "Speed/Indep/Src/EAXSound/Ginsu/ginsuhelper.h"
#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

struct SNDSAMPLEFORMAT {
    unsigned short samplerate;
    unsigned char channels;
    unsigned char samplerep;
};

struct SNDPACKET {
    SNDSAMPLEFORMAT ssf;
    unsigned int numframes : 31;
    unsigned int continuation : 1;
    int sizeofsamples;
    void *psamples[6];
};

extern "C" {
int SNDPKTPLAY_overhead(int packetcount);
int SNDPKTPLAY_submit(int packetinstancehandle, SNDPACKET *psp);
}

void GinsuSynthesis::PacketReleaseCallback(void *samples, void *clientdata) {
    GinsuSynthesis *object = static_cast<GinsuSynthesis *>(clientdata);
    object->HandlePacketRelease(static_cast<short *>(samples));
}

void GinsuSynthesis::HandlePacketRelease(short *samples) {
    float change;
    float jumpTime;
    int jumpDist;
    int sampleCount;

    jumpTime = 0.0f;
    jumpDist = 0;

    if (mSynthData) {
        mCurrentCycle = mSynthData->SampleToCycle(mCurrentPos);
    }

    change = static_cast<float>(mTargetPos - mCurrentPos) / static_cast<float>(mPacketCountdown);

    if (mNoJumpRemaining < mPacketSize && mSynthData) {
        float playbackCycle = mSynthData->SampleToCycle(mPlaybackPos);
        float playbackRate = static_cast<float>(mPacketSize) / mSynthData->CyclePeriod(playbackCycle);
        float currentRate = change / mSynthData->CyclePeriod(mCurrentCycle);
        float posDiff = mCurrentCycle - playbackCycle;
        float rateDiff = currentRate - playbackRate;
        float nojumpTime = static_cast<float>(mNoJumpRemaining) / static_cast<float>(mPacketSize);
        float nojumpDist = posDiff + rateDiff * nojumpTime;
        float packetDist = posDiff + rateDiff;

        if (IntFloor(nojumpDist) == IntFloor(packetDist)) {
            float maxDist = rateDiff * 0.99999994f;
            if (maxDist < 0.0f) {
                maxDist = -maxDist;
            }
            if (static_cast<float>(IntCeil(maxDist)) < (nojumpDist < 0.0f ? -nojumpDist : nojumpDist)) {
                jumpTime = nojumpTime;
                jumpDist = static_cast<int>(nojumpDist);
            }
        } else {
            if (nojumpDist < packetDist) {
                jumpDist = IntCeil(nojumpDist);
            } else {
                jumpDist = IntFloor(nojumpDist);
            }
            jumpTime = (static_cast<float>(jumpDist) - posDiff) / rateDiff;
        }
    }

    if (jumpDist == 0) {
        jumpTime = 1.0f;
    }

    sampleCount = IntRound(jumpTime * static_cast<float>(mPacketSize));

    if (mSynthData) {
        mSynthData->GetSamples(mPlaybackPos, sampleCount, samples);
    }

    mPlaybackPos += sampleCount;
    mNoJumpRemaining -= sampleCount;
    if (mNoJumpRemaining < 0) {
        mNoJumpRemaining = 0;
    }

    if (jumpDist != 0) {
        short *dest = samples + sampleCount;
        float cycle;
        short buff[256];
        float blend;
        float blendstep;
        int count;

        if (mSynthData) {
            mSynthData->GetSamples(mPlaybackPos, mOverlapSize, dest);
            cycle = mSynthData->SampleToCycle(mPlaybackPos);
            mPlaybackPos = mSynthData->CycleToSample(cycle + static_cast<float>(jumpDist));
            mSynthData->GetSamples(mPlaybackPos, mOverlapSize, buff);
        }

        blend = 0.0f;
        blendstep = 1.0f / static_cast<float>(mOverlapSize);

        if (mOverlapSize > 0) {
            int i;

            for (i = 0; i < mOverlapSize; i++) {
                float val = static_cast<float>(dest[i]) + blend * static_cast<float>(buff[i] - dest[i]);
                dest[i] = static_cast<short>(IntRound(val));
                blend += blendstep;
            }
        }

        sampleCount += mOverlapSize;
        mPlaybackPos += mOverlapSize;
        count = mPacketSize - sampleCount;
        mNoJumpRemaining = mNoJumpSize - mOverlapSize;

        if (count > 0) {
            if (mSynthData) {
                mSynthData->GetSamples(mPlaybackPos, count, samples + sampleCount);
            }

            sampleCount = mPacketSize;
            mPlaybackPos += count;
            mNoJumpRemaining -= count;
        }
    }

    mCurrentPos = IntRound(static_cast<float>(mCurrentPos) + change);

    if (mPacketCountdown > 1) {
        mPacketCountdown--;
    }

    SNDPACKET sp;

    sp.numframes = sampleCount;
    sp.psamples[0] = samples;

    SNDSYS_entercritical();
    SNDPKTPLAY_submit(mPacketHandle, &sp);
    SNDSYS_leavecritical();
}

int GinsuSynthesis::GetMemblockSize() {
    int overhead = SNDPKTPLAY_overhead(8);
    int packetSize = IntCeil(2212.0f);

    return overhead + packetSize;
}
