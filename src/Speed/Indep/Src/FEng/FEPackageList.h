#ifndef FENG_FEPACKAGELIST_H
#define FENG_FEPACKAGELIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "FEPackage.h"
#include "FEList.h"

class FEPackageList {
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
    };
    FEPackage *FindPackage(const char *pName, unsigned char ControllerIndex) const;
    void AddPackage(FEPackage *pPack);
    void AddPackageAfter(FEPackage *pPack, FEPackage *pAfter);
    bool RemovePackage(FEPackage *pPack);
    void ReplaceParentLinks(const FEPackage *pParent, const FEPackage *pReplacement);
    u32 GetCount() {
        return Packages.GetNumElements();
    };

  private:
    FEList Packages; // offset 0x0, size 0x10
};

#endif
