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
    return FENG_NEW FEString(*this, bReference);
}

void FEString::SetLabel(const char* pString) {
    if (pLabelName) {
        delete[] pLabelName;
    }

    pLabelName = 0;
    if (pString) {
        unsigned long Len = FEngStrLen(pString) + 1;

        pLabelName = FENG_NEW char[Len];
        FEngStrCpy(pLabelName, pString);
    }

    LabelHash = FEHashUpper(pLabelName);
    Flags |= 0x400000;
    if (pLabelCallback) {
        pLabelCallback->OnLabelChanged(this);
    }
}
