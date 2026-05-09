#ifndef FENG_FENGINE_H
#define FENG_FENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "FEPackage.h"
#include "FEMouse.h"
#include "FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEObjectSorter.h"
#include "Speed/Indep/Src/FEng/FEPackageList.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEButtonMap.h"

struct FEJoyPad;
struct FEGameInterface;
struct FEObjectMouseState;
struct FEMessageResponse;
struct FEMatrix4;
struct FEPackageCommand;

struct FETypeLib {
    FEList List;                 // offset 0x0, size 0x10
    bool bAutoCreateHideScripts; // offset 0x10, size 0x1

    inline FETypeLib() : bAutoCreateHideScripts(false) {}
    inline ~FETypeLib() {}
    inline void Shutdown() {}
    inline void SetAutoCreateHide(bool bValue) {
        bAutoCreateHideScripts = bValue;
    }
    inline bool GetAutoCreateHide() const {
        return bAutoCreateHideScripts;
    }
    inline FETypeNode *FindType(const char *pName) {
        return static_cast<FETypeNode *>(List.FindNode(pName));
    }
    inline void AddType(FETypeNode *pNode) {
        List.AddTail(pNode);
    }
    inline void RemoveType(FETypeNode *pNode) {
        List.RemNode(pNode);
    }
    inline FETypeNode *GetFirstType() {
        return static_cast<FETypeNode *>(List.GetHead());
    }
    inline const FEList *GetList() {
        return &List;
    }

    bool Startup();
    FETypeNode *FindType(unsigned long TypeID);
    FEObject *CreateFEObject(FETypeNode *pType, bool bInitScript);
    FEObject *CreateFEObject(unsigned long TypeID, bool bInitScript);
    FEScript *CreateObjectScript(FETypeNode *pType);
    FEScript *CreateObjectScript(unsigned long TypeID);
    FETypeNode *CreateBaseObjectType(const char *pName);
    FETypeNode *CreateImageObjectType(const char *pName);
    FETypeNode *CreateMultiImageObjectType(const char *pName);
};

struct FEPackageButtonRec {
    u32 PackageHash; // offset 0x0, size 0x4
    u32 ButtonGUID;  // offset 0x4, size 0x4
};

// total size: 0x5268
class FEngine {
  private:
    void RenderGroup(FEGroup *pGroup, FEMatrix4 &mParent, FEMatrix4 &mAccum, unsigned short RenderContext);
    void RenderObject(FEObject *pObj, FEMatrix4 &mParent, unsigned short RenderContext);
    void ProcessPadsForPackage(FEPackage *pPackage);
    void ProcessMouseForPackage(FEPackage *pPackage);
    void UpdateMouseState(FEPackage *pPack, FEObjectMouseState *pState, float mx, float my);
    void ProcessMessageQueue();
    bool ProcessListBoxResponses(FEObject *pObj, unsigned long MsgID);
    bool ProcessListBoxResponses(FEObject *pObj, FEPackage *pPack, unsigned long MsgID);
    bool ProcessCodeListBoxResponses(FEObject *pObj, unsigned long MsgID);
    bool ProcessCodeListBoxResponses(FEObject *pObj, FEPackage *pPack, unsigned long MsgID);
    void ProcessObjectMessage(FEObject *pObj, FEPackage *pPack, unsigned long MsgID, unsigned long uControlMask);
    void ProcessGlobalMessage(FEPackage *pPack, unsigned long MsgID, unsigned long uControlMask);
    void ProcessResponses(FEMessageResponse *pResp, FEObject *pObj, FEPackage *pPack, unsigned long uControlMask);
    FEPackageCommand *FindQueuedNodeWithControl();
    void ProcessPackageCommands();

  public:
    inline void ToggleErrorScreenMode(bool b) {
        bErrorScreenMode = b;
    }
    inline bool IsErrorScreenMode() {
        return bErrorScreenMode;
    }
    FEObjectSorter<1024> &GetSorter() {
        return Sorter;
    };

    FEngine();
    ~FEngine();

    void ReleaseAll();
    FEPackage *LoadPackage();
    bool UnloadPackage(FEPackage *pPackage);
    void UnloadLibraryPackage(FEPackage *pLibPack);
    void SetExecution(bool bProcessEverything);
    void SetProcessInput(FEPackage *pkg, bool bProcess);
    void SetInitialState();
    void SetNumJoyPads(unsigned char Count);
    FEJoyPad *GetJoyPad();
    void SetUseMouse();
    FEMouse *GetMouse();
    FETypeLib &GetTypeLib();

