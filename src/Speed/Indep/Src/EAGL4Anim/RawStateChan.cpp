#include "RawStateChan.h"

namespace EAGL4Anim {

namespace {

static unsigned char *GetRawStateKeyData(RawStateChan *rawStateChan) {
    unsigned char *keyData = reinterpret_cast<unsigned char *>(rawStateChan->GetDecodeData());
    unsigned char numFields = rawStateChan->GetNumFields();

    keyData += numFields * sizeof(unsigned short);
    if ((numFields & 1) == 0) {
        keyData += sizeof(unsigned short);
    }

    return keyData;
}

static const unsigned char *GetRawStateKeyData(const RawStateChan *rawStateChan) {
    const unsigned char *keyData = reinterpret_cast<const unsigned char *>(rawStateChan->GetDecodeData());
    unsigned char numFields = rawStateChan->GetNumFields();

    keyData += numFields * sizeof(unsigned short);
    if ((numFields & 1) == 0) {
        keyData += sizeof(unsigned short);
    }

    return keyData;
}

} // namespace

FnRawStateChan::~FnRawStateChan() {}

bool FnRawStateChan::GetLength(float &timeLength) const {
    const RawStateChan *rawStateChan = reinterpret_cast<const RawStateChan *>(mpAnim);

    timeLength = static_cast<float>(rawStateChan->GetNumFrames());
    return true;
}

void FnRawStateChan::Eval(float, float time, float *dofs) {
    EvalState(time, reinterpret_cast<State *>(dofs));
}

void FnRawStateChan::Decode(unsigned char *src, unsigned char *dest) const {
    const RawStateChan *rawStateChan = reinterpret_cast<const RawStateChan *>(mpAnim);
    unsigned int value = 0;
    unsigned int numBits = 0;

    for (int i = 0; i < rawStateChan->GetNumFields(); i++) {
        unsigned short decodeData = rawStateChan->GetDecodeData()[i];
        unsigned short storedNumBitsInPowersOf2 = decodeData >> 13;
        unsigned char destNumBytes = static_cast<unsigned char>(((decodeData >> 11) & 3) + 1);
        unsigned char destByteOffset = static_cast<unsigned char>(decodeData);

        switch (storedNumBitsInPowersOf2) {
        case 0:
            numBits = (numBits + 1) & 0xFF;
            value = (*src >> ((8 - numBits) & 0x1F)) & 1;
            break;
        case 1:
            numBits = (numBits + 2) & 0xFF;
            value = (*src >> ((8 - numBits) & 0x1F)) & 3;
            break;
        case 2:
            numBits = (numBits + 4) & 0xFF;
            value = (*src >> ((8 - numBits) & 0x3F)) & 0xF;
            break;
        case 3:
            value = *src;
            src += 1;
            break;
        case 4:
            value = *reinterpret_cast<unsigned short *>(src);
            src += 2;
            break;
        case 5:
            value = *reinterpret_cast<unsigned int *>(src);
            src += 4;
            break;
        }

        if (numBits > 7) {
            src += 1;
            numBits = 0;
        }

        switch (destNumBytes) {
        case 1:
            *reinterpret_cast<char *>(&dest[destByteOffset]) = static_cast<char>(value);
            break;
        case 2:
            *reinterpret_cast<short *>(&dest[destByteOffset]) = static_cast<short>(value);
            break;
        case 4:
            *reinterpret_cast<unsigned int *>(&dest[destByteOffset]) = value;
            break;
        }
    }
}

bool FnRawStateChan::EvalState(float time, State *s) {
    RawStateChan *rawStateChan = reinterpret_cast<RawStateChan *>(mpAnim);
    unsigned char keySize = rawStateChan->GetKeySize();
    unsigned char *keyData = GetRawStateKeyData(rawStateChan);
    int keyIdx = mKeyIdx;

    if (*reinterpret_cast<float *>(&keyData[keyIdx * keySize]) > time) {
        keyIdx--;

        while (keyIdx >= 0) {
            float *keyTime = reinterpret_cast<float *>(&keyData[keyIdx * keySize]);

            if (*keyTime <= time) {
                Decode(reinterpret_cast<unsigned char *>(keyTime) + sizeof(float), reinterpret_cast<unsigned char *>(s));
                mKeyIdx = keyIdx;
                return true;
            }

            keyIdx--;
        }

        Decode(keyData + sizeof(float), reinterpret_cast<unsigned char *>(s));
        mKeyIdx = 0;
    } else {
        for (; keyIdx < rawStateChan->GetNumKeys(); keyIdx++) {
            unsigned char *currKey = &keyData[keyIdx * keySize];

            if (time < *reinterpret_cast<float *>(currKey + keySize)) {
                Decode(currKey + sizeof(float), reinterpret_cast<unsigned char *>(s));
                mKeyIdx = keyIdx;
                return true;
            }
        }

        Decode(keyData + keySize * (rawStateChan->GetNumKeys() - 1) + sizeof(float), reinterpret_cast<unsigned char *>(s));
        mKeyIdx = rawStateChan->GetNumKeys() - 1;
    }

    return true;
}

bool FnRawStateChan::FindTime(const StateTest &test, float startTime, float &resultTime) {
    const RawStateChan *rawStateChan = reinterpret_cast<const RawStateChan *>(mpAnim);
    const unsigned char *keyData = GetRawStateKeyData(rawStateChan);
    unsigned char keySize = rawStateChan->GetKeySize();
    unsigned char stateBuffer[84];

    for (int keyIdx = 0; keyIdx < rawStateChan->GetNumKeys(); keyIdx++) {
        const unsigned char *currKey = &keyData[keyIdx * keySize];
        float keyTime = *reinterpret_cast<const float *>(currKey);

        if (startTime < keyTime) {
            Decode(const_cast<unsigned char *>(currKey + sizeof(float)), stateBuffer);
            if (test.Pass(reinterpret_cast<const State *>(stateBuffer))) {
                resultTime = keyTime;
                return true;
            }
        }
    }

    return false;
}

}; // namespace EAGL4Anim
