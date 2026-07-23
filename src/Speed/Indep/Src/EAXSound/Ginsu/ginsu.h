//
//
#ifndef GINSU_H
#define GINSU_H

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

// total size: 0xB0
// Decl: 8
class GinsuSynthData : public AudioMemBase {
  public:
    GinsuSynthData();

    // Overrides: AudioMemBase
    ~GinsuSynthData() override {} // Decl: 11

    bool BindToData(void *ptr);

    float GetMinFrequency() const {
        return this->mMinFrequency;
    } // Decl: 15
    float GetMaxFrequency() const {
        return this->mMaxFrequency;
    } // Decl: 16
    int GetSampleRate() const {
        return this->mSampleRate;
    } // Decl: 17

    int FrequencyToSample(float freq) const;
    int CycleToSample(float cycle) const;
    float CyclePeriod(float cycle) const;
    float SampleToCycle(int sample) const;
    bool GetSamples(int startSample, int numSamples, short int *dest);

  private:
    void DecodeBlock(int block);

    float mMinFrequency; // offset 0x4, size 0x4, Decl: 31
    float mMaxFrequency; // offset 0x8, size 0x4, Decl: 32
    int mSegCount;       // offset 0xC, size 0x4, Decl: 33

    int mCycleCount;  // offset 0x10, size 0x4, Decl: 35
    int mSampleCount; // offset 0x14, size 0x4, Decl: 36
    int mSampleRate;  // offset 0x18, size 0x4, Decl: 37
    int *mFreqPos;    // offset 0x1C, size 0x4, Decl: 38

    int *mCyclePos;             // offset 0x20, size 0x4, Decl: 40
    unsigned char *mSampleData; // offset 0x24, size 0x4, Decl: 41
    float mMinPeriod;           // offset 0x28, size 0x4, Decl: 42

    float mSample[32]; // offset 0x2C, size 0x80
    int mCurrentBlock; // offset 0xAC, size 0x4
};

// total size: 0x44
// Decl: 52
class GinsuSynthesis : public AudioMemBase {
  public:
    GinsuSynthesis(void *memblock, int size); // Decl: 68

    static int GetMemblockSize();

    // Overrides: AudioMemBase
    ~GinsuSynthesis() override;

    bool SetSynthData(GinsuSynthData &data);
    int StartSynthesis(float startFreq);
    bool UpdateFrequency(float targetFreq, float latency);
    float GetCurrentPitch();
    void StopSynthesis();

  private:
    static void PacketReleaseCallback(void *samples, void *clientdata);
    void HandlePacketRelease(short int *samples);

    short *mPacketData[2];      // offset 0x4, size 0x8, Decl: 73
    int mMaxPacketSize;         // offset 0xC, size 0x4, Decl: 74
    int mNoJumpSize;            // offset 0x10, size 0x4, Decl: 75
    int mPacketSize;            // offset 0x14, size 0x4, Decl: 76
    int mOverlapSize;           // offset 0x18, size 0x4, Decl: 77
    int mPacketHandle;          // offset 0x1C, size 0x4, Decl: 78
    int mSndHandle;             // offset 0x20, size 0x4, Decl: 79
    int mSampleRate;            // offset 0x24, size 0x4, Decl: 80
    GinsuSynthData *mSynthData; // offset 0x28, size 0x4, Decl: 82
    int mPlaybackPos;           // offset 0x2C, size 0x4, Decl: 84
    int mCurrentPos;            // offset 0x30, size 0x4, Decl: 85
    int mTargetPos;             // offset 0x34, size 0x4, Decl: 86
    int mPacketCountdown;       // offset 0x38, size 0x4, Decl: 87
    int mNoJumpRemaining;       // offset 0x3C, size 0x4, Decl: 88
    float mCurrentCycle;        // offset 0x40, size 0x4, Decl: 89
};

#endif
