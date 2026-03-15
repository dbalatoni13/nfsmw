#ifndef INTERFACES_SIMMODELS_ISCENERYMODEL_H
#define INTERFACES_SIMMODELS_ISCENERYMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x8
class ISceneryModel : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISceneryModel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISceneryModel() {}

    virtual bool GetSceneryTransform(UMath::Matrix4 &matrix) const;
    virtual void RestoreScene();
    virtual unsigned int GetSpawnerID() const {}
    virtual void WakeUp();
    virtual bool IsExcluded(unsigned int scenery_exclusion_flag) const {}
};

#endif
