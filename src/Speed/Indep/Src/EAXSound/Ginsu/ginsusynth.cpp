#include "ginsu.h"
#include "ginsuhelper.h"

#include <snd/sndo.h>

static const float NOJUMP_DURATION = 0.011f;   // size: 0x4, Decl: 17
static const float PACKET_DURATION = 0.011f;   // size: 0x4, Decl: 18
static const float OVERLAP_DURATION = 0.0005f; // size: 0x4, Decl: 19

void GinsuSynthesis::PacketReleaseCallback(void *samples, void *clientdata) {
    GinsuSynthesis *object = static_cast<GinsuSynthesis *>(clientdata);
    object->HandlePacketRelease(static_cast<short *>(samples));
}

// UNSOLVED, functional match
void GinsuSynthesis::HandlePacketRelease(short *samples) {
    if (this->mSynthData != nullptr) {
        this->mCurrentCycle = this->mSynthData->SampleToCycle(this->mCurrentPos);
    }

    float change = static_cast<float>(this->mTargetPos - this->mCurrentPos) / static_cast<float>(this->mPacketCountdown);
    float jumpTime = 0.0f;
    int jumpDist = 0;

    if (this->mNoJumpRemaining < this->mPacketSize && this->mSynthData != nullptr) {
        float playbackCycle = this->mSynthData->SampleToCycle(this->mPlaybackPos);
        float playbackRate = static_cast<float>(this->mPacketSize) / this->mSynthData->CyclePeriod(playbackCycle);
        float currentRate = change / this->mSynthData->CyclePeriod(this->mCurrentCycle);
        float posDiff = this->mCurrentCycle - playbackCycle;
        float rateDiff = currentRate - playbackRate;
        float nojumpTime = static_cast<float>(this->mNoJumpRemaining) / static_cast<float>(this->mPacketSize);
        float nojumpDist = posDiff + rateDiff * nojumpTime;
        float packetDist = posDiff + rateDiff;

        if (IntFloor(nojumpDist) != IntFloor(packetDist)) {
            if (nojumpDist < packetDist) {
                jumpDist = IntCeil(nojumpDist);
            } else {
                jumpDist = IntFloor(nojumpDist);
            }

            jumpTime = (static_cast<float>(jumpDist) - posDiff) / rateDiff;
        } else {
            float maxDist = static_cast<float>(IntCeil(fabsf(rateDiff * 0.99999994f)));

            if (fabsf(nojumpDist) > maxDist) {
                jumpTime = nojumpTime;

                if (nojumpDist < 0.0f) {
                    jumpDist = IntCeil(nojumpDist);
                } else {
                    jumpDist = IntFloor(nojumpDist);
                }
            }
        }
    }

    if (jumpDist == 0) {
        jumpTime = 1.0f;
    }

    int sampleCount = IntRound(jumpTime * static_cast<float>(this->mPacketSize));

    if (this->mSynthData != nullptr) {
        this->mSynthData->GetSamples(this->mPlaybackPos, sampleCount, samples);
    }

    this->mPlaybackPos += sampleCount;
    this->mNoJumpRemaining -= sampleCount;

    if (this->mNoJumpRemaining < 0) {
        this->mNoJumpRemaining = 0;
    }

    if (jumpDist != 0) {
        short *dest = samples + sampleCount;
        float cycle;
        short buff[256];

        if (this->mSynthData != nullptr) {
            this->mSynthData->GetSamples(this->mPlaybackPos, this->mOverlapSize, dest);

            cycle = this->mSynthData->SampleToCycle(this->mPlaybackPos);

            this->mPlaybackPos = this->mSynthData->CycleToSample(cycle + static_cast<float>(jumpDist));

            this->mSynthData->GetSamples(this->mPlaybackPos, this->mOverlapSize, buff);
        }

        float blend = 0.0f;
        float blendstep = 1.0f / static_cast<float>(this->mOverlapSize);

        for (int i = 0; i < this->mOverlapSize; i++) {
            float val = static_cast<float>(dest[i]) + blend * static_cast<float>(buff[i] - dest[i]);

            dest[i] = static_cast<short>(IntRound(val));
            blend += blendstep;
        }

        sampleCount += this->mOverlapSize;
        this->mPlaybackPos += this->mOverlapSize;
        int count = this->mPacketSize - sampleCount;
        this->mNoJumpRemaining = this->mNoJumpSize - this->mOverlapSize;

        if (count > 0) {
            if (this->mSynthData != nullptr) {
                this->mSynthData->GetSamples(this->mPlaybackPos, count, samples + sampleCount);
            }

            sampleCount = this->mPacketSize;
            this->mPlaybackPos += count;
            this->mNoJumpRemaining -= count;
        }
    }

    this->mCurrentPos = IntRound(static_cast<float>(this->mCurrentPos) + change);

    if (this->mPacketCountdown > 1) {
        this->mPacketCountdown--;
    }

    SNDPACKET sp;

    sp.numframes = sampleCount;
    sp.psamples[0] = samples;

    SNDSYS_entercritical();
    SNDPKTPLAY_submit(this->mPacketHandle, &sp);
    SNDSYS_leavecritical();
}

