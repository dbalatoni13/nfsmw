#include <cstddef>
namespace Snd {

struct UTALKSTATE_CODA {
    unsigned char *dataptr;
    unsigned int shiftreg;
    int bitcount;
    int mode;
    int voicing_threshold;
    float stepval[64];
    float coeff[12];
    float output[12];
    float history[324];
    float data[432];
};

struct MTFSTATE {
    unsigned int shiftreg;
    unsigned int bufframes;
    int doWholeInit;
};

struct CMTBLKDecf {
    static void *operator new(size_t size);
    static void operator delete(void *ptr);

    void initmut(unsigned char *data, UTALKSTATE_CODA *state, int doWholeInit);

    CMTBLKDecf();
    int Feed(void *pSampleData, int sampleDataSize, int numSamples);
    int Decode(float **pDstBuf, int numSamples);
    MTFSTATE GetState();
    void SetState(MTFSTATE *pstate, int ignoresamples);
    void SetCodecVersion(int codecversion);

    UTALKSTATE_CODA mutstate;
    unsigned int mbufframes;
    unsigned char *mpSrc;
    float *mpDst;
    short mRawframes;
    short mOffset;
    int mRemainingSamples;
    int mSampleDataSize;
    int mdoWholeInit;
    int mHasPCMData;
    int mCodecVersion;
};

extern void *(*CODANew)(unsigned int size);
extern void (*CODADelete)(void *pmem);

namespace Coda {
extern void *(*gpMemCpy)(void *pdst, void const *psrc, unsigned int size);
}

typedef struct DECODETABLETYPE {
    int state;
    int bits;
    float sample;
} DECODETABLETYPE;

static unsigned int bitmask[9] = {
    0, 1, 3, 7, 15, 31, 63, 127, 255,
};

static float coeff_table[64] = {
    0.0f, -0.9967759847640991f, -0.990327000617981f, -0.983879029750824f,
    -0.9774309992790222f, -0.970982015132904f, -0.9645339846611023f, -0.9580850005149841f,
    -0.9516370296478271f, -0.9307540059089661f, -0.9049599766731262f, -0.8791670203208923f,
    -0.8533729910850525f, -0.8275790214538574f, -0.8017860054969788f, -0.7759919762611389f,
    -0.7501980066299438f, -0.7244049906730652f, -0.6986110210418701f, -0.6706349849700928f,
    -0.619047999382019f, -0.5674600005149841f, -0.5158730149269104f, -0.4642859995365143f,
    -0.41269800066947937f, -0.36111098527908325f, -0.3095239996910095f, -0.2579370141029358f,
    -0.20634900033473969f, -0.15476199984550476f, -0.10317499935626984f, -0.051587000489234924f,
    0.0f, 0.051587000489234924f, 0.10317499935626984f, 0.15476199984550476f,
    0.20634900033473969f, 0.2579370141029358f, 0.3095239996910095f, 0.36111098527908325f,
    0.41269800066947937f, 0.4642859995365143f, 0.5158730149269104f, 0.5674600005149841f,
    0.619047999382019f, 0.6706349849700928f, 0.6986110210418701f, 0.7244049906730652f,
    0.7501980066299438f, 0.7759919762611389f, 0.8017860054969788f, 0.8275790214538574f,
    0.8533729910850525f, 0.8791670203208923f, 0.9049599766731262f, 0.9307540059089661f,
    0.9516370296478271f, 0.9580850005149841f, 0.9645339846611023f, 0.970982015132904f,
    0.9774309992790222f, 0.983879029750824f, 0.990327000617981f, 0.9967759847640991f,
};

static unsigned char index_table[2][256] = {
    {
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x11,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x15,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x12,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x19,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x11,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x16,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x12,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x00,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x11,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x15,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x12,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x1A,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x11,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x16,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0D, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x12,
        0x04, 0x06, 0x05, 0x09, 0x04, 0x06, 0x05, 0x0E, 0x04, 0x06, 0x05, 0x0A, 0x04, 0x06, 0x05, 0x02,
    },
    {
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x17,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x1B,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x18,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x01,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x17,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x1C,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x18,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x03,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x17,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x1B,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x18,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x01,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x17,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x1C,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x13, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x18,
        0x04, 0x0B, 0x07, 0x0F, 0x04, 0x0C, 0x08, 0x14, 0x04, 0x0B, 0x07, 0x10, 0x04, 0x0C, 0x08, 0x03,
    },
};

static DECODETABLETYPE decode_table[29] = {
    { 1, 8, 0.0f },
    { 1, 7, 0.0f },
    { 0, 8, 0.0f },
    { 0, 7, 0.0f },
    { 0, 2, 0.0f },
    { 0, 2, -1.0f },
    { 0, 2, 1.0f },
    { 0, 3, -1.0f },
    { 0, 3, 1.0f },
    { 1, 4, -2.0f },
    { 1, 4, 2.0f },
    { 1, 3, -2.0f },
    { 1, 3, 2.0f },
    { 1, 5, -3.0f },
    { 1, 5, 3.0f },
    { 1, 4, -3.0f },
    { 1, 4, 3.0f },
    { 1, 6, -4.0f },
    { 1, 6, 4.0f },
    { 1, 5, -4.0f },
    { 1, 5, 4.0f },
    { 1, 7, -5.0f },
    { 1, 7, 5.0f },
    { 1, 6, -5.0f },
    { 1, 6, 5.0f },
    { 1, 8, -6.0f },
    { 1, 8, 6.0f },
    { 1, 7, -6.0f },
    { 1, 7, 6.0f },
};

void *CMTBLKDecf::operator new(size_t size)
{
    return CODANew(size);
}

void CMTBLKDecf::operator delete(void *ptr)
{
    CODADelete(ptr);
}

static unsigned int getbits(UTALKSTATE_CODA *state, int count)
{
    unsigned int result;

    result = state->shiftreg & bitmask[count];
    state->shiftreg >>= count;
    state->bitcount -= count;
    if (state->bitcount <= 7) {
        state->shiftreg |= *state->dataptr++ << state->bitcount;
        state->bitcount += 8;
    }
    return result;
}

void discardbits(UTALKSTATE_CODA *state, int count)
{
    state->shiftreg >>= count;
    state->bitcount -= count;
    if (state->bitcount <= 7) {
        state->shiftreg |= *state->dataptr++ << state->bitcount;
        state->bitcount += 8;
    }
}

void readsamples(UTALKSTATE_CODA *state, int voiced, float *sample, int step)
{
    int decode_state;
    int index;
    int run;
    int code;
    int n;
    int i;
    int ps2compilerkludge;

    if (voiced) {
        decode_state = 0;
        n = 0;
        do {
            index = index_table[decode_state][state->shiftreg & 0xff];
            decode_state = decode_table[index].state;
            discardbits(state, decode_table[index].bits);
            if (index > 3) {
                sample[n] = decode_table[index].sample;
                n += step;
            } else if (index > 1) {
                run = getbits(state, 6) + 7;
                if (n + step * run > 108)
                    run = (108 - n) / step;
                for (i = 0; i < run; i++) {
                    sample[n] = 0.0f;
                    n += step;
                }
            } else {
                code = 7;
                while (getbits(state, 1) == 1)
                    code++;
                if (getbits(state, 1) == 1)
                    sample[n] = code;
                else
                    sample[n] = -code;
                n += step;
            }
        } while (n < 108);
    } else {
        ps2compilerkludge = 108;
        for (i = 0; i < ps2compilerkludge; i += step) {
            n = state->shiftreg & 3;
            if (n == 1) {
                sample[i] = -2.0f;
                discardbits(state, 2);
            } else if (n == 3) {
                sample[i] = 2.0f;
                discardbits(state, 2);
            } else {
                sample[i] = 0.0f;
                discardbits(state, 1);
            }
        }
    }
}

static void interpolate(float *sample)
{
    int i;

    for (i = 0; i < 108; i += 2)
        sample[i] = (sample[i - 1] + sample[i + 1]) * 0.5973859429359436f +
                    (sample[i - 3] + sample[i + 3]) * -0.1145915612578392f +
                    (sample[i - 5] + sample[i + 5]) * 0.018032679334282875f;
}

static void reftolpc(float *ref, float *lpc)
{
    float u[12];
    float y[12];
    float s;
    int i;
    int j;

    for (i = 10; i >= 0; i--)
        u[i + 1] = ref[i];
    u[0] = 1.0f;
    for (j = 0; j < 12; j++) {
        s = -ref[11] * u[11];
        for (i = 10; i >= 0; i--) {
            s -= ref[i] * u[i];
            u[i + 1] = ref[i] * s + u[i];
        }
        u[0] = s;
        y[j] = s;
        for (i = 0; i < j; i++)
            s -= lpc[i] * y[j - i - 1];
        lpc[j] = s;
    }
}

static void filter(UTALKSTATE_CODA *state, int index, int count)
{
    float lpc[12];
    float *data;
    int i;

    data = &state->data[index];
    reftolpc(state->coeff, lpc);
    for (i = 0; i < count; i++) {
        data[0] = state->output[11] =
            data[0] + lpc[0] * state->output[0] + lpc[1] * state->output[1] + lpc[2] * state->output[2] + 
            lpc[3] * state->output[3] + lpc[4] * state->output[4] + 
            lpc[5] * state->output[5] + lpc[6] * state->output[6] + 
            lpc[7] * state->output[7] + lpc[8] * state->output[8] + 
            lpc[9] * state->output[9] + lpc[10] * state->output[10] + 
            lpc[11] * state->output[11];
        data[1] = state->output[10] =
            data[1] + lpc[0] * state->output[11] + lpc[1] * state->output[0] + lpc[2] * state->output[1] + 
            lpc[3] * state->output[2] + lpc[4] * state->output[3] + 
            lpc[5] * state->output[4] + lpc[6] * state->output[5] + 
            lpc[7] * state->output[6] + lpc[8] * state->output[7] + 
            lpc[9] * state->output[8] + lpc[10] * state->output[9] + 
            lpc[11] * state->output[10];
        data[2] = state->output[9] =
            data[2] + lpc[0] * state->output[10] + lpc[1] * state->output[11] + 
            lpc[2] * state->output[0] + lpc[3] * state->output[1] + 
            lpc[4] * state->output[2] + lpc[5] * state->output[3] + 
            lpc[6] * state->output[4] + lpc[7] * state->output[5] + 
            lpc[8] * state->output[6] + lpc[9] * state->output[7] + 
            lpc[10] * state->output[8] + lpc[11] * state->output[9];
        data[3] = state->output[8] =
            data[3] + lpc[0] * state->output[9] + lpc[1] * state->output[10] + 
            lpc[2] * state->output[11] + lpc[3] * state->output[0] + 
            lpc[4] * state->output[1] + lpc[5] * state->output[2] + 
            lpc[6] * state->output[3] + lpc[7] * state->output[4] + 
            lpc[8] * state->output[5] + lpc[9] * state->output[6] + 
            lpc[10] * state->output[7] + lpc[11] * state->output[8];
        data[4] = state->output[7] =
            data[4] + lpc[0] * state->output[8] + lpc[1] * state->output[9] + lpc[2] * state->output[10] + 
            lpc[3] * state->output[11] + lpc[4] * state->output[0] + 
            lpc[5] * state->output[1] + lpc[6] * state->output[2] + 
            lpc[7] * state->output[3] + lpc[8] * state->output[4] + 
            lpc[9] * state->output[5] + lpc[10] * state->output[6] + 
            lpc[11] * state->output[7];
        data[5] = state->output[6] =
            data[5] + lpc[0] * state->output[7] + lpc[1] * state->output[8] + lpc[2] * state->output[9] + 
            lpc[3] * state->output[10] + lpc[4] * state->output[11] + 
            lpc[5] * state->output[0] + lpc[6] * state->output[1] + 
            lpc[7] * state->output[2] + lpc[8] * state->output[3] + 
            lpc[9] * state->output[4] + lpc[10] * state->output[5] + 
            lpc[11] * state->output[6];
        data[6] = state->output[5] =
            data[6] + lpc[0] * state->output[6] + lpc[1] * state->output[7] + lpc[2] * state->output[8] + 
            lpc[3] * state->output[9] + lpc[4] * state->output[10] + 
            lpc[5] * state->output[11] + lpc[6] * state->output[0] + 
            lpc[7] * state->output[1] + lpc[8] * state->output[2] + 
            lpc[9] * state->output[3] + lpc[10] * state->output[4] + 
            lpc[11] * state->output[5];
        data[7] = state->output[4] =
            data[7] + lpc[0] * state->output[5] + lpc[1] * state->output[6] + lpc[2] * state->output[7] + 
            lpc[3] * state->output[8] + lpc[4] * state->output[9] + 
            lpc[5] * state->output[10] + lpc[6] * state->output[11] + 
            lpc[7] * state->output[0] + lpc[8] * state->output[1] + 
            lpc[9] * state->output[2] + lpc[10] * state->output[3] + 
            lpc[11] * state->output[4];
        data[8] = state->output[3] =
            data[8] + lpc[0] * state->output[4] + lpc[1] * state->output[5] + lpc[2] * state->output[6] + 
            lpc[3] * state->output[7] + lpc[4] * state->output[8] + 
            lpc[5] * state->output[9] + lpc[6] * state->output[10] + 
            lpc[7] * state->output[11] + lpc[8] * state->output[0] + 
            lpc[9] * state->output[1] + lpc[10] * state->output[2] + 
            lpc[11] * state->output[3];
        data[9] = state->output[2] =
            data[9] + lpc[0] * state->output[3] + lpc[1] * state->output[4] + lpc[2] * state->output[5] + 
            lpc[3] * state->output[6] + lpc[4] * state->output[7] + 
            lpc[5] * state->output[8] + lpc[6] * state->output[9] + 
            lpc[7] * state->output[10] + lpc[8] * state->output[11] + 
            lpc[9] * state->output[0] + lpc[10] * state->output[1] + 
            lpc[11] * state->output[2];
        data[10] = state->output[1] =
            data[10] + lpc[0] * state->output[2] + lpc[1] * state->output[3] + lpc[2] * state->output[4] + 
            lpc[3] * state->output[5] + lpc[4] * state->output[6] + 
            lpc[5] * state->output[7] + lpc[6] * state->output[8] + 
            lpc[7] * state->output[9] + lpc[8] * state->output[10] + 
            lpc[9] * state->output[11] + lpc[10] * state->output[0] + 
            lpc[11] * state->output[1];
        data[11] = state->output[0] =
            data[11] + lpc[0] * state->output[1] + lpc[1] * state->output[2] + lpc[2] * state->output[3] + 
            lpc[3] * state->output[4] + lpc[4] * state->output[5] + 
            lpc[5] * state->output[6] + lpc[6] * state->output[7] + 
            lpc[7] * state->output[8] + lpc[8] * state->output[9] + 
            lpc[9] * state->output[10] + lpc[10] * state->output[11] + 
            lpc[11] * state->output[0];
        data += 12;
    }
}

void CMTBLKDecf::initmut(unsigned char *data, UTALKSTATE_CODA *state, int doWholeInit)
{
    float gain;
    int i;

    state->bitcount = 8;
    state->dataptr = data + 1;
    state->shiftreg = state->dataptr[-1];
    if (doWholeInit) {
        state->mode = getbits(state, 1);
        state->voicing_threshold = 32 - getbits(state, 4);
        state->stepval[0] = (getbits(state, 4) + 1) * 8.0f;
        gain = getbits(state, 6) * 0.001f + 1.04f;
        for (i = 1; i < 64; i++)
            state->stepval[i] = state->stepval[i - 1] * gain;
        for (i = 0; i < 12; i++) {
            state->coeff[i] = 0.0f;
            state->output[i] = 0.0f;
        }
        for (i = 0; i < 324; i++)
            state->history[i] = 0.0f;
    }
}

void decodemut(UTALKSTATE_CODA *state)
{
    float sample[118];
    float delta[12];
    float hgain;
    float sgain;
    int lar;
    int voiced;
    int offset;
    int imaging;
    int subwin;
    int hindex;
    int index;
    int i;

    lar = getbits(state, 6);
    voiced = lar < state->voicing_threshold;
    delta[0] = (coeff_table[lar] - state->coeff[0]) * 0.25f;
    for (i = 1; i < 4; i++)
        delta[i] = (coeff_table[getbits(state, 6)] - state->coeff[i]) * 0.25f;
    for (i = 4; i < 12; i++)
        delta[i] = (coeff_table[getbits(state, 5) + 16] - state->coeff[i]) * 0.25f;
    for (subwin = 0; subwin < 4; subwin++) {
        hindex = (subwin + 2) * 108 - getbits(state, 8);
        hgain = getbits(state, 4) * (1.0f / 15.0f);
        sgain = state->stepval[getbits(state, 6)];
        if (state->mode == 0) {
            readsamples(state, voiced, &sample[5], 1);
        } else {
            offset = getbits(state, 1);
            imaging = getbits(state, 1);
            readsamples(state, voiced, &sample[5 + offset], 2);
            if (imaging) {
                for (i = 0; i < 108; i += 2)
                    sample[i + 6 - offset] = 0.0f;
            } else {
                for (i = 0; i < 5; i++)
                    sample[i] = 0.0f;
                for (i = 113; i < 118; i++)
                    sample[i] = 0.0f;
                interpolate(sample + 6 - offset);
                sgain *= 0.5f;
            }
        }
        index = subwin * 108;
        for (i = 0; i < 108; i++)
            state->data[index + i] = hgain * state->history[hindex + i] + sgain * sample[i + 5];
    }
    Coda::gpMemCpy(state->history, &state->data[108], sizeof(state->history));
    for (i = 0; i < 12; i++)
        state->coeff[i] += delta[i];
    filter(state, 0, 1);
    for (i = 0; i < 12; i++)
        state->coeff[i] += delta[i];
    filter(state, 12, 1);
    for (i = 0; i < 12; i++)
        state->coeff[i] += delta[i];
    filter(state, 24, 1);
    for (i = 0; i < 12; i++)
        state->coeff[i] += delta[i];
    filter(state, 36, 33);
}

CMTBLKDecf::CMTBLKDecf()
{
    mbufframes = 0;
    mRemainingSamples = 0;
    mSampleDataSize = 0;
    mRawframes = 0;
    mOffset = 0;
    mdoWholeInit = 0;
    mHasPCMData = 0;
    mCodecVersion = 1;
}

int CMTBLKDecf::Feed(void *pSampleData, int sampleDataSize, int numSamples)
{
    if (pSampleData == 0 || mRemainingSamples != 0)
        return -1;
    mRemainingSamples = numSamples;
    mSampleDataSize = sampleDataSize;
    mbufframes = 0;
    mpSrc = (unsigned char *)pSampleData;
    if (mCodecVersion == 1) {
        if (*mpSrc == 0xee)
            mHasPCMData = 1;
        else
            mHasPCMData = 0;
        initmut((unsigned char *)pSampleData + 1, &mutstate, mdoWholeInit);
    } else {
        initmut((unsigned char *)pSampleData, &mutstate, mdoWholeInit);
    }
    return 0;
}

inline void CopyU(short &dst, short &src)
{
    ((unsigned char *)&dst)[0] = ((unsigned char *)&src)[0];
    ((unsigned char *)&dst)[1] = ((unsigned char *)&src)[1];
}

inline void PutUB(short &dst, short &src)
{
    CopyU(dst, src);
}

inline void Reverse(short &dst, short &src)
{
    ((unsigned char *)&dst)[0] = ((unsigned char *)&src)[1];
    ((unsigned char *)&dst)[1] = ((unsigned char *)&src)[0];
}

inline void PutUL(short &dst, short &src)
{
    Reverse(dst, src);
}

inline void PutU(bool bigendian, short &dst, short &src)
{
    if (bigendian)
        PutUB(dst, src);
    else
        PutUL(dst, src);
}

int CMTBLKDecf::Decode(float **pDstBuf, int numSamples)
{
    int tmpframecnt;
    int maxsamples;
    int count = mbufframes;
    short i;
    float *pdstdata;
    short temp;
    unsigned char *mpSrcTemp;

    if (mRemainingSamples == 0)
        return 0;
    mpDst = *pDstBuf;
    maxsamples = mRemainingSamples;
    if (maxsamples > numSamples)
        maxsamples = numSamples;
    tmpframecnt = maxsamples;
    while (tmpframecnt > 0) {
        if (tmpframecnt < count)
            count = tmpframecnt;
        Coda::gpMemCpy(mpDst, &mutstate.data[432 - mbufframes], count * sizeof(float));
        mpDst += count;
        mbufframes -= count;
        tmpframecnt -= count;
        if (tmpframecnt > 0) {
            decodemut(&mutstate);
            mbufframes = 432;
            count = 432;
            if (mCodecVersion == 1) {
                mpSrc = mutstate.dataptr - 1;
                mpSrcTemp = mpSrc;
                if (mHasPCMData) {
                    PutU(true, mOffset, *(short *)mpSrcTemp);
                    mpSrcTemp += 2;
                    PutU(true, mRawframes, *(short *)mpSrcTemp);
                    mpSrcTemp += 2;
                    mpSrc = mpSrc + mRawframes * 2 + 4;
                    mutstate.dataptr = mpSrc + 1;
                    pdstdata = &mutstate.data[mOffset];
                    for (i = 0; i < mRawframes; i++) {
                        PutU(true, temp, *(short *)mpSrcTemp);
                        *pdstdata++ = temp;
                        mpSrcTemp += 2;
                    }
                    mHasPCMData = 0;
                }
                if (*mpSrc == 0xee)
                    mHasPCMData = 1;
                mutstate.bitcount = 8;
                mutstate.shiftreg = *mutstate.dataptr;
                mutstate.dataptr++;
            }
        }
    }
    mRemainingSamples -= maxsamples;
    return maxsamples;
}

MTFSTATE CMTBLKDecf::GetState()
{
    MTFSTATE mts;

    mts.shiftreg = mutstate.shiftreg;
    mts.doWholeInit = mdoWholeInit;
    mts.bufframes = mbufframes;
    return mts;
}

void CMTBLKDecf::SetState(MTFSTATE *pstate, int ignoresamples)
{
    mutstate.shiftreg = pstate->shiftreg;
    mdoWholeInit = pstate->doWholeInit;
    mbufframes = pstate->bufframes;
}

void CMTBLKDecf::SetCodecVersion(int codecversion)
{
    mCodecVersion = codecversion;
}

} // namespace Snd
