#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned long GetStringLength(const short* pString) {
    if (!pString) {
        return 0;
    }

    unsigned long length = 0;

    if (*pString == 0) {
        return 0;
    }

    do {
        length++;
    } while (pString[length] != 0);

    return length;
}

template <class T> void CopyString(short* pDst, const T* pSrc) {
    if (!pDst) {
        return;
    }

    if (pSrc) {
        unsigned short value = *pSrc;

        while (value != 0) {
            *pDst = value;
            pSrc++;
            pDst++;
            value = *pSrc;
        }
    }

    *pDst = 0;
}

template <class T> void CopyString(short* pDst, const T* pSrc, unsigned long ulMaxLength) {
    if (!pDst) {
        return;
    }

    if (ulMaxLength == 0) {
        return;
    }

    if (pSrc) {
        T ch = *pSrc;
        unsigned long length = 0;
        ulMaxLength--;
        if (ch != 0 && ulMaxLength != 0) {
            do {
                length++;
                *pDst = *pSrc;
                pDst++;
                pSrc++;
                if (*pSrc == 0) {
                    break;
                }
            } while (ulMaxLength != length);
        }
    }

    *pDst = 0;
}

FEWideString::FEWideString() {
    mpsString = 0;
    mulBufferLength = 0;
    mulBufferLength = Length();
}

FEWideString::FEWideString(const FEWideString& string) {
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

FEWideString& FEWideString::operator=(const FEWideString& string) {
    if (string.mpsString) {
        short* pString = AllocateString(GetStringLength(string.mpsString) + 1);

        mpsString = pString;
        CopyString(pString, string.mpsString);
    }

    return *this;
}

FEWideString& FEWideString::operator=(const short* psString) {
    if (!psString) {
        return *this;
    }

    short* pString = AllocateString(GetStringLength(psString) + 1);

    mpsString = pString;
    CopyString(pString, psString);

    return *this;
}

unsigned long FEWideString::Length() const { return GetStringLength(mpsString); }

void FEWideString::SetLength(const unsigned long newLength) {
    unsigned long length = Length();

    if (newLength > length) {
        short* pString;

        mulBufferLength = newLength;
        pString = static_cast<short*>(FEngMalloc((newLength + 1) * 2, 0, 0));
        CopyString(pString, mpsString);
        if (mpsString) {
            delete[] mpsString;
        }
        mpsString = pString;
    }
}

short* FEWideString::AllocateString(const unsigned long newLength) {
    if (newLength > mulBufferLength) {
        SetLength(newLength);
    }

    return mpsString;
}

template void CopyString<short>(short* pDst, const short* pSrc);
template void CopyString<short>(short* pDst, const short* pSrc, unsigned long ulMaxLength);
