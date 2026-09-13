#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

const u32 FEKeyTypeSize[7] = {4, 4, 4, 8, 12, 16, 16}; // size: 0x1C, address: 0x803EA920, Decl: 11

const bool FEKeyInterpValid[7][5] = { // size: 0x23, address: 0xFFFFFFFF, Decl: 21
    {true, false, false, false, false}, {false, false, false, false, false}, {false, false, false, false, false}, {false, false, false, false, false},
    {true, false, false, false, true},  {false, false, false, true, false},  {false, false, true, false, false}};

i32 FEKeyInterpDefault[7] = {0, 1, 1, 1, 1, 1, 1}; // size: 0x1C, address: 0xFFFFFFFF, Decl: 32

void FEFieldNode::SetDefault(void *pSrc) {
    if (pDefault != nullptr) {
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

void FETypeNode::AddField(const char *pName, i32 Type) {
    FEFieldNode *pField;
    pField = FNEW FEFieldNode();
    pField->SetName(pName);
    pField->SetType(Type);
    pField->SetSize(FEKeyTypeSize[Type]);
    AppendField(pField);
    UpdateOffsets();
}

void FETypeNode::UpdateOffsets() {
    u32 Offset = 0;
    FEFieldNode *pField = GetFirstField();
    while (pField != nullptr) {
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
    while (pNode != nullptr) {
        if (FEngStrICmp(pNode->GetName(), pName) == 0) {
            break;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}
