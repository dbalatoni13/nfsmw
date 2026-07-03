#ifndef FETYPENODE_H_
#define FETYPENODE_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEList.h"

// total size: 0x28
class FEFieldNode;

typedef enum {
    PT_Bool = 0,
    PT_Int = 1,
    PT_Float = 2,
    PT_Vector2 = 3,
    PT_Vector3 = 4,
    PT_Quaternion = 5,
    PT_Color = 6,
    PT_ParamTypeCount = 7
} FEParamType;

typedef enum { IT_None = 0, IT_Linear = 1, IT_Spline = 2, IT_MoveToLinear = 3, IT_MoveToSpline = 4, IT_InterpTypeCount = 5 } FEInterpMethod;

typedef enum { AT_Once = 0, AT_Loop = 1, AT_PingPong = 2, AT_ActionTypeCount = 3 } FEPlayActions;

// File: speed/indep/src/feng/FETypeNode.h
// total size: 0x24
// Decl: speed/indep/src/feng/FETypeNode.h:25
class FEFieldNode : public FENode {
  private:
    i32 Type;     // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FETypeNode.h:27
    u32 Size;     // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FETypeNode.h:28
    u32 Offset;   // offset 0x1C, size 0x4, Decl: speed/indep/src/feng/FETypeNode.h:29
    u8 *pDefault; // offset 0x20, size 0x4, Decl: speed/indep/src/feng/FETypeNode.h:30

  public:
    FEFieldNode() : Size(0), Offset(0), pDefault(nullptr) {} // Decl: speed/indep/src/feng/FETypeNode.h:33
    ~FEFieldNode() override {                                // Decl: speed/indep/src/feng/FETypeNode.h:34
        if (pDefault) {
            delete[] pDefault;
        }
    }

    i32 GetType() const {
        return Type;
    }
    void SetType(i32 NewType) { // Decl: speed/indep/src/feng/FETypeNode.h:37
        Type = NewType;
    }

    u32 GetSize() const {
        return Size;
    }
    void SetSize(u32 Val) { // Decl: speed/indep/src/feng/FETypeNode.h:40
        Size = Val;
    }

    u32 GetOffset() const {
        return Offset;
    }
    void SetOffset(u32 Val) { // Decl: speed/indep/src/feng/FETypeNode.h:43
        Offset = Val;
    }

    void SetDefault(void *pSrc);

    void GetDefault(void *pDest);

    void *const GetDefault() { // Decl: speed/indep/src/feng/FETypeNode.h:49
        return pDefault;
    }

    struct FEFieldNode *GetNext() const { // Decl: speed/indep/src/feng/FETypeNode.h:52
        return static_cast<FEFieldNode *>(FENode::GetNext());
    }
    struct FEFieldNode *GetPrev() const {
        return static_cast<FEFieldNode *>(FENode::GetPrev());
    }
};

// total size: 0x28
// Decl: speed/indep/src/feng/FETypeNode.h:65
class FETypeNode : public FENode {
  private:
    FEMinList Fields; // offset 0x14, size 0x10, Decl: speed/indep/src/feng/FETypeNode.h:67
    u32 TypeID;       // offset 0x24, size 0x4, Decl: speed/indep/src/feng/FETypeNode.h:68

  public:
    void InsertField(FEFieldNode *pField, FEFieldNode *pInsertAfter) {} // Decl: speed/indep/src/feng/FETypeNode.h:71
    void AppendField(FEFieldNode *pField) {                             // Decl: speed/indep/src/feng/FETypeNode.h:72
        Fields.AddTail(pField);
    }
    void RemoveField(FEFieldNode *pField) { // Decl: speed/indep/src/feng/FETypeNode.h:73
        Fields.RemNode(pField);
    }

    void AddField(const char *pName, i32 Type);

    void UpdateOffsets();

    i32 GetFieldCount() { // Decl: speed/indep/src/feng/FETypeNode.h:78
        return Fields.GetNumElements();
    }

    FEFieldNode *GetField(i32 Index) { // Decl: speed/indep/src/feng/FETypeNode.h:80
        return reinterpret_cast<FEFieldNode *>(Fields.FindNode(static_cast<u32>(Index)));
    }

    FEFieldNode *GetField(const char *pName); // Decl: speed/indep/src/feng/FETypeNode.h:80

    FEFieldNode *GetFirstField() { // Decl: speed/indep/src/feng/FETypeNode.h:81
        return reinterpret_cast<FEFieldNode *>(Fields.GetHead());
    }
    i32 GetFieldIndex(FEFieldNode *pNode) { // Decl: speed/indep/src/feng/FETypeNode.h:82
        return Fields.ElementNumber(pNode);
    }

    u32 GetID() { // Decl: speed/indep/src/feng/FETypeNode.h:84
        return TypeID;
    }
    void SetID(u32 ID) { // Decl: speed/indep/src/feng/FETypeNode.h:85
        TypeID = ID;
    }

    u32 GetTypeSize();

    FETypeNode *GetNext() { // Decl: speed/indep/src/feng/FETypeNode.h:89
        return static_cast<FETypeNode *>(FENode::GetNext());
    }
    FETypeNode *GetPrev() { // Decl: speed/indep/src/feng/FETypeNode.h:90
        return static_cast<FETypeNode *>(FENode::GetPrev());
    }
};

#endif
