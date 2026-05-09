#ifndef FENG_FETYPENODE_H
#define FENG_FETYPENODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include <types.h>

#include "Speed/Indep/Src/FEng/FEList.h"
// struct FEFieldNode;
// struct FETypeNode;

// total size: 0x28
class FEFieldNode;

class FETypeNode : public FENode {
  public:
    FEMinList Fields; // offset 0x14, size 0x10
    u32 TypeID;       // offset 0x24, size 0x4

    inline FETypeNode *GetNext() {
        return static_cast<FETypeNode *>(FENode::GetNext());
    }
    inline FETypeNode *GetPrev() {
        return static_cast<FETypeNode *>(FENode::GetPrev());
    }
    inline FEFieldNode *GetFirstField() {
        return reinterpret_cast<FEFieldNode *>(Fields.GetHead());
    };
    inline u32 GetID() {
        return TypeID;
    }
    inline void SetID(u32 ID) {
        TypeID = ID;
    }

    void AddField(const char *pName, i32 Type);
    void UpdateOffsets();
    u32 GetTypeSize();
    FEFieldNode *GetField(const char *pName);
    inline FEFieldNode *GetField(i32 Index) {
        return reinterpret_cast<FEFieldNode *>(Fields.FindNode(static_cast<u32>(Index)));
    };
};

class FEFieldNode : public FENode {
  public:
    i32 Type;     // offset 0x14, size 0x4
    u32 Size;     // offset 0x18, size 0x4
    u32 Offset;   // offset 0x1C, size 0x4
    u8 *pDefault; // offset 0x20, size 0x4

    inline FEFieldNode() : Size(0), Offset(0), pDefault(nullptr) {}
    ~FEFieldNode() override;

    inline i32 GetType() const {
        return Type;
    }
    inline void SetType(i32 NewType) {
        Type = NewType;
    }
    inline u32 GetSize() const {
        return Size;
    }
    inline void SetSize(u32 Val) {
        Size = Val;
    }
    inline u32 GetOffset() const {
        return Offset;
    }
    inline void SetOffset(u32 Val) {
        Offset = Val;
    }
    inline const void *GetDefaultPtr() {
        return pDefault;
    }
    inline FEFieldNode *GetNext() const {
        return static_cast<FEFieldNode *>(FENode::GetNext());
    }
    inline FEFieldNode *GetPrev() const {
        return static_cast<FEFieldNode *>(FENode::GetPrev());
    }

    void SetDefault(void *pSrc);
    void GetDefault(void *pDest);
};

#endif
