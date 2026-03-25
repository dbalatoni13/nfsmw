#ifndef INTERFACES_SIMMODELS_IPLACEABLESCENERY_H
#define INTERFACES_SIMMODELS_IPLACEABLESCENERY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

// total size: 0x8
class IPlaceableScenery : public UTL::COM::IUnknown, public UTL::Collections::Countable<IPlaceableScenery> {
  public:
    static HINTERFACE _IHandle();

    IPlaceableScenery(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, (HINTERFACE)_IHandle) {}

    virtual ~IPlaceableScenery();

    virtual void Destroy();
    virtual void PickUp();
    virtual bool Place(const UMath::Matrix4 &transform, bool snap_to_ground);

    static IPlaceableScenery *CreateInstance(const char *name, unsigned int attributes);
};

#endif
