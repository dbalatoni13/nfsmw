#ifndef FENG_FENGINE_H
#define FENG_FENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEMouse.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"

struct FEJoyPad;
struct FEGameInterface;
struct FEObjectMouseState;
struct FEMessageResponse;
struct FEScript;
struct FEPackageCommand;
struct FEFieldNode;

// total size: 0x28
struct FETypeNode : public FENode {
    FEMinList Fields;    // offset 0x14, size 0x10
    unsigned long TypeID; // offset 0x24, size 0x4

    inline FETypeNode* GetNext() { return static_cast<FETypeNode*>(FENode::GetNext()); }
    inline FETypeNode* GetPrev() { return static_cast<FETypeNode*>(FENode::GetPrev()); }
    inline FEFieldNode* GetFirstField();
    inline unsigned long GetID() { return TypeID; }
    inline void SetID(unsigned long ID) { TypeID = ID; }

    void AddField(const char* pName, long Type);
    void UpdateOffsets();
    unsigned long GetTypeSize();
    FEFieldNode* GetField(const char* pName);
    inline FEFieldNode* GetField(int Index);
};

// total size: 0x24
struct FEFieldNode : public FENode {
    int Type;              // offset 0x14, size 0x4
    unsigned long Size;    // offset 0x18, size 0x4
    unsigned long Offset;  // offset 0x1C, size 0x4
    unsigned char* pDefault; // offset 0x20, size 0x4

    inline FEFieldNode() : Size(0), Offset(0), pDefault(nullptr) {}
    ~FEFieldNode() override;

    inline int GetType() const { return Type; }
    inline void SetType(int NewType) { Type = NewType; }
    inline unsigned long GetSize() const { return Size; }
    inline void SetSize(unsigned long Val) { Size = Val; }
    inline unsigned long GetOffset() const { return Offset; }
    inline void SetOffset(unsigned long Val) { Offset = Val; }
    inline const void* GetDefaultPtr() { return pDefault; }
    inline FEFieldNode* GetNext() const { return static_cast<FEFieldNode*>(FENode::GetNext()); }
    inline FEFieldNode* GetPrev() const { return static_cast<FEFieldNode*>(FENode::GetPrev()); }

    void SetDefault(void* pSrc);
    void GetDefault(void* pDest);
};

inline FEFieldNode* FETypeNode::GetFirstField() { return static_cast<FEFieldNode*>(Fields.GetHead()); }

inline FEFieldNode* FETypeNode::GetField(int Index) {
    return static_cast<FEFieldNode*>(Fields.FindNode(static_cast<unsigned long>(Index)));
}

// total size: 0x8
struct SFERadixKey {
    FEObject* pobObject; // offset 0x0
    unsigned long ulKey;  // offset 0x4
};

// total size: 0x4004
template <int N>
struct FEObjectSorter {
    unsigned long mulNumObjects;          // offset 0x0, size 0x4
    SFERadixKey mastFinalList[N];         // offset 0x4, size 0x2000
    SFERadixKey mastScratchList[N];       // offset 0x2004, size 0x2000

    inline void Zero() { mulNumObjects = 0; }
    inline FEObjectSorter() { Zero(); }
    inline SFERadixKey* GetListPtr() { return mastFinalList; }
    inline unsigned long GetNumObjects() { return mulNumObjects; }
    inline void AddObject(FEObject* pobObject, float fZValue) {
        mastFinalList[mulNumObjects].pobObject = pobObject;
        mastFinalList[mulNumObjects].ulKey = *reinterpret_cast<unsigned long*>(&fZValue);
        mulNumObjects++;
    }
    void SortObjects();
};

#include "Speed/Indep/Src/FEng/FEObjectSorter.h"

// total size: 0x14
struct FETypeLib {
    FEList List;                // offset 0x0, size 0x10
    bool bAutoCreateHideScripts; // offset 0x10, size 0x1

    inline FETypeLib() : bAutoCreateHideScripts(false) {}
    inline ~FETypeLib() {}
    inline void Shutdown() {}
    inline void SetAutoCreateHide(bool bValue) { bAutoCreateHideScripts = bValue; }
    inline bool GetAutoCreateHide() const { return bAutoCreateHideScripts; }
    inline FETypeNode* FindType(const char* pName) { return static_cast<FETypeNode*>(List.FindNode(pName)); }
    inline void AddType(FETypeNode* pNode) { List.AddTail(pNode); }
    inline void RemoveType(FETypeNode* pNode) { List.RemNode(pNode); }
    inline FETypeNode* GetFirstType() { return static_cast<FETypeNode*>(List.GetHead()); }
    inline const FEList* GetList() { return &List; }

