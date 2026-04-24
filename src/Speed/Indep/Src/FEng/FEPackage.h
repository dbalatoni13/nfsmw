#ifndef FENG_FEPACKAGE_H
#define FENG_FEPACKAGE_H

#include "FEObject.h"
#include "FEMsgTargetList.h"
#include "FEngStandard.h"

struct FEObjectCallback;
struct FEGroup;
struct FEngine;
struct FEGameInterface;
struct FEResourceRequest;
struct FELibraryRef;
struct FEMessageResponse;
struct FEPackageRenderInfo;
struct FEListBox;
#include "FETypes.h"

enum FEButtonWrapMode {
    Wrap_None = 0,
    Wrap_Horizontal = 1,
    Wrap_Vertical = 2,
    Wrap_Both = 3,
};

struct FEObjectMouseState {
    FEObject* pObject;    // offset 0x0, size 0x4
    FEPoint Offset;       // offset 0x4, size 0x8
    unsigned long Flags;  // offset 0xC, size 0x4

    FEObjectMouseState();
    ~FEObjectMouseState();

    static inline void* operator new[](unsigned int size) {
        return FEngMalloc(size, nullptr, 0);
    }

    inline bool GetBit(unsigned long bit) { return (Flags & bit) != 0; }
    inline void SetBit(unsigned long bit, bool state) {
        if (state) {
            Flags |= bit;
        } else {
            Flags &= ~bit;
        }
    }
};

// total size: 0x14
struct FENode : public FEMinNode {
    char* name;            // offset 0xC, size 0x4
    unsigned int nameHash; // offset 0x10, size 0x4

    FENode();
    ~FENode() override;
    bool SetName(const char* theName);

    inline const char* GetName() const { return name; }
    inline const unsigned int GetNameHash() const { return nameHash; }
    inline FENode* GetNext() const { return static_cast<FENode*>(FEMinNode::GetNext()); }
    inline FENode* GetPrev() const { return static_cast<FENode*>(FEMinNode::GetPrev()); }
};

// total size: 0x10
struct FEList : public FEMinList {
    inline FENode* GetHead() const { return static_cast<FENode*>(FEMinList::GetHead()); }
    inline FENode* RemNode(FEMinNode* n) { return static_cast<FENode*>(FEMinList::RemNode(n)); }
    FENode* FindNode(const char* pName, FENode* node) const;
    FENode* FindNode(const char* pName) const;
};

// total size: 0x8
struct FEButtonMap {
    FEObject** pList;    // offset 0x0, size 0x4
    unsigned long Count; // offset 0x4, size 0x4

    inline FEButtonMap() : pList(nullptr), Count(0) {}
    inline ~FEButtonMap() {
        if (pList) {
            delete[] pList;
        }
    }
    inline unsigned long GetCount() { return Count; }
    inline void SetButton(unsigned long Index, FEObject* pButton) { pList[Index] = pButton; }
    inline FEObject* GetButton(unsigned long Index) { return pList[Index]; }

    void SetCount(unsigned long NewCount);
    FEObject* GetButtonFrom(FEObject* pButton, long Direction, FEGameInterface* pInterface, FEButtonWrapMode WrapMode);
    void ComputeButtonLocation(FEObject* pObj, FEGameInterface* pInterface, FEVector2& Loc);
};

// total size: 0xC4
struct FEPackage : public FENode {
    static unsigned long uHoldDirtyFlags;

    bool bExecuting;                      // offset 0x14, size 0x1
    bool bUseIdleList;                    // offset 0x18, size 0x1
    bool bIsLibrary;                      // offset 0x1C, size 0x1
    bool bStartEqualsAccept;              // offset 0x20, size 0x1
    bool bErrorScreen;                    // offset 0x24, size 0x1
    int Priority;                         // offset 0x28, size 0x4
    unsigned long Controllers;            // offset 0x2C, size 0x4
    unsigned long OldControllers;         // offset 0x30, size 0x4
    bool bInputEnabled;                   // offset 0x34, size 0x1
    unsigned long VersionNumber;          // offset 0x38, size 0x4
    FEngine* pEnginePtr;                  // offset 0x3C, size 0x4
    int iTickIncrement;                   // offset 0x40, size 0x4
    char* pFilename;                      // offset 0x44, size 0x4
    FEPackage* pParentPackage;            // offset 0x48, size 0x4
    unsigned long UserParam;              // offset 0x4C, size 0x4
    FEMinList Objects;                    // offset 0x50, size 0x10
    FEMinList Responses;                  // offset 0x60, size 0x10
    unsigned long NumRequests;            // offset 0x70, size 0x4
    FEResourceRequest* pRequests;         // offset 0x74, size 0x4
    unsigned long NumMsgTargets;          // offset 0x78, size 0x4
    FEMsgTargetList* pMsgTargets;         // offset 0x7C, size 0x4
    FEList LibrariesUsed;                 // offset 0x80, size 0x10
    inline FEList& GetLibraryList() { return LibrariesUsed; }
    unsigned long NumLibRefs;             // offset 0x90, size 0x4
    FELibraryRef* pLibRefs;              // offset 0x94, size 0x4
    FEObject* pCurrentButton;            // offset 0x98, size 0x4
    FEButtonMap ButtonMap;               // offset 0x9C, size 0x8
    char* pResourceNames;                // offset 0xA4, size 0x4
    FEMinList Comments;                  // offset 0xA8, size 0x10
    FEObjectMouseState* MouseObjectStates; // offset 0xB8, size 0x4
    int NumMouseObjects;                 // offset 0xBC, size 0x4
    int NumMouseObjectsCounter;          // offset 0xC0, size 0x4