    //

    inline FEPackageList *GetPackageList() {
        FEPackageList *p = &PackList;
        return p;
    }

    inline void SetInterface(FEGameInterface *pNewInterface) {
        pInterface = pNewInterface;
    }

    FEPackage *LoadPackage(const void *pPackageData, bool bLoadAsLibrary);

    FEPackage *PushPackage(const char *pPackageName, const unsigned char Level, const unsigned long ControlMask);
    FEPackage *GetFirstIdle() const;
    void AddToIdleList(FEPackage *pPack);
    void RemoveFromIdleList(FEPackage *pPack);
    FEPackage *FindIdlePackage(const char *pName) const;
    FEPackage *GetFirstLibrary() const;
    void AddToLibraryList(FEPackage *pPack);
    void RemoveFromLibraryList(FEPackage *pPack);
    FEPackage *FindPackageWithControl();
    void QueuePackagePush(const char *pPackageName, unsigned long ControlMask);
    void QueuePackageSwitch(const char *pPackageName, unsigned long ControlMask);
    void QueuePackagePop();
    void QueuePackageCommand(long Command, unsigned long ControlMask, const char *pPackageName);
    void QueuePackageUserTransfer(FEPackage *pPack, bool bPush, unsigned long ControlMask);
    void QueueMessage(unsigned long MsgID, FEObject *pFrom, FEPackage *pFromPackage, FEObject *pTo, unsigned long ControlMask);
    void SendMessageToGame(unsigned long MsgID, FEObject *pFrom, FEPackage *pFromPackage, unsigned long uControlMask);
    int GetNumPackagesBelowPriority(unsigned char priority);
    void MakeLoadedPackagesDirty();
    void Render();
    void Update(long, unsigned int);
    bool ForAllObjects(FEObjectCallback &Callback);

    FEPackage *FindLibraryPackage(unsigned long NameHash) const;
    void PopPackage();
    void RemovePackage(FEPackage *pPack);
    void RecordLastPackageButton(unsigned long PackHash, unsigned long ButtonGUID);
    unsigned long RecallLastPackageButton(unsigned long PackHash);
    bool RecordPackageMarker(const char *pName);
    const char *RecallPackageMarker();
    void ClearPackageMarkers();

  private:
    bool bExecuting;                               // offset 0x0, size 0x1
    bool bMouseActive;                             // offset 0x4, size 0x1
    bool bLoadObjectNames;                         // offset 0x8, size 0x1
    bool bLoadScriptNames;                         // offset 0xC, size 0x1
    struct FEJoyPad *pJoyPad;                      // offset 0x10, size 0x4
    struct FEMouse Mouse;                          // offset 0x14, size 0x24
    unsigned long FastRep;                         // offset 0x38, size 0x4
    unsigned long FastRepCache;                    // offset 0x3C, size 0x4
    unsigned long PadHoldRegistered;               // offset 0x40, size 0x4
    unsigned long HoldDecrement[19];               // offset 0x44, size 0x4C
    struct FEObject *HeldButtons[19];              // offset 0x90, size 0x4C
    FEButtonWrapMode WrapMode;                     // offset 0xDC, size 0x4
    unsigned long NumJoyPads;                      // offset 0xE0, size 0x4
    unsigned short uGroupContext;                  // offset 0xE4, size 0x2
    struct FEPackageList PackList;                 // offset 0xE8, size 0x10
    struct FEList IdleList;                        // offset 0xF8, size 0x10
    struct FEList LibraryList;                     // offset 0x108, size 0x10
    struct FEGameInterface *pInterface;            // offset 0x118, size 0x4
    struct FEObjectSorter<1024> Sorter;            // offset 0x11C, size 0x4004
    struct FEMinList MsgQ;                         // offset 0x4120, size 0x10
    struct FEList PackageCommands;                 // offset 0x4130, size 0x10
    struct FETypeLib TypeLib;                      // offset 0x4140, size 0x14
    int CurrentPackageRecordIndex;                 // offset 0x4154, size 0x4
    char RecordedPackageNames[256][16];            // offset 0x4158, size 0x1000
    int NextButtonRecordIndex;                     // offset 0x5158, size 0x4
    FEPackageButtonRec RecordedPackageButtons[32]; // offset 0x515C, size 0x100
    bool bErrorScreenMode;                         // offset 0x525C, size 0x1
    bool bRenderedRecently;                        // offset 0x5260, size 0x1
    static u32 SysGUID;
    bool bDebugMessages; // offset 0x5264, size 0x1
};

#endif
