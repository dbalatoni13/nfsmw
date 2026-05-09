#ifndef _FESTRING
#define _FESTRING

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "FEObject.h"
#include "FEWideString.h"

class FEString;

class FELabelCallback {
  public:
    virtual void OnLabelChanged(FEString *text) = 0;
};

// total size: 0x78
class FEString : public FEObject {
  public:
    inline FEString() : FEObject(), pLabelName(nullptr), LabelHash(0xFFFFFFFF), string(), Format(0), Leading(0), MaxWidth(0) {}
    FEString(const FEString &String, bool bReference);
    ~FEString() override;

    inline i16 *GetString() {
        return string.mpsString;
    };
    inline void SetString(i16 *pNewText) {
        string = pNewText;
    };
    inline void SetString(const char *pcString) {
        string = pcString;
    };
    inline void SetStringFromUTF8(const char *pUTF8String) {}
    void SetLabel(const char *pString);
    inline const char *GetLabel() const;
    inline u32 GetLabelHash() {
        return LabelHash;
    };
    inline void SetLabelHash(u32 Hash) {
        Flags |= 0x400000;
        LabelHash = Hash;
        if (pLabelCallback) {
            pLabelCallback->OnLabelChanged(this);
        }
    };
    inline FEWideString &GetWideString();

    FEObject *Clone(bool bReference) override;

    static inline void SetLabelCallback(FELabelCallback *pCallback);

  private:
    char *pLabelName; // offset 0x5C, size 0x4
    u32 LabelHash;    // offset 0x60, size 0x4

    static FELabelCallback *pLabelCallback;

  public:
    FEWideString string; // offset 0x64, size 0x8
    u32 Format;          // offset 0x6C, size 0x4
    i32 Leading;         // offset 0x70, size 0x4
    u32 MaxWidth;        // offset 0x74, size 0x4
};

#endif
