#ifndef FEPACKAGE_H_
#define FEPACKAGE_H_

// #include "Speed/Indep/Src/FEng/fengine.h"
#include <types.h>
#include "FETypes.h"
#include "FEButtonMap.h"
#include "FEObject.h"
#include "FEMsgTargetList.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FELibraryRef.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"

class FEngine;

// total size: 0x4
// Decl: 44
class PackageInitStateCB : public FEObjectCallback {
  public:
    PackageInitStateCB() {}

    bool Callback(FEObject *pObj) override;
};

// total size: 0x8
// Decl: 54
struct FELoadPackageDesc {
    char *pPackageName; // offset 0x0, size 0x4, Decl: 55
    u32 Type;           // offset 0x4, size 0x4, Decl: 56
};

// total size: 0xC
// Decl: 60
struct FELoadPackageDescArray {
    u32 Num;                    // offset 0x0, size 0x4, Decl: 61
    FELoadPackageDesc Descs[1]; // offset 0x4, size 0x8, Decl: 62
};

// total size: 0x14
// Decl: 66
class FEObjectComment : public FEMinNode {
  public:
    u32 ObjectGUID; // offset 0xC, size 0x4, Decl: 67
    char *pStr;     // offset 0x10, size 0x4, Decl: 68

    FEObjectComment() {}

    FEObjectComment *GetNext() { // Decl: 70
        return static_cast<FEObjectComment *>(FEMinNode::GetNext());
    }

    FEObjectComment *GetPrev() { // Decl: 71
        return static_cast<FEObjectComment *>(FEMinNode::GetPrev());
    }
};

static const u32 FEMouseFlag_MouseOver = 1;         // size: 0x4, Decl: 80
static const u32 FEMouseFlag_MouseLeftPressed = 2;  // size: 0x4, Decl: 81
static const u32 FEMouseFlag_MouseRightPressed = 4; // size: 0x4, Decl: 82

// total size: 0x10
// Decl: 85
class FEObjectMouseState {
  public:
    FEObjectMouseState();  // Decl: 87
    ~FEObjectMouseState(); // Decl: 88

    FEObject *pObject; // offset 0x0, size 0x4, Decl: 90
    FEPoint Offset;    // offset 0x4, size 0x8, Decl: 91
    u32 Flags;         // offset 0xC, size 0x4, Decl: 92

    bool GetBit(u32 bit) { // Decl: 94
        return (Flags & bit) != 0;
    }
    void SetBit(u32 bit, bool state) { // Decl: 95
        if (state) {
            Flags |= bit;
        } else {
            Flags &= ~bit;
        }
    }
};

// total size: 0xC4
// Decl: 104
class FEPackage : public FENode {
  private:
    bool bExecuting;                       // offset 0x14, size 0x1, Decl: 106
    bool bUseIdleList;                     // offset 0x18, size 0x1, Decl: 107
    bool bIsLibrary;                       // offset 0x1C, size 0x1, Decl: 108
    bool bStartEqualsAccept;               // offset 0x20, size 0x1, Decl: 109
    bool bErrorScreen;                     // offset 0x24, size 0x1, Decl: 111
    i32 Priority;                          // offset 0x28, size 0x4, Decl: 113
    u32 Controllers;                       // offset 0x2C, size 0x4, Decl: 114
    u32 OldControllers;                    // offset 0x30, size 0x4, Decl: 115
    bool bInputEnabled;                    // offset 0x34, size 0x1, Decl: 116
    u32 VersionNumber;                     // offset 0x38, size 0x4, Decl: 117
    FEngine *pEnginePtr;                   // offset 0x3C, size 0x4, Decl: 119
    i32 iTickIncrement;                    // offset 0x40, size 0x4, Decl: 120
    char *pFilename;                       // offset 0x44, size 0x4, Decl: 122
    FEPackage *pParentPackage;             // offset 0x48, size 0x4, Decl: 123
    u32 UserParam;                         // offset 0x4C, size 0x4, Decl: 125
    FEMinList Objects;                     // offset 0x50, size 0x10, Decl: 127
    FEMinList Responses;                   // offset 0x60, size 0x10, Decl: 128
    u32 NumRequests;                       // offset 0x70, size 0x4, Decl: 130
    FEResourceRequest *pRequests;          // offset 0x74, size 0x4, Decl: 131
    u32 NumMsgTargets;                     // offset 0x78, size 0x4, Decl: 133
    FEMsgTargetList *pMsgTargets;          // offset 0x7C, size 0x4, Decl: 134
    FEList LibrariesUsed;                  // offset 0x80, size 0x10, Decl: 137
    u32 NumLibRefs;                        // offset 0x90, size 0x4, Decl: 138
    FELibraryRef *pLibRefs;                // offset 0x94, size 0x4, Decl: 139
    FEObject *pCurrentButton;              // offset 0x98, size 0x4, Decl: 141
    FEButtonMap ButtonMap;                 // offset 0x9C, size 0x8, Decl: 143
    char *pResourceNames;                  // offset 0xA4, size 0x4, Decl: 145
    FEMinList Comments;                    // offset 0xA8, size 0x10, Decl: 147
    static u32 uHoldDirtyFlags;            // size: 0x4, address: 0x8041D158, Decl: 26
    FEObjectMouseState *MouseObjectStates; // offset 0xB8, size 0x4, Decl: 150
    int NumMouseObjects;                   // offset 0xBC, size 0x4, Decl: 151
    int NumMouseObjectsCounter;            // offset 0xC0, size 0x4

