#ifndef INTERFACES_SIMMODELS_IMODEL_H
#define INTERFACES_SIMMODELS_IMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

struct HMODEL__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HMODEL__ *HMODEL;

class IModel : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HMODEL, IModel, 434>, public UTL::Collections::Listable<IModel, 434> {
  public:
    IModel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IModel() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
};

#endif
