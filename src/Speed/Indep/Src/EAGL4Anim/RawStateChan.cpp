#include "RawStateChan.h"

namespace EAGL4Anim {

void FnRawStateChan::Decode(unsigned char *src, unsigned char *dest) const {

    RawStateChan *c = reinterpret_cast<RawStateChan *>(mpAnim);

    unsigned int r = 0;
    unsigned char bitIdx = 0;

    for (int idx = 0; idx < c->GetNumFields(); idx++) {

        unsigned char storedNumBitsInPowersOf2;
        unsigned char destNumBytes;
        unsigned char destByteOffset;

        c->GetDecodeData(idx, storedNumBitsInPowersOf2, destNumBytes, destByteOffset);

        switch (storedNumBitsInPowersOf2) {

        case 5:

            r = *reinterpret_cast<unsigned int *>(src);
            src += 4;
            break;

        case 4:

            r = *reinterpret_cast<unsigned short *>(src);
            src += 2;
            break;

        case 3:

            r = *src;
            src += 1;
            break;

        case 2:

            bitIdx += 4;
            r = (*src >> (8 - bitIdx)) & 15;
            break;

        case 1:

            bitIdx += 2;
            r = (*src >> (8 - bitIdx)) & 3;
            break;

        case 0:

            bitIdx += 1;
            r = (*src >> (8 - bitIdx)) & 1;
            break;
        }

        if (bitIdx > 7) {

            src += 1;
            bitIdx = 0;
        }

        switch (destNumBytes) {

        case 4:

            *reinterpret_cast<unsigned int *>(dest + destByteOffset) = r;
            break;

        case 2:

            *reinterpret_cast<unsigned short *>(dest + destByteOffset) = static_cast<unsigned short>(r);
            break;

        case 1:

            dest[destByteOffset] = static_cast<unsigned char>(r);
            break;
        }
    }
}

bool FnRawStateChan::EvalState(float time, State *s) {

    RawStateChan *c = reinterpret_cast<RawStateChan *>(mpAnim);
    int i;
    unsigned char *k;
    unsigned char *dest = reinterpret_cast<unsigned char *>(s);

    // El objetivo lee `mKeyIdx` UNA sola vez (`lwz r11, 0x10(r29)`) y lo hace
    // DENTRO del bloque de union del if/else de GetKeyData: el producto por
    // GetKeySize() esta escrito aqui, no dentro del accesor.
    if (time >= *reinterpret_cast<float *>(c->GetKeyData(0) + mKeyIdx * c->GetKeySize())) {

        i = mKeyIdx;

        while (i < c->GetNumKeys()) {

            k = c->GetKeyData(i);

            if (time < *reinterpret_cast<float *>(&k[c->GetKeySize()])) {

                Decode(k + 4, dest);
                mKeyIdx = i;
                return true;
            }

            i++;
        }

        k = c->GetKeyData(0) + (c->GetNumKeys() - 1) * c->GetKeySize();
        Decode(k + 4, dest);
        mKeyIdx = c->GetNumKeys() - 1;

    } else {

        for (i = mKeyIdx - 1; i >= 0; i--) {

            k = c->GetKeyData(i);

            if (time >= *reinterpret_cast<float *>(&k)) {

                Decode(k + 4, dest);
                mKeyIdx = i;
                return true;
            }
        }

        k = c->GetKeyData(0);
        Decode(k + 4, dest);
        mKeyIdx = 0;
    }

    return true;
}

bool FnRawStateChan::FindTime(const StateTest &test, float startTime, float &resultTime) {

    RawStateChan *c = reinterpret_cast<RawStateChan *>(mpAnim);
    int i;
    unsigned char *k;
    const char kMaxSize = 80;
    unsigned char state[kMaxSize];
    float keyTime;

    for (i = 0; i < c->GetNumKeys(); i++) {

        k = c->GetKeyData(0);
        keyTime = *reinterpret_cast<float *>(k + i * c->GetKeySize());
        k += i * c->GetKeySize();

        if (keyTime > startTime) {

            Decode(k + 4, state);

            if (test.Pass(reinterpret_cast<State *>(state))) {

                resultTime = keyTime;
                return true;
            }
        }
    }

    return false;
}

inline FnRawStateChan::~FnRawStateChan() {}

inline bool FnRawStateChan::GetLength(float &timeLength) const {
    timeLength = static_cast<float>(reinterpret_cast<RawStateChan *>(mpAnim)->GetNumFrames());
    return true;
}

inline void FnRawStateChan::Eval(float, float time, float *dofs) {
    EvalState(time, reinterpret_cast<State *>(dofs));
}

}; // namespace EAGL4Anim
