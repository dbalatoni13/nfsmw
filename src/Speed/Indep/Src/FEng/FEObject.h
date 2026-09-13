#ifndef FEOBJECT_H_
#define FEOBJECT_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEMessageResponse.h"

class FEObject;
class FERenderObject;

class FEObjectDestructorCallback {
  public:
    virtual void OnDestroy(FEObject *pObject) = 0;
};

// Decl: 48
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

enum FEResourceType {
    RT_None = 0,
    RT_Image = 1,
    RT_Font = 2,
    RT_Model = 3,
    RT_Movie = 4,
    RT_Effect = 5,
    RT_AnimImage = 6,
    RT_MultiImage = 7,
    RT_ResourceTypeCount = 8,
};

// total size: 0x44
// Decl: 86
class FEObjData {
  public:
    FEColor Col;      // offset 0x0, size 0x10, Decl: 88
    FEVector3 Pivot;  // offset 0x10, size 0xC, Decl: 89
    FEVector3 Pos;    // offset 0x1C, size 0xC, Decl: 90
    FEQuaternion Rot; // offset 0x28, size 0x10, Decl: 91
    FEVector3 Size;   // offset 0x38, size 0xC, Decl: 92
};

enum FEObject_Flags {
    FF_HideInEdit = 1 << 31,
    FF_AffectAllScripts = 1 << 30,
    FF_PerspectiveProjection = 1 << 29,
    FF_IsButton = 1 << 28,
    FF_ObjectLocked = 1 << 27,
    FF_IgnoreButton = 1 << 26,
    FF_Dirty = 1 << 25,
    FF_DirtyTransform = 1 << 24,
    FF_DirtyColor = 1 << 23,
    FF_DirtyCode = 1 << 22,
    FF_CodeSuppliedResource = 1 << 21,
    FF_UsesLibraryObject = 1 << 20,
    FF_DontNavigate = 1 << 19,
    FF_SaveStaticTracks = 1 << 18,
    FF_MouseObject = 1 << 17
};

static const i32 FF_GameFlagsMask = 0x0000FFFF;   // size: 0x4, Decl: 119
static const i32 FF_EngineFlagsMask = 0xFFFF0000; // size: 0x4, Decl: 120

static const u32 GameFlagsMask = 0x7FFFF;                                                          // size: 0x4, Decl: 122
static const u32 FEngDirtyFlagsMask = FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform | FF_Dirty; // size: 0x4, Decl: 123

// total size: 0x5C
// Decl: 131
class FEObject : public FEMinNode {
  public:
    static FEObjectDestructorCallback *pDestructorCallback; // size: 0x4, address: 0x8041D154, Decl: 20

    u32 GUID;     // offset 0xC, size 0x4, Decl: 138
    u32 NameHash; // offset 0x10, size 0x4, Decl: 139
    char *pName;  // offset 0x14, size 0x4, Decl: 140

    FEObjType Type; // offset 0x18, size 0x4, Decl: 142
    u32 Flags;      // offset 0x1C, size 0x4, Decl: 143

    u16 RenderContext; // offset 0x20, size 0x2, Decl: 145
    u16 ResourceIndex; // offset 0x22, size 0x2, Decl: 146
    u32 Handle;        // offset 0x24, size 0x4, Decl: 147
    u32 UserParam;     // offset 0x28, size 0x4, Decl: 148

    u8 *pData;    // offset 0x2C, size 0x4, Decl: 150
    u32 DataSize; // offset 0x30, size 0x4, Decl: 151

    FEMinList Responses; // offset 0x34, size 0x10, Decl: 157
    FEMinList Scripts;   // offset 0x44, size 0x10, Decl: 158

    FEScript *pCurrentScript; // offset 0x54, size 0x4, Decl: 160

    FERenderObject *Cached; // offset 0x58, size 0x4, Decl: 164

    void SetCurrentScript(FEScript *pScript);

    FEObjData *GetObjData() const {
        return reinterpret_cast<FEObjData *>(pData);
    }

    FEScript *GetFirstScript() const {
        return reinterpret_cast<FEScript *>(Scripts.GetHead());
    }

    u32 GetNumScripts() const { // Decl: 171
        return Scripts.GetNumElements();
    }

    FEScript *GetScript(u32 Index) const { // Decl: 172
        return static_cast<FEScript *>(Scripts.FindNode(Index));
    }

    FEScript *FindScript(u32 ID) const;

    FEMessageResponse *GetFirstResponse() const {
        return reinterpret_cast<FEMessageResponse *>(Responses.GetHead());
    }

    u32 GetNumResponses() const { // Decl: 176
        return Responses.GetNumElements();
    }

    FEMessageResponse *GetResponse(u32 Index) const { // Decl: 177
        return static_cast<FEMessageResponse *>(Responses.FindNode(Index));
    }

    FEMessageResponse *FindResponse(u32 MsgID) const;

    FEObject();

    ~FEObject() override;

    void SetDataSize(u32 Size);

    void SetName(const char *pNewName);

    void SetNameHash(const u32 nameHash) { // Decl: 192
        NameHash = nameHash;
    }

    void SetPivot(const FEVector3 &pivot, bool bRelative);

    void SetPosition(const FEVector3 &position, bool bRelative);

    void SetRotation(const FEQuaternion &rotation, bool bRelative);

    void SetSize(const FEVector3 &size, bool bRelative);

    void SetColor(const FEColor &color, bool bRelative);

    void SetScript(u32 ID, bool bForce);

    void SetScript(FEScript *pScript, bool bForce);

    void SetupMoveToTracks();

    u32 GetDataOffset(FEKeyTrack_Indices track);

    virtual FEObject *Clone(bool bReference);

    FEObject *GetNext() const {
        return static_cast<FEObject *>(FEMinNode::GetNext());
    }

    FEObject *GetPrev() const { // Decl: 209
        return static_cast<FEObject *>(FEMinNode::GetPrev());
    }

    FEObject(const FEObject &Object, bool bReference);

  protected:
    void SetTrackValue(FEKeyTrack_Indices track, const FEVector3 &value, bool bRelative);

    void SetTrackValue(FEKeyTrack_Indices track, const FEVector2 &value, bool bRelative);

    void SetTrackValue(FEKeyTrack_Indices track, const FEColor &value, bool bRelative);
};

#endif
