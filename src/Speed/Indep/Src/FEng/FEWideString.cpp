#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

u32 GetStringLength(const i16 *pString) {
    if (!pString) {
        return 0;
    }

    u32 i = 0;

    if (*pString == 0) {
        return 0;
    }

    do {
        i++;
    } while (pString[i] != 0);

    return i;
}

FEWideString::FEWideString() {
    mpsString = 0;
    mulBufferLength = 0;
    mulBufferLength = Length();
}

FEWideString::FEWideString(const FEWideString &string) {
    mpsString = 0;
    mulBufferLength = 0;
    *this = string;
    mulBufferLength = Length();
}

FEWideString::~FEWideString() {
    if (mpsString) {
        delete[] mpsString;
    }
}

FEWideString &FEWideString::operator=(const FEWideString &string) {
    if (string.mpsString) {
        mpsString = AllocateString(GetStringLength(string.mpsString) + 1);
        CopyString(mpsString, string.mpsString);
    }

    return *this;
}

FEWideString &FEWideString::operator=(const i16 *psString) {
    if (!psString) {
        return *this;
    }

    mpsString = AllocateString(GetStringLength(psString) + 1);
    CopyString(mpsString, psString);

    return *this;
}

u32 FEWideString::Length() const {
    return GetStringLength(mpsString);
}

void FEWideString::SetLength(const u32 newLength) {
    u32 length = Length();

    if (newLength > length) {
        mulBufferLength = newLength;
        i16 *psNewString = FNEW i16[newLength + 1];
        CopyString(psNewString, mpsString);
        if (mpsString) {
            delete[] mpsString;
        }
        mpsString = psNewString;
    }
}

i16 *FEWideString::AllocateString(const u32 newLength) {
    if (newLength > mulBufferLength) {
        SetLength(newLength);
    }

    return mpsString;
}
