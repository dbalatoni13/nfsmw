#ifndef INTERFACES_SIMMODELS_ISCENERYMODEL_H
#define INTERFACES_SIMMODELS_ISCENERYMODEL_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x8
class ISceneryModel : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISceneryModel);

    virtual bool GetSceneryTransform(UMath::Matrix4 &matrix) const = 0;
    virtual void RestoreScene() = 0;
    virtual unsigned int GetSpawnerID() const = 0;
    virtual void WakeUp() = 0;
    virtual bool IsExcluded(unsigned int scenery_exclusion_flag) const = 0;
};

#endif
