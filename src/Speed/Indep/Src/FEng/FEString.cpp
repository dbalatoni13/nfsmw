#include <new>

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FELabelCallback* FEString::pLabelCallback;

FEString::FEString(const FEString& String, bool bReference)
    : FEObject(String, bReference), pLabelName(0), string(String.string), Format(String.Format), Leading(String.Leading),
      MaxWidth(String.MaxWidth) {
    SetLabel(String.pLabelName);
    string.SetLength(String.string.mulBufferLength);
}

FEString::~FEString() {
    if (pLabelName) {
        delete[] pLabelName;
    }
}

FEObject* FEString::Clone(bool bReference) {
    FEString* pString = static_cast<FEString*>(FEngMalloc(sizeof(FEString), 0, 0));
    new (pString) FEString(*this, bReference);
    return pString;
}

void FEString::SetLabel(const char* pString) {
    if (pLabelName) {
        delete[] pLabelName;
    }

    pLabelName = 0;
    if (pString) {
        pLabelName = static_cast<char*>(FEngMalloc(FEngStrLen(pString) + 1, 0, 0));
        FEngStrCpy(pLabelName, pString);
    }

    LabelHash = FEHashUpper(pLabelName);
    Flags |= 0x400000;
    if (pLabelCallback) {
        pLabelCallback->OnLabelChanged(this);
    }
}
