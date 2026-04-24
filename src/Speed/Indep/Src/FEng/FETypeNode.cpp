#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

extern const unsigned long FEKeyTypeSize[];

FEFieldNode::~FEFieldNode() {
    if (pDefault) {
        delete[] pDefault;
    }
}

void FEFieldNode::SetDefault(void* pSrc) {
    if (pDefault) {
        delete[] pDefault;
    }
    pDefault = nullptr;
    if (Size != 0) {
        pDefault = reinterpret_cast<unsigned char*>(FENG_NEW char[Size]);
        FEngMemCpy(pDefault, pSrc, Size);
    }
}

void FEFieldNode::GetDefault(void* pDest) {
    if (pDefault) {
        FEngMemCpy(pDest, pDefault, Size);
    }
}

void FETypeNode::AddField(const char* pName, long iType) {
    FEFieldNode* pField;
    pField = FENG_NEW FEFieldNode();
    pField->SetName(pName);
    pField->SetType(iType);
    pField->SetSize(FEKeyTypeSize[iType]);
    Fields.AddNode(static_cast<FEMinNode*>(Fields.GetTail()), pField);
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
            break;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}
