// !! BUILD BREAK (zFe2 agent) !!
// Commit ab675cc1 broke the zFe2 TU. NgcAs fails with "Unrecognised opcode"
// at feWidget.cpp(21). Bisected: 7c2ee32b builds, ab675cc1 does not.
// The header scaffolding added in ab675cc1 pushes NgcAs past an internal
// limit. The assembler error is non-deterministic (.4b, .l, .string, etc.)
// which points to memory corruption inside NgcAs/wibo.
// Please trim headers or split scaffolding to fix. I'm blocked on zFe2.
// -- zFe2 agent
#ifndef _FEOBJECT
#define _FEOBJECT

#include "types.h"
#include "FEList.h"

struct FEScript;
struct FERenderObject;
struct FEObjData;
struct FEMessageResponse;
struct FEObject;

struct FEObjectDestructorCallback {
    virtual void OnDestroy(FEObject* pObject) = 0;
};
struct FEVector2;
struct FEVector3;
struct FEColor;
struct FEQuaternion;
enum FEKeyTrack_Indices {
    FETrack_Color = 0,
    FETrack_Pivot = 1,
    FETrack_Position = 2,
    FETrack_Rotation = 3,
    FETrack_Size = 4,
    FETrack_UpperLeft = 5,
    FETrack_LowerRight = 6,
    FETrack_FrameNumber = 7,
    FETrack_Color1 = 7,
    FETrack_Color2 = 8,
    FETrack_Color3 = 9,
    FETrack_Color4 = 10,
    Num_BaseFETracks = 11,
};

enum FEObjType {
    FE_None = 0,
    FE_Image = 1,
    FE_String = 2,
    FE_Model = 3,
    FE_List = 4,
    FE_Group = 5,
    FE_CodeList = 6,
    FE_Movie = 7,
    FE_Effect = 8,
    FE_ColoredImage = 9,
    FE_AnimImage = 10,
    FE_SimpleImage = 11,
    FE_MultiImage = 12,
    FE_UserMin = 256,
};

// total size: 0x5C
struct FEObject : public FEMinNode {
    static FEObjectDestructorCallback* pDestructorCallback;

    unsigned long GUID;           // offset 0xC, size 0x4
    unsigned long NameHash;       // offset 0x10, size 0x4
    char* pName;                  // offset 0x14, size 0x4
    FEObjType Type;               // offset 0x18, size 0x4
    unsigned long Flags;          // offset 0x1C, size 0x4
    unsigned short RenderContext; // offset 0x20, size 0x2
    unsigned short ResourceIndex; // offset 0x22, size 0x2
    unsigned long Handle;         // offset 0x24, size 0x4
    unsigned long UserParam;      // offset 0x28, size 0x4
    unsigned char* pData;         // offset 0x2C, size 0x4
    unsigned long DataSize;       // offset 0x30, size 0x4
    FEMinList Responses;          // offset 0x34, size 0x10
    FEMinList Scripts;            // offset 0x44, size 0x10
    FEScript* pCurrentScript;     // offset 0x54, size 0x4
    FERenderObject* Cached;       // offset 0x58, size 0x4

    inline FEObjData* GetObjData() const { return reinterpret_cast<FEObjData*>(pData); }
    inline FEScript* GetFirstScript() const { return reinterpret_cast<FEScript*>(Scripts.GetHead()); }
    inline unsigned long GetNumScripts() const;
    inline FEScript* GetScript(unsigned long Index) const;
    inline FEMessageResponse* GetFirstResponse() const { return reinterpret_cast<FEMessageResponse*>(Responses.GetHead()); }
    inline unsigned long GetNumResponses() const;
    inline FEMessageResponse* GetResponse(unsigned long Index) const;
    inline void SetNameHash(const unsigned long nameHash);
    inline FEObject* GetNext() const { return static_cast<FEObject*>(FEMinNode::GetNext()); }
    inline FEObject* GetPrev() const { return static_cast<FEObject*>(FEMinNode::GetPrev()); }

    FEObject();
    FEObject(const FEObject& Object, bool bReference);
    ~FEObject() override;

    void SetDataSize(unsigned long Size);
    void SetName(const char* pNewName);
    FEScript* FindScript(unsigned long ID) const;
    void SetupMoveToTracks();
    void SetCurrentScript(FEScript* pScript);
    FEMessageResponse* FindResponse(unsigned long MsgID) const;

    void SetPivot(const FEVector3& pivot, bool bRelative);
    void SetPosition(const FEVector3& position, bool bRelative);
    void SetRotation(const FEQuaternion& rotation, bool bRelative);
    void SetSize(const FEVector3& size, bool bRelative);
    void SetColor(const FEColor& color, bool bRelative);
    void SetScript(unsigned long ID, bool bForce);
    void SetScript(FEScript* pScript, bool bForce);
    unsigned long GetDataOffset(FEKeyTrack_Indices track);

    virtual FEObject* Clone(bool bReference);

protected:
    void SetTrackValue(FEKeyTrack_Indices track, const FEVector3& value, bool bRelative);
    void SetTrackValue(FEKeyTrack_Indices track, const FEVector2& value, bool bRelative);
    void SetTrackValue(FEKeyTrack_Indices track, const FEColor& value, bool bRelative);
};

#endif
