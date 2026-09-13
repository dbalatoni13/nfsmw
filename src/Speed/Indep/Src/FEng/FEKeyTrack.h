#ifndef FEKEYTRACK_H_
#define FEKEYTRACK_H_

#include "FEGenericVal.h"
#include "FERefList.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

// Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEKeyTrack.h:51
inline i32 FEFramesToTicks(i32 Frames) {
    return Frames * 16;
}

// total size: 0x14
// Decl: 59
typedef struct {
    i32 tTime;        // offset 0x0, size 0x4, Decl: 60
    FEGenericVal Val; // offset 0x4, size 0x10, Decl: 61
} FEKeyData;

// total size: 0x20
// Decl: 70
class FEKeyNode : public FEMinNode {
  public:
    ~FEKeyNode() override {}

    static void *operator new(size_t size, int) {}

    static void *operator new(size_t);

    static void operator delete(void *pNode, int) {}

    static void operator delete(void *pNode);

  private:
    static ObjectPool<FEKeyNode, 256> NodePool; // size: 0x10, address: 0x80473D38, Decl: 15

  private:
    void Init() { // Decl: 74
    }

  public:
    FEKeyNode() { // Decl: 77
        Init();
    }

    // TODO: this is definitely FEKeyData
    i32 tTime;        // offset 0xC, size 0x4, Decl: 85
    FEGenericVal Val; // offset 0x10, size 0x10, Decl: 86

    FEKeyData *GetKeyData() { // Decl: 88
        return reinterpret_cast<FEKeyData *>(&tTime);
    }

    FEKeyNode *GetNext() { // Decl: 90
        return static_cast<FEKeyNode *>(FEMinNode::GetNext());
    }

    FEKeyNode *GetPrev() { // Decl: 91
        return static_cast<FEKeyNode *>(FEMinNode::GetPrev());
    }
};

// total size: 0x38
// Decl: 100
class FEKeyTrack {
  public:
    u8 ParamType;        // offset 0x0, size 0x1, Decl: 102
    u8 ParamSize;        // offset 0x1, size 0x1, Decl: 103
    u8 InterpType;       // offset 0x2, size 0x1, Decl: 104
    u8 InterpAction;     // offset 0x3, size 0x1, Decl: 105
    i32 Length : 24;     // offset 0x4, size 0x4
    i32 LongOffset : 8;  // offset 0x4, size 0x4
    FEKeyNode BaseKey;   // offset 0x8, size 0x20, Decl: 109
    FERefList DeltaKeys; // offset 0x28, size 0x10, Decl: 110

    void operator=(FEKeyTrack &Src); // Decl: 112
    u32 ComputeSize();               // Decl: 113

    FEKeyNode *GetBaseKey() { // Decl: 116
        return &BaseKey;
    }

    FEKeyNode *GetFirstDeltaKey() { // Decl: 119
        return static_cast<FEKeyNode *>(DeltaKeys.GetHead());
    }

    FEKeyNode *GetKeyNumber(u32 Number) {}

    FEKeyNode *GetKeyAt(i32 tTime);

    FEKeyNode *GetDeltaKeyAt(i32 tTime);

    bool CanDeleteKeyAt(i32 tTime);

    void InsertKey(FEKeyNode *pNewKey); // Decl: 133

    FEKeyNode *CreateKeyAt(i32 tTime);

    bool GenerateNewKeyAt(i32 tTime);

    bool IsReference() const {
        return DeltaKeys.IsReference();
    }

    void BreakReference(); // Decl: 147
                           // Decl: 149
    FEKeyTrack() : ParamType(0), ParamSize(0), InterpType(0), InterpAction(0), Length(0), LongOffset(0), BaseKey(), DeltaKeys() {}
    ~FEKeyTrack() {}

    bool SetKeyValueAt(i32 tTime, i32 &Val);

    bool SetKeyValueAt(i32 tTime, FEVector3 &Val);

    bool SetKeyValueAt(i32 tTime, FEVector2 &Val);

    bool SetKeyValueAt(i32 tTime, FEColor &Val);

    bool SetKeyValueAt(i32 tTime, FEQuaternion &Val);

    void SetBaseValueAt(i32 tTime, i32 &Val);

    void SetBaseValueAt(i32 tTime, FEVector3 &Val);

    void SetBaseValueAt(i32 tTime, FEVector2 &Val);

    void SetBaseValueAt(i32 tTime, FEColor &Val);

    void SetBaseValueAt(i32 tTime, FEQuaternion &Val);
};

#endif
