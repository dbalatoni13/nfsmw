#include "ginsu.h"
#include "ginsuhelper.h"

// total size: 0x24
// Decl: 7
struct GinsuDataLayout {
    char id[4];            // offset 0x0, size 0x4, Decl: 8
    char ver[2];           // offset 0x4, size 0x2, Decl: 9
    short flags;           // offset 0x6, size 0x2, Decl: 10
    float minFrequency;    // offset 0x8, size 0x4, Decl: 11
    float maxFrequency;    // offset 0xC, size 0x4, Decl: 12
    int segCount;          // offset 0x10, size 0x4, Decl: 13
    int cycleCount;        // offset 0x14, size 0x4, Decl: 14
    int sampleCount;       // offset 0x18, size 0x4, Decl: 15
    int sampleRate;        // offset 0x1C, size 0x4, Decl: 16
    unsigned char data[1]; // offset 0x20, size 0x1, Decl: 17
};

static float xafilterf[2][4] = {
    {0.0f, 0.9375f, 1.796875f, 1.53125f},
    {0.0f, 0.0f, -0.8125f, -0.859375f},
};

static float xatablef[16][16] = {
    {0.0f, 4096.0f, 8192.0f, 12288.0f, 16384.0f, 20480.0f, 24576.0f, 28672.0f, -32768.0f, -28672.0f, -24576.0f, -20480.0f, -16384.0f, -12288.0f,
     -8192.0f, -4096.0f},
    {0.0f, 2048.0f, 4096.0f, 6144.0f, 8192.0f, 10240.0f, 12288.0f, 14336.0f, -16384.0f, -14336.0f, -12288.0f, -10240.0f, -8192.0f, -6144.0f, -4096.0f,
     -2048.0f},
    {0.0f, 1024.0f, 2048.0f, 3072.0f, 4096.0f, 5120.0f, 6144.0f, 7168.0f, -8192.0f, -7168.0f, -6144.0f, -5120.0f, -4096.0f, -3072.0f, -2048.0f,
     -1024.0f},
    {0.0f, 512.0f, 1024.0f, 1536.0f, 2048.0f, 2560.0f, 3072.0f, 3584.0f, -4096.0f, -3584.0f, -3072.0f, -2560.0f, -2048.0f, -1536.0f, -1024.0f,
     -512.0f},
    {0.0f, 256.0f, 512.0f, 768.0f, 1024.0f, 1280.0f, 1536.0f, 1792.0f, -2048.0f, -1792.0f, -1536.0f, -1280.0f, -1024.0f, -768.0f, -512.0f, -256.0f},
    {0.0f, 128.0f, 256.0f, 384.0f, 512.0f, 640.0f, 768.0f, 896.0f, -1024.0f, -896.0f, -768.0f, -640.0f, -512.0f, -384.0f, -256.0f, -128.0f},
    {0.0f, 64.0f, 128.0f, 192.0f, 256.0f, 320.0f, 384.0f, 448.0f, -512.0f, -448.0f, -384.0f, -320.0f, -256.0f, -192.0f, -128.0f, -64.0f},
    {0.0f, 32.0f, 64.0f, 96.0f, 128.0f, 160.0f, 192.0f, 224.0f, -256.0f, -224.0f, -192.0f, -160.0f, -128.0f, -96.0f, -64.0f, -32.0f},
    {0.0f, 16.0f, 32.0f, 48.0f, 64.0f, 80.0f, 96.0f, 112.0f, -128.0f, -112.0f, -96.0f, -80.0f, -64.0f, -48.0f, -32.0f, -16.0f},
    {0.0f, 8.0f, 16.0f, 24.0f, 32.0f, 40.0f, 48.0f, 56.0f, -64.0f, -56.0f, -48.0f, -40.0f, -32.0f, -24.0f, -16.0f, -8.0f},
    {0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f, 28.0f, -32.0f, -28.0f, -24.0f, -20.0f, -16.0f, -12.0f, -8.0f, -4.0f},
    {0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, -16.0f, -14.0f, -12.0f, -10.0f, -8.0f, -6.0f, -4.0f, -2.0f},
    {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, -8.0f, -7.0f, -6.0f, -5.0f, -4.0f, -3.0f, -2.0f, -1.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
};

static inline short convertsample(float x) {
    int val = static_cast<int>(x);

    if (val >= 0x8000) {
        return 0x7fff;
    }
    if (val < -0x8000) {
        return -0x8000;
    }

    return static_cast<short>(val);
}

static void swapbytes(unsigned char *data) {
    unsigned char t1 = data[0];
    unsigned char t2 = data[1];

    data[0] = data[3];
    data[1] = data[2];
    data[2] = t2;
    data[3] = t1;
}

static void AdjustEndienness(GinsuDataLayout *memdata) {
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->minFrequency));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->maxFrequency));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->segCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->cycleCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->sampleCount));
    swapbytes(reinterpret_cast<unsigned char *>(&memdata->sampleRate));

    unsigned char *data = memdata->data;
    int count = memdata->segCount + 2 + memdata->cycleCount;

    for (int i = 0; i < count; i++) {
        swapbytes(data);
        data += 4;
    }

    memdata->flags = 1;
}