    void IssueScriptMessages(FEngine *pEngine, FEObject *pObjPtr, FEScript *pScript, i32 tFrom, i32 tTo);

    void UpdateGroup(FEGroup *pGroup, const i32 tDeltaTicks);

    void UpdateObjectTracks(FEObject *pObject, FEScript *pScript);

    void SetNumLibraryRefs(u32 NewCount);

  public:
    FEPackage();
    ~FEPackage() override;

    void SetFilename(const char *pName);

    char *GetFilename() { // Decl: 174
        return pFilename;
    }

    u32 GetVersion() { // Decl: 180
        return VersionNumber;
    }

    u32 GetNumParentObjects() { // Decl: 183
        return Objects.GetNumElements();
    }

    void SetExecute(bool bExec) { // Decl: 185
        bExecuting = bExec;
    }

    void SetUseIdleList(bool bUseIdle) { // Decl: 186
        bUseIdleList = bUseIdle;
    }

    bool UsesIdleList() { // Decl: 187
        return bUseIdleList;
    }

    bool IsErrorScreen() const {
        return bErrorScreen;
    }

    void SetErrorScreen(bool b) { // Decl: 190
        bErrorScreen = b;
    }

    i32 GetPriority() const {
        return Priority;
    }

    void SetPriority(i32 NewPri) { // Decl: 193
        Priority = NewPri;
    }

    bool IsLibrary() const { // Decl: 195
        return bIsLibrary;
    }

    void SetStartEqualsAccept(bool bVal) { // Decl: 197
        bStartEqualsAccept = bVal;
    }

    bool StartEqualsAccept() const {
        return bStartEqualsAccept;
    }

    u32 GetUserParam() const { // Decl: 201
        return UserParam;
    }

    void SetUserParam(u32 NewParam) { // Decl: 202
        UserParam = NewParam;
    }

    void SetParentPackage(FEPackage *pPack) { // Decl: 204
        pParentPackage = pPack;
    }

    FEPackage *GetParentPackage() { // Decl: 205
        return pParentPackage;
    }

    void BuildMessageTargetLists();

    void BuildButtonMap();

    void BuildMouseObjectStateList();

    bool Startup(FEGameInterface *pGameInterface);

    void Shutdown(FEGameInterface *pGameInterface);

    bool InitializePackage();

    u32 GetControlMask() const {
        return Controllers;
    }

    void SetControlMask(u32 ControlMask) { // Decl: 221
        Controllers = ControlMask;
    }

    u32 GetOldControlMask() const {
        return OldControllers;
    }

    void SetOldControlMask(u32 ControlMask) { // Decl: 224
        OldControllers = ControlMask;
    }

    bool IsInputEnabled() const {
        return bInputEnabled;
    }

    void SetInputEnabled(bool b) { // Decl: 227
        bInputEnabled = b;
    }

    u32 GetNumResponses() { // Decl: 229
        return Responses.GetNumElements();
    }

    FEMessageResponse *GetFirstResponse() { // Decl: 232
        return reinterpret_cast<FEMessageResponse *>(Responses.GetHead());
    }

    FEMessageResponse *GetResponse(u32 Index) { // Decl: 235
        return reinterpret_cast<FEMessageResponse *>(Responses.FindNode(Index));
    }

