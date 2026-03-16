#ifndef _FESTRING
#define _FESTRING

#include "FEObject.h"
#include "FEWideString.h"

struct FELabelCallback;

// total size: 0x78
struct FEString : public FEObject {
    static FELabelCallback* pLabelCallback;

private:
    char* pLabelName;          // offset 0x5C, size 0x4
    unsigned long LabelHash;   // offset 0x60, size 0x4

public:
    FEWideString string;       // offset 0x64, size 0x8
    unsigned long Format;      // offset 0x6C, size 0x4
    int Leading;               // offset 0x70, size 0x4
    unsigned long MaxWidth;    // offset 0x74, size 0x4

    inline FEString()
        : FEObject() //
        , pLabelName(nullptr) //
        , LabelHash(0xFFFFFFFF) //
        , string() //
        , Format(0) //
        , Leading(0) //
        , MaxWidth(0)
    {
    }
    FEString(const FEString& String, bool bReference);
    ~FEString() override;

    inline short* GetString();
    inline void SetString(short* pNewText);
    inline void SetString(const char* pcString);
    inline void SetStringFromUTF8(const char* pUTF8String) {}
    inline const char* GetLabel() const;
    inline unsigned long GetLabelHash();
    inline void SetLabelHash(unsigned long Hash);
    inline FEWideString& GetWideString();

    void SetLabel(const char* pString);

    FEObject* Clone(bool bReference) override;

    static inline void SetLabelCallback(FELabelCallback* pCallback);
};

struct FELabelCallback {
    virtual void OnLabelChanged(FEString* text) = 0;
};

inline void FEString::SetLabelHash(unsigned long Hash) {
    Flags |= 0x400000;
    LabelHash = Hash;
    if (pLabelCallback) {
        pLabelCallback->OnLabelChanged(this);
    }
}

inline unsigned long FEString::GetLabelHash() {
    return LabelHash;
}

inline short *FEString::GetString() {
    return string.mpsString;
}

#endif
