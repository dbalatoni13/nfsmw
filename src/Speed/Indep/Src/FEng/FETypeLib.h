#ifndef FETYPELIB_H_
#define FETYPELIB_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEObject.h"

class FETypeLib {
    FEList List;                 // offset 0x0, size 0x10, Decl: 31
    bool bAutoCreateHideScripts; // offset 0x10, size 0x1, Decl: 32

  public:
    FETypeLib() : bAutoCreateHideScripts(false) {} // Decl: 35
    ~FETypeLib() {}
    bool Startup();    // Decl: 37
    void Shutdown() {} // Decl: 38

    void SetAutoCreateHide(bool bValue) { // Decl: 40
        bAutoCreateHideScripts = bValue;
    }

    bool GetAutoCreateHide() const {
        return bAutoCreateHideScripts;
    }

    FETypeNode *CreateBaseObjectType(const char *pName);

    FETypeNode *FindType(const char *pName) {
        return static_cast<FETypeNode *>(List.FindNode(pName));
    }

    FETypeNode *FindType(u32 TypeID);

    void AddType(FETypeNode *pNode) { // Decl: 48
        List.AddTail(pNode);
    }
    void RemoveType(FETypeNode *pNode) { // Decl: 49
        List.RemNode(pNode);
    }

    FETypeNode *GetFirstType() { // Decl: 51
        return static_cast<FETypeNode *>(List.GetHead());
    }

    FEObject *CreateFEObject(FETypeNode *pType, bool bInitScript); // Decl: 53
    FEObject *CreateFEObject(u32 TypeID, bool bInitScript);

    FEScript *CreateObjectScript(FETypeNode *pType); // Decl: 56

    FEScript *CreateObjectScript(u32 TypeID);

    FEList *const GetList() { // Decl: 60
        return &List;
    }

    FETypeNode *CreateImageObjectType(const char *pName);

    FETypeNode *CreateMultiImageObjectType(const char *pName);
};

#endif
