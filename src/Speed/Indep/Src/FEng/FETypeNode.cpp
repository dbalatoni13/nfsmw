#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

const u32 FEKeyTypeSize[7] = {4, 4, 4, 8, 12, 16, 16}; // size: 0x1C, address: 0x803EA920, Decl: speed/indep/src/feng/FEKeyTypes.cpp:11

const bool FEKeyInterpValid[7][5] = { // size: 0x23, address: 0xFFFFFFFF, Decl: speed/indep/src/feng/FEKeyTypes.cpp:21
    {1, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {1, 0, 0, 0, 1}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}};

i32 FEKeyInterpDefault[7] = {0, 1, 1, 1, 1, 1, 1}; // size: 0x1C, address: 0xFFFFFFFF, Decl: speed/indep/src/feng/FEKeyTypes.cpp:32

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
    if (pDefault) {
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
    u32 Size = 0;
    FEFieldNode *pField = GetFirstField();
    while (pField) {
        Size += pField->GetSize();
        pField = pField->GetNext();
    }
    return Size;
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