void GinsuSynthData::DecodeBlock(int block) {
    unsigned char *src = this->mSampleData + block * 0x13;
    float *out = this->mSample + 2;
    this->mCurrentBlock = block;
    this->mSample[0] = static_cast<float>(static_cast<int>((src[0] & 0xF0) + static_cast<char>(src[1]) * 0x100));
    this->mSample[1] = static_cast<float>(static_cast<int>((src[2] & 0xF0) + static_cast<char>(src[3]) * 0x100));

    int filt = src[0] & 0xF;
    float f0 = xafilterf[0][filt];
    float f1 = xafilterf[1][filt];
    int shift = src[2] & 0xF;
    src += 4;

    for (int i = 0; i < 15; i++) {
        int s0 = src[i] >> 4;
        int s1 = src[i] & 0xF;

        *out = xatablef[shift][s0] + f0 * out[-1] + f1 * out[-2];
        out[1] = xatablef[shift][s1] + f0 * out[0] + f1 * out[-1];
        out += 2;
    }
}

GinsuSynthData::GinsuSynthData()
    : mMinFrequency(0.0f), //
      mMaxFrequency(0.0f), //
      mSegCount(0),        //
      mCycleCount(0),      //
      mSampleCount(0),     //
      mSampleRate(0) {}

// UNSOLVED
bool GinsuSynthData::BindToData(void *ptr) {
    GinsuDataLayout *memdata = static_cast<GinsuDataLayout *>(ptr);

    this->mMinFrequency = 0.0f;
    this->mMaxFrequency = 0.0f;
    this->mSegCount = 0;
    this->mCycleCount = 0;
    this->mSampleCount = 0;
    this->mSampleRate = 0;

    if (*reinterpret_cast<unsigned int *>(memdata) != 'Gnsu' || memdata->ver[0] != '2') {
        return false;
    }

#ifdef NATIVE_ENDIAN_BIG
    if (memdata->flags == 0) {
        AdjustEndienness(memdata);
    }
#endif

    this->mMinFrequency = memdata->minFrequency;
    this->mMaxFrequency = memdata->maxFrequency;
    this->mSegCount = memdata->segCount;
    this->mCycleCount = memdata->cycleCount;
    this->mSampleCount = memdata->sampleCount;
    this->mSampleRate = memdata->sampleRate;
    this->mFreqPos = reinterpret_cast<int *>(memdata->data);
    this->mCyclePos = this->mFreqPos + (this->mSegCount + 1);
    this->mSampleData = reinterpret_cast<unsigned char *>(this->mCyclePos + (this->mCycleCount + 1));
    this->mCurrentBlock = -1;
    int minperiod = this->mSampleCount;

    for (int i = 0; i < this->mCycleCount; i++) {
        int period = this->mCyclePos[i + 1] - this->mCyclePos[i];

        if (period < minperiod) {
            minperiod = period;
        }
    }

    this->mMinPeriod = static_cast<float>(minperiod);
    return true;
}

// UNSOLVED
int GinsuSynthData::FrequencyToSample(float freq) const {
    float seg;
    int i;
    float a;
    int samp;

    if (this->mSegCount < 1) {
        samp = 0;
    } else if (freq <= this->mMinFrequency) {
        samp = this->mFreqPos[0];
    } else if (freq >= this->mMaxFrequency) {
        samp = this->mFreqPos[this->mSegCount];
    } else {
        seg = static_cast<float>(this->mSegCount) * (freq - this->mMinFrequency) / (this->mMaxFrequency - this->mMinFrequency);
        i = IntFloor(seg);
        a = static_cast<float>(this->mFreqPos[i + 1] - this->mFreqPos[i]);
        samp = IntRound(a * (seg - static_cast<float>(i)) + static_cast<float>(this->mFreqPos[i]));
    }

    return samp;
}

