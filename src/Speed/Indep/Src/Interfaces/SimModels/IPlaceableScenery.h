#ifndef IPLACEABLESCENERY_H
#define IPLACEABLESCENERY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x8
class IPlaceableScenery : public UTL::COM::IUnknown, public UTL::Collections::Countable<IPlaceableScenery> {
  public:
    DECL_INTERFACE(IPlaceableScenery);

    virtual void PickUp();
    virtual bool Place(const UMath::Matrix4 &transform, bool snap_to_ground);
    virtual void Destroy();

    static IPlaceableScenery *CreateInstance(const char *name, Attrib::Key attributes);
};

#endif
