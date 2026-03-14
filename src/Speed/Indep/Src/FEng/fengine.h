#ifndef FENG_FENGINE_H
#define FENG_FENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEPackage.h"

struct FEJoyPad;

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

    FEngine();
    void SetNumJoyPads(unsigned char Count);
    void SetExecution(bool bProcessEverything);
    void SetInitialState();
    void Render();
    void Update(long, unsigned int);
    FEPackage* PushPackage(const char* pPackageName, unsigned char Level, unsigned long ControlMask);
    bool UnloadPackage(FEPackage* pPackage);
    FEPackage* FindIdlePackage(const char* pName) const;
    FEPackage* FindPackageWithControl();
    void QueuePackagePush(const char* pPackageName, unsigned long ControlMask);
    void QueuePackageSwitch(const char* pPackageName, unsigned long ControlMask);
    void QueuePackagePop();
    void QueueMessage(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, FEObject* pTo, unsigned long ControlMask);
    void SendMessageToGame(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, unsigned long uControlMask);
    int GetNumPackagesBelowPriority(unsigned char priority);
    void MakeLoadedPackagesDirty();
};

#endif
