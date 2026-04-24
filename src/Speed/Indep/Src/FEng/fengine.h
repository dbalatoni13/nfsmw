#ifndef FENG_FENGINE_H
#define FENG_FENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEPackage.h"

struct FEJoyPad;
struct FEGameInterface;
struct FEObjectMouseState;
struct FEMessageResponse;
struct FEMatrix4;
struct FEPackageCommand;

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
};

// total size: 0x5268
struct FEngine {
    static unsigned long SysGUID;

    bool bExecuting;            // offset 0x0
    bool bMouseActive;          // offset 0x4
    bool bLoadObjectNames;      // offset 0x8
    bool bLoadScriptNames;      // offset 0xC
    FEJoyPad* pJoyPad;          // offset 0x10
    char _padMouse[0xD4];       // offset 0x14 to 0xE8
    FEPackageList PackList;     // offset 0xE8, size 0x10
    FEList IdleList;            // offset 0xF8, size 0x10
    FEList LibraryList;         // offset 0x108, size 0x10
    FEGameInterface* pInterface; // offset 0x118
    char _padRest[0x5140];      // offset 0x11C to 0x525C
    bool bErrorScreenMode;      // offset 0x525C
    bool bRenderedRecently;     // offset 0x5260
    bool bDebugMessages;        // offset 0x5264

    inline FEPackageList* GetPackageList() {
        FEPackageList* p = &PackList;
        return p;
    }

    inline void SetInterface(FEGameInterface* pNewInterface) { pInterface = pNewInterface; }
    inline void ToggleErrorScreenMode(bool b) { bErrorScreenMode = b; }
    inline bool IsErrorScreenMode() { return bErrorScreenMode; }

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
