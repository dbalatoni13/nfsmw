#include "Speed/Indep/Src/EAXSound/Ginsu/ginsu.h"
#include "Speed/Indep/Src/EAXSound/Ginsu/ginsuhelper.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
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

struct SNDSAMPLEATTR {
    short detune;
    signed char priority;
    signed char vol;
    signed char pan;
    signed char fxlevel0;
    signed char bendrange;
    unsigned char platformver;
    unsigned short rendermode;
    char padchar[2];
    unsigned short azimuth[6];
    void *ptsdata[6];
    int tsdatasize[6];
    void *puserdata[4];
    int userdatasize[4];
};

extern "C" int SNDPKTPLAY_create(void (*packetreleasecallback)(void *, void *), void *unknown, void *clientdata, void *pmem, int memsize);
extern "C" int SNDPKTPLAY_destroy(int packetinstancehandle);
extern "C" int SNDPKTPLAY_overhead(int packetcount);
extern "C" int SNDPKTPLAY_start(int packetinstancehandle, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDPLAYOPTS *pspo);
extern "C" int SNDPKTPLAY_stop(int packetinstancehandle);
extern "C" int SNDPKTPLAY_submit(int packetinstancehandle, SNDPACKET *psp);
extern "C" void SND_attrsetdef(SNDSAMPLEATTR *pssa);
extern "C" void SNDplaysetdef(SNDPLAYOPTS *pspo);

void GinsuSynthesis::PacketReleaseCallback(void *samples, void *clientdata) {
    GinsuSynthesis *object = static_cast<GinsuSynthesis *>(clientdata);
    object->HandlePacketRelease(static_cast<short *>(samples));
}

