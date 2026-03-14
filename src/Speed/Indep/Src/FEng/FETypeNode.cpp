#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

extern const unsigned long FEKeyTypeSize[];

void FEFieldNode::SetDefault(void* pSrc) {
    if (!pDefault) {
        pDefault = new (FEngMalloc(Size, nullptr, 0)) unsigned char[Size];
    }
    FEngMemCpy(pDefault, pSrc, Size);
}

void FEFieldNode::GetDefault(void* pDest) {
    if (pDefault) {
        FEngMemCpy(pDest, pDefault, Size);
    }
}

void FETypeNode::AddField(const char* pName, int Type) {
    FEFieldNode* pField = new (FEngMalloc(sizeof(FEFieldNode), nullptr, 0)) FEFieldNode();
    pField->SetName(pName);
    pField->SetType(Type);
    pField->SetSize(FEKeyTypeSize[Type]);
    AppendField(pField);
    UpdateOffsets();
}

void FETypeNode::UpdateOffsets() {
    unsigned long Offset = 0;
    FEFieldNode* pField = GetFirstField();
    while (pField) {
        pField->SetOffset(Offset);
        Offset += pField->GetSize();
        pField = pField->GetNext();
    }
}

unsigned long FETypeNode::GetTypeSize() {
    unsigned long Size = 0;
    FEFieldNode* pField = GetFirstField();
    while (pField) {
        Size += pField->GetSize();
        pField = pField->GetNext();
    }
    return Size;
}

FEFieldNode* FETypeNode::GetField(const char* pName) {
    FEFieldNode* pNode = GetFirstField();
    while (pNode) {
        if (FEngStrICmp(pNode->GetName(), pName) == 0) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return nullptr;
}