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
    bool bExecuting;            // offset 0x0
    char _pad1[0x3];
    bool bMouseActive;          // offset 0x4
    char _pad2[0x3];
    bool bLoadObjectNames;      // offset 0x8
    char _pad3[0x3];
    bool bLoadScriptNames;      // offset 0xC
    char _pad4[0x3];
    FEJoyPad* pJoyPad;          // offset 0x10
    char _padMouse[0xD4];       // offset 0x14 to 0xE8
    FEPackageList PackList;     // offset 0xE8, size 0x10
    FEList IdleList;            // offset 0xF8, size 0x10
    FEList LibraryList;         // offset 0x108, size 0x10
    FEGameInterface* pInterface; // offset 0x118
    char _padRest[0x5140];      // offset 0x11C to 0x525C
    bool bErrorScreenMode;      // offset 0x525C
    char _pad5[0x3];
    bool bRenderedRecently;     // offset 0x5260
    char _pad6[0x3];
    bool bDebugMessages;        // offset 0x5264

    inline FEPackageList* GetPackageList() { return &PackList; }

    void Render();
    void Update(long, unsigned int);
    FEPackage* FindIdlePackage(const char* pName) const;
    FEPackage* FindPackageWithControl();
};

#endif
