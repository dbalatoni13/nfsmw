#ifndef FENG_FETYPELIB_H
#define FENG_FETYPELIB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEPackage.h"

struct FETypeNode;
struct FEObject;
struct FEScript;

// total size: 0x14
struct FETypeLib {
    FEList List;                   // offset 0x0, size 0x10
    bool bAutoCreateHideScripts;   // offset 0x10, size 0x1

    inline FETypeLib() : bAutoCreateHideScripts(false) {}
    inline ~FETypeLib() {}

    inline void Shutdown() { List.DestroyList(); }
    inline void SetAutoCreateHide(bool bValue) { bAutoCreateHideScripts = bValue; }
    inline bool GetAutoCreateHide() const { return bAutoCreateHideScripts; }

    inline FETypeNode* FindType(const char* pName) {
        return static_cast<FETypeNode*>(List.FindNode(pName));
    }

    inline void AddType(FETypeNode* pNode) { List.AddTail(pNode); }
    inline void RemoveType(FETypeNode* pNode) { List.RemNode(pNode); }
    inline FETypeNode* GetFirstType() { return static_cast<FETypeNode*>(List.GetHead()); }
    inline const FEList* GetList() { return &List; }

    FETypeNode* CreateBaseObjectType(const char* pName);
    FETypeNode* CreateImageObjectType(const char* pName);
    FETypeNode* CreateMultiImageObjectType(const char* pName);
    bool Startup();
    FETypeNode* FindType(unsigned long TypeID);
    FEObject* CreateFEObject(FETypeNode* pType, bool bInitScript);
    FEObject* CreateFEObject(unsigned long TypeID, bool bInitScript);
    FEScript* CreateObjectScript(FETypeNode* pType);
    FEScript* CreateObjectScript(unsigned long TypeID);
};

#endif
