#include <cstddef>

namespace Snd {

// total size: 0x8
struct XAFSTATE {
    float sample1; // offset 0x0
    float sample2; // offset 0x4
};

// total size: 0x14
struct MXAPACKETF {
    int numframes;        // offset 0x0
    float sample1;        // offset 0x4
    float sample2;        // offset 0x8
    unsigned char *psrc;  // offset 0xC
    float *pdst;          // offset 0x10
};

// total size: 0x90
struct EAXAVARSF {
    int residual;          // offset 0x0
    int numsamples;        // offset 0x4
    int sampledatasize;    // offset 0x8
    float *presidue;       // offset 0xC
    float xablockprev[2];  // offset 0x10
    float xablock[30];     // offset 0x18
};

// total size: 0xA8
struct CEAXABLKDecf {
    static void *operator new(size_t size);
    static void operator delete(void *ptr);

    CEAXABLKDecf();
    ~CEAXABLKDecf() {}
    int Feed(void *pSampleData, int sampleDataSize, int numSamples);
    int Decode(float **pDstBuf, int numSamples);
    XAFSTATE GetState();
    void SetState(XAFSTATE *pxas);

    void (*decodexa)(MXAPACKETF *); // offset 0x0
    EAXAVARSF xav;                  // offset 0x4
    MXAPACKETF xap;                 // offset 0x94
};

extern void *(*CODANew)(unsigned int size);
extern void (*CODADelete)(void *pmem);

float xafilterf[2][4] = {
    {0.0f, 0.9375f, 1.796875f, 1.53125f},
    {0.0f, 0.0f, -0.8125f, -0.859375f},
};

float xatablef[16][16] = {
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

void process_raw_block(MXAPACKETF *pxap) {
    unsigned int i;
    short s;

    if (*pxap->psrc == 0xEE) {
        unsigned char *pb = (unsigned char *)&s;

        pxap->psrc++;
        pb[0] = pxap->psrc[0];
        pb[1] = pxap->psrc[1];
        pxap->psrc += 2;
        pxap->sample1 = (float)s;
        pb[0] = pxap->psrc[0];
        pb[1] = pxap->psrc[1];
        pxap->psrc += 2;
        pxap->sample2 = (float)s;
        for (i = 0; i < 28; i++) {
            unsigned char *pbs = (unsigned char *)&s;

            pbs[0] = pxap->psrc[0];
            pbs[1] = pxap->psrc[1];
            *pxap->pdst = (float)s;
            pxap->pdst++;
            pxap->psrc += 2;
        }
    }
}

void decodexac(MXAPACKETF *pxap) {
    int i;
    int filt;
    int shift;
    unsigned char b;
    unsigned char *ps;
    float f0;
    float f1;

    ps = pxap->psrc;
    pxap->pdst[-2] = pxap->sample2;
    pxap->pdst[-1] = pxap->sample1;
    while (pxap->numframes > 0) {
        if (*ps == 0xEE) {
            process_raw_block(pxap);
            pxap->numframes -= 28;
        } else {
            filt = *pxap->psrc >> 4;
            pxap->numframes -= 28;
            f0 = xafilterf[0][filt];
            f1 = xafilterf[1][filt];
            shift = *pxap->psrc & 0xF;
            pxap->psrc++;
            for (i = 0; i < 14; i++) {
                b = pxap->psrc[i];
                pxap->pdst[0] = xatablef[shift][b >> 4] + pxap->pdst[-1] * f0 + pxap->pdst[-2] * f1;
                pxap->pdst[1] = xatablef[shift][b & 0xF] + pxap->pdst[0] * f0 + pxap->pdst[-1] * f1;
                pxap->pdst += 2;
            }
            pxap->psrc += 14;
            pxap->sample2 = pxap->pdst[-2];
            pxap->sample1 = pxap->pdst[-1];
        }
        ps = pxap->psrc;
    }
}

void *CEAXABLKDecf::operator new(size_t size) {
    return CODANew(size);
}

void CEAXABLKDecf::operator delete(void *ptr) {
    CODADelete(ptr);
}

CEAXABLKDecf::CEAXABLKDecf() {
    xav.sampledatasize = 0;
    xav.numsamples = 0;
    xav.residual = 0;
    xap.sample1 = 0.0f;
    xap.sample2 = 0.0f;
}

int CEAXABLKDecf::Feed(void *pSampleData, int sampleDataSize, int numSamples) {
    if (pSampleData == NULL) {
        return -1;
    }
    if (xav.numsamples != 0) {
        return -1;
    }
    xav.numsamples = numSamples;
    xav.sampledatasize = sampleDataSize;
    xap.psrc = (unsigned char *)pSampleData;
    return 0;
}

int CEAXABLKDecf::Decode(float **pDstBuf, int numSamples) {
    int decoded;
    int i;
    int n;
    int k;
    int blocks;
    int rem;
    float *pdst;

    xap.pdst = *pDstBuf;
    decoded = 0;
    if (xav.numsamples == 0) {
        return 0;
    }
    n = xav.numsamples;
    if (n > numSamples) {
        n = numSamples;
    }
    if (xav.residual != 0) {
        if (xav.residual > n) {
            k = n;
        } else {
            k = xav.residual;
        }
        for (i = 0; i < k; i++) {
            *xap.pdst = *xav.presidue;
            xap.pdst++;
            xav.presidue++;
        }
        n -= k;
        xav.numsamples -= k;
        xav.residual -= k;
        decoded += k;
    }
    blocks = (n / 28) * 28;
    rem = n - blocks;
    xap.numframes = blocks;
    if (blocks > 0) {
        decoded += blocks;
        decodexac(&xap);
        xav.numsamples -= blocks;
    }
    if (rem > 0) {
        pdst = xap.pdst;
        xap.numframes = rem;
        xap.pdst = &xav.xablock[2];
        i = 0;
        decodexac(&xap);
        xav.numsamples -= rem;
        xav.presidue = xap.pdst + xap.numframes;
        xav.residual = -xap.numframes;
        xap.pdst -= 28;
        for (; i < rem; i++) {
            pdst[i] = xap.pdst[i];
        }
        decoded += rem;
    }
    if (xav.numsamples <= 0) {
        xav.residual = 0;
    }
    return decoded;
}

XAFSTATE CEAXABLKDecf::GetState() {
    XAFSTATE xtmp;

    xtmp.sample1 = xap.sample1;
    xtmp.sample2 = xap.sample2;
    return xtmp;
}

void CEAXABLKDecf::SetState(XAFSTATE *pxas) {
    xap.sample1 = pxas->sample1;
    xap.sample2 = pxas->sample2;
}

} // namespace Snd
