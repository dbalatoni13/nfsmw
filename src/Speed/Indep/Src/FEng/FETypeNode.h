#ifndef FENG_FETYPENODE_H
#define FENG_FETYPENODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEPackage.h"

// total size: 0x24
struct FEFieldNode : public FENode {
    int Type;              // offset 0x14, size 0x4
    unsigned long Size;    // offset 0x18, size 0x4
    unsigned long Offset;  // offset 0x1C, size 0x4
    unsigned char* pDefault; // offset 0x20, size 0x4

    inline FEFieldNode() : Type(0), Size(0), Offset(0), pDefault(nullptr) {}
    ~FEFieldNode() override {}

    inline int GetType() const { return Type; }
    inline void SetType(int NewType) { Type = NewType; }
    inline unsigned long GetSize() const { return Size; }
    inline void SetSize(unsigned long Val) { Size = Val; }
    inline unsigned long GetOffset() const { return Offset; }
    inline void SetOffset(unsigned long Val) { Offset = Val; }
    inline const void* GetDefault() { return pDefault; }
    inline FEFieldNode* GetNext() const { return static_cast<FEFieldNode*>(FENode::GetNext()); }
    inline FEFieldNode* GetPrev() const { return static_cast<FEFieldNode*>(FENode::GetPrev()); }

    void SetDefault(void* pSrc);
    void GetDefault(void* pDest);
};

// total size: 0x28
struct FETypeNode : public FENode {
    FEMinList Fields;    // offset 0x14, size 0x10
    unsigned long TypeID; // offset 0x24, size 0x4

    inline FETypeNode() : TypeID(0) {}
    ~FETypeNode() override {}

    inline void InsertField(FEFieldNode* pField, FEFieldNode* pInsertAfter) { Fields.AddNode(pInsertAfter, pField); }
    inline void AppendField(FEFieldNode* pField) { Fields.AddTail(pField); }
    inline void RemoveField(FEFieldNode* pField) { Fields.RemNode(pField); }
    inline int GetFieldCount() { return Fields.GetNumElements(); }
    inline FEFieldNode* GetField(int Index) { return static_cast<FEFieldNode*>(Fields.GetIndex(Index)); }
    inline FEFieldNode* GetFirstField() { return static_cast<FEFieldNode*>(Fields.GetHead()); }
    inline unsigned long GetID() { return TypeID; }
    inline void SetID(unsigned long ID) { TypeID = ID; }
    inline FETypeNode* GetNext() { return static_cast<FETypeNode*>(FENode::GetNext()); }
    inline FETypeNode* GetPrev() { return static_cast<FETypeNode*>(FENode::GetPrev()); }

    void AddField(const char* pName, int Type);
    void UpdateOffsets();
    unsigned long GetTypeSize();
    FEFieldNode* GetField(const char* pName);
};

#endif
