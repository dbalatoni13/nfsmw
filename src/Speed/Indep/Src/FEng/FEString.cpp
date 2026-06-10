// #include <new>

#include "Speed/Indep/Src/FEng/FEString.h"
#include "FEObject.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FELabelCallback *FEString::pLabelCallback;

FEString::FEString(const FEString &String, bool bReference)
    : FEObject(reinterpret_cast<const FEObject &>(String), bReference), pLabelName(0), string(String.string), Format(String.Format),
      Leading(String.Leading), MaxWidth(String.MaxWidth) {
    SetLabel(String.pLabelName);
    string.SetLength(String.string.mulBufferLength);
}

FEString::~FEString() {
    if (pLabelName) {
        delete[] pLabelName;
    }
}

FEObject *FEString::Clone(bool bReference) {
    return reinterpret_cast<FEObject *>(FNEW FEString(*this, bReference));
}

void FEString::SetLabel(const char *pString) {
    if (pLabelName) {
        delete[] pLabelName;
    }

    pLabelName = 0;
    if (pString) {
        unsigned long Len = FEngStrLen(pString) + 1;

        pLabelName = FNEW char[Len];
        FEngStrCpy(pLabelName, pString);
    }

    LabelHash = FEHashUpper(pLabelName);
    Flags |= 0x400000;
    if (pLabelCallback) {
        pLabelCallback->OnLabelChanged(this);
    }
}