// UNSOLVED
int GinsuSynthData::CycleToSample(float cycle) const {
    int i;
    float a;
    int samp;

    if (this->mCycleCount < 1) {
        samp = 0;
    } else if (cycle <= 0.0f) {
        samp = this->mCyclePos[0];
    } else if (cycle >= static_cast<float>(this->mCycleCount)) {
        samp = this->mCyclePos[this->mCycleCount];
    } else {
        i = IntFloor(cycle);
        a = static_cast<float>(this->mCyclePos[i + 1] - this->mCyclePos[i]);
        samp = IntRound(a * (cycle - static_cast<float>(i)) + static_cast<float>(this->mCyclePos[i]));
    }

    return samp;
}

float GinsuSynthData::CyclePeriod(float cycle) const {
    if (this->mCycleCount < 1) {
        return 0.0f;
    }

    float startPeriod;
    float endPeriod;
    int i = IntFloor(cycle);
    if (i < 1) {
        if (i < 0) {
            cycle = 0.0f;
            i = 0;
        }
        startPeriod = static_cast<float>(this->mCyclePos[1] - this->mCyclePos[0]);
        endPeriod = static_cast<float>(this->mCyclePos[2] - this->mCyclePos[0]) * 0.5f;
    } else {
        if (i >= this->mCycleCount - 1) {
            if (i >= this->mCycleCount) {
                i = this->mCycleCount - 1;
                cycle = static_cast<float>(this->mCycleCount);
            }
            startPeriod = static_cast<float>(this->mCyclePos[this->mCycleCount] - this->mCyclePos[this->mCycleCount - 2]) * 0.5f;
            endPeriod = static_cast<float>(this->mCyclePos[this->mCycleCount] - this->mCyclePos[this->mCycleCount - 1]);
        } else {
            startPeriod = static_cast<float>(this->mCyclePos[i + 1] - this->mCyclePos[i - 1]) * 0.5f;
            endPeriod = static_cast<float>(this->mCyclePos[i + 2] - this->mCyclePos[i]) * 0.5f;
        }
    }

    float a = cycle - static_cast<float>(i);
    return startPeriod + a * (endPeriod - startPeriod);
}

float GinsuSynthData::SampleToCycle(int sample) const {
    if (mCycleCount <= 0) {
        return 0.0f;
    }
    if (sample <= mCyclePos[0]) {
        return 0.0f;
    }
    if (sample >= mCyclePos[mCycleCount]) {
        return static_cast<float>(mCycleCount);
    }

    int low = 0;
    int high = mCycleCount;
    int guess;
    while (true) {
        while (true) {
            guess = low + IntFloor((static_cast<float>(sample - mCyclePos[low]) / static_cast<float>(mCyclePos[high] - mCyclePos[low])) *
                                   static_cast<float>(high - low));
            if (sample >= mCyclePos[guess]) {
                break;
            } else {
                int newlow = guess - IntCeil(static_cast<float>(mCyclePos[guess] - sample) / mMinPeriod);
                high = guess;
                if (newlow > low) {
                    low = newlow;
                }
            }
        }

        if (sample < mCyclePos[guess + 1]) {
            break;
        } else {
            int newhigh = guess + IntCeil(static_cast<float>(sample - mCyclePos[guess]) / mMinPeriod) + 1;
            low = guess + 1;
            if (newhigh < high) {
                high = newhigh;
            }
        }
    }

    float s1 = static_cast<float>(mCyclePos[guess]);
    float s2 = static_cast<float>(mCyclePos[guess + 1]);
    float cycle = (static_cast<float>(sample) - s1) / (s2 - s1);
    return cycle + static_cast<float>(guess);
}

bool GinsuSynthData::GetSamples(int startSample, int numSamples, short *dest) {
    int endSample = startSample + numSamples - 1;

    if (startSample < 0 || endSample >= this->mSampleCount) {
        return false;
    }

    int block = startSample >> 5;

    if (block != this->mCurrentBlock) {
        this->DecodeBlock(block);
    }

    int index = startSample & 0x1f;

    for (int i = 0; i < numSamples; i++) {
        dest[i] = convertsample(this->mSample[index]);
        index++;

        if (index == 0x20) {
            this->DecodeBlock(this->mCurrentBlock + 1);
            index = 0;
        }
    }

    return true;
}
