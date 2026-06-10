#ifndef FESTRING_H_
#define FESTRING_H_

#include <types.h>
#include "FEObject.h"
#include "FEWideString.h"

class FEString;

typedef enum {
    _FESTRING_JUSTIFY_HCENTER = 0,
    _FESTRING_JUSTIFY_HRIGHT = 1,
    _FESTRING_JUSTIFY_VCENTER = 2,
    _FESTRING_JUSTIFY_VBOTTOM = 3,
    _FESTRING_FORMAT_WORDWRAP = 4,
    FESTRING_JUSTIFY_FORCEU32 = -1
} FEStringFormatting;

class FELabelCallback {
  public:
    virtual void OnLabelChanged(FEString *text) = 0;
};

// File: speed/indep/src/feng/FEString.h
// total size: 0x78
// Decl: speed/indep/src/feng/FEString.h:59
class FEString : public FEObject {
  private:
    char *pLabelName; // offset 0x5C, size 0x4, Decl: speed/indep/src/feng/FEString.h:61
    u32 LabelHash;    // offset 0x60, size 0x4, Decl: speed/indep/src/feng/FEString.h:62

    static FELabelCallback *pLabelCallback; // size: 0x4, address: 0x8041D15C, Decl: speed/indep/src/feng/FEString.cpp:16

  public:
    FEString() // Decl: speed/indep/src/feng/FEString.h:67
        : FEObject(), pLabelName(nullptr), LabelHash(0xFFFFFFFF), string(), Format(0), Leading(0), MaxWidth(0) {}
    FEString(const FEString &String, bool bReference); // Decl: speed/indep/src/feng/FEString.h:67

    ~FEString() override {} // Decl: speed/indep/src/feng/FEString.h:69

    FEWideString string; // offset 0x64, size 0x8, Decl: speed/indep/src/feng/FEString.h:72
    u32 Format;          // offset 0x6C, size 0x4, Decl: speed/indep/src/feng/FEString.h:73
    i32 Leading;         // offset 0x70, size 0x4, Decl: speed/indep/src/feng/FEString.h:74
    u32 MaxWidth;        // offset 0x74, size 0x4, Decl: speed/indep/src/feng/FEString.h:75

    i16 *GetString() { // Decl: speed/indep/src/feng/FEString.h:78
        return string.mpsString;
    }

    void SetString(short *pNewText) {
        string = pNewText;
    }

    void SetString(const char *pcString) {
        string = pcString;
    }

    void SetStringFromUTF8(const char *pUTF8String) {}

    void SetLabel(const char *pString);

    char *const GetLabel() const {}

    u32 GetLabelHash() { // Decl: speed/indep/src/feng/FEString.h:94
        return LabelHash;
    }

    void SetLabelHash(u32 Hash) { // Decl: speed/indep/src/feng/FEString.h:97
        Flags |= 0x400000;
        LabelHash = Hash;
        if (pLabelCallback) {
            pLabelCallback->OnLabelChanged(this);
        }
    }

    FEWideString &GetWideString() {} // Decl: speed/indep/src/feng/FEString.h:106

    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FEString.h:108

    static void SetLabelCallback(FELabelCallback *pCallback) {} // Decl: speed/indep/src/feng/FEString.h:110
};

#endif
