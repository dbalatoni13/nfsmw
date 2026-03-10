#ifndef INTERFACES_SIMMODELS_ITRIGGERABLEMODEL_H
#define INTERFACES_SIMMODELS_ITRIGGERABLEMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class ITriggerableModel : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITriggerableModel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ITriggerableModel() {}

    virtual void PlaceTrigger(const UMath::Matrix4 &mat, bool retrigger) = 0;
};

#endif