    FEMessageResponse *FindResponse(u32 MsgID);

    void AddResponse(FEMessageResponse *pResp) { // Decl: 240
        Responses.AddTail(reinterpret_cast<FEMinNode *>(pResp));
    }

    void PurgeResponses() { // Decl: 241
        Responses.Purge();
    }

    void RemoveResponse(FEMessageResponse *pResp) { // Decl: 242
        Responses.RemNode(reinterpret_cast<FEMinNode *>(pResp));
    }

    FEObject *GetCurrentButton() { // Decl: 245
        return pCurrentButton;
    }

    void SetCurrentButton(FEObject *pNewButton, bool bSendMsgs);

    FEButtonMap *GetButtonMap() { // Decl: 247
        return &ButtonMap;
    }

    FEObject *FindObjectByName(const char *pName);

    FEObject *FindObjectByHash(u32 NameHash);

    FEObject *FindObjectByGUID(u32 GUID);

    void AddObject(FEObject *pObject) { // Decl: 253
        Objects.AddTail(static_cast<FEMinNode *>(pObject));
    }

    void AddObjectAfter(FEObject *pObject, FEObject *pAddAfter) { // Decl: 254
        Objects.AddNode(static_cast<FEMinNode *>(pAddAfter), static_cast<FEMinNode *>(pObject));
    }

    void RemoveObject(FEObject *pObject) { // Decl: 256
        Objects.RemNode(static_cast<FEMinNode *>(pObject));
    }

    FEObject *GetFirstObject() { // Decl: 258
        return static_cast<FEObject *>(Objects.GetHead());
    }

    FEObject *GetLastObject() { // Decl: 259
        return static_cast<FEObject *>(Objects.GetTail());
    }

    void Update(FEngine *pEngine, const i32 tDeltaTicks);

    bool ForAllChildren(FEGroup *pGroup, FEObjectCallback &Callback);

    bool ForAllObjects(FEObjectCallback &Callback);

    FEPackage *GetNext() { // Decl: 265
        return static_cast<FEPackage *>(FEMinNode::GetNext());
    }

    FEPackage *GetPrev() { // Decl: 266
        return static_cast<FEPackage *>(FEMinNode::GetPrev());
    }

    FEMsgTargetList *const GetMessageTargetList(u32 Index) const { // Decl: 268
        return &pMsgTargets[Index];
    }

    u32 GetNumMessageTargets() const { // Decl: 269
        return NumMsgTargets;
    }

    FEMsgTargetList *GetMessageTargets(u32 MsgID);

    FELoadPackageDescArray *GetLoadPackageDescArray();

    void FreeLoadPackageDescArray(FELoadPackageDescArray *pDescArray) const;

    const char *FindComment(u32 ObjectGUID);

    void SetComment(u32 ObjectGUID, const char *pString);

    FEMinList &GetCommentList() { // Decl: 278
        return Comments;
    }

    void AddMouseObjectState(FEObject *obj);

    void UpdateMouseObjectOffsets(FEObject *obj);

    FEObjectMouseState *GetMouseObjectStates() { // Decl: 282
        return MouseObjectStates;
    }

    FEObjectMouseState *GetMouseObjectState(FEObject *obj);

    int GetNumMouseObjects() const {
        return NumMouseObjects;
    }

  public:
    bool AddLibraryReference(u32 ObjGUID, u32 LibGUID, FEPackage *pLibPackage);

    FELibraryRef *FindLibraryReference(u32 ObjGUID) const;

    bool RemoveLibraryReference(u32 ObjGUID);

    u32 NumLibraryReferences() const { // Decl: 290
        return NumLibRefs;
    }
    FELibraryRef *GetLibraryReference(u32 Index) const { // Decl: 291
        return &pLibRefs[Index];
    }

    void PurgeUnusedPackageLibraries();

    FEList &GetLibraryList() { // Decl: 297
        return LibrariesUsed;
    }

    void ConnectObjectResources();

    void ConnectTargetLists();

    void ConnectButtonMap();

    void UpdateObject(FEObject *pObject, const i32 tDeltaTicks);

    void SetTickIncrement(i32 tDeltaTicks) { // Decl: 311
        iTickIncrement = tDeltaTicks;
    }

    friend class FEngine;
    friend class FEPackageReader;
};

#endif