int GinsuSynthesis::GetMemblockSize() {
    int overhead = SNDPKTPLAY_overhead(8);
    int packetSize = IntCeil(2212.0f);

    return overhead + packetSize;
}

GinsuSynthesis::GinsuSynthesis(void *memblock, int size) {
    this->mPacketHandle = -1;
    this->mSampleRate = 0;
    this->mSynthData = nullptr;

    int overhead = SNDPKTPLAY_overhead(8);
    char *overheadMem;

    this->mMaxPacketSize = (static_cast<unsigned int>(size - overhead) >> 2) & 0x3ffffffe;

    if (this->mMaxPacketSize > 0x4f) {
        this->mPacketData[0] = static_cast<short *>(memblock);
        this->mPacketData[1] = static_cast<short *>(memblock) + this->mMaxPacketSize;

        overheadMem = static_cast<char *>(memblock) + this->mMaxPacketSize * 4;

        this->mPacketHandle = SNDPKTPLAY_create(PacketReleaseCallback, nullptr, this, overheadMem, overhead);
    }
}

GinsuSynthesis::~GinsuSynthesis() {
    this->StopSynthesis();
    SNDPKTPLAY_destroy(this->mPacketHandle);
}

bool GinsuSynthesis::SetSynthData(GinsuSynthData &data) {
    int samprate = data.GetSampleRate();

    if (samprate == 0) {
        return false;
    }

    if (this->mSampleRate > 0 && samprate != this->mSampleRate) {
        return false;
    }

    int nojumpsize = IntRound(static_cast<float>(samprate) * NOJUMP_DURATION);
    int packetsize = IntRound(static_cast<float>(samprate) * PACKET_DURATION);
    int overlapsize = IntRound(static_cast<float>(samprate) * OVERLAP_DURATION);

    if (packetsize + overlapsize > this->mMaxPacketSize) {
        return false;
    }

    SNDSYS_entercritical();

    this->mNoJumpSize = nojumpsize;
    this->mPacketSize = packetsize;
    this->mOverlapSize = overlapsize;
    this->mSynthData = &data;

    SNDSYS_leavecritical();

    return true;
}

int GinsuSynthesis::StartSynthesis(float startFreq) {
    if (this->mPacketHandle < 0 || this->mSynthData == nullptr) {
        return -1;
    }

    this->StopSynthesis();

    this->mPlaybackPos = this->mSynthData->FrequencyToSample(startFreq);
    this->mPacketCountdown = 1;
    this->mNoJumpRemaining = this->mNoJumpSize;
    this->mCurrentPos = this->mPlaybackPos;
    this->mTargetPos = this->mPlaybackPos;
    this->mCurrentCycle = this->mSynthData->SampleToCycle(this->mPlaybackPos);

    SNDSAMPLEFORMAT ssf;
    ssf.samplerate = static_cast<unsigned short>(this->mSynthData->GetSampleRate());
    ssf.samplerep = 7;
    ssf.channels = 1;

    SNDSAMPLEATTR ssa;
    SND_attrsetdef(&ssa);
    ssa.rendermode = 4;

    SNDPLAYOPTS spo;
    SNDplaysetdef(&spo);
    spo.vol = 0;

    SNDSYS_entercritical();

    this->mSndHandle = SNDPKTPLAY_start(this->mPacketHandle, &ssf, &ssa, &spo);

    if (this->mSndHandle >= 0) {
        bMemSet(this->mPacketData[0], 0, this->mPacketSize * 2);
        bMemSet(this->mPacketData[1], 0, this->mPacketSize * 2);

        SNDPACKET sp;

        this->mSampleRate = this->mSynthData->GetSampleRate();

        sp.numframes = this->mPacketSize;
        sp.psamples[0] = this->mPacketData[0];

        SNDPKTPLAY_submit(this->mPacketHandle, &sp);

        sp.psamples[0] = this->mPacketData[1];

        SNDPKTPLAY_submit(this->mPacketHandle, &sp);
    }

    SNDSYS_leavecritical();

    return this->mSndHandle;
}

bool GinsuSynthesis::UpdateFrequency(float targetFreq, float latency) {
    int sample = 0;

    if (this->mSynthData != nullptr) {
        sample = this->mSynthData->FrequencyToSample(targetFreq);
    }

    SNDSYS_entercritical();

    this->mTargetPos = sample;
    this->mPacketCountdown = IntFloor(latency * 0.09090909f) + 1;

    SNDSYS_leavecritical();

    return true;
}

float GinsuSynthesis::GetCurrentPitch() {
    if (this->mSampleRate == 0 || this->mSynthData == nullptr) {
        return 0.0f;
    }

    return static_cast<float>(this->mSampleRate) / this->mSynthData->CyclePeriod(this->mCurrentCycle + 0.5f);
}

void GinsuSynthesis::StopSynthesis() {
    if (this->mSampleRate != 0) {
        SNDSYS_entercritical();
        SNDPKTPLAY_stop(this->mPacketHandle);
        SNDSYS_leavecritical();

        this->mSampleRate = 0;
    }
}
