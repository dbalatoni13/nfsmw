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

#define ESCAPE_CHARACTER '\\' // :87

// File: speed/indep/src/feng/FEWideString.h
// total size: 0x8
// Decl: speed/indep/src/feng/FEWideString.h:94
class FEWideString {
  public:
    i16 *mpsString;      // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEWideString.h:97
    u32 mulBufferLength; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEWideString.h:98

    FEWideString(); // Decl: speed/indep/src/feng/FEWideString.h:100
    FEWideString(const short *psString);
    FEWideString(const char *pcString);
    FEWideString(const FEWideString &string);
    ~FEWideString(); // Decl: speed/indep/src/feng/FEWideString.h:104

    bool operator==(const FEWideString &string); // Decl: speed/indep/src/feng/FEWideString.h:106

    FEWideString &operator=(const FEWideString &string);
    FEWideString &operator=(const short *psString);

    FEWideString &operator+=(const FEWideString &string);
    FEWideString &operator+=(const short *pString);
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

#endif
