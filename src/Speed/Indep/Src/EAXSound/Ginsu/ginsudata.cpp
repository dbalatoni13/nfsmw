#include "Speed/Indep/Src/EAXSound/Ginsu/ginsu.h"
#include "Speed/Indep/Src/EAXSound/Ginsu/ginsuhelper.h"

// total size: 0x24
struct GinsuDataLayout {
    char id[4];                // offset 0x0, size 0x4
    char ver[2];               // offset 0x4, size 0x2
    short flags;               // offset 0x6, size 0x2
    float minFrequency;        // offset 0x8, size 0x4
    float maxFrequency;        // offset 0xC, size 0x4
    int segCount;              // offset 0x10, size 0x4
    int cycleCount;            // offset 0x14, size 0x4
    int sampleCount;           // offset 0x18, size 0x4
    int sampleRate;            // offset 0x1C, size 0x4
    unsigned char data[1];     // offset 0x20, size 0x1
};

static void swapbytes(unsigned char *data) {
    unsigned char t1 = data[0];
    unsigned char t2 = data[1];

    data[0] = data[3];
    data[1] = data[2];
    data[2] = t2;
    data[3] = t1;
}

static void AdjustEndienness(GinsuDataLayout *memdata) {
    unsigned char *data = memdata->data;
    int count;

    swapbytes(reinterpret_cast<unsigned char *>(&memdata->minFrequency));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->maxFrequency));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->segCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->cycleCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->sampleCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->sampleRate));

    count = memdata->segCount + memdata->cycleCount + 2;
    if (count > 0) {
        int i = 0;
        do {
            i++;
            swapbytes(data);
            data += 4;
        } while (i < count);
    }

    memdata->flags = 1;
}

GinsuSynthData::GinsuSynthData()
    : mMinFrequency(0.0f), //
      mMaxFrequency(0.0f), //
      mSegCount(0), //
      mCycleCount(0), //
      mSampleCount(0), //
      mSampleRate(0) {}

bool GinsuSynthData::BindToData(void *ptr) {
    GinsuDataLayout *memdata = static_cast<GinsuDataLayout *>(ptr);
    int minperiod;

    mSampleRate = 0;
    mMaxFrequency = 0.0f;
    mMinFrequency = 0.0f;
    mSegCount = 0;
    mCycleCount = 0;
    mSampleCount = 0;

    if (*reinterpret_cast<unsigned int *>(memdata) == 0x476E7375 && memdata->ver[0] == '2') {
        if (memdata->flags == 0) {
            AdjustEndienness(memdata);
        }

        mMinFrequency = memdata->minFrequency;
        mMaxFrequency = memdata->maxFrequency;
        mSegCount = memdata->segCount;
        mCycleCount = memdata->cycleCount;
        minperiod = memdata->sampleCount;
        mSampleCount = minperiod;
        mSampleData = reinterpret_cast<unsigned char *>(reinterpret_cast<int *>(memdata->data) + mSegCount + mCycleCount + 2);
        mSampleRate = memdata->sampleRate;
        mCurrentBlock = -1;
        mFreqPos = reinterpret_cast<int *>(memdata->data);
        mCyclePos = mFreqPos + mSegCount + 1;

        if (mCycleCount > 0) {
            int i;
            for (i = 0; i < mCycleCount; i++) {
                int period = mCyclePos[i + 1] - mCyclePos[i];
                if (period < minperiod) {
                    minperiod = period;
                }
            }
        }

        mMinPeriod = static_cast<float>(minperiod);
        return true;
    }

    return false;
}

int GinsuSynthData::FrequencyToSample(float freq) const {
    float seg;
    int i;
    float a;
    int samp;

    if (mSegCount < 1) {
        samp = 0;
    } else if (mMinFrequency < freq) {
        if (freq < mMaxFrequency) {
            seg = (static_cast<float>(mSegCount) * (freq - mMinFrequency)) / (mMaxFrequency - mMinFrequency);
            i = IntFloor(seg);
            samp = mFreqPos[i];
            a = static_cast<float>(samp) + (seg - static_cast<float>(i)) * static_cast<float>(mFreqPos[i + 1] - samp);
            samp = IntRound(a);
        } else {
            samp = mFreqPos[mSegCount];
        }
    } else {
        samp = mFreqPos[0];
    }

    return samp;
}

