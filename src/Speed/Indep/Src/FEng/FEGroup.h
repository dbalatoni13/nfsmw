#ifndef FEGROUP_H_
#define FEGROUP_H_

#include "FEObject.h"

// File: speed/indep/src/feng/FEGroup.h
// total size: 0x6C
// Decl: speed/indep/src/feng/FEGroup.h:28
class FEGroup : public FEObject {
  private:
    FEMinList Children; // offset 0x5C, size 0x10, Decl: speed/indep/src/feng/FEGroup.h:30

  public:
    FEGroup() {} // Decl: speed/indep/src/feng/FEGroup.h:33
    FEGroup(const FEGroup &Object, bool bCloneChildren, bool bReference);
    ~FEGroup() override {}
    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FEGroup.h:36

    void AddObject(FEObject *pObj) { // Decl: speed/indep/src/feng/FEGroup.h:38
        Children.AddTail(pObj);
    }
    void AddObjectAfter(FEObject *pObj, FEObject *pAddAfter) { // Decl: speed/indep/src/feng/FEGroup.h:39
        Children.AddNode(pAddAfter, pObj);
    }

    void RemoveObject(FEObject *pObj) { // Decl: speed/indep/src/feng/FEGroup.h:41
        Children.RemNode(pObj);
    }

    u32 GetNumChildren() const {
        return Children.GetNumElements();
    }

    FEObject *GetFirstChild() const {
        return static_cast<FEObject *>(Children.GetHead());
    }
    FEObject *GetLastChild() const {
        return static_cast<FEObject *>(Children.GetTail());
    }

    FEObject *FindChild(u32 NameHash) const;
    FEObject *FindChild(const char *pName) const;

    FEObject *FindChildRecursive(u32 NameHash) const;
    FEObject *FindChildRecursive(const char *pName) const;
};

#endif
