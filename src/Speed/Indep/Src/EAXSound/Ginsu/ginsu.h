#ifndef EAXSOUND_GINSU_GINSU_H
#define EAXSOUND_GINSU_GINSU_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

class GinsuSynthData : public AudioMemBase {
  public:
    ~GinsuSynthData() override;

    float GetMinFrequency() const { return mMinFrequency; }
    float GetMaxFrequency() const { return mMaxFrequency; }
    int GetSampleRate() const { return mSampleRate; }

    void DecodeBlock(int block);
    GinsuSynthData();
    bool BindToData(void *ptr);
    int FrequencyToSample(float freq) const;
    int CycleToSample(float cycle) const;
    float CyclePeriod(float cycle) const;
    float SampleToCycle(int sample) const;
    bool GetSamples(int startSample, int numSamples, short *dest);

  private:
    float mMinFrequency;           // offset 0x4, size 0x4
    float mMaxFrequency;           // offset 0x8, size 0x4
    int mSegCount;                 // offset 0xC, size 0x4
    int mCycleCount;               // offset 0x10, size 0x4
    int mSampleCount;              // offset 0x14, size 0x4
    int mSampleRate;               // offset 0x18, size 0x4
    int *mFreqPos;                 // offset 0x1C, size 0x4
    int *mCyclePos;                // offset 0x20, size 0x4
    unsigned char *mSampleData;    // offset 0x24, size 0x4
    float mMinPeriod;              // offset 0x28, size 0x4
    float mSample[32];             // offset 0x2C, size 0x80
    int mCurrentBlock;             // offset 0xAC, size 0x4
};

class GinsuSynthesis : public AudioMemBase {
  public:
    static void PacketReleaseCallback(void *samples, void *clientdata);
    void HandlePacketRelease(short *samples);
    static int GetMemblockSize();
    GinsuSynthesis(void *memblock, int size);
    ~GinsuSynthesis() override;
    bool SetSynthData(GinsuSynthData &data);
    int StartSynthesis(float startFreq);
    bool UpdateFrequency(float targetFreq, float latency);
    float GetCurrentPitch();
    void StopSynthesis();

  private:
    short *mPacketData[2];         // offset 0x4, size 0x8
    int mMaxPacketSize;            // offset 0xC, size 0x4
    int mNoJumpSize;               // offset 0x10, size 0x4
    int mPacketSize;               // offset 0x14, size 0x4
    int mOverlapSize;              // offset 0x18, size 0x4
    int mPacketHandle;             // offset 0x1C, size 0x4
    int mSndHandle;                // offset 0x20, size 0x4
    int mSampleRate;               // offset 0x24, size 0x4
    GinsuSynthData *mSynthData;    // offset 0x28, size 0x4
    int mPlaybackPos;              // offset 0x2C, size 0x4
    int mCurrentPos;               // offset 0x30, size 0x4
    int mTargetPos;                // offset 0x34, size 0x4
    int mPacketCountdown;          // offset 0x38, size 0x4
    int mNoJumpRemaining;          // offset 0x3C, size 0x4
    float mCurrentCycle;           // offset 0x40, size 0x4
};

#endif
