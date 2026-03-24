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
        char decodeInfo[3];
        decodeInfo[0] = static_cast<char>(decodeData >> 13);
        decodeInfo[1] = static_cast<char>(((decodeData >> 11) & 3) + 1);
        decodeInfo[2] = static_cast<char>(decodeData);

        if (decodeInfo[0] == 2) {
            goto Decode4Bits;
        } else if (decodeInfo[0] > 2) {
            goto DecodeWide;
        } else if (decodeInfo[0] == 0) {
            goto Decode1Bit;
        } else if (decodeInfo[0] == 1) {
            goto Decode2Bits;
        }
        goto DecodeDone;

    DecodeWide:
        if (decodeInfo[0] == 4) {
            goto DecodeU16;
        }
        if (decodeInfo[0] < 4) {
            goto DecodeU8;
        }
        if (decodeInfo[0] != 5) {
            goto DecodeDone;
        }
        value = *reinterpret_cast<unsigned int *>(src);
        src += 4;
        goto DecodeDone;

    DecodeU16:
        value = *reinterpret_cast<unsigned short *>(src);
        src += 2;
        goto DecodeDone;

    DecodeU8:
        value = *src;
        src += 1;
        goto DecodeDone;

    Decode4Bits:
        numBits = (numBits + 4) & 0xFF;
        value = (*src >> (8 - numBits)) & 0xF;
        goto DecodeDone;

    Decode2Bits:
        numBits = (numBits + 2) & 0xFF;
        value = (*src >> (8 - numBits)) & 3;
        goto DecodeDone;

    Decode1Bit:
        numBits = (numBits + 1) & 0xFF;
        value = (*src >> (8 - numBits)) & 1;

    DecodeDone:

        if (numBits > 7) {
            src += 1;
            numBits = 0;
        }

        if (static_cast<unsigned char>(decodeInfo[1]) == 2) {
            goto Store2Bytes;
        } else if (static_cast<unsigned char>(decodeInfo[1]) > 2) {
            goto StoreWide;
        } else if (static_cast<unsigned char>(decodeInfo[1]) == 1) {
            goto Store1Byte;
        }
        goto StoreDone;

    StoreWide:
        if (static_cast<unsigned char>(decodeInfo[1]) != 4) {
            goto StoreDone;
        }
        *reinterpret_cast<unsigned int *>(&dest[static_cast<unsigned char>(decodeInfo[2])]) = value;
        goto StoreDone;

    Store2Bytes:
        *reinterpret_cast<unsigned short *>(&dest[static_cast<unsigned char>(decodeInfo[2])]) =
            static_cast<unsigned short>(value);
        goto StoreDone;

    Store1Byte:
        dest[static_cast<unsigned char>(decodeInfo[2])] = static_cast<unsigned char>(value);

    StoreDone:
        ;
    }
}

bool FnRawStateChan::EvalState(float time, State *s) {
    RawStateChan *rawStateChan = reinterpret_cast<RawStateChan *>(mpAnim);
    unsigned char numFields = rawStateChan->GetNumFields();
    unsigned char keySize = rawStateChan->GetKeySize();
    unsigned short numKeys;
    unsigned char *keyData;
    int keyIdx = mKeyIdx;
    unsigned char *currKey;

    if ((numFields & 1) == 0) {
        keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 12;
    } else {
        keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 10;
    }

    if (*reinterpret_cast<float *>(&keyData[keyIdx * keySize]) <= time) {
        numKeys = rawStateChan->GetNumKeys();

        if (keyIdx < numKeys) {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 10;

            do {
                currKey = keyData + 2;
                if ((numFields & 1) != 0) {
                    currKey = keyData;
                }
                currKey += keyIdx * keySize;

                if (time < *reinterpret_cast<float *>(currKey + keySize)) {
                    goto KeyFound;
                }

                keyIdx++;
            } while (keyIdx < numKeys);
        }

        if ((numFields & 1) == 0) {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 12;
        } else {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 10;
        }

        Decode(keyData + keySize * (numKeys - 1) + sizeof(float), reinterpret_cast<unsigned char *>(s));
        mKeyIdx = numKeys - 1;
    } else {
        keyIdx--;

        if (keyIdx > -1) {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 10;

            do {
                currKey = keyData + 2;
                if ((numFields & 1) != 0) {
                    currKey = keyData;
                }
                currKey += keyIdx * keySize;

                if (*reinterpret_cast<float *>(currKey) <= time) {
                    goto KeyFound;
                }

                keyIdx--;
            } while (keyIdx > -1);
        }

        if ((numFields & 1) == 0) {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 12;
        } else {
            keyData = reinterpret_cast<unsigned char *>(rawStateChan) + numFields * sizeof(unsigned short) + 10;
        }

        Decode(keyData + sizeof(float), reinterpret_cast<unsigned char *>(s));
        mKeyIdx = 0;
    }

    return true;

KeyFound:
    Decode(currKey + sizeof(float), reinterpret_cast<unsigned char *>(s));
    mKeyIdx = keyIdx;
    return true;
}

bool FnRawStateChan::FindTime(const StateTest &test, float startTime, float &resultTime) {
    const RawStateChan *rawStateChan = reinterpret_cast<const RawStateChan *>(mpAnim);
    unsigned char stateBuffer[84];
    int keyIdx = 0;
    unsigned short numKeys = rawStateChan->GetNumKeys();

    if (numKeys != 0) {
        unsigned char numFields = rawStateChan->GetNumFields();

        while (true) {
            int keyDataOffset;
            const float *currKey;
            float keyTime;

            if ((numFields & 1) == 0) {
                keyDataOffset = rawStateChan->GetNumFields() * sizeof(unsigned short) + 12;
            } else {
                keyDataOffset = rawStateChan->GetNumFields() * sizeof(unsigned short) + 10;
            }

            currKey = reinterpret_cast<const float *>(
                reinterpret_cast<const unsigned char *>(rawStateChan) + keyDataOffset + keyIdx * rawStateChan->GetKeySize());
            keyTime = *currKey;

            if (startTime < keyTime) {
                Decode(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(currKey + 1)), stateBuffer);
                if (test.Pass(reinterpret_cast<const State *>(stateBuffer))) {
                    resultTime = keyTime;
                    return true;
                }

                numKeys = rawStateChan->GetNumKeys();
            }

            keyIdx++;
            if (numKeys <= keyIdx) {
                break;
            }

            numFields = rawStateChan->GetNumFields();
        }
    }

    return false;
}

}; // namespace EAGL4Anim
