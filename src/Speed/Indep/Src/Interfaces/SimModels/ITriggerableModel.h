#ifndef ITRIGGERABLEMODEL_H
#define ITRIGGERABLEMODEL_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class ITriggerableModel : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITriggerableModel);

    virtual void PlaceTrigger(const UMath::Matrix4 &matrix, bool enable);
};

#endif
