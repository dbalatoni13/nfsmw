#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

// c36feng: FEKeyTypeSize y FEKeyInterpValid se han movido a FEKeyTypes.cpp,
// que es donde el DWARF los declara y donde su direccion (+0x138) los pone.
extern const u32 FEKeyTypeSize[7];

void FEFieldNode::SetDefault(void *pSrc) {
    if (pDefault) {
        delete[] pDefault;
    }
    pDefault = nullptr;
    if (Size != 0) {
        pDefault = FNEW u8[Size];
        FEngMemCpy(pDefault, pSrc, Size);
    }
}

void FEFieldNode::GetDefault(void *pDest) {
    if (pDefault != nullptr) {
        FEngMemCpy(pDest, pDefault, Size);
    }
}

void FETypeNode::AddField(const char *pName, i32 iType) {
    FEFieldNode *pField;
    pField = FNEW FEFieldNode();
    pField->SetName(pName);
    pField->SetType(iType);
    pField->SetSize(FEKeyTypeSize[iType]);
    AppendField(pField);
    UpdateOffsets();
}

void FETypeNode::UpdateOffsets() {
    u32 Offset = 0;
    FEFieldNode *pField = GetFirstField();
    while (pField) {
        pField->SetOffset(Offset);
        Offset += pField->GetSize();
        pField = pField->GetNext();
    }
}

u32 FETypeNode::GetTypeSize() {
    FEFieldNode *pField = GetFirstField();
    u32 Result = 0;
    while (pField != nullptr) {
        Result += pField->GetSize();
        pField = pField->GetNext();
    }
    return Result;
}

FEFieldNode *FETypeNode::GetField(const char *pName) {
    FEFieldNode *pNode = GetFirstField();
    while (pNode) {
        if (FEngStrICmp(pNode->GetName(), pName) == 0) {
            break;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}