    bool Startup();
    FETypeNode* FindType(unsigned long TypeID);
    FEObject* CreateFEObject(FETypeNode* pType, bool bInitScript);
    FEObject* CreateFEObject(unsigned long TypeID, bool bInitScript);
    FEScript* CreateObjectScript(FETypeNode* pType);
    FEScript* CreateObjectScript(unsigned long TypeID);
    FETypeNode* CreateBaseObjectType(const char* pName);
    FETypeNode* CreateImageObjectType(const char* pName);
    FETypeNode* CreateMultiImageObjectType(const char* pName);
};

// total size: 0x8
struct FEPackageButtonRec {
    unsigned long PackageHash; // offset 0x0, size 0x4
    unsigned long ButtonGUID;  // offset 0x4, size 0x4
};

// FEButtonWrapMode defined in FEPackage.h

struct FEPackageList {
    FEList Packages; // offset 0x0, size 0x10

    inline FEPackage* GetFirstPackage() const {
        return static_cast<FEPackage*>(Packages.GetHead());
    }
    inline FEPackage* GetLastPackage() const {
        return static_cast<FEPackage*>(Packages.GetTail());
    }
    inline FEPackage* FindPackage(const char* pName) const {
        return static_cast<FEPackage*>(Packages.FindNode(pName));
    }
    inline unsigned long GetCount() const { return Packages.GetNumElements(); }

    void AddPackage(FEPackage* pPack);
    void AddPackageAfter(FEPackage* pPack, FEPackage* pAfter);
    FEPackage* FindPackage(const char* pName, unsigned char ControllerIndex) const;
    bool RemovePackage(FEPackage* pPack);
    void ReplaceParentLinks(const FEPackage* pParent, const FEPackage* pReplacement);
};

// total size: 0x40
// FEMatrix4 is now defined in FETypes.h

// total size: 0x5268
struct FEngine {
    static unsigned long SysGUID;

    bool bExecuting;                          // offset 0x0, size 0x1
    bool bMouseActive;                        // offset 0x4, size 0x1
    bool bLoadObjectNames;                    // offset 0x8, size 0x1
    bool bLoadScriptNames;                    // offset 0xC, size 0x1
    FEJoyPad* pJoyPad;                        // offset 0x10, size 0x4
    FEMouse Mouse;                            // offset 0x14, size 0x24
    unsigned long FastRep;                    // offset 0x38, size 0x4
    unsigned long FastRepCache;               // offset 0x3C, size 0x4
    unsigned long PadHoldRegistered;          // offset 0x40, size 0x4
    unsigned long HoldDecrement[19];          // offset 0x44, size 0x4C
    FEObject* HeldButtons[19];               // offset 0x90, size 0x4C
    FEButtonWrapMode WrapMode;               // offset 0xDC, size 0x4
    unsigned long NumJoyPads;                // offset 0xE0, size 0x4
    unsigned short uGroupContext;            // offset 0xE4, size 0x2
    FEPackageList PackList;                  // offset 0xE8, size 0x10
    FEList IdleList;                         // offset 0xF8, size 0x10
    FEList LibraryList;                      // offset 0x108, size 0x10
    FEGameInterface* pInterface;             // offset 0x118, size 0x4
    FEObjectSorter<1024> Sorter;              // offset 0x11C, size 0x4004
    FEMinList MsgQ;                          // offset 0x4120, size 0x10
    FEList PackageCommands;                  // offset 0x4130, size 0x10
    FETypeLib TypeLib;                       // offset 0x4140, size 0x14
    int CurrentPackageRecordIndex;           // offset 0x4154, size 0x4
    char RecordedPackageNames[16][256];      // offset 0x4158, size 0x1000
    int NextButtonRecordIndex;               // offset 0x5158, size 0x4
    FEPackageButtonRec RecordedPackageButtons[32]; // offset 0x515C, size 0x100
    bool bErrorScreenMode;                   // offset 0x525C, size 0x1
    bool bRenderedRecently;                  // offset 0x5260, size 0x1
    bool bDebugMessages;                     // offset 0x5264, size 0x1

    inline FEPackageList* GetPackageList() {
        FEPackageList* p = &PackList;
        return p;
    }

