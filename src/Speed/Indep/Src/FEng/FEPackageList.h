#ifndef FEPACKAGELIST_H_
#define FEPACKAGELIST_H_

#include <types.h>

#include "FEPackage.h"
#include "FEList.h"

// File: speed/indep/src/feng/FEPackageList.h
// total size: 0x10
// Decl: speed/indep/src/feng/FEPackageList.h:22
class FEPackageList {
  private:
    FEList Packages; // offset 0x0, size 0x10, Decl: speed/indep/src/feng/FEPackageList.h:24

  public:
    FEPackageList() {}
    ~FEPackageList() {}

    FEPackage *GetFirstPackage() const {
        return static_cast<FEPackage *>(Packages.GetHead());
    }

    FEPackage *GetLastPackage() const {
        return static_cast<FEPackage *>(Packages.GetTail());
    }

    FEPackage *FindPackage(const char *pName) const {
        return static_cast<FEPackage *>(Packages.FindNode(pName));
    }

    FEPackage *FindPackage(const char *pName, u8 ControllerIndex) const;

    void AddPackage(FEPackage *pPack); // Decl: speed/indep/src/feng/FEPackageList.h:36

    void AddPackageAfter(FEPackage *pPack, FEPackage *pAfter); // Decl: speed/indep/src/feng/FEPackageList.h:40

    bool RemovePackage(FEPackage *pPack); // Decl: speed/indep/src/feng/FEPackageList.h:42

    void ReplaceParentLinks(const FEPackage *pParent, const FEPackage *pReplacement); // Decl: speed/indep/src/feng/FEPackageList.h:45

    u32 GetCount() const { // Decl: speed/indep/src/feng/FEPackageList.h:48
        return Packages.GetNumElements();
    }
};

#endif
