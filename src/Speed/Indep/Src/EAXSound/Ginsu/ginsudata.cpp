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

static float xafilterf[2][4] = {
    { 0.0f, 0.9375f, 1.796875f, 1.53125f },
    { 0.0f, 0.0f, -0.8125f, -0.859375f },
};

static float xatablef[16][16] = {
    { 0.0f, 4096.0f, 8192.0f, 12288.0f, 16384.0f, 20480.0f, 24576.0f, 28672.0f, -32768.0f, -28672.0f, -24576.0f, -20480.0f, -16384.0f, -12288.0f, -8192.0f, -4096.0f },
    { 0.0f, 2048.0f, 4096.0f, 6144.0f, 8192.0f, 10240.0f, 12288.0f, 14336.0f, -16384.0f, -14336.0f, -12288.0f, -10240.0f, -8192.0f, -6144.0f, -4096.0f, -2048.0f },
    { 0.0f, 1024.0f, 2048.0f, 3072.0f, 4096.0f, 5120.0f, 6144.0f, 7168.0f, -8192.0f, -7168.0f, -6144.0f, -5120.0f, -4096.0f, -3072.0f, -2048.0f, -1024.0f },
    { 0.0f, 512.0f, 1024.0f, 1536.0f, 2048.0f, 2560.0f, 3072.0f, 3584.0f, -4096.0f, -3584.0f, -3072.0f, -2560.0f, -2048.0f, -1536.0f, -1024.0f, -512.0f },
    { 0.0f, 256.0f, 512.0f, 768.0f, 1024.0f, 1280.0f, 1536.0f, 1792.0f, -2048.0f, -1792.0f, -1536.0f, -1280.0f, -1024.0f, -768.0f, -512.0f, -256.0f },
    { 0.0f, 128.0f, 256.0f, 384.0f, 512.0f, 640.0f, 768.0f, 896.0f, -1024.0f, -896.0f, -768.0f, -640.0f, -512.0f, -384.0f, -256.0f, -128.0f },
    { 0.0f, 64.0f, 128.0f, 192.0f, 256.0f, 320.0f, 384.0f, 448.0f, -512.0f, -448.0f, -384.0f, -320.0f, -256.0f, -192.0f, -128.0f, -64.0f },
    { 0.0f, 32.0f, 64.0f, 96.0f, 128.0f, 160.0f, 192.0f, 224.0f, -256.0f, -224.0f, -192.0f, -160.0f, -128.0f, -96.0f, -64.0f, -32.0f },
    { 0.0f, 16.0f, 32.0f, 48.0f, 64.0f, 80.0f, 96.0f, 112.0f, -128.0f, -112.0f, -96.0f, -80.0f, -64.0f, -48.0f, -32.0f, -16.0f },
    { 0.0f, 8.0f, 16.0f, 24.0f, 32.0f, 40.0f, 48.0f, 56.0f, -64.0f, -56.0f, -48.0f, -40.0f, -32.0f, -24.0f, -16.0f, -8.0f },
    { 0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f, 28.0f, -32.0f, -28.0f, -24.0f, -20.0f, -16.0f, -12.0f, -8.0f, -4.0f },
    { 0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, -16.0f, -14.0f, -12.0f, -10.0f, -8.0f, -6.0f, -4.0f, -2.0f },
    { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, -8.0f, -7.0f, -6.0f, -5.0f, -4.0f, -3.0f, -2.0f, -1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
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

void GinsuSynthData::DecodeBlock(int block) {
    unsigned char *src = mSampleData + block * 0x13;
    float *out = mSample;
    int filt;
    float f0;
    float f1;
    int shift;

    mCurrentBlock = block;
    mSample[0] = static_cast<float>(static_cast<int>((src[0] & 0xF0) + static_cast<char>(src[1]) * 0x100));
    mSample[1] = static_cast<float>(static_cast<int>((src[2] & 0xF0) + static_cast<char>(src[3]) * 0x100));

    filt = src[0] & 0xF;
    shift = src[2] & 0xF;
    f0 = xafilterf[1][filt];
    f1 = xafilterf[0][filt];

    {
        int i;
        for (i = 0; i < 0xF; i++) {
            int s0;
            int s1;

            out += 2;
            s0 = src[i + 4] >> 4;
            s1 = src[i + 4] & 0xF;
            *out = xatablef[shift][s0] + f1 * out[-1] + f0 * out[-2];
            out[1] = xatablef[shift][s1] + f1 * out[0] + f0 * out[-1];
        }
    }
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

    i = mSegCount;
    if (i < 1) {
        return 0;
    }

    seg = mMinFrequency;
    if (freq <= seg) {
        return mFreqPos[0];
    }

    if (freq < mMaxFrequency) {
        seg = (static_cast<float>(i) * (freq - seg)) / (mMaxFrequency - seg);
        i = IntFloor(seg);
        samp = mFreqPos[i];
        a = static_cast<float>(samp) + (seg - static_cast<float>(i)) * static_cast<float>(mFreqPos[i + 1] - samp);
        if (0.0f < a) {
            return static_cast<int>(a + 0.5f);
        }

        return static_cast<int>(a - 0.5f);
    }

    return mFreqPos[i];
}

int GinsuSynthData::CycleToSample(float cycle) const {
    int i;
    float a;
    int samp;

    i = mCycleCount;
    samp = 0;
    if (i > 0) {
        if (cycle <= 0.0f) {
            return mCyclePos[0];
        }

        if (static_cast<float>(i) <= cycle) {
            return mCyclePos[i];
        }

        i = IntFloor(cycle);
        samp = mCyclePos[i];
        a = static_cast<float>(samp) + (cycle - static_cast<float>(i)) * static_cast<float>(mCyclePos[i + 1] - samp);
        if (a <= 0.0f) {
            return static_cast<int>(a - 0.5f);
        }

        samp = static_cast<int>(a + 0.5f);
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
    float cycle;

    high = mCycleCount;
    if (high <= 0) {
        return 0.0f;
    }

    if (sample <= mCyclePos[0]) {
        return 0.0f;
    }

    if (sample >= mCyclePos[high]) {
        return static_cast<float>(high);
    }

    low = 0;
    while (true) {
        while (true) {
            cycle = (static_cast<float>(sample - mCyclePos[low]) / static_cast<float>(mCyclePos[high] - mCyclePos[low])) *
                    static_cast<float>(high - low);
            guess = low + IntFloor(cycle);
            if (sample >= mCyclePos[guess]) {
                break;
            }

            {
                cycle = static_cast<float>(mCyclePos[guess] - sample) / mMinPeriod;
                int newlow = guess - IntCeil(cycle);
                high = guess;
                if (newlow > low) {
                    low = newlow;
                }
            }
        }

        if (sample < mCyclePos[guess + 1]) {
            break;
        }

        {
            cycle = static_cast<float>(sample - mCyclePos[guess]) / mMinPeriod;
            int newhigh = guess + IntCeil(cycle) + 1;
            low = guess + 1;
            if (newhigh < high) {
                high = newhigh;
            }
        }
    }

    s1 = static_cast<float>(mCyclePos[guess]);
    s2 = static_cast<float>(mCyclePos[guess + 1]);
    cycle = (static_cast<float>(sample) - s1) / (s2 - s1);
    return cycle + static_cast<float>(guess);
}

bool GinsuSynthData::GetSamples(int startSample, int numSamples, short *dest) {
    int endSample;
    int block;
    int index;

    if (startSample < 0 || mSampleCount <= startSample + numSamples - 1) {
        return false;
    }

    block = startSample >> 5;
    if (block != mCurrentBlock) {
        DecodeBlock(block);
    }

    index = startSample & 0x1F;
    if (numSamples > 0) {
        int i;
        for (i = 0; i < numSamples; i++) {
            endSample = static_cast<int>(mSample[index]);
            if (endSample < 0x8000) {
                if (endSample < -0x8000) {
                    endSample = -0x8000;
                }
            } else {
                endSample = 0x7FFF;
            }

            dest[i] = static_cast<short>(endSample);
            index++;
            if (index == 0x20) {
                DecodeBlock(mCurrentBlock + 1);
                index = 0;
            }
        }
    }

    return true;
}

GinsuSynthData::~GinsuSynthData() {}
