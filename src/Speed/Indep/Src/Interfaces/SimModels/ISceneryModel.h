#ifndef INTERFACES_SIMMODELS_ISCENERYMODEL_H
#define INTERFACES_SIMMODELS_ISCENERYMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"


class ISceneryModel : public UTL::COM::IUnknown {
    // total size: 0x8

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISceneryModel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISceneryModel() {}

    virtual unsigned int GetSpawnerID() {}
    virtual void RestoreScene();
    virtual bool GetSceneryTransform(UMath::Matrix4) const;
    virtual void WakeUp();
    virtual bool IsExcluded(unsigned int scenery_exclusion_flag) {}
};

#endif
