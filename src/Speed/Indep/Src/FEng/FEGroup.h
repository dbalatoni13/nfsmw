#ifndef FENG_FEGROUP_H
#define FENG_FEGROUP_H

#include "FEObject.h"

// total size: 0x6C
struct FEGroup : public FEObject {
    FEMinList Children; // offset 0x5C, size 0x10

    inline FEGroup() : FEObject(), Children() {}
    FEGroup(const FEGroup& Object, bool bCloneChildren, bool bReference);

    inline void AddObject(FEObject* pObj) { Children.AddTail(pObj); }
    inline void AddObjectAfter(FEObject* pObj, FEObject* pAddAfter) { Children.AddNode(pAddAfter, pObj); }
    inline void RemoveObject(FEObject* pObj) { Children.RemNode(pObj); }

    inline unsigned long GetNumChildren() const { return Children.GetNumElements(); }
    inline FEObject* GetFirstChild() const { return static_cast<FEObject*>(Children.GetHead()); }
    inline FEObject* GetLastChild() const { return static_cast<FEObject*>(Children.GetTail()); }

    FEObject* FindChild(unsigned long NameHash) const;
    FEObject* FindChild(const char* pName) const;
    FEObject* FindChildRecursive(unsigned long NameHash) const;
    FEObject* FindChildRecursive(const char* pName) const;

    FEObject* Clone(bool bReference) override;
};

#endif
