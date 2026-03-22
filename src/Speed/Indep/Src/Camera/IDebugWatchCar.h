#ifndef CAMERA_IDEBUGWATCHCAR_H
#define CAMERA_IDEBUGWATCHCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

class ISimable;

class IDebugWatchCar : public UTL::COM::IUnknown, public UTL::Collections::Listable<IDebugWatchCar, 2> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDebugWatchCar(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDebugWatchCar() {}

    virtual ISimable *GetSimable() = 0;
};

#endif
