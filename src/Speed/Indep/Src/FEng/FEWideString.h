#ifndef FEWIDESTRING_H_
#define FEWIDESTRING_H_

#include <types.h>

// File: speed/indep/src/feng/FEWideString.cpp
// total size: 0x8
// Decl: speed/indep/src/feng/FEWideString.cpp:48
struct ColorSpecialSequence {
    char *pCharString; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEWideString.cpp:49
    i16 ConvertedCode; // offset 0x4, size 0x2, Decl: speed/indep/src/feng/FEWideString.cpp:50

    bool operator==(const i16 *pString) {}

    bool operator==(const char *pString) {}

    bool operator==(const u8 *pString) {}
};

static const i16 WIDE_CODE_RESERVED = -32768; // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:67
static const i16 WIDE_CODE_MASK = -255;       // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:68
static const i16 WIDE_VALUE_MASK = 255;       // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:69
static const i16 WIDE_MODE_MASK = -16384;     // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:70
static const i16 RED_CODE = -32512;           // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:72
static const i16 GREEN_CODE = -32256;         // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:73
static const i16 BLUE_CODE = -32000;          // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:74
static const i16 ALPHA_CODE = -31744;         // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:75
static const i16 WHOLE_CODE = -31488;         // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:76
static const i16 WHOLEA_CODE = -31232;        // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:77
static const i16 SETR_CODE = -30976;          // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:78
static const i16 SETG_CODE = -30720;          // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:79
static const i16 SETB_CODE = -30464;          // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:80

static const i16 MODCOLOR_CODE = -16128;  // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:82
static const i16 SETCOLOR_CODE = -15872;  // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:83
static const i16 SETCOLORA_CODE = -15616; // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:84
static const i16 ADDCOLOR_CODE = -15360;  // size: 0x2, Decl: speed/indep/src/feng/FEWideString.h:85

#define ESCAPE_CHARACTER '\\' // :87

// File: speed/indep/src/feng/FEWideString.h
// total size: 0x8
// Decl: speed/indep/src/feng/FEWideString.h:94
class FEWideString {
  public:
    i16 *mpsString;      // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEWideString.h:97
    u32 mulBufferLength; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEWideString.h:98

    FEWideString(); // Decl: speed/indep/src/feng/FEWideString.h:100
    FEWideString(const i16 *psString);
    FEWideString(const char *pcString);
    FEWideString(const FEWideString &string);
    ~FEWideString(); // Decl: speed/indep/src/feng/FEWideString.h:104

    bool operator==(const FEWideString &string); // Decl: speed/indep/src/feng/FEWideString.h:106

    FEWideString &operator=(const FEWideString &string);
    FEWideString &operator=(const i16 *psString);
    FEWideString &operator=(const char *pcString);

    FEWideString &operator+=(const FEWideString &string);
    FEWideString &operator+=(const i16 *pString);
    FEWideString &operator+=(const char *pcString);

    void Associate(i16 *pString) {} // Decl: speed/indep/src/feng/FEWideString.h:116
    i16 *Disaccociate() {}          // Decl: speed/indep/src/feng/FEWideString.h:117

    operator i16 *() {
        return mpsString;
    }
    operator i16 const *() const {
        return mpsString;
    }

    u32 Length() const; // Decl: speed/indep/src/feng/FEWideString.h:122

    const i16 operator[](int i) {} // Decl: speed/indep/src/feng/FEWideString.h:123
    const i16 operator[](int i) const {}

    void SetLength(u32 newLength); // Decl: speed/indep/src/feng/FEWideString.h:126

    char *Convert(char *pString) const; // Decl: speed/indep/src/feng/FEWideString.h:128

    i16 *AllocateString(u32 newLength); // Decl: speed/indep/src/feng/FEWideString.h:130

    void ConvertToSpecial(const FEWideString &string) {}

    void ConvertToSpecial(const char *pcString);

    void ConvertToSpecial(const u8 *pcString);

    void ConvertToSpecial(const i16 *psString);

    void ConvertFromSpecial(char *pcString); // Decl: speed/indep/src/feng/FEWideString.h:139
    void ConvertFromSpecial(i16 *psString);
    void ConvertFromSpecial(FEWideString &string); // Decl: speed/indep/src/feng/FEWideString.h:141
};

template <class T>
void CopyString(i16 *pDst, const T *pSrc) // Decl: speed/indep/src/feng/FEWideString.h:149
{
    if (pDst == nullptr) {
        return;
    }
    if (pSrc != nullptr) {
        while (*pSrc != '\0') {
            *pDst = *pSrc;
            pSrc++;
            pDst++;
        }
    }

    *pDst = '\0';
}

template <class T>
void CopyString(i16 *pDst, const T *pSrc, u32 ulMaxLength) // Decl: speed/indep/src/feng/FEWideString.h:171
{
    if (pDst == nullptr) {
        return;
    }
    if (ulMaxLength == 0) {
        return;
    }

    if (pSrc != nullptr) {
        u32 i = 0;
        ulMaxLength--;
        if (*pSrc != 0 && ulMaxLength != 0) {
            do {
                i++;
                *pDst = *pSrc;
                pSrc++;
                pDst++;
                if (*pSrc == '\0') {
                    break;
                }
            } while (ulMaxLength != i);
        }
    }

    *pDst = '\0';
}

u32 GetStringLength(const i16 *pString);

#endif
