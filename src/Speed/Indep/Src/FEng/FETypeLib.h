#ifndef FETYPELIB_H_
#define FETYPELIB_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FEObject.h"

class FETypeLib {
    FEList List;                 // offset 0x0, size 0x10, Decl: speed/indep/src/feng/FETypeLib.h:31
    bool bAutoCreateHideScripts; // offset 0x10, size 0x1, Decl: speed/indep/src/feng/FETypeLib.h:32

  public:
    FETypeLib() : bAutoCreateHideScripts(false) {} // Decl: speed/indep/src/feng/FETypeLib.h:35
    ~FETypeLib() {}
    bool Startup();    // Decl: speed/indep/src/feng/FETypeLib.h:37
    void Shutdown() {} // Decl: speed/indep/src/feng/FETypeLib.h:38

    void SetAutoCreateHide(bool bValue) { // Decl: speed/indep/src/feng/FETypeLib.h:40
        bAutoCreateHideScripts = bValue;
    }

    bool GetAutoCreateHide() const {
        return bAutoCreateHideScripts;
    }

    FETypeNode *CreateBaseObjectType(const char *pName);

    FETypeNode *FindType(const char *pName) {
        return static_cast<FETypeNode *>(List.FindNode(pName));
    }

    FETypeNode *FindType(unsigned long TypeID);

    void AddType(FETypeNode *pNode) { // Decl: speed/indep/src/feng/FETypeLib.h:48
        List.AddTail(pNode);
    }
    void RemoveType(FETypeNode *pNode) { // Decl: speed/indep/src/feng/FETypeLib.h:49
        List.RemNode(pNode);
    }

    FETypeNode *GetFirstType() { // Decl: speed/indep/src/feng/FETypeLib.h:51
        return static_cast<FETypeNode *>(List.GetHead());
    }

    FEObject *CreateFEObject(FETypeNode *pType, bool bInitScript); // Decl: speed/indep/src/feng/FETypeLib.h:53
    FEObject *CreateFEObject(unsigned long TypeID, bool bInitScript);

    FEScript *CreateObjectScript(FETypeNode *pType); // Decl: speed/indep/src/feng/FETypeLib.h:56

    FEScript *CreateObjectScript(unsigned long TypeID);

    FEList *const GetList() { // Decl: speed/indep/src/feng/FETypeLib.h:60
        return &List;
    }

    FETypeNode *CreateImageObjectType(const char *pName);

    FETypeNode *CreateMultiImageObjectType(const char *pName);
};

#endif
