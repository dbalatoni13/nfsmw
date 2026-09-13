#ifndef FETYPENODE_H_
#define FETYPENODE_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEList.h"

// total size: 0x28
class FEFieldNode;

enum FEParamType {
    PT_Bool = 0,
    PT_Int = 1,
    PT_Float = 2,
    PT_Vector2 = 3,
    PT_Vector3 = 4,
    PT_Quaternion = 5,
    PT_Color = 6,
    PT_ParamTypeCount = 7,
};

enum FEInterpMethod {
    IT_None = 0,
    IT_Linear = 1,
    IT_Spline = 2,
    IT_MoveToLinear = 3,
    IT_MoveToSpline = 4,
    IT_InterpTypeCount = 5,
};

enum FEPlayActions {
    AT_Once = 0,
    AT_Loop = 1,
    AT_PingPong = 2,
    AT_ActionTypeCount = 3,
};

// total size: 0x24
// Decl: 25
class FEFieldNode : public FENode {
  private:
    i32 Type;     // offset 0x14, size 0x4, Decl: 27
    u32 Size;     // offset 0x18, size 0x4, Decl: 28
    u32 Offset;   // offset 0x1C, size 0x4, Decl: 29
    u8 *pDefault; // offset 0x20, size 0x4, Decl: 30

  public:
    FEFieldNode() : Size(0), Offset(0), pDefault(nullptr) {} // Decl: 33
    ~FEFieldNode() override {                                // Decl: 34
        if (pDefault != nullptr) {
            delete[] pDefault;
        }
    }

    i32 GetType() const {
        return Type;
    }
    void SetType(i32 NewType) { // Decl: 37
        Type = NewType;
    }

    u32 GetSize() const {
        return Size;
    }
    void SetSize(u32 Val) { // Decl: 40
        Size = Val;
    }

    u32 GetOffset() const {
        return Offset;
    }
    void SetOffset(u32 Val) { // Decl: 43
        Offset = Val;
    }

    void SetDefault(void *pSrc);

    void GetDefault(void *pDest);

    void *const GetDefault() { // Decl: 49
        return pDefault;
    }

    struct FEFieldNode *GetNext() const { // Decl: 52
        return static_cast<FEFieldNode *>(FEMinNode::GetNext());
    }
    struct FEFieldNode *GetPrev() const {
        return static_cast<FEFieldNode *>(FEMinNode::GetPrev());
    }
};

// total size: 0x28
// Decl: 65
class FETypeNode : public FENode {
  private:
    FEMinList Fields; // offset 0x14, size 0x10, Decl: 67
    u32 TypeID;       // offset 0x24, size 0x4, Decl: 68

  public:
    void InsertField(FEFieldNode *pField, FEFieldNode *pInsertAfter) {} // Decl: 71
    void AppendField(FEFieldNode *pField) {                             // Decl: 72
        Fields.AddTail(pField);
    }
    void RemoveField(FEFieldNode *pField) { // Decl: 73
        Fields.RemNode(pField);
    }

    void AddField(const char *pName, i32 Type);

    void UpdateOffsets();

    i32 GetFieldCount() { // Decl: 78
        return Fields.GetNumElements();
    }

    FEFieldNode *GetField(i32 Index) { // Decl: 80
        return reinterpret_cast<FEFieldNode *>(Fields.FindNode(static_cast<u32>(Index)));
    }

    FEFieldNode *GetField(const char *pName); // Decl: 80

    FEFieldNode *GetFirstField() { // Decl: 81
        return reinterpret_cast<FEFieldNode *>(Fields.GetHead());
    }
    i32 GetFieldIndex(FEFieldNode *pNode) { // Decl: 82
        return Fields.ElementNumber(pNode);
    }

    u32 GetID() { // Decl: 84
        return TypeID;
    }
    void SetID(u32 ID) { // Decl: 85
        TypeID = ID;
    }

    u32 GetTypeSize();

    FETypeNode *GetNext() { // Decl: 89
        return static_cast<FETypeNode *>(FEMinNode::GetNext());
    }
    FETypeNode *GetPrev() { // Decl: 90
        return static_cast<FETypeNode *>(FEMinNode::GetPrev());
    }
};

#endif