    inline void SetInterface(FEGameInterface* pNewInterface) { pInterface = pNewInterface; }
    inline void ToggleErrorScreenMode(bool b) { bErrorScreenMode = b; }
    inline bool IsErrorScreenMode() { return bErrorScreenMode; }
    inline FEObjectSorter<1024>& GetSorter() { return Sorter; }
    inline FEJoyPad* GetJoyPad(unsigned char Index) { return &pJoyPad[Index]; }
    inline void SetUseMouse(bool bUseMouse) { bMouseActive = bUseMouse; }
    inline FEMouse* GetMouse() { return &Mouse; }
    inline FETypeLib& GetTypeLib() { return TypeLib; }
    inline void SetLoadObjectNames(bool bLoadNames) { bLoadObjectNames = bLoadNames; }
    inline bool GetLoadObjectNames() const { return bLoadObjectNames; }
    inline void SetLoadScriptNames(bool bLoadNames) { bLoadScriptNames = bLoadNames; }
    inline bool GetLoadScriptNames() const { return bLoadScriptNames; }
    inline void SetWrapMode(FEButtonWrapMode NewMode) { WrapMode = NewMode; }
    inline FEButtonWrapMode GetWrapMode() { return WrapMode; }
    static inline unsigned long GetNextGUID() { return SysGUID++; }
    static inline unsigned long GetSysGUID() { return SysGUID; }
    static inline void SetSysGUID(unsigned long NewGUID) { SysGUID = NewGUID; }
    inline int GetNumPackageMarkers() const { return CurrentPackageRecordIndex; }
    inline const char* GetPackageMarker(unsigned long Index) const { return RecordedPackageNames[Index]; }
    inline void DebugMessages(bool bDebug) { bDebugMessages = bDebug; }

    FEngine();
    ~FEngine();
    void ReleaseAll();
    void SetNumJoyPads(unsigned char Count);
    void SetExecution(bool bProcessEverything);
    void SetProcessInput(FEPackage* pkg, bool bProcess);
    void SetInitialState();
    FEPackage* LoadPackage(const void* pPackageData, bool bLoadAsLibrary);
    bool UnloadPackage(FEPackage* pPackage);
    void UnloadLibraryPackage(FEPackage* pLibPack);
    FEPackage* PushPackage(const char* pPackageName, const unsigned char Level, const unsigned long ControlMask);
    FEPackage* GetFirstIdle() const;
    void AddToIdleList(FEPackage* pPack);
    void RemoveFromIdleList(FEPackage* pPack);
    FEPackage* FindIdlePackage(const char* pName) const;
    FEPackage* GetFirstLibrary() const;
    void AddToLibraryList(FEPackage* pPack);
    void RemoveFromLibraryList(FEPackage* pPack);
    FEPackage* FindPackageWithControl();
    void QueuePackagePush(const char* pPackageName, unsigned long ControlMask);
    void QueuePackageSwitch(const char* pPackageName, unsigned long ControlMask);
    void QueuePackagePop();
    void QueuePackageCommand(long Command, unsigned long ControlMask, const char* pPackageName);
    void QueuePackageUserTransfer(FEPackage* pPack, bool bPush, unsigned long ControlMask);
    void QueueMessage(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, FEObject* pTo, unsigned long ControlMask);
    void SendMessageToGame(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, unsigned long uControlMask);
    int GetNumPackagesBelowPriority(unsigned char priority);
    void MakeLoadedPackagesDirty();
    void Render();
    void RenderGroup(FEGroup* pGroup, FEMatrix4& mParent, FEMatrix4& mAccum, unsigned short RenderContext);
    void RenderObject(FEObject* pObj, FEMatrix4& mParent, unsigned short RenderContext);
    void Update(long, unsigned int);
    bool ForAllObjects(FEObjectCallback& Callback);
    void ProcessResponses(FEMessageResponse* pResp, FEObject* pObj, FEPackage* pPack, unsigned long uControlMask);
    void ProcessPadsForPackage(FEPackage* pPackage);
    void ProcessMouseForPackage(FEPackage* pPackage);
    void UpdateMouseState(FEPackage* pPack, FEObjectMouseState* pState, float mx, float my);
    void ProcessMessageQueue();
    void ProcessPackageCommands();
    bool ProcessListBoxResponses(FEObject* pObj, unsigned long MsgID);
    bool ProcessListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID);
    bool ProcessCodeListBoxResponses(FEObject* pObj, unsigned long MsgID);
    bool ProcessCodeListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID);
    void ProcessObjectMessage(FEObject* pObj, FEPackage* pPack, unsigned long MsgID, unsigned long uControlMask);
    void ProcessGlobalMessage(FEPackage* pPack, unsigned long MsgID, unsigned long uControlMask);
    FEPackage* FindLibraryPackage(unsigned long NameHash) const;
    FEPackageCommand* FindQueuedNodeWithControl();
    void PopPackage();
    void RemovePackage(FEPackage* pPack);
    void RecordLastPackageButton(unsigned long PackHash, unsigned long ButtonGUID);
    unsigned long RecallLastPackageButton(unsigned long PackHash);
    bool RecordPackageMarker(const char* pName);
    const char* RecallPackageMarker();
    void ClearPackageMarkers();
};

#endif