int GinsuSynthData::CycleToSample(float cycle) const {
    int i;
    float a;
    int samp;

    if (mCycleCount < 1) {
        samp = 0;
    } else if (0.0f < cycle) {
        if (cycle < static_cast<float>(mCycleCount)) {
            i = IntFloor(cycle);
            samp = mCyclePos[i];
            a = static_cast<float>(samp) + (cycle - static_cast<float>(i)) * static_cast<float>(mCyclePos[i + 1] - samp);
            samp = IntRound(a);
        } else {
            samp = mCyclePos[mCycleCount];
        }
    } else {
        samp = mCyclePos[0];
    }

    return samp;
}

float GinsuSynthData::CyclePeriod(float cycle) const {
    float startPeriod;
    float endPeriod;
    int i;
    float a;

    if (mCycleCount < 1) {
        return 0.0f;
    }

    i = IntFloor(cycle);
    if (i < 1) {
        if (i < 0) {
            cycle = 0.0f;
            i = 0;
        }
        startPeriod = static_cast<float>(mCyclePos[1] - mCyclePos[0]);
        endPeriod = static_cast<float>(mCyclePos[2] - mCyclePos[0]) * 0.5f;
    } else if (i < mCycleCount - 1) {
        startPeriod = static_cast<float>(mCyclePos[i + 1] - mCyclePos[i - 1]) * 0.5f;
        endPeriod = static_cast<float>(mCyclePos[i + 2] - mCyclePos[i]) * 0.5f;
    } else {
        if (mCycleCount <= i) {
            cycle = static_cast<float>(mCycleCount);
            i = mCycleCount - 1;
        }
        startPeriod = static_cast<float>(mCyclePos[mCycleCount] - mCyclePos[mCycleCount - 2]) * 0.5f;
        endPeriod = static_cast<float>(mCyclePos[mCycleCount] - mCyclePos[mCycleCount - 1]);
    }

    a = cycle - static_cast<float>(i);
    return a * (endPeriod - startPeriod) + startPeriod;
}

float GinsuSynthData::SampleToCycle(int sample) const {
    int low;
    int high;
    int guess;
    float s1;
    float s2;
    float cycle = 0.0f;

    high = mCycleCount;
    if (high > 0 && mCyclePos[0] < sample) {
        if (sample < mCyclePos[high]) {
            low = 0;
            while (true) {
                while (true) {
                    cycle = (static_cast<float>(sample - mCyclePos[low]) / static_cast<float>(mCyclePos[high] - mCyclePos[low])) * static_cast<float>(high - low);
                    guess = low + IntFloor(cycle);
                    if (mCyclePos[guess] <= sample) {
                        break;
                    }
                    {
                        int newlow = guess - IntCeil(static_cast<float>(mCyclePos[guess] - sample) / mMinPeriod);
                        high = guess;
                        if (low < newlow) {
                            low = newlow;
                        }
                    }
                }

                if (sample < mCyclePos[guess + 1]) {
                    break;
                }

                {
                    int newhigh = guess + IntCeil(static_cast<float>(sample - mCyclePos[guess]) / mMinPeriod) + 1;
                    low = guess + 1;
                    if (newhigh < high) {
                        high = newhigh;
                    }
                }
            }

            s1 = static_cast<float>(mCyclePos[guess]);
            s2 = static_cast<float>(mCyclePos[guess + 1]);
            cycle = static_cast<float>(guess) + (static_cast<float>(sample) - s1) / (s2 - s1);
        } else {
            cycle = static_cast<float>(high);
        }
    }

    return cycle;
}

GinsuSynthData::~GinsuSynthData() {}