void GinsuSynthesis::HandlePacketRelease(short *samples) {
    float change;
    float jumpTime;
    int jumpDist;
    int sampleCount;

    if (mSynthData) {
        mCurrentCycle = mSynthData->SampleToCycle(mCurrentPos);
    }

    change = static_cast<float>(mTargetPos - mCurrentPos) / static_cast<float>(mPacketCountdown);
    jumpTime = 0.0f;
    jumpDist = 0;

    if (mNoJumpRemaining < mPacketSize && mSynthData) {
        float playbackCycle = mSynthData->SampleToCycle(mPlaybackPos);
        float playbackRate = static_cast<float>(mPacketSize) / mSynthData->CyclePeriod(playbackCycle);
        float currentRate = change / mSynthData->CyclePeriod(mCurrentCycle);
        float posDiff = mCurrentCycle - playbackCycle;
        float rateDiff = currentRate - playbackRate;
        float nojumpTime = static_cast<float>(mNoJumpRemaining) / static_cast<float>(mPacketSize);
        float nojumpDist = posDiff + rateDiff * nojumpTime;
        float packetDist = posDiff + rateDiff;

        if (IntFloor(nojumpDist) != IntFloor(packetDist)) {
            if (packetDist <= nojumpDist) {
                if (0.0f <= nojumpDist) {
                    jumpDist = static_cast<int>(nojumpDist);
                } else {
                    jumpDist = IntFloor(nojumpDist);
                }
            } else {
                if (nojumpDist <= 0.0f) {
                    jumpDist = static_cast<int>(nojumpDist);
                } else {
                    jumpDist = IntCeil(nojumpDist);
                }
            }
            jumpTime = (static_cast<float>(jumpDist) - posDiff) / rateDiff;
        } else {
            float maxDist = bAbs(rateDiff * 0.99999994f);

            if (static_cast<float>(IntCeil(maxDist)) < bAbs(nojumpDist)) {
                jumpTime = nojumpTime;
                if (0.0f <= nojumpDist) {
                    jumpDist = IntCeil(nojumpDist);
                } else {
                    jumpDist = static_cast<int>(nojumpDist);
                }
            }
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

GinsuSynthesis::GinsuSynthesis(void *memblock, int size) {
    int overhead;
    char *overheadMem;

    mPacketHandle = -1;
    mSynthData = 0;
    mSampleRate = 0;

    overhead = SNDPKTPLAY_overhead(8);
    mMaxPacketSize = (static_cast<unsigned int>(size - overhead) >> 2) & 0x3FFFFFFE;
    if (mMaxPacketSize > 0x4F) {
        mPacketData[0] = static_cast<short *>(memblock);
        mPacketData[1] = static_cast<short *>(memblock) + mMaxPacketSize;
        overheadMem = static_cast<char *>(memblock) + mMaxPacketSize * 4;
        mPacketHandle = SNDPKTPLAY_create(PacketReleaseCallback, 0, this, overheadMem, overhead);
    }
}

GinsuSynthesis::~GinsuSynthesis() {
    StopSynthesis();
    SNDPKTPLAY_destroy(mPacketHandle);
}

bool GinsuSynthesis::SetSynthData(GinsuSynthData &data) {
    int samprate;
    int nojumpsize;
    int packetsize;
    int overlapsize;

    samprate = data.GetSampleRate();
    if (samprate != 0) {
        if (mSampleRate > 0 && samprate != mSampleRate) {
            return false;
        }

        nojumpsize = IntRound(static_cast<float>(samprate) * 0.011f);
        packetsize = IntRound(static_cast<float>(samprate) * 0.011f);
        overlapsize = IntRound(static_cast<float>(samprate) * 0.0005f);
        if (packetsize + overlapsize <= mMaxPacketSize) {
            SNDSYS_entercritical();
            mSynthData = &data;
            mNoJumpSize = nojumpsize;
            mPacketSize = packetsize;
            mOverlapSize = overlapsize;
            SNDSYS_leavecritical();
            return true;
        }
    }

    return false;
}

int GinsuSynthesis::StartSynthesis(float startFreq) {
    SNDSAMPLEFORMAT ssf;
    SNDSAMPLEATTR ssa;
    SNDPLAYOPTS spo;

    if (mPacketHandle < 0 || !mSynthData) {
        return -1;
    }

    StopSynthesis();
    mPacketCountdown = 1;
    mNoJumpRemaining = mNoJumpSize;
    mPlaybackPos = mSynthData->FrequencyToSample(startFreq);
    mCurrentPos = mPlaybackPos;
    mTargetPos = mPlaybackPos;
    mCurrentCycle = mSynthData->SampleToCycle(mPlaybackPos);

    ssf.samplerate = static_cast<unsigned short>(mSynthData->GetSampleRate());
    ssf.samplerep = 7;
    ssf.channels = 1;

    SND_attrsetdef(&ssa);
    ssa.rendermode = 4;
    SNDplaysetdef(&spo);
    spo.vol = 0;

    SNDSYS_entercritical();
    mSndHandle = SNDPKTPLAY_start(mPacketHandle, &ssf, &ssa, &spo);
    if (mSndHandle > -1) {
        bMemSet(mPacketData[0], 0, mPacketSize * 2);
        bMemSet(mPacketData[1], 0, mPacketSize * 2);

        {
            SNDPACKET sp;

            sp.numframes = mPacketSize;
            sp.psamples[0] = mPacketData[0];
            mSampleRate = mSynthData->GetSampleRate();
            SNDPKTPLAY_submit(mPacketHandle, &sp);
            sp.psamples[0] = mPacketData[1];
            SNDPKTPLAY_submit(mPacketHandle, &sp);
        }
    }
    SNDSYS_leavecritical();

    return mSndHandle;
}

bool GinsuSynthesis::UpdateFrequency(float targetFreq, float latency) {
    int sample;

    sample = 0;
    if (mSynthData) {
        sample = mSynthData->FrequencyToSample(targetFreq);
    }

    SNDSYS_entercritical();
    mTargetPos = sample;
    mPacketCountdown = IntFloor(latency * 0.09090909f) + 1;
    SNDSYS_leavecritical();
    return true;
}

float GinsuSynthesis::GetCurrentPitch() {
    if (mSampleRate == 0 || !mSynthData) {
        return 0.0f;
    }

    return static_cast<float>(mSampleRate) / mSynthData->CyclePeriod(mCurrentCycle + 0.5f);
}

void GinsuSynthesis::StopSynthesis() {
    if (mSampleRate != 0) {
        SNDSYS_entercritical();
        SNDPKTPLAY_stop(mPacketHandle);
        SNDSYS_leavecritical();
        mSampleRate = 0;
    }
}
