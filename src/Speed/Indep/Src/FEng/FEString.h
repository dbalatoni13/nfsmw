#ifndef FESTRING_H_
#define FESTRING_H_

#include <types.h>
#include "FEObject.h"
#include "FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

class FEString;

typedef enum {
    _FESTRING_JUSTIFY_HCENTER = 0,
    _FESTRING_JUSTIFY_HRIGHT = 1,
    _FESTRING_JUSTIFY_VCENTER = 2,
    _FESTRING_JUSTIFY_VBOTTOM = 3,
    _FESTRING_FORMAT_WORDWRAP = 4,
    FESTRING_JUSTIFY_FORCEU32 = -1
} FEStringFormatting;

static const u32 FESTRING_JUSTIFY_HCENTER = 1 << _FESTRING_JUSTIFY_HCENTER;                    // size: 0x4, Decl: speed/indep/src/feng/FEString.h:36
static const u32 FESTRING_JUSTIFY_HRIGHT = 1 << _FESTRING_JUSTIFY_HRIGHT;                      // size: 0x4, Decl: speed/indep/src/feng/FEString.h:37
static const u32 FESTRING_JUSTIFY_VCENTER = 1 << _FESTRING_JUSTIFY_VCENTER;                    // size: 0x4, Decl: speed/indep/src/feng/FEString.h:38
static const u32 FESTRING_JUSTIFY_VBOTTOM = 1 << _FESTRING_JUSTIFY_VBOTTOM;                    // size: 0x4, Decl: speed/indep/src/feng/FEString.h:39
static const u32 FESTRING_JUSTIFY_HMASK = FESTRING_JUSTIFY_HCENTER | FESTRING_JUSTIFY_HRIGHT;  // size: 0x4, Decl: speed/indep/src/feng/FEString.h:41
static const u32 FESTRING_JUSTIFY_VMASK = FESTRING_JUSTIFY_VCENTER | FESTRING_JUSTIFY_VBOTTOM; // size: 0x4, Decl: speed/indep/src/feng/FEString.h:42
static const u32 FESTRING_FORMAT_WORDWRAP = 1 << _FESTRING_FORMAT_WORDWRAP;                    // size: 0x4, Decl: speed/indep/src/feng/FEString.h:45

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

    static FELabelCallback *pLabelCallback; // size: 0x4, Decl: speed/indep/src/feng/FEString.h:64

  public:
    FEString() // Decl: speed/indep/src/feng/FEString.h:67
        : FEObject(), pLabelName(nullptr), LabelHash(0xFFFFFFFF), string(), Format(0), Leading(0), MaxWidth(0) {
        Type = FE_String;
    }
    FEString(const FEString &String, bool bReference); // Decl: speed/indep/src/feng/FEString.h:67

    ~FEString() override { // Decl: speed/indep/src/feng/FEString.h:69
        if (pLabelName) {
            delete[] pLabelName;
        }
    }

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
        Flags |= FF_DirtyCode;
        LabelHash = Hash;
        if (pLabelCallback) {
            pLabelCallback->OnLabelChanged(this);
        }
    }

    FEWideString &GetWideString() {} // Decl: speed/indep/src/feng/FEString.h:106

    FEObject *Clone(bool bReference) override { // Decl: speed/indep/src/feng/FEString.h:108
        return FNEW FEString(*this, bReference);
    }

    static void SetLabelCallback(FELabelCallback *pCallback) {} // Decl: speed/indep/src/feng/FEString.h:110
};

#endif
