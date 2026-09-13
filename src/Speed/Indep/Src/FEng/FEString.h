#ifndef FESTRING_H_
#define FESTRING_H_

#include <types.h>
#include "FEObject.h"
#include "FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

class FEString;

enum FEStringFormatting {
    _FESTRING_JUSTIFY_HCENTER = 0,
    _FESTRING_JUSTIFY_HRIGHT = 1,
    _FESTRING_JUSTIFY_VCENTER = 2,
    _FESTRING_JUSTIFY_VBOTTOM = 3,
    _FESTRING_FORMAT_WORDWRAP = 4,
    FESTRING_JUSTIFY_FORCEU32 = -1,
};

static const u32 FESTRING_JUSTIFY_HCENTER = 1 << _FESTRING_JUSTIFY_HCENTER;                    // size: 0x4, Decl: 36
static const u32 FESTRING_JUSTIFY_HRIGHT = 1 << _FESTRING_JUSTIFY_HRIGHT;                      // size: 0x4, Decl: 37
static const u32 FESTRING_JUSTIFY_VCENTER = 1 << _FESTRING_JUSTIFY_VCENTER;                    // size: 0x4, Decl: 38
static const u32 FESTRING_JUSTIFY_VBOTTOM = 1 << _FESTRING_JUSTIFY_VBOTTOM;                    // size: 0x4, Decl: 39
static const u32 FESTRING_JUSTIFY_HMASK = FESTRING_JUSTIFY_HCENTER | FESTRING_JUSTIFY_HRIGHT;  // size: 0x4, Decl: 41
static const u32 FESTRING_JUSTIFY_VMASK = FESTRING_JUSTIFY_VCENTER | FESTRING_JUSTIFY_VBOTTOM; // size: 0x4, Decl: 42
static const u32 FESTRING_FORMAT_WORDWRAP = 1 << _FESTRING_FORMAT_WORDWRAP;                    // size: 0x4, Decl: 45

class FELabelCallback {
  public:
    virtual void OnLabelChanged(FEString *text) = 0;
};

// total size: 0x78
// Decl: 59
class FEString : public FEObject {
  private:
    char *pLabelName; // offset 0x5C, size 0x4, Decl: 61
    u32 LabelHash;    // offset 0x60, size 0x4, Decl: 62

    static FELabelCallback *pLabelCallback; // size: 0x4, Decl: 64

  public:
    FEString() // Decl: 67
        : FEObject(), pLabelName(nullptr), LabelHash(0xFFFFFFFF), string(), Format(0), Leading(0), MaxWidth(0) {
        Type = FE_String;
    }
    FEString(const FEString &String, bool bReference); // Decl: 67

    ~FEString() override { // Decl: 69
        if (pLabelName != nullptr) {
            delete[] pLabelName;
        }
    }

    FEWideString string; // offset 0x64, size 0x8, Decl: 72
    u32 Format;          // offset 0x6C, size 0x4, Decl: 73
    i32 Leading;         // offset 0x70, size 0x4, Decl: 74
    u32 MaxWidth;        // offset 0x74, size 0x4, Decl: 75

    i16 *GetString() { // Decl: 78
        return string;
    }

    void SetString(i16 *pNewText) {
        string = pNewText;
        Flags |= FF_DirtyCode;
    }

    void SetString(const char *pcString) {
        string = pcString;
        Flags |= FF_DirtyCode;
    }

    void SetStringFromUTF8(const char *pUTF8String) {}

    void SetLabel(const char *pString);

    const char *GetLabel() const {
        return pLabelName;
    }

    u32 GetLabelHash() { // Decl: 94
        return LabelHash;
    }

    void SetLabelHash(u32 Hash) { // Decl: 97
        Flags |= FF_DirtyCode;
        LabelHash = Hash;
        if (pLabelCallback != nullptr) {
            pLabelCallback->OnLabelChanged(this);
        }
    }

    FEWideString &GetWideString() { // Decl: 106
        return string;
    }

    FEObject *Clone(bool bReference) override { // Decl: 108
        return FNEW FEString(*this, bReference);
    }

    static void SetLabelCallback(FELabelCallback *pCallback) {} // Decl: 110
};

#endif
