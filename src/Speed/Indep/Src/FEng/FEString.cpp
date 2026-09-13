#include "Speed/Indep/Src/FEng/FEString.h"
#include "FEObject.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FELabelCallback *FEString::pLabelCallback; // size: 0x4, address: 0x8041D15C, Decl: 16

FEString::FEString(const FEString &String, bool bReference)
    : FEObject(String, bReference), pLabelName(nullptr), string(String.string), Format(String.Format), Leading(String.Leading),
      MaxWidth(String.MaxWidth) {
    SetLabel(String.GetLabel());
    string.SetLength(String.string.mulBufferLength);
}

void FEString::SetLabel(const char *pString) {
    if (pLabelName != nullptr) {
        delete[] pLabelName;
    }
    pLabelName = nullptr;

    if (pString != nullptr) {
        u32 Len = FEngStrLen(pString) + 1;

        pLabelName = FNEW char[Len];
        FEngStrCpy(pLabelName, pString);
    }

    LabelHash = FEHashUpper(pLabelName);
    Flags |= FF_DirtyCode;
    if (pLabelCallback != nullptr) {
        pLabelCallback->OnLabelChanged(this);
    }
}