    FEPackage();
    ~FEPackage() override;

    bool InitializePackage();
    void Shutdown(FEGameInterface* pGameInterface);
    void Update(FEngine* pEngine, long tDeltaTicks);

    inline FEObject* GetCurrentButton() { return pCurrentButton; }
    inline FEButtonMap* GetButtonMap() { return &ButtonMap; }
    inline FEObject* GetFirstObject() { return static_cast<FEObject*>(Objects.GetHead()); }
    inline FEMessageResponse* GetFirstResponse() { return reinterpret_cast<FEMessageResponse*>(Responses.GetHead()); }
    inline FEPackage* GetNext() { return static_cast<FEPackage*>(FENode::GetNext()); }
    inline FEPackage* GetPrev() { return static_cast<FEPackage*>(FENode::GetPrev()); }
    inline unsigned long GetControlMask() const { return Controllers; }
    inline void SetControlMask(unsigned long ControlMask) { Controllers = ControlMask; }
    inline unsigned long GetOldControlMask() const { return OldControllers; }
    inline void SetOldControlMask(unsigned long ControlMask) { OldControllers = ControlMask; }
    inline void SetErrorScreen(bool b) { bErrorScreen = b; }
    inline bool IsInputEnabled() const { return bInputEnabled; }
    inline void SetInputEnabled(bool b) { bInputEnabled = b; }
    inline bool IsErrorScreen() const { return bErrorScreen; }
    inline int GetPriority() const { return Priority; }
    inline void SetPriority(int NewPri) { Priority = NewPri; }
    inline bool IsLibrary() const { return bIsLibrary; }
    inline void SetStartEqualsAccept(bool bVal) { bStartEqualsAccept = bVal; }
    inline bool StartEqualsAccept() const { return bStartEqualsAccept; }
    inline unsigned long GetUserParam() const { return UserParam; }
    inline void SetUserParam(unsigned long NewParam) { UserParam = NewParam; }
    inline void SetParentPackage(FEPackage* pPack) { pParentPackage = pPack; }
    inline FEPackage* GetParentPackage() { return pParentPackage; }
    inline unsigned long GetVersion() const { return VersionNumber; }
    inline char* GetFilename() { return pFilename; }
    inline unsigned long GetNumParentObjects() { return Objects.GetNumElements(); }
    inline void SetTickIncrement(int tDeltaTicks) { iTickIncrement = tDeltaTicks; }
    inline void SetExecute(bool bExec) { bExecuting = bExec; }
    inline void SetUseIdleList(bool bUseIdle) { bUseIdleList = bUseIdle; }
    inline bool UsesIdleList() { return bUseIdleList; }
    inline FEObject* GetLastObject() { return static_cast<FEObject*>(Objects.GetTail()); }
    inline void AddObject(FEObject* pObject) { Objects.AddTail(static_cast<FEMinNode*>(pObject)); }
    inline void AddObjectAfter(FEObject* pObject, FEObject* pAddAfter) { Objects.AddNode(static_cast<FEMinNode*>(pAddAfter), static_cast<FEMinNode*>(pObject)); }
    inline void RemoveObject(FEObject* pObject) { Objects.RemNode(static_cast<FEMinNode*>(pObject)); }
    inline unsigned long GetNumResponses() { return Responses.GetNumElements(); }
    inline void AddResponse(FEMessageResponse* pResp) { Responses.AddTail(reinterpret_cast<FEMinNode*>(pResp)); }
    inline void PurgeResponses() { Responses.Purge(); }
    inline void RemoveResponse(FEMessageResponse* pResp) { Responses.RemNode(reinterpret_cast<FEMinNode*>(pResp)); }
    inline FEMessageResponse* GetResponse(unsigned long Index) { return reinterpret_cast<FEMessageResponse*>(Responses.FindNode(Index)); }
    inline const FEMsgTargetList* GetMessageTargetList(unsigned long Index) const { return &pMsgTargets[Index]; }

    FEMsgTargetList* GetMessageTargets(unsigned long MsgID);
    FEObject* FindObjectByHash(unsigned long NameHash);
    FEObject* FindObjectByGUID(unsigned long GUID);
    FEMessageResponse* FindResponse(unsigned long MsgID);
    void SetCurrentButton(FEObject* pNewButton, bool bSendMsgs);
    bool ForAllChildren(FEGroup* pGroup, FEObjectCallback& Callback);
    bool ForAllObjects(FEObjectCallback& Callback);
    void SetFilename(const char* pName);
    void SetNumLibraryRefs(unsigned long NewCount);
    FELibraryRef* FindLibraryReference(unsigned long ObjGUID) const;
    void ConnectObjectResources();
    void BuildMouseObjectStateList();
    bool Startup(FEGameInterface* pGameInterface);

    void IssueScriptMessages(FEngine* pEngine, FEObject* pObj, FEScript* pScript, long tOldTime, long tNewTime);
    void UpdateObject(FEObject* pObj, long tDeltaTicks);
    void UpdateObjectTracks(FEObject* pObj, FEScript* pScript);

    void UpdateGroup(FEGroup* pGroup, long tDeltaTicks);
    void AddMouseObjectState(FEObject* pObj);
    void UpdateMouseObjectOffsets(FEObject* pObj);
};

#endif
